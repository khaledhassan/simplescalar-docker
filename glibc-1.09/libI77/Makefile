
subdir	:= libI77

headers	:= f2c.h

defines := $(defines) -D_POSIX_SOURCE -DNON_UNIX_STDIO

routines := IVersion endfile inquire rsfe uio wsne backspace err lread \
	      rsli util xwsne iclose fmt lwrite rsne wref dfe fmtlib iopen \
	      sfe wrtfmt dolio iio rdfmt sue wsfe due ilnw irewind typesize \
	      wsle

distribute := fio fmt fp lio rawio

extra-objs := libI77.a

install-lib := libI77.a

# F2C's code is not too clean.
override +gccwarn := -w

include ../Rules

$(objpfx)libI77.a: $(dep-dummy-lib); $(make-dummy-lib)
lib: $(objpfx)libI77.a

