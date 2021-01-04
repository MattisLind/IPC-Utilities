/******************************************************************************
 *
 *      types.h
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

#if !defined(TYPES_H) /* don't allow multiple includes of this file */
#define TYPES_H 1
typedef	struct { int r[1]; } *	physadr;

typedef unsigned short	iord_t;
typedef char		swck_t;
typedef unsigned char	use_t;
#define MAXSUSE		255		/* maximum share count on swap */
#define NREGS_S		13		/* number of regs saved in save (a/d2-7,pc) */

typedef	long		daddr_t;
typedef	char *		caddr_t;
typedef	unsigned int	uint;
typedef	unsigned short	ushort;
typedef	ushort		ino_t;
typedef short		cnt_t;
typedef	long		time_t;
typedef	int		label_t[NREGS_S];
typedef	long		dev_t;
typedef	long		off_t;
typedef	long		paddr_t;
typedef	long		key_t;
typedef ushort		pgadr_t;
#endif not TYPES_H
