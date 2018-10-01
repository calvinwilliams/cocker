#include "cockerinit_in.h"

static int streambridge( int accepted_sock , int ptm_fd , pid_t bash_pid )
{
	fd_set		read_fds ;
	struct timeval	timeout ;
	pid_t		pid ;
	int		status ;
	char		buf[ 4096 ] ;
	int		len ;
	
	int		nret = 0 ;
	
	while(1)
	{
		FD_ZERO( & read_fds );
		FD_SET( accepted_sock , & read_fds );
		FD_SET( ptm_fd , & read_fds );
		timeout.tv_sec = 1 ;
		timeout.tv_usec = 0 ;
		nret = select( MAX(accepted_sock,ptm_fd)+1 , & read_fds , NULL , NULL , & timeout ) ;
		if( nret == -1 )
		{
			printf( "select failed , errno[%d]\n" , errno );
			return -1;
		}
		else if( nret == 0 )
		{
			pid = waitpid( bash_pid , & status , WNOHANG );
			if( pid == -1 )
			{
				return -1;
			}
			else if( pid == 0 )
			{
				;
			}
			else if( pid == bash_pid )
			{
				printf( "waitpid child ok\n" );
				return 0;
			}
			else
			{
				return -2;
			}
		}
		
		if( FD_ISSET( accepted_sock , & read_fds ) )
		{
			len = read( accepted_sock , buf , sizeof(buf)-1 ) ;
			if( len == -1 )
			{
				printf( "read accepted sock failed , errno[%d]\n" , errno );
				return -1;
			}
			
			nret = writen( ptm_fd , buf , len , NULL ) ;
			if( nret == -1 )
			{
				printf( "writen ptm fd failed , errno[%d]\n" , errno );
				return -1;
			}
		}
		
		if( FD_ISSET( ptm_fd , & read_fds ) )
		{
			len = read( ptm_fd , buf , sizeof(buf)-1 ) ;
			if( len == -1 )
			{
				printf( "read ptm fd failed , errno[%d]\n" , errno );
				return -1;
			}
			
			nret = writen( accepted_sock , buf , len , NULL ) ;
			if( nret == -1 )
			{
				printf( "writen accepted sock failed , errno[%d]\n" , errno );
				return -1;
			}
		}
	}
	
	return 0;
}

static int process( int accepted_sock )
{
	struct termios	origin_termios ;
	struct winsize	origin_winsize ;
	int		ptm_fd ;
	pid_t		bash_pid ;
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
	
	/*
	signal( SIGCLD , SIG_IGN );
	signal( SIGCHLD , SIG_IGN );
	*/
	
	printf( "pty_fork ...\n" );
	bash_pid = pty_fork( & origin_termios , & origin_winsize , & ptm_fd ) ;
	if( bash_pid == -1 )
	{
		printf( "pty_fork failed , errno[%d]\n" , errno );
		return -1;
	}
	else if( bash_pid == 0 )
	{
		printf( "pty_fork child\n" );
		
		nret = tcgetattr( STDIN_FILENO , & ptm_termios ) ;
		if( nret == -1 )
		{
			printf( "tcgetattr new STDIN_FILENO setting failed , errno[%d]\n" , errno );
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
			printf( "tcsetattr new STDIN_FILENO for setting failed , errno[%d]\n" , errno );
			exit(1);
		}
		
		nret = execl( "/bin/bash" , "bash" , NULL ) ;
		if( nret == -1 )
		{
			printf( "execl failed , errno[%d]\n" , errno );
			return -1;
		}
		
		exit(1);
	}
	else
	{
		printf( "pty_fork parent\n" );
		
		streambridge( accepted_sock , ptm_fd , bash_pid );
		
		close( accepted_sock );
		close( ptm_fd );
	}
	
	return 0;
}

static int cockerinit()
{
	int			listen_sock ;
	struct sockaddr_in	listen_addr ;
	int			accepted_sock ;
	struct sockaddr_in	accepted_addr ;
	socklen_t		accepted_addr_len ;
	
	int			nret = 0 ;
	
	listen_sock = socket( AF_INET , SOCK_STREAM , IPPROTO_TCP ) ;
	if( listen_sock == -1 )
	{
		printf( "socket failed , errno[%d]\n" , errno );
		return -1;
	}
	
	memset( & listen_addr , 0x00 , sizeof(struct sockaddr_in) );
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.s_addr = INADDR_ANY ;
	listen_addr.sin_port = htons( (unsigned short)9527 );
	
	nret = bind( listen_sock , (struct sockaddr *) & listen_addr , sizeof(struct sockaddr) ) ;
	if( nret == -1 )
	{
		printf( "bind failed , errno[%d]\n" , errno );
		return -1;
	}
	
	nret = listen( listen_sock , 1024 ) ;
	if( nret == -1 )
	{
		printf( "listen failed , errno[%d]\n" , errno );
		return -1;
	}
	
	while(1)
	{
		accepted_addr_len = sizeof(struct sockaddr) ;
		accepted_sock = accept( listen_sock , (struct sockaddr *) & accepted_addr, & accepted_addr_len );
		if( accepted_sock == - 1 )
		{
			printf( "accept failed , errno[%d]\n" , errno );
			break;
		}
		else
		{
			printf( "accept ok\n" );
		}
		
		process( accepted_sock );
		
		close( accepted_sock );
	}
	
	close( listen_sock );
	
	return 0;
}

int main()
{
	return -cockerinit();
}

