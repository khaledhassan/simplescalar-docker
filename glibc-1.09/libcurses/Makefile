
subdir	:= libcurses

headers	:= curses.h

#defines := $(defines) -D_POSIX_SOURCE -DNON_UNIX_STDIO

routines := addbytes addch addnstr box clear clrtobot clrtoeol cr_put \
	    ctrace cur_hash curses delch deleteln delwin erase fullname \
	    getch getstr id_subwins idlok initscr insch insertln longname \
	    move mvwin newwin overlay overwrite pchar printw refresh \
	    scanw scroll setterm standout toucholap touchwin tscroll tstp \
	    tty unctrl

#tests := ex1 ex2 life twinkle1 twinkle2

extra-objs := libcurses.a

install-lib := libcurses.a

# libcurses's code is not too clean.
override +gccwarn := -w

include ../Rules

$(objpfx)libcurses.a: $(dep-dummy-lib); $(make-dummy-lib)
lib: $(objpfx)libcurses.a

