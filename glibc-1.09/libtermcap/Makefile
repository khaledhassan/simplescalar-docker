
subdir	:= libtermcap

headers	:= termcap.h

defines := $(defines) -DNO_ARG_ARRAY

routines := termcap tparam tc_vers

extra-objs := libtermcap.a

install-lib := libtermcap.a

include ../Rules

$(objpfx)libtermcap.a: $(dep-dummy-lib); $(make-dummy-lib)
lib: $(objpfx)libtermcap.a

