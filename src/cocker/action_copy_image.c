#include "cocker_in.h"

int DoAction_copy_image( struct CockerEnvironment *env )
{
	char		from_image_path_base[ PATH_MAX + 1 ] ;
	char		to_image_path_base[ PATH_MAX + 1 ] ;
	char		cmd[ 4096 ] ;

	int		nret = 0 ;
	
	/* preprocess input parameters */
	Snprintf( from_image_path_base , sizeof(from_image_path_base)-1 , "%s/%s" , env->images_path_base , env->cmd_para.__from_image );
	nret = access( from_image_path_base , F_OK ) ;
	I1TER1( "*** ERROR : image '%s' not found\n" , env->cmd_para.__from_image )
	
	Snprintf( to_image_path_base , sizeof(to_image_path_base)-1 , "%s/%s" , env->images_path_base , env->cmd_para.__to_image );
	nret = access( to_image_path_base , F_OK ) ;
	I0TER1( "*** ERROR : image '%s' exist\n" , env->cmd_para.__to_image )
	
	/* copy image folders and files */
	nret = SnprintfAndSystem( cmd , sizeof(cmd) , "cp -rf %s %s" , from_image_path_base , to_image_path_base ) ;
	INTER1( "*** ERROR : SnprintfAndSystem [cp -rf %s %s] failed[%d] , errno[%d]\n" , from_image_path_base , to_image_path_base , nret , errno )
	EIDTI( "system [%s] ok\n" , cmd )
	
	printf( "OK\n" );
	
	return 0;
}

