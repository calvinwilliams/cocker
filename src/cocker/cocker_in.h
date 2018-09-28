#ifndef _H_COCKER_IN_
#define _H_COCKER_IN_

#include "cocker_util.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CommandParameter
{
	char		*_action ;
	char		*_show ;
	
	char		*__image_id ;
	char		*__container_id ;
	char		*__host_name ;
	char		*__net ;
	char		*__host_if_name ;
	char		*__vip ;
	char		*__port_mapping ;
	
	char		*__attach ;
	
	char		*__debug ;
	char		*__forcely ;
} ;

struct CockerEnvironment
{
	struct CommandParameter	cmd_para ;
	
	char			cocker_home[ PATH_MAX + 1 ] ;
	
	char			images_path_base[ PATH_MAX + 1 ] ;
	char			containers_path_base[ PATH_MAX + 1 ] ;
	
	char			container_id[ CONTAINER_NAME_MAX + 1 ] ;
	char			container_path_base[ PATH_MAX + 1 ] ;
	
	char			net[ NET_LEN_MAX + 1 ] ;
	char			host_if_name[ ETHERNET_NAME_MAX + 1 ] ;
	
	char			netns_name[ ETHERNET_NAME_MAX + 1 ] ;
	char			netbr_name[ ETHERNET_NAME_MAX + 1 ] ;
	char			eth_name[ ETHERNET_NAME_MAX + 1 ] ;
	char			veth_name[ ETHERNET_NAME_MAX + 1 ] ;
	char			veth_sname[ ETHERNET_NAME_MAX + 1 ] ;
	char			netbr_ip[ IP_LEN_MAX + 1 ] ;
} ;

/*
 * util
 */

void GetEthernetName( char *container_id , struct CockerEnvironment *cocker_env );

/*
 * environment
 */

int CreateCockerEnvironment( struct CockerEnvironment **pp_cocker_env );
void DestroyCockerEnvironment( struct CockerEnvironment **pp_cocker_env );

int DoShow_images( struct CockerEnvironment *cocker_env );
int DoShow_containers( struct CockerEnvironment *cocker_env );
int DoAction_create( struct CockerEnvironment *cocker_env );
int DoAction_destroy( struct CockerEnvironment *cocker_env );
int DoAction_start( struct CockerEnvironment *cocker_env );
int DoAction_stop( struct CockerEnvironment *cocker_env );
int DoAction_kill( struct CockerEnvironment *cocker_env );
int DoAction_install_test( struct CockerEnvironment *cocker_env );

/* depend on
$ sudo yum install -y bridge-utils
$ echo "1" >/proc/sys/net/ipv4/ip_forward
*/

/* for test
$ cocker -a install_test --debug
$ cocker -s images
$ cocker -s containers
$ cocker -a create --image-id test --host-name test --net BRIDGE --vip 166.88.0.2 --debug --container-id test
$ cocker -a create --image-id test --host-name test --net HOST --vip 166.88.0.2 --debug
$ cocker -a create --image-id test --host-name test --net CUSTOM --vip 166.88.0.2 --debug
$ cocker -a start --attach --debug --container-id test
$ cocker -a stop --debug --container-id test
$ cocker -a stop --debug --forcely --container-id test
$ cocker -a destroy --debug --container-id test
$ cocker -a destroy --debug --forcely --container-id test
*/

#ifdef __cplusplus
}
#endif

#endif

