/*
 * cocker - Container Machine Engine
 * author	: calvin
 * email	: calvinwilliams@163.com
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#include "cocker_in.h"

#define WAIT_FOR_SHUTDOWN_COUNT		5

int DoAction_destroy( struct CockerEnvironment *env )
{
	int		i ;
	char		pid_str[ PID_LEN_MAX + 1 ] ;
	pid_t		pid ;
	char		net[ NET_LEN_MAX + 1 ] ;
	char		container_net_file[ PATH_MAX + 1 ] ;
	char		container_pid_file[ PATH_MAX + 1 ] ;
	char		container_rwlayer_path_base[ PATH_MAX + 1 ] ;
	char		cmd[ 4096 ] ;

	int		nret = 0 ;
	
	/* preprocess input parameters */
	Snprintf( env->container_path_base , sizeof(env->container_path_base)-1 , "%s/%s" , env->containers_path_base , env->cmd_para.__container_id );
	nret = access( env->container_path_base , F_OK ) ;
	I1TER1( "*** ERROR : container '%s' not found\n" , env->cmd_para.__container_id )
	
	GetEthernetNames( env , env->cmd_para.__container_id );
	
	/* if --shutdown optional */
	if( env->cmd_para.__shutdown )
	{
		nret = _DoAction_kill( env , SIGTERM ) ;
		if( nret )
		{
			if( env->cmd_para.__forcely == NULL )
				return -1;
		}
		
		Snprintf( container_pid_file , sizeof(container_pid_file) , "%s/%s/pid" , env->containers_path_base , env->cmd_para.__container_id ) ;
		for( i = 0 ; i < WAIT_FOR_SHUTDOWN_COUNT ; i++ )
		{
			nret = access( container_pid_file , F_OK ) ;
			if( nret == -1 )
				break;
			
			sleep(1);
		}
		if( i >= WAIT_FOR_SHUTDOWN_COUNT )
		ER1( "*** ERROR : wait for shutdown failed\n" )
	}
	
	/* read pid file */
	memset( pid_str , 0x00 , sizeof(pid_str) );
	memset( container_pid_file , 0x00 , sizeof(container_pid_file) );
	nret = ReadFileLine( pid_str , sizeof(pid_str)-1 , container_pid_file , sizeof(container_pid_file) , "%s/%s/pid" , env->containers_path_base , env->cmd_para.__container_id ) ;
	if( nret == 0 )
	{
		TrimEnter( pid_str );
		
		pid = atoi(pid_str) ;
		if( pid > 0 )
		{
			nret = kill( pid , 0 ) ;
			I0TER1( "*** ERROR : container is already running\n" )
		}
	}
	
	Snprintf( container_rwlayer_path_base , sizeof(container_rwlayer_path_base)-1 , "%s/rwlayer" , env->container_path_base );
	nret = access( container_rwlayer_path_base , F_OK ) ;
	I1TER1( "*** ERROR : container '%s' invalid\n" , env->cmd_para.__container_id )
	
	/* clean container resouce */
	if( env->cmd_para.__forcely )
	{
		CleanContainerResource( env );
	}
	
	/* read net file */
	memset( net , 0x00 , sizeof(net) );
	memset( container_net_file , 0x00 , sizeof(container_net_file) );
	nret = ReadFileLine( net , sizeof(net) , container_net_file , sizeof(container_net_file) , "%s/net" , env->container_path_base ) ;
	ILTER1( "*** ERROR : ReadFileLine net failed\n" )
	EIDTI( "read file net ok\n" )
	
	TrimEnter( net );
	
	/* destroy network-namespace */
	if( STRCMP( net , == , "BRIDGE" ) || env->cmd_para.__forcely )
	{
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns del %s" , env->netns_name ) ;
		INTEFR1( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
		EIDTI( "system [%s] ok\n" , cmd )
	}
	else if( STRCMP( net , == , "CUSTOM" ) )
	{
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns del %s" , env->netns_name ) ;
		INTEFR1( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
		EIDTI( "system [%s] ok\n" , cmd )
	}
	
	/* destroy container folders and files */
	nret = SnprintfAndSystem( cmd , sizeof(cmd) , "rm -rf %s" , env->container_path_base ) ;
	INTEFR1( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
	EIDTI( "system [%s] ok\n" , cmd )
	
	printf( "OK\n" );
	
	return 0;
}

