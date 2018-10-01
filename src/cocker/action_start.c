#include "cocker_in.h"

static unsigned char	stack_bottom[ 1024 * 1024 ] = {0} ;

static int VHostEntry( void *p )
{
	struct CockerEnvironment	*env = (struct CockerEnvironment *)p ;
	
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
	
	if( STRCMP( env->net , == , "BRIDGE" ) || STRCMP( env->net , == , "CUSTOM" ) )
	{
		/* setns */
		memset( netns_path , 0x00 , sizeof(netns_path) );
		len = snprintf( netns_path , sizeof(netns_path)-1 , "/var/run/netns/%s" , env->netns_name ) ;
		IxTPx( SNPRINTF_OVERFLOW(len,sizeof(netns_path)-1) , exit(9) , "*** ERROR : netns path overflow\n" )
		
		netns_fd = open( netns_path , O_RDONLY ) ;
		IxTPx( (netns_fd==-1) , exit(9) , "*** ERROR : open netns path failed , errno[%d]\n" , errno )
		
		nret = setns( netns_fd , CLONE_NEWNET ) ;
		IxTPx( nret == -1 , exit(9) , "*** ERROR : setns failed , errno[%d]\n" , errno )
		EIDTP( "setns %s ok\n" , netns_path )
		
		close( netns_fd );
	}
	
	/* basic seting */
	setuid(0);
	setgid(0);
	setgroups(0,NULL);
	
	/* sethostname */
	nret = ReadFileLine( hostname , sizeof(hostname)-1 , container_hostname_file , sizeof(container_hostname_file) , "%s/%s/hostname" , env->containers_path_base , env->container_id ) ;
	INTPRx( (exit(9)) , "*** ERROR : ReadFileLine hostname in container '%s' failed\n" , env->container_id )
	EIDTP( "read file %s ok\n" , container_hostname_file )
	
	TrimEnter( hostname );
	sethostname( hostname , strlen(hostname) );
	
	IDTP( "sethostname [%s] ok\n" , hostname )
	
	/* mount filesystem */
	nret = ReadFileLine( image , sizeof(image)-1 , NULL , -1 , "%s/%s/images" , env->containers_path_base , env->container_id ) ;
	INTPRx( (exit(9)) , "*** ERROR : ReadFileLine images in container '%s' failed\n" , env->container_id )
	EIDTP( "read file %s ok\n" , container_images_file )
	
	TrimEnter( image );
	
	memset( mount_target , 0x00 , sizeof(mount_target) );
	len = snprintf( mount_target , sizeof(mount_target)-1 , "%s/%s/merged" , env->containers_path_base , env->container_id ) ;
	IxTPx( SNPRINTF_OVERFLOW(len,sizeof(mount_target)-1) , (exit(9)) , "*** ERROR : snprintf overflow\n" )
	
	memset( mount_data , 0x00 , sizeof(mount_data) );
	if( image[0] == '\0' )
		len = snprintf( mount_data , sizeof(mount_data)-1 , "upperdir=%s/%s/rwlayer,workdir=%s/%s/workdir" , env->containers_path_base,env->container_id , env->containers_path_base,env->container_id ) ;
	else
		len = snprintf( mount_data , sizeof(mount_data)-1 , "lowerdir=%s/%s/rlayer,upperdir=%s/%s/rwlayer,workdir=%s/%s/workdir" , env->images_path_base,image , env->containers_path_base,env->container_id , env->containers_path_base,env->container_id ) ;
	IxTPx( SNPRINTF_OVERFLOW(len,sizeof(mount_data)-1) , exit(9) , "*** ERROR : snprintf overflow\n" )
	
	nret = mount( "overlay" , mount_target , "overlay" , MS_MGC_VAL , (void*)mount_data ) ;
	I1TPRx( (exit(9)) , "*** ERROR : mount[%s][%s] failed , errno[%d]\n" , mount_data , mount_target , errno )
	EIDTP( "mount [%s][%s][%s][0x%X][%s] ok\n" , "overlay" , mount_target , "overlay" , MS_MGC_VAL , mount_data )
	
	/* chroot */
	chroot( mount_target );
	if( env->cmd_para.__debug )
	{
		printf( "chroot [%s] ok\n" , mount_target );
	}
	if( env->cmd_para.__debug )
	{
		printf( "chdir [%s] ok\n" , "/root" );
	}
	
	/* mount /proc */
	mount( "proc" , "/proc" , "proc" , MS_MGC_VAL , NULL );
	IDTP( "mount [%s][%s][%s][0x%X][%s] ok\n" , "proc" , "/proc" , "proc" , MS_MGC_VAL , "(null)" )
	
	/* mount /dev/pts */
	mount( "devpts" , "/dev/pts" , "devpts" , MS_MGC_VAL , NULL );
	IDTP( "mount [%s][%s][%s][0x%X][%s] ok\n" , "devpts" , "/dev/pts" , "devpts" , MS_MGC_VAL , "(null)" )
	
	/* change /root */
	chdir( "/root" );
	
	/* execl */
	nret = execl( "/bin/bash" , "bash" , "--login" , NULL ) ;
	I1TPRx( exit(9) , "*** ERROR : execl failed , errno[%d]\n" , errno )
	
	return 0;
}

