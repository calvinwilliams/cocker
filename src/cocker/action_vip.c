/*
 * cocker - Container Machine Engine
 * author	: calvin
 * email	: calvinwilliams@163.com
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#include "cocker_in.h"

int DoAction_vip( struct CockerEnvironment *env )
{
	char		container_vip_file[ PATH_MAX + 1 ] ;
	
	int		nret = 0 ;
	
	/* preprocess input parameters */
	Snprintf( env->container_path_base , sizeof(env->container_path_base)-1 , "%s/%s" , env->containers_path_base , env->cmd_para.__container );
	nret = access( env->container_path_base , F_OK ) ;
	I1TER1( "*** ERROR : container '%s' not found\n" , env->cmd_para.__container )
	
	/* modify vip */
	nret = WriteFileLine( env->cmd_para.__vip , container_vip_file , sizeof(container_vip_file) , "%s/vip" , env->container_path_base ) ;
	INTER1( "*** ERROR : WriteFileLine vip failed[%d] , errno[%d]\n" , nret , errno )
	EIDTE( "write file [%s] ok\n" , container_vip_file )
	
	printf( "OK\n" );
	
	return 0;	
}

