/*
 * cocker - Container Machine Engine
 * author	: calvin
 * email	: calvinwilliams@163.com
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#include "cocker_in.h"

int DoAction_install_test( struct CockerEnvironment *env )
{
	char		version_path_base[ PATH_MAX + 1 ] ;
	char		image_rlayer_path[ PATH_MAX + 1 ] ;
	char		cmd[ 4096 ] ;
	
	int		nret = 0 ;
	
	/* preprocess input parameters */
	nret = SnprintfAndMakeDir( version_path_base , sizeof(version_path_base)-1 , "%s/test" , env->images_path_base ) ;
	if( env->cmd_para.__version )
	{
		nret = SnprintfAndMakeDir( env->image_path_base , sizeof(env->image_path_base)-1 , "%s/%s" , version_path_base , env->cmd_para.__version ) ;
	}
	else
	{
		nret = SnprintfAndMakeDir( env->image_path_base , sizeof(env->image_path_base)-1 , "%s/_" , version_path_base ) ;
	}
	INTER1( "*** ERROR : SnprintfAndMakeDir image_path_base failed[%d] , errno[%d]\n" , nret , errno )
	EIDTI( "mkdir %s ok\n" , env->image_path_base )
	
	/* create image */
	nret = SnprintfAndMakeDir( image_rlayer_path , sizeof(image_rlayer_path)-1 , "%s/rlayer" , env->image_path_base ) ;
	INTER1( "*** ERROR : SnprintfAndMakeDir rlayer failed[%d] , errno[%d]\n" , nret , errno )
	EIDTI( "mkdir %s ok\n" , image_rlayer_path )
	
	nret = SnprintfAndSystem( cmd , sizeof(cmd) , "cocker_install_test.sh %s" , image_rlayer_path ) ;
	INTER1( "*** ERROR : SnprintfAndSystem [cocker_install_test.sh %s] failed[%d] , errno[%d]\n" , env->image_path_base , nret , errno )
	EIDTI( "system [%s] ok\n" , cmd )
	
	printf( "OK\n" );
	
	return 0;
}

