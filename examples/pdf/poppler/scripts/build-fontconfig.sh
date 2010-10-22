#!/bin/sh

set -e
WORK=`pwd`

./configure --host=arm-linux --prefix=/usr/bticino-examples --with-arch=arm --with-freetype-config="$WORK/../../install/usr/bticino-examples/bin/freetype-config --prefix=$WORK/../../install/usr/bticino-examples" --with-expat-includes=$WORK/../../install/usr/bticino-examples/include --with-expat-lib=$WORK/../../install/usr/bticino-examples/lib --disable-libxml2
make -j4
make install DESTDIR=$WORK/../../install
