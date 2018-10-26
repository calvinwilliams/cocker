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
	/*
	struct termios	origin_termios ;
	struct winsize	origin_winsize ;
	*/
	struct termios	ptm_termios ;
	
	int		nret = 0 ;
	
	/*
	nret = tcgetattr( STDIN_FILENO , & origin_termios ) ;
	if( nret == -1 )
	{
		ERRORLOGC( "*** ERROR : tcgetattr STDIN_FILENO for origin failed , errno[%d]\n" , errno )
		exit(1);
	}
	
	nret = ioctl( STDIN_FILENO , TIOCGWINSZ , & origin_winsize ) ;
	if( nret == -1 )
	{
		ERRORLOGC( "*** ERROR : ioctl STDIN_FILENO for origin failed , errno[%d]\n" , errno )
		exit(1);
	}
	*/
	
	signal( SIGCLD , SIG_IGN );
	signal( SIGCHLD , SIG_IGN );
	
	printf( "pty_fork ...\n" );
	// env->bash_pid = pty_fork( & origin_termios , & origin_winsize , & (env->ptm_fd) ) ;
	env->bash_pid = pty_fork( NULL , NULL , & (env->ptm_fd) ) ;
	if( env->bash_pid < 0 )
	{
		FATALLOGC( "*** ERROR : pty_fork failed[%d] , errno[%d]\n" , env->bash_pid , errno )
		exit(9);
	}
	else if( env->bash_pid == 0 )
	{
		INFOLOGC( "pty_fork child\n" )
		
		nret = tcgetattr( STDIN_FILENO , & ptm_termios ) ;
		if( nret == -1 )
		{
			FATALLOGC( "*** ERROR : tcgetattr failed[%d] , errno[%d]\n" , nret , errno )
			exit(1);
		}
		
		/*
		ptm_termios.c_lflag &= ~(ECHO|ECHOE|ECHOK|ECHONL) ;
		ptm_termios.c_oflag &= ~(ONLCR) ;
		*/
		
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
		
		nret = execl( "/bin/bash" , "bash" , "-l" , NULL ) ;
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
		
		pts_and_tcp_bridge( env );
		
		close( env->accepted_sock );
		close( env->ptm_fd );
	}
	
	return 0;
}

