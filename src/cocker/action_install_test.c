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
	char		image_rlayer_path[ PATH_MAX + 1 ] ;
	char		image_author_file[ PATH_MAX + 1 ] ;
	time_t		now_tt ;
	struct tm	now_tm ;
	char		time_str[ CREATE_DATATIME_LEN_MAX ] ;
	char		image_create_datetime_file[ PATH_MAX + 1 ] ;
	char		image_version_file[ PATH_MAX + 1 ] ;
	char		cmd[ 4096 ] ;
	
	int		nret = 0 ;
	
	/* preprocess input parameters */
	nret = SnprintfAndMakeDir( env->image_path_base , sizeof(env->image_path_base)-1 , "%s/test" , env->images_path_base ) ;
	INTER1( "*** ERROR : SnprintfAndMakeDir image_path_base failed[%d] , errno[%d]\n" , nret , errno )
	EIDTI( "mkdir %s ok\n" , env->image_path_base )
	
	/* create image */
	nret = SnprintfAndMakeDir( image_rlayer_path , sizeof(image_rlayer_path)-1 , "%s/rlayer" , env->image_path_base ) ;
	INTER1( "*** ERROR : SnprintfAndMakeDir rlayer failed[%d] , errno[%d]\n" , nret , errno )
	EIDTI( "mkdir %s ok\n" , image_rlayer_path )
	
	nret = SnprintfAndSystem( cmd , sizeof(cmd) , "cocker_install_test.sh %s" , image_rlayer_path ) ;
	INTER1( "*** ERROR : SnprintfAndSystem [cocker_install_test.sh %s] failed[%d] , errno[%d]\n" , env->image_path_base , nret , errno )
	EIDTI( "system [%s] ok\n" , cmd )
	
	if( env->cmd_para.__author )
	{
		nret = WriteFileLine( env->cmd_para.__author , image_author_file , sizeof(image_author_file)-1 , "%s/author" , env->image_path_base ) ;
		INTER1( "*** ERROR : WriteFileLine author failed[%d] , errno[%d]\n" , nret , errno )
		EIDTI( "WriteFileLine %s ok\n" , image_author_file )
	}
	
	time( & now_tt );
	localtime_r( & now_tt , & now_tm );
	memset( time_str , 0x00 , sizeof(time_str) );
	strftime( time_str , sizeof(time_str) , "%Y-%m-%dT%H:%M:%S" , & now_tm ) ;
	nret = WriteFileLine( time_str , image_create_datetime_file , sizeof(image_create_datetime_file)-1 , "%s/create_datetime" , env->image_path_base ) ;
	INTER1( "*** ERROR : WriteFileLine create_datetime failed[%d] , errno[%d]\n" , nret , errno )
	EIDTI( "WriteFileLine %s ok\n" , image_create_datetime_file )
	
	if( env->cmd_para.__version )
	{
		nret = WriteFileLine( env->cmd_para.__version , image_version_file , sizeof(image_version_file)-1 , "%s/version" , env->image_path_base ) ;
		INTER1( "*** ERROR : WriteFileLine version failed[%d] , errno[%d]\n" , nret , errno )
		EIDTI( "WriteFileLine %s ok\n" , image_version_file )
	}
	
	printf( "OK\n" );
	
	return 0;
}

