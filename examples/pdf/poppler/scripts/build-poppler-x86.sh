#!/bin/sh

set -e
WORK=`pwd`
VERSION="-Wl,-Bsymbolic -Wl,-Bsymbolic-functions"

LDFLAGS="$VERSION" \
PATH=/lib/qt4lib.x86/bin:$PATH \
POPPLER_QT4_CFLAGS=-I/lib/qt4lib.x86/include \
POPPLER_QT4_LIBS=-L/lib/qt4lib.x86/lib \
./configure --without-x --disable-cms --disable-utils \
    --disable-abiword-output --disable-poppler-qt --disable-poppler-cpp \
    --disable-gdk --disable-poppler-glib --disable-cairo-output \
    --enable-splash-output --enable-libpng --disable-libcurl \
    --enable-zlib --prefix=/usr/bticino-examples-x86
for i in goo fofi splash poppler qt4/src; do
    make -C $i -j4
    make -C $i install DESTDIR=$WORK/../../install-x86
done
