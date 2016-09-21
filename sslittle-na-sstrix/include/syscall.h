#ifdef __mips
#define SYS_syscall 0
#endif /* __mips */
#define SYS_exit 1
#define SYS_fork 2
#define SYS_read 3
#define SYS_write 4
#define SYS_open 5
#define SYS_close 6
#define SYS_creat 8
#define SYS_link 9
#define SYS_unlink 10
#define SYS_execv 11
#define SYS_chdir 12
#define SYS_mknod 14
#define SYS_chmod 15
#define SYS_chown 16
#ifdef __mips
#define SYS_brk 17			/* 17 is old: sbreak */
#else
#endif /* __mips */
#define SYS_lseek 19
#define SYS_getpid 20
#define SYS_mount 21
#define SYS_umount 22
#define SYS_getuid 24
#define SYS_ptrace 26
#define SYS_access 33
#define SYS_sync 36
#define SYS_kill 37
#define SYS_stat 38
#define SYS_lstat 40
#define SYS_dup 41
#define SYS_pipe 42
#define SYS_profil 44
#define SYS_getgid 47
#define SYS_acct 51
#define SYS_ioctl 54
#define SYS_reboot 55
#define SYS_symlink 57
#define SYS_readlink 58
#define SYS_execve 59
#define SYS_umask 60
#define SYS_chroot 61
#define SYS_fstat 62
#define SYS_getpagesize 64
#define SYS_mremap 65
#ifdef __mips
#define SYS_vfork 66			/* 66 is old: vfork */
#else /* !mips */
#endif /* __mips */
#define SYS_sbrk 69
#define SYS_sstk 70
#define SYS_mmap 71
#ifdef __mips
#define SYS_vadvise 72			/* 72 is old: vadvise */
#else /* !mips */
#endif /* __mips */
#define SYS_munmap 73
#define SYS_mprotect 74
#define SYS_madvise 75
#define SYS_vhangup 76
#define SYS_mincore 78
#define SYS_getgroups 79
#define SYS_setgroups 80
#define SYS_getpgrp 81
#define SYS_setpgrp 82
#define SYS_setitimer 83
#define SYS_wait3 84
#define SYS_wait SYS_wait3
#define SYS_swapon 85
#define SYS_getitimer 86
#define SYS_gethostname 87
#define SYS_sethostname 88
#define SYS_getdtablesize 89
#define SYS_dup2 90
#define SYS_getdopt 91
#define SYS_fcntl 92
#define SYS_select 93
#define SYS_setdopt 94
#define SYS_fsync 95
#define SYS_setpriority 96
#define SYS_socket 97
#define SYS_connect 98
#define SYS_accept 99
#define SYS_getpriority 100
#define SYS_send 101
#define SYS_recv 102
#ifdef __mips
#define SYS_sigreturn 103			/* new sigreturn */
#else /* !mips */
#endif /* __mips */
#define SYS_bind 104
#define SYS_setsockopt 105
#define SYS_listen 106
#define SYS_sigvec 108
#define SYS_sigblock 109
#define SYS_sigsetmask 110
#define SYS_sigpause 111
#define SYS_sigstack 112
#define SYS_recvmsg 113
#define SYS_sendmsg 114
#define SYS_gettimeofday 116
#define SYS_getrusage 117
#define SYS_getsockopt 118
#define SYS_readv 120
#define SYS_writev 121
#define SYS_settimeofday 122
#define SYS_fchown 123
#define SYS_fchmod 124
#define SYS_recvfrom 125
#define SYS_setreuid 126
#define SYS_setregid 127
#define SYS_rename 128
#define SYS_truncate 129
#define SYS_ftruncate 130
#define SYS_flock 131
#define SYS_sendto 133
#define SYS_shutdown 134
#define SYS_socketpair 135
#define SYS_mkdir 136
#define SYS_rmdir 137
#define SYS_utimes 138
#ifdef __mips
#define SYS_sigcleanup 139     /* from 4.2 longjmp; same as SYS_sigreturn */
#else /* !mips */
#endif /* __mips */
#define SYS_adjtime 140
#define SYS_getpeername 141
#define SYS_gethostid 142
#define SYS_sethostid 143
#define SYS_getrlimit 144
#define SYS_setrlimit 145
#define SYS_killpg 146
#define SYS_setquota 148
#define SYS_quota 149
#define SYS_getsockname 150
#ifdef __mips
#define SYS_sysmips 151	/* for floating point control */
#define SYS_cacheflush 152
#define SYS_cachectl 153
#define SYS_atomic_op 155
#ifdef notdef		/* __ultrix may or may not do these */
#define SYS_debug 154
#define SYS_statfs 160
#define SYS_fstatfs 161
#define SYS_unmount 162
#define SYS_quotactl 168
#define SYS_mount 170
#define SYS_hdwconf 171
#endif /* notdef */
#define SYS_nfs_svc 158
#define SYS_nfssvc 158 /* cruft - delete when kernel fixed */
#define SYS_nfs_biod 163
#define SYS_async_daemon 163 /* cruft - delete when kernel fixed */
#define SYS_nfs_getfh 164
#define SYS_getfh 164 /* cruft - delete when kernel fixed */
#define SYS_getdirentries 159
#define SYS_getdomainname 165
#define SYS_setdomainname 166
#define SYS_exportfs 169
#else /* vax */
#define SYS_msgctl 151
#define SYS_msgget 152
#define SYS_msgrcv 153
#define SYS_msgsnd 154
#define SYS_semctl 155
#define SYS_semget 156
#define SYS_semop 157
#define SYS_uname 158
#define SYS_shmsys 159
#define SYS_plock 160
#define SYS_lockf 161
#define SYS_ustat 162
#define SYS_getmnt 163
#define SYS_getdirentries 164
#define SYS_nfs_biod 165	 /* 165 -- reserved for ultrix */
#define SYS_nfs_getfh 166      /* 166 -- reserved for ultrix */
#define SYS_nfs_svc 167      /* 167 -- reserved for ultrix */
#define SYS_exportfs 168	 /* 168 -- reserved for ultrix */
#define SYS_getdomainname 169	 /* 169 -- reserved for ultrix */
#define SYS_setdomainname 170	 /* 170 -- reserved for ultrix */
#define SYS_sigpending 171	 /* 171 -- reserved for ultrix */
#define SYS_setsid 172
#define SYS_waitpid 173
#endif /* __vax */
#ifdef __mips
#define SYS_msgctl 172
#define SYS_msgget 173
#define SYS_msgrcv 174
#define SYS_msgsnd 175
#define SYS_semctl 176
#define SYS_semget 177
#define SYS_semop 178
#define SYS_uname 179
#define SYS_shmsys 180
#define SYS_plock 181
#define SYS_lockf 182
#define SYS_ustat 183
#define SYS_getmnt 184
#ifdef notdef
#define SYS_mount 185
#define SYS_umount 186
#endif /* notdef */
#define SYS_sigpending 187
#define SYS_setsid 188
#define SYS_waitpid 189
#endif /* __mips */
#define SYS_utc_gettime 233	 /* 233 -- same as osf/1 */
#define SYS_utc_adjtime 234	 /* 234 -- same as osf/1 */
#define SYS_audcntl 252
#define SYS_audgen 253
#define SYS_startcpu 254	 /* 254 -- ultrix private */
#define SYS_stopcpu 255	 /* 255 -- ultrix private */
#define SYS_getsysinfo 256	 /* 256 -- ultrix private */
#define SYS_setsysinfo 257	 /* 257 -- ultrix private */
