#include "cocker_in.h"

int CreateCockerEnvironment( struct CockerEnvironment **pp_cocker_env )
{
	struct CockerEnvironment	*cocker_env = NULL ;
	
	cocker_env = (struct CockerEnvironment *)malloc( sizeof(struct CockerEnvironment) ) ;
	if( cocker_env == NULL )
	{
		printf( "*** ERROR : malloc failed , errno[%d]\n" , errno );
		return 1;
	}
	memset( cocker_env , 0x00 , sizeof(struct CockerEnvironment) );
	
	(*pp_cocker_env) = cocker_env ;
	
	return 0;
}

void DestroyCockerEnvironment( struct CockerEnvironment **pp_cocker_env )
{
	free( (*pp_cocker_env) );
	(*pp_cocker_env) = NULL ;
	
	return;
}

