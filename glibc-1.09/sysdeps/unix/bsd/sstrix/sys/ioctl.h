/* Copyright (C) 1991, 1992, 1993, 1994 Free Software Foundation, Inc.
This file is part of the GNU C Library.

The GNU C Library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

The GNU C Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with the GNU C Library; see the file COPYING.LIB.  If
not, write to the, 1992 Free Software Foundation, Inc., 675 Mass Ave,
Cambridge, MA 02139, USA.  */

/* Get the list of `ioctl' requests and related constants.  */

#define ALLDELAY 0x0000ff00
#define ANYP 0x000000c0
#define AUTOFLOW 0x02000000
#define BNOFLSH 0x80000000
#define BS0 0x00000000
#define BS1 0x00008000
#define BSDELAY 0x00008000
#define BSDLY 0x00008000
#define CBAUD 0x0000000f
#define CBREAK 0x00000002
#define CDEL 0x000000ff
#define CESC 0x0000005c
#define CNSWTCH 0x00000000
#define CNUL 0x00000000
#define CR0 0x00000000
#define CR1 0x00001000
#define CR2 0x00002000
#define CR3 0x00003000
#define CRDELAY 0x00003000
#define CRDLY 0x00003000
#define CRMOD 0x00000010
#define CRTBS 0x00010000
#define CRTERA 0x00040000
#define CRTKIL 0x04000000
#define CSWTCH 0x0000001a
#define CTLECH 0x10000000
#define DECCTQ 0x40000000
#define DEVGETGEOM 0x407c7602
#define DEVIOCGET 0x403c7601
#define DIOCDGTPT 0x40487003
#define DIOCGETPT 0x40487001
#define DIOCSETPT 0x80487002
#define DKIOCACC 0xc0146404
#define DKIOCDOP 0x80086402
#define DKIOCEXCL 0xc0046405
#define DKIOCGET 0x40086403
#define DKIOCHDR 0x20006401
#define ELCLRPID 0x20006504
#define ELGETPID 0x40046501
#define ELGETTIME 0x40046507
#define ELMOVPTR 0x80046502
#define ELREINIT 0x20006503
#define ELSETPID 0xc0086500
#define ELWARNOFF 0x20006505
#define ELWARNON 0x20006506
#define EVENP 0x00000080
#define FF0 0x00000000
#define FF1 0x00004000
#define FFDLY 0x00004000
#define FIOASYNC 0x8004667d
#define FIOCINUSE 0x20006604
#define FIOCLEX 0x20006601
#define FIOGETOWN 0x4004667b
#define FIONBDONE 0x80046678
#define FIONBIO 0x8004667e
#define FIONBUF 0x8004667a
#define FIONCLEX 0x20006602
#define FIONONBUF 0x20006679
#define FIONREAD 0x4004667f
#define FIOSETOWN 0x8004667c
#define FIOSINUSE 0x20006603
#define HCLDISC 0x00000005
#define IUCLC 0x00000200
#define LAUTOFLOW 0x00000200
#define LCASE 0x00000004
#define LCRTBS 0x00000001
#define LCRTERA 0x00000004
#define LCRTKIL 0x00000400
#define LCTLECH 0x00001000
#define LDECCTQ 0x00004000
#define LFLUSHO 0x00000080
#define LITOUT 0x00200000
#define LLITOUT 0x00000020
#define LMDMBUF 0x00000010
#define LNOFLSH 0xffff8000
#define LNOHANG 0x00000100
#define LOBLK 0x00001000
#define LPASS8 0x00000800
#define LPENDIN 0x00002000
#define LPRTERA 0x00000002
#define LTILDE 0x00000008
#define LTOSTOP 0x00000040
#define NCC 0x0000000a
#define NETLDISC 0x00000001
#define NL0 0x00000000
#define NL1 0x00000100
#define NL2 0x00000200
#define NL3 0x00000300
#define NLDELAY 0x00000300
#define NLDLY 0x00000100
#define NOHANG 0x01000000
#define NTABLDISC 0x00000004
#define NTTYDISC 0x00000002
#define OBAUD 0x000f0000
#define OCRNL 0x00000008
#define ODDP 0x00000040
#define OFDEL 0x00000080
#define OFILL 0x00000040
#define OLCUC 0x00000002
#define ONLRET 0x00000020
#define ONOCR 0x00000010
#define OTTYDISC 0x00000000
#define PASS8 0x08000000
#define PAUTOFLOW 0x00000010
#define PCBREAK 0x00000002
#define PCMDISC 0x00000008
#define PCRTBS 0x00000004
#define PCRTERA 0x00000008
#define PCRTKIL 0x00000010
#define PCTLECH 0x00000001
#define PFLUSHO 0x00000002
#define PIEXTEN 0x00000080
#define PLITOUT 0x00000004
#define PNL2 0x00000008
#define PPENDIN 0x00000001
#define PPRTERA 0x00000002
#define PRAW 0x00000020
#define PRTERA 0x00020000
#define PTILDE 0x00000001
#define PTOSTOP 0x00000040
#define RAW 0x00000020
#define SIOCADDMULTI 0xc0206918
#define SIOCADDRT 0x8034720a
#define SIOCARPREQ 0xc0206928
#define SIOCATMARK 0x40047307
#define SIOCDARP 0x80246920
#define SIOCDELMULTI 0xc0206919
#define SIOCDELRT 0x8034720b
#define SIOCDISABLBACK 0x80206922
#define SIOCEEUPDATE 0x805c6930
#define SIOCENABLBACK 0x80206921
#define SIOCGARP 0xc024691f
#define SIOCGETEVENTS 0xc000692e
#define SIOCGHIWAT 0x40047301
#define SIOCGIFADDR 0xc020690d
#define SIOCGIFBRDADDR 0xc0206912
#define SIOCGIFCONF 0xc0086914
#define SIOCGIFDSTADDR 0xc020690f
#define SIOCGIFFLAGS 0xc0206911
#define SIOCGIFMETRIC 0xc0206929
#define SIOCGIFNETMASK 0xc0206915
#define SIOCGLOWAT 0x40047303
#define SIOCGPGRP 0x40047309
#define SIOCIFRESET 0x8020692f
#define SIOCMANREQ 0xc000692d
#define SIOCRDCTRS 0xc078691a
#define SIOCRDZCTRS 0xc078691b
#define SIOCRPHYSADDR 0xc01c691c
#define SIOCSARP 0x8024691e
#define SIOCSHIWAT 0x80047300
#define SIOCSIFADDR 0x8020690c
#define SIOCSIFBRDADDR 0x80206913
#define SIOCSIFDSTADDR 0x8020690e
#define SIOCSIFFLAGS 0x80206910
#define SIOCSIFMETRIC 0x8020692a
#define SIOCSIFNETMASK 0x80206916
#define SIOCSLOWAT 0x80047302
#define SIOCSPGRP 0x80047308
#define SIOCSPHYSADDR 0xc0206917
#define SIOCSTATE 0xc0446923
#define SLPDISC 0x00000007
#define SSPEED 0x00000007
#define TAB0 0x00000000
#define TAB1 0x00000400
#define TAB2 0x00000800
#define TAB3 0x00000c00
#define TABDLY 0x00000c00
#define TABLDISC 0x00000003
#define TANDEM 0x00000001
#define TAUTOFLOW 0x00100000
#define TBDELAY 0x00000c00
#define TCBREAK 0x00020000
#define TCFLSH 0x2000745c
#define TCRTBS 0x00040000
#define TCRTERA 0x00080000
#define TCRTKIL 0x00100000
#define TCSBRK 0x2000745e
#define TCTLECH 0x00010000
#define TCXONC 0x2000745d
#define TERMIODISC 0x00000006
#define TFLUSHO 0x00020000
#define TILDE 0x00080000
#define TIOAUTO 0x20007456
#define TIOCCAR 0x2000745f
#define TIOCCBRK 0x2000747a
#define TIOCCDTR 0x20007478
#define TIOCCINUSE 0x20006604
#define TIOCCMLB 0x20007464
#define TIOCEXCL 0x2000740d
#define TIOCFLUSH 0x80047410
#define TIOCGETC 0x40067412
#define TIOCGETD 0x40047400
#define TIOCGETP 0x40067408
#define TIOCGLTC 0x40067474
#define TIOCGPGRP 0x40047477
#define TIOCGWINSZ 0x40087468
#define TIOCHPCL 0x20007402
#define TIOCLBIC 0x8004747e
#define TIOCLBIS 0x8004747f
#define TIOCLGET 0x4004747c
#define TIOCLSET 0x8004747d
#define TIOCMASTER 0x80047457
#define TIOCMBIC 0x8004746b
#define TIOCMBIS 0x8004746c
#define TIOCMGET 0x4004746a
#define TIOCMODEM 0x80047462
#define TIOCMODG 0x40047403
#define TIOCMODS 0x80047404
#define TIOCMSET 0x8004746d
#define TIOCM_CAR 0x00000040
#define TIOCM_CD 0x00000040
#define TIOCM_CTS 0x00000020
#define TIOCM_DSR 0x00000100
#define TIOCM_DTR 0x00000002
#define TIOCM_LE 0x00000001
#define TIOCM_RI 0x00000080
#define TIOCM_RNG 0x00000080
#define TIOCM_RTS 0x00000004
#define TIOCM_SR 0x00000010
#define TIOCM_ST 0x00000008
#define TIOCNCAR 0x20007460
#define TIOCNMODEM 0x80047463
#define TIOCNOTTY 0x20007471
#define TIOCNXCL 0x2000740e
#define TIOCOUTQ 0x40047473
#define TIOCPKT 0x80047470
#define TIOCPKT_DATA 0x00000000
#define TIOCPKT_DOSTOP 0x00000020
#define TIOCPKT_FLUSHREAD 0x00000001
#define TIOCPKT_FLUSHWRITE 0x00000002
#define TIOCPKT_IOCTL 0x00000040
#define TIOCPKT_NOSTOP 0x00000010
#define TIOCPKT_START 0x00000008
#define TIOCPKT_STOP 0x00000004
#define TIOCREMOTE 0x80047469
#define TIOCSBRK 0x2000747b
#define TIOCSDTR 0x20007479
#define TIOCSETC 0x80067411
#define TIOCSETD 0x80047401
#define TIOCSETN 0x8006740a
#define TIOCSETP 0x80067409
#define TIOCSINUSE 0x20006603
#define TIOCSLTC 0x80067475
#define TIOCSMLB 0x20007465
#define TIOCSPGRP 0x80047476
#define TIOCSTART 0x2000746e
#define TIOCSTI 0x80017472
#define TIOCSTOP 0x2000746f
#define TIOCSWINSZ 0x80087467
#define TIOCUCNTL 0x80047466
#define TIOCWONLINE 0x20007461
#define TIOCX29GET 0xc07f7802
#define TIOCX29SET 0x807f7801
#define TLITOUT 0x00040000
#define TNL2 0x00080000
#define TPENDIN 0x00010000
#define TPRTERA 0x00020000
#define TRAW 0x00200000
#define TTILDE 0x00010000
#define VFLUSH 0x0000000f
#define VQUOTE 0x00000012
#define VRPRNT 0x0000000e
#define VSWTCH 0x00000007
#define VT0 0x00000000
#define VT1 0x00000200
#define VTDELAY 0x00004000
#define VTDLY 0x00000200
#define X29DSIZE 0x0000007f
#define X29QD_BADMSG 0x000000ff
#define X29QD_BREAKIND 0x00000003
#define X29QD_CONTROL 0x000000fd
#define X29QD_ERROR 0x00000005
#define X29QD_IND 0x00000000
#define X29QD_INVBREAK 0x00000001
#define X29QD_READ 0x00000004
#define X29QD_RLSE 0x000000fe
#define X29QD_SET 0x00000002
#define X29QD_SETREAD 0x00000006
#define XCASE 0x00000004
#define XTABS 0x00000c00

