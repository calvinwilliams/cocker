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
	char		image[ IMAGES_ID_LEN_MAX + 1 ] ;
	char		old_version[ PATH_MAX + 1 ] ;
	char		new_version[ PATH_MAX + 1 ] ;
	char		cmd[ 4096 ] ;
	
	int		nret = 0 ;
	
	/* preprocess input parameters */
	memset( image , 0x00 , sizeof(image) );
	strncpy( image , env->cmd_para.__image , sizeof(image)-1 );
	SplitImageVersion( image , old_version , sizeof(old_version) );
	if( old_version[0] == '\0' )
		strcpy( old_version , "_" );
	
	Snprintf( env->image_path_base , sizeof(env->image_path_base)-1 , "%s/%s/%s" , env->images_path_base , image , old_version );
	nret = access( env->image_path_base , F_OK ) ;
	I1TER1( "*** ERROR : image '%s' not found\n" , env->cmd_para.__image )
	
	memset( new_version , 0x00 , sizeof(new_version) );
	if( env->cmd_para.__version )
	{
		strncpy( new_version , env->cmd_para.__version , sizeof(new_version)-1 );
	}
	if( new_version[0] == '\0' )
	{
		strcpy( new_version , "_" );
	}
	
	/* modify version */
	IxTER1( (STRCMP( old_version,==,new_version)) , "version equal\n" )
	
	nret = SnprintfAndSystem( cmd , sizeof(cmd) , "mv %s/%s/%s %s/%s/%s" , env->images_path_base , image , old_version , env->images_path_base , image , new_version ) ;
	INTER1( "*** ERROR : SnprintfAndSystem [mv %s/%s/%s %s/%s/%s] failed[%d] , errno[%d]\n" , env->images_path_base , image , old_version , env->images_path_base , image , new_version , nret , errno )
	EIDTI( "modify version ok\n" )
	
	printf( "OK\n" );
	
	return 0;	
}

