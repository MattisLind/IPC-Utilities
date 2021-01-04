/******************************************************************************
 *
 *      ino.h
 *
 *	Inode structure as it appears on a disk block.
 *
 *	5.0 IPC Header File
 *
 *	************************************************************
 *	*     "(c) Copyright Hewlett-Packard Company, 1985.   All  *
 *	*     rights are reserved.  Copying or other reproduction  *
 *	*     of this program except for archival purposes is      *
 *	*     prohibited without the prior written consent of      *
 *	*     Hewlett-Packard Company."				   *
 *	************************************************************
 *
 ******************************************************************************/

struct dinode
{
	ushort di_mode;		/* mode and type of file */
	short	di_nlink;    	/* number of links to file */
	ushort	di_uid;      	/* owner's user id */
	ushort	di_gid;      	/* owner's group id */
	off_t	di_size;     	/* number of bytes in file */
	union {
		char  	di_a[40];	/* disk block addresses */
		dev_t	di_r;
	} di_un;
	time_t	di_atime;   	/* time last accessed */
	time_t	di_mtime;   	/* time last modified */
	time_t	di_ctime;   	/* time created */
};
#define di_addr di_un.di_a
#define di_rdev di_un.di_r
/*
 * the 40 address bytes:
 *	39 used; 13 addresses
 *	of 3 bytes each.
 */
