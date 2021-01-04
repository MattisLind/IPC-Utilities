/*
 *  Utilities for reading HP-UX 5.0 filesystems
 *  Version 1
 *
 *  This program may be freely redistributed,
 *  but this entire comment MUST remain intact.
 *
 *  Copyright (c) 2000, Peter A. Johnson
 */

#ifndef __IPC_UTILS_H__
#define __IPC_UTILS_H__
#include "ipc/types_protect.h"
#include "ipc/types.h"
#include "ipc/filsys.h"
#include "ipc/ino.h"
#include "ipc/disc_hdr.h"
#include "ipc/dir.h"
#include "ipc/stat.h"
#include "ipc/types_unprotect.h"

#include <stdio.h>

typedef unsigned char IPC_Block[BSIZE];
typedef IPC_daddr_t IPC_Block_table[NINDIR];
typedef struct IPC_Filsys_Struct {
  FILE *fp;
  unsigned char   *raw_data;
  IPC_Block       *block_data;
  struct disc_hdr *disc_hdr;
  struct filsys   *filsys;
  struct dinode   *dinode;
} IPC_Filsys;

typedef struct IPC_File_Struct {
  IPC_Filsys *filsys;
  struct dinode *dinode;
  int inum;
  long offset;
  long block_index[4];
  IPC_Block *current_block;
  IPC_Block_table *block_table[3];
} IPC_File;

extern unsigned dinode_addr(struct dinode *dinode, unsigned addr);
extern int dinode_isdir(struct dinode *dinode);
extern void IPC_close(IPC_File *fp);
extern IPC_File *IPC_open(IPC_Filsys *fs, const char *fname);
extern int IPC_read(IPC_File *fp, void *buf, int nbytes);
extern IPC_Filsys *IPC_Mount(char *device);
extern int IPC_stat(IPC_Filsys *fp, const char *path, struct stat *buf);

static char *ipc_utils_h_revision = "$Id: ipc_utils.h,v 1.4 2000/10/25 15:32:12 pete Exp $";
#endif
