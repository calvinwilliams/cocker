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
	MY_NAME="${MY_NAME}="
fi

printf "Version or enter null : "
read VERSION
if [ x"${VERSION}" != x"" ] ; then
	VERSION=":${VERSION}"
fi

IMAGE_FILENAME="${MY_NAME}rhel-7.4-sshd-x86_64${VERSION}.cockerimage"

rm -rf appliance.d supermin.d

rm -f ${IMAGE_FILENAME}

supermin5 -v --prepare openssh-server -o supermin.d
supermin5 -v --build --format chroot supermin.d -o appliance.d

IMAGE_RLAYER_PATH_BASE="appliance.d"

echo "sshd:x:74:74:Privilege-separated SSH:/var/empty/sshd:/sbin/nologin" >>${IMAGE_RLAYER_PATH_BASE}/etc/passwd
# echo "root:root" | chpasswd -R ${IMAGE_RLAYER_PATH_BASE}
# echo "calvin:calvin" | chpasswd -R ${IMAGE_RLAYER_PATH_BASE}
ssh-keygen -t rsa -f ${IMAGE_RLAYER_PATH_BASE}/etc/ssh/ssh_host_rsa_key
ssh-keygen -t rsa -f ${IMAGE_RLAYER_PATH_BASE}/etc/ssh/ssh_host_ecdsa_key

cd appliance.d && tar --numeric-owner -cvzf ../${IMAGE_FILENAME} * && cd ..

rm -rf appliance.d supermin.d

echo "NOTICE : run 'passwd root' later in container"
echo "NOTICE : run 'nohup /usr/sbin/sshd -D' later in container"

