#include "vhm_in.h"

int CreateVhmEnvironment( struct VhmEnvironment **pp_vhm_env )
{
	struct VhmEnvironment	*vhm_env = NULL ;
	
	vhm_env = (struct VhmEnvironment *)malloc( sizeof(struct VhmEnvironment) ) ;
	if( vhm_env == NULL )
	{
		printf( "*** ERROR : malloc failed , errno[%d]\n" , errno );
		return 1;
	}
	memset( vhm_env , 0x00 , sizeof(struct VhmEnvironment) );
	
	(*pp_vhm_env) = vhm_env ;
	
	return 0;
}

void DestroyVhmEnvironment( struct VhmEnvironment **pp_vhm_env )
{
	free( (*pp_vhm_env) );
	(*pp_vhm_env) = NULL ;
	
	return;
}

