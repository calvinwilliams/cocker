#include "cocker_in.h"

int DoAction_install_test( struct CockerEnvironment *cocker_env )
{
	char		image_path_base[ PATH_MAX ] ;
	char		image_rlayer_path[ PATH_MAX ] ;
	char		cmd[ 4096 ] ;
	
	int		nret = 0 ;
	
	/* create image */
	nret = SnprintfAndMakeDir( image_path_base , sizeof(image_path_base)-1 , "%s/test" , cocker_env->images_path_base ) ;
	if( nret )
	{
		printf( "*** ERROR : SnprintfAndMakeDir image_path_base failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	else if( cocker_env->cmd_para.__debug )
	{
		printf( "mkdir %s ok\n" , image_path_base );
	}
	
	nret = SnprintfAndMakeDir( image_rlayer_path , sizeof(image_rlayer_path)-1 , "%s/rlayer" , image_path_base ) ;
	if( nret )
	{
		printf( "*** ERROR : SnprintfAndMakeDir rlayer failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	else if( cocker_env->cmd_para.__debug )
	{
		printf( "mkdir %s ok\n" , image_rlayer_path );
	}
	
	nret = SnprintfAndSystem( cmd , sizeof(cmd) , "cocker_install_test.sh %s" , image_rlayer_path ) ;
	if( nret )
	{
		printf( "*** ERROR : SnprintfAndSystem [cocker_install_test.sh %s] failed[%d] , errno[%d]\n" , image_path_base , nret , errno );
		return -1;
	}
	else if( cocker_env->cmd_para.__debug )
	{
		printf( "system [%s] ok\n" , cmd );
	}
	
	printf( "OK\n" );
	
	return 0;
}

