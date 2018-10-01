#include "cockerinit_in.h"

int process( struct CockerInitEnvironment *env )
{
	struct termios	origin_termios ;
	struct winsize	origin_winsize ;
	struct termios	ptm_termios ;
	
	int		nret = 0 ;
	
	nret = tcgetattr( STDIN_FILENO , & origin_termios ) ;
	if( nret == -1 )
	{
		printf( "tcgetattr STDIN_FILENO for origin failed , errno[%d]\n" , errno );
		exit(1);
	}
	
	nret = ioctl( STDIN_FILENO , TIOCGWINSZ , & origin_winsize ) ;
	if( nret == -1 )
	{
		printf( "ioctl STDIN_FILENO for origin failed , errno[%d]\n" , errno );
		exit(1);
	}
	
	signal( SIGCLD , SIG_IGN );
	signal( SIGCHLD , SIG_IGN );
	
	printf( "pty_fork ...\n" );
	env->bash_pid = pty_fork( & origin_termios , & origin_winsize , & (env->ptm_fd) ) ;
	if( env->bash_pid == -1 )
	{
		printf( "pty_fork failed , errno[%d]\n" , errno );
		exit(9);
	}
	else if( env->bash_pid == 0 )
	{
		printf( "pty_fork child\n" );
		
		nret = tcgetattr( STDIN_FILENO , & ptm_termios ) ;
		if( nret == -1 )
		{
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
			exit(1);
		}
		
		nret = execl( "/bin/bash" , "bash" , NULL ) ;
		if( nret == -1 )
		{
			exit(1);
		}
		
		exit(1);
	}
	else
	{
		printf( "pty_fork parent\n" );
		
		pts_bridge( env );
		
		close( env->accepted_sock );
		close( env->ptm_fd );
	}
	
	return 0;
}

