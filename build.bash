#!/usr/bin/env bash

# Archives already extracted, this script should build simplescalar

# Install simpleutils
cd $IDIR/simpleutils-990811
./configure -host=$HOST -target=$TARGET -with-gnu-as -with-gnu-ld -prefix=$IDIR
make
make install

# Install simulator
cd $IDIR/simplesim-3.0
make config-pisa
make
# Test simulator
./sim-safe tests/bin.little/test-math

# Install cross-compiler
cd $IDIR/gcc-2.7.2.3
export PATH=$PATH:$IDIR/sslittle-na-sstrix/bin
./configure -host=$HOST -target=$TARGET -with-gnu-as -with-gnu-ld -prefix=$IDIR
chmod -R +w .
## Apply first patch
patch -p1 <../patches/gcc-changes-after-configured.patch
## Copy files as described in instructions
cp ./patched/sys/cdefs.h ../sslittle-na-sstrix/include/sys/cdefs.h
cp ../sslittle-na-sstrix/lib/libc.a ../lib/
cp ../sslittle-na-sstrix/lib/crt0.o ../lib/
## Replace ar and ranlib from tarball
tar xzvf ../tarballs/ar_and_ranlib.tar.gz -C $IDIR/sslittle-na-sstrix/bin
chmod +w $IDIR/sslittle-na-sstrix/bin/ar $IDIR/sslittle-na-sstrix/bin/ranlib
## Try compiling
### original instructions had just a regular make here but copying the second make command helps avoid a conflict between i386 and x86-64 architectures
### choice of flags inspired by <http://stackoverflow.com/questions/5383325/how-to-create-a-32-bit-shared-library-on-a-64-bit-platform-with-autotools>
make LANGUAGES="c c++" CFLAGS="-O" CC="gcc -m32" LDFLAGS="-m32"
## Apply second patch
patch -p1 < ../patches/gcc-insn-output.patch
## Try compiling again...
make LANGUAGES="c c++" CFLAGS="-O" CC="gcc -m32" LDFLAGS="-m32"
## Apply third patch!
patch -p1 < ../patches/gcc-cxxmain.patch
## Try compiling one last time...
make LANGUAGES="c c++" CFLAGS="-O" CC="gcc -m32" LDFLAGS="-m32"
make install LANGUAGES="c c++" CFLAGS="-O" CC="gcc -m32" LDFLAGS="-m32"

