#include "cocker_in.h"

int DoAction_create( struct CockerEnvironment *cocker_env )
{
	char		cmd[ 4096 ] ;
	
	char		pid_str[ PID_LEN_MAX + 1 ] ;
	
	char		container_path_base[ PATH_MAX ] ;
	char		container_rwlayer_path[ PATH_MAX ] ;
	char		container_rwlayer_etc_path[ PATH_MAX ] ;
	char		container_merged_path[ PATH_MAX ] ;
	char		container_workdir_path[ PATH_MAX ] ;
	char		container_images_file[ PATH_MAX ] ;
	char		container_hostname_file[ PATH_MAX ] ;
	char		container_rwlayer_net_file[ PATH_MAX ] ;
	char		container_rwlayer_netns_file[ PATH_MAX ] ;
	
	int		nret = 0 ;
	
	/* preprocess input parameters */
	if( cocker_env->cmd_para.__container_id == NULL )
	{
		memset( cocker_env->container_id , 0x00 , sizeof(cocker_env->container_id) );
		GenerateContainerId( cocker_env->cmd_para.__image_id , cocker_env->container_id );
	}
	else
	{
		memset( cocker_env->container_id , 0x00 , sizeof(cocker_env->container_id) );
		strncpy( cocker_env->container_id , cocker_env->cmd_para.__container_id , sizeof(cocker_env->container_id)-1 );
	}
	
	if( cocker_env->cmd_para.__host_name == NULL )
	{
		cocker_env->cmd_para.__host_name = cocker_env->container_id ;
	}
	
	/* read pid file */
	nret = ReadFileLine( pid_str , sizeof(pid_str)-1 , NULL , -1 , "%s/%s/pid" , cocker_env->containers_path_base , cocker_env->container_id ) ;
	if( nret == 0 )
	{
		printf( "*** ERROR : container is already running\n" );
		return 0;
	}
	TrimEnter( pid_str );
	
	/* create container folders and files */
	nret = SnprintfAndMakeDir( container_path_base , sizeof(container_path_base)-1 , "%s/%s" , cocker_env->containers_path_base , cocker_env->container_id ) ;
	if( nret )
	{
		printf( "*** ERROR : SnprintfAndMakeDir / failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	else if( cocker_env->cmd_para.__debug )
	{
		printf( "mkdir %s ok\n" , container_path_base );
	}
	
	nret = SnprintfAndMakeDir( container_rwlayer_path , sizeof(container_rwlayer_path) , "%s/rwlayer" , container_path_base ) ;
	if( nret )
	{
		printf( "*** ERROR : SnprintfAndMakeDir rwlayer failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	else if( cocker_env->cmd_para.__debug )
	{
		printf( "mkdir %s ok\n" , container_rwlayer_path );
	}
	
	nret = SnprintfAndMakeDir( container_rwlayer_etc_path , sizeof(container_rwlayer_etc_path) , "%s/rwlayer/etc" , container_path_base ) ;
	if( nret )
	{
		printf( "*** ERROR : SnprintfAndMakeDir /etc failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	else if( cocker_env->cmd_para.__debug )
	{
		printf( "mkdir %s ok\n" , container_rwlayer_etc_path );
	}
	
	nret = SnprintfAndMakeDir( container_merged_path , sizeof(container_merged_path) , "%s/merged" , container_path_base ) ;
	if( nret )
	{
		printf( "*** ERROR : SnprintfAndMakeDir merged failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	else if( cocker_env->cmd_para.__debug )
	{
		printf( "mkdir %s ok\n" , container_merged_path );
	}
	
	nret = SnprintfAndMakeDir( container_workdir_path , sizeof(container_workdir_path) , "%s/workdir" , container_path_base ) ;
	if( nret )
	{
		printf( "*** ERROR : SnprintfAndMakeDir workdir failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	else if( cocker_env->cmd_para.__debug )
	{
		printf( "mkdir %s ok\n" , container_workdir_path );
	}
	
	if( cocker_env->cmd_para.__image_id )
	{
		nret = WriteFileLine( cocker_env->cmd_para.__image_id , container_images_file , sizeof(container_images_file) , "%s/images" , container_path_base ) ;
		if( nret )
		{
			printf( "*** ERROR : WriteFileLine images failed[%d] , errno[%d]\n" , nret , errno );
			return -1;
		}
		else if( cocker_env->cmd_para.__debug )
		{
			printf( "write file %s ok\n" , container_images_file );
		}
	}
	else
	{
		nret = WriteFileLine( "" , container_images_file , sizeof(container_images_file) , "%s/images" , container_path_base ) ;
		if( nret )
		{
			printf( "*** ERROR : WriteFileLine images failed[%d] , errno[%d]\n" , nret , errno );
			return -1;
		}
		else if( cocker_env->cmd_para.__debug )
		{
			printf( "write file %s ok\n" , container_images_file );
		}
	}
	
	nret = WriteFileLine( cocker_env->cmd_para.__host_name , container_hostname_file , sizeof(container_hostname_file) , "%s/hostname" , container_path_base ) ;
	if( nret )
	{
		printf( "*** ERROR : WriteFileLine hostname failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	else if( cocker_env->cmd_para.__debug )
	{
		printf( "write file %s ok\n" , container_hostname_file );
	}
	
	/* create network */
	nret = WriteFileLine( cocker_env->net , container_rwlayer_net_file , sizeof(container_rwlayer_net_file) , "%s/net" , container_path_base ) ;
	if( nret )
	{
		printf( "*** ERROR : WriteFileLine vip failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	else if( cocker_env->cmd_para.__debug )
	{
		printf( "write file [%s] ok\n" , container_rwlayer_net_file );
	}
	
	if( STRCMP( cocker_env->net , == , "BRIDGE" ) || STRCMP( cocker_env->net , == , "CUSTOM" ) )
	{
		GetEthernetName( cocker_env->container_id , cocker_env );
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip link add %s type veth peer name %s" , cocker_env->eth_name , cocker_env->veth_name ) ;
		if( nret )
		{
			printf( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
			return -1;
		}
		else if( cocker_env->cmd_para.__debug )
		{
			printf( "system [%s] ok\n" , cmd );
		}
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "iptables -t nat -A POSTROUTING -o %s -j MASQUERADE" , cocker_env->host_if_name ) ;
		if( nret )
		{
			printf( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
			return -1;
		}
		else if( cocker_env->cmd_para.__debug )
		{
			printf( "system [%s] ok\n" , cmd );
		}
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "brctl addif %s %s" , cocker_env->netbr_name , cocker_env->eth_name ) ;
		if( nret )
		{
			printf( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
			return -1;
		}
		else if( cocker_env->cmd_para.__debug )
		{
			printf( "system [%s] ok\n" , cmd );
		}
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ifconfig %s 0.0.0.0" , cocker_env->eth_name ) ;
		if( nret )
		{
			printf( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
			return -1;
		}
		else if( cocker_env->cmd_para.__debug )
		{
			printf( "system [%s] ok\n" , cmd );
		}
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns add %s" , cocker_env->netns_name ) ;
		if( nret )
		{
			printf( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
			return -1;
		}
		else if( cocker_env->cmd_para.__debug )
		{
			printf( "system [%s] ok\n" , cmd );
		}
		
		nret = WriteFileLine( cocker_env->netns_name , container_rwlayer_netns_file , sizeof(container_rwlayer_netns_file) , "%s/netns" , container_path_base ) ;
		if( nret )
		{
			printf( "*** ERROR : WriteFileLine netns failed[%d] , errno[%d]\n" , nret , errno );
			return -1;
		}
		else if( cocker_env->cmd_para.__debug )
		{
			printf( "write file [%s] ok\n" , container_rwlayer_netns_file );
		}
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip link set %s netns %s" , cocker_env->veth_name , cocker_env->netns_name ) ;
		if( nret )
		{
			printf( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
			return -1;
		}
		else if( cocker_env->cmd_para.__debug )
		{
			printf( "system [%s] ok\n" , cmd );
		}
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns exec %s ip link set %s name %s" , cocker_env->netns_name , cocker_env->veth_name , cocker_env->veth_sname ) ;
		if( nret )
		{
			printf( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
			return -1;
		}
		else if( cocker_env->cmd_para.__debug )
		{
			printf( "system [%s] ok\n" , cmd );
		}
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns exec %s ifconfig %s %s" , cocker_env->netns_name , cocker_env->veth_sname , cocker_env->cmd_para.__vip ) ;
		if( nret )
		{
			printf( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
			return -1;
		}
		else if( cocker_env->cmd_para.__debug )
		{
			printf( "system [%s] ok\n" , cmd );
		}
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns exec %s route add default gw %s netmask 0.0.0.0 dev %s" , cocker_env->netns_name , cocker_env->netbr_ip , cocker_env->veth_sname ) ;
		if( nret )
		{
			printf( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
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
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns add %s" , cocker_env->netns_name ) ;
		if( nret )
		{
			printf( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
			return -1;
		}
		else if( cocker_env->cmd_para.__debug )
		{
			printf( "system [%s] ok\n" , cmd );
		}
		
		nret = WriteFileLine( cocker_env->netns_name , container_rwlayer_netns_file , sizeof(container_rwlayer_netns_file) , "%s/netns" , container_path_base ) ;
		if( nret )
		{
			printf( "*** ERROR : WriteFileLine netns failed[%d] , errno[%d]\n" , nret , errno );
			return -1;
		}
		else if( cocker_env->cmd_para.__debug )
		{
			printf( "write file [%s] ok\n" , container_rwlayer_netns_file );
		}
	}
	
	printf( "OK\n" );
	
	return 0;
}

