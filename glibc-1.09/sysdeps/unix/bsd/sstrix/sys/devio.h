#ifndef	DEVIO_INCLUDE
#define	DEVIO_INCLUDE	1

#define DEV_SIZE	0x08

#define DEV_UGH(x,y,z)	uprintf("%s: unit# %d: %s\n",x,y,z)

struct	devget	{
	short	category;
	short	bus;
	char	interface[DEV_SIZE];
	char	device[DEV_SIZE];
	short	adpt_num;
	short	nexus_num;
	short	bus_num;
	short	ctlr_num;
	short	rctlr_num;
	short	slave_num;
	char	dev_name[DEV_SIZE];
	short	unit_num;
	unsigned soft_count;
	unsigned hard_count;
	long	stat;
	long	category_stat;
};

#define DEV_TAPE	0x00
#define DEV_DISK	0x01
#define DEV_TERMINAL	0x02
#define DEV_PRINTER	0x03
#define DEV_SPECIAL	0x04

#define DEV_UB		0x00
#define DEV_QB		0x01
#define DEV_MB		0x02
#define DEV_BI		0x03
#define DEV_CI		0x04
#define DEV_NB		0x05
#define DEV_MSI		0x06
#define DEV_SCSI        0x07
#define DEV_XMI		0x08
#define DEV_BICI	0x09
#define DEV_XMICI	0x0A
#define	DEV_UNKBUS	0xff

#define DEV_UNKNOWN	"UNKNOWN"

#define DEV_AIO 	"AIO"
#define DEV_CXAB16	"CXAB16"
#define DEV_DEBNT	"DEBNT"
#define DEV_DHB32	"DHB32"
#define DEV_DHQVCXY	"DHQVCXY"
#define DEV_DHU11	"DHU11"
#define DEV_DMB32	"DMB32"
#define DEV_DMF32	"DMF32"
#define DEV_DMZ32	"DMZ32"
#define DEV_DSSC	"DSSC"
#define DEV_DZ11	"DZ11"
#define DEV_DZ32	"DZ32"
#define DEV_DZQ11	"DZQ11"
#define DEV_DZV11	"DZV11"
#define DEV_HSC40	"HSC40"
#define DEV_HSC50	"HSC50"
#define DEV_HSC60	"HSC60"
#define DEV_HSC70	"HSC70"
#define DEV_HSC90	"HSC90"
#define DEV_IDC 	"IDC"
#define DEV_KDA50	"KDA50"
#define DEV_KDB50	"KDB50"
#define DEV_KDM70	"KDM70"
#define DEV_KFBTA	"KFBTA"
#define DEV_KFQSA	"KFQSA"
#define DEV_KLESI	"KLESI"
#define DEV_KRQ50	"KRQ50"
#define DEV_KRU50	"KRU50"
#define DEV_KSB50	"KSB50"
#define DEV_LAT 	"LAT"
#define DEV_MF_SLU      "MF_SLU"
#define DEV_RH		"RH"
#define DEV_RK711	"RK711"
#define DEV_RLU211	"RLU211"
#define DEV_RLV211	"RLV211"
#define DEV_RQDX1	"RQDX1"
#define DEV_RQDX2	"RQDX2"
#define DEV_RQDX3	"RQDX3"
#define DEV_RQDX4	"RQDX4"
#define DEV_RRD40	"RRD40"
#define DEV_RRD42	"RRD42"
#define DEV_RRD50	"RRD50"
#define DEV_RUX50	"RUX50"
#define DEV_SCSI_GEN    "SCSI"
#define DEV_TBK70	"TBK70"
#define DEV_TBL70	"TBK7L"
#define DEV_TM03	"TM03"
#define DEV_TM32	"TM32"
#define DEV_TM78	"TM78"
#define DEV_TQK50	"TQK50"
#define DEV_TQK70	"TQK70"
#define DEV_TQL70	"TQK7L"
#define DEV_TSU05	"TSU05"
#define DEV_TSU11	"TSU11"
#define DEV_TSV05	"TSV05"
#define DEV_TUK50	"TUK50"
#define DEV_TUK70	"TUK70"
#define DEV_TUU80	"TU80"
#define DEV_UDA50	"UDA50"
#define DEV_UDA50A	"UDA50A"
#define DEV_VCB01	"VCB01"
#define DEV_VCB02	"VCB02"

