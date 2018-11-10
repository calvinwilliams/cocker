######################################################################
# cocker - Container Machine Engine
# author : calvin
# email  : calvinwilliams@163.com
#
# Licensed under the LGPL v2.1, see the file LICENSE in base directory.
######################################################################

#!/bin/bash

which supermin5 >/dev/null
if [ $? -ne 0 ] ; then
	echo "*** ERROR : Install supermin5 first"
fi

printf "Your name or enter null : "
read MY_NAME
if [ x"${MY_NAME}" != x"" ] ; then
	MY_NAME="${MY_NAME}@"
fi

printf "Version or enter null : "
read VERSION
if [ x"${VERSION}" != x"" ] ; then
	VERSION=":${VERSION}"
fi

IMAGE_FILENAME="${MY_NAME}rhel-7.4-gcc-x86_64${VERSION}.cockerimage"

rm -rf appliance.d supermin.d

rm -f ${IMAGE_FILENAME}

supermin5 -v --prepare bash coreutils -o supermin.d
supermin5 -v --prepare bash gcc make -o supermin.d
supermin5 -v --build --format chroot supermin.d -o appliance.d

IMAGE_RLAYER_PATH_BASE="appliance.d"

rm -f ${IMAGE_RLAYER_PATH_BASE}/usr/bin/ld
cp -f /usr/bin/ld ${IMAGE_RLAYER_PATH_BASE}/usr/bin/

cd appliance.d && tar --numeric-owner -cvzf ../${IMAGE_FILENAME} * && cd ..

rm -rf appliance.d supermin.d

