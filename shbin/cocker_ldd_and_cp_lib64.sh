#!/bin/bash

mkdir -p ../lib ../lib64

ls -1 | while read EXEFILE ; do
	ldd $EXEFILE | awk '{if(NF==4&&$2=="=>")print $3;else if(NF==2)print $1}' | while read LIBFILE ; do
		if [ ! -f ..$LIBFILE ] ; then
			cp $LIBFILE ../lib64/
		fi
	done
done

