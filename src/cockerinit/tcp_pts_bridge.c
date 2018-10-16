#include "cockerinit_in.h"

int tcp_pts_bridge( struct CockerInitEnvironment *env )
{
	fd_set		read_fds ;
	char		buf[ 4096 ] ;
	int		len ;
	
	int		nret = 0 ;
	
	while(1)
	{
		FD_ZERO( & read_fds );
		FD_SET( env->accepted_sock , & read_fds );
		FD_SET( env->ptm_fd , & read_fds );
		nret = select( MAX(env->accepted_sock,env->ptm_fd)+1 , & read_fds , NULL , NULL , NULL ) ;
		if( nret == -1 )
		{
			return -1;
		}
		
		if( FD_ISSET( env->accepted_sock , & read_fds ) )
		{
			len = read( env->accepted_sock , buf , sizeof(buf)-1 ) ;
			if( len == -1 )
			{
				return -1;
			}
			
			nret = writen( env->ptm_fd , buf , len , NULL ) ;
			if( nret == -1 )
			{
				return -1;
			}
		}
		
		if( FD_ISSET( env->ptm_fd , & read_fds ) )
		{
			len = read( env->ptm_fd , buf , sizeof(buf)-1 ) ;
			if( len == -1 )
			{
				return -1;
			}
			
			nret = writen( env->accepted_sock , buf , len , NULL ) ;
			if( nret == -1 )
			{
				return -1;
			}
		}
	}
	
	return 0;
}

