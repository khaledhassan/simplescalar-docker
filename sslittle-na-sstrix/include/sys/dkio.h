#define NO_ACCESS -13

struct	dkop	{
	short	dk_op;
	daddr_t dk_count;
};

struct	dkget	{
	short	dk_type;
	short	dk_dsreg;
	short	dk_erreg;
	short	dk_resid;
};

struct	dkacc	{
	short	dk_opcode;
	long	dk_lbn;
	long	dk_length;
	unsigned dk_status;
	unsigned dk_flags;
};
