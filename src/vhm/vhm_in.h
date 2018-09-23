#ifndef _H_VHM_IN_
#define _H_VHM_IN_

#include "vh_util.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CommandParameter
{
	char		*_action ;
	
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

int VhmAction_Create( struct VhmEnvironment *vhm_env );
int VhmAction_InstallTest( struct VhmEnvironment *vhm_env );




#ifdef __cplusplus
}
#endif

#endif

