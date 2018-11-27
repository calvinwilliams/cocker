/*
 * cocker - Container Machine Engine
 * author	: calvin
 * email	: calvinwilliams@163.com
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#include "cocker_in.h"

int DoAction_to_container( struct CockerEnvironment *env )
{
	char			image[ IMAGES_ID_LEN_MAX + 1 ] ;
	char			version[ PATH_MAX + 1 ] ;
	char			*p = NULL ;
	char			*p2 = NULL ;
	char			cmd[ 4096 ] ;
	char			version_path_base[ PATH_MAX + 1 ] ;
	
	int			nret = 0 ;
	
	/* preprocess input parameters */
	memset( image , 0x00 , sizeof(image) );
	strncpy( image , env->cmd_para.__from_image , sizeof(image)-1 );
	
	p = image ;
	p2 = strchr( p , ':' ) ;
	if( p2 == NULL )
	{
		Snprintf( version_path_base , sizeof(version_path_base) , "%s/%s" , env->images_path_base , p ) ;
		nret = GetMaxVersionPath( version_path_base , version , sizeof(version) ) ;
		INTER1( "*** ERROR : GetMaxVersionPath[%s] failed[%d]\n" , version_path_base , nret )
		
		nret = SnprintfAndCheckDir( NULL , -1 , "%s/%s/%s/rlayer" , env->images_path_base , p , version ) ;
	}
	else
	{
		strcpy( version , "_" );
		(*p2) = '0' ;
		nret = SnprintfAndCheckDir( NULL , -1 , "%s/%.*s/%s/rlayer" , env->images_path_base , (int)(p2-p) , p , p2+1 ) ;
	}
	INTER1( "*** ERROR : image[%s] not found\n" , image )
	
	Snprintf( version_path_base , sizeof(version_path_base) , "%s/%s" , env->images_path_base , image );
	
	Snprintf( env->container_path_base , sizeof(env->container_path_base)-1 , "%s/%s" , env->containers_path_base , env->cmd_para.__to_container );
	nret = access( env->container_path_base , F_OK ) ;
	I0TER1( "*** ERROR : container '%s' exist\n" , env->cmd_para.__to_container )
	
	GetEthernetNames( env , env->cmd_para.__to_container );
	
	/* create container folders and files */
	nret = CreateContainer( env , env->cmd_para.__image , env->cmd_para.__to_container ) ;
	INTER1( "*** ERROR : CreateContainer failed[%d] , errno[%d]\n" , nret , errno )
	EIDTI( "CreateContainer %s from %s ok\n" , env->cmd_para.__to_container , env->cmd_para.__from_image )
	
	nret = SnprintfAndSystem( cmd , sizeof(cmd) , "mv -f %s/%s/rlayer/* %s/rwlayer/" , version_path_base , version , env->container_path_base ) ;
	INTER1( "*** ERROR : SnprintfAndSystem [mv -f %s/%s/rlayer/* %s/rwlayer/] failed[%d] , errno[%d]\n" , version_path_base , version , env->container_path_base , nret , errno )
	EIDTI( "system [%s] ok\n" , cmd )
	
	nret = WriteFileLine( version , NULL , -1 , "%s/version" , env->container_path_base ) ;
	INTER1( "*** ERROR : WriteFileLine version failed[%d] , errno[%d]\n" , nret , errno )
	EIDTI( "write file %s ok\n" , "version" )
	
	nret = SnprintfAndSystem( cmd , sizeof(cmd) , "rm -rf %s/%s" , version_path_base , version ) ;
	INTER1( "*** ERROR : SnprintfAndSystem [rm -rf %s/%s] failed[%d] , errno[%d]\n" , version_path_base , version , nret , errno )
	EIDTI( "system [%s] ok\n" , cmd )
	
	nret = IsDirectoryEmpty( version_path_base ) ;
	if( nret == 0 )
	{
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "rmdir %s" , version_path_base ) ;
		INTER1( "*** ERROR : SnprintfAndSystem [rmdir %s] failed[%d] , errno[%d]\n" , version_path_base , nret , errno )
		EIDTI( "system [%s] ok\n" , cmd )
	}
	
	printf( "OK\n" );
	
	return 0;
}

