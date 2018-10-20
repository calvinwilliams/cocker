#include "cocker_in.h"

int DoAction_to_container( struct CockerEnvironment *env )
{
	char		cmd[ 4096 ] ;
	char		image_author_file[ PATH_MAX + 1 ] ;
	char		image_create_datetime_file[ PATH_MAX + 1 ] ;
	char		image_version_file[ PATH_MAX + 1 ] ;

	int		nret = 0 ;
	
	/* preprocess input parameters */
	Snprintf( env->image_path_base , sizeof(env->image_path_base)-1 , "%s/%s" , env->images_path_base , env->cmd_para.__from_image );
	nret = access( env->image_path_base , F_OK ) ;
	I1TER1( "*** ERROR : image '%s' not found\n" , env->cmd_para.__to_image )
	
	Snprintf( env->container_path_base , sizeof(env->container_path_base)-1 , "%s/%s" , env->containers_path_base , env->cmd_para.__to_container );
	nret = access( env->container_path_base , F_OK ) ;
	I0TER1( "*** ERROR : container '%s' exist\n" , env->cmd_para.__to_container )
	
	GetEthernetNames( env , env->cmd_para.__to_container );
	
	/* create container folders and files */
	nret = CreateContainer( env , env->cmd_para.__image_id , env->cmd_para.__to_container ) ;
	INTER1( "*** ERROR : CreateContainer failed[%d] , errno[%d]\n" , nret , errno )
	EIDTE( "CreateContainer %s from %s ok\n" , env->cmd_para.__to_container , env->cmd_para.__from_image )
	
	nret = SnprintfAndSystem( cmd , sizeof(cmd) , "mv -f %s/rlayer/* %s/rwlayer/" , env->image_path_base , env->container_path_base ) ;
	INTER1( "*** ERROR : SnprintfAndSystem [mv -f %s/rlayer/* %s/rwlayer/] failed[%d] , errno[%d]\n" , env->image_path_base , env->container_path_base , nret , errno )
	EIDTE( "system [%s] ok\n" , cmd )
	
	Snprintf( image_author_file , sizeof(image_author_file) , "%s/author" , env->image_path_base );
	nret = access( image_author_file , F_OK ) ;
	if( nret == 0 )
	{
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "mv %s %s/" , image_author_file , env->container_path_base ) ;
		INTER1( "*** ERROR : SnprintfAndSystem [mv %s %s/] failed[%d] , errno[%d]\n" , image_author_file , env->container_path_base , nret , errno )
		EIDTE( "system [%s] ok\n" , cmd )
	}
	
	Snprintf( image_create_datetime_file , sizeof(image_create_datetime_file) , "%s/create_datetime" , env->image_path_base );
	nret = access( image_create_datetime_file , F_OK ) ;
	if( nret == 0 )
	{
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "mv %s %s/" , image_create_datetime_file , env->container_path_base ) ;
		INTER1( "*** ERROR : SnprintfAndSystem [mv %s %s/] failed[%d] , errno[%d]\n" , image_create_datetime_file , env->container_path_base , nret , errno )
		EIDTE( "system [%s] ok\n" , cmd )
	}
	
	Snprintf( image_version_file , sizeof(image_version_file) , "%s/version" , env->image_path_base );
	nret = access( image_version_file , F_OK ) ;
	if( nret == 0 )
	{
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "mv %s %s/" , image_version_file , env->container_path_base ) ;
		INTER1( "*** ERROR : SnprintfAndSystem [mv %s %s/] failed[%d] , errno[%d]\n" , image_version_file , env->container_path_base , nret , errno )
		EIDTE( "system [%s] ok\n" , cmd )
	}
	
	nret = SnprintfAndSystem( cmd , sizeof(cmd) , "rm -rf %s" , env->image_path_base ) ;
	INTER1( "*** ERROR : SnprintfAndSystem [rm -rf %s] failed[%d] , errno[%d]\n" , env->image_path_base , nret , errno )
	EIDTE( "system [%s] ok\n" , cmd )
	
	printf( "OK\n" );
	
	return 0;
}

