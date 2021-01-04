/******************************************************************************
 *
 *      filsys.h
 *
 *	Fundamental constants of the filesystem.
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

#ifndef BSIZE
#define	BSIZE	1024		/* size of secondary block (bytes) */
#endif				/* also defined in param.h */
#define	BMASK	(BSIZE-1)	/* BSIZE-1 */
#define	BSHIFT	10		/* LOG2(BSIZE) */

#define	NINDIR	(BSIZE/sizeof(IPC_daddr_t))
#define	NMASK	(NINDIR-1)	/* NINDIR-1 */
#define	NSHIFT	8		/* LOG2(NINDIR) */

#define	INOPB	16		/* inodes per block */
#define	INOSHFT 4 		/* LOG2(INOPB) */
#define INOSHIFT INOSHFT	/* for compatibility between SysV and HP-UX */
#define	INOMOD  0xf		/* value to and to give offset */

#define	ROOTINO	((IPC_ino_t)2)	/* i number of all roots */
#define	SUPERB	((IPC_daddr_t)1)	/* block number of the super block */

#define	DIRSIZ	14		/* max characters per directory */

#define	NICINOD	250		/* number of superblock inodes */
#define	NICFREE	100		/* number of superblock free blocks */

/* system V inode and block limits for the superblock */

#define NICINODV 100
#define NICFREEV 50

/* Some macros for units conversion */

/* Core clicks (1024 bytes) to disk blocks */
#define	ctod(x)	(x)

/* inumber to disk address */
#define	itod(x)	(daddr_t)((((unsigned)x+INOPB*2-1)>>INOSHFT))

/* inumber to disk offset */
#define	itoo(x)	(int)((x+31)&0xf)		/* x mod INOPB */

/* get proper inode pointer limit */

#define NICINODF ((fp->s_type == HPUX) ? NICINOD : NICINODV)
#define NICFREEF ((fp->s_type == HPUX) ? NICFREE : NICFREEV)



/* Structure of the super-block */

struct	filsys
{
	long  	s_isize;		/* size in blocks of i-list */
	daddr_t	s_fsize;		/* size in blocks of entire volume */
	long 	s_nfree;		/* number of addresses in s_free */
	daddr_t	s_free[NICFREE];	/* free block list */
	long 	s_ninode;		/* number of i-nodes in s_inode */
	ino_t	s_inode[NICINOD];	/* free i-node list */
	char	s_flock;		/* lock during free list manipulation */
	char	s_ilock;		/* lock during i-list manipulation */
	char  	s_fmod; 		/* super block modified flag */
	char	s_ronly;		/* mounted read-only flag */
	time_t	s_time; 		/* last super block update */
	short	s_dinfo[4];		/* device information */
	daddr_t	s_tfree;		/* total free blocks*/
	ino_t	s_tinode;		/* total free inodes */
	char	s_fname[6];		/* file system name */
	char	s_fpack[6];		/* file system pack name */
	long	s_magic;	/* magic number to indicate new file system */
	long	s_type;		/* type of new file system */
};
/*
 * Structure of the super-block
 */
struct	filsysv
{
	ushort	s_isize;	/* size in blocks of i-list */
	daddr_t	s_fsize;	/* size in blocks of entire volume */
	short	s_nfree;	/* number of addresses in s_free */
	daddr_t	s_free[NICFREEV];	/* free block list */
	short	s_ninode;	/* number of i-nodes in s_inode */
	ino_t	s_inode[NICINODV];	/* free i-node list */
	char	s_flock;	/* lock during free list manipulation */
	char	s_ilock;	/* lock during i-list manipulation */
	char  	s_fmod; 	/* super block modified flag */
	char	s_ronly;	/* mounted read-only flag */
	time_t	s_time; 	/* last super block update */
	short	s_dinfo[4];	/* device information */
	daddr_t	s_tfree;	/* total free blocks*/
	ino_t	s_tinode;	/* total free inodes */
	char	s_fname[6];	/* file system name */
	char	s_fpack[6];	/* file system pack name */
	long	s_fill[13];	/* ADJUST to make sizeof filsys be 512 */
	long	s_magic;	/* magic number to indicate new file system */
	long	s_type;		/* type of new file system */
};

#define	FsMAGIC	0xfd187e20	/* s_magic number */

#define	Fs1b	1	/* 512 byte block */
#define	Fs2b	2	/* 1024 byte block */