int DoAction_start( struct CockerEnvironment *env )
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
	Snprintf( env->container_path_base , sizeof(env->container_path_base)-1 , "%s/%s" , env->containers_path_base , env->cmd_para.__container_id );
	nret = access( env->container_path_base , F_OK ) ;
	INTPR1( "*** ERROR : container '%s' not found\n" , env->cmd_para.__container_id )
	
	/* read pid file */
	nret = ReadFileLine( pid_str , sizeof(pid_str)-1 , NULL , -1 , "%s/%s/pid" , env->containers_path_base , env->container_id ) ;
	if( nret == 0 )
	{
		TrimEnter( pid_str );
		pid = atoi(pid_str) ;
		if( pid > 0 )
		{
			nret = kill( pid , 0 ) ;
			I0TPR1( "*** ERROR : container is already running\n" )
		}
	}
	
	/* read net file */
	nret = ReadFileLine( env->net , sizeof(env->net) , container_net_file , sizeof(container_net_file) , "%s/net" , env->container_path_base ) ;
	ILTPR1( "*** ERROR : ReadFileLine net failed\n" )
	EIDTP( "read file %s ok\n" , container_net_file )
	
	TrimEnter( env->net );
	
	/* up network */
	if( STRCMP( env->net , == , "BRIDGE" ) )
	{
		/* up network */
		nret = ReadFileLine( env->vip , sizeof(env->vip) , container_vip_file , sizeof(container_vip_file) , "%s/vip" , env->container_path_base ) ;
		ILTPR1( "*** ERROR : ReadFileLine net failed\n" )
		EIDTP( "read file %s ok\n" , container_vip_file )
		
		TrimEnter( env->vip );
		
		memset( env->port_mapping , 0x00 , sizeof(env->port_mapping) );
		nret = ReadFileLine( env->port_mapping , sizeof(env->port_mapping) , container_port_mapping_file , sizeof(container_port_mapping_file) , "%s/port_mapping" , env->container_path_base ) ;
		ILTx( (memset( env->port_mapping,0x00,sizeof(env->port_mapping))) )
		EIDTP( "read file %s ok\n" , container_port_mapping_file )
		
		TrimEnter( env->port_mapping );
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip link add %s type veth peer name %s" , env->veth1_name , env->veth0_name ) ;
		INTPR1( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
		EIDTP( "system [%s] ok\n" , cmd )
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "brctl addif %s %s" , env->netbr_name , env->veth1_name ) ;
		INTPR1( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
		EIDTP( "system [%s] ok\n" , cmd )
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ifconfig %s 0.0.0.0" , env->veth1_name ) ;
		INTPR1( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
		EIDTP( "system [%s] ok\n" , cmd )
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip link set %s netns %s" , env->veth0_name , env->netns_name ) ;
		INTPR1( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
		EIDTP( "system [%s] ok\n" , cmd )
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns exec %s ip link set %s name %s" , env->netns_name , env->veth0_name , env->veth0_sname ) ;
		INTPR1( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
		EIDTP( "system [%s] ok\n" , cmd )
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns exec %s ifconfig %s %s" , env->netns_name , env->veth0_sname , env->vip ) ;
		INTPR1( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
		EIDTP( "system [%s] ok\n" , cmd )
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns exec %s route add default gw %s netmask 0.0.0.0 dev %s" , env->netns_name , env->netbr_ip , env->veth0_sname ) ;
		INTPR1( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
		EIDTP( "system [%s] ok\n" , cmd )
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ifconfig %s up" , env->veth1_name ) ;
		INTPR1( "*** ERROR : system [%s] failed [%d], errno[%d]\n" , cmd , nret , errno )
		EIDTP( "system [%s] ok\n" , cmd )
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns exec %s ifconfig %s up" , env->netns_name , env->veth0_sname ) ;
		INTPR1( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
		EIDTP( "system [%s] ok\n" , cmd )
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns exec %s ifconfig lo up" , env->netns_name ) ;
		INTPR1( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
		EIDTP( "system [%s] ok\n" , cmd )
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "iptables -t nat -A POSTROUTING -o %s -j MASQUERADE" , env->host_eth_name ) ;
		INTPR1( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
		EIDTP( "system [%s] ok\n" , cmd )
		
		if( env->port_mapping[0] )
		{
			memset( src_port_str , 0x00 , sizeof(src_port_str) );
			sscanf( env->port_mapping , "%[^:]:%d" , src_port_str , & (env->dst_port) );
			env->src_port = atoi(src_port_str) ;
			IxTPR1( (env->src_port<=0||env->dst_port<=0) , "*** ERROR : file port_mapping value [%s] invalid\n" , env->port_mapping )
			
			nret = SnprintfAndSystem( cmd , sizeof(cmd) , "iptables -t nat -A PREROUTING -i %s -p tcp -m tcp --dport %d -j DNAT --to-destination %s:%d" , env->host_eth_name , env->src_port , env->vip , env->dst_port ) ;
			INTPR1( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
			EIDTP( "system [%s] ok\n" , cmd )
		}
	}
	
	/* create container */
	if( STRCMP( env->net , == , "HOST" ) )
	{
		pid = clone( VHostEntry , stack_bottom+sizeof(stack_bottom) , CLONE_NEWNS|CLONE_NEWPID|CLONE_NEWIPC|CLONE_NEWUTS , (void*)env ) ;
	}
	else
	{
		pid = clone( VHostEntry , stack_bottom+sizeof(stack_bottom) , CLONE_NEWNS|CLONE_NEWPID|CLONE_NEWIPC|CLONE_NEWUTS|CLONE_NEWNET , (void*)env ) ;
	}
	IxTPR1( (pid==-1) , "*** ERROR : clone failed[%d] , errno[%d]\n" , pid , errno )
	
	/* write pid file */
	memset( pid_str , 0x00 , sizeof(pid_str) );
	snprintf( pid_str , sizeof(pid_str)-1 , "%d" , pid );
	nret = WriteFileLine( pid_str , container_pid_file , sizeof(container_pid_file) , "%s/pid" , env->container_path_base , env->container_id ) ;
	INTPR1( "*** ERROR : WriteFileLine failed[%d] , errno[%d]\n" , nret , errno )
	EIDTP( "write file %s ok\n" , container_pid_file )
	
	/* wait for container end */
	while(1)
	{
		pid = waitpid( -1 , NULL , __WCLONE ) ;
		if( pid == -1 )
		{
			if( errno == ECHILD )
				break;
			
			printf( "*** ERROR : waitpid failed , errno[%d]\n" , errno ); fflush(stdout);
			return -1;
		}
	}
	
	/* down network */
	if( STRCMP( env->net , == , "BRIDGE" ) )
	{
		/* down network */
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "iptables -t nat -D POSTROUTING -o %s -j MASQUERADE" , env->host_eth_name ) ;
		INTP( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
		EIDTP( "system [%s] ok\n" , cmd )
		
		if( env->port_mapping[0] )
		{
			nret = SnprintfAndSystem( cmd , sizeof(cmd) , "iptables -t nat -D PREROUTING -i %s -p tcp -m tcp --dport %d -j DNAT --to-destination %s:%d" , env->host_eth_name , env->src_port , env->vip , env->dst_port ) ;
			INTPR1( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
			EIDTP( "system [%s] ok\n" , cmd )
		}
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ifconfig %s down" , env->veth1_name ) ;
		INTP( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
		EIDTP( "system [%s] ok\n" , cmd )
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns exec %s ifconfig %s down" , env->netns_name , env->veth0_sname ) ;
		INTP( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
		EIDTP( "system [%s] ok\n" , cmd )
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns exec %s ifconfig lo down" , env->netns_name ) ;
		INTP( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
		EIDTP( "system [%s] ok\n" , cmd )
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "brctl delif %s %s" , env->netbr_name , env->veth1_name ) ;
		INTPFR1( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
		EIDTP( "system [%s] ok\n" , cmd )
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip link del %s" , env->veth1_name ) ;
		INTPFR1( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
		EIDTP( "system [%s] ok\n" , cmd )
	}
	
	/* cleanup pid file */
	nret = unlink( container_pid_file ) ;
	INTP( "*** ERROR : unlink %s failed\n" , container_pid_file )
	EIDTP( "unlink %s ok\n" , container_pid_file )
	
	/* umount */
	memset( mount_target , 0x00 , sizeof(mount_target) );
	len = snprintf( mount_target , sizeof(mount_target)-1 , "%s/merged/proc" , env->container_path_base ) ;
	IxTPR1( SNPRINTF_OVERFLOW(len,sizeof(mount_target)-1) , "*** ERROR : snprintf overflow\n" )
	E
	{
		nret = umount( mount_target ) ;
		I1TP( "*** ERROR : umount proc failed\n" )
		EIDTP( "umount %s ok\n" , mount_target )
	}
	
	memset( mount_target , 0x00 , sizeof(mount_target) );
	len = snprintf( mount_target , sizeof(mount_target)-1 , "%s/merged/dev/pts" , env->container_path_base ) ;
	IxTPR1( SNPRINTF_OVERFLOW(len,sizeof(mount_target)-1) , "*** ERROR : snprintf overflow\n" )
	E
	{
		nret = umount( mount_target ) ;
		I1TP( "*** ERROR : umount proc failed\n" )
		EIDTP( "umount %s ok\n" , mount_target )
	}
	
	memset( mount_target , 0x00 , sizeof(mount_target) );
	len = snprintf( mount_target , sizeof(mount_target)-1 , "%s/merged" , env->container_path_base ) ;
	IxTPR1( SNPRINTF_OVERFLOW(len,sizeof(mount_target)-1) , "*** ERROR : snprintf overflow\n" )
	E
	{
		nret = umount( mount_target ) ;
		I1TP( "*** ERROR : umount merged failed\n" )
		EIDTP( "umount %s ok\n" , mount_target )
	}
	
	return 0;
}

