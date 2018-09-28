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
	
	char		*__image ;
	char		*__container ;
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
	
	char			cocker_home[ PATH_MAX ] ;
	char			images_path_base[ PATH_MAX ] ;
	char			containers_path_base[ PATH_MAX ] ;
	char			container_path_base[ PATH_MAX ] ;
	char			net[ 16 ] ;
	char			host_if_name[ 16 ] ;
	char			vip[ IP_MAX ] ;
} ;

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
$ cocker -a create --image test --container test --host-name test --net bridge --vip 166.88.0.2 --debug
$ cocker -a create --image test --container test --host-name test --net host --vip 166.88.0.2 --debug
$ cocker -a create --image test --container test --host-name test --net custom --vip 166.88.0.2 --debug
$ cocker -a start --container test --attach --debug
$ cocker -a stop --container test --debug
$ cocker -a stop --container test --debug --forcely
$ cocker -a destroy --container test --debug
$ cocker -a destroy --container test --debug --forcely
*/

#ifdef __cplusplus
}
#endif

#endif

