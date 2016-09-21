struct route {
	struct	rtentry *ro_rt;
	struct	sockaddr ro_dst;
};

struct rtentry {
	u_long	rt_hash;
	struct	sockaddr rt_dst;
	struct	sockaddr rt_gateway;
	short	rt_flags;
	short	rt_refcnt;
	u_long	rt_use;
	struct	ifnet *rt_ifp;
	struct rtentry *rt_next;
};

#define	RTF_UP		0x1
#define	RTF_GATEWAY	0x2
#define	RTF_HOST	0x4
#define RTF_DYNAMIC	0x10
#define RTF_MODIFIED	0x20

struct	rtstat {
	short	rts_badredirect;
	short	rts_dynamic;
	short	rts_newgateway;
	short	rts_unreach;
	short	rts_wildcard;
};

