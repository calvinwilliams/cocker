#!/bin/bash

############################################################
# cocker shell
# copyright by calvin<calvinwilliams@163.com> 2018
############################################################

if [ $# -ne 1 ] ; then
	echo "USAGE : cocker_install_test.sh (container_path_base)"
	exit 1
fi

IMAGE_RLAYER_PATH_BASE=$1

cd ${IMAGE_RLAYER_PATH_BASE}
mkdir -p bin sbin lib lib64 etc root dev proc var

cd /bin
cp bash tty which ls cat echo rm pwd cd mkdir rmdir clear ps grep more id uname hostname vi vim awk sed tr file ldd top netstat ping telnet ssh nc ${IMAGE_RLAYER_PATH_BASE}/bin/
cp cockerinit ${IMAGE_RLAYER_PATH_BASE}/bin/
cd ${IMAGE_RLAYER_PATH_BASE}/bin/
cocker_ldd_and_cp_lib64.sh

cd /sbin
cp ifconfig route ip iptables ${IMAGE_RLAYER_PATH_BASE}/sbin/
cd ${IMAGE_RLAYER_PATH_BASE}/sbin/
cocker_ldd_and_cp_lib64.sh

cp /bin/cocker_etc_profile_template.sh ${IMAGE_RLAYER_PATH_BASE}/etc/profile
cp /bin/cocker_profile_template.sh ${IMAGE_RLAYER_PATH_BASE}/root/.profile

echo "root:x:0:0:root:/root:/bin/bash" >${IMAGE_RLAYER_PATH_BASE}/etc/passwd
echo "root:x:0:" >${IMAGE_RLAYER_PATH_BASE}/etc/group

mkdir -p ${IMAGE_RLAYER_PATH_BASE}/dev/pts

