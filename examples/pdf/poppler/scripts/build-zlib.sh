#!/bin/sh

set -e
WORK=`pwd`

CC=arm-linux-gcc ./configure --prefix=/usr/bticino-examples
make -j4
make install DESTDIR=$WORK/../../install
