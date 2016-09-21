struct	arphdr {
	u_short	ar_hrd;
#define ARPHRD_ETHER 	1
	u_short	ar_pro;
	u_char	ar_hln;
	u_char	ar_pln;
	u_short	ar_op;
#define	ARPOP_REQUEST	1
#define	ARPOP_REPLY	2
#define RARPOP_REQUEST  3
#define RARPOP_REPLY    4
};

struct arpreq {
	struct	sockaddr arp_pa;
	struct	sockaddr arp_ha;
	int	arp_flags;
};

#define	ATF_INUSE	0x01
#define ATF_COM		0x02
#define	ATF_PERM	0x04
#define	ATF_PUBL	0x08
#define	ATF_USETRAILERS	0x10

#define BSD_TRAILERS	1
