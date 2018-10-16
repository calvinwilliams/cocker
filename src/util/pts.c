#include "cocker_util.h"

pid_t pty_fork( struct termios *p_origin_termios , struct winsize *p_origin_winsize , int *p_ptm_fd )
{
	int		ptm_fd ;
	int		pts_fd ;
	char		pts_pathfilename[ PATH_MAX + 1 ] ;
	int		lock ;
	
	pid_t		pid ;
	
	int		nret = 0 ;
	
	ptm_fd = open( "/dev/pts/ptmx" , O_RDWR ) ;
	if( ptm_fd == -1 )
		return -1;
	
	nret = ptsname_r( ptm_fd , pts_pathfilename , sizeof(pts_pathfilename) ) ;
	if( nret == -1 )
	{
		close( ptm_fd );
		return -2;
	}
	
	nret = chmod( pts_pathfilename , S_IRUSR|S_IWUSR|S_IWGRP ) ;
	if( nret == -1 )
	{
		close( ptm_fd );
		return -3;
	}
	
	lock = 0 ;
	nret = ioctl( ptm_fd , TIOCSPTLCK , & lock ) ;
	if( nret == -1 )
	{
		close( ptm_fd );
		return -4;
	}
	
	pid = fork() ;
	if( pid == -1 )
	{
		return -11;
	}
	else if( pid == 0 )
	{
		close( ptm_fd );
		
		nret = setsid() ;
		if( nret == -1 )
			return -21;
		
		pts_fd = open( pts_pathfilename , O_RDWR ) ;
		if( pts_fd == -1 )
			return -22;
		
#if defined(TIOCSCTTY)
		nret = ioctl( pts_fd , TIOCSCTTY , NULL ) ;
		if( nret == -1 )
			return -23;
#endif
		
		if( p_origin_termios )
		{
			nret = tcsetattr( pts_fd , TCSANOW , p_origin_termios ) ;
			if( nret == -1 )
				return -24;
		}
		
		if( p_origin_winsize )
		{
			nret = ioctl( pts_fd , TIOCSWINSZ , p_origin_winsize ) ;
			if( nret == -1 )
				return -25;
		}
		
		nret = dup2( pts_fd , STDIN_FILENO ) ;
		if( nret == -1 )
			return -26;
		
		nret = dup2( pts_fd , STDOUT_FILENO ) ;
		if( nret == -1 )
			return -27;
		
		nret = dup2( pts_fd , STDERR_FILENO ) ;
		if( nret == -1 )
			return -28;
		
		if( pts_fd != STDIN_FILENO && pts_fd != STDOUT_FILENO && pts_fd != STDERR_FILENO )
			close( pts_fd );
		
		return 0;
	}
	else
	{
		if( p_ptm_fd )
			(*p_ptm_fd) = ptm_fd ;
		
		return pid;
	}
}

