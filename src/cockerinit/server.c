/*
 * cocker - Container Machine Engine
 * author	: calvin
 * email	: calvinwilliams@163.com
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#include "cockerinit_in.h"

int server( struct CockerInitEnvironment *env )
{
	struct pollfd		poll_fds[ 2 ] ;
	socklen_t		accepted_addr_len ;
	pid_t			pid ;
	int			status ;
	
	int			nret = 0 ;
	
	signal( SIGPIPE , SIG_IGN );
	
	env->listen_sock = socket( AF_UNIX , SOCK_STREAM , 0 ) ;
	if( env->listen_sock == -1 )
	{
		E( "*** ERROR : socket failed , errno[%d]\n" , errno )
		return -1;
	}
	
	memset( & (env->listen_addr) , 0x00 , sizeof(struct sockaddr_un) );
	env->listen_addr.sun_family = AF_UNIX ;
	snprintf( env->listen_addr.sun_path , sizeof(env->listen_addr.sun_path)-1 , "/dev/cocker.sock" );
	if( access( env->listen_addr.sun_path , F_OK ) == 0 )
	{
		unlink( env->listen_addr.sun_path );
	}
	nret = bind( env->listen_sock , (struct sockaddr *) & (env->listen_addr) , sizeof(struct sockaddr_un) ) ;
	if( nret == -1 )
	{
		E( "*** ERROR : bind failed , errno[%d]\n" , errno )
		return -1;
	}
	
	nret = listen( env->listen_sock , 1024 ) ;
	if( nret == -1 )
	{
		E( "*** ERROR : listen failed , errno[%d]\n" , errno )
		return -1;
	}
	
	while(1)
	{
_POLL :
		poll_fds[0].fd = env->alive_pipe_0 ;
		poll_fds[0].events = POLLIN|POLLHUP ;
		poll_fds[0].revents = 0 ;
		poll_fds[1].fd = env->listen_sock ;
		poll_fds[1].events = POLLIN|POLLHUP ;
		poll_fds[1].revents = 0 ;
		nret = poll( poll_fds , 2 , 1000 ) ;
		if( nret == -1 )
		{
			return -1;
		}
		else if( nret == 0 )
		{
_WAITPID :
			pid = waitpid( -1 , & status , WNOHANG );
			if( pid == -1 )
			{
				if( errno == ECHILD )
				{
					INFOLOGC( "waitpid ECHILD\n" )
				}
				else
				{
					FATALLOGC( "*** ERROR : waitpid failed , errno[%d]\n" , errno )
					return -1;
				}
			}
			else if( pid > 0 )
			{
				INFOLOGC( "waitpid[%d] ok\n" , pid )
				goto _WAITPID;
			}
			
			goto _POLL;
		}
		
		if( (poll_fds[0].revents&POLLIN) || (poll_fds[0].revents&POLLHUP) )
		{
			INFOLOGC( "alive pipe broken\n" )
			return 0;
		}
		
		if( (poll_fds[1].revents&POLLIN) || (poll_fds[1].revents&POLLHUP) )
		{
			char		ch ;
			
			accepted_addr_len = sizeof(struct sockaddr_un) ;
			env->accepted_sock = accept( env->listen_sock , (struct sockaddr *) & (env->accepted_addr) , & accepted_addr_len );
			if( env->accepted_sock == - 1 )
			{
				ERRORLOGC( "*** ERROR : accept failed , errno[%d]\n" , errno )
				return -1;
			}
			
			INFOLOGC( "new session accepted" )
			
			/*
			signal( SIGCLD , SIG_IGN );
			signal( SIGCHLD , SIG_IGN );
			*/
			
			nret = recv( env->accepted_sock , & ch , 1 , 0 ) ;
			if( nret <= 0 )
			{
				ERRORLOGC( "*** ERROR : recv 'C' failed , errno[%d]\n" , errno )
				return -1;
			}
			else
			{
				INFOLOGC( "recv ok , [%c]\n" , ch )
			}
			
			if( ch == 'C' )
			{
				pid = fork() ;
				if( pid == -1 )
				{
					ERRORLOGC( "*** ERROR : fork failed , errno[%d]\n" , errno )
					return -1;
				}
				else if( pid == 0 )
				{
					close( env->listen_sock );
					
					exit(-create_pty( env ));
				}
			}
			
			close( env->accepted_sock );
		}
	}
	
	close( env->listen_sock );
	
	return 0;
}

