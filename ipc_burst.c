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
 * Create a directory structure and files matching a given IPC
 * filesystem.  This program performs the equivelent of a "tar"
 * extract on an IPC filesystem.
 */

static char *ipc_burst_revision = "$Id: ipc_burst.c,v 1.4 2000/10/25 15:32:12 pete Exp $";

#include "ipc_utils.h"
#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <utime.h>

static void set_time(IPC_Filsys *fs,
		     const char *ipcname, const char *unixname)
{
  struct utimbuf utimbuf;
  struct stat stat;
  IPC_stat(fs,ipcname,&stat);
  utimbuf.actime = stat.st_atime;
  utimbuf.modtime = stat.st_mtime;
  utime(unixname,&utimbuf);
  chmod(unixname,stat.st_mode);
}

static void burst(IPC_Filsys *fs, const char *path, const char *dest)
{
  struct stat stat;
  IPC_stat(fs,path,&stat);
  if (stat.st_mode & S_IFDIR)
    {				/* directory */
      IPC_File *fp;
      struct direct direct;
      char *newdir = (char *) alloca(strlen(dest)+strlen(path)+2);
      strcpy(newdir,dest); strcat(newdir,"/"); strcat(newdir,path);
      if (mkdir(newdir,stat.st_mode))
	{
	  perror(newdir);
	  exit(-1);
	}
      set_time(fs,path,newdir);

      fp = IPC_open(fs,path);
      while (IPC_read(fp,&direct,sizeof(direct)) != -1)
	if (direct.d_ino != 0 &&
	    strcmp(direct.d_name,".") &&
	    strcmp(direct.d_name,".."))
	  {
	    char *buf = (char *) alloca(strlen(path)+DIRSIZ+2);
	    strcpy(buf,path);
	    if (strcmp(path,"/")) strcat(buf,"/");
	    strcat(buf,direct.d_name);
	    buf[strlen(path)+DIRSIZ+1] = '\0';
	    burst(fs,buf,dest);
	  }
    }
  else if (stat.st_mode & S_IFCHR)
    {				/* character special */
    }
  else if (stat.st_mode & S_IFBLK)
    {				/* block special */
    }
  else if (stat.st_mode & S_IFREG)
    {				/* regular */
      FILE *unixfp;
      IPC_File *ipcfp;
      char buf;
      char *filename = (char *) alloca(strlen(dest)+strlen(path)+2);
      strcpy(filename,dest); strcat(filename,"/"); strcat(filename,path);

      if ((unixfp = fopen(filename,"w")) == 0)
	{
	  perror(filename);
	  exit(-1);
	}
      set_time(fs,path,filename);
      
      ipcfp = IPC_open(fs,path);
      while (IPC_read(ipcfp,&buf,1) != -1)
	putc(buf,unixfp);
      fclose(unixfp);
      IPC_close(ipcfp);
      printf("cp IPC:%s %s\n",path,filename);
    }
  else if (stat.st_mode & S_IFIFO)
    {				/* fifo */
    }
}

main(int argc, char **argv)
{
  IPC_Filsys *fs;
  IPC_File *fp;
  struct direct direct;
  char *path;

  if (argc != 3)
    {
      fprintf(stderr,"Usage: %s <IPC file system> <path>\n",argv[0]);
      return -1;
    }
  fs = IPC_Mount(argv[1]);
  if (!fs)
    {
      perror(argv[1]);
      return -1;
    }

  burst(fs,"/",argv[2]);

}
