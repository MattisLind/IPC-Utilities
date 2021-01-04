/*
 *  Utilities for reading HP-UX 5.0 filesystems
 *  Version 1
 *
 *  This program may be freely redistributed,
 *  but this entire comment MUST remain intact.
 *
 *  Copyright (c) 2000, Peter A. Johnson
 */

static char *ipc_utils_revision = "$Id: ipc_utils.c,v 1.4 2000/10/25 15:32:11 pete Exp $";

#include "ipc_utils.h"

#include <sys/types.h>
#include <sys/mman.h>
#include <assert.h>
#include <string.h>

unsigned dinode_addr(struct dinode *dinode, unsigned addr)
{
  unsigned char a[3];
  assert(addr < 13);
  a[0] = dinode->di_addr[3*addr];
  a[1] = dinode->di_addr[3*addr+1];
  a[2] = dinode->di_addr[3*addr+2];
  return a[0]<<16|a[1]<<8|a[2];
}

int dinode_isdir(struct dinode *dinode)
{
  return dinode->di_mode & 040000;
}
  
static void IPC_advance_block(IPC_File *fp)
{
  if (fp->block_index[0] < 9)
    {
      fp->block_index[0]++;
      fp->current_block = fp->filsys->block_data +
	dinode_addr(fp->dinode,fp->block_index[0]);
    }
  else if (fp->block_index[0] == 9) /* go to single indirect */
    {
      fp->block_index[1] = 0;
      fp->block_index[0] = 10;
      fp->block_table[0] = (IPC_Block_table *)
	(fp->filsys->block_data +
	 dinode_addr(fp->dinode,fp->block_index[0]));
      fp->current_block = fp->filsys->block_data +
	 (*fp->block_table[0])[fp->block_index[1]];
    }
  else if (fp->block_index[0] == 10)
    {
      if (fp->block_index[1] < NINDIR-1)
	{
	  fp->block_index[1]++;
	  fp->current_block = fp->filsys->block_data +
	    (*fp->block_table[0])[fp->block_index[1]];
	}
      else			/* go to double indirect */
	{
	  fp->block_index[2] = 0;
	  fp->block_index[1] = 0;
	  fp->block_index[0] = 11;
	  fp->block_table[0] = (IPC_Block_table *)
	    (fp->filsys->block_data +
	     dinode_addr(fp->dinode,fp->block_index[0]));
	  fp->block_table[1] = (IPC_Block_table *)
	    fp->filsys->block_data +
	    (*fp->block_table[0])[fp->block_index[1]];
	  fp->current_block = fp->filsys->block_data +
	    (*fp->block_table[1])[fp->block_index[2]];
	}
    }
  else if (fp->block_index[0] == 11)
    {
      if (fp->block_index[2] < NINDIR-1)
	{
	  fp->block_index[2]++;
	  fp->current_block = fp->filsys->block_data +
	    (*fp->block_table[1])[fp->block_index[2]];
	}
      else if (fp->block_index[1] < NINDIR-1)
	{
	  fp->block_index[2] = 0;
	  fp->block_index[1]++;
	  fp->block_table[1] = (IPC_Block_table *)
	    fp->filsys->block_data +
	    (*fp->block_table[0])[fp->block_index[1]];
	  fp->current_block = fp->filsys->block_data +
	    (*fp->block_table[1])[fp->block_index[2]];
	}
      else
	{			/* go to triple indirect */

	  /* WARNING: triple indirect files have not been tested.
	     Triple indirect should only be needed for file sizes
	     greater than 65,802k bytes. My rough guess is here just
	     in case it works! PAJ */

	  fp->block_index[3] = 0;
	  fp->block_index[2] = 0;
	  fp->block_index[1] = 0;
	  fp->block_index[0] = 12;
	  fp->block_table[0] = (IPC_Block_table *)
	    (fp->filsys->block_data +
	     dinode_addr(fp->dinode,fp->block_index[0]));
	  fp->block_table[1] = (IPC_Block_table *)
	    fp->filsys->block_data +
	    (*fp->block_table[0])[fp->block_index[1]];
	  fp->block_table[2] = (IPC_Block_table *)
	    fp->filsys->block_data +
	    (*fp->block_table[1])[fp->block_index[2]];
	  fp->current_block = fp->filsys->block_data +
	    (*fp->block_table[2])[fp->block_index[3]];
	}
    }
  else if (fp->block_index[0] == 12)
    {
      if (fp->block_index[3] < NINDIR-1)
	{
	  fp->block_index[2]++;
	  fp->current_block = fp->filsys->block_data +
	    (*fp->block_table[2])[fp->block_index[3]];
	}
      else if (fp->block_index[2] < NINDIR-1)
	{
	  fp->block_index[3] = 0;
	  fp->block_index[2]++;
	  fp->block_table[2] = (IPC_Block_table *)
	    fp->filsys->block_data +
	    (*fp->block_table[1])[fp->block_index[2]];
	  fp->current_block = fp->filsys->block_data +
	    (*fp->block_table[2])[fp->block_index[3]];
	}
      else if (fp->block_index[1] < NINDIR-1)
	{
	  fp->block_index[3] = 0;
	  fp->block_index[2] = 0;
	  fp->block_index[1]++;
	  fp->block_table[1] = (IPC_Block_table *)
	    fp->filsys->block_data +
	    (*fp->block_table[0])[fp->block_index[1]];
	  fp->block_table[2] = (IPC_Block_table *)
	    fp->filsys->block_data +
	    (*fp->block_table[1])[fp->block_index[2]];
	  fp->current_block = fp->filsys->block_data +
	    (*fp->block_table[2])[fp->block_index[3]];
	}
      else assert(0);		/* file is too big */
    }
}

