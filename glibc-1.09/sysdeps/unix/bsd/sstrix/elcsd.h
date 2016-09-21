#ifndef __elcsd__
#define __elcsd__

#define MODE		0644
#define RHTSIZE		256
#define MAX_PATH	255
#define MAX_NLEN	32
#define WTSIZE  	10
#define OFTSIZE  	10
#define OFTINDX  	1
#define FSYNC		1
#define NOFSYNC		0
#define ALERT		1
#define NONALERT	0
#define BSIZE		512
#define LOCLOG		0x0001
#define LOGREM		0x0002
#define REMLOG		0x0004
#define PRILOG		0x0005
#define ELSTRTRLOG 	1
#define ELGOAHEAD 	2
#define ELSHUTDOWN 	3
#define ELSTARTUP 	4
#define ELNOGO	 	-1
#define ELWNDALL 	-1
#define ELOPEN_WNDW 	1
#define ELCLOSE_WNDW 	2
#define ELSHTDWN_WNDW 	3
#define ELDISABLE 	3
#define ELINITIALIZE 	4
#define ELRECONFIG 	5
#define ELLOCK	 	6
#define FSPERCENT	2

char el[] = "/syserr.";
char remotes[] = "remotes";
char cfgfile[] = "/etc/elcsd.conf";
char logpath[] = "/usr/adm/elcsdlog";
char elcsckt[] = "/dev/elcscntlsckt";
char elisckt[] = "/dev/elicntlsckt";

struct elcfg {
	short status;
	short limfsize;
	char elpath[MAX_PATH];
	char bupath[MAX_PATH];
	char supath[MAX_PATH];
	char rhpath[MAX_PATH];
	char rhostn[MAX_NLEN];
};
struct elcfg elcfg;

struct elrht {
	char hname[MAX_NLEN];
	char where;
};
struct elrht elrht[RHTSIZE];
struct elrht *chkrht();

struct eloft {
	int fdes;
	char hname[MAX_NLEN];
	char fname[MAX_PATH];
	long marker;
};
struct eloft eloft[OFTSIZE] = { 0 };
struct eloft *eloftp, *gofts(),*goftp();

struct wndpkt {
	short msgtyp;
	short class;
	short type;
	short ctldevtyp;
	short num;
	short unitnum;
};
struct wndmsg {
	struct wndpkt wndpkt;
	int sfalen;
	struct sockaddr_un sfa;
};
struct wndwt {
	short class;
	short type;
	short ctldevtyp;
	short num;
	short unitnum;
	int wsalen;
	struct sockaddr_un wsa;
};
struct wndwt wndwt[WTSIZE];

struct elparam {
	long pid;
	long sid;
};

char *elcsmode[] = {
	"multi-user mode",
	"single user mode",
	"windowing only mode, no logging to disk!",
	0
};

char *elcsmsg[] = {
"\0",
"exiting, open error on",
"exiting, ioctl error getting errlog pid",
"startup in",
"exiting, ioctl error setting errlog pid",
"exiting, error gethostname:",
"select error",
"write pipe error",
"exiting, failure to log local errors to any errlog file",
"ioctl error moving kernel errlog output pointer",
"remote host shutdown",
"stream socket accept error",
"error forking wndw process",
"datagram socket sendto error",
"failure to log local priority errors to any errlog file",
"stream socket send error",
"gethostbyaddr error",
"stream socket recv error",
"unknown host check /etc/hosts file for entry:",
"logging remote systems too!",
"read pipe error",
"unix domain socket sendto error",
"no errlog path for",
"exiting, no hostname - hostname not set!",
"ioctl error reinitializing kernel errlog pointers",
"ioctl error clearing errlog pid",
"shutdown",
"unix domain bind error on path /dev/elcscntlsckt",
"unix domain socket error",
"unknown service elcsd/udp, check /etc/services file for entry",
"unknown host check /etc/hosts file for entry:",
"datagram socket bind error",
"datagram socket error",
"stream socket listen error",
"stream socket bind error",
"stream socket error",
"exiting, can't open/setup main or backup errlog file",
"can't open/setup main errlog file, opened backup errlog file!",
"open/setup single user mode errlog file",
"error coping sinlge user errlog file to main errlog file",
"error coping backup errlog file to main errlog file",
"can't log remote system errors!",
"bytes remaining in errlog file, until limit size is exceeded",
"exceeded errlog file limit size, error not logged to disk!",
"logging remotely to",
"stream socket connect error",
"can't log remotely!",
"open failure on file",
"read error",
"write error",
"error can't open/setup main errlog file",
"error can't open/setup backup errlog file",
"exiting, error can't open/setup single user errlog file",
"error writing to errlog file",
"error can't open/setup errlog file for host:",
"logging locally to",
"exiting, can't open elcsd.conf file",
"exiting, elcsd.conf needed info. or commented out",
"reinitialized kernel errlog buffer pointers",
"reconfigured per elcsd.conf",
"host address not in /etc/hosts", 
"elcsd.conf not set up right, missing or removed delimiter or not 7 entries between delimiters!", 
"error on saved kernel errlog buffer file from dump:", 
"remote host startup", 
"single user file corrupted",
"system name not defined in remote host table",
"exiting, can't move kernel errlog output pointer",
"file system almost full, CAN'T log errors to",
"reset errlog pid in kernel",
"exiting, can't malloc space for file system data",
"can't get system date",
	0
};
#endif /* __elcsd__ */
