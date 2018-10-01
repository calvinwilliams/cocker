#include "cockerinit_in.h"

int server( struct CockerInitEnvironment *env )
{
	socklen_t		accepted_addr_len ;
	pid_t			pid ;
	
	int			nret = 0 ;
	
	env->listen_sock = socket( AF_UNIX , SOCK_STREAM , 0 ) ;
	if( env->listen_sock == -1 )
	{
		printf( "*** ERROR : socket failed , errno[%d]\n" , errno );
		return -1;
	}
	
	memset( & (env->listen_addr) , 0x00 , sizeof(struct sockaddr_un) );
	env->listen_addr.sun_family = AF_UNIX;
	snprintf( env->listen_addr.sun_path , sizeof(env->listen_addr.sun_path)-1 , "/dev/cocker.sock" );
	
	nret = bind( env->listen_sock , (struct sockaddr *) & (env->listen_addr) , sizeof(struct sockaddr_un) ) ;
	if( nret == -1 )
	{
		printf( "*** ERROR : bind failed , errno[%d]\n" , errno );
		return -1;
	}
	
	nret = listen( env->listen_sock , 1024 ) ;
	if( nret == -1 )
	{
		printf( "*** ERROR : listen failed , errno[%d]\n" , errno );
		return -1;
	}
	
	while(1)
	{
		accepted_addr_len = sizeof(struct sockaddr_un) ;
		env->accepted_sock = accept( env->listen_sock , (struct sockaddr *) & (env->accepted_addr) , & accepted_addr_len );
		if( env->accepted_sock == - 1 )
		{
			printf( "*** ERROR : accept failed , errno[%d]\n" , errno );
			break;
		}
		else
		{
			printf( "accept ok\n" );
		}
		
		signal( SIGCLD , SIG_IGN );
		signal( SIGCHLD , SIG_IGN );
		
		pid = fork() ;
		if( pid == -1 )
		{
			printf( "*** ERROR : fork failed , errno[%d]\n" , errno );
			exit(9);
		}
		else if( pid == 0 )
		{
			exit(-process( env ));
		}
		
		close( env->accepted_sock );
	}
	
	close( env->listen_sock );
	
	return 0;
}

