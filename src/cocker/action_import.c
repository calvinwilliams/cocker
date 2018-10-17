#include "cocker_in.h"

int DoAction_import( struct CockerEnvironment *env )
{
	int		image_file_len ;
	char		image_id[ IMAGE_ID_LEN_MAX + 1 ] ;
	char		image_rlayer_path_base[ PATH_MAX + 1 ] ;
	char		current_path[ PATH_MAX + 1 ] ;
	char		cmd[ 4096 ] ;

	int		nret = 0 ;
	
	/* preprocess input parameters */
	if( env->cmd_para.__image_id == NULL )
	{
		image_file_len = strlen(env->cmd_para.__image_file) ;
		IxTER1( (image_file_len < sizeof(COCKERIMAGE_FILE_EXTNAME)-1) , "image file[%s] too small\n" , env->cmd_para.__image_file )
		IxTER1( (STRCMP(env->cmd_para.__image_file+image_file_len-(sizeof(COCKERIMAGE_FILE_EXTNAME)-1),!=,COCKERIMAGE_FILE_EXTNAME)) , "image file name[%s] invalid\n" , env->cmd_para.__image_file )
		
		memset( image_id , 0x00 , sizeof(image_id) );
		strncpy( image_id , env->cmd_para.__image_file , MIN(image_file_len-1-(sizeof(COCKERIMAGE_FILE_EXTNAME)-1),sizeof(image_id)-1) );
	}
	else
	{
		memset( image_id , 0x00 , sizeof(image_id) );
		strncpy( image_id , env->cmd_para.__image_id , sizeof(image_id)-1 );
	}
	
	Snprintf( env->image_path_base , sizeof(env->image_path_base)-1 , "%s/%s" , env->images_path_base , image_id );
	nret = access( env->image_path_base , F_OK ) ;
	I0TER1( "*** ERROR : image '%s' exist\n" , image_id )
	
	/* pack image folders and files */
	nret = SnprintfAndMakeDir( env->image_path_base , sizeof(env->image_path_base)-1 , "%s/%s" , env->images_path_base , image_id ) ;
	INTER1( "*** ERROR : SnprintfAndMakeDir / failed[%d] , errno[%d]\n" , nret , errno )
	EIDTI( "mkdir %s ok\n" , env->image_path_base )
	
	nret = SnprintfAndMakeDir( image_rlayer_path_base , sizeof(image_rlayer_path_base)-1 , "%s/rlayer" , env->image_path_base ) ;
	INTER1( "*** ERROR : SnprintfAndMakeDir /rlayer failed[%d] , errno[%d]\n" , nret , errno )
	EIDTI( "mkdir %s ok\n" , image_rlayer_path_base )
	
	memset( current_path , 0x00 , sizeof(current_path) );
	getcwd( current_path , sizeof(current_path) );
	
	nret = SnprintfAndSystem( cmd , sizeof(cmd) , "cd %s/rlayer/ && tar xzf %s/%s" , env->image_path_base , current_path , env->cmd_para.__image_file ) ;
	INTER1( "*** ERROR : SnprintfAndSystem [cd %s/rlayer/ && tar xzf %s/%s] failed[%d] , errno[%d]\n" , env->image_path_base , current_path , env->cmd_para.__image_file , nret , errno )
	EIDTI( "system [%s] ok\n" , cmd )
	
	printf( "OK\n" );
	
	return 0;
}

