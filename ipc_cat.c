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
 * Cat a file from an IPC filesystem.
 */

static char *ipc_cat_revision = "$Id: ipc_cat.c,v 1.4 2000/10/25 15:32:12 pete Exp $";

#include "ipc_utils.h"
#include <stdio.h>
main(int argc, char **argv)
{
  IPC_Filsys *fs;
  IPC_File *fp;
  char buf;

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
  fp = IPC_open(fs,argv[2]);
  if (!fp)
    {
      fprintf(stderr,"%s: no such file or directory\n",argv[2]);
      return -1;
    }
  if (dinode_isdir(fp->dinode))
    {
      fprintf(stderr,"%s: directory\n",argv[2]);
    }
  else while (IPC_read(fp,&buf,1) != -1)
    {
      putchar(buf);
    }
  IPC_close(fp);
}
