#include "cocker_in.h"

static int tcp_bridge( int connected_sock )
{
	fd_set		read_fds ;
	char		buf[ 4096 ] ;
	int		len ;
	
	int		nret = 0 ;
	
	while(1)
	{
		FD_ZERO( & read_fds );
		FD_SET( connected_sock , & read_fds );
		FD_SET( STDIN_FILENO , & read_fds );
		nret = select( connected_sock+1 , & read_fds , NULL , NULL , NULL ) ;
		if( nret == -1 )
		{
			E( "*** ERROR : select failed , errno[%d]\n" , errno )
			return -1;
		}
		
		if( FD_ISSET( 0 , & read_fds ) )
		{
			len = read( 0 , buf , sizeof(buf)-1 ) ;
			if( len == -1 )
			{
				E( "*** ERROR : read 0 failed , errno[%d]\n" , errno )
				return -1;
			}
			
			nret = writen( connected_sock , buf , len , NULL ) ;
			if( nret == -1 )
			{
				E( "*** ERROR : writen connected_sock failed , errno[%d]\n" , errno )
				return -1;
			}
		}
		
		if( FD_ISSET( connected_sock , & read_fds ) )
		{
			len = read( connected_sock , buf , sizeof(buf)-1 ) ;
			if( len == -1 )
			{
				E( "*** ERROR : read connected_sock failed , errno[%d]\n" , errno )
				return -1;
			}
			
			nret = writen( 1 , buf , len , NULL ) ;
			if( nret == -1 )
			{
				E( "*** ERROR : writen 1 failed , errno[%d]\n" , errno )
				return -1;
			}
		}
	}
	
	return 0;
}

int DoAction_attach( struct CockerEnvironment *env )
{
	char			container_merge_path[ PATH_MAX ] ;
	
	int			connected_sock ;
	struct sockaddr_un	connected_addr ;
	
	int			nret = 0 ;
	
	/* preprocess input parameters */
	Snprintf( env->container_path_base , sizeof(env->container_path_base)-1 , "%s/%s" , env->containers_path_base , env->container_id );
	nret = access( env->container_path_base , F_OK ) ;
	I1TER1( "*** ERROR : container '%s' not found\n" , env->container_id )
	
	Snprintf( container_merge_path , sizeof(container_merge_path)-1 , "%s/merged" , env->container_path_base );
	nret = access( container_merge_path , F_OK ) ;
	I1TER1( "*** ERROR : merged not exist in container '%s'\n" , env->cmd_para.__container_id )
	
	/* client connect to cockerinit */
	connected_sock = socket( AF_UNIX , SOCK_STREAM , 0 ) ;
	if( connected_sock == -1 )
	{
		E( "*** ERROR : socket failed , errno[%d]\n" , errno )
		return -1;
	}
	
	memset( & connected_addr , 0x00 , sizeof(struct sockaddr_un) );
	connected_addr.sun_family = AF_UNIX ;
	snprintf( connected_addr.sun_path , sizeof(connected_addr.sun_path)-1 , "%s/dev/cocker.sock" , container_merge_path );
	nret = connect( connected_sock , (struct sockaddr *) & connected_addr , sizeof(struct sockaddr_un) ) ;
	if( nret == -1 )
	{
		E( "*** ERROR : connect[%s] failed , errno[%d]\n" , connected_addr.sun_path , errno )
		return -1;
	}
	else
	{
		E( "connect[%s] ok\n" , connected_addr.sun_path )
	}
	
	tcp_bridge( connected_sock );
	
	close( connected_sock );
	
	return 0;

}

