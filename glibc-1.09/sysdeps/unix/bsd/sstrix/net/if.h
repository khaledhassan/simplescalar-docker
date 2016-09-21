#define	IFVERLEN	512
struct ifnet {
	char	*if_name;
	short	if_unit;
	short	if_mtu;
#ifdef old
	int	if_net;
#endif
	short	if_flags;
	short	if_timer;
	int	if_metric;
	struct	ifaddr *if_addrlist;
	struct	sockaddr if_addr;
	struct	ifqueue {
		struct	mbuf *ifq_head;
		struct	mbuf *ifq_tail;
		int	ifq_len;
		int	ifq_maxlen;
		int	ifq_drops;
	} if_snd;

	int	(*if_init)();
	int	(*if_output)();
	int	(*if_ioctl)();
	int	(*if_reset)();
	int	(*if_watchdog)();

	int	if_ipackets;
	int	if_ierrors;
	int	if_opackets;
	int	if_oerrors;
	int	if_collisions;

	int	d_affinity;
	char	if_version[IFVERLEN];
	struct	ifnet *if_next;
	int	if_type;
	int	(*if_start)();
	int	if_sysid_type;
};


struct ifaddr {
	struct	sockaddr ifa_addr;
	union {
		struct	sockaddr ifu_broadaddr;
		struct	sockaddr ifu_dstaddr;
	} ifa_ifu;
#define	ifa_broadaddr	ifa_ifu.ifu_broadaddr
#define	ifa_dstaddr	ifa_ifu.ifu_dstaddr
	struct	ifnet *ifa_ifp;
	struct	ifaddr *ifa_next;
};


#define	IFF_UP		0x1
#define	IFF_BROADCAST	0x2
#define	IFF_DEBUG	0x4
#define	IFF_LOOPBACK	0x8
#define	IFF_POINTOPOINT	0x10
#define	IFF_NOTRAILERS	0x20
#define	IFF_RUNNING	0x40
#define	IFF_NOARP	0x80
#define	IFF_CANTCHANGE	(IFF_BROADCAST | IFF_POINTOPOINT | IFF_RUNNING)
#define IFF_PROMISC	0x100
#define IFF_ALLMULTI	0x200
#define IFF_DYNPROTO	0x400
#define IFF_MOP		0x800
#define IFF_OACTIVE	0x1000
#define IFF_802HDR	0x2000
#define	IFF_PFCOPYALL	0x4000

#define IFT_OTHER	0x1
#define IFT_1822	0x2
#define IFT_HDH1822	0x3
#define IFT_X25DDN	0x4
#define IFT_X25		0x5
#define	IFT_ETHER	0x6
#define	IFT_ISO88023	0x7
#define	IFT_ISO88024	0x8
#define	IFT_ISO88025	0x9
#define	IFT_ISO88026	0xa
#define	IFT_STARLAN	0xb
#define	IFT_P10		0xc
#define	IFT_P80		0xd
#define IFT_HY		0xe
#define IFT_FDDI	0xf
#define IFT_LAPB	0x10
#define IFT_SDLC	0x11
#define IFT_T1		0x12
#define IFT_CEPT	0x13
#define IFT_ISDNBASIC	0x14
#define IFT_ISDNPRIMARY	0x15
#define IFT_PTPSERIAL	0x16
#define	IFT_LOOP	0x18
#define IFT_EON		0x19
#define	IFT_XETHER	0x1a
#define	IFT_NSIP	0x1b
#define	IFT_SLIP	0x1c

#define	IF_QFULL(ifq)		((ifq)->ifq_len >= (ifq)->ifq_maxlen)
#define	IF_DROP(ifq)		((ifq)->ifq_drops++)
#define	IF_ENQUEUE(ifq, m) { \
	(m)->m_act = 0; \
	if ((ifq)->ifq_tail == 0) \
		(ifq)->ifq_head = m; \
	else \
		(ifq)->ifq_tail->m_act = m; \
	(ifq)->ifq_tail = m; \
	(ifq)->ifq_len++; \
}
#define	IF_ENQUEUEIF(ifq, m, ifp) { \
	(m)->m_act = 0; \
	if ((ifq)->ifq_tail == 0) \
		(ifq)->ifq_head = m; \
	else \
		(ifq)->ifq_tail->m_act = m; \
	(ifq)->ifq_tail = m; \
	(ifq)->ifq_len++; \
	(m)->m_ifp = ifp; \
}
#define	IF_PREPEND(ifq, m) { \
	(m)->m_act = (ifq)->ifq_head; \
	if ((ifq)->ifq_tail == 0) \
		(ifq)->ifq_tail = (m); \
	(ifq)->ifq_head = (m); \
	(ifq)->ifq_len++; \
}

