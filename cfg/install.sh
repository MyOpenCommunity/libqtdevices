#!/bin/bash
if [ $# == 1 ]
then
	cp $1 /nfs_btouch/home/bticino/cfg/conf.xml
else
	echo "Copia il file di configurazione passato come argomento nell'nfs esportato per il BTouch"
	echo "Utilizzo: ./install.sh <conf_file.xml>"
fi
