#ifndef _H_VHM_IN_
#define _H_VHM_IN_

#include "vh_util.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CommandParameter
{
	char		*_action ;
	char		*_show ;
	
	char		*__vhost ;
	char		*__vtemplate ;
	char		*__host_name ;
} ;

struct VhmEnvironment
{
	struct CommandParameter	cmd_para ;
	
	char			openvh_home[ PATH_MAX ] ;
	char			vtemplates_path_base[ PATH_MAX ] ;
	char			vhosts_path_base[ PATH_MAX ] ;
} ;

/*
 * environment
 */

int CreateVhmEnvironment( struct VhmEnvironment **pp_vhm_env );
void DestroyVhmEnvironment( struct VhmEnvironment **pp_vhm_env );

int VhmShow_vtemplates( struct VhmEnvironment *vhm_env );
int VhmShow_vhosts( struct VhmEnvironment *vhm_env );
int VhmAction_create( struct VhmEnvironment *vhm_env );
int VhmAction_destroy( struct VhmEnvironment *vhm_env );
int VhmAction_start( struct VhmEnvironment *vhm_env );
int VhmAction_stop( struct VhmEnvironment *vhm_env );
int VhmAction_install_test( struct VhmEnvironment *vhm_env );




#ifdef __cplusplus
}
#endif

#endif

