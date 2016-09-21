struct utmp {
	char	ut_line[8];		/* tty name */
	char	ut_name[8];		/* user id */
	char	ut_host[16];		/* host name, if remote */
	long	ut_time;		/* time on */
};

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

#define	UTMP_FILE	"/etc/utmp"
#define	WTMP_FILE	"/usr/adm/wtmp"

#define	EMPTY		""
#define	BOOT_MSG	"~"
#define	OTIME_MSG	"|"
#define	NTIME_MSG	"}"

#ifdef __SYSTEM_FIVE
#define	ut_user ut_name
#endif /* __SYSTEM_FIVE */
