#include "cocker_in.h"

int DoAction_create( struct CockerEnvironment *cocker_env )
{
	char		cmd[ 4096 ] ;
	int		len ;
	
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
	
	char		netns_name[ ETHERNET_NAME_MAX + 1 ] ;
	char		netbr_name[ ETHERNET_NAME_MAX + 1 ] ;
	char		veth1_name[ ETHERNET_NAME_MAX + 1 ] ;
	char		veth0_name[ ETHERNET_NAME_MAX + 1 ] ;
	char		veth0_sname[ ETHERNET_NAME_MAX + 1 ] ;
	char		netbr_ip[ IP_LEN_MAX ] ;
	
	int		nret = 0 ;
	
	/* preprocess input parameters */
	if( cocker_env->cmd_para.__host_name == NULL )
		cocker_env->cmd_para.__host_name = cocker_env->cmd_para.__container ;
	
	/* read pid file */
	nret = ReadFileLine( pid_str , sizeof(pid_str)-1 , NULL , -1 , "%s/%s/pid" , cocker_env->containers_path_base , cocker_env->cmd_para.__container ) ;
	if( nret == 0 )
	{
		printf( "*** ERROR : container is already running\n" );
		return 0;
	}
	TrimEnter( pid_str );
	
	/* create container folders and files */
	nret = SnprintfAndMakeDir( container_path_base , sizeof(container_path_base)-1 , "%s/%s" , cocker_env->containers_path_base , cocker_env->cmd_para.__container ) ;
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
	
	if( cocker_env->cmd_para.__image )
	{
		nret = WriteFileLine( cocker_env->cmd_para.__image , container_images_file , sizeof(container_images_file) , "%s/images" , container_path_base ) ;
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
		memset( netns_name , 0x00 , sizeof(netns_name) );
		len = snprintf( netns_name , sizeof(netns_name)-1 , "netns-%s" , cocker_env->cmd_para.__container ) ;
		if( SNPRINTF_OVERFLOW(len,sizeof(netns_name)-1) )
		{
			printf( "*** ERROR : netns name overflow\n" );
			return -1;
		}
		
		memset( netbr_name , 0x00 , sizeof(netbr_name) );
		len = snprintf( netbr_name , sizeof(netbr_name)-1 , "cocker0" ) ;
		if( SNPRINTF_OVERFLOW(len,sizeof(netbr_name)-1) )
		{
			printf( "*** ERROR : netbr name overflow\n" );
			return -1;
		}
		
		memset( veth1_name , 0x00 , sizeof(veth1_name) );
		len = snprintf( veth1_name , sizeof(veth1_name)-1 , "veth1-%s" , cocker_env->cmd_para.__container ) ;
		if( SNPRINTF_OVERFLOW(len,sizeof(veth1_name)-1) )
		{
			printf( "*** ERROR : veth1 name overflow\n" );
			return -1;
		}
		
		memset( veth0_name , 0x00 , sizeof(veth0_name) );
		len = snprintf( veth0_name , sizeof(veth0_name)-1 , "veth0-%s" , cocker_env->cmd_para.__container ) ;
		if( SNPRINTF_OVERFLOW(len,sizeof(veth0_name)-1) )
		{
			printf( "*** ERROR : veth0 name overflow\n" );
			return -1;
		}
		
		memset( veth0_sname , 0x00 , sizeof(veth0_sname) );
		len = snprintf( veth0_sname , sizeof(veth0_sname)-1 , "eth0" ) ;
		if( SNPRINTF_OVERFLOW(len,sizeof(veth0_sname)-1) )
		{
			printf( "*** ERROR : veth0 sname overflow\n" );
			return -1;
		}
		
		if( STRCMP( cocker_env->net , == , "BRIDGE" ) )
		{
			nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip link add %s type veth peer name %s" , veth1_name , veth0_name ) ;
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
			
			nret = SnprintfAndSystem( cmd , sizeof(cmd) , "brctl addif %s %s" , netbr_name , veth1_name ) ;
			if( nret )
			{
				printf( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
				return -1;
			}
			else if( cocker_env->cmd_para.__debug )
			{
				printf( "system [%s] ok\n" , cmd );
			}
			
			nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ifconfig %s 0.0.0.0" , veth1_name ) ;
			if( nret )
			{
				printf( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
				return -1;
			}
			else if( cocker_env->cmd_para.__debug )
			{
				printf( "system [%s] ok\n" , cmd );
			}
			
			nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns add %s" , netns_name ) ;
			if( nret )
			{
				printf( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
				return -1;
			}
			else if( cocker_env->cmd_para.__debug )
			{
				printf( "system [%s] ok\n" , cmd );
			}
			
			nret = WriteFileLine( netns_name , container_rwlayer_netns_file , sizeof(container_rwlayer_netns_file) , "%s/netns" , container_path_base ) ;
			if( nret )
			{
				printf( "*** ERROR : WriteFileLine netns failed[%d] , errno[%d]\n" , nret , errno );
				return -1;
			}
			else if( cocker_env->cmd_para.__debug )
			{
				printf( "write file [%s] ok\n" , container_rwlayer_netns_file );
			}
			
			nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip link set %s netns %s" , veth0_name , netns_name ) ;
			if( nret )
			{
				printf( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
				return -1;
			}
			else if( cocker_env->cmd_para.__debug )
			{
				printf( "system [%s] ok\n" , cmd );
			}
			
			nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns exec %s ip link set %s name %s" , netns_name , veth0_name , veth0_sname ) ;
			if( nret )
			{
				printf( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
				return -1;
			}
			else if( cocker_env->cmd_para.__debug )
			{
				printf( "system [%s] ok\n" , cmd );
			}
			
			nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns exec %s ifconfig %s %s" , netns_name , veth0_sname , cocker_env->cmd_para.__vip ) ;
			if( nret )
			{
				printf( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
				return -1;
			}
			else if( cocker_env->cmd_para.__debug )
			{
				printf( "system [%s] ok\n" , cmd );
			}
			
			nret = SnprintfAndPopen( netbr_ip , sizeof(netbr_ip) , cmd , sizeof(cmd) , "ifconfig cocker0 | grep -w inet | awk '{print $2}'" ) ;
			if( nret )
			{
				printf( "*** ERROR : SnprintfAndPopen [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
				return -1;
			}
			TrimEnter( netbr_ip );
			
			nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns exec %s route add default gw %s netmask 0.0.0.0 dev %s" , netns_name , netbr_ip , veth0_sname ) ;
			if( nret )
			{
				printf( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
				return -1;
			}
			else if( cocker_env->cmd_para.__debug )
			{
				printf( "system [%s] ok\n" , cmd );
			}
			
			/*
			nret = WriteFileLine( cocker_env->vip , NULL , -1 , "%s/vgateway" , container_path_base ) ;
			if( nret )
			{
				printf( "*** ERROR : WriteFileLine vip failed[%d] , errno[%d]\n" , nret , errno );
				return -1;
			}
			else if( cocker_env->cmd_para.__debug )
			{
				printf( "system [%s] ok\n" , cmd );
			}
			*/
		}
		else if( STRCMP( cocker_env->net , == , "CUSTOM" ) )
		{
			nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns add %s" , netns_name ) ;
			if( nret )
			{
				printf( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
				return -1;
			}
			else if( cocker_env->cmd_para.__debug )
			{
				printf( "system [%s] ok\n" , cmd );
			}
			
			nret = WriteFileLine( netns_name , container_rwlayer_netns_file , sizeof(container_rwlayer_netns_file) , "%s/netns" , container_path_base ) ;
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
	}
	
	printf( "OK\n" );
	
	return 0;
}

