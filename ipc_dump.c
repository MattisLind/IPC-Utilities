/*
 *  Utilities for reading HP-UX 5.0 filesystems
 *  Version 1
 *
 *  This program may be freely redistributed,
 *  but this entire comment MUST remain intact.
 *
 *  Copyright (c) 2000, Peter A. Johnson
 */

/*
 * Dump header and superblock information for an IPC filesystem.
 */

static char *ipc_dump_revision = "$Id: ipc_dump.c,v 1.4 2000/10/25 15:32:12 pete Exp $";

#include "ipc_utils.h"
#include <stdio.h>

static void dump_disc_hdr(FILE *fp, struct disc_hdr *hdr)
{
  fprintf(fp,"struct disc_hdr {\n");
  fprintf(fp,"  short\tmlfi =\t%x\n",hdr->mlfi);
  fprintf(fp,"  char	vollbl[6] = %s\n",hdr->vollbl);
  fprintf(fp,"  long	l_dstart = %x\n",hdr->l_dstart);
  fprintf(fp,"  short	hack_3000 = %x\n",hdr->hack_3000);
  fprintf(fp,"  short	dummy1 = %x\n",hdr->dummy1);
  fprintf(fp,"  long	l_dirlen = %x\n",hdr->l_dirlen);
  fprintf(fp,"  short	version = %x\n",hdr->version);
  fprintf(fp,"  short	dummy2 = %x\n",hdr->dummy2);
  fprintf(fp,"  int	trkpersurf = %x\n",hdr->trkpersurf);
  fprintf(fp,"  int	surfpermed = %x\n",hdr->surfpermed);
  fprintf(fp,"  int	sectpertrk = %x\n",hdr->sectpertrk);
  fprintf(fp,"  char	vctime[6] = %x/%x/%x %x:%x:%x\n",
	  (unsigned char)hdr->vctime[0], (unsigned char)hdr->vctime[1],
	  (unsigned char)hdr->vctime[2], (unsigned char)hdr->vctime[3],
	  (unsigned char)hdr->vctime[4], (unsigned char)hdr->vctime[5]);
  fprintf(fp,"}\n");
}

static void dump_filsys(FILE *fp, struct filsys *filsys)
{
  int i;
  char buf[7];
  fprintf(fp,"struct filsys {\n");
  fprintf(fp,"  long  	s_isize = %x	/* size in blocks of i-list */\n",
	  filsys->s_isize);
  fprintf(fp,"  daddr_t	s_fsize = %x	/* size in blocks of entire volume */\n",
	  filsys->s_fsize);
  fprintf(fp,"  long 	s_nfree = %x	/* number of addresses in s_free */\n",
	  filsys->s_nfree);
  for (i=0; i<NICFREE; i++)
    fprintf(fp,"  daddr_t	s_free[%d] = %x	/* free block list */\n",
	    i,filsys->s_free[i]);
  fprintf(fp,"  long 	s_ninode = %x	/* number of i-nodes in s_inode */\n",
	  filsys->s_ninode);
  for (i=0; i<NICINOD; i++)
    fprintf(fp,"  ino_t	s_inode[%d] = %x	/* free i-node list */\n",
	    i,filsys->s_inode[i]);
  fprintf(fp,"  char	s_flock = %x	/* lock during free list manipulation */\n",
	  filsys->s_flock);
  fprintf(fp,"  char	s_ilock = %x	/* lock during i-list manipulation */\n",
	  filsys->s_ilock);
  fprintf(fp,"  char  	s_fmod = %x	/* super block modified flag */\n",
	  filsys->s_fmod);
  fprintf(fp,"  char	s_ronly = %x	/* mounted read-only flag */\n",
	  filsys->s_ronly);
  fprintf(fp,"  time_t	s_time = %x	/* last super block update */\n",
	  filsys->s_time);
  for (i=0; i<4; i++)
    fprintf(fp,"  short	s_dinfo[%d] = %x	/* device information */\n",
	    i,filsys->s_dinfo[i]);
  fprintf(fp,"  daddr_t	s_tfree = %x	/* total free blocks*/\n",
	  filsys->s_tfree);
  fprintf(fp,"  ino_t	s_tinode = %x	/* total free inodes */\n",
	  filsys->s_tinode);
  strncpy(buf,filsys->s_fname,6);
  buf[6] = '\0';
  fprintf(fp,"  char	s_fname[6] = %s	/* file system name */\n",
	  buf);
  strncpy(buf,filsys->s_fpack,6);
  buf[6] = '\0';
  fprintf(fp,"  char	s_fpack[6] = %s	/* file system pack name */\n",
	  buf);
  fprintf(fp,"  long	s_magic = %x	/* magic number to indicate new file system */\n",
	  filsys->s_magic);
  fprintf(fp,"  long	s_type = %x	/* type of new file system */\n",
	  filsys->s_type);
  fprintf(fp,"}\n");
};

static void dump_dinode(FILE *fp, struct dinode *dinode)
{
  int i;

  fprintf(fp,"struct dinode {\n");
  fprintf(fp,"  ushort  di_mode = 0%o\t/* mode and type of file */\n",dinode->di_mode);
  fprintf(fp,"  short   di_nlink = %x\t/* number of links to file */\n",dinode->di_nlink);
  fprintf(fp,"  ushort  di_uid = %x\t/* owner's user id */\n",dinode->di_uid);
  fprintf(fp,"  ushort  di_gid = %x\t/* owner's group id */\n",dinode->di_gid);
  fprintf(fp,"  off_t   di_size = %x\t/* number of bytes in file */\n",dinode->di_size);
  fprintf(fp,"  di_a[40] = { ");
  for (i=0; i<13; i++) fprintf(fp,"%x, ",dinode_addr(dinode,i));
  fprintf(fp,"}\n");
  fprintf(fp,"  time_t  di_atime = %x\t/* time last accessed */\n",dinode->di_atime);
  fprintf(fp,"  time_t  di_mtime = %x\t/* time last modified */\n",dinode->di_mtime);
  fprintf(fp,"  time_t  di_ctime = %x\t/* time created */\n",dinode->di_ctime);
  fprintf(fp,"}\n");
}

main(int argc, char **argv)
{
  IPC_Filsys *fs;
  IPC_File *fp;
  unsigned char buf[2];
  struct direct direct;
  buf[1] = '\0';
  fs = IPC_Mount(argv[1]);

  dump_disc_hdr(stdout,fs->disc_hdr);
  dump_filsys(stdout,fs->filsys);
}
