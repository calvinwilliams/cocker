######################################################################
# cocker - Container Machine Engine
# author : calvin
# email  : calvinwilliams@163.com
#
# Licensed under the LGPL v2.1, see the file LICENSE in base directory.
######################################################################

#!/bin/bash

if [ $# -ne 1 ] ; then
	echo "USAGE : . cocker_container_root.sh (container)"
	exit 1
fi

CONTAINER=$1

export COCKER_CONTAINER_ROOT=`cocker -s container_root -c $CONTAINER`

