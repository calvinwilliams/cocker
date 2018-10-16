#include "cocker_in.h"

int DoAction_to_image( struct CockerEnvironment *env )
{
	char		container_pid_file[ PATH_MAX ] ;
	char		pid_str[ PID_LEN_MAX + 1 ] ;
	pid_t		pid ;
	
	char		cmd[ 4096 ] ;

	int		nret = 0 ;
	
	/* preprocess input parameters */
	Snprintf( env->container_path_base , sizeof(env->container_path_base)-1 , "%s/%s" , env->containers_path_base , env->cmd_para.__from_container );
	nret = access( env->container_path_base , F_OK ) ;
	I1TER1( "*** ERROR : container '%s' not found\n" , env->cmd_para.__from_container )
	
	Snprintf( env->image_path_base , sizeof(env->image_path_base)-1 , "%s/%s" , env->images_path_base , env->cmd_para.__to_image );
	nret = access( env->container_path_base , F_OK ) ;
	I0TER1( "*** ERROR : image '%s' exist\n" , env->cmd_para.__to_image )
	
	/* read pid file */
	nret = ReadFileLine( pid_str , sizeof(pid_str)-1 , container_pid_file , sizeof(container_pid_file) , "%s/%s/pid" , env->containers_path_base , env->cmd_para.__from_container ) ;
	if( nret == 0 )
	{
		pid = atoi(pid_str) ;
		if( pid > 0 )
		{
			nret = kill( pid , 0 ) ;
			I0TER1( "*** ERROR : container is already running\n" )
		}
	}
	
	/* create image */
	nret = SnprintfAndMakeDir( env->image_path_base , sizeof(env->image_path_base)-1 , "%s/%s" , env->images_path_base , env->cmd_para.__to_image ) ;
	INTER1( "*** ERROR : SnprintfAndMakeDir image_path_base failed[%d] , errno[%d]\n" , nret , errno )
	EIDTE( "mkdir %s ok\n" , env->image_path_base )
	
	nret = SnprintfAndSystem( cmd , sizeof(cmd) , "cp -rf %s/rwlayer/* %s/rlayer/" , env->container_path_base , env->image_path_base ) ;
	INTER1( "*** ERROR : SnprintfAndSystem [cp -rf %s/rwlayer/* %s/rlayer/] failed[%d] , errno[%d]\n" , env->container_path_base , env->image_path_base , nret , errno )
	EIDTE( "system [%s] ok\n" , cmd )
	
	nret = SnprintfAndSystem( cmd , sizeof(cmd) , "rm -rf %s" , env->container_path_base ) ;
	INTER1( "*** ERROR : SnprintfAndSystem [rm -rf %s] failed[%d] , errno[%d]\n" , env->container_path_base , nret , errno )
	EIDTE( "system [%s] ok\n" , cmd )
	
	printf( "OK\n" );
	
	return 0;
}

