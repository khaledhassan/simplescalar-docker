#define BBSIZE		8192
#define SBSIZE		8192
#define	BBLOCK		((daddr_t)(0))
#define	SBLOCK		((daddr_t)(BBLOCK + BBSIZE / DEV_BSIZE))

#define	ROOTINO		((ino_t)2)
#define LOSTFOUNDINO	(ROOTINO + 1)

#define	NRPOS		8

#define	MAXIPG		2048

#define MINBSIZE	4096
#define	MAXCPG		32

#define MAXMNTLEN 500
#define MAXCSBUFS 32

struct csum {
	long	cs_ndir;
	long	cs_nbfree;
	long	cs_nifree;
	long	cs_nffree;
};

#define PT_MAGIC	0x032957
#define PT_VALID	1
 
struct pt {
	long	pt_magic;
	int	pt_valid;
	struct  pt_info {
		int	pi_nblocks;
		daddr_t	pi_blkoff;
	} pt_part[8];
};

#define	FS_MAGIC	0x011954
struct	fs
{
	struct	fs *fs_link;
	struct	fs *fs_rlink;
	daddr_t	fs_sblkno;
	daddr_t	fs_cblkno;
	daddr_t	fs_iblkno;
	daddr_t	fs_dblkno;
	long	fs_cgoffset;
	long	fs_cgmask;
	time_t 	fs_time;
	long	fs_size;
	long	fs_dsize;
	long	fs_ncg;
	long	fs_bsize;
	long	fs_fsize;
	long	fs_frag;
	long	fs_minfree;
	long	fs_rotdelay;
	long	fs_rps;
	long	fs_fmask;
	long	fs_bshift;
	long	fs_fshift;
	long	fs_maxcontig;
	long	fs_maxbpg;
	long	fs_fragshift;
	long	fs_fsbtodb;
	long	fs_sbsize;
	long	fs_csmask;
	long	fs_csshift;
	long	fs_nindir;
	long	fs_inopb;
	long	fs_nspf;
	long	fs_optim;
	long	fs_sparecon[5];
	daddr_t fs_csaddr;
	long	fs_cssize;
	long	fs_cgsize;
	long	fs_ntrak;
	long	fs_nsect;
	long  	fs_spc;
	long	fs_ncyl;
	long	fs_cpg;
	long	fs_ipg;
	long	fs_fpg;
	struct	csum fs_cstotal;
	char   	fs_fmod;
	char   	fs_clean;
	char   	fs_ronly;
	char   	fs_flags;
#define	fs_cleantimer	fs_flags
	char	fs_fsmnt[MAXMNTLEN];
	char	fs_deftimer;
	char	fs_extra[3];
	time_t	fs_lastfsck;
	u_int	fs_gennum;
	long	fs_cgrotor;
	struct	csum *fs_csp[MAXCSBUFS];
	long	fs_cpc;
	short	fs_postbl[MAXCPG][NRPOS];
	long	fs_magic;
	u_char	fs_rotbl[1];
};

#define FS_OPTTIME	0
#define FS_OPTSPACE	1

#define fs_cs(fs, indx) \
	fs_csp[(indx) >> (fs)->fs_csshift][(indx) & ~(fs)->fs_csmask]
#define	MAXBPC	(SBSIZE - sizeof (struct fs))

#define	CG_MAGIC	0x090255
struct	cg {
	struct	cg *cg_link;
	struct	cg *cg_rlink;
	time_t	cg_time;
	long	cg_cgx;
	short	cg_ncyl;
	short	cg_niblk;
	long	cg_ndblk;
	struct	csum cg_cs;
	long	cg_rotor;
	long	cg_frotor;
	long	cg_irotor;
	long	cg_frsum[MAXFRAG];
	long	cg_btot[MAXCPG];
	short	cg_b[MAXCPG][NRPOS];
	char	cg_iused[MAXIPG/NBBY];
	long	cg_magic;
	u_char	cg_free[1];
};

#define	MAXBPG(fs) \
	(fragstoblks((fs), (NBBY * ((fs)->fs_bsize - (sizeof (struct cg))))))

#define fsbtodb(fs, b)	((b) << (fs)->fs_fsbtodb)
#define	dbtofsb(fs, b)	((b) >> (fs)->fs_fsbtodb)

