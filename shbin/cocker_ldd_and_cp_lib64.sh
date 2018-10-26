/*
 * cocker - Container Machine Engine
 * author	: calvin
 * email	: calvinwilliams@163.com
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#!/bin/bash

############################################################
# cocker shell
# copyright by calvin<calvinwilliams@163.com> 2018
############################################################

mkdir -p ../lib ../lib64

ls -1 | while read EXEFILE ; do
	ldd $EXEFILE | awk '{if(NF==4&&$2=="=>")print $3;else if(NF==2)print $1}' | while read LIBFILE ; do
		if [ ! -f ..$LIBFILE ] ; then
			cp $LIBFILE ../lib64/
		fi
	done
done

