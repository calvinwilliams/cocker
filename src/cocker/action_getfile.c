/*
 * cocker - Container Machine Engine
 * author	: calvin
 * email	: calvinwilliams@163.com
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#include "cocker_in.h"

int DoAction_getfile( struct CockerEnvironment *env )
{
	char		src_file[ PATH_MAX + 1 ] ;
	char		cmd[ 4096 ] ;
	
	int		nret = 0 ;
	
	SetLogcFile( "/var/cocker/cocker.log" );
	SetLogcLevel( LOGCLEVEL_INFO );
	
	/* preprocess input parameters */
	Snprintf( env->container_path_base , sizeof(env->container_path_base)-1 , "%s/%s" , env->containers_path_base , env->cmd_para.__container );
	nret = access( env->container_path_base , F_OK ) ;
	INTER1( "*** ERROR : container '%s' not found\n" , env->cmd_para.__container )
	
	Snprintf( src_file , sizeof(src_file)-1 , "%s/merged%s" , env->container_path_base , env->cmd_para.__src_file );
	nret = access( src_file , F_OK ) ;
	INTER1( "*** ERROR : src file '%s' not found\n" , src_file )
	
	/* get file */
	nret = SnprintfAndSystem( cmd , sizeof(cmd) , "cp -rf %s %s" , src_file , env->cmd_para.__dst_file );
	INTER1( "*** ERROR : system [%s] failed[%d]\\n" , cmd , nret )
	
	printf( "OK\n" );
	
	return 0;
}

