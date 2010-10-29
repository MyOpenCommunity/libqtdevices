#!/bin/sh

set -e
WORK=`pwd`
VERSION="-Wl,-Bsymbolic"

PATH=/lib/qt4lib/bin:$PATH \
CC=arm-linux-gcc \
CXX=arm-linux-g++ \
POPPLER_QT4_CFLAGS=-I/lib/qt4lib/include \
POPPLER_QT4_LIBS=-L/lib/qt4lib/lib \
FREETYPE_CFLAGS=-I$WORK/../../install/usr/bticino-examples/include \
FREETYPE_LIBS=-L$WORK/../../install/usr/bticino-examples/lib \
LIBPNG_CFLAGS=-I$WORK/../../install/usr/bticino-examples/include \
LIBPNG_LIBS=-L$WORK/../../install/usr/bticino-examples/lib \
CPPFLAGS=-I$WORK/../../install/usr/bticino-examples/include \
LDFLAGS="-L$WORK/../../install/usr/bticino-examples/lib $VERSION" \
FONTCONFIG_CFLAGS=-L$WORK/../../install/usr/bticino-examples/include \
FONTCONFIG_LIBS=-L$WORK/../../install/usr/bticino-examples/lib \
./configure --host=arm-linux --without-x --disable-cms --disable-utils \
    --disable-abiword-output --disable-poppler-qt --disable-poppler-cpp \
    --disable-gdk --disable-poppler-glib --disable-cairo-output \
    --enable-splash-output --enable-libpng --disable-libcurl \
    --disable-largefile --enable-zlib --prefix=/usr/bticino-examples
for i in goo fofi splash poppler qt4/src; do
    make -C $i -j4
    make -C $i install DESTDIR=$WORK/../../install
done
