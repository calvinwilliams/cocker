#ifndef _H_COCKERINIT_IN_
#define _H_COCKERINIT_IN_

#include "cocker_util.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CockerInitEnvironment
{
	char			container_id[ CONTAINER_ID_LEN_MAX + 1 ] ;
	
	int			alive_pipe_0 ;
	
	int			listen_sock ;
	struct sockaddr_un	listen_addr ;
	int			accepted_sock ;
	struct sockaddr_un	accepted_addr ;
	
	int			ptm_fd ;
	pid_t			bash_pid ;
} ;

int server( struct CockerInitEnvironment *env );
int process( struct CockerInitEnvironment *env );
int tcp_pts_bridge( struct CockerInitEnvironment *env );

#ifdef __cplusplus
}
#endif

#endif

