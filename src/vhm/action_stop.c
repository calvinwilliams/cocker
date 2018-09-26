#include "vhm_in.h"

int VhmAction_stop( struct VhmEnvironment *vhm_env )
{
	char		mount_target[ PATH_MAX ] ;
	int		len ;
	
	char		pid_str[ 20 + 1 ] ;
	pid_t		pid ;
	
	int		nret = 0 ;
	
	/* umount */
	memset( mount_target , 0x00 , sizeof(mount_target) );
	len = snprintf( mount_target , sizeof(mount_target)-1 , "%s/%s/merged/proc" , vhm_env->vhosts_path_base , vhm_env->cmd_para.__vhost ) ;
	if( SNPRINTF_OVERFLOW(len,sizeof(mount_target)-1) )
	{
		printf( "*** ERROR : snprintf failed\n" );
		return -1;
	}
	nret = umount( mount_target ) ;
	if( nret == -1 )
	{
		printf( "*** ERROR : umount proc failed\n" );
		return -1;
	}
	
	memset( mount_target , 0x00 , sizeof(mount_target) );
	len = snprintf( mount_target , sizeof(mount_target)-1 , "%s/%s/merged" , vhm_env->vhosts_path_base , vhm_env->cmd_para.__vhost ) ;
	if( SNPRINTF_OVERFLOW(len,sizeof(mount_target)-1) )
	{
		printf( "*** ERROR : snprintf failed\n" );
		return -1;
	}
	nret = umount( mount_target ) ;
	if( nret == -1 )
	{
		printf( "*** ERROR : umount merged failed\n" );
		return -1;
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