/* On a Unix system, the system <sys/ioctl.h> probably defines some of the
   symbols we define in <sys/ttydefaults.h> (usually with the same values).
   The code to generate <ioctls.h> has omitted these symbols to avoid the
   conflict, but a Unix program expects <sys/ioctl.h> to define them, so we
   must include <sys/ttydefaults.h> here.  */
#include <sys/ttydefaults.h>

#if	defined(TIOCGETC) || defined(TIOCSETC)
/* Type of ARG for TIOCGETC and TIOCSETC requests.  */
struct tchars
{
  char t_intrc;			/* Interrupt character.  */
  char t_quitc;			/* Quit character.  */
  char t_startc;		/* Start-output character.  */
  char t_stopc;			/* Stop-output character.  */
  char t_eofc;			/* End-of-file character.  */
  char t_brkc;			/* Input delimiter character.  */
};

#define	_IOT_tchars	/* Hurd ioctl type field.  */ \
  _IOT (_IOTS (char), 6, 0, 0, 0, 0)
#endif

#if	defined(TIOCGLTC) || defined(TIOCSLTC)
/* Type of ARG for TIOCGLTC and TIOCSLTC requests.  */
struct ltchars
{
  char t_suspc;			/* Suspend character.  */
  char t_dsuspc;		/* Delayed suspend character.  */
  char t_rprntc;		/* Reprint-line character.  */
  char t_flushc;		/* Flush-output character.  */
  char t_werasc;		/* Word-erase character.  */
  char t_lnextc;		/* Literal-next character.  */
};

