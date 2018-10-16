#ifndef _H_COCKER_IN_
#define _H_COCKER_IN_

#include "cocker_util.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CommandParameter
{
	char			*_action ;
	char			*_show ;
	
	char			*__image_id ;
	char			*__container_id ;
	char			*__host_name ;
	char			*__net ;
	char			*__host_eth ;
	char			*__vip ;
	char			*__port_mapping ;
	char			*__cpus ;
	char			*__cpu_quota ;
	char			*__mem_limit ;
	
	char			*__attach ;
	
	char			*__from_image ;
	char			*__to_image ;
	char			*__from_container ;
	char			*__to_container ;
	
	char			*__debug ;
	char			*__forcely ;
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

/*
 * environment
 */

int CreateCockerEnvironment( struct CockerEnvironment **pp_env );
void DestroyCockerEnvironment( struct CockerEnvironment **pp_env );

int CleanContainerResource( struct CockerEnvironment *env );

int CreateContainer( struct CockerEnvironment *env , char *__image_id , char *__container_id );

int DoShow_images( struct CockerEnvironment *env );
int DoShow_containers( struct CockerEnvironment *env );

int DoAction_install_test( struct CockerEnvironment *env );

int DoAction_create( struct CockerEnvironment *env );
int DoAction_destroy( struct CockerEnvironment *env );
int DoAction_boot( struct CockerEnvironment *env );
int DoAction_attach( struct CockerEnvironment *env );
int DoAction_shutdown( struct CockerEnvironment *env );
int DoAction_kill( struct CockerEnvironment *env );

int DoAction_vip( struct CockerEnvironment *env );
int DoAction_port_mapping( struct CockerEnvironment *env );

int DoAction_to_image( struct CockerEnvironment *env );
int DoAction_to_container( struct CockerEnvironment *env );

int DoAction_copy_image( struct CockerEnvironment *env );
int DoAction_del_image( struct CockerEnvironment *env );

/* depend on
sudo yum install -y telnet
sudo yum install -y nmap-ncat
sudo yum install -y bridge-utils
yum install -y man-pages
yum install -y man-pages-zh-CN
echo "1" >/proc/sys/net/ipv4/ip_forward
	or add net.ipv4.ip_forward=1 to /etc/sysctl.com and run sysctl -p
*/

/* for test
cocker -a install_test -d
cocker -s images
cocker -s containers
cocker -a create -d -m test --host test --net BRIDGE --vip 166.88.0.2 --port-mapping 19527:9527 -c test
cocker -a create -d -m test --host test --net HOST --vip 166.88.0.2
cocker -a create -d -m test --host test --net CUSTOM --vip 166.88.0.2
cocker -a boot -d --cpus 1 --cpu-quota 30% --mem-limit 100M -t -c test
cocker -a attach -d -c test
cocker -a shutdown -d -c test
cocker -a destroy -d -c test
cocker -a destroy -d -f -c test
cocker -a vip -d --vip 166.88.0.3 -c test
cocker -a port_mapping -d --port-mapping 19528:9528 -c test
cocker -a to_image -d --from-container test --to-image test2
cocker -a to_container -d -m test --from-image test2 --host test --net BRIDGE --vip 166.88.0.2 --port-mapping 19527:9527 --to-container test
cocker -a copy_image -d --from-image test --to-image test2
cocker -a del_image -d -m test2

ps -ef | grep -v grep | grep cockerinit | awk '{print $2}' | xargs kill -9

sudo cp ~/src/cocker/src/cockerinit/cockerinit /var/cocker/images/test/rlayer/bin/
sudo cp ~/src/cocker/src/util/libcocker_util.so /var/cocker/images/test/rlayer/lib64/

echo "let S=0 ; while [ 1 ] ; do let S++; done" >test.sh
*/

#ifdef __cplusplus
}
#endif

#endif

