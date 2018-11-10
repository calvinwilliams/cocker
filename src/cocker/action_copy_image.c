/*
 * cocker - Container Machine Engine
 * author	: calvin
 * email	: calvinwilliams@163.com
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#include "cocker_in.h"

int DoAction_copy_image( struct CockerEnvironment *env )
{
	char		from_image[ IMAGES_ID_LEN_MAX + 1 ] ;
	char		from_version[ PATH_MAX + 1 ] ;
	char		to_image[ IMAGES_ID_LEN_MAX + 1 ] ;
	char		to_version[ PATH_MAX + 1 ] ;
	char		*p = NULL ;
	char		*p2 = NULL ;
	char		from_image_path_base[ PATH_MAX + 1 ] ;
	char		to_image_path_base[ PATH_MAX + 1 ] ;
	char		cmd[ 4096 ] ;
	
	int		nret = 0 ;
	
	/* preprocess input parameters */
	if( env->cmd_para.__from_image )
	{
		memset( from_image , 0x00 , sizeof(from_image) );
		strncpy( from_image , env->cmd_para.__from_image , sizeof(from_image)-1 );
	}
	p = from_image ;
	p2 = strchr( p , ':' ) ;
	memset( from_version , 0x00 , sizeof(from_version) );
	if( p2 )
	{
		strncpy( from_version , p2+1 , sizeof(from_version)-1 );
		(*p2)= '\0' ;
	}
	if( from_version[0] == '\0' )
	{
		strcpy( from_version , "_" );
	}
	
	Snprintf( from_image_path_base , sizeof(from_image_path_base)-1 , "%s/%s/%s" , env->images_path_base , from_image , from_version );
	nret = access( from_image_path_base , F_OK ) ;
	I1TER1( "*** ERROR : image '%s' not found\n" , env->cmd_para.__from_image )
	
	if( env->cmd_para.__to_image )
	{
		memset( to_image , 0x00 , sizeof(to_image) );
		strncpy( to_image , env->cmd_para.__to_image , sizeof(to_image)-1 );
	}
	p = to_image ;
	p2 = strchr( p , ':' ) ;
	memset( to_version , 0x00 , sizeof(to_version) );
	if( p2 )
	{
		strncpy( to_version , p2+1 , sizeof(to_version)-1 );
		(*p2)= '\0' ;
	}
	if( to_version[0] == '\0' )
	{
		strcpy( to_version , "_" );
	}
	
	Snprintf( to_image_path_base , sizeof(to_image_path_base)-1 , "%s/%s/%s" , env->images_path_base , to_image , to_version );
	nret = access( to_image_path_base , F_OK ) ;
	I0TER1( "*** ERROR : image '%s' exist\n" , env->cmd_para.__to_image )
	
	/* copy image folders and files */
	SnprintfAndMakeDir( NULL , -1 , "%s/%s" , env->images_path_base , to_image );
	
	nret = SnprintfAndSystem( cmd , sizeof(cmd) , "cp -rf %s %s" , from_image_path_base , to_image_path_base ) ;
	INTER1( "*** ERROR : SnprintfAndSystem [cp -rf %s %s] failed[%d] , errno[%d]\n" , from_image_path_base , to_image_path_base , nret , errno )
	EIDTI( "system [%s] ok\n" , cmd )
	
	printf( "OK\n" );
	
	return 0;
}

