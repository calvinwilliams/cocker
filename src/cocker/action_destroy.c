#include "cocker_in.h"

int DoAction_destroy( struct CockerEnvironment *cocker_env )
{
	char		pid_str[ PID_LEN_MAX + 1 ] ;
	pid_t		pid ;
	
	char		container_net_file[ PATH_MAX ] ;
	
	char		cmd[ 4096 ] ;

	char		container_path_base[ PATH_MAX ] ;
	char		rwlayer_path[ PATH_MAX ] ;
	char		hostname_path[ PATH_MAX ] ;
	
	int		nret = 0 ;
	
	/* preprocess input parameters */
	Snprintf( cocker_env->container_path_base , sizeof(cocker_env->container_path_base)-1 , "%s/%s" , cocker_env->containers_path_base , cocker_env->cmd_para.__container_id );
	nret = access( cocker_env->container_path_base , F_OK ) ;
	if( nret == -1 )
	{
		printf( "*** ERROR : container '%s' not found\n" , cocker_env->cmd_para.__container_id );
		return -1;
	}
	
	/* read pid file */
	nret = ReadFileLine( pid_str , sizeof(pid_str)-1 , NULL , -1 , "%s/%s/pid" , cocker_env->containers_path_base , cocker_env->container_id ) ;
	if( nret == 0 )
	{
		pid = atoi(pid_str) ;
		if( pid > 0 )
		{
			nret = kill( pid , 0 ) ;
			if( nret == 0 )
			{
				printf( "*** ERROR : container is already running\n" );
				return 0;
			}
		}
	}
	TrimEnter( pid_str );
	
	/* read net file */
	nret = ReadFileLine( cocker_env->net , sizeof(cocker_env->net) , container_net_file , sizeof(container_net_file) , "%s/net" , cocker_env->container_path_base ) ;
	if( nret < 0 )
	{
		printf( "*** ERROR : ReadFileLine net failed\n" );
		return -1;
	}
	else if( cocker_env->cmd_para.__debug )
	{
		printf( "read file %s ok\n" , container_net_file );
	}
	TrimEnter( cocker_env->net );
	
	/* destroy network */
	if( STRCMP( cocker_env->net , == , "BRIDGE" ) || cocker_env->cmd_para.__forcely )
	{
		GetEthernetName( cocker_env->container_id , cocker_env );
		
		/*
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "iptables -t nat -D POSTROUTING -o %s -j MASQUERADE" , cocker_env->host_eth ) ;
		if( nret )
		{
			printf( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
			if( ! cocker_env->cmd_para.__forcely )
				return -1;
		}
		else if( cocker_env->cmd_para.__debug )
		{
			printf( "system [%s] ok\n" , cmd );
		}
		*/
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ifconfig %s down" , cocker_env->eth_name ) ;
		if( nret )
		{
			printf( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
			if( ! cocker_env->cmd_para.__forcely )
				return -1;
		}
		else if( cocker_env->cmd_para.__debug )
		{
			printf( "system [%s] ok\n" , cmd );
		}
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns exec %s ifconfig %s down" , cocker_env->netns_name , cocker_env->veth_sname ) ;
		if( nret )
		{
			printf( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
			if( ! cocker_env->cmd_para.__forcely )
				return -1;
		}
		else if( cocker_env->cmd_para.__debug )
		{
			printf( "system [%s] ok\n" , cmd );
		}
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "brctl delif %s %s" , cocker_env->netbr_name , cocker_env->eth_name ) ;
		if( nret )
		{
			printf( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
			if( ! cocker_env->cmd_para.__forcely )
				return -1;
		}
		else if( cocker_env->cmd_para.__debug )
		{
			printf( "system [%s] ok\n" , cmd );
		}
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip link del %s" , cocker_env->eth_name ) ;
		if( nret )
		{
			printf( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
			if( ! cocker_env->cmd_para.__forcely )
				return -1;
		}
		else if( cocker_env->cmd_para.__debug )
		{
			printf( "system [%s] ok\n" , cmd );
		}
		
		/*
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns exec %s ip link del %s" , netns_name , veth0_sname ) ;
		if( nret )
		{
			printf( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
		}
		*/
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns del %s" , cocker_env->netns_name ) ;
		if( nret )
		{
			printf( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
			if( ! cocker_env->cmd_para.__forcely )
				return -1;
		}
		else if( cocker_env->cmd_para.__debug )
		{
			printf( "system [%s] ok\n" , cmd );
		}
	}
	else if( STRCMP( cocker_env->net , == , "CUSTOM" ) )
	{
		GetEthernetName( cocker_env->container_id , cocker_env );
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns del %s" , cocker_env->netns_name ) ;
		if( nret )
		{
			printf( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
			if( ! cocker_env->cmd_para.__forcely )
				return -1;
		}
		else if( cocker_env->cmd_para.__debug )
		{
			printf( "system [%s] ok\n" , cmd );
		}
	}
	
	/* destroy container folders and files */
	nret = snprintf( container_path_base , sizeof(container_path_base)-1 , "%s/%s" , cocker_env->containers_path_base , cocker_env->container_id ) ;
	if( SNPRINTF_OVERFLOW( nret , sizeof(container_path_base)-1 ) )
	{
		printf( "snprintf overflow" );
		return -1;
	}
	
	nret = snprintf( rwlayer_path , sizeof(rwlayer_path)-1 , "%s/rwlayer" , container_path_base ) ;
	if( SNPRINTF_OVERFLOW( nret , sizeof(rwlayer_path)-1 ) )
	{
		printf( "snprintf overflow\n" );
		return -1;
	}
	
	nret = snprintf( hostname_path , sizeof(hostname_path)-1 , "%s/hostname" , container_path_base ) ;
	if( SNPRINTF_OVERFLOW( nret , sizeof(hostname_path)-1 ) )
	{
		printf( "snprintf overflow\n" );
		return -1;
	}
	
	if( access( rwlayer_path , W_OK ) == 0 && access( hostname_path , W_OK ) == 0 )
	{
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "rm -rf %s" , container_path_base ) ;
		if( nret )
		{
			printf( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
			if( ! cocker_env->cmd_para.__forcely )
				return -1;
		}
		else if( cocker_env->cmd_para.__debug )
		{
			printf( "system [%s] ok\n" , cmd );
		}
	}
	
	printf( "OK\n" );
	
	return 0;
}

