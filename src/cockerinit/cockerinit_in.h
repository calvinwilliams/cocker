#ifndef _H_COCKERINIT_IN_
#define _H_COCKERINIT_IN_

#include "cocker_util.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CommandParameter
{
	char			*__container_id ;
	char			*__single ;
} ;

struct CockerInitEnvironment
{
	struct CommandParameter	cmd_para ;
	
	int			alive_pipe_0 ;
	
	int			listen_sock ;
	struct sockaddr_un	listen_addr ;
	int			accepted_sock ;
	struct sockaddr_un	accepted_addr ;
	
	int			ptm_fd ;
	pid_t			bash_pid ;
} ;

int server( struct CockerInitEnvironment *env );
int create_pty( struct CockerInitEnvironment *env );
int pts_and_tcp_bridge( struct CockerInitEnvironment *env );

#ifdef __cplusplus
}
#endif

#endif

