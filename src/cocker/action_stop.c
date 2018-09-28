#include "cocker_in.h"

static int _DoAction_kill( struct CockerEnvironment *cocker_env , int signal_no )
{
	char		container_pid_file[ PATH_MAX ] ;
	char		pid_str[ 20 + 1 ] ;
	pid_t		pid ;
	
	int		nret = 0 ;
	
	nret = SnprintfAndMakeDir( cocker_env->container_path_base , sizeof(cocker_env->container_path_base)-1 , "%s/%s" , cocker_env->containers_path_base , cocker_env->cmd_para.__container ) ;
	if( nret )
	{
		printf( "*** ERROR : SnprintfAndMakeDir[%s] failed[%d]\n" , cocker_env->container_path_base , nret );
		return -1;
	}
	else if( cocker_env->cmd_para.__debug )
	{
		printf( "mkdir %s ok\n" , cocker_env->container_path_base );
	}
	
	/* read pid file */
	nret = ReadFileLine( pid_str , sizeof(pid_str)-1 , container_pid_file , sizeof(container_pid_file) , "%s/%s/pid" , cocker_env->containers_path_base , cocker_env->cmd_para.__container ) ;
	if( nret )
	{
		printf( "*** ERROR : SnprintfAndUnlink %s failed\n" , container_pid_file );
		return 0;
	}
	
	pid = atoi(pid_str) ;
	
	/* kill clone process */
	kill( pid , signal_no );
	
	printf( "OK\n" );
	
	return 0;
}

int DoAction_stop( struct CockerEnvironment *cocker_env )
{
	return _DoAction_kill( cocker_env , SIGTERM );
}

int DoAction_kill( struct CockerEnvironment *cocker_env )
{
	return _DoAction_kill( cocker_env , SIGKILL );
}

