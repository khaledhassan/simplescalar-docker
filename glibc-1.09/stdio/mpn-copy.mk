mpn-found-1 := $(filter $(gmp-srcdir)/mpn/sparc8/%,$(mpn-found))
mpn-copy-1 := $(patsubst $(gmp-srcdir)/mpn/sparc8/%,$(sysdep_dir)/sparc/sparc8/%,$(mpn-found-1))
mpn-found := $(filter-out $(mpn-found-1),$(mpn-found))
mpn-copy-sysdep := $(mpn-copy-sysdep) $(mpn-copy-1)
$(mpn-copy-1): $(sysdep_dir)/sparc/sparc8/%: \
	       $(ignore gmp2glibc.sed) $(gmp-srcdir)/mpn/sparc8/%
	$(gmp2glibc)
mpn-found-1 := $(filter $(gmp-srcdir)/mpn/sparc9/%,$(mpn-found))
mpn-copy-1 := $(patsubst $(gmp-srcdir)/mpn/sparc9/%,$(sysdep_dir)/sparc/sparc9/%,$(mpn-found-1))
mpn-found := $(filter-out $(mpn-found-1),$(mpn-found))
mpn-copy-sysdep := $(mpn-copy-sysdep) $(mpn-copy-1)
$(mpn-copy-1): $(sysdep_dir)/sparc/sparc9/%: \
	       $(ignore gmp2glibc.sed) $(gmp-srcdir)/mpn/sparc9/%
	$(gmp2glibc)
mpn-found-1 := $(filter $(gmp-srcdir)/mpn/mc68000/%,$(mpn-found))
mpn-copy-1 := $(patsubst $(gmp-srcdir)/mpn/mc68000/%,$(sysdep_dir)/m68k/m68000/%,$(mpn-found-1))
mpn-found := $(filter-out $(mpn-found-1),$(mpn-found))
mpn-copy-sysdep := $(mpn-copy-sysdep) $(mpn-copy-1)
$(mpn-copy-1): $(sysdep_dir)/m68k/m68000/%: \
	       $(ignore gmp2glibc.sed) $(gmp-srcdir)/mpn/mc68000/%
	$(gmp2glibc)
mpn-found-1 := $(filter $(gmp-srcdir)/mpn/mc68020/%,$(mpn-found))
mpn-copy-1 := $(patsubst $(gmp-srcdir)/mpn/mc68020/%,$(sysdep_dir)/m68k/m68020/%,$(mpn-found-1))
mpn-found := $(filter-out $(mpn-found-1),$(mpn-found))
mpn-copy-sysdep := $(mpn-copy-sysdep) $(mpn-copy-1)
$(mpn-copy-1): $(sysdep_dir)/m68k/m68020/%: \
	       $(ignore gmp2glibc.sed) $(gmp-srcdir)/mpn/mc68020/%
	$(gmp2glibc)
mpn-found-1 := $(filter $(gmp-srcdir)/mpn/mc88100/%,$(mpn-found))
mpn-copy-1 := $(patsubst $(gmp-srcdir)/mpn/mc88100/%,$(sysdep_dir)/m88k/m88100/%,$(mpn-found-1))
mpn-found := $(filter-out $(mpn-found-1),$(mpn-found))
mpn-copy-sysdep := $(mpn-copy-sysdep) $(mpn-copy-1)
$(mpn-copy-1): $(sysdep_dir)/m88k/m88100/%: \
	       $(ignore gmp2glibc.sed) $(gmp-srcdir)/mpn/mc88100/%
	$(gmp2glibc)
mpn-found-1 := $(filter $(gmp-srcdir)/mpn/mc88110/%,$(mpn-found))
mpn-copy-1 := $(patsubst $(gmp-srcdir)/mpn/mc88110/%,$(sysdep_dir)/m88k/m88110/%,$(mpn-found-1))
mpn-found := $(filter-out $(mpn-found-1),$(mpn-found))
mpn-copy-sysdep := $(mpn-copy-sysdep) $(mpn-copy-1)
$(mpn-copy-1): $(sysdep_dir)/m88k/m88110/%: \
	       $(ignore gmp2glibc.sed) $(gmp-srcdir)/mpn/mc88110/%
	$(gmp2glibc)
mpn-found-1 := $(filter $(gmp-srcdir)/mpn/r3000/%,$(mpn-found))
mpn-copy-1 := $(patsubst $(gmp-srcdir)/mpn/r3000/%,$(sysdep_dir)/mips/r3000/%,$(mpn-found-1))
mpn-found := $(filter-out $(mpn-found-1),$(mpn-found))
mpn-copy-sysdep := $(mpn-copy-sysdep) $(mpn-copy-1)
$(mpn-copy-1): $(sysdep_dir)/mips/r3000/%: \
	       $(ignore gmp2glibc.sed) $(gmp-srcdir)/mpn/r3000/%
	$(gmp2glibc)
mpn-found-1 := $(filter $(gmp-srcdir)/mpn/r4000/%,$(mpn-found))
mpn-copy-1 := $(patsubst $(gmp-srcdir)/mpn/r4000/%,$(sysdep_dir)/mips/r4000/%,$(mpn-found-1))
mpn-found := $(filter-out $(mpn-found-1),$(mpn-found))
mpn-copy-sysdep := $(mpn-copy-sysdep) $(mpn-copy-1)
$(mpn-copy-1): $(sysdep_dir)/mips/r4000/%: \
	       $(ignore gmp2glibc.sed) $(gmp-srcdir)/mpn/r4000/%
	$(gmp2glibc)
mpn-found-1 := $(filter $(gmp-srcdir)/mpn/hppa1_0/%,$(mpn-found))
mpn-copy-1 := $(patsubst $(gmp-srcdir)/mpn/hppa1_0/%,$(sysdep_dir)/hppa/hppa1.0/%,$(mpn-found-1))
mpn-found := $(filter-out $(mpn-found-1),$(mpn-found))
mpn-copy-sysdep := $(mpn-copy-sysdep) $(mpn-copy-1)
$(mpn-copy-1): $(sysdep_dir)/hppa/hppa1.0/%: \
	       $(ignore gmp2glibc.sed) $(gmp-srcdir)/mpn/hppa1_0/%
	$(gmp2glibc)
mpn-found-1 := $(filter $(gmp-srcdir)/mpn/hppa1_1/%,$(mpn-found))
mpn-copy-1 := $(patsubst $(gmp-srcdir)/mpn/hppa1_1/%,$(sysdep_dir)/hppa/hppa1.1/%,$(mpn-found-1))
mpn-found := $(filter-out $(mpn-found-1),$(mpn-found))
mpn-copy-sysdep := $(mpn-copy-sysdep) $(mpn-copy-1)
$(mpn-copy-1): $(sysdep_dir)/hppa/hppa1.1/%: \
	       $(ignore gmp2glibc.sed) $(gmp-srcdir)/mpn/hppa1_1/%
	$(gmp2glibc)