#ifdef BSD43
#define	IF_ADJ(m) { \
	(m)->m_off += sizeof(struct ifnet *); \
	(m)->m_len -= sizeof(struct ifnet *); \
	if ((m)->m_len == 0) { \
		struct mbuf *n; \
		MFREE((m), n); \
		(m) = n; \
	} \
}
#else
#define	IF_ADJ(m) { \
	if ((m)->m_len == 0) { \
		struct mbuf *n; \
		MFREE((m), n); \
		(m) = n; \
	} \
}
#endif
#define	IF_DEQUEUEIF(ifq, m, ifp) { \
	(m) = (ifq)->ifq_head; \
	if (m) { \
		if (((ifq)->ifq_head = (m)->m_act) == 0) \
			(ifq)->ifq_tail = 0; \
		(m)->m_act = 0; \
		(ifq)->ifq_len--; \
		(ifp) = (m)->m_ifp; \
	} \
}
#define	IF_DEQUEUE(ifq, m) { \
	(m) = (ifq)->ifq_head; \
	if (m) { \
		if (((ifq)->ifq_head = (m)->m_act) == 0) \
			(ifq)->ifq_tail = 0; \
		(m)->m_act = 0; \
		(ifq)->ifq_len--; \
	} \
}

#define	IFQ_MAXLEN	512
#define	IFNET_SLOWHZ	1

struct	ifreq {
#define	IFNAMSIZ	16
	char	ifr_name[IFNAMSIZ];
	union {
		struct	sockaddr ifru_addr;
		struct	sockaddr ifru_dstaddr;
		struct	sockaddr ifru_broadaddr;
		short	ifru_flags;
		int	ifru_metric;
		caddr_t	ifru_data;
	} ifr_ifru;
#define	ifr_addr	ifr_ifru.ifru_addr
#define	ifr_dstaddr	ifr_ifru.ifru_dstaddr
#define	ifr_broadaddr	ifr_ifru.ifru_broadaddr
#define	ifr_flags	ifr_ifru.ifru_flags
#define ifr_metric	ifr_ifru.ifru_metric
#define	ifr_data	ifr_ifru.ifru_data
};


struct ifdevea {
        char    ifr_name[IFNAMSIZ];
        u_char default_pa[6];
        u_char current_pa[6];
};


#define	IFMAXENTITY	5

struct ifstate {
	char    ifr_name[IFNAMSIZ];
	u_short	if_family;
	u_short	if_next_family;
	u_short	if_mode:3,
		if_ustate:1,
		if_nomuxhdr:1,
		if_dstate:4,
		if_xferctl:1,
		if_rdstate:1,
		if_wrstate:1,
		if_reserved:4;
	u_short if_dataportstate:3,
	        if_allocate:1,
	        if_deallocate:1,
	        if_start:1,
	        if_stop:1,
	        if_txabort:1,
	        if_setframe:1,
	        if_frametype:3,
	        if_errorchecktype:3,
		if_setrxbuffsize:1;
	struct protosw *if_pr;
	int     if_rxbuffsize;
	int     if_lec[IFMAXENTITY];
	u_char  *if_lei;
	u_short if_leisiz;
	caddr_t if_clientname;
	caddr_t if_clientref;
};

#define	IFS_USROFF	0x0
#define	IFS_USRON	0x1
#define	IFS_DDCMPFDX	0x0
#define	IFS_MOP		0x1
#define	IFS_DDCMPHDXP	0x2
#define	IFS_DDCMPHDXS	0x3
#define IFS_NOMUXHDR	0x1
#define IFS_MUXHDR	0x0
#define IFS_HALTED	0x0
#define IFS_STARTING	0x1
#define IFS_RUNNING	0x3
#define IFS_HALTING	0x4
#define IFS_OWNREQ	0x5
#define IFS_OWNREL	0x6
#define IFS_ENTEREDMOP	0x7
#define IFS_XFERCTL	0x1
#define IFS_RDSTATE	0x1
#define IFS_WRSTATE	0x1

#define IFS_OPEN		0x0
#define IFS_OPENDISABLED	0x1
#define IFS_CALLATTACHED	0x2
#define IFS_CLOSEPENDING	0x3

#define IFS_ALLOCATE		0x1
#define IFS_DEALLOCATE		0x1
#define IFS_STARTPORT		0x1
#define IFS_STOPPORT		0x1
#define IFS_TXABORT		0x1
#define IFS_SETFRAME		0x1
#define IFS_SETRXBUFFSIZE	0x1

#define IFS_HDLC		0x0
#define IFS_SDLC		0x1
#define IFS_DDCMP		0x2
#define IFS_BISYNC		0x3
#define IFS_GENBYTE		0x4

#define IFS_CRC_CCITT_1		0x0
#define IFS_CRC_CCITT_0		0x1
#define IFS_LRC_VRC_ODD		0x2
#define IFS_CRC_16		0x3
#define IFS_VRC_ODD		0x4
#define IFS_VRC_EVEN		0x5
#define IFS_LRC_VRC_EVEN	0x6
#define IFS_CRC_NONE		0x7

struct ifdata {
	u_char data[128];
};

struct	ifconf {
	int	ifc_len;
	union {
		caddr_t	ifcu_buf;
		struct	ifreq *ifcu_req;
	} ifc_ifcu;
#define	ifc_buf	ifc_ifcu.ifcu_buf
#define	ifc_req	ifc_ifcu.ifcu_req
};

