#include "cockerinit_in.h"

int server( struct CockerInitEnvironment *env )
{
	fd_set			read_fds ;
	struct timeval		timeout ;
	socklen_t		accepted_addr_len ;
	pid_t			pid ;
	int			status ;
	
	int			nret = 0 ;
	
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
		FD_ZERO( & read_fds );
		FD_SET( env->alive_pipe_0 , & read_fds );
		FD_SET( env->listen_sock , & read_fds );
		timeout.tv_sec = 1 ;
		timeout.tv_usec = 0 ;
		nret = select( MAX(env->alive_pipe_0,env->listen_sock)+1 , & read_fds , NULL , NULL , & timeout ) ;
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
					INFOLOGC( "waitpid ...\n" )
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
		}
		
		if( FD_ISSET( env->alive_pipe_0 , & read_fds ) )
		{
			INFOLOGC( "alive pipe broken\n" )
			return 0;
		}
		
		if( FD_ISSET( env->listen_sock , & read_fds ) )
		{
			accepted_addr_len = sizeof(struct sockaddr_un) ;
			env->accepted_sock = accept( env->listen_sock , (struct sockaddr *) & (env->accepted_addr) , & accepted_addr_len );
			if( env->accepted_sock == - 1 )
			{
				ERRORLOGC( "*** ERROR : accept failed , errno[%d]\n" , errno )
				return -1;
			}
			
			INFOLOGC( "new session accepted" )
			
			signal( SIGCLD , SIG_IGN );
			signal( SIGCHLD , SIG_IGN );
			
			pid = fork() ;
			if( pid == -1 )
			{
				ERRORLOGC( "*** ERROR : fork failed , errno[%d]\n" , errno )
				return -1;
			}
			else if( pid == 0 )
			{
				exit(-process( env ));
			}
			
			close( env->accepted_sock );
		}
	}
	
	close( env->listen_sock );
	
	return 0;
}

