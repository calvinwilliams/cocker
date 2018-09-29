#include "cocker_in.h"

/* for debug
$ mount -t overlay overlay -o lowerdir=/var/cocker/images/test/rlayer,upperdir=/var/cocker/containers/test/rwlayer,workdir=/var/cocker/containers/test/workdir /var/cocker/containers/test/merged
$ chroot /var/cocker/containers/test/merged
$ mount -t proc proc /proc

$ umount /proc
$ umount /var/cocker/container/test/merged
*/

static unsigned char	stack_bottom[ 1024 * 1024 ] = {0} ;

static int VHostEntry( void *p )
{
	struct CockerEnvironment	*cocker_env = (struct CockerEnvironment *)p ;
	
	int				len ;
	
	char				netns_path[ PATH_MAX + 1 ] ;
	int				netns_fd ;
	
	char				container_hostname_file[ PATH_MAX + 1 ] ;
	char				hostname[ HOST_NAME_MAX + 1 ] ;
	char				container_images_file[ PATH_MAX + 1 ] ;
	char				image[ PATH_MAX + 1 ] ;
	char				mount_target[ PATH_MAX + 1 ] ;
	char				mount_data[ 4096 ] ;
	
	int				nret = 0 ;
	
	if( STRCMP( cocker_env->net , == , "BRIDGE" ) || STRCMP( cocker_env->net , == , "CUSTOM" ) )
	{
		/* setns */
		memset( netns_path , 0x00 , sizeof(netns_path) );
		len = snprintf( netns_path , sizeof(netns_path)-1 , "/var/run/netns/%s" , cocker_env->netns_name ) ;
		if( SNPRINTF_OVERFLOW(len,sizeof(netns_path)-1) )
		{
			printf( "*** ERROR : netns path overflow\n" );
			return -1;
		}
		netns_fd = open( netns_path , O_RDONLY ) ;
		if( netns_fd == -1 )
		{
			printf( "*** ERROR : open netns path failed , errno[%d]\n" , errno );
			return -1;
		}
		
		nret = setns( netns_fd , CLONE_NEWNET ) ;
		if( nret == -1 )
		{
			printf( "*** ERROR : setns failed , errno[%d]\n" , errno );
			return -1;
		}
		else if( cocker_env->cmd_para.__debug )
		{
			printf( "setns %s ok\n" , netns_path );
		}
		
		close( netns_fd );
	}
	
	/* basic seting */
	setuid(0);
	setgid(0);
	setgroups(0,NULL);
	
	/* sethostname */
	nret = ReadFileLine( hostname , sizeof(hostname)-1 , container_hostname_file , sizeof(container_hostname_file) , "%s/%s/hostname" , cocker_env->containers_path_base , cocker_env->container_id ) ;
	if( nret )
	{
		printf( "*** ERROR : ReadFileLine hostname in container '%s' failed\n" , cocker_env->container_id );
		return -1;
	}
	else if( cocker_env->cmd_para.__debug )
	{
		printf( "read file %s ok\n" , container_hostname_file );
	}
	TrimEnter( hostname );
	sethostname( hostname , strlen(hostname) );
	if( cocker_env->cmd_para.__debug )
	{
		printf( "sethostname [%s] ok\n" , hostname );
	}
	
	/* mount filesystem */
	nret = ReadFileLine( image , sizeof(image)-1 , NULL , -1 , "%s/%s/images" , cocker_env->containers_path_base , cocker_env->container_id ) ;
	if( nret )
	{
		printf( "*** ERROR : ReadFileLine images in container '%s' failed\n" , cocker_env->container_id );
		return -1;
	}
	else if( cocker_env->cmd_para.__debug )
	{
		printf( "read file %s ok\n" , container_images_file );
	}
	TrimEnter( image );
	
	memset( mount_target , 0x00 , sizeof(mount_target) );
	len = snprintf( mount_target , sizeof(mount_target)-1 , "%s/%s/merged" , cocker_env->containers_path_base , cocker_env->container_id ) ;
	if( SNPRINTF_OVERFLOW(len,sizeof(mount_target)-1) )
	{
		printf( "*** ERROR : snprintf failed\n" );
		return -1;
	}
	
	memset( mount_data , 0x00 , sizeof(mount_data) );
	if( image[0] == '\0' )
		len = snprintf( mount_data , sizeof(mount_data)-1 , "upperdir=%s/%s/rwlayer,workdir=%s/%s/workdir" , cocker_env->containers_path_base,cocker_env->container_id , cocker_env->containers_path_base,cocker_env->container_id ) ;
	else
		len = snprintf( mount_data , sizeof(mount_data)-1 , "lowerdir=%s/%s/rlayer,upperdir=%s/%s/rwlayer,workdir=%s/%s/workdir" , cocker_env->images_path_base,image , cocker_env->containers_path_base,cocker_env->container_id , cocker_env->containers_path_base,cocker_env->container_id ) ;
	if( SNPRINTF_OVERFLOW(len,sizeof(mount_data)-1) )
	{
		printf( "*** ERROR : snprintf failed\n" );
		return -1;
	}
	nret = mount( "overlay" , mount_target , "overlay" , MS_MGC_VAL , (void*)mount_data ) ;
	if( nret == -1 )
	{
		printf( "*** ERROR : mount[%s][%s] failed , errno[%d]\n" , mount_data , mount_target , errno );
		return -1;
	}
	else if( cocker_env->cmd_para.__debug )
	{
		printf( "mount [%s][%s][%s][0x%X][%s] ok\n" , "overlay" , mount_target , "overlay" , MS_MGC_VAL , mount_data );
	}
	
	/* chroot */
	chroot( mount_target );
	if( cocker_env->cmd_para.__debug )
	{
		printf( "chroot [%s] ok\n" , mount_target );
	}
	chdir( "/root" );
	if( cocker_env->cmd_para.__debug )
	{
		printf( "chdir [%s] ok\n" , "/root" );
	}
	
	/* mount /proc */
	mount( "proc" , "/proc" , "proc" , MS_MGC_VAL , NULL );
	if( cocker_env->cmd_para.__debug )
	{
		printf( "mount [%s][%s][%s][0x%X][%s] ok\n" , "proc" , "/proc" , "proc" , MS_MGC_VAL , "(null)" );
	}
	
	/* execl */
	nret = execl( "/bin/bash" , "bash" , "--login" , NULL ) ;
	if( nret == -1 )
	{
		printf( "*** ERROR : execl failed , errno[%d]\n" , errno );
		exit(9);
	}
	
	return 0;
}