struct ifeeprom {
	char	ife_name[IFNAMSIZ];
	u_char	ife_data[64];
	u_long	ife_offset;
	u_long	ife_blklen;
	u_long	ife_lastblk;
};
#define IFE_NOTLAST     0x0
#define IFE_LASTBLOCK   0x1

#define IFE_SUCCESS     0x0
#define IFE_RETRY       0x1
#define IFE_FAIL        0x2

struct estat {
	u_short	est_seconds;
	u_int	est_bytercvd;
	u_int	est_bytesent;
	u_int	est_blokrcvd;
	u_int	est_bloksent;
	u_int	est_mbytercvd;
	u_int	est_mblokrcvd;
	u_int	est_deferred;
	u_int	est_single;
	u_int	est_multiple;
	u_short	est_sendfail_bm;

	u_short	est_sendfail;
	u_short	est_collis;
	u_short	est_recvfail_bm;

	u_short	est_recvfail;
	u_short	est_unrecog;
	u_short	est_overrun;
	u_short	est_sysbuf;
	u_short	est_userbuf;
	u_int	est_mbytesent;
	u_int	est_mbloksent;
};

struct dstat {
	u_short	dst_seconds;
	u_int	dst_bytercvd;
	u_int	dst_bytesent;
	u_int	dst_blockrcvd;
	u_int	dst_blocksent;
	u_short	dst_inbound_bm;

	u_char	dst_inbound;
	u_short	dst_outbound_bm;

	u_char	dst_outbound;
	u_char	dst_remotetmo;
	u_char	dst_localtmo;
	u_short	dst_remotebuf_bm;

	u_char	dst_remotebuf;
	u_short	dst_localbuf_bm;

	u_char	dst_localbuf;
	u_char	dst_select;
	u_short	dst_selecttmo_bm;

	u_char	dst_selecttmo;
	u_short	dst_remotesta_bm;

	u_char	dst_remotesta;
	u_short	dst_localsta_bm;

	u_char	dst_localsta;
};


struct fstat {
        u_short fst_second;
	u_int	fst_frame;
	u_int	fst_error;
	u_int 	fst_lost;
        u_int   fst_bytercvd;
        u_int   fst_bytesent;
        u_int   fst_pdurcvd;
        u_int   fst_pdusent;
        u_int   fst_mbytercvd;
	u_int   fst_mpdurcvd;
        u_int   fst_mbytesent;
        u_int   fst_mpdusent;
	u_short	fst_underrun;
        u_short fst_sendfail;
        u_short	fst_fcserror;
	u_short	fst_fseerror;
	u_short	fst_pdualig;
	u_short	fst_pdulen;
	u_short	fst_pduunrecog;
	u_short fst_mpduunrecog;
        u_short fst_overrun;
        u_short fst_sysbuf;
        u_short fst_userbuf;
	u_short fst_ringinit;
	u_short fst_ringinitrcv;
	u_short fst_ringbeacon;
	u_short fst_duptoken;
	u_short fst_dupaddfail;
	u_short	fst_ringpurge;
	u_short fst_bridgestrip;
	u_short fst_traceinit;
	u_short fst_tracerecv;
	u_short fst_lem_rej;
	u_short fst_lem_events;
	u_short fst_lct_rej;
	u_short fst_tne_exp_rej;
	u_short fst_connection;
	u_short fst_ebf_error;
};


struct ctrreq {
	char	ctr_name[IFNAMSIZ];
	char	ctr_type;
	union {
		struct estat ctrc_ether;
		struct dstat ctrc_ddcmp;
		struct fstat ctrc_fddi;
#if 0
		struct fstatus status_fddi;
		struct fddismt smt_fddi;
		struct fddimac mac_fddi;
		struct fddipath path_fddi;
		struct fddiport port_fddi;
		struct fddiatta atta_fddi;
#endif
	} ctr_ctrs;
};

#define CTR_ETHER 0
#define CTR_DDCMP 1
#define CTR_FDDI 2
#define FDDIMIB_SMT	3
#define FDDIMIB_MAC	4
#define FDDIMIB_PATH	5
#define FDDIMIB_PORT	6
#define FDDIMIB_ATTA	7
#define FDDI_STATUS	8
#define ctr_ether ctr_ctrs.ctrc_ether
#define ctr_ddcmp ctr_ctrs.ctrc_ddcmp
#define ctr_fddi  ctr_ctrs.ctrc_fddi
#define sts_fddi  ctr_ctrs.status_fddi
#define fmib_smt  ctr_ctrs.smt_fddi
#define fmib_mac  ctr_ctrs.mac_fddi
#define fmib_path  ctr_ctrs.path_fddi
#define fmib_port  ctr_ctrs.port_fddi
#define fmib_atta  ctr_ctrs.atta_fddi

#define CTR_HDRCRC	0
#define CTR_DATCRC	1
#define CTR_BUFUNAVAIL	0

#include <net/if_arp.h>
