/*
 * cocker - Container Machine Engine
 * author	: calvin
 * email	: calvinwilliams@163.com
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#include "cocker_in.h"

int DoShow_container_root( struct CockerEnvironment *env )
{
	char		container_root_path[ PATH_MAX + 1 ] ;
	
	int		nret = 0 ;
	
	SetLogcFile( "/var/cocker/cocker.log" );
	SetLogcLevel( LOGCLEVEL_INFO );
	
	/* preprocess input parameters */
	Snprintf( env->container_path_base , sizeof(env->container_path_base)-1 , "%s/%s" , env->containers_path_base , env->cmd_para.__container );
	nret = access( env->container_path_base , F_OK ) ;
	INTER1( "*** ERROR : container '%s' not found\n" , env->cmd_para.__container )
	
	Snprintf( container_root_path , sizeof(container_root_path) , "%s/merged" , env->container_path_base );
	
	/* get container root */
	printf( "%s\n" , container_root_path );
	
	return 0;
}