#define	cgbase(fs, c)	((daddr_t)((fs)->fs_fpg * (c)))
#define cgstart(fs, c) \
	(cgbase(fs, c) + (fs)->fs_cgoffset * ((c) & ~((fs)->fs_cgmask)))
#define	cgsblock(fs, c)	(cgstart(fs, c) + (fs)->fs_sblkno)
#define	cgtod(fs, c)	(cgstart(fs, c) + (fs)->fs_cblkno)
#define	cgimin(fs, c)	(cgstart(fs, c) + (fs)->fs_iblkno)
#define	cgdmin(fs, c)	(cgstart(fs, c) + (fs)->fs_dblkno)

#define	itoo(fs, x)	((x) % INOPB(fs))
#define	itog(fs, x)	((x) / (fs)->fs_ipg)
#define	itod(fs, x) \
	((daddr_t)(cgimin(fs, itog(fs, x)) + \
	(blkstofrags((fs), (((x) % (fs)->fs_ipg) / INOPB(fs))))))

#define	dtog(fs, d)	((d) / (fs)->fs_fpg)
#define	dtogd(fs, d)	((d) % (fs)->fs_fpg)

#define blkmap(fs, map, loc) \
    (((map)[(loc) / NBBY] >> ((loc) % NBBY)) & (0xff >> (NBBY - (fs)->fs_frag)))
#define cbtocylno(fs, bno) \
	((bno) * NSPF(fs) / (fs)->fs_spc)
#define cbtorpos(fs, bno) \
    ((bno) * NSPF(fs) % (fs)->fs_spc % (fs)->fs_nsect * NRPOS / (fs)->fs_nsect)

#define blkoff(fs, loc)\
	((loc) & ~(fs)->fs_bmask)
#define fragoff(fs, loc)\
	((loc) & ~(fs)->fs_fmask)
#define lblkno(fs, loc)\
	((loc) >> (fs)->fs_bshift)
#define numfrags(fs, loc)\
	((loc) >> (fs)->fs_fshift)
#define blkroundup(fs, size)\
	(((size) + (fs)->fs_bsize - 1) & (fs)->fs_bmask)
#define fragroundup(fs, size)\
	(((size) + (fs)->fs_fsize - 1) & (fs)->fs_fmask)
#define fragstoblks(fs, frags)\
	((frags) >> (fs)->fs_fragshift)
#define blkstofrags(fs, blks)\
	((blks) << (fs)->fs_fragshift)
#define fragnum(fs, fsb)\
	((fsb) & ((fs)->fs_frag - 1))
#define blknum(fs, fsb)\
	((fsb) &~ ((fs)->fs_frag - 1))

#define freespace(fs, percentreserved) \
	(blkstofrags((fs), (fs)->fs_cstotal.cs_nbfree) + \
	(fs)->fs_cstotal.cs_nffree - ((fs)->fs_dsize * (percentreserved) / 100))

#define blksize(fs, gp, lbn) \
	(((lbn) >= NDADDR || (gp)->g_size >= ((lbn) + 1) << (fs)->fs_bshift) \
	    ? (fs)->fs_bsize \
	    : (fragroundup(fs, blkoff(fs, (gp)->g_size))))
#define dblksize(fs, dip, lbn) \
	(((lbn) >= NDADDR || (dip)->di_size >= ((lbn) + 1) << (fs)->fs_bshift) \
	    ? (fs)->fs_bsize \
	    : (fragroundup(fs, blkoff(fs, (dip)->di_size))))

#define	NSPB(fs)	((fs)->fs_nspf << (fs)->fs_fragshift)
#define	NSPF(fs)	((fs)->fs_nspf)

#define	INOPB(fs)	((fs)->fs_inopb)
#define	INOPF(fs)	((fs)->fs_inopb >> (fs)->fs_fragshift)

#define	NINDIR(fs)	((fs)->fs_nindir)

#define FSCLEAN_TIMEOUTFACTOR 20
#define FSCLEAN_UPDATES	10000
#define FS_CLEAN	30

#define CHECK_CLEAN_THRESHOLD(fs, devname, ceiling, reason) \
        { \
		if (!fs->fs_cleantimer || fs->fs_cleantimer-- <= 1) { \
			fs->fs_cleantimer = 0; \
			uprintf("Warning, %s has exceeded %d %s threshold, fsck(8) is advised\n", devname, ceiling, reason); \
		} \
	}
