#!/bin/sh

set -e
WORK=`pwd`

./configure --host=arm-linux --prefix=/usr/bticino-examples
make -j4
make install DESTDIR=$WORK/../../install