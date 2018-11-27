/*
 * cocker - Container Machine Engine
 * author	: calvin
 * email	: calvinwilliams@163.com
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#include "cocker_in.h"

#define TRY_CONNECT_COUNT	5

struct termios          	g_termios_init ;
struct termios          	g_termios_raw ;

static void RestoreTerminalAttr()
{
	tcsetattr( 0 , TCSANOW , & g_termios_init );
	return; 
}

static int tcp_and_pts_bridge( int connected_sock )
{
	struct pollfd	poll_fds[ 1 ] ;
	char		buf[ 4096 ] ;
	int		len ;
	
	int		nret = 0 ;
	
	tcgetattr( STDIN_FILENO , & g_termios_init );
	atexit( & RestoreTerminalAttr );
	memcpy( & g_termios_raw , & g_termios_init , sizeof(struct termios) );
	cfmakeraw( & g_termios_raw );
	tcsetattr( STDIN_FILENO , TCSANOW , & g_termios_raw ) ;
	
	while(1)
	{
		poll_fds[0].fd = connected_sock ;
		poll_fds[0].events = POLLIN|POLLHUP ;
		poll_fds[0].revents = 0 ;
		nret = poll( poll_fds , 2 , 1000 ) ;
		if( nret == 0 )
		{
			break;
		}
		else if( nret == -1 )
		{
			E( "*** ERROR : select failed , errno[%d]\n" , errno )
			return -1;
		}
		
		if( (poll_fds[0].revents&POLLIN) || (poll_fds[0].revents&POLLHUP) )
		{
			len = read( connected_sock , buf , sizeof(buf)-1 ) ;
			if( len == 0 )
			{
				return 0;
			}
			else if( len == -1 )
			{
				E( "*** ERROR : read connected_sock failed , errno[%d]\n" , errno )
				return -1;
			}
			
			nret = writen( STDOUT_FILENO , buf , len , NULL ) ;
			if( nret == -1 )
			{
				E( "*** ERROR : writen STDOUT_FILENO failed , errno[%d]\n" , errno )
				return -1;
			}
			
			break;
		}
	}
	
	return 0;
}

int DoAction_run( struct CockerEnvironment *env )
{
	char			container_merge_path[ PATH_MAX + 1 ] ;
	
	int			connected_sock ;
	struct sockaddr_un	connected_addr ;
	int			i ;
	struct winsize		origin_winsize ;
	char			*bash_cmd = NULL ;
	
	int			nret = 0 ;
	
	/* preprocess input parameters */
	Snprintf( env->container_path_base , sizeof(env->container_path_base)-1 , "%s/%s" , env->containers_path_base , env->cmd_para.__container );
	nret = access( env->container_path_base , F_OK ) ;
	I1TER1( "*** ERROR : container '%s' not found\n" , env->cmd_para.__container )
	
	Snprintf( container_merge_path , sizeof(container_merge_path)-1 , "%s/merged" , env->container_path_base );
	nret = access( container_merge_path , F_OK ) ;
	I1TER1( "*** ERROR : merged not exist in container '%s'\n" , env->cmd_para.__container )
	
	GetEthernetNames( env , env->cmd_para.__container );
	
	/* client connect to cockerinit */
	connected_sock = socket( AF_UNIX , SOCK_STREAM , 0 ) ;
	if( connected_sock == -1 )
	{
		E( "*** ERROR : socket failed , errno[%d]\n" , errno )
		return -1;
	}
	
	for( i = 0 ; i < TRY_CONNECT_COUNT ; i++ )
	{
		memset( & connected_addr , 0x00 , sizeof(struct sockaddr_un) );
		connected_addr.sun_family = AF_UNIX ;
		snprintf( connected_addr.sun_path , sizeof(connected_addr.sun_path)-1 , "%s/dev/cocker.sock" , container_merge_path );
		nret = connect( connected_sock , (struct sockaddr *) & connected_addr , sizeof(struct sockaddr_un) ) ;
		if( nret == 0 )
			break;
		
		sleep(1);
	}
	if( i >= TRY_CONNECT_COUNT )
	{
		E( "*** ERROR : connect[%s] failed , errno[%d]\n" , connected_addr.sun_path , errno )
		return -1;
	}
	
	nret = ioctl( STDIN_FILENO , TIOCGWINSZ , & origin_winsize ) ;
	if( nret == -1 )
	{
		E( "*** ERROR : ioctl STDIN_FILENO for origin failed , errno[%d]\n" , errno )
		return -1;
	}
	
	nret = send( connected_sock , "C" , 1 , 0 ) ;
	if( nret == -1 )
	{
		E( "*** ERROR : send 'C' failed , errno[%d]\n" , errno )
		return -1;
	}
	
	nret = send( connected_sock , (void*) & origin_winsize , sizeof(struct winsize) , 0 ) ;
	if( nret == -1 )
	{
		E( "*** ERROR : send winsize failed , errno[%d]\n" , errno )
		return -1;
	}
	
	bash_cmd = env->cmd_para.__cmd ;
	nret = send( connected_sock , bash_cmd , strlen(bash_cmd) , 0 ) ;
	if( nret == -1 )
	{
		E( "*** ERROR : send cmd failed , errno[%d]\n" , errno )
		return -1;
	}
	
	tcp_and_pts_bridge( connected_sock );
	
	RestoreTerminalAttr();
	
	close( connected_sock );
	
	return 0;

}

