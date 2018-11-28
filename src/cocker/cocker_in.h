/*
 * cocker - Container Machine Engine
 * author	: calvin
 * email	: calvinwilliams@163.com
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#ifndef _H_COCKER_IN_
#define _H_COCKER_IN_

#include "cocker_util.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CockerVolume
{
	char			*host_path ;
	int			host_path_len ;
	char			*container_path ;
	
	struct list_head	volume_node ;
} ;

struct CommandParameter
{
	char			*_action ;
	char			*_show ;
	
	char			*__version ;
	char			*__image ;
	char			*__container ;
	char			*__host_name ;
	char			*__net ;
	char			*__host_eth ;
	char			*__vip ;
	char			*__port_mapping ;
	char			*__cpus ;
	char			*__cpu_quota ;
	char			*__mem_limit ;
	
	char			*__boot ;
	char			*__attach ;
	char			*__exec ;
	char			*__shutdown ;
	
	char			*__from_image ;
	char			*__to_image ;
	char			*__from_container ;
	char			*__to_container ;
	
	char			*__image_file ;
	
	char			*__debug ;
	char			*__forcely ;
	
	struct list_head	volume_list ;
	char			*__volume ;
	
	char			*__srepo ;
	char			*__match ;
	
	char			*__cmd ;
	char			*__template_file ;
	char			*__mapping_file ;
	char			*__instance_file ;
	char			*__src_file ;
	char			*__dst_file ;
} ;

struct CockerEnvironment
{
	struct CommandParameter	cmd_para ;
	unsigned char		cgroup_enable ;
	
	char			cocker_home[ PATH_MAX + 1 ] ;
	char			images_path_base[ PATH_MAX + 1 ] ;
	char			containers_path_base[ PATH_MAX + 1 ] ;
	char			netbr_ip[ IP_LEN_MAX + 1 ] ;
	
	char			host_eth_name[ ETHERNET_NAME_LEN_MAX + 1 ] ;
	char			host_eth_ip[ IP_LEN_MAX + 1 ] ;
	
	char			version_path_base[ PATH_MAX + 1 ] ;
	char			image_path_base[ PATH_MAX + 1 ] ;
	char			container_path_base[ PATH_MAX + 1 ] ;
	int			src_port ;
	int			dst_port ;
	char			netns_name[ ETHERNET_NAME_LEN_MAX + 1 ] ;
	char			netbr_name[ ETHERNET_NAME_LEN_MAX + 1 ] ;
	char			veth1_name[ ETHERNET_NAME_LEN_MAX + 1 ] ;
	char			veth0_name[ ETHERNET_NAME_LEN_MAX + 1 ] ;
	char			veth0_sname[ ETHERNET_NAME_LEN_MAX + 1 ] ;
	
	int			alive_pipe[ 2 ] ;
} ;

/*
 * util
 */

void GetEthernetNames( struct CockerEnvironment *env , char *container_id );
int SplitImageVersion( char *image_id , char *version , int version_bufsize );
int GetMaxVersionPath( char *version_path_base , char *max_version , int max_version_bufsize );

/*
 * environment
 */

int CreateCockerEnvironment( struct CockerEnvironment **pp_env );
void DestroyCockerEnvironment( struct CockerEnvironment **pp_env );

int CleanContainerResource( struct CockerEnvironment *env );

int CreateContainer( struct CockerEnvironment *env , char *__image_id , char *__container_id );

int DoShow_images( struct CockerEnvironment *env );
int DoShow_containers( struct CockerEnvironment *env );
int DoShow_container_root( struct CockerEnvironment *env );

int DoAction_install_test( struct CockerEnvironment *env );

int DoAction_create( struct CockerEnvironment *env );
int DoAction_destroy( struct CockerEnvironment *env );
int DoAction_boot( struct CockerEnvironment *env );
int DoAction_attach( struct CockerEnvironment *env );
int DoAction_run( struct CockerEnvironment *env );
int DoAction_rplfile( struct CockerEnvironment *env );
int DoAction_putfile( struct CockerEnvironment *env );
int DoAction_getfile( struct CockerEnvironment *env );
int DoAction_shutdown( struct CockerEnvironment *env );
int _DoAction_kill( struct CockerEnvironment *env , int signal_no );
int DoAction_kill( struct CockerEnvironment *env );

int DoAction_version( struct CockerEnvironment *env );

int DoAction_vip( struct CockerEnvironment *env );
int DoAction_port_mapping( struct CockerEnvironment *env );

int DoAction_volume( struct CockerEnvironment *env );

int DoAction_to_image( struct CockerEnvironment *env );
int DoAction_to_container( struct CockerEnvironment *env );

int DoAction_copy_image( struct CockerEnvironment *env );
int DoAction_del_image( struct CockerEnvironment *env );

int DoAction_export( struct CockerEnvironment *env );
int DoAction_import( struct CockerEnvironment *env );

int DoShow_ssearch( struct CockerEnvironment *env );

int DoAction_spush( struct CockerEnvironment *env );
int DoAction_spull( struct CockerEnvironment *env );

/* depend on
yum install -y telnet
yum install -y nmap-ncat
yum install -y bridge-utils
yum install -y man-pages
yum install -y man-pages-zh-CN
echo "1" >/proc/sys/net/ipv4/ip_forward
	or add net.ipv4.ip_forward=1 to /etc/sysctl.conf and run sysctl -p
*/

