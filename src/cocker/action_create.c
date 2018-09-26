#include "vhm_in.h"

/* for test
$ vhm -a create --vtemplate test --vhost test --host-name test
*/

/* NOTICE
$ sodu yum install -y bridge-utils
*/

int VhmAction_create( struct VhmEnvironment *vhm_env )
{
	char		cmd[ 4096 ] ;
	int		len ;
	
	char		vhost_path_base[ PATH_MAX ] ;
	
	char		netns_name[ ETHERNET_NAME_MAX + 1 ] ;
	char		veth1_name[ ETHERNET_NAME_MAX + 1 ] ;
	char		veth0_name[ ETHERNET_NAME_MAX + 1 ] ;
	char		veth0_sname[ ETHERNET_NAME_MAX + 1 ] ;
	char		vnetbr_name[ ETHERNET_NAME_MAX + 1 ] ;
	
	int		nret = 0 ;
	
	/* create vhost folders and files */
	nret = SnprintfAndMakeDir( vhost_path_base , sizeof(vhost_path_base)-1 , "%s/%s" , vhm_env->vhosts_path_base , vhm_env->cmd_para.__vhost ) ;
	if( nret )
	{
		printf( "*** ERROR : SnprintfAndMakeDir / failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	
	nret = SnprintfAndMakeDir( NULL , -1 , "%s/rwlayer" , vhost_path_base ) ;
	if( nret )
	{
		printf( "*** ERROR : SnprintfAndMakeDir rwlayer failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	
	nret = SnprintfAndMakeDir( NULL , -1 , "%s/rwlayer/etc" , vhost_path_base ) ;
	if( nret )
	{
		printf( "*** ERROR : SnprintfAndMakeDir /etc failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	
	nret = SnprintfAndMakeDir( NULL , -1 , "%s/merged" , vhost_path_base ) ;
	if( nret )
	{
		printf( "*** ERROR : SnprintfAndMakeDir merged failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	
	nret = SnprintfAndMakeDir( NULL , -1 , "%s/workdir" , vhost_path_base ) ;
	if( nret )
	{
		printf( "*** ERROR : SnprintfAndMakeDir workdir failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	
	if( vhm_env->cmd_para.__vtemplate )
	{
		nret = WriteFileLine( vhm_env->cmd_para.__vtemplate , NULL , -1 , "%s/vtemplates" , vhost_path_base ) ;
		if( nret )
		{
			printf( "*** ERROR : WriteFileLine vtemplates failed[%d] , errno[%d]\n" , nret , errno );
			return -1;
		}
	}
	else
	{
		nret = WriteFileLine( "" , NULL , -1 , "%s/vtemplates" , vhost_path_base ) ;
		if( nret )
		{
			printf( "*** ERROR : WriteFileLine vtemplates failed[%d] , errno[%d]\n" , nret , errno );
			return -1;
		}
	}
	
	nret = WriteFileLine( vhm_env->cmd_para.__host_name , NULL , -1 , "%s/hostname" , vhost_path_base ) ;
	if( nret )
	{
		printf( "*** ERROR : WriteFileLine hostname failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	
	/* create network */
	memset( netns_name , 0x00 , sizeof(netns_name) );
	len = snprintf( netns_name , sizeof(netns_name)-1 , "netns-%s" , vhm_env->cmd_para.__vhost ) ;
	if( SNPRINTF_OVERFLOW(len,sizeof(netns_name)-1) )
	{
		printf( "*** ERROR : netns name overflow\n" );
		return -1;
	}
	
	memset( veth1_name , 0x00 , sizeof(veth1_name) );
	len = snprintf( veth1_name , sizeof(veth1_name)-1 , "veth1-%s" , vhm_env->cmd_para.__vhost ) ;
	if( SNPRINTF_OVERFLOW(len,sizeof(veth1_name)-1) )
	{
		printf( "*** ERROR : veth1 name overflow\n" );
		return -1;
	}
	
	memset( veth0_name , 0x00 , sizeof(veth0_name) );
	len = snprintf( veth0_name , sizeof(veth0_name)-1 , "veth0-%s" , vhm_env->cmd_para.__vhost ) ;
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
	len = snprintf( vnetbr_name , sizeof(vnetbr_name)-1 , "vnetbr-%s" , vhm_env->cmd_para.__vhost ) ;
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
	
	nret = SnprintfAndSystem( cmd , sizeof(cmd) , "iptables -t nat -A POSTROUTING -o ens192 -j MASQUERADE" ) ;
	if( nret )
	{
		printf( "*** ERROR : [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
		return -1;
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
	
	nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ifconfig %s 192.168.8.1" , vnetbr_name ) ;
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
	
	nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns exec %s ifconfig %s 192.168.8.88" , netns_name , veth0_sname ) ;
	if( nret )
	{
		printf( "*** ERROR : [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
		return -1;
	}
	
	printf( "OK\n" );
	
	return 0;
}