static IPC_File *IPC_open_inode(IPC_Filsys *fs, int inum)
{
  IPC_File *fp;

  if (inum < ROOTINO) return NULL;
  fp = (IPC_File *) malloc(sizeof(IPC_File));
  fp->filsys = fs;
  fp->inum = inum;
  fp->dinode = fs->dinode+inum-1; /* No idea why we need -1. */
  fp->offset = 0;
  fp->block_index[0] = 0;
  fp->block_index[1] = 0;
  fp->block_index[2] = 0;
  fp->block_index[3] = 0;
  fp->current_block = fp->filsys->block_data + dinode_addr(fp->dinode,0);
  return fp;
}

void IPC_close(IPC_File *fp)
{
  free(fp);
}

static int IPC_find_inode(IPC_File *dir, const char *fname)
{
  struct direct direct;
  assert(dinode_isdir(dir->dinode));
  while (IPC_read(dir,&direct,sizeof(struct direct)) != -1)
    if (!strncmp(direct.d_name,fname,DIRSIZ) && direct.d_ino > 0)
      return direct.d_ino;
  return 0;
}
  
IPC_File *IPC_open(IPC_Filsys *fs, const char *fname)
{
  IPC_File *dir = IPC_open_inode(fs,2);
  char *fn = (char *) alloca(strlen(fname)+1);
  char *dname;
  int ino;

  strcpy(fn,fname);
  while (*fn == '/') fn++;
  for (dname = strtok(fn,"/"); dname ;dname = strtok(NULL,"/"))
    {
      ino = IPC_find_inode(dir,dname);
      IPC_close(dir);
      dir = IPC_open_inode(fs,ino);
    }
  return dir;
}

int IPC_read(IPC_File *fp, void *buf, int nbytes)
{
  if (fp->offset >= fp->dinode->di_size) return -1;

  if (nbytes > fp->dinode->di_size - fp->offset)
    nbytes = fp->dinode->di_size - fp->offset;

  if ((fp->offset&BMASK) + nbytes <= BSIZE)
    {
      memcpy(buf,((unsigned char *)fp->current_block)+
	     (fp->offset&BMASK),nbytes);
      if ((fp->offset&BMASK) + nbytes == BSIZE)
	IPC_advance_block(fp);
    }
  else
    {
      int nfirst = BSIZE - fp->offset&BMASK;
      int nrest = nbytes - nfirst;
      memcpy(buf,fp->current_block,nfirst);
      IPC_advance_block(fp);
      buf += nfirst;
      for (;nrest > BSIZE; nrest -= BSIZE, buf += BSIZE)
	{
	  memcpy(buf,fp->current_block,BSIZE);
	  IPC_advance_block(fp);
	}
      memcpy(buf,fp->current_block,nrest);
      
    }
  fp->offset += nbytes;
  return nbytes;
}
  
IPC_Filsys *IPC_Mount(char *device)
{
  FILE *fp;
  IPC_Filsys *fs = NULL;
  fp = fopen(device,"r");
  if (!fp)
    {
      perror(device);
    }
  else
    {
      long length;
      unsigned char *file;
      fseek(fp,0,SEEK_END);
      length = ftell(fp);
      file = mmap(0,length,PROT_READ,MAP_PRIVATE,fileno(fp),0);
      if (file)
	{
	  fs = (IPC_Filsys *)malloc(sizeof(IPC_Filsys ));
	  fs->fp = fp;
	  fs->raw_data = file;
	  fs->block_data = (IPC_Block *)file;
	  fs->disc_hdr = (struct disc_hdr *)file;
	  fs->filsys = (struct filsys *)fs->block_data[SUPERB];
	  fs->dinode = (struct dinode *)fs->block_data[ROOTINO];
	  /* If the file system size is greater than the file size,
	     there is definately a problem. If the file system size is
	     less, then we possibly got more data off the physical
	     devise than the file system really needs. This may or may
	     not be a problem.  Here we only report an error if we are
	     hosed for certain. */
	  if (fs->filsys->s_fsize*1024 > length)
	    {
	      fprintf(stderr,
		      "%s: file size (%d) too small for file "
		      "system size (%d).\n",
		      device,length,fs->filsys->s_fsize*1024);
	      free(fs);
	      fs = NULL;
	    }
	}
    }
  return fs;
    
}

int IPC_stat(IPC_Filsys *fs, const char *path, struct stat *buf)
{
  IPC_File *fp;

  fp = IPC_open(fs,path);
  if (!fp) return -1;
  buf->st_dev = 0;
  buf->st_rdev = 0;
  buf->st_ino = fp->inum;
  buf->st_mode = fp->dinode->di_mode;
  buf->st_nlink = fp->dinode->di_nlink;
  buf->st_uid = fp->dinode->di_uid;
  buf->st_gid = fp->dinode->di_gid;
  buf->st_size = fp->dinode->di_size;
  buf->st_atime = fp->dinode->di_atime;
  buf->st_mtime = fp->dinode->di_mtime;
  buf->st_ctime = fp->dinode->di_ctime;
  return 0;
}
