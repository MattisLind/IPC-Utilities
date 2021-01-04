/******************************************************************************
 *
 *      dir.h
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

#ifndef	DIRSIZ
#define	DIRSIZ	14
#endif
struct	direct
{
	ino_t	d_ino;
	char	d_name[DIRSIZ];
};
