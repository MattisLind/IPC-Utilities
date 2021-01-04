/******************************************************************************
 *
 *      disc_hdr.h
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

#define HPUX	0x3000	/* media logical format id for HPUX */
#define SYSV	0x4000	/* media logical format id for SYSTEM V */

struct disc_hdr {
	short	mlfi;	/* media logical format identifier */
	char	vollbl[6];	/* volume label - disk name */
	long	l_dstart;	/* sec in which dir starts */
	short	hack_3000;	/* need a 010000 for hp3000 */
	short	dummy1;		/* set to 0 */
	long	l_dirlen;	/* length of LIF dir */
	short	version;	/* set to 1 because we include ext 1 */
	short	dummy2;		/* set to 0 */
	/*
	 * LIF level 1 extensions
	 */
	int	trkpersurf;	/* number of tracks per surface */
	int	surfpermed;	/* number of surfaces per media */
	int	sectpertrk;	/* number of sectors per track */
	char	vctime[6];	/* volume time of creation */
				/* format YYMMDDHHMMSS in bcd */
};

/* sector 1 is all 0; rest of sector 0 is zeros */

/*
 * The lif_filent structure defines a file entry in a
 * LIF directory.
 */
struct lif_filent {
	char	l_filnam[10];
	short	l_filtyp;
	long	l_startsec;
	long	l_fillen;
	char	l_create_time[6];	/* 12 bcd digits */
	short	l_flagwd;		/* bool : 1 -- last volume */
					/* int  : 15 -- volume nubter */
					/* always T,1 */
	long	l_startexec;		/* start exec for system file */
};

typedef struct lif_filent	lif_filtab[];

#define LIF_ASCII	1
#define LIF_SYSTEM	-5822
#define LIF_EMPTY	0
#define LIF_END_DIR	-1
