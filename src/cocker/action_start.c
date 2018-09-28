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
	
	char				netns_path[ PATH_MAX ] ;
	int				netns_fd ;
	
	char				container_hostname_file[ PATH_MAX ] ;
	char				hostname[ HOST_NAME_MAX ] ;
	char				container_images_file[ PATH_MAX ] ;
	char				image[ PATH_MAX ] ;
	char				mount_target[ PATH_MAX ] ;
	char				mount_data[ 4096 ] ;
	
	int				nret = 0 ;
	
	if( STRCMP( cocker_env->net , == , "bridge" ) || STRCMP( cocker_env->net , == , "custom" ) )
	{
		/* setns */
		memset( netns_path , 0x00 , sizeof(netns_path) );
		len = snprintf( netns_path , sizeof(netns_path)-1 , "/var/run/netns/netns-%s" , cocker_env->cmd_para.__container ) ;
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
	nret = ReadFileLine( hostname , sizeof(hostname)-1 , container_hostname_file , sizeof(container_hostname_file) , "%s/%s/hostname" , cocker_env->containers_path_base , cocker_env->cmd_para.__container ) ;
	if( nret )
	{
		printf( "*** ERROR : ReadFileLine hostname in container '%s' failed\n" , cocker_env->cmd_para.__container );
		return -1;
	}
	else if( cocker_env->cmd_para.__debug )
	{
		printf( "read file %s ok\n" , container_hostname_file );
	}
	sethostname( hostname , strlen(hostname) );
	if( cocker_env->cmd_para.__debug )
	{
		printf( "sethostname [%s] ok\n" , hostname );
	}
	
	/* mount filesystem */
	nret = ReadFileLine( image , sizeof(image)-1 , NULL , -1 , "%s/%s/images" , cocker_env->containers_path_base , cocker_env->cmd_para.__container ) ;
	if( nret )
	{
		printf( "*** ERROR : ReadFileLine images in container '%s' failed\n" , cocker_env->cmd_para.__container );
		return -1;
	}
	else if( cocker_env->cmd_para.__debug )
	{
		printf( "read file %s ok\n" , container_images_file );
	}
	
	memset( mount_target , 0x00 , sizeof(mount_target) );
	len = snprintf( mount_target , sizeof(mount_target)-1 , "%s/%s/merged" , cocker_env->containers_path_base , cocker_env->cmd_para.__container ) ;
	if( SNPRINTF_OVERFLOW(len,sizeof(mount_target)-1) )
	{
		printf( "*** ERROR : snprintf failed\n" );
		return -1;
	}
	
	memset( mount_data , 0x00 , sizeof(mount_data) );
	if( image[0] == '\0' )
		len = snprintf( mount_data , sizeof(mount_data)-1 , "upperdir=%s/%s/rwlayer,workdir=%s/%s/workdir" , cocker_env->containers_path_base,cocker_env->cmd_para.__container , cocker_env->containers_path_base,cocker_env->cmd_para.__container ) ;
	else
		len = snprintf( mount_data , sizeof(mount_data)-1 , "lowerdir=%s/%s/rlayer,upperdir=%s/%s/rwlayer,workdir=%s/%s/workdir" , cocker_env->images_path_base,image , cocker_env->containers_path_base,cocker_env->cmd_para.__container , cocker_env->containers_path_base,cocker_env->cmd_para.__container ) ;
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
		printf( "mount [%s][%s][%s][%d][%s] ok\n" , "overlay" , mount_target , "overlay" , MS_MGC_VAL , mount_data );
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
		printf( "mount [%s][%s][%s][%d][%s] ok\n" , "proc" , "/proc" , "proc" , MS_MGC_VAL , "(null)" );
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
	
	char		cmd[ 4096 ] ;
	int		len ;
	
	char		netns_name[ ETHERNET_NAME_MAX + 1 ] ;
	char		netbr_name[ ETHERNET_NAME_MAX + 1 ] ;
	char		veth1_name[ ETHERNET_NAME_MAX + 1 ] ;
	char		veth0_name[ ETHERNET_NAME_MAX + 1 ] ;
	char		veth0_sname[ ETHERNET_NAME_MAX + 1 ] ;
	
	pid_t		pid ;
	char		pid_str[ 20 + 1 ] ;
	
	char		mount_target[ PATH_MAX ] ;
	
	int		nret = 0 ;
	
	Snprintf( cocker_env->container_path_base , sizeof(cocker_env->container_path_base)-1 , "%s/%s" , cocker_env->containers_path_base , cocker_env->cmd_para.__container );
	nret = access( cocker_env->container_path_base , F_OK ) ;
	if( nret == -1 )
	{
		printf( "*** ERROR : container '%s' not found\n" , cocker_env->cmd_para.__container );
		return -1;
	}
	
	/* read pid file */
	nret = ReadFileLine( pid_str , sizeof(pid_str)-1 , NULL , -1 , "%s/%s/pid" , cocker_env->containers_path_base , cocker_env->cmd_para.__container ) ;
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
	
	/* up network */
	if( STRCMP( cocker_env->net , == , "bridge" ) )
	{
		/* up network */
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
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ifconfig %s up" , veth1_name ) ;
		if( nret )
		{
			printf( "*** ERROR : system [%s] failed [%d], errno[%d]\n" , cmd , nret , errno );
			return -1;
		}
		else if( cocker_env->cmd_para.__debug )
		{
			printf( "system [%s] ok\n" , cmd );
		}
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns exec %s ifconfig %s up" , netns_name , veth0_sname ) ;
		if( nret )
		{
			printf( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
			return -1;
		}
		else if( cocker_env->cmd_para.__debug )
		{
			printf( "system [%s] ok\n" , cmd );
		}
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns exec %s ifconfig lo up" , netns_name ) ;
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
	
	/* create container */
	pid = clone( VHostEntry , stack_bottom+sizeof(stack_bottom) , CLONE_NEWNS|CLONE_NEWPID|CLONE_NEWIPC|CLONE_NEWUTS|CLONE_NEWNET , (void*)cocker_env ) ;
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
	nret = WriteFileLine( pid_str , container_pid_file , sizeof(container_pid_file) , "%s/%s/pid" , cocker_env->containers_path_base , cocker_env->cmd_para.__container ) ;
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
	if( STRCMP( cocker_env->net , == , "bridge" ) )
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
	
	return 0;
}

