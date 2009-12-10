#!/bin/bash
# This script will automatically change symlinks to the correct BTouch binary and cfg
# directory based on the most recent binary file found.
# You must have as many symlinks as the number of exebutables defined below, with names:
#  cfg.[binary file name]

#set -x
EXECUTABLES="BTouch.x86 BTouch.touchx.x86 touchx/TouchX.touchx.x86"
echo $EXECUTABLES

NEWEST=
for f in $EXECUTABLES
do
    if [ -z "$NEWEST" ]
    then 
        NEWEST=$f
    elif [ "$f" -nt "$NEWEST" ] 
    then
        NEWEST=$f
    fi
done

ln -sf $NEWEST BTouch


CONF_SUFFIX=`basename $NEWEST`
CONF_DIR=cfg.$CONF_SUFFIX
if [ -h $CONF_DIR ]; then
	ln -sfn $CONF_DIR cfg;
else
	echo "No $CONF_DIR present";
	exit 1;
fi
