#include "vhm_in.h"

int VhmAction_stop( struct VhmEnvironment *vhm_env )
{
	char		cmd[ 4096 ] ;
	int		len ;
	
	char		netns_name[ ETHERNET_NAME_MAX + 1 ] ;
	char		veth1_name[ ETHERNET_NAME_MAX + 1 ] ;
	char		veth0_name[ ETHERNET_NAME_MAX + 1 ] ;
	char		veth0_sname[ ETHERNET_NAME_MAX + 1 ] ;
	char		vnetbr_name[ ETHERNET_NAME_MAX + 1 ] ;
	
	char		mount_target[ PATH_MAX ] ;
	
	char		pid_str[ 20 + 1 ] ;
	pid_t		pid ;
	
	int		nret = 0 ;
	
	/* down network */
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
		printf( "*** ERROR : veth0 name overflow\n" );
		return -1;
	}
	
	memset( vnetbr_name , 0x00 , sizeof(vnetbr_name) );
	len = snprintf( vnetbr_name , sizeof(vnetbr_name)-1 , "vnetbr-%s" , vhm_env->cmd_para.__vhost ) ;
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
	
	/* umount */
	memset( mount_target , 0x00 , sizeof(mount_target) );
	len = snprintf( mount_target , sizeof(mount_target)-1 , "%s/%s/merged/proc" , vhm_env->vhosts_path_base , vhm_env->cmd_para.__vhost ) ;
	if( SNPRINTF_OVERFLOW(len,sizeof(mount_target)-1) )
	{
		printf( "*** ERROR : snprintf failed\n" );
		return -1;
	}
	else
	{
		nret = umount( mount_target ) ;
		if( nret == -1 )
		{
			printf( "*** ERROR : umount proc failed\n" );
			return -1;
		}
	}
	
	memset( mount_target , 0x00 , sizeof(mount_target) );
	len = snprintf( mount_target , sizeof(mount_target)-1 , "%s/%s/merged" , vhm_env->vhosts_path_base , vhm_env->cmd_para.__vhost ) ;
	if( SNPRINTF_OVERFLOW(len,sizeof(mount_target)-1) )
	{
		printf( "*** ERROR : snprintf failed\n" );
		return -1;
	}
	else
	{
		nret = umount( mount_target ) ;
		if( nret == -1 )
		{
			printf( "*** ERROR : umount merged failed\n" );
			return -1;
		}
	}
	
	/* read pid file */
	nret = ReadFileLine( pid_str , sizeof(pid_str)-1 , NULL , -1 , "%s/%s/pid" , vhm_env->vhosts_path_base , vhm_env->cmd_para.__vhost ) ;
	if( nret == 0 )
	{
		pid = atoi(pid_str) ;
		
		/* kill clone process */
		kill( pid , SIGKILL );
		
		/* cleanup pid file */
		SnprintfAndUnlink( NULL , -1 , "%s/%s/pid" , vhm_env->vhosts_path_base , vhm_env->cmd_para.__vhost );
	}
	
	printf( "OK\n" );
	
	return 0;
}

