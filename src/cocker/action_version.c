/*
 * cocker - Container Machine Engine
 * author	: calvin
 * email	: calvinwilliams@163.com
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#include "cocker_in.h"

int DoAction_version( struct CockerEnvironment *env )
{
	char		image_version_file[ PATH_MAX + 1 ] ;
	
	int		nret = 0 ;
	
	/* preprocess input parameters */
	Snprintf( env->image_path_base , sizeof(env->image_path_base)-1 , "%s/%s" , env->images_path_base , env->cmd_para.__image_id );
	nret = access( env->image_path_base , F_OK ) ;
	I1TER1( "*** ERROR : image '%s' not found\n" , env->cmd_para.__image_id )
	
	/* modify vip */
	nret = WriteFileLine( env->cmd_para.__version , image_version_file , sizeof(image_version_file) , "%s/version" , env->image_path_base ) ;
	INTER1( "*** ERROR : WriteFileLine version failed[%d] , errno[%d]\n" , nret , errno )
	EIDTE( "write file [%s] ok\n" , image_version_file )
	
	printf( "OK\n" );
	
	return 0;	
}

