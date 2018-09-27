#include "cocker_in.h"

int DoAction_kill( struct CockerEnvironment *cocker_env )
{
	char		pid_str[ 20 + 1 ] ;
	pid_t		pid ;
	char		container_pid_file[ PATH_MAX ] ;
	
	int		nret = 0 ;
	
	/* read pid file */
	nret = ReadFileLine( pid_str , sizeof(pid_str)-1 , NULL , -1 , "%s/%s/pid" , cocker_env->containers_path_base , cocker_env->cmd_para.__container ) ;
	if( nret == 0 )
	{
		pid = atoi(pid_str) ;
		
		/* kill clone process */
		kill( pid , SIGKILL );
		
		/* cleanup pid file */
		nret = SnprintfAndUnlink( container_pid_file , sizeof(container_pid_file) , "%s/%s/pid" , cocker_env->containers_path_base , cocker_env->cmd_para.__container ) ;
		if( nret )
		{
			printf( "*** ERROR : SnprintfAndUnlink %s failed\n" , container_pid_file );
		}
		else if( cocker_env->cmd_para.__debug )
		{
			printf( "unlink %s ok\n" , container_pid_file );
		}
	}
	
	return 0;
}

int DoAction_stop( struct CockerEnvironment *cocker_env )
{
	char		pid_str[ 20 + 1 ] ;
	
	char		container_vip_file[ PATH_MAX ] ;
	
	char		cmd[ 4096 ] ;
	int		len ;
	
	char		netns_name[ ETHERNET_NAME_MAX + 1 ] ;
	char		netbr_name[ ETHERNET_NAME_MAX + 1 ] ;
	char		veth1_name[ ETHERNET_NAME_MAX + 1 ] ;
	char		veth0_name[ ETHERNET_NAME_MAX + 1 ] ;
	char		veth0_sname[ ETHERNET_NAME_MAX + 1 ] ;
	
	char		mount_target[ PATH_MAX ] ;
	
	int		nret = 0 ;
	
	/* read pid file */
	nret = ReadFileLine( pid_str , sizeof(pid_str)-1 , NULL , -1 , "%s/%s/pid" , cocker_env->containers_path_base , cocker_env->cmd_para.__container ) ;
	if( nret )
	{
		printf( "*** ERROR : container is not running\n" );
		if( ! cocker_env->cmd_para.__forcely )
			return 0;
	}
	
	/* read vip file */
	nret = ReadFileLine( cocker_env->vip , sizeof(cocker_env->vip) , container_vip_file , sizeof(container_vip_file) , "%s/%s/vip" , cocker_env->containers_path_base , cocker_env->cmd_para.__container ) ;
	if( nret < 0 )
	{
		printf( "*** ERROR : ReadFileLine vip failed\n" );
		return -1;
	}
	else if( cocker_env->cmd_para.__debug )
	{
		printf( "read file %s ok\n" , container_vip_file );
	}
	
	if( cocker_env->vip[0] )
	{
		/* down network */
		memset( netns_name , 0x00 , sizeof(netns_name) );
		len = snprintf( netns_name , sizeof(netns_name)-1 , "netns-%s" , cocker_env->cmd_para.__container ) ;
		if( SNPRINTF_OVERFLOW(len,sizeof(netns_name)-1) )
		{
			printf( "*** ERROR : netns name overflow\n" );
			if( ! cocker_env->cmd_para.__forcely )
				return -1;
		}
		
		memset( netbr_name , 0x00 , sizeof(netbr_name) );
		len = snprintf( netbr_name , sizeof(netbr_name)-1 , "cocker0" ) ;
		if( SNPRINTF_OVERFLOW(len,sizeof(netbr_name)-1) )
		{
			printf( "*** ERROR : netbr name overflow\n" );
			if( ! cocker_env->cmd_para.__forcely )
				return -1;
		}
		
		memset( veth1_name , 0x00 , sizeof(veth1_name) );
		len = snprintf( veth1_name , sizeof(veth1_name)-1 , "veth1-%s" , cocker_env->cmd_para.__container ) ;
		if( SNPRINTF_OVERFLOW(len,sizeof(veth1_name)-1) )
		{
			printf( "*** ERROR : veth1 name overflow\n" );
			if( ! cocker_env->cmd_para.__forcely )
				return -1;
		}
		
		memset( veth0_name , 0x00 , sizeof(veth0_name) );
		len = snprintf( veth0_name , sizeof(veth0_name)-1 , "veth0-%s" , cocker_env->cmd_para.__container ) ;
		if( SNPRINTF_OVERFLOW(len,sizeof(veth0_name)-1) )
		{
			printf( "*** ERROR : veth0 name overflow\n" );
			if( ! cocker_env->cmd_para.__forcely )
				return -1;
		}
		
		memset( veth0_sname , 0x00 , sizeof(veth0_sname) );
		len = snprintf( veth0_sname , sizeof(veth0_sname)-1 , "eth0" ) ;
		if( SNPRINTF_OVERFLOW(len,sizeof(veth0_sname)-1) )
		{
			printf( "*** ERROR : veth0 name overflow\n" );
			if( ! cocker_env->cmd_para.__forcely )
				return -1;
		}
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ifconfig %s down" , veth1_name ) ;
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
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ifconfig %s down" , netbr_name ) ;
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
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns exec %s ifconfig %s down" , netns_name , veth0_sname ) ;
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
	
	/* umount */
	memset( mount_target , 0x00 , sizeof(mount_target) );
	len = snprintf( mount_target , sizeof(mount_target)-1 , "%s/%s/merged/proc" , cocker_env->containers_path_base , cocker_env->cmd_para.__container ) ;
	if( SNPRINTF_OVERFLOW(len,sizeof(mount_target)-1) )
	{
		printf( "*** ERROR : snprintf failed\n" );
		if( ! cocker_env->cmd_para.__forcely )
			return -1;
	}
	else
	{
		nret = umount( mount_target ) ;
		if( nret == -1 )
		{
			printf( "*** ERROR : umount proc failed\n" );
			if( ! cocker_env->cmd_para.__forcely )
				return -1;
		}
		else if( cocker_env->cmd_para.__debug )
		{
			printf( "umount %s ok\n" , mount_target );
		}
	}
	
	memset( mount_target , 0x00 , sizeof(mount_target) );
	len = snprintf( mount_target , sizeof(mount_target)-1 , "%s/%s/merged" , cocker_env->containers_path_base , cocker_env->cmd_para.__container ) ;
	if( SNPRINTF_OVERFLOW(len,sizeof(mount_target)-1) )
	{
		printf( "*** ERROR : snprintf failed\n" );
		if( ! cocker_env->cmd_para.__forcely )
			return -1;
	}
	else
	{
		nret = umount( mount_target ) ;
		if( nret == -1 )
		{
			printf( "*** ERROR : umount merged failed\n" );
			if( ! cocker_env->cmd_para.__forcely )
				return -1;
		}
		else if( cocker_env->cmd_para.__debug )
		{
			printf( "umount %s ok\n" , mount_target );
		}
	}
	
	DoAction_kill( cocker_env );
	
	printf( "OK\n" );
	
	return 0;
}