/* for test
cocker -s images
cocker -s containers
cocker -s container_root

cocker -a install_test -d --version "1.0.0"

cocker -a create -d -m test -c test
cocker -a create -d -m test --host test --net BRIDGE --vip 166.88.0.2 --port-mapping 19527:9527 -c test
cocker -a create -d -m test --host test --net HOST --vip 166.88.0.2
cocker -a create -d -m test --host test --net CUSTOM --vip 166.88.0.2
cocker -a create -d -m "test:test2" --host test --net BRIDGE --vip 166.88.0.2 --port-mapping 19527:9527 -c test
cocker -a create -d -m test --volume "/tmp:/tmp" --volume "/mnt/cdrom:/mnt/cdrom" --host test --net BRIDGE --vip 166.88.0.2 --port-mapping 19527:9527 -c test
cocker -a create -d -m test --host test --net BRIDGE --vip 166.88.0.2 --port-mapping 19527:9527 -c test -b
cocker -a create -d -m test --host test --net BRIDGE --vip 166.88.0.2 --port-mapping 19527:9527 -c test -b -t
cocker -a create -d -m test --host test --net BRIDGE --vip 166.88.0.2 --port-mapping 19527:9527 -c test -b -t -e "/bin/bash -l"
cocker -a create -d -m "calvin=rhel-7.4-x86_64:1.0.0,calvin=rhel-7.4-gcc-x86_64" --host test --net BRIDGE --vip 166.88.0.2 --port-mapping "19527:9527,5142:5142" -c test
cocker -a create -d -m "calvin=rhel-7.4-x86_64,calvin=rhel-7.4-sshd-x86_64" --host test --net BRIDGE --vip 166.88.0.2 --port-mapping "2222:22" -c test
cocker -a boot -d -c test
cocker -a boot -d -c test -t
cocker -a boot -d --cpus 1 --cpu-quota 30% --mem-limit 100M -c test -t
cocker -a boot -d -c test -t -e "/bin/bash -l"
cocker -a attach -d -c test
cocker -a run -d -c test --cmd "hostname"
cocker -a shutdown -d -c test
cocker -a destroy -d -c test
cocker -a destroy -d -c test -h
cocker -a destroy -d -f -c test

printf "\${LEAF} ÎÒµÄÊ÷Ò¶\n" >map.txt
cocker -a rplfile -d -c test --template-file "/root/tpl.txt" --mapping-file "map.txt" --instance-file "/root/ins.txt"
cocker -a rplfile -d -c test --template-file "/root/tpl.txt" --mapping-file "map.txt"

cocker -a putfile -c test --src-file "map.txt" --dst-file "/root/"
cocker -a getfile -c test --src-file "/root/map.txt" --dst-file "./"

cocker -a version -d -m test --version "1.0.1"
cocker -a version -d -m "test:1.0.1" --version "1.0.2"
cocker -a version -d -m "test:1.0.2"

cocker -a vip -d --vip 166.88.0.3 -c test
cocker -a port_mapping -d --port-mapping 19528:9528 -c test
cocker -a volume -d --volume "/tmp:/tmp" --volume "/mnt/cdrom:/mnt/cdrom" -c test

cocker -a to_container -d --from-image test --host test --net BRIDGE --vip 166.88.0.2 --port-mapping 19527:9527 --to-container test
cocker -a to_image -d --from-container test --to-image test

cocker -a copy_image -d --from-image test --to-image "test2:1.0.0"
cocker -a del_image -d -m "test2:1.0.0"

cocker -a export -d -m "test:1.1.0"
cocker -a del_image -d -m "test:1.1.0"
cocker -a import -d -m "test:1.1.0"

cocker -a import -d --image-file test.cockerimage
cocker -a import -d --image-file rhel-7.4-x86_64.cockerimage -m rhel-7.4
cocker -a import -d --image-file rhel-7.4-gcc-x86_64.cockerimage -m rhel-7.4-gcc

cocker -a spush -m "test:1.0.0" -d
cocker -a del_image -m "test:1.0.0" -d
cocker -a spull -m "test:1.0.0" -d

cocker -s ssearch --srepo "cockerimages@192.168.6.74"
cocker -s ssearch --match test

cocker -a spush -m calvin=rhel-7.4-x86_64:1.0.0 -d -f

. cocker_container_root.sh test
*/

/* for test
ps -ef | grep -v grep | grep cockerinit | awk '{print $2}' | xargs kill -9

sudo cp ~/src/cocker/src/cockerinit/cockerinit /var/cocker/images/test/rlayer/bin/
sudo cp ~/src/cocker/src/util/libcocker_util.so /var/cocker/images/test/rlayer/lib64/

ip netns del netnstest

echo "let S=0 ; while [ 1 ] ; do let S++; done" >test.sh

cp /home/calvin/src/cocker/src/cockerinit/cockerinit /var/cocker/images/calvin=rhel-7.4-x86_64/1.0.0/rlayer/usr/bin/cockerinit
cocker -a boot -c G6
cocker -a shutdown -c G6
cocker -a run -c "G6" --cmd "nohup /usr/sbin/sshd -D &"


cocker -a create -m "calvin=rhel-7.4-x86_64,calvin=sshd,calvin=G6" --host G6 --net BRIDGE --vip 166.88.0.2 --port-mapping "8600:8600,2222:222" -c "G6" -b -e "/root/bin/G6 -f /root/etc/G6.conf --no-daemon" -d
cocker -a boot -c "G6"
cocker -a attach -c "G6"
cocker -a boot -c "G6" -e "/root/bin/G6 -f /root/etc/G6.conf --no-daemon"
cocker -a shutdown -c G6
cocker -a shutdown -c G6 -f
cocker -a destroy -c G6
cocker -a destroy -c G6 -h
*/

#ifdef __cplusplus
}
#endif

#endif

