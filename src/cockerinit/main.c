/*
 * cocker - Container Machine Engine
 * author	: calvin
 * email	: calvinwilliams@163.com
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#include "cockerinit_in.h"

int main( int argc , char *argv[] )
{
	struct CockerInitEnvironment	*env = NULL ;
	int				i ;
	char				*p = NULL ;
	
	int				nret = 0 ;
	
	mkdir( "/var/cocker" , 00777 );
	SetLogcFile( "/var/cocker/cockerinit.log" );
	SetLogcLevel( LOGCLEVEL_INFO );
	
	INFOLOGC( "--- cockerinit begin ---" )
	
	env = (struct CockerInitEnvironment *)malloc( sizeof(struct CockerInitEnvironment) ) ;
	if( env == NULL )
	{
		E( "*** ERROR : malloc failed , errno[%d]\n" , errno )
		return 1;
	}
	memset( env , 0x00 , sizeof(struct CockerInitEnvironment) );
	
	for( i = 1 ; i < argc ; i++ )
	{
		if( STRCMP( argv[i] , == , "-v" ) )
		{
			E( "cockerinit v%s build %s %s\n" , _COCKER_VERSION , __DATE__ , __TIME__ )
			free( env );
			exit(0);
		}
		else if( STRCMP( argv[i] , == , "--container" ) && i + 1 < argc )
		{
			env->cmd_para.__container_id = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "--single" ) )
		{
			env->cmd_para.__single = argv[i] ;
		}
		else
		{
			E( "*** ERROR : invalid parameter '%s'\n" , argv[i] )
			return -7;
		}
	}
	if( env->cmd_para.__container_id == NULL )
	{
		E( "*** ERROR : '--container' need for running\n" )
		return -7;
	}
	
	p = getenv( "COCKER_ALIVE_PIPE" ) ;
	if( p == NULL )
	{
		E( "*** ERROR : please run me by cocker\n" )
		return 1;
	}
	else
	{
		I( "getenv(\"COCKER_ALIVE_PIPE\")[%s]\n" , p )
	}
	env->alive_pipe_0 = atoi(p) ;
	
	nret = server( env ) ;
	
	free( env );
	
	INFOLOGC( "--- cockerinit end ---" )
	
	return -nret;
}
