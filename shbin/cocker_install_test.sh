######################################################################
# cocker - Container Machine Engine
# author : calvin
# email  : calvinwilliams@163.com
#
# Licensed under the LGPL v2.1, see the file LICENSE in base directory.
######################################################################

#!/bin/bash

if [ $# -ne 1 ] ; then
	echo "USAGE : cocker_install_test.sh (image_path_base)"
	exit 1
fi

IMAGE_RLAYER_PATH_BASE=$1

cd ${IMAGE_RLAYER_PATH_BASE}
mkdir -p -m 755 bin sbin lib lib64 usr etc root dev proc mnt var tmp
mkdir -p -m 755 mnt/cdrom

# install /bin and /lib64
cd /bin
cp bash tty which locale env ls cat echo cp mv rm pwd cd mkdir rmdir clear ps grep more id uname hostname vi vim awk sed tr file ldd top netstat ping telnet ssh nc mount umount ${IMAGE_RLAYER_PATH_BASE}/bin/
cp cockerinit ${IMAGE_RLAYER_PATH_BASE}/bin/
cd ${IMAGE_RLAYER_PATH_BASE}/bin/
cocker_ldd_and_cp_lib64.sh

# install /sbin and /lib64
cd /sbin
cp ifconfig route ip iptables ${IMAGE_RLAYER_PATH_BASE}/sbin/
cd ${IMAGE_RLAYER_PATH_BASE}/sbin/
cocker_ldd_and_cp_lib64.sh

# install /etc/profile and $HOME/.profile
cp /bin/cocker_etc_profile_template.sh ${IMAGE_RLAYER_PATH_BASE}/etc/profile
cp /bin/cocker_profile_template.sh ${IMAGE_RLAYER_PATH_BASE}/root/.profile

# install /etc/passwd and /etc/group
echo "root:x:0:0:root:/root:/bin/bash" >${IMAGE_RLAYER_PATH_BASE}/etc/passwd
echo "root:x:0:" >${IMAGE_RLAYER_PATH_BASE}/etc/group

# install /dev/pts
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

# install /usr/share/terminfo
mkdir -p ${IMAGE_RLAYER_PATH_BASE}/usr/share
cp -rf /usr/share/terminfo ${IMAGE_RLAYER_PATH_BASE}/usr/share/

# install other test files
printf "{ \"leaf\":\"\${LEAF}\" }\n" >${IMAGE_RLAYER_PATH_BASE}/root/tpl.txt

