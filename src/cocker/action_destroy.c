#include "cocker_in.h"

int DoAction_destroy( struct CockerEnvironment *cocker_env )
{
	char		cmd[ 4096 ] ;
	int		len ;
	
	char		netns_name[ ETHERNET_NAME_MAX + 1 ] ;
	char		veth1_name[ ETHERNET_NAME_MAX + 1 ] ;
	char		veth0_name[ ETHERNET_NAME_MAX + 1 ] ;
	char		veth0_sname[ ETHERNET_NAME_MAX + 1 ] ;
	char		vnetbr_name[ ETHERNET_NAME_MAX + 1 ] ;
	
	char		container_path_base[ PATH_MAX ] ;
	char		rwlayer_path[ PATH_MAX ] ;
	char		hostname_path[ PATH_MAX ] ;
	
	int		nret = 0 ;
	
	nret = ReadFileLine( cocker_env->vip , sizeof(cocker_env->vip) , NULL , -1 , "%s/%s/vip" , cocker_env->containers_path_base , cocker_env->cmd_para.__container ) ;
	if( nret < 0 )
	{
		printf( "*** ERROR : ReadFileLine vip failed\n" );
		return -1;
	}
	
	if( cocker_env->vip[0] )
	{
		/* destroy network */
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
			printf( "*** ERROR : veth0 name overflow\n" );
			return -1;
		}
		
		memset( vnetbr_name , 0x00 , sizeof(vnetbr_name) );
		len = snprintf( vnetbr_name , sizeof(vnetbr_name)-1 , "vnetbr-%s" , cocker_env->cmd_para.__container ) ;
		if( SNPRINTF_OVERFLOW(len,sizeof(vnetbr_name)-1) )
		{
			printf( "*** ERROR : vnetbr name overflow\n" );
			return -1;
		}
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ifconfig %s down" , veth1_name ) ;
		if( nret )
		{
			printf( "*** ERROR : [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
		}
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ifconfig %s down" , vnetbr_name ) ;
		if( nret )
		{
			printf( "*** ERROR : [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
		}
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns exec %s ifconfig %s down" , netns_name , veth0_sname ) ;
		if( nret )
		{
			printf( "*** ERROR : [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
		}
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "brctl delif %s %s" , vnetbr_name , veth1_name ) ;
		if( nret )
		{
			printf( "*** ERROR : [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
		}
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "brctl delbr %s" , vnetbr_name ) ;
		if( nret )
		{
			printf( "*** ERROR : [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
		}
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip link del %s" , veth1_name ) ;
		if( nret )
		{
			printf( "*** ERROR : [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
		}
		
		/*
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns exec %s ip link del %s" , netns_name , veth0_sname ) ;
		if( nret )
		{
			printf( "*** ERROR : [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
		}
		*/
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns del %s" , netns_name ) ;
		if( nret )
		{
			printf( "*** ERROR : [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
		}
	}
	
	/* destroy container folders and files */
	nret = snprintf( container_path_base , sizeof(container_path_base)-1 , "%s/%s" , cocker_env->containers_path_base , cocker_env->cmd_para.__container ) ;
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
		memset( cmd , 0x00 , sizeof(cmd) );
		snprintf( cmd , sizeof(cmd)-1 , "rm -rf %s" , container_path_base );
		system( cmd );
	}
	
	printf( "OK\n" );
	
	return 0;
}

