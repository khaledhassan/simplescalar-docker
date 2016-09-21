# Copyright (C) 1991, 1992, 1993, 1994 Free Software Foundation, Inc.
# This file is part of the GNU C Library.

# The GNU C Library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Library General Public License as
# published by the Free Software Foundation; either version 2 of the
# License, or (at your option) any later version.

# The GNU C Library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Library General Public License for more details.

# You should have received a copy of the GNU Library General Public
# License along with the GNU C Library; see the file COPYING.LIB.  If
# not, write to the Free Software Foundation, Inc., 675 Mass Ave,
# Cambridge, MA 02139, USA.

#
#	Makefile for malloc routines
#
subdir	:= malloc

all: 

dist-headers := malloc.h
headers := $(dist-headers) obstack.h
tests := mallocbug

# Things which get pasted together into gmalloc.c.  It is important that
# valloc is first, so that getpagesize.h is included before other things.
gmalloc-routines := valloc malloc free cfree realloc calloc morecore memalign 
# Things to include in the standalone distribution.
dist-routines = $(gmalloc-routines) \
		mcheck mtrace mstats vm-limit ralloc
routines = $(dist-routines) obstack

# Frob these guys' copying notices.
gpl2lgpl := obstack.c obstack.h vm-limit.c mem-limits.h ralloc.c

install-lib := libmcheck.a
non-lib.a := libmcheck.a

distribute := mcheck-init.c OChangeLog TODO \
	      malloc/gmalloc-head.c dist-README dist-Makefile \
	      mtrace.awk mem-limits.h getpagesize.h

# These should be removed by `make clean'.
extra-objs = mcheck-init.o libmcheck.a


# Make the standalone malloc distribution.
malloc-dist := README COPYING.LIB Makefile ChangeLog OChangeLog \
	       $(addsuffix .c,$(dist-routines)) $(dist-headers) \
	       getpagesize.h mem-limits.h gmalloc.c \
	       gmalloc-head.c mtrace.awk
%.uu: %
	uuencode $< < $< > $@-tmp
	mv -f $@-tmp $@
%.Z: %
	compress -c $< > $@-tmp
	mv -f $@-tmp $@
%.gz: %
	gzip -9 -v -c $< > $@-tmp
	mv -f $@-tmp $@

malloc.tar: $(addprefix malloc/,$(malloc-dist))
	tar cho$(verbose)f $@ $^

malloc/ChangeLog: ../ChangeLog
	changelog-extract --regexp 'malloc/.*' < $< > $@.new
	(echo ''; echo 'Find older changes in OChangeLog.') >> $@.new
	chmod a-w $@.new
	mv -f $@.new $@

malloc/Makefile: dist-Makefile Makefile
	sed -e 's,<GMALLOC-SOURCES>,$(addsuffix .c,$(gmalloc-routines)),' \
	    -e 's,<DIST-SOURCES>,$(addsuffix .c,$(dist-routines)),' \
	    -e 's,<DIST-OBJECTS>,$(addsuffix .o,$(dist-routines)),' \
	    -e 's,<DIST-HEADERS>,$(dist-headers),' < $< > $@-tmp
	mv -f $@-tmp $@
# Make it unwritable to avoid accidentally changing the file,
# since it is generated and any changes would be lost.
	chmod a-w $@
malloc/gmalloc.c: malloc/Makefile \
		  $(addprefix malloc/,$(headers) \
			      	      $(addsuffix .c,$(dist-routines)))
	$(MAKE) -C malloc gmalloc.c
malloc/README: dist-README
	cp -f $< $@
malloc/%: ../sysdeps/generic/%
	cp -f $< $@
malloc/%: %
	cp -f $< $@

include ../Rules

$(objpfx)libmcheck.a: $(objpfx)mcheck-init.o
	-rm -f $@
	ln $< $@

lib: $(objpfx)libmcheck.a
