/*
 * cocker - Container Machine Engine
 * author	: calvin
 * email	: calvinwilliams@163.com
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#include "cocker_in.h"

int DoAction_spush( struct CockerEnvironment *env )
{
	char		image[ IMAGES_ID_LEN_MAX + 1 ] ;
	char		version[ PATH_MAX + 1 ] ;
	char		image_file[ PATH_MAX + 1 ] ;
	char		current_path[ PATH_MAX + 1 ] ;
	char		cmd[ 4096 ] ;
	char		repo[ REPO_LEN_MAX + 1 ] ;
	char		repo_file[ PATH_MAX + 1 ] ;

	int		nret = 0 ;
	int		nret2 = 0 ;
	
	/* preprocess input parameters */
	Snprintf( image , sizeof(image) , "%s" , env->cmd_para.__image );
	SplitImageVersion( image , version , sizeof(version) );
	
	Snprintf( image_file , sizeof(image_file) , "%s%s%s.cockerimage" , image , (version[0]=='_'?"":":") , version );
	
	Snprintf( env->image_path_base , sizeof(env->image_path_base)-1 , "%s/%s/%s" , env->images_path_base , image , version );
	nret = access( env->image_path_base , F_OK ) ;
	I1TER1( "*** ERROR : image '%s' not found\n" , env->cmd_para.__image )
	
	memset( repo , 0x00 , sizeof(repo) );
	memset( repo_file , 0x00 , sizeof(repo_file) );
	nret = ReadFileLine( repo , sizeof(repo) , repo_file , sizeof(repo_file) , "%s/repo" , env->cocker_home ) ;
	I1TER1( "read %s failed\n" , repo_file )
	EIDTI( "read %s ok\n" , repo_file )
	
	/* pack and upload image */
	nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ssh %s \"ls ./%s\" >/dev/null 2>/dev/null" , repo , image_file ) ;
	if( nret == 0 )
	{
		E( "*** ERROR : image '%s' exist in repo\n" , image_file );
		if( ! env->cmd_para.__forcely )
			return nret;
	}
	
	memset( current_path , 0x00 , sizeof(current_path) );
	getcwd( current_path , sizeof(current_path) );
	
	nret = SnprintfAndSystem( cmd , sizeof(cmd) , "cd %s/rlayer/ && tar --force-local -czf %s * && mv %s %s/" , env->image_path_base , image_file , image_file , current_path ) ;
	INTER1( "*** ERROR : SnprintfAndSystem [cd %s/rlayer/ && tar --force-local -czf %s * && mv %s %s/] failed[%d] , errno[%d]\n" , env->image_path_base , image_file , image_file , current_path , nret , errno )
	EIDTI( "system [%s] ok\n" , cmd )
	
	if( env->cmd_para.__debug )
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "scp ./%s %s:~/" , image_file , repo ) ;
	else
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "scp ./%s %s:~/ >/dev/null 2>/dev/null" , image_file , repo ) ;
	INTEx( {if(nret2==0)nret2=nret;} , "*** ERROR : SnprintfAndSystem [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
	EIDTI( "system [%s] ok\n" , cmd )
	
	nret = SnprintfAndSystem( cmd , sizeof(cmd) , "rm %s" , image_file ) ;
	INTER1( "*** ERROR : SnprintfAndSystem [rm %s] failed[%d] , errno[%d]\n" , image_file , nret , errno )
	EIDTI( "system [%s] ok\n" , cmd )
	
	printf( "OK\n" );
	
	if( nret2 )
		return nret2;
	else
		return 0;
}

