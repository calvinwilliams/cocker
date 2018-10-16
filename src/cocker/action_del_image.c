#include "cocker_in.h"

int DoAction_del_image( struct CockerEnvironment *env )
{
	char		image_rlayer_path_base[ PATH_MAX + 1 ] ;
	char		cmd[ 4096 ] ;

	int		nret = 0 ;
	
	/* preprocess input parameters */
	Snprintf( env->image_path_base , sizeof(env->image_path_base)-1 , "%s/%s" , env->images_path_base , env->cmd_para.__image_id );
	nret = access( env->image_path_base , F_OK ) ;
	I1TER1( "*** ERROR : image '%s' not found\n" , env->cmd_para.__image_id )
	
	/* del image folders and files */
	Snprintf( image_rlayer_path_base , sizeof(image_rlayer_path_base)-1 , "%s/rlayer" , env->image_path_base );
	nret = access( image_rlayer_path_base , F_OK ) ;
	I1TER1( "*** ERROR : image '%s' invalid\n" , env->cmd_para.__image_id )
	
	nret = SnprintfAndSystem( cmd , sizeof(cmd) , "rm -rf %s" , env->image_path_base ) ;
	INTER1( "*** ERROR : SnprintfAndSystem [rm -rf %s] failed[%d] , errno[%d]\n" , env->image_path_base , nret , errno )
	EIDTI( "system [%s] ok\n" , cmd )
	
	printf( "OK\n" );
	
	return 0;
}

