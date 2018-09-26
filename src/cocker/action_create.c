#include "cocker_in.h"

int DoAction_create( struct CockerEnvironment *cocker_env )
{
	char		cmd[ 4096 ] ;
	int		len ;
	
	char		container_path_base[ PATH_MAX ] ;
	
	char		netns_name[ ETHERNET_NAME_MAX + 1 ] ;
	char		veth1_name[ ETHERNET_NAME_MAX + 1 ] ;
	char		veth0_name[ ETHERNET_NAME_MAX + 1 ] ;
	char		veth0_sname[ ETHERNET_NAME_MAX + 1 ] ;
	char		vnetbr_name[ ETHERNET_NAME_MAX + 1 ] ;
	
	int		nret = 0 ;
	
	/* preprocess input parameters */
	if( cocker_env->cmd_para.__host_name == NULL )
		cocker_env->cmd_para.__host_name = cocker_env->cmd_para.__container ;
	
	if( cocker_env->cmd_para.__vip )
	{
		char	*p = NULL ;
		
		memset( cocker_env->vip , 0x00 , sizeof(cocker_env->vip) );
		strncpy( cocker_env->vip , cocker_env->cmd_para.__vip , sizeof(cocker_env->vip)-1 );
		
		memset( cocker_env->vgateway , 0x00 , sizeof(cocker_env->vgateway) );
		strncpy( cocker_env->vgateway , cocker_env->cmd_para.__vip , sizeof(cocker_env->vgateway)-1 );
		p = strchr( cocker_env->vgateway , '.' ) ;
		if( p )
		{
			p = strchr( p+1 , '.' ) ;
			if( p )
			{
				p = strchr( p+1 , '.' ) ;
				if( p )
				{
					strcpy( p , ".1" );
				}
			}
		}
	}
	
	/* create container folders and files */
	nret = SnprintfAndMakeDir( container_path_base , sizeof(container_path_base)-1 , "%s/%s" , cocker_env->containers_path_base , cocker_env->cmd_para.__container ) ;
	if( nret )
	{
		printf( "*** ERROR : SnprintfAndMakeDir / failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	
	nret = SnprintfAndMakeDir( NULL , -1 , "%s/rwlayer" , container_path_base ) ;
	if( nret )
	{
		printf( "*** ERROR : SnprintfAndMakeDir rwlayer failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	
	nret = SnprintfAndMakeDir( NULL , -1 , "%s/rwlayer/etc" , container_path_base ) ;
	if( nret )
	{
		printf( "*** ERROR : SnprintfAndMakeDir /etc failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	
	nret = SnprintfAndMakeDir( NULL , -1 , "%s/merged" , container_path_base ) ;
	if( nret )
	{
		printf( "*** ERROR : SnprintfAndMakeDir merged failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	
	nret = SnprintfAndMakeDir( NULL , -1 , "%s/workdir" , container_path_base ) ;
	if( nret )
	{
		printf( "*** ERROR : SnprintfAndMakeDir workdir failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	
	if( cocker_env->cmd_para.__image )
	{
		nret = WriteFileLine( cocker_env->cmd_para.__image , NULL , -1 , "%s/images" , container_path_base ) ;
		if( nret )
		{
			printf( "*** ERROR : WriteFileLine images failed[%d] , errno[%d]\n" , nret , errno );
			return -1;
		}
	}
	else
	{
		nret = WriteFileLine( "" , NULL , -1 , "%s/images" , container_path_base ) ;
		if( nret )
		{
			printf( "*** ERROR : WriteFileLine images failed[%d] , errno[%d]\n" , nret , errno );
			return -1;
		}
	}
	
	nret = WriteFileLine( cocker_env->cmd_para.__host_name , NULL , -1 , "%s/hostname" , container_path_base ) ;
	if( nret )
	{
		printf( "*** ERROR : WriteFileLine hostname failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	
	/* create network */
	if( cocker_env->vip[0] )
	{
		memset( netns_name , 0x00 , sizeof(netns_name) );
		len = snprintf( netns_name , sizeof(netns_name)-1 , "netns-%s" , cocker_env->cmd_para.__container ) ;
		if( SNPRINTF_OVERFLOW(len,sizeof(netns_name)-1) )
		{
			printf( "*** ERROR : netns name overflow\n" );
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
		
		memset( vnetbr_name , 0x00 , sizeof(vnetbr_name) );
		len = snprintf( vnetbr_name , sizeof(vnetbr_name)-1 , "vnetbr-%s" , cocker_env->cmd_para.__container ) ;
		if( SNPRINTF_OVERFLOW(len,sizeof(vnetbr_name)-1) )
		{
			printf( "*** ERROR : vnetbr name overflow\n" );
			return -1;
		}
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip link add %s type veth peer name %s" , veth1_name , veth0_name ) ;
		if( nret )
		{
			printf( "*** ERROR : [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
			return -1;
		}
		
		if( cocker_env->cmd_para.__nat_postrouting )
		{
			nret = SnprintfAndSystem( cmd , sizeof(cmd) , "iptables -t nat -A POSTROUTING -o %s -j MASQUERADE" , cocker_env->cmd_para.__nat_postrouting ) ;
			if( nret )
			{
				printf( "*** ERROR : [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
				return -1;
			}
		}
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "brctl addbr %s" , vnetbr_name ) ;
		if( nret )
		{
			printf( "*** ERROR : [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
			return -1;
		}
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "brctl addif %s %s" , vnetbr_name , veth1_name ) ;
		if( nret )
		{
			printf( "*** ERROR : [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
			return -1;
		}
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ifconfig %s 0.0.0.0" , veth1_name ) ;
		if( nret )
		{
			printf( "*** ERROR : [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
			return -1;
		}
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ifconfig %s %s" , vnetbr_name , cocker_env->vgateway ) ;
		if( nret )
		{
			printf( "*** ERROR : [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
			return -1;
		}
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns add %s" , netns_name ) ;
		if( nret )
		{
			printf( "*** ERROR : [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
			return -1;
		}
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip link set %s netns %s" , veth0_name , netns_name ) ;
		if( nret )
		{
			printf( "*** ERROR : [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
			return -1;
		}
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns exec %s ip link set %s name %s" , netns_name , veth0_name , veth0_sname ) ;
		if( nret )
		{
			printf( "*** ERROR : [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
			return -1;
		}
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns exec %s ifconfig %s %s" , netns_name , veth0_sname , cocker_env->cmd_para.__vip ) ;
		if( nret )
		{
			printf( "*** ERROR : [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
			return -1;
		}
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns exec %s route add default gw %s netmask 0.0.0.0 dev %s" , netns_name , cocker_env->vgateway , veth0_sname ) ;
		if( nret )
		{
			printf( "*** ERROR : [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
			return -1;
		}
		
		nret = WriteFileLine( cocker_env->vip , NULL , -1 , "%s/vip" , container_path_base ) ;
		if( nret )
		{
			printf( "*** ERROR : WriteFileLine vip failed[%d] , errno[%d]\n" , nret , errno );
			return -1;
		}
		
		nret = WriteFileLine( cocker_env->vip , NULL , -1 , "%s/vgateway" , container_path_base ) ;
		if( nret )
		{
			printf( "*** ERROR : WriteFileLine vip failed[%d] , errno[%d]\n" , nret , errno );
			return -1;
		}
	}
	
	printf( "OK\n" );
	
	return 0;
}

