/*
 * cocker - Container Machine Engine
 * author	: calvin
 * email	: calvinwilliams@163.com
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#include "cockerinit_in.h"

int pts_and_tcp_bridge( struct CockerInitEnvironment *env )
{
	struct pollfd	poll_fds[ 2 ] ;
	char		buf[ 4096 ] ;
	int		len ;
	
	int		nret = 0 ;
	
	while(1)
	{
		poll_fds[0].fd = env->accepted_sock ;
		poll_fds[0].events = POLLIN|POLLHUP ;
		poll_fds[0].revents = 0 ;
		poll_fds[1].fd = env->ptm_fd ;
		poll_fds[1].events = POLLIN|POLLHUP ;
		poll_fds[1].revents = 0 ;
		nret = poll( poll_fds , 2 , -1 ) ;
		if( nret == -1 )
		{
			FATALLOGC( "poll failed , errno[%d]\n" , errno )
			return -1;
		}
		
		if( (poll_fds[0].revents&POLLIN) || (poll_fds[0].revents&POLLHUP) )
		{
			len = read( env->accepted_sock , buf , sizeof(buf)-1 ) ;
			if( len == 0 )
			{
				INFOLOGC( "read 0 from accepted sock\n" )
				return 0;
			}
			else if( len == -1 )
			{
				ERRORLOGC( "read from accepted sock failed , errno[%d]\n" , errno )
				return -11;
			}
			
			nret = writen( env->ptm_fd , buf , len , NULL ) ;
			if( nret == -1 )
			{
				ERRORLOGC( "write to ptm fd failed , errno[%d]\n" , errno )
				return -12;
			}
		}
		
		if( (poll_fds[1].revents&POLLIN) || (poll_fds[1].revents&POLLHUP) )
		{
			len = read( env->ptm_fd , buf , sizeof(buf)-1 ) ;
			if( len == 0 )
			{
				INFOLOGC( "read 0 from ptm fd\n" )
				return 0;
			}
			else if( len == -1 )
			{
				if( errno == EIO )
				{
					INFOLOGC( "read from ptm fd IOERR , errno[%d]\n" , errno )
					return 0;
				}
				else
				{
					ERRORLOGC( "read from ptm fd failed , errno[%d]\n" , errno )
					return -21;
				}
			}
			
			nret = writen( env->accepted_sock , buf , len , NULL ) ;
			if( nret == -1 )
			{
				ERRORLOGC( "write to accepted sock failed , errno[%d]\n" , errno )
				return -22;
			}
		}
	}
	
	return 0;
}

