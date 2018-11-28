/*
 * cocker - Container Machine Engine
 * author	: calvin
 * email	: calvinwilliams@163.com
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#include "cocker_in.h"

int _DoAction_kill( struct CockerEnvironment *env , int signal_no )
{
	char		container_pid_file[ PATH_MAX + 1 ] ;
	char		pid_str[ PID_LEN_MAX + 1 ] ;
	pid_t		pid ;
	
	int		nret = 0 ;
	
	/* preprocess input parameters */
	nret = SnprintfAndMakeDir( env->container_path_base , sizeof(env->container_path_base)-1 , "%s/%s" , env->containers_path_base , env->cmd_para.__container ) ;
	INTER1( "*** ERROR : SnprintfAndMakeDir[%s] failed[%d]\n" , env->container_path_base , nret )
	
	GetEthernetNames( env , env->cmd_para.__container );
	
	/* read pid file */
	memset( pid_str , 0x00 , sizeof(pid_str) );
	memset( container_pid_file , 0x00 , sizeof(container_pid_file) );
	nret = ReadFileLine( pid_str , sizeof(pid_str)-1 , container_pid_file , sizeof(container_pid_file) , "%s/pid" , env->container_path_base ) ;
	INTER1( "*** ERROR : SnprintfAndUnlink %s failed\n" , container_pid_file )
	
	TrimEnter( pid_str );
	
	/* stop container */
	pid = atoi(pid_str) ;
	if( pid > 0 )
	{
		nret = kill( pid , 0 ) ;
		if( nret == 0 )
		{
			/* kill clone create_pty */
			kill( pid , signal_no );
			
			while(1)
			{
				nret = kill( pid , 0 ) ;
				if( nret == -1 )
				{
					/* destroy container */
					CleanContainerResource( env );
					break;
				}
				
				sleep(1);
			}
		}
		else
		{
			printf( "*** ERROR : container[%s] expection\n" , pid_str );
			
			if( env->cmd_para.__forcely )
			{
				/* destroy container */
				CleanContainerResource( env );
			}
		}
	}
	else
	{
		printf( "*** ERROR : container is not running\n" );
	}
	
	return 0;
}

int DoAction_shutdown( struct CockerEnvironment *env )
{
	int		nret = 0 ;
	
	nret = _DoAction_kill( env , SIGTERM ) ;
	if( nret == 0 )
	I( "OK\n" )
	
	return nret;
}

int DoAction_kill( struct CockerEnvironment *env )
{
	int		nret = 0 ;
	
	nret = _DoAction_kill( env , SIGKILL ) ;
	if( nret == 0 )
	I( "OK\n" )
	
	return nret;
}

