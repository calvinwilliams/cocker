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

IMAGE_FILENAME="${MY_NAME}rhel-7.4-x86_64${VERSION}.cockerimage"

rm -rf appliance.d supermin.d

rm -f ${IMAGE_FILENAME}

supermin5 -v --prepare bash coreutils -o supermin.d
supermin5 -v --prepare bash coreutils which man-db procps-ng hostname vim-minimal file iputils openssh-clients nmap-ncat util-linux tar wget curl net-tools iproute iptables iptables-services sysvinit-tools time bc m4 expect less findutils sysstat iotop ksh tcsh ftp zip gzip at shadow-utils dos2unix psmisc cronie telnet initscripts sudo cups-client lsof tcpdump sysctl glibc-common binutils valgrind strace yum -o supermin.d
supermin5 -v --build --format chroot supermin.d -o appliance.d

IMAGE_RLAYER_PATH_BASE="appliance.d"

mknod -m 600 ${IMAGE_RLAYER_PATH_BASE}/dev/console c 5 1
mknod -m 600 ${IMAGE_RLAYER_PATH_BASE}/dev/initctl p
mknod -m 666 ${IMAGE_RLAYER_PATH_BASE}/dev/full c 1 7
mknod -m 666 ${IMAGE_RLAYER_PATH_BASE}/dev/null c 1 3
mknod -m 666 ${IMAGE_RLAYER_PATH_BASE}/dev/zero c 1 5
mknod -m 666 ${IMAGE_RLAYER_PATH_BASE}/dev/random c 1 8
mknod -m 666 ${IMAGE_RLAYER_PATH_BASE}/dev/urandom c 1 9
mknod -m 666 ${IMAGE_RLAYER_PATH_BASE}/dev/ptmx c 5 2
mknod -m 666 ${IMAGE_RLAYER_PATH_BASE}/dev/tty c 5 0
mknod -m 666 ${IMAGE_RLAYER_PATH_BASE}/dev/tty0 c 4 0
mkdir -p ${IMAGE_RLAYER_PATH_BASE}/dev/pts

cp /bin/cockerinit ${IMAGE_RLAYER_PATH_BASE}/bin/
cp /lib64/libcocker_util.so ${IMAGE_RLAYER_PATH_BASE}/lib64/

cp /etc/resolv.conf ${IMAGE_RLAYER_PATH_BASE}/etc/

mkdir -p /mnt/cdrom

cd appliance.d && tar --numeric-owner -cvzf ../${IMAGE_FILENAME} * && cd ..

rm -rf appliance.d supermin.d

