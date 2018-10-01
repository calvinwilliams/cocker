#include "cocker_in.h"

int DoAction_install_test( struct CockerEnvironment *env )
{
	char		image_path_base[ PATH_MAX ] ;
	char		image_rlayer_path[ PATH_MAX ] ;
	char		cmd[ 4096 ] ;
	
	int		nret = 0 ;
	
	/* create image */
	nret = SnprintfAndMakeDir( image_path_base , sizeof(image_path_base)-1 , "%s/test" , env->images_path_base ) ;
	INTPR1( "*** ERROR : SnprintfAndMakeDir image_path_base failed[%d] , errno[%d]\n" , nret , errno )
	EIDTP( "mkdir %s ok\n" , image_path_base )
	
	nret = SnprintfAndMakeDir( image_rlayer_path , sizeof(image_rlayer_path)-1 , "%s/rlayer" , image_path_base ) ;
	INTPR1( "*** ERROR : SnprintfAndMakeDir rlayer failed[%d] , errno[%d]\n" , nret , errno )
	EIDTP( "mkdir %s ok\n" , image_rlayer_path )
	
	nret = SnprintfAndSystem( cmd , sizeof(cmd) , "cocker_install_test.sh %s" , image_rlayer_path ) ;
	INTPR1( "*** ERROR : SnprintfAndSystem [cocker_install_test.sh %s] failed[%d] , errno[%d]\n" , image_path_base , nret , errno )
	EIDTP( "system [%s] ok\n" , cmd )
	
	printf( "OK\n" );
	
	return 0;
}

