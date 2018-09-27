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
	char		*__nat_postrouting ;
	char		*__vip ;
	char		*__port_mapping ;
	
	char		*__debug ;
	char		*__forcely ;
} ;

struct CockerEnvironment
{
	struct CommandParameter	cmd_para ;
	
	char			cocker_home[ PATH_MAX ] ;
	char			images_path_base[ PATH_MAX ] ;
	char			containers_path_base[ PATH_MAX ] ;
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
int DoAction_install_test( struct CockerEnvironment *cocker_env );

int DoAction_kill( struct CockerEnvironment *cocker_env );

/* depend on
$ sudo yum install -y bridge-utils
*/

/* for test
$ cocker -a install_test
$ cocker -a create --image test --container test --host-name test --vip 192.168.8.88 --nat-postrouting ens33
$ cocker -a start --container test
$ cocker -a stop --container test
$ cocker -a stop --container test --force
$ cocker -a destroy --container test
$ cocker -a destroy --container test --force
*/

#ifdef __cplusplus
}
#endif

#endif

