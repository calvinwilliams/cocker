#include "cockerinit_in.h"

int main( int argc , char *argv[] )
{
	struct CockerInitEnvironment	*env = NULL ;
	int				i ;
	
	int				nret = 0 ;
	
	env = (struct CockerInitEnvironment *)malloc( sizeof(struct CockerInitEnvironment) ) ;
	if( env == NULL )
	{
		printf( "*** ERROR : malloc failed , errno[%d]\n" , errno );
		return 1;
	}
	memset( env , 0x00 , sizeof(struct CockerInitEnvironment) );
	
	for( i = 1 ; i < argc ; i++ )
	{
		if( STRCMP( argv[i] , == , "-v" ) )
		{
			printf( "cockerinit v%s build %s %s\n" , _COCKER_VERSION , __DATE__ , __TIME__ );
			free( env );
			exit(0);
		}
		else if( STRCMP( argv[i] , == , "--container" ) && i + 1 < argc )
		{
			strncpy( env->container_id , argv[i+1] , sizeof(env->container_id)-1 );
			i++;
		}
		else
		{
			printf( "*** ERROR : invalid parameter '%s'\n" , argv[i] );
			return -7;
		}
	}
	if( env->container_id[0] == '\0' )
	{
		printf( "*** ERROR : '--container' need for running\n" );
		return -7;
	}
	
	nret = server( env ) ;
	
	free( env );
	
	return -nret;
}