int DoAction_start( struct CockerEnvironment *cocker_env )
{
	char		container_pid_file[ PATH_MAX ] ;
	char		container_net_file[ PATH_MAX + 1 ] ;
	char		container_vip_file[ PATH_MAX + 1 ] ;
	char		container_port_mapping_file[ PATH_MAX + 1 ] ;
	
	char		src_port_str[ 20 + 1 ] ;
	
	char		cmd[ 4096 ] ;
	int		len ;
	
	pid_t		pid ;
	char		pid_str[ 20 + 1 ] ;
	
	char		mount_target[ PATH_MAX ] ;
	
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
		TrimEnter( pid_str );
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
	
	/* up network */
	if( STRCMP( cocker_env->net , == , "BRIDGE" ) )
	{
		/* up network */
		GetEthernetName( cocker_env->container_id , cocker_env );
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ifconfig %s up" , cocker_env->eth_name ) ;
		if( nret )
		{
			printf( "*** ERROR : system [%s] failed [%d], errno[%d]\n" , cmd , nret , errno );
			return -1;
		}
		else if( cocker_env->cmd_para.__debug )
		{
			printf( "system [%s] ok\n" , cmd );
		}
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns exec %s ifconfig %s up" , cocker_env->netns_name , cocker_env->veth_sname ) ;
		if( nret )
		{
			printf( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
			return -1;
		}
		else if( cocker_env->cmd_para.__debug )
		{
			printf( "system [%s] ok\n" , cmd );
		}
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns exec %s ifconfig lo up" , cocker_env->netns_name ) ;
		if( nret )
		{
			printf( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
			return -1;
		}
		else if( cocker_env->cmd_para.__debug )
		{
			printf( "system [%s] ok\n" , cmd );
		}
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "iptables -t nat -A POSTROUTING -o %s -j MASQUERADE" , cocker_env->host_eth ) ;
		if( nret )
		{
			printf( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
			return -1;
		}
		else if( cocker_env->cmd_para.__debug )
		{
			printf( "system [%s] ok\n" , cmd );
		}
		
		nret = ReadFileLine( cocker_env->vip , sizeof(cocker_env->vip) , container_vip_file , sizeof(container_vip_file) , "%s/vip" , cocker_env->container_path_base ) ;
		if( nret < 0 )
		{
			printf( "*** ERROR : ReadFileLine net failed\n" );
			return -1;
		}
		else if( cocker_env->cmd_para.__debug )
		{
			printf( "read file %s ok\n" , container_vip_file );
		}
		TrimEnter( cocker_env->vip );
		
		memset( cocker_env->port_mapping , 0x00 , sizeof(cocker_env->port_mapping) );
		nret = ReadFileLine( cocker_env->port_mapping , sizeof(cocker_env->port_mapping) , container_port_mapping_file , sizeof(container_port_mapping_file) , "%s/port_mapping" , cocker_env->container_path_base ) ;
		if( nret < 0 )
		{
			memset( cocker_env->port_mapping , 0x00 , sizeof(cocker_env->port_mapping) );
		}
		else if( cocker_env->cmd_para.__debug )
		{
			printf( "read file %s ok\n" , container_port_mapping_file );
		}
		TrimEnter( cocker_env->port_mapping );
		
		if( cocker_env->port_mapping[0] )
		{
			memset( src_port_str , 0x00 , sizeof(src_port_str) );
			sscanf( cocker_env->port_mapping , "%[^:]:%d" , src_port_str , & (cocker_env->dst_port) );
			cocker_env->src_port = atoi(src_port_str) ;
			if( cocker_env->src_port <= 0 || cocker_env->dst_port <= 0 )
			{
				printf( "*** ERROR : file port_mapping value [%s] invalid\n" , cocker_env->port_mapping );
				return -1;
			}
			
			nret = SnprintfAndSystem( cmd , sizeof(cmd) , "iptables -t nat -A PREROUTING -i %s -p tcp -m tcp --dport %d -j DNAT --to-destination %s:%d" , cocker_env->host_eth , cocker_env->src_port , cocker_env->vip , cocker_env->dst_port ) ;
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
			nret = SnprintfAndSystem( cmd , sizeof(cmd) , "iptables -t nat -A POSTROUTING -s 166.88.0.0/16 -j SNAT --to-source 66.88.1.61" ) ;
			if( nret )
			{
				printf( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
				return -1;
			}
			else if( cocker_env->cmd_para.__debug )
			{
				printf( "system [%s] ok\n" , cmd );
			}
			*/
		}
	}
	
	/* create container */
	if( STRCMP( cocker_env->net , == , "HOST" ) )
	{
		pid = clone( VHostEntry , stack_bottom+sizeof(stack_bottom) , CLONE_NEWNS|CLONE_NEWPID|CLONE_NEWIPC|CLONE_NEWUTS , (void*)cocker_env ) ;
	}
	else
	{
		pid = clone( VHostEntry , stack_bottom+sizeof(stack_bottom) , CLONE_NEWNS|CLONE_NEWPID|CLONE_NEWIPC|CLONE_NEWUTS|CLONE_NEWNET , (void*)cocker_env ) ;
	}
	if( pid == -1 )
	{
		printf( "*** ERROR : clone failed[%d] , errno[%d]\n" , pid , errno );
		cocker_env->cmd_para.__forcely = "--force" ;
		DoAction_stop( cocker_env );
		return -1;
	}
	
	/* write pid file */
	memset( pid_str , 0x00 , sizeof(pid_str) );
	snprintf( pid_str , sizeof(pid_str)-1 , "%d" , pid );
	nret = WriteFileLine( pid_str , container_pid_file , sizeof(container_pid_file) , "%s/%s/pid" , cocker_env->containers_path_base , cocker_env->container_id ) ;
	if( nret )
	{
		printf( "*** ERROR : WriteFileLine failed[%d] , errno[%d]\n" , nret , errno );
		cocker_env->cmd_para.__forcely = "--force" ;
		DoAction_stop( cocker_env );
		return -1;
	}
	else if( cocker_env->cmd_para.__debug )
	{
		printf( "write file %s ok\n" , container_pid_file );
	}
	
	/* wait for container end */
	while(1)
	{
		pid = waitpid( -1 , NULL , __WCLONE ) ;
		if( pid == -1 )
		{
			if( errno == ECHILD )
				break;
			
			printf( "*** ERROR : waitpid failed , errno[%d]\n" , errno ); fflush(stdout);
			cocker_env->cmd_para.__forcely = "--force" ;
			DoAction_stop( cocker_env );
			return -1;
		}
	}
	
	/* down network */
	if( STRCMP( cocker_env->net , == , "BRIDGE" ) )
	{
		/* down network */
		GetEthernetName( cocker_env->container_id , cocker_env );
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "iptables -t nat -D POSTROUTING -o %s -j MASQUERADE" , cocker_env->host_eth ) ;
		if( nret )
		{
			printf( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
			return -1;
		}
		else if( cocker_env->cmd_para.__debug )
		{
			printf( "system [%s] ok\n" , cmd );
		}
		
		if( cocker_env->port_mapping[0] )
		{
			nret = SnprintfAndSystem( cmd , sizeof(cmd) , "iptables -t nat -D PREROUTING -i %s -p tcp -m tcp --dport %d -j DNAT --to-destination %s:%d" , cocker_env->host_eth , cocker_env->src_port , cocker_env->vip , cocker_env->dst_port ) ;
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
			nret = SnprintfAndSystem( cmd , sizeof(cmd) , "iptables -t nat -D POSTROUTING -s 166.88.0.0/16 -j SNAT --to-source 66.88.1.61" ) ;
			if( nret )
			{
				printf( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
				return -1;
			}
			else if( cocker_env->cmd_para.__debug )
			{
				printf( "system [%s] ok\n" , cmd );
			}
			*/
		}
		
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
	}
	
	/* cleanup pid file */
	nret = unlink( container_pid_file ) ;
	if( nret )
	{
		printf( "*** ERROR : unlink %s failed\n" , container_pid_file );
	}
	else if( cocker_env->cmd_para.__debug )
	{
		printf( "unlink %s ok\n" , container_pid_file );
	}
	
	/* umount */
	memset( mount_target , 0x00 , sizeof(mount_target) );
	len = snprintf( mount_target , sizeof(mount_target)-1 , "%s/%s/merged/proc" , cocker_env->containers_path_base , cocker_env->container_id ) ;
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
	len = snprintf( mount_target , sizeof(mount_target)-1 , "%s/%s/merged" , cocker_env->containers_path_base , cocker_env->container_id ) ;
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
	
	return 0;
}

