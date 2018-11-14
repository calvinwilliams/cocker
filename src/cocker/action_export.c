/*
 * cocker - Container Machine Engine
 * author	: calvin
 * email	: calvinwilliams@163.com
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#include "cocker_in.h"

int DoAction_export( struct CockerEnvironment *env )
{
	char		image[ IMAGES_ID_LEN_MAX + 1 ] ;
	char		version[ PATH_MAX + 1 ] ;
	char		image_file[ PATH_MAX + 1 ] ;
	char		current_path[ PATH_MAX + 1 ] ;
	char		cmd[ 4096 ] ;

	int		nret = 0 ;
	
	/* preprocess input parameters */
	Snprintf( image , sizeof(image) , "%s" , env->cmd_para.__image );
	SplitImageVersion( image , version , sizeof(version) );
	
	Snprintf( image_file , sizeof(image_file) , "%s%s%s.cockerimage" , image , (version[0]?":":"") , version );
	
	Snprintf( env->image_path_base , sizeof(env->image_path_base)-1 , "%s/%s/%s" , env->images_path_base , image , version );
	nret = access( env->image_path_base , F_OK ) ;
	I1TER1( "*** ERROR : image '%s' not found\n" , env->cmd_para.__image )
	
	/* pack image folders and files */
	memset( current_path , 0x00 , sizeof(current_path) );
	getcwd( current_path , sizeof(current_path) );
	
	nret = SnprintfAndSystem( cmd , sizeof(cmd) , "cd %s/rlayer/ && tar --force-local -czf %s * && mv %s %s/" , env->image_path_base , image_file , image_file , current_path ) ;
	INTER1( "*** ERROR : SnprintfAndSystem [cd %s/rlayer/ && tar --force-local -czf %s * && mv %s %s/] failed[%d] , errno[%d]\n" , env->image_path_base , image_file , image_file , current_path , nret , errno )
	EIDTI( "system [%s] ok\n" , cmd )
	
	printf( "OK\n" );
	
	return 0;
}

