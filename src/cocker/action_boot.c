/*
 * cocker - Container Machine Engine
 * author	: calvin
 * email	: calvinwilliams@163.com
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#include "cocker_in.h"

static unsigned char	stack_bottom[ 8*1024*1024 ] = { 0 } ;

static int	g_TERM_flag = 0 ;

static void sig_proc( int sig_no )
{
	g_TERM_flag = 1 ;
	return;
}

static int InvertLowerDirs( struct CockerEnvironment *env , char *images , char *mount_data , int *p_len , int *p_remain_len )
{
	int		l ;
	char		*p = NULL ;
	char		*p2 = NULL ;
	char		version[ PATH_MAX + 1 ] ;
	
	int		nret = 0 ;
	
	if( images )
	{
		p = strtok( images , "," ) ;
	}
	else
	{
		p = strtok( NULL , "," ) ;
	}
	if( p )
	{
		nret = InvertLowerDirs( env , NULL , mount_data , p_len , p_remain_len ) ;
		if( nret )
			return nret;
		
		if( (*p_len) == 0 )
		{
			l = snprintf( mount_data+(*p_len) , (*p_remain_len) , "lowerdir=" ) ;
			IxTEx( SNPRINTF_OVERFLOW(l,(*p_remain_len)) , exit(9) , "*** ERROR : snprintf overflow\n" )
			(*p_len) += l ;
			(*p_remain_len) -= l ;
		}
		else
		{
			l = snprintf( mount_data+(*p_len) , (*p_remain_len) , ":" ) ;
			IxTEx( SNPRINTF_OVERFLOW(l,(*p_remain_len)) , exit(9) , "*** ERROR : snprintf overflow\n" )
			(*p_len) += l ;
			(*p_remain_len) -= l ;
		}
		
		p2 = strchr( p , ':' ) ;
		if( p2 == NULL )
		{
			Snprintf( env->version_path_base , sizeof(env->version_path_base) , "%s/%s" , env->images_path_base , p ) ;
			nret = GetMaxVersionPath( env->version_path_base , version , sizeof(version) ) ;
			INTER1( "*** ERROR : GetMaxVersionPath[%s] failed[%d]\n" , env->version_path_base , nret )
			
			l = snprintf( mount_data+(*p_len) , (*p_remain_len) , "%s/%s/%s/rlayer" , env->images_path_base , p , version ) ;
		}
		else
		{
			l = snprintf( mount_data+(*p_len) , (*p_remain_len) , "%s/%.*s/%s/rlayer" , env->images_path_base , (int)(p2-p) , p , p2+1 ) ;
		}
		IxTEx( SNPRINTF_OVERFLOW(l,(*p_remain_len)) , exit(9) , "*** ERROR : snprintf overflow\n" )
		(*p_len) += l ;
		(*p_remain_len) -= l ;
		
		if( images )
		{
			l = snprintf( mount_data+(*p_len) , (*p_remain_len) , "," ) ;
			IxTEx( SNPRINTF_OVERFLOW(l,(*p_remain_len)) , exit(9) , "*** ERROR : snprintf overflow\n" )
			(*p_len) += l ;
			(*p_remain_len) -= l ;
		}
	}
	
	return 0;
}

static int CloneEntry( void *pv )
{
	struct CockerEnvironment	*env = (struct CockerEnvironment *)pv ;
	
	char				container_net_file[ PATH_MAX + 1 ] ;
	char				net[ NET_LEN_MAX + 1 ] ;
	int				len ;
	int				l ;
	int				remain_len ;
	
	char				netns_path[ PATH_MAX + 1 ] ;
	int				netns_fd ;
	
	char				container_hostname_file[ PATH_MAX + 1 ] ;
	char				hostname[ HOST_NAME_MAX + 1 ] ;
	char				container_image_file[ PATH_MAX + 1 ] ;
	char				image[ PATH_MAX + 1 ] ;
	char				container_volume_file[ PATH_MAX + 1 ] ;
	FILE				*container_volume_fp = NULL ;
	char				mount_target[ PATH_MAX + 1 ] ;
	char				mount_data[ 4096 ] ;
	struct CockerVolume		volume ;
	char				mount_volume[ PATH_MAX + 1 ] ;
	
	char				cmd[ 4096 ] ;
	
	char				pid_str[ 20 + 1 ] ;
	char				cgroup_cpuset_cpus_file[ PATH_MAX + 1 ] ;
	char				cgroup_cpuset_mems_file[ PATH_MAX + 1 ] ;
	char				cgroup_cpuset_tasks_file[ PATH_MAX + 1 ] ;
	char				cgroup_cpu_cfs_period_us_file[ PATH_MAX + 1 ] ;
	char				cgroup_cpu_cfs_quota_us_file[ PATH_MAX + 1 ] ;
	char				cgroup_cpu_tasks_file[ PATH_MAX + 1 ] ;
	char				cgroup_memory_limit_in_bytes_file[ PATH_MAX + 1 ] ;
	char				cgroup_memory_memsw_limit_in_bytes_file[ PATH_MAX + 1 ] ;
	char				cgroup_memory_tasks_file[ PATH_MAX + 1 ] ;
	
	char				fd_str[ 20 + 1 ] ;
	
	int				argc ;
	char				*argv[64] = { NULL } ;
#if 1
	char				*p = NULL ;
	char				*p2 = NULL ;
#endif
	int				i ;
	
	int				nret = 0 ;
	
	signal( SIGTERM , SIG_DFL );
	
	SetLogcFile( "/var/cocker/cocker.log" );
	SetLogcLevel( LOGCLEVEL_INFO );
	
	memset( net , 0x00 , sizeof(net) );
	memset( container_net_file , 0x00 , sizeof(container_net_file) );
	nret = ReadFileLine( net , sizeof(net) , container_net_file , sizeof(container_net_file) , "%s/net" , env->container_path_base ) ;
	ILTER1( "*** ERROR : ReadFileLine net failed\n" )
	EIDTI( "read file %s ok\n" , container_net_file )
	
	TrimEnter( net );
	
	if( STRCMP( net , == , "BRIDGE" ) || STRCMP( net , == , "CUSTOM" ) )
	{
		/* setns */
		memset( netns_path , 0x00 , sizeof(netns_path) );
		len = snprintf( netns_path , sizeof(netns_path)-1 , "/var/run/netns/%s" , env->netns_name ) ;
		IxTEx( SNPRINTF_OVERFLOW(len,sizeof(netns_path)-1) , exit(9) , "*** ERROR : netns path overflow\n" )
		
		netns_fd = open( netns_path , O_RDONLY ) ;
		IxTEx( (netns_fd==-1) , exit(9) , "*** ERROR : open netns path failed , errno[%d]\n" , errno )
		EIDTI( "open %s ok\n" , netns_path )
		
		nret = setns( netns_fd , CLONE_NEWNET ) ;
		IxTEx( nret == -1 , exit(9) , "*** ERROR : setns failed , errno[%d]\n" , errno )
		EIDTI( "setns %s ok\n" , netns_path )
		
		close( netns_fd );
	}
	
	/* basic seting */
	setuid(0);
	setgid(0);
	setgroups(0,NULL);
	
	/* sethostname */
	memset( hostname , 0x00 , sizeof(hostname) );
	memset( container_hostname_file , 0x00 , sizeof(container_hostname_file) );
	nret = ReadFileLine( hostname , sizeof(hostname)-1 , container_hostname_file , sizeof(container_hostname_file) , "%s/hostname" , env->container_path_base ) ;
	INTEx( (exit(9)) , "*** ERROR : ReadFileLine hostname in container '%s' failed\n" , env->cmd_para.__container )
	EIDTI( "read file %s ok\n" , container_hostname_file )
	
	TrimEnter( hostname );
	if( hostname[0] )
	{
		sethostname( hostname , strlen(hostname) );
	}
	
	IDTI( "sethostname [%s] ok\n" , hostname )
	
	/* mount filesystem */
	memset( mount_target , 0x00 , sizeof(mount_target) );
	len = snprintf( mount_target , sizeof(mount_target)-1 , "%s/merged" , env->container_path_base ) ;
	IxTEx( SNPRINTF_OVERFLOW(len,sizeof(mount_target)-1) , (exit(9)) , "*** ERROR : snprintf overflow\n" )
	
	memset( image , 0x00 , sizeof(image) );
	memset( container_image_file , 0x00 , sizeof(container_image_file) );
	nret = ReadFileLine( image , sizeof(image)-1 , container_image_file , sizeof(container_image_file) , "%s/image" , env->container_path_base ) ;
	if( nret )
	{
		char		container_rwlayer_path[ PATH_MAX + 1 ] ;
		
		memset( container_image_file , 0x00 , sizeof(container_image_file) );
		memset( image , 0x00 , sizeof(image) );
		
		Snprintf( container_rwlayer_path , sizeof(container_rwlayer_path) , "%s/rwlayer" , env->container_path_base );
		nret = mount( container_rwlayer_path , mount_target , NULL , MS_MGC_VAL|MS_BIND , NULL ) ;
		I1TERx( (exit(9)) , "*** ERROR : mount[%s][%s] failed , errno[%d]\n" , container_rwlayer_path , mount_target , errno )
		EIDTI( "mount [%s][%s][%s][0x%X][%s] ok\n" , container_rwlayer_path , mount_target , "(null)" , MS_MGC_VAL|MS_BIND , "(null)" )
	}
	else
	{
		/*
		I0TI( "read file %s ok\n" , container_image_file )
		*/
		
		TrimEnter( image );
		
		memset( mount_data , 0x00 , sizeof(mount_data) );
		if( image[0] == '\0' )
		{
			len = snprintf( mount_data , sizeof(mount_data)-1 , "upperdir=%s/rwlayer,workdir=%s/workdir" , env->container_path_base , env->container_path_base ) ;
			IxTEx( SNPRINTF_OVERFLOW(len,sizeof(mount_data)-1) , exit(9) , "*** ERROR : snprintf overflow\n" )
		}
		else
		{
			len = 0 ;
			remain_len = sizeof(mount_data)-1 ;
			nret = InvertLowerDirs( env , image , mount_data , & len , & remain_len ) ;
			INTEx( exit(9) , "*** ERROR : InvertLowerDirs overflow\n" )
			
			l = snprintf( mount_data+len , remain_len , "upperdir=%s/rwlayer,workdir=%s/workdir" , env->container_path_base , env->container_path_base ) ;
			IxTEx( SNPRINTF_OVERFLOW(l,remain_len) , exit(9) , "*** ERROR : snprintf overflow\n" )
			len += l ;
			remain_len -= l ;
		}
		nret = mount( "overlay" , mount_target , "overlay" , MS_MGC_VAL , (void*)mount_data ) ;
		I1TERx( (exit(9)) , "*** ERROR : mount[%s][%s] failed , errno[%d]\n" , mount_data , mount_target , errno )
		EIDTI( "mount [%s][%s][%s][0x%X][%s] ok\n" , "overlay" , mount_target , "overlay" , MS_MGC_VAL , mount_data )
	}
	
	/* mount volumes */
	Snprintf( container_volume_file , sizeof(container_volume_file) , "%s/volume" , env->container_path_base ) ;
	container_volume_fp = fopen( container_volume_file , "r" ) ;
	if( container_volume_fp )
	{
		char		file_buffer[ 4096 ] ;
		char		*p = NULL ;
		
		while(1)
		{
			memset( file_buffer , 0x00 , sizeof(file_buffer) );
			if( fgets( file_buffer , sizeof(file_buffer) , container_volume_fp ) == NULL )
				break;
			TrimEnter( file_buffer );
			
			volume.host_path = file_buffer ;
			p = strchr( volume.host_path , ':' ) ;
			if( p )
			{
				volume.host_path_len = p - volume.host_path ;
				volume.container_path = p + 1 ;
				(*p) = '\0' ;
			}
			else
			{
				volume.host_path_len = strlen(volume.host_path) ;
				volume.container_path = volume.host_path ;
			}
			
			memset( mount_volume , 0x00 , sizeof(mount_volume) );
			len = snprintf( mount_volume , sizeof(mount_volume)-1 , "%s/merged%s" , env->container_path_base , volume.container_path ) ;
			IxTEx( SNPRINTF_OVERFLOW(len,sizeof(mount_volume)-1) , (exit(9)) , "*** ERROR : snprintf overflow\n" )
			
			mkdir( mount_volume , 0777 );
			nret = mount( volume.host_path , mount_volume , NULL , MS_MGC_VAL|MS_BIND , NULL ) ;
			I1TERx( (exit(9)) , "*** ERROR : mount[%s][%s] failed , errno[%d]\n" , volume.host_path , mount_volume , errno )
			EIDTI( "mount [%s][%s][%s][0x%X][%s] ok\n" , volume.host_path , mount_volume , "(null)" , MS_MGC_VAL|MS_BIND , "(null)" )
		}
		
		fclose( container_volume_fp );
	}
	
	/* enable system limits */
	memset( pid_str , 0x00 , sizeof(pid_str) );
	snprintf( pid_str , sizeof(pid_str)-1 , "%d" , getpid() );
	
	if( env->cgroup_enable )
	{
		if( env->cmd_para.__cpus )
		{
			nret = SnprintfAndSystem( cmd , sizeof(cmd) , "mkdir %s/cpuset/cocker_%s" , CGROUP_PATH , env->cmd_para.__container ) ;
			INTER1( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
			EIDTI( "system [%s] ok\n" , cmd )
			
			nret = WriteFileLine( env->cmd_para.__cpus , cgroup_cpuset_cpus_file , sizeof(cgroup_cpuset_cpus_file) , "%s/cpuset/cocker_%s/cpuset.cpus" , CGROUP_PATH , env->cmd_para.__container ) ;
			INTER1( "*** ERROR : WriteFileLine cpuset.cpus failed[%d] , errno[%d]\n" , nret , errno )
			EIDTI( "write file %s ok\n" , cgroup_cpuset_cpus_file )
			
			nret = WriteFileLine( env->cmd_para.__cpus , cgroup_cpuset_mems_file , sizeof(cgroup_cpuset_mems_file) , "%s/cpuset/cocker_%s/cpuset.mems" , CGROUP_PATH , env->cmd_para.__container ) ;
			INTER1( "*** ERROR : WriteFileLine cpuset.mems failed[%d] , errno[%d]\n" , nret , errno )
			EIDTI( "write file %s ok\n" , cgroup_cpuset_mems_file )
			
			nret = WriteFileLine( pid_str , cgroup_cpuset_tasks_file , sizeof(cgroup_cpuset_tasks_file) , "%s/cpuset/cocker_%s/tasks" , CGROUP_PATH , env->cmd_para.__container ) ;
			INTER1( "*** ERROR : WriteFileLine cpuset.tasks failed[%d] , errno[%d]\n" , nret , errno )
			EIDTI( "write file %s ok\n" , cgroup_cpuset_tasks_file )
		}
		
		if( env->cmd_para.__cpu_quota )
		{
			nret = SnprintfAndSystem( cmd , sizeof(cmd) , "mkdir %s/cpu,cpuacct/cocker_%s" , CGROUP_PATH , env->cmd_para.__container ) ;
			INTER1( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
			EIDTI( "system [%s] ok\n" , cmd )
			
			nret = WriteFileLine( "1000000" , cgroup_cpu_cfs_period_us_file , sizeof(cgroup_cpu_cfs_period_us_file) , "%s/cpu,cpuacct/cocker_%s/cpu.cfs_period_us" , CGROUP_PATH , env->cmd_para.__container ) ;
			INTER1( "*** ERROR : WriteFileLine cpu.cfs_period_us failed[%d] , errno[%d]\n" , nret , errno )
			EIDTI( "write file %s ok\n" , cgroup_cpu_cfs_period_us_file )
			
			if( env->cmd_para.__cpu_quota[strlen(env->cmd_para.__cpu_quota)-1] == '%' )
			{
				char	buf[ 20 + 1 ] ;
				
				memset( buf , 0x00 , sizeof(buf) );
				snprintf( buf , sizeof(buf)-1 , "%d" , 1000000/100*atoi(env->cmd_para.__cpu_quota) );
				nret = WriteFileLine( buf , cgroup_cpu_cfs_quota_us_file , sizeof(cgroup_cpu_cfs_quota_us_file) , "%s/cpu,cpuacct/cocker_%s/cpu.cfs_quota_us" , CGROUP_PATH , env->cmd_para.__container ) ;
			}
			else
			{
				nret = WriteFileLine( env->cmd_para.__cpu_quota , cgroup_cpuset_mems_file , sizeof(cgroup_cpuset_mems_file) , "%s/cpu,cpuacct/cocker_%s/cpu.cfs_quota_us" , CGROUP_PATH , env->cmd_para.__container ) ;
			}
			INTER1( "*** ERROR : WriteFileLine cpu.cfs_quota_us failed[%d] , errno[%d]\n" , nret , errno )
			EIDTI( "write file %s ok\n" , cgroup_cpu_cfs_quota_us_file )
			
			nret = WriteFileLine( pid_str , cgroup_cpu_tasks_file , sizeof(cgroup_cpu_tasks_file) , "%s/cpu,cpuacct/cocker_%s/tasks" , CGROUP_PATH , env->cmd_para.__container ) ;
			INTER1( "*** ERROR : WriteFileLine cpu.tasks failed[%d] , errno[%d]\n" , nret , errno )
			EIDTI( "write file %s ok\n" , cgroup_cpu_tasks_file )
		}
		
		if( env->cmd_para.__mem_limit )
		{
			nret = SnprintfAndSystem( cmd , sizeof(cmd) , "mkdir %s/memory/cocker_%s" , CGROUP_PATH , env->cmd_para.__container ) ;
			INTER1( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
			EIDTI( "system [%s] ok\n" , cmd )
			
			nret = WriteFileLine( env->cmd_para.__mem_limit , cgroup_memory_limit_in_bytes_file , sizeof(cgroup_memory_limit_in_bytes_file) , "%s/memory/cocker_%s/memory.limit_in_bytes" , CGROUP_PATH , env->cmd_para.__container ) ;
			INTER1( "*** ERROR : WriteFileLine memory.limit_in_bytes failed[%d] , errno[%d]\n" , nret , errno )
			EIDTI( "write file %s ok\n" , cgroup_memory_limit_in_bytes_file )
			
			nret = WriteFileLine( env->cmd_para.__mem_limit , cgroup_memory_memsw_limit_in_bytes_file , sizeof(cgroup_memory_memsw_limit_in_bytes_file) , "%s/memory/cocker_%s/memory.memsw.limit_in_bytes" , CGROUP_PATH , env->cmd_para.__container ) ;
			INTER1( "*** ERROR : WriteFileLine memory.memsw.limit_in_bytes failed[%d] , errno[%d]\n" , nret , errno )
			EIDTI( "write file %s ok\n" , cgroup_memory_memsw_limit_in_bytes_file )
			
			nret = WriteFileLine( pid_str , cgroup_memory_tasks_file , sizeof(cgroup_memory_tasks_file) , "%s/memory/cocker_%s/tasks" , CGROUP_PATH , env->cmd_para.__container ) ;
			INTER1( "*** ERROR : WriteFileLine memory.tasks failed[%d] , errno[%d]\n" , nret , errno )
			EIDTI( "write file %s ok\n" , cgroup_memory_tasks_file )
		}
	}
	
	/* chroot
	----------------------------------------------------------------------------------------------------
	*/
	/*
	I( "chroot [%s] ...\n" , mount_target )
	*/
	nret = chroot( mount_target ) ;
	
	SetLogcFile( "/cocker.log" );
	SetLogcLevel( LOGCLEVEL_INFO );
	
	if( env->cmd_para.__debug )
	{
		I( "chroot [%s] ok\n" , mount_target )
	}
	
	/* mount /proc */
	mount( "proc" , "/proc" , "proc" , MS_MGC_VAL , NULL );
	IDTI( "mount [%s][%s][%s][0x%X][%s] ok\n" , "proc" , "/proc" , "proc" , MS_MGC_VAL , "(null)" )
	
	/* mount /dev/pts */
	mount( "devpts" , "/dev/pts" , "devpts" , MS_MGC_VAL , NULL );
	IDTI( "mount [%s][%s][%s][0x%X][%s] ok\n" , "devpts" , "/dev/pts" , "devpts" , MS_MGC_VAL , "(null)" )
	
	/* change /root */
	/*
	I( "chdir [%s] ...\n" , "/root" )
	*/
	nret = chdir( "/root" ) ;
	if( env->cmd_para.__debug )
	{
		I( "chdir [%s] ok\n" , "/root" )
	}
	
	/* save alive pipe fd */
	Snprintf( fd_str , sizeof(fd_str) , "%d" , env->alive_pipe[0] );
	setenv( "COCKER_ALIVE_PIPE" , fd_str , 1 );
	
	close( env->alive_pipe[1] );
	
	/* execl */
	for( i = 1 ; i <= 31 ; i++ )
		signal( i , SIG_DFL );
	
	if( env->cmd_para.__exec == NULL )
	{
		argc = 0 ;
		argv[argc++] = "/bin/cockerinit" ;
		argv[argc++] = "cockerinit" ;
		argv[argc++] = "--container" ;
		argv[argc++] = env->cmd_para.__container ;
		argv[argc++] = NULL ;
		for( i = 0 ; i < argc ; i++ )
			I( "argv[%d]=[%s]\n" , i , argv[i] )
		nret = execv( argv[0] , argv+1 ) ;
	}
	else
	{
		argc = 0 ;
		p = strtok( env->cmd_para.__exec , " \t" ) ;
		while( p )
		{
			if( argc >= sizeof(argv)/sizeof(argv[0])-1 )
			ER1( "*** ERROR : exec too long\n" )
			
			if( argc == 0 )
			{
				argv[argc++] = p ;
				p2 = strrchr( p , '/' ) ;
				if( p2 )
					argv[argc++] = p2 + 1 ;
				else
					argv[argc++] = p ;
			}
			else
			{
				argv[argc++] = p ;
			}
			
			p = strtok( NULL , " \t" ) ;
		}
		argv[argc++] = NULL ;
		
		for( i = 0 ; i < argc ; i++ )
			I( "argv[%d]=[%s]\n" , i , argv[i] )
		nret = execv( argv[0] , argv+1 ) ;
#if 0
		nret = execl( "/bin/bash" , "bash" , "-l" , "-c" , env->cmd_para.__exec , NULL ) ;
#endif
	}
	I1TERx( exit(9) , "*** ERROR : execl failed , errno[%d]\n" , errno )
	
	exit(9);
}

int CleanContainerResource( struct CockerEnvironment *env )
{
	char			net[ NET_LEN_MAX + 1 ] ;
	char			vip[ IP_LEN_MAX + 1 ] ;
	
	char			cmd[ 4096 ] ;
	int			len ;
	
	char			container_pid_file[ PATH_MAX + 1 ] ;
	char			container_net_file[ PATH_MAX + 1 ] ;
	char			container_vip_file[ PATH_MAX + 1 ] ;
	char			container_port_mapping_file[ PATH_MAX + 1 ] ;
	
	char			pid_str[ PID_LEN_MAX + 1 ] ;
	
	char			port_mapping[ PORT_MAP_LEN_MAX + 1 ] ;
	char			*p = NULL ;
	char			src_port_str[ 20 + 1 ] ;
	
	char			container_volume_file[ PATH_MAX + 1 ] ;
	FILE			*container_volume_fp = NULL ;
	struct CockerVolume	volume ;
	char			mount_volume[ PATH_MAX + 1 ] ;
	char			mount_target[ PATH_MAX + 1 ] ;
	
	int			nret = 0 ;
	
	/* read pid file */
	memset( pid_str , 0x00 , sizeof(pid_str) );
	memset( container_pid_file , 0x00 , sizeof(container_pid_file) );
	nret = ReadFileLine( pid_str , sizeof(pid_str)-1 , container_pid_file , sizeof(container_pid_file) , "%s/pid" , env->container_path_base ) ;
	INTE( "*** ERROR : ReadFileLine pid failed\n" )
	if( nret == 0 )
	{
		/* cleanup pid file */
		nret = unlink( container_pid_file ) ;
		INTE( "*** ERROR : unlink %s failed\n" , container_pid_file )
		EIDTI( "unlink %s ok\n" , container_pid_file )
	}
	
	/* read net file */
	memset( net , 0x00 , sizeof(net) );
	memset( container_net_file , 0x00 , sizeof(container_net_file) );
	nret = ReadFileLine( net , sizeof(net) , container_net_file , sizeof(container_net_file) , "%s/net" , env->container_path_base ) ;
	ILTER1( "*** ERROR : ReadFileLine net failed\n" )
	EIDTI( "read file net ok\n" )
	
	TrimEnter( net );
	
	/* destroy network */
	if( STRCMP( net , == , "BRIDGE" ) )
	{
		memset( vip , 0x00 , sizeof(vip) );
		memset( container_vip_file , 0x00 , sizeof(container_vip_file) );
		nret = ReadFileLine( vip , sizeof(vip) , container_vip_file , sizeof(container_vip_file) , "%s/vip" , env->container_path_base ) ;
		ILTER1( "*** ERROR : ReadFileLine net failed\n" )
		EIDTI( "read file %s ok\n" , container_vip_file )
		
		TrimEnter( vip );
		
		memset( port_mapping , 0x00 , sizeof(port_mapping) );
		memset( container_port_mapping_file , 0x00 , sizeof(container_port_mapping_file) );
		nret = ReadFileLine( port_mapping , sizeof(port_mapping) , container_port_mapping_file , sizeof(container_port_mapping_file) , "%s/port_mapping" , env->container_path_base ) ;
		ILTx( (memset( port_mapping,0x00,sizeof(port_mapping))) )
		EIDTI( "read file %s ok\n" , container_port_mapping_file )
		
		TrimEnter( port_mapping );
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "iptables -t nat -D POSTROUTING -o %s -j MASQUERADE" , env->host_eth_name ) ;
		INTE( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
		EIDTI( "system [%s] ok\n" , cmd )
		
		p = strtok( port_mapping , "," ) ;
		while( p )
		{
			memset( src_port_str , 0x00 , sizeof(src_port_str) );
			sscanf( p , "%[^:]:%d" , src_port_str , & (env->dst_port) );
			env->src_port = atoi(src_port_str) ;
			IxTER1( (env->src_port<=0||env->dst_port<=0) , "*** ERROR : file port_mapping value [%s] invalid\n" , p )
			
			nret = SnprintfAndSystem( cmd , sizeof(cmd) , "iptables -t nat -D PREROUTING -p tcp -m tcp --dport %d -j DNAT --to-destination %s:%d" , env->src_port , vip , env->dst_port ) ;
			INTE( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
			EIDTI( "system [%s] ok\n" , cmd )
			
			nret = SnprintfAndSystem( cmd , sizeof(cmd) , "iptables -t nat -D OUTPUT -p tcp -m tcp --dport %d -j DNAT --to-destination %s:%d" , env->src_port , vip , env->dst_port ) ;
			INTE( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
			EIDTI( "system [%s] ok\n" , cmd )
			
			p = strtok( NULL , "," ) ;
		}
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ifconfig %s down" , env->veth1_name ) ;
		INTE( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
		EIDTI( "system [%s] ok\n" , cmd )
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns exec %s ifconfig %s down" , env->netns_name , env->veth0_sname ) ;
		INTE( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
		EIDTI( "system [%s] ok\n" , cmd )
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns exec %s ifconfig lo down" , env->netns_name ) ;
		INTE( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
		EIDTI( "system [%s] ok\n" , cmd )
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "brctl delif %s %s" , env->netbr_name , env->veth1_name ) ;
		INTE( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
		EIDTI( "system [%s] ok\n" , cmd )
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip link del %s" , env->veth1_name ) ;
		INTE( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
		EIDTI( "system [%s] ok\n" , cmd )
	}
	
	/* disable system limits */
	if( env->cgroup_enable )
	{
		if( env->cmd_para.__cpus )
		{
			nret = SnprintfAndSystem( cmd , sizeof(cmd) , "rmdir %s/cpuset/cocker_%s" , CGROUP_PATH , env->cmd_para.__container ) ;
			INTE( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
			EIDTI( "system [%s] ok\n" , cmd )
		}
		
		if( env->cmd_para.__cpu_quota )
		{
			nret = SnprintfAndSystem( cmd , sizeof(cmd) , "rmdir %s/cpu,cpuacct/cocker_%s" , CGROUP_PATH , env->cmd_para.__container ) ;
			INTE( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
			EIDTI( "system [%s] ok\n" , cmd )
		}
		
		if( env->cmd_para.__mem_limit )
		{
			nret = SnprintfAndSystem( cmd , sizeof(cmd) , "rmdir %s/memory/cocker_%s" , CGROUP_PATH , env->cmd_para.__container ) ;
			INTE( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
			EIDTI( "system [%s] ok\n" , cmd )
		}
	}
	
	/* umount volumes */
	Snprintf( container_volume_file , sizeof(container_volume_file) , "%s/volume" , env->container_path_base ) ;
	container_volume_fp = fopen( container_volume_file , "r" ) ;
	if( container_volume_fp )
	{
		char		file_buffer[ 4096 ] ;
		char		*p = NULL ;
		
		while(1)
		{
			memset( file_buffer , 0x00 , sizeof(file_buffer) );
			if( fgets( file_buffer , sizeof(file_buffer) , container_volume_fp ) == NULL )
				break;
			TrimEnter( file_buffer );
			
			volume.host_path = file_buffer ;
			p = strchr( volume.host_path , ':' ) ;
			if( p )
			{
				volume.host_path_len = p - volume.host_path ;
				volume.container_path = p + 1 ;
				(*p) = '\0' ;
			}
			else
			{
				volume.host_path_len = strlen(volume.host_path) ;
				volume.container_path = volume.host_path ;
			}
			
			memset( mount_volume , 0x00 , sizeof(mount_volume) );
			len = snprintf( mount_volume , sizeof(mount_volume)-1 , "%s/merged%s" , env->container_path_base , volume.container_path ) ;
			IxTEx( SNPRINTF_OVERFLOW(len,sizeof(mount_volume)-1) , (exit(9)) , "*** ERROR : snprintf overflow\n" )
			
			nret = umount( mount_volume ) ;
			I1TE( "*** ERROR : umount[%s] failed , errno[%d]\n" , mount_volume , errno )
			EIDTI( "umount [%s] ok\n" , mount_volume )
		}
		
		fclose( container_volume_fp );
	}
	
	/* umount */
	memset( mount_target , 0x00 , sizeof(mount_target) );
	len = snprintf( mount_target , sizeof(mount_target)-1 , "%s/merged/proc" , env->container_path_base ) ;
	IxTER1( SNPRINTF_OVERFLOW(len,sizeof(mount_target)-1) , "*** ERROR : snprintf overflow\n" )
	else
	{
		while(1)
		{
			nret = umount( mount_target ) ;
			I1TE( "*** ERROR : umount %s failed , errno[%d]\n" , mount_target , errno )
			EIDTI( "umount %s ok\n" , mount_target )
			if( nret == 0 )
				break;
			if( nret == -1 && errno != EBUSY )
				break;
			sleep(1);
		}
	}
	
	memset( mount_target , 0x00 , sizeof(mount_target) );
	len = snprintf( mount_target , sizeof(mount_target)-1 , "%s/merged/dev/pts" , env->container_path_base ) ;
	IxTER1( SNPRINTF_OVERFLOW(len,sizeof(mount_target)-1) , "*** ERROR : snprintf overflow\n" )
	else
	{
		while(1)
		{
			nret = umount( mount_target ) ;
			I1TE( "*** ERROR : umount %s failed , errno[%d]\n" , mount_target , errno )
			EIDTI( "umount %s ok\n" , mount_target )
			if( nret == 0 )
				break;
			if( nret == -1 && errno != EBUSY )
				break;
			sleep(1);
		}
	}
	
	memset( mount_target , 0x00 , sizeof(mount_target) );
	len = snprintf( mount_target , sizeof(mount_target)-1 , "%s/merged" , env->container_path_base ) ;
	IxTER1( SNPRINTF_OVERFLOW(len,sizeof(mount_target)-1) , "*** ERROR : snprintf overflow\n" )
	else
	{
		while(1)
		{
			nret = umount( mount_target ) ;
			I1TE( "*** ERROR : umount %s failed , errno[%d]\n" , mount_target , errno )
			EIDTI( "umount %s ok\n" , mount_target )
			if( nret == 0 )
				break;
			if( nret == -1 && errno != EBUSY )
				break;
			sleep(1);
		}
	}
	
	return 0;
}

int DoAction_boot( struct CockerEnvironment *env )
{
	char		net[ NET_LEN_MAX + 1 ] ;
	char		vip[ IP_LEN_MAX + 1 ] ;
	char		container_pid_file[ PATH_MAX + 1 ] ;
	char		container_net_file[ PATH_MAX + 1 ] ;
	char		container_vip_file[ PATH_MAX + 1 ] ;
	char		container_port_mapping_file[ PATH_MAX + 1 ] ;
	
	char		port_mapping[ PORT_MAP_LEN_MAX ] ;
	char		*p = NULL ;
	char		src_port_str[ 20 + 1 ] ;
	
	char		cmd[ 4096 ] ;
	
	pid_t		pid ;
	int		null_fd ;
	char		pid_str[ 20 + 1 ] ;
	pid_t		container_root_pid ;
	int		i ;
	
	int		nret = 0 ;
	
	SetLogcFile( "/var/cocker/cocker.log" );
	SetLogcLevel( LOGCLEVEL_INFO );
	
	/* preprocess input parameters */
	Snprintf( env->container_path_base , sizeof(env->container_path_base)-1 , "%s/%s" , env->containers_path_base , env->cmd_para.__container );
	nret = access( env->container_path_base , F_OK ) ;
	INTER1( "*** ERROR : container '%s' not found\n" , env->cmd_para.__container )
	
	GetEthernetNames( env , env->cmd_para.__container );
	
	/* read pid file */
	memset( pid_str , 0x00 , sizeof(pid_str) );
	nret = ReadFileLine( pid_str , sizeof(pid_str)-1 , NULL , -1 , "%s/pid" , env->container_path_base ) ;
	if( nret == 0 )
	{
		TrimEnter( pid_str );
		pid = atoi(pid_str) ;
		if( pid > 0 )
		{
			nret = kill( pid , 0 ) ;
			I0TER1( "*** ERROR : container is already running\n" )
		}
	}
	
	/* read net file */
	memset( net , 0x00 , sizeof(net) );
	memset( container_net_file , 0x00 , sizeof(container_net_file) );
	nret = ReadFileLine( net , sizeof(net) , container_net_file , sizeof(container_net_file) , "%s/net" , env->container_path_base ) ;
	ILTER1( "*** ERROR : ReadFileLine net failed\n" )
	EIDTI( "read file %s ok\n" , container_net_file )
	
	TrimEnter( net );
	
	/* create network */
	if( STRCMP( net , == , "BRIDGE" ) )
	{
		memset( vip , 0x00 , sizeof(vip) );
		memset( container_vip_file , 0x00 , sizeof(container_vip_file) );
		nret = ReadFileLine( vip , sizeof(vip) , container_vip_file , sizeof(container_vip_file) , "%s/vip" , env->container_path_base ) ;
		ILTER1( "*** ERROR : ReadFileLine net failed\n" )
		EIDTI( "read file %s ok\n" , container_vip_file )
		
		TrimEnter( vip );
		
		memset( port_mapping , 0x00 , sizeof(port_mapping) );
		memset( container_port_mapping_file , 0x00 , sizeof(container_port_mapping_file) );
		nret = ReadFileLine( port_mapping , sizeof(port_mapping)-1 , container_port_mapping_file , sizeof(container_port_mapping_file) , "%s/port_mapping" , env->container_path_base ) ;
		ILTx( (memset(port_mapping,0x00,sizeof(port_mapping))) )
		EIDTI( "read file %s ok\n" , container_port_mapping_file )
		
		TrimEnter( port_mapping );
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip link add %s type veth peer name %s" , env->veth1_name , env->veth0_name ) ;
		INTER1( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
		EIDTI( "system [%s] ok\n" , cmd )
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "brctl addif %s %s" , env->netbr_name , env->veth1_name ) ;
		INTER1( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
		EIDTI( "system [%s] ok\n" , cmd )
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ifconfig %s 0.0.0.0" , env->veth1_name ) ;
		INTER1( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
		EIDTI( "system [%s] ok\n" , cmd )
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip link set %s netns %s" , env->veth0_name , env->netns_name ) ;
		INTER1( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
		EIDTI( "system [%s] ok\n" , cmd )
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns exec %s ip link set %s name %s" , env->netns_name , env->veth0_name , env->veth0_sname ) ;
		INTER1( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
		EIDTI( "system [%s] ok\n" , cmd )
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns exec %s ifconfig %s %s" , env->netns_name , env->veth0_sname , vip ) ;
		INTER1( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
		EIDTI( "system [%s] ok\n" , cmd )
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns exec %s route add default gw %s netmask 0.0.0.0 dev %s" , env->netns_name , env->netbr_ip , env->veth0_sname ) ;
		INTER1( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
		EIDTI( "system [%s] ok\n" , cmd )
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ifconfig %s up" , env->veth1_name ) ;
		INTER1( "*** ERROR : system [%s] failed [%d], errno[%d]\n" , cmd , nret , errno )
		EIDTI( "system [%s] ok\n" , cmd )
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns exec %s ifconfig %s up" , env->netns_name , env->veth0_sname ) ;
		INTER1( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
		EIDTI( "system [%s] ok\n" , cmd )
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns exec %s ifconfig lo up" , env->netns_name ) ;
		INTER1( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
		EIDTI( "system [%s] ok\n" , cmd )
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "iptables -t nat -A POSTROUTING -o %s -j MASQUERADE" , env->host_eth_name ) ;
		INTER1( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
		EIDTI( "system [%s] ok\n" , cmd )
		
		p = strtok( port_mapping , "," ) ;
		while( p )
		{
			memset( src_port_str , 0x00 , sizeof(src_port_str) );
			sscanf( p , "%[^:]:%d" , src_port_str , & (env->dst_port) );
			env->src_port = atoi(src_port_str) ;
			IxTER1( (env->src_port<=0||env->dst_port<=0) , "*** ERROR : file port_mapping value [%s] invalid\n" , p )
			
			nret = SnprintfAndSystem( cmd , sizeof(cmd) , "iptables -t nat -A PREROUTING -p tcp -m tcp --dport %d -j DNAT --to-destination %s:%d" , env->src_port , vip , env->dst_port ) ;
			INTER1( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
			EIDTI( "system [%s] ok\n" , cmd )
			
			nret = SnprintfAndSystem( cmd , sizeof(cmd) , "iptables -t nat -A OUTPUT -p tcp -m tcp --dport %d -j DNAT --to-destination %s:%d" , env->src_port , vip , env->dst_port ) ;
			INTER1( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
			EIDTI( "system [%s] ok\n" , cmd )
			
			p = strtok( NULL , "," ) ;
		}
	}
	
	/*
	if( env->cmd_para.__exec == NULL )
	{
	*/
		signal( SIGCLD , SIG_IGN );
		signal( SIGCHLD , SIG_IGN );
		signal( SIGTERM , sig_proc );
		
		container_root_pid = fork() ;
		if( container_root_pid == -1 )
		{
			E( "*** ERROR : fork failed , errno[%d]\n" , errno )
			goto _END ;
		}
		else if( container_root_pid > 0 )
		{
			if( env->cmd_para.__attach )
			{
				sleep(1);
				exit( -DoAction_attach(env) ) ;
			}
			else
			{
				printf( "OK\n" );
			}
			
			return 0;
		}
		
		signal( SIGCLD , SIG_DFL );
		signal( SIGCHLD , SIG_DFL );
		
		SetLogcFile( "/var/cocker/cocker.log" );
		SetLogcLevel( LOGCLEVEL_INFO );
		
		setsid();
		
		null_fd = open( "/dev/null" , O_RDWR ) ;
		if( null_fd == -1 )
		{
			E( "*** ERROR : open /dev/null failed , errno[%d]\n" , errno )
			goto _END ;
		}
		
		dup2( null_fd , 0 );
		dup2( null_fd , 1 );
		dup2( null_fd , 2 );
	/*
	}
	*/
	
	/* create alive pipe */
	nret = pipe( env->alive_pipe ) ;
	INTEx( goto _END , "*** ERROR : pipe failed[%d] , errno[%d]\n" , nret , errno )
	EIDTI( "create alive pipe ok\n" )
	
	/* write pid file */
	memset( pid_str , 0x00 , sizeof(pid_str) );
	snprintf( pid_str , sizeof(pid_str)-1 , "%d" , getpid() );
	nret = WriteFileLine( pid_str , container_pid_file , sizeof(container_pid_file) , "%s/pid" , env->container_path_base ) ;
	INTEx( goto _END , "*** ERROR : WriteFileLine failed[%d] , errno[%d]\n" , nret , errno )
	EIDTI( "write file %s ok\n" , container_pid_file )
	
	/* create container */
	signal( SIGCLD , SIG_DFL );
	signal( SIGCHLD , SIG_DFL );
	
	if( STRCMP( net , == , "HOST" ) )
	{
		pid = clone( CloneEntry , stack_bottom+sizeof(stack_bottom) , CLONE_NEWNS|CLONE_NEWPID|CLONE_NEWIPC|CLONE_NEWUTS , (void*)env ) ;
	}
	else
	{
		pid = clone( CloneEntry , stack_bottom+sizeof(stack_bottom) , CLONE_NEWNS|CLONE_NEWPID|CLONE_NEWIPC|CLONE_NEWUTS|CLONE_NEWNET , (void*)env ) ;
	}
	IxTEx( (pid==-1) , exit(9) , "*** ERROR : clone failed[%d] , errno[%d]\n" , pid , errno )
	EIDTI( "clone success\n" )
	
	close( env->alive_pipe[0] );
	
	/*
	if( env->cmd_para.__exec )
	{
		return 0;
	}
	*/
	
	/* wait for container end */
	while(1)
	{
		IDTI( "waitpid __WCLONE ...\n" )
		pid = waitpid( -1 , NULL , __WCLONE ) ;
		if( pid == -1 )
		{
			if( g_TERM_flag == 1 )
			{
				I( "waitpid interrupted by SIGTERM\n" )
				
				kill( container_root_pid , SIGTERM );
				for( i = 0 ; i < 5 ; i++ )
				{
					nret = kill( container_root_pid , 0 ) ;
					if( nret == -1 )
						break;
				}
				if( i >= 5 )
				{
					kill( container_root_pid , SIGKILL );
					for( i = 0 ; i < 5 ; i++ )
					{
						nret = kill( container_root_pid , 0 ) ;
						if( nret == -1 )
							break;
					}
				}
				
				break;
			}
			
			if( errno == ECHILD )
			{
				I( "waitpid interrupted by errno ECHILD\n" )
				break;
			}
			
			E( "*** ERROR : waitpid __WCLONE failed , errno[%d]\n" , errno )
			exit(1);
		}
		else if( pid > 0 )
		{
			I( "waitpid __WCLONE return[%d]\n" , pid )
			break;
		}
	}
	
_END :
	
	close( env->alive_pipe[1] );
	
	/*
	CleanContainerResource( env );
	*/
	
	return 0;
}

