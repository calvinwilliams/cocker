/*
 * cocker - Container Machine Engine
 * author	: calvin
 * email	: calvinwilliams@163.com
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#include "cockerinit_in.h"

int create_pty( struct CockerInitEnvironment *env )
{
	struct winsize	origin_winsize ;
	char		cmd[ 4096 ] ;
	struct termios	ptm_termios ;
#if 0
	int		argc ;
	char		*argv[64] = { NULL } ;
	char		*p = NULL ;
	char		*p2 = NULL ;
	int		i ;
#endif
	
	int		nret = 0 ;
	
	SetLogcFile( "/cockerinit.log" );
	SetLogcLevel( LOGCLEVEL_INFO );

	memset( & origin_winsize , 0x00 , sizeof(struct winsize) );
	nret = recv( env->accepted_sock , & origin_winsize , sizeof(struct winsize) , 0 ) ;
	if( nret <= 0 )
	{
		FATALLOGC( "*** ERROR : recv winsize failed[%d] , errno[%d]\n" , nret , errno )
		exit(9);
	}
	else
	{
		INFOLOGC( "recv winsize ok , [%d]bytes\n" , nret )
	}
	
	memset( cmd , 0x00 , sizeof(cmd) );
	nret = recv( env->accepted_sock , cmd , sizeof(cmd) , 0 ) ;
	if( nret <= 0 )
	{
		FATALLOGC( "*** ERROR : recv cmd failed[%d] , errno[%d]\n" , nret , errno )
		exit(9);
	}
	else
	{
		INFOLOGC( "recv cmd ok  , [%d]bytes , cmd[%s]\n" , nret , cmd );
	}
	
	/*
	signal( SIGCLD , SIG_IGN );
	signal( SIGCHLD , SIG_IGN );
	*/
	
	/* printf( "pty_fork ...\n" ); */
	env->bash_pid = pty_fork( NULL , & origin_winsize , & (env->ptm_fd) ) ;
	if( env->bash_pid < 0 )
	{
		FATALLOGC( "*** ERROR : pty_fork failed[%d] , errno[%d]\n" , env->bash_pid , errno )
		exit(9);
	}
	else if( env->bash_pid == 0 )
	{
		nret = tcgetattr( STDIN_FILENO , & ptm_termios ) ;
		if( nret == -1 )
		{
			FATALLOGC( "*** ERROR : tcgetattr failed[%d] , errno[%d]\n" , nret , errno )
			exit(1);
		}
		
		ptm_termios.c_lflag |= ECHO ;
		ptm_termios.c_oflag |= ONLCR | XTABS ;
		ptm_termios.c_iflag |= ICRNL ;
		ptm_termios.c_iflag &= ~IXOFF ;
		
		nret = tcsetattr( STDIN_FILENO , TCSANOW , & ptm_termios ) ;
		if( nret == -1 )
		{
			FATALLOGC( "*** ERROR : tcsetattr failed[%d] , errno[%d]\n" , nret , errno )
			exit(1);
		}
		
#if 0
		argc = 0 ;
		p = strtok( cmd , " \t" ) ;
		while( p )
		{
			if( argc >= sizeof(argv)/sizeof(argv[0])-1 )
				break;
			
			if( argc == 0 )
			{
				argv[argc++] = p ;
				argv[argc++] = p ;
				p2 = strrchr( argv[1] , '/' ) ;
				if( p2 )
				{
					argv[1] = p2+1 ;
				}
			}
			else
			{
				argv[argc++] = p ;
			}
			
			p = strtok( NULL , " \t" ) ;
		}
		argv[argc++] = NULL ;
		
		for( i = 0 ; i < argc ; i++ )
			INFOLOGC( "argv[%d]=[%s]\n" , i , argv[i] )
		
		if( strchr( argv[0] , '/' ) )
			nret = execv( argv[0] , argv+1 ) ;
		else
			nret = execvp( argv[0] , argv+1 ) ;
#endif
		if( STRCMP( cmd , == , "/bin/bash -l" ) )
			nret = execl( "/bin/bash" , "bash" , "-l" , NULL ) ;
		else
			nret = execl( "/bin/bash" , "bash" , "-c" , cmd , NULL ) ;
		if( nret == -1 )
		{
			FATALLOGC( "*** ERROR : execl failed[%d] , errno[%d]\n" , nret , errno )
			exit(1);
		}
		
		exit(1);
	}
	else
	{
		INFOLOGC( "pty_fork parent\n" )
		
		nret = pts_and_tcp_bridge( env );
		INFOLOGC( "pts_and_tcp_bridge return[%d]\n" , nret )
		
		INFOLOGC( "close accepted sock and ptm_fd\n" )
		close( env->accepted_sock );
		close( env->ptm_fd );
	}
	
	return 0;
}

