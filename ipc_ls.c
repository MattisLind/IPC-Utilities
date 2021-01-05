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
 * Do a UNIX style long listing of a file or directory specified.
 */

static char *ipc_ls_revision = "$Id: ipc_ls.c,v 1.4 2000/10/25 15:32:12 pete Exp $";

#include "ipc_utils.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

static void ls(const char *path, const struct stat *stat)
{
  char *user = (char *) alloca(9);
  char *group = (char *) alloca(9);
  char *months[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
		      "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  struct tm *tm;

  putchar((stat->st_mode & S_IFDIR) ? 'd' :
	  ((stat->st_mode & S_IFCHR) ? 'c' :
	   ((stat->st_mode & S_IFBLK) ? 'b' :
	    ((stat->st_mode & S_IFIFO) ? '=' : '-'))));
  putchar((stat->st_mode & S_IREAD) ? 'r' : '-');
  putchar((stat->st_mode & S_IWRITE) ? 'w' : '-');
  putchar((stat->st_mode & S_ISUID) ? 's' :
	  ((stat->st_mode & S_IEXEC) ? 'x' : '-'));
  putchar((stat->st_mode & (S_IREAD>>3)) ? 'r' : '-');
  putchar((stat->st_mode & (S_IWRITE>>3)) ? 'w' : '-');
  putchar((stat->st_mode & S_ISGID) ? 's' :
	  ((stat->st_mode & (S_IEXEC>>3)) ? 'x' : '-'));
  putchar((stat->st_mode & (S_IREAD>>6)) ? 'r' : '-');
  putchar((stat->st_mode & (S_IWRITE>>6)) ? 'w' : '-');
  putchar((stat->st_mode & S_ISVTX) ? 't' :
	  ((stat->st_mode & (S_IEXEC>>6)) ? 'x' : '-'));

  if (stat->st_uid == 0) user = "root";
  else if (stat->st_uid == 100) user = "ipc_user";
  else sprintf(user,"%8d",stat->st_uid);

  if (stat->st_gid == 0) group = "root";
  else if (stat->st_gid == 100) group = "user";
  else sprintf(group,"%8d",stat->st_gid);

  printf("%4d %8s%8s%7ld ",stat->st_nlink, user, group, stat->st_size);
  tm = localtime(&stat->st_mtime);
  printf("%s %2d %5d ",months[tm->tm_mon], tm->tm_mday, tm->tm_year+1900);
  printf("%s\n",path);
}

int main(int argc, char **argv)
{
  IPC_Filsys *fs;
  IPC_File *fp;
  struct direct direct;
  char *path;

  if (argc < 2)
    {
      fprintf(stderr,"Usage: %s <IPC file system> [<path>]\n",argv[0]);
      return -1;
    }
  if (argc == 2) path = "/";
  else path = argv[2];
  fs = IPC_Mount(argv[1]);
  if (!fs)
    {
      perror(argv[1]);
      return -1;
    }
  fp = IPC_open(fs,path);
  if (!fp)
    {
      fprintf(stderr,"%s: no such file or directory\n", path);
      return -1;
    }
  if (!dinode_isdir(fp->dinode))
    {				/* List a regular file */
      struct stat stat;
      IPC_stat(fs,path,&stat);
      ls(path,&stat);
    }
  else {			/* List a directory */
    struct stat stat;
    char *prefix = (char *) alloca(strlen(path)+2);

    /* Try to ignore leading dots and slashes. */
    if (strspn(path,"/.") == strlen(path)) prefix = "";
    else
      {
	strcpy(prefix,path);
	strcat(prefix,"/");
      }

    while (IPC_read(fp,&direct,sizeof(struct direct)) != -1)
      {
	struct stat stat;
	char *buf = (char *) alloca(strlen(prefix)+DIRSIZ+2);
	strcpy(buf,prefix);
	strncat(buf,direct.d_name,DIRSIZ);
	buf[strlen(prefix)+DIRSIZ+1] = '\0';
	IPC_stat(fs,buf,&stat);
	ls(buf,&stat);
      }
  }
  IPC_close(fp);
}