#define	_IOT_ltchars	/* Hurd ioctl type field.  */ \
  _IOT (_IOTS (char), 6, 0, 0, 0, 0)
#endif

/* Type of ARG for TIOCGETP and TIOCSETP requests (and gtty and stty).  */
struct sgttyb
{
  char sg_ispeed;		/* Input speed.  */
  char sg_ospeed;		/* Output speed.  */
  char sg_erase;		/* Erase character.  */
  char sg_kill;			/* Kill character.  */
  short int sg_flags;		/* Mode flags.  */
};

#define	_IOT_sgttyb	/* Hurd ioctl type field.  */ \
  _IOT (_IOTS (char), 6, _IOTS (short int), 1, 0, 0)

#if	defined(TIOCGWINSZ) || defined(TIOCSWINSZ)
/* Type of ARG for TIOCGWINSZ and TIOCSWINSZ requests.  */
struct winsize
{
  unsigned short int ws_row;	/* Rows, in characters.  */
  unsigned short int ws_col;	/* Columns, in characters.  */

  /* These are not actually used.  */
  unsigned short int ws_xpixel;	/* Horizontal pixels.  */
  unsigned short int ws_ypixel;	/* Vertical pixels.  */
};

#define	_IOT_winsize	/* Hurd ioctl type field.  */ \
  _IOT (_IOTS (unsigned short int), 4, 0, 0, 0, 0)