#define DEV_VS_SLU	"VS_SLU"
#define DEV_FF_SLU	"FF_SLU"
#define DEV_VS_DISK	"VS_DISK"
#define DEV_VS_TAPE	"VS_TAPE"
#define DEV_VS_NI	"VS_NI"
#define	DEV_VS_SCSI	"VS_SCSI"

#define DEV_TM_SLE	"TM_SLE"

#define DEV_ESE20	"ESE20"
#define DEV_ESE25       "ESE25"
#define DEV_MOUSE	"VSXXXAA"
#define DEV_R80 	"R80"
#define DEV_RA60	"RA60"
#define DEV_RA70	"RA70"
#define DEV_RA71	"RA71"
#define DEV_RA72	"RA72"
#define DEV_RA80	"RA80"
#define DEV_RA81	"RA81"
#define DEV_RA82	"RA82"
#define DEV_RA90	"RA90"
#define DEV_RA92	"RA92"
#define DEV_RAMDISK	"RAMDISK"
#define DEV_RC25	"RC25"
#define DEV_RC25F	"RC25F"
#define DEV_RD31	"RD31"
#define DEV_RD32	"RD32"
#define DEV_RD33	"RD33"
#define DEV_RD51	"RD51"
#define DEV_RD52	"RD52"
#define DEV_RD53	"RD53"
#define DEV_RD54	"RD54"
#define DEV_RF30	"RF30"
#define DEV_RF31	"RF31"
#define DEV_RFH31	"RFH31"
#define DEV_RF71	"RF71"
#define DEV_RF72	"RF72"
#define DEV_RFH72	"RFH72"
#define DEV_RF73	"RF73"
#define DEV_RFH73	"RFH73"
#define DEV_RK07	"RK07"
#define DEV_RL02	"RL02"
#define DEV_RM03	"RM03"
#define DEV_RM05	"RM05"
#define DEV_RM80	"RM80"
#define DEV_RP05	"RP05"
#define DEV_RP06	"RP06"
#define DEV_RP07	"RP07"
#define DEV_RV20	"RV20"
#define DEV_RV60	"RV60"
#define DEV_RX18	"RX18"
#define DEV_RX23	"RX23"
#define DEV_RX26	"RX26"
#define DEV_RZ23L	"RZ23L"
#define DEV_RX33	"RX33"
#define DEV_RX35	"RX35"
#define DEV_RX50	"RX50"
#define DEV_RZ22	"RZ22"
#define DEV_RZ23	"RZ23"
#define DEV_RZ24        "RZ24"
#define DEV_RZ25	"RZ25"
#define DEV_RZ55	"RZ55"
#define DEV_RZ56	"RZ56"
#define DEV_RZ57        "RZ57"
#define	DEV_RZxx	"RZxx"
#define DEV_SVS00	"SVS00"
#define DEV_TA78	"TA78/9"
#define DEV_TA79	"TA79"
#define DEV_TA81	"TA81"
#define DEV_TA90	"TA90"
#define DEV_TA91	"TA91"
#define DEV_TABLET	"VSXXXAB"
#define DEV_TE16	"TE16"
#define DEV_TF30	"TF30"
#define DEV_TF70	"TF70"
#define DEV_TF70L	"TF70L"
#define DEV_TF85	"TF85"
#define DEV_TK50	"TK50"
#define DEV_TK70	"TK70"
#define DEV_TRACE	"TRACE"
#define DEV_TS05	"TS05"
#define DEV_TS11	"TS11"
#define DEV_TU45	"TU45"
#define DEV_TU77	"TU77"
#define DEV_TU78	"TU78/9"
#define DEV_TU80	"TU80"
#define DEV_TU81	"TU81"
#define DEV_TU81E	"TU81E"
#define DEV_TLZ04       "TLZ04"
#define DEV_TZ05	"TZ05"
#define DEV_TZ07	"TZ07"
#define DEV_TZK08	"TZK08"
#define DEV_TZK10	"TZK10"
#define DEV_TZ30	"TZ30"
#define	DEV_TZxx	"TZxx"
#define DEV_TZQIC	"TZQIC"
#define DEV_TZ9TRK	"TZ9TRK"
#define DEV_TZ8MM	"TZ8MM"
#define DEV_TZRDAT	"TZRDAT"
#define DEV_VR100	"VR100"
#define DEV_VR260	"VR260"
#define DEV_VR290	"VR290"
#define DEV_VT100	"VT100"
#define DEV_VT101	"VT101"
#define DEV_VT102	"VT102"
#define DEV_VT125	"VT125"
#define DEV_VT220	"VT220"
#define DEV_VT240	"VT240"
#define DEV_VT241	"VT241"
#define DEV_VT320	"VT320"
#define DEV_VT330	"VT330"
#define DEV_VT340	"VT340"
#define DEV_XOS 	"XOS"
#define DEV_ECRM	"Buffer"

