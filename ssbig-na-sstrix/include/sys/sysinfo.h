
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

#define GSI_PROG_ENV	1
#define GSI_MAX_UPROCS	2
#define	GSI_TTYP	3
#define GSI_NETBLK	4
#define GSI_BOOTDEV	5

#ifdef __mips__
#define GSI_UACSYS      6
#define GSI_UACPARNT    7
#define GSI_UACPROC     8
#endif /* __mips */

#define GSI_LMF         9
#define GSI_WSD_TYPE    10
#define GSI_WSD_UNITS   11
#define GSI_MMAPALIGN   12
#define	GSI_BOOTTYPE	13
#define GSI_VPTOTAL	14
#define GSI_SCS		15
#define	GSI_PHYSMEM	19
#define GSI_DNAUID	20
#define	GSI_BOOTCTLR	21
#define	GSI_CONSTYPE	22

#define	SSI_NVPAIRS	1
#define	SSI_ZERO_STRUCT	2
#define	SSI_SET_STRUCT	3

#define	SSIN_NFSPORTMON 1
#define	SSIN_NFSSETLOCK	2
#define SSIN_PROG_ENV	3

#ifdef __mips__
#define SSIN_UACSYS	4
#define SSIN_UACPARNT	5
#define SSIN_UACPROC	6
#endif /* __mips */

#define SSI_LMF         7
#define SSI_LOGIN	8

