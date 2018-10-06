#include "cocker_in.h"

int DoAction_destroy( struct CockerEnvironment *env )
{
	char		pid_str[ PID_LEN_MAX + 1 ] ;
	pid_t		pid ;
	
	char		container_net_file[ PATH_MAX ] ;
	
	char		cmd[ 4096 ] ;

	int		nret = 0 ;
	
	/* preprocess input parameters */
	Snprintf( env->container_path_base , sizeof(env->container_path_base)-1 , "%s/%s" , env->containers_path_base , env->container_id );
	nret = access( env->container_path_base , F_OK ) ;
	I1TER1( "*** ERROR : container '%s' not found\n" , env->cmd_para.__container_id )
	
	/* read pid file */
	nret = ReadFileLine( pid_str , sizeof(pid_str)-1 , NULL , -1 , "%s/%s/pid" , env->containers_path_base , env->container_id ) ;
	if( nret == 0 )
	{
		pid = atoi(pid_str) ;
		if( pid > 0 )
		{
			nret = kill( pid , 0 ) ;
			I0TER1( "*** ERROR : container is already running\n" )
		}
	}
	
	TrimEnter( pid_str );
	
	/* clean container resouce */
	if( env->cmd_para.__forcely )
	{
		CleanContainerResource( env );
	}
	
	/* read net file */
	nret = ReadFileLine( env->net , sizeof(env->net) , container_net_file , sizeof(container_net_file) , "%s/net" , env->container_path_base ) ;
	ILTER1( "*** ERROR : ReadFileLine net failed\n" )
	EIDTE( "read file %s ok\n" , container_net_file )
	
	TrimEnter( env->net );
	
	/* destroy network-namespace */
	if( STRCMP( env->net , == , "BRIDGE" ) || env->cmd_para.__forcely )
	{
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns del %s" , env->netns_name ) ;
		INTEFR1( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
		EIDTE( "system [%s] ok\n" , cmd )
	}
	else if( STRCMP( env->net , == , "CUSTOM" ) )
	{
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns del %s" , env->netns_name ) ;
		INTEFR1( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
		EIDTE( "system [%s] ok\n" , cmd )
	}
	
	/* destroy container folders and files */
	nret = SnprintfAndSystem( cmd , sizeof(cmd) , "rm -rf %s" , env->container_path_base ) ;
	INTEFR1( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
	EIDTE( "system [%s] ok\n" , cmd )
	
	printf( "OK\n" );
	
	return 0;
}