#endif

#if	defined (TIOCGSIZE) || defined (TIOCSSIZE)
#  if defined (TIOCGWINSZ) && TIOCGSIZE == TIOCGWINSZ
/* Many systems that have TIOCGWINSZ define TIOCGSIZE for source
   compatibility with Sun; they define `struct ttysize' to have identical
   layout as `struct winsize' and #define TIOCGSIZE to be TIOCGWINSZ
   (likewise TIOCSSIZE and TIOCSWINSZ).  */
struct ttysize
{
  unsigned short int ts_lines;
  unsigned short int ts_cols;
  unsigned short int ts_xxx;
  unsigned short int ts_yyy;
};
#define	_IOT_ttysize	_IOT_winsize
#  else
/* Suns use a different layout for `struct ttysize', and TIOCGSIZE and
   TIOCGWINSZ are separate commands that do the same thing with different
   structures (likewise TIOCSSIZE and TIOCSWINSZ).  */
struct ttysize
{
  int ts_lines, ts_cols;	/* Lines and columns, in characters.  */
};
#  endif
#endif

/* Perform the I/O control operation specified by REQUEST on FD.
   One argument may follow; its presence and type depend on REQUEST.
   Return value depends on REQUEST.  Usually -1 indicates error.  */
extern int __ioctl __P ((int __fd, unsigned long int __request, ...));
extern int ioctl __P ((int __fd, unsigned long int __request, ...));

