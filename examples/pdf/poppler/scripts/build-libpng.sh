#!/bin/sh

set -e
WORK=`pwd`

CFLAGS=-I$WORK/../../install/usr/bticino-examples/include \
LDFLAGS=-L$WORK/../../install/usr/bticino-examples/lib \
./configure --host=arm-linux --prefix=/usr/bticino-examples
make -j4
make install DESTDIR=$WORK/../../install
