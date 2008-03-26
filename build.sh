#!/bin/sh
export QTDIR=$HOME/src/bticino/myhome/qt-embedded-free-3.3.4
export QMAKESPEC=qws/linux-H4684_IP-g++
export PATH=$QTDIR/bin:$PATH
make
