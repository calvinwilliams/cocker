#include "vhm_in.h"

/* for test
$ vhm -a start --vhost test
*/

/* for debug
$ mount -t overlay overlay -o lowerdir=/var/openvh/vtemplate/test/rlayer,upperdir=/var/openvh/vhost/test/rwlayer,workdir=/var/openvh/vhost/test/workdir /var/openvh/vhost/test/merged
$ chroot /var/openvh/vhost/test/merged
$ mount -t proc proc /proc

$ umount /proc
$ umount /var/openvh/vhost/test/merged
*/

static unsigned char	stack_bottom[ 1024 * 1024 ] = "" ;

static int VHostEntry( void *p )
{
	struct VhmEnvironment	*vhm_env = (struct VhmEnvironment *)p ;
	char			root_path[ PATH_MAX ] ;
	int			len ;
	
	int			nret = 0 ;
	
	setuid(0);
	setgid(0);
	setgroups(0,NULL);
	
	sethostname( vhm_env->cmd_para.__host_name , strlen(vhm_env->cmd_para.__host_name) );
	
	unshare( CLONE_NEWUSER );
	
	memset( root_path , 0x00 , sizeof(root_path) );
	len = snprintf( root_path , sizeof(root_path)-1 , "%s/%s/merged" , vhm_env->vhosts_path_base , vhm_env->cmd_para.__vhost ) ;
	if( SNPRINTF_OVERFLOW(len,sizeof(root_path)-1) )
	{
		printf( "*** ERROR : snprintf failed\n" );
		return -1;
	}
	chroot( root_path );
	chdir( "/" );
	
	mount( "proc" , "/proc" , "proc" , 0 , "" );
	
	nret = execl( "/bin/bash" , "bash" , "--login" , NULL ) ;
	if( nret == -1 )
	{
		printf( "*** ERROR : execl failed , errno[%d]\n" , errno );
		return -1;
	}
	
	fflush( stdout );
	
	return 0;
}

int VhmAction_start( struct VhmEnvironment *vhm_env )
{
	char		vtemplate[ PATH_MAX ] ;
	char		cmd[ 4096 ] ;
	int		len ;
	
	int		nret = 0 ;
	
	/* mount filesystem */
	nret = ReadFileLine( vtemplate , sizeof(vtemplate)-1 , "%s/%s/vtemplates" , vhm_env->vhosts_path_base , vhm_env->cmd_para.__vhost ) ;
	if( nret )
	{
		printf( "*** ERROR : ReadFileLine vtemplates in vhost '%s' failed\n" , vhm_env->cmd_para.__vhost );
		return -1;
	}
	
	memset( cmd , 0x00 , sizeof(cmd) );
	if( vtemplate[0] == '\0' )
		len = snprintf( cmd , sizeof(cmd)-1 , "mount -t overlay overlay -o upperdir=%s/%s/rwlayer,workdir=%s/%s/workdir %s/%s/merged" , vhm_env->vhosts_path_base , vhm_env->cmd_para.__vhost , vhm_env->vhosts_path_base , vhm_env->cmd_para.__vhost , vhm_env->vhosts_path_base , vhm_env->cmd_para.__vhost ) ;
	else
		len = snprintf( cmd , sizeof(cmd)-1 , "mount -t overlay overlay -o lowerdir=%s/%s/rlayer,upperdir=%s/%s/rwlayer,workdir=%s/%s/workdir %s/%s/merged" , vhm_env->vtemplates_path_base , vtemplate , vhm_env->vhosts_path_base , vhm_env->cmd_para.__vhost , vhm_env->vhosts_path_base , vhm_env->cmd_para.__vhost , vhm_env->vhosts_path_base , vhm_env->cmd_para.__vhost ) ;
printf( "DEBUG - cmd[%s]\n" , cmd );
	if( SNPRINTF_OVERFLOW(len,sizeof(cmd)-1) )
	{
		printf( "*** ERROR : snprintf failed\n" );
		return -1;
	}
	
	/* create vhost */
	nret = clone( VHostEntry , stack_bottom+sizeof(stack_bottom) , CLONE_NEWNS|CLONE_NEWPID|CLONE_NEWIPC|CLONE_NEWUTS|CLONE_NEWNET , (void*)vhm_env ) ;
	if( nret == -1 )
	{
		printf( "*** ERROR : clone failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	
	while(1)
	{
		nret = wait( NULL ) ;
		if( nret == -1 )
			break;
	}
	
	return 0;
}