#define DEV_BOM 	0x01
#define DEV_EOM 	0x02
#define DEV_OFFLINE	0x04
#define DEV_WRTLCK	0x08
#define DEV_BLANK	0x10
#define DEV_WRITTEN	0x20
#define DEV_CSE 	0x40
#define DEV_SOFTERR	0x80
#define DEV_HARDERR	0x100
#define DEV_DONE	0x200
#define DEV_RETRY	0x400
#define DEV_ERASED	0x800

#define DEV_TPMARK	0x01
#define DEV_SHRTREC	0x02
#define DEV_RDOPP	0x04
#define DEV_RWDING	0x08
#define DEV_800BPI	0x10
#define DEV_1600BPI	0x20
#define DEV_6250BPI	0x40
#define DEV_6666BPI	0x80
#define DEV_10240BPI	0x100
#define DEV_38000BPI	0x200
#define DEV_LOADER	0x400
#define DEV_38000_CP	0x800
#define DEV_76000BPI	0x1000
#define DEV_76000_CP	0x2000

#define DEV_8000_BPI	0x4000
#define DEV_10000_BPI   0x8000
#define DEV_16000_BPI	0x10000

#define DEV_61000_BPI	0x20000
#define DEV_54000_BPI	0x40000

#define DEV_DISKPART	minor(dev)%0x08
#define	DEV_DPMASK	0x07
#define	DEV_MC_COUNT	0x08

#define	DEV_3_HD2S	0x10
#define	DEV_3_DD2S	0x20
#define	DEV_5_HD2S	0x30
#define	DEV_5_DD1S	0x40
#define	DEV_5_LD2S	0x50
#define	DEV_5_DD2S	0x60
#define	DEV_3_ED2S	0x70
#define	DEV_X_XXXX	0xf0
#define	DEV_DDMASK	0xf0

#define DEV_MODEM	0x01
#define DEV_MODEM_ON	0x02

typedef union devgeom {
    struct {
	unsigned long	dev_size;
	unsigned short	ntracks;
	unsigned short	nsectors;
	unsigned short	ncylinders;
	unsigned long   attributes;
    } geom_info;
    unsigned char	pad[124];
} DEVGEOMST;

#define DEVGEOM_REMOVE  0x01

#define	CONSOLE_DTYPE	0
#define	QVSS_DTYPE	1
#define	QDSS_DTYPE	2
#define	SS_DTYPE	3
#define	SM_DTYPE	4
#define	SG_DTYPE        5
#define	LYNX_DTYPE	6
#define	FC_DTYPE	7
#define	FG_DTYPE	8
#define	PMM_DTYPE	9
#define	PMC_DTYPE	10
#define	CFB_DTYPE	11
#define	GA_DTYPE	12
#define	GQ_DTYPE	13
#define	WS_DTYPE	14

#endif /*	DEVIO_INCLUDE */
