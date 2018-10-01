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
	
	char			*__attach ;
	
	char			*__debug ;
	char			*__forcely ;
} ;

struct CockerEnvironment
{
	struct CommandParameter	cmd_para ;
	
	char			cocker_home[ PATH_MAX + 1 ] ;
	char			images_path_base[ PATH_MAX + 1 ] ;
	char			containers_path_base[ PATH_MAX + 1 ] ;
	char			netbr_ip[ IP_LEN_MAX + 1 ] ;
	
	char			image_id[ IMAGE_NAME_MAX + 1 ] ;
	char			container_id[ CONTAINER_NAME_MAX + 1 ] ;
	char			net[ NET_LEN_MAX + 1 ] ;
	char			host_eth_name[ ETHERNET_NAME_MAX + 1 ] ;
	char			vip[ IP_LEN_MAX + 1 ] ;
	char			port_mapping[ 20+1+20 + 1 ] ;
	
	char			container_path_base[ PATH_MAX + 1 ] ;
	
	int			src_port ;
	int			dst_port ;
	
	char			netns_name[ ETHERNET_NAME_MAX + 1 ] ;
	char			netbr_name[ ETHERNET_NAME_MAX + 1 ] ;
	char			veth1_name[ ETHERNET_NAME_MAX + 1 ] ;
	char			veth0_name[ ETHERNET_NAME_MAX + 1 ] ;
	char			veth0_sname[ ETHERNET_NAME_MAX + 1 ] ;
} ;

/*
 * util
 */


/*
 * environment
 */

int CreateCockerEnvironment( struct CockerEnvironment **pp_env );
void DestroyCockerEnvironment( struct CockerEnvironment **pp_env );

int DoShow_images( struct CockerEnvironment *env );
int DoShow_containers( struct CockerEnvironment *env );
int DoAction_create( struct CockerEnvironment *env );
int DoAction_destroy( struct CockerEnvironment *env );
int DoAction_start( struct CockerEnvironment *env );
int DoAction_stop( struct CockerEnvironment *env );
int DoAction_kill( struct CockerEnvironment *env );
int DoAction_vip( struct CockerEnvironment *env );
int DoAction_port_mapping( struct CockerEnvironment *env );
int DoAction_install_test( struct CockerEnvironment *env );

/* depend on
$ sudo yum install -y bridge-utils
$ echo "1" >/proc/sys/net/ipv4/ip_forward
*/

/* for test
$ cocker -a install_test --debug
$ cocker -s images
$ cocker -s containers
$ cocker -a create --debug --image test --host test --net BRIDGE --vip 166.88.0.2 --port-mapping 19527:9527 --container test
$ cocker -a create --debug --image test --host test --net HOST --vip 166.88.0.2
$ cocker -a create --debug --image test --host test --net CUSTOM --vip 166.88.0.2
$ cocker -a start --debug --attach --container test
$ cocker -a stop --debug --container test
$ cocker -a stop --debug --forcely --container test
$ cocker -a destroy --debug --container test
$ cocker -a destroy --debug --forcely --container test
$ cocker -a vip --debug --vip 166.88.0.3 --container test
$ cocker -a port_mapping --debug --port-mapping 19528:9528 --container test
*/

#ifdef __cplusplus
}
#endif

#endif

