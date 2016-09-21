
#if !defined(__STDC__) && !defined(__DECC) && !defined(__ANSI_COMPAT)

#define __ANSI_COMPAT

#ifdef ultrix
#define __ultrix      ultrix
#endif

#ifdef __unix__
#define __unix        unix
#endif

#ifdef bsd4_2
#define __bsd4_2      bsd4_2
#endif

#ifdef __vax__
#define __vax         vax
#endif

#ifdef VAX
#define __VAX         VAX
#endif

#ifdef __mips__
#define __mips        mips
#endif

#ifdef __host_mips__
#define __host_mips   host_mips
#endif

#ifdef __MIPSEL__
#define __MIPSEL      MIPSEL
#endif

#ifdef __MIPSEB__
#define __MIPSEB      MIPSEB
#endif

#ifdef SYSTEM_FIVE
#define __SYSTEM_FIVE SYSTEM_FIVE
#endif

#ifdef POSIX
#define __POSIX       POSIX
#endif

#ifdef GFLOAT
#define __GFLOAT        GFLOAT
#endif

#ifdef LANGUAGE_C
#define __LANGUAGE_C  LANGUAGE_C
#endif

#ifdef vaxc
#define __vaxc   vaxc
#define __VAXC   VAXC
#define __vax11c vax11c
#define __VAX11C VAX11C
#endif

#ifdef MOXIE
#define __MOXIE   MOXIE
#endif

#ifdef ULTRIX022
#define __ULTRIX022 ULTRIX022
#endif

#endif

#define PGS_PER_MB	256
#define MINMEM_MB	6
#define MINMEM_PGS	(MINMEM_MB * PGS_PER_MB)
#define MAXMEM_MB	480
#define MAXMEM_PGS	(MAXMEM_MB * PGS_PER_MB)

#define	NBPG		4096
#define	PGOFSET		(NBPG-1)
#define	PGSHIFT		12
#define	DBSHIFT		9

#define	CLSIZE		1
#define	CLSIZELOG2	0

#define	SSIZE		1
#define	SINCR		1

#define	UPAGES		2
#define FORKPAGES	UPAGES
#define	KERNELSTACK	0xffffe000
#define	UADDR		0xffffc000
#define	UVPN		(UADDR>>PGSHIFT)
#ifndef UCLEAR
#define UCLEAR		4
#endif

#define	ctos(x)	(x)
#define	stoc(x)	(x)

#define	ctod(x)	((x)<<(PGSHIFT-DBSHIFT))
#define	dtoc(x)	((unsigned)(x)>>(PGSHIFT-DBSHIFT))
#define	dtob(x)	((x)<<DBSHIFT)

#define	ctob(x)	((x)<<PGSHIFT)

#define	btoc(x)	(((unsigned)(x)+PGOFSET)>>PGSHIFT)

#define	USERMODE(sr)	(((sr) & SR_KUP) == SR_KUP)
#ifndef LOCORE
extern unsigned int sr_usermask;
#endif 
#define	BASEPRI(sr)	(((sr) & SR_IMASK) == (sr_usermask & SR_IMASK))

#ifdef SABLE
#define	DELAY(n)	{ register int N = 3*(n); while (--N > 0); }
#else
#define	DELAY(n) { microdelay(n);}
#endif
#define	NBPTE		4
#define	NUMPTEPG	(NBPG/NBPTE)

#define	NPTEPG		(NBPG/(sizeof (struct pte)))

#ifndef SMSEG
#define	SMSEG	6
#endif

#ifdef __vax
#ifndef SMMAX
#define	SMMAX	256
#endif

#ifndef SMMIN
#define	SMMIN	0
#endif

#ifndef SMBRK
#define	SMBRK	64
#endif

#ifndef SMSMAT
#define	MAXSMAT MAXDSIZ
#else
#define	MAXSMAT	(SMSMAT*1024*1024) 
#endif
#endif

#ifdef __mips
#ifndef SMMAX
#define	SMMAX	32
#endif

#ifndef SMMIN
#define	SMMIN	0
#endif

#ifndef SMBRK
#define	SMBRK	10
#endif

#define	MAXSMAT	0
#endif


#define	NMOUNT	254
#define	MSWAPX	254
#define NUM_FS  0xff
#define MINPGTHRESH 8192

#define	NOFILE	64

#define	CANBSIZ	256
#ifdef __vax
#define	NCARGS	10240
#endif
#ifdef __mips
#define	NCARGS	20480
#endif
#define	NGROUPS	32

#define	NOGROUP	-1

#define PCATCH  0400
#define PMASK   0177
#define	PSWP	0
#define	PINOD	10
#define	PRIBIO	20
#define	PRIUBA	24
#define	PZERO	25
#define	PPIPE	26
#define	PWAIT	30
#define	PLOCK	35
#define	PSLEP	40
#define	PUSER	50

#define	NZERO 20

#ifndef LOCORE
#include <signal.h>

#define	ISSIG(p,l) \
	((p)->p_sig && ((p)->p_trace&STRC || \
	 ((p)->p_sig &~ ((p)->p_sigignore | (p)->p_sigmask))) && issig(l))
#endif

#define	NBBY	8
#define	NBPW	sizeof(int)

#define NBUNS   (sizeof(unsigned) * NBBY)
 
#ifndef NULL
#define	NULL	0
#endif
#define	CMASK	022
#define	NODEV	(dev_t)(-1)

#define	CLBYTES		(CLSIZE*NBPG)
#define	CLOFSET		(CLSIZE*NBPG-1)
#define	claligned(x)	((((int)(x))&CLOFSET)==0)
#define	CLOFF		CLOFSET
#define	CLSHIFT		(PGSHIFT+CLSIZELOG2)

#if CLSIZE==1
#define	clbase(i)	(i)
#define	clrnd(i)	(i)
#else
#define	clbase(i)	((i) &~ (CLSIZE-1))
#define	clrnd(i)	(((i) + (CLSIZE-1)) &~ (CLSIZE-1))
#endif

#ifndef INTRLVE
#define	dkblock(bp)	((bp)->b_blkno)
#define	dkunit(bp)	(minor((bp)->b_dev) >> 3)
#endif

#define	CBSIZE	52
#define	CROUND	0x3F

#include	<sys/types.h>

#define	MAXBSIZE	8192
#define	DEV_BSIZE	512
#define	DEV_BSHIFT	9
#define BLKDEV_IOSIZE	2048
#define MAXFRAG 	8
#define FSDUNIT		1024

#define	btodb(bytes)							\
	((unsigned)(bytes) >> DEV_BSHIFT)
#define	dbtob(db)							\
	((unsigned)(db) << DEV_BSHIFT)

#define	bdbtofsb(bn)	((bn) / (BLKDEV_IOSIZE/DEV_BSIZE))

#define MAXPATHLEN	1024
#define MAXSYMLINKS	32
#define MAXHOSTNAMELEN 64

#define	setbit(a,i)	((a)[(i)/NBBY] |= 1<<((i)%NBBY))
#define	clrbit(a,i)	((a)[(i)/NBBY] &= ~(1<<((i)%NBBY)))
#define	isset(a,i)	((a)[(i)/NBBY] & (1<<((i)%NBBY)))
#define	isclr(a,i)	(((a)[(i)/NBBY] & (1<<((i)%NBBY))) == 0)

#define	MIN(a,b) (((a)<(b))?(a):(b))
#define	MAX(a,b) (((a)>(b))?(a):(b))

#define	howmany(x, y)	(((x)+((y)-1))/(y))
#define	roundup(x, y)	((((x)+((y)-1))/(y))*(y))

