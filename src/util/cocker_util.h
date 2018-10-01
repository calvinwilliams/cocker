#ifndef _H_VH_UTIL_IN_
#define _H_VH_UTIL_IN_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <grp.h>
#include <sys/mount.h>
#include <locale.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ifaddrs.h>
#include <sys/time.h>
#include <termios.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "openssl/md5.h"
#define __USE_GNU
#include <sched.h>

int ptmname_r(int fd, char * buf, size_t buflen);
int ptsname_r(int fd, char * buf, size_t buflen);

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MAX
#define MAX(_a_,_b_) ( (_a_)>(_b_)?(_a_):(_b_) )
#endif

#ifndef MIN
#define MIN(_a_,_b_) ( (_a_)<(_b_)?(_a_):(_b_) )
#endif

#ifndef STRCMP
#define STRCMP(_a_,_C_,_b_) ( strcmp(_a_,_b_) _C_ 0 )
#define STRNCMP(_a_,_C_,_b_,_n_) ( strncmp(_a_,_b_,_n_) _C_ 0 )
#endif

#ifndef STRICMP
#if ( defined _WIN32 )
#define STRICMP(_a_,_C_,_b_) ( stricmp(_a_,_b_) _C_ 0 )
#define STRNICMP(_a_,_C_,_b_,_n_) ( strnicmp(_a_,_b_,_n_) _C_ 0 )
#elif ( defined __unix ) || ( defined _AIX ) || ( defined __linux__ ) || ( defined __hpux )
#define STRICMP(_a_,_C_,_b_) ( strcasecmp(_a_,_b_) _C_ 0 )
#define STRNICMP(_a_,_C_,_b_,_n_) ( strncasecmp(_a_,_b_,_n_) _C_ 0 )
#endif
#endif

#ifndef SNPRINTF_OVERFLOW
#define SNPRINTF_OVERFLOW(_ret_,_sizeof_)	( (_ret_) == -1 || (_ret_) >= (_sizeof_) )
#endif

#ifndef OVERLAY_RET
#define OVERLAY_RET(_ret_,_inframe_ret_)	((_inframe_ret_)<0?-(_ret_)+(_inframe_ret_):(_ret_)+(_inframe_ret_))
#endif

#ifndef IMAGE_NAME_MAX
#define IMAGE_NAME_MAX		10
#endif

#ifndef CONTAINER_NAME_MAX
#define CONTAINER_NAME_MAX	10
#endif

#ifndef ETHERNET_NAME_MAX
#define ETHERNET_NAME_MAX	16
#endif

#ifndef NET_LEN_MAX
#define NET_LEN_MAX		10
#endif

#ifndef NETNS_NAME_MAX
#define NETNS_NAME_MAX		256
#endif

#ifndef IP_LEN_MAX
#define IP_LEN_MAX		20
#endif

#ifndef PID_LEN_MAX
#define PID_LEN_MAX		20
#endif

/*
 * statement macro
 */

#define INTx(_return_statement_,...) \
	if( nret ) \
	{ \
		_return_statement_; \
	} \

#define INTP(...) \
	if( nret ) \
	{ \
		printf( __VA_ARGS__ ); \
	} \

#define INTPR1(...) \
	if( nret ) \
	{ \
		printf( __VA_ARGS__ ); \
		return -1; \
	} \

#define INTPRX(_return_val_,...) \
	if( nret ) \
	{ \
		printf( __VA_ARGS__ ); \
		return (_return_val_); \
	} \

#define INTPx(_return_statement_,...) \
	if( nret ) \
	{ \
		printf( __VA_ARGS__ ); \
		_return_statement_; \
	} \

#define INTPFR1(...) \
	if( nret ) \
	{ \
		printf( __VA_ARGS__ ); \
		if( ! env->cmd_para.__forcely ) \
			return -1; \
	} \

#define ILTPR1(...) \
	if( nret < 0 ) \
	{ \
		printf( __VA_ARGS__ ); \
		return -1; \
	} \

#define ILTx(_return_statement_) \
	if( nret < 0 ) \
	{ \
		_return_statement_; \
	} \

#define I0TPR1(...) \
	if( nret == 0 ) \
	{ \
		printf( __VA_ARGS__ ); \
		return -1; \
	} \

#define I1TP(...) \
	if( nret == -1 ) \
	{ \
		printf( __VA_ARGS__ ); \
	} \

#define I1TPR1(...) \
	if( nret == -1 ) \
	{ \
		printf( __VA_ARGS__ ); \
		return -1; \
	} \

#define I1TPRX(_return_val_,...) \
	if( nret == -1 ) \
	{ \
		printf( __VA_ARGS__ ); \
		return (_return_val_); \
	} \

#define I1TPRx(_return_statement_,...) \
	if( nret == -1 ) \
	{ \
		printf( __VA_ARGS__ ); \
		_return_statement_; \
	} \

#define IxTPR1(_condition_exp_,...) \
	if( (_condition_exp_) ) \
	{ \
		printf( __VA_ARGS__ ); \
		return -1; \
	} \

#define IxTPFR1(_condition_exp_,...) \
	if( (_condition_exp_) ) \
	{ \
		printf( __VA_ARGS__ ); \
		if( ! env->cmd_para.__forcely ) \
			return -1; \
	} \

#define IxTPRX(_condition_exp_,_return_val_,...) \
	if( (_condition_exp_) ) \
	{ \
		printf( __VA_ARGS__ ); \
		return (_return_val_); \
	} \

#define IxTPx(_condition_exp_,_return_statement_,...) \
	if( (_condition_exp_) ) \
	{ \
		printf( __VA_ARGS__ ); \
		_return_statement_; \
	} \

#define IDTP(...) \
	if( env->cmd_para.__debug ) \
	{ \
		printf( __VA_ARGS__ ); \
	} \

#define E \
	else \

#define EIDTP(...) \
	else if( env->cmd_para.__debug ) \
	{ \
		printf( __VA_ARGS__ ); \
	} \

extern char		*_COCKER_VERSION ;

/*
 * file
 */

char *SnprintfV( char *path_buf , int path_bufsize , char *path_format , va_list valist );
char *Snprintf( char *path_buf , int path_bufsize , char *path_format , ... );

int CheckAndMakeDir( char *path );

int SnprintfAndCheckDir( char *path_buf , int path_bufsize , char *path_format , ... );
int SnprintfAndChangeDir( char *path_buf , int path_bufsize , char *path_format , ... );
int SnprintfAndMakeDir( char *path_buf , int path_bufsize , char *path_format , ... );

int SnprintfAndUnlink( char *path_buf , int path_bufsize , char *path_format , ... );

int SnprintfAndSystem( char *cmd_buf , int cmd_bufsize , char *cmd_format , ... );
int SnprintfAndPopen( char *output_buf , int output_bufsize , char *cmd_buf , int cmd_bufsize , char *cmd_format , ... );

int WriteFileLine( char *fileline , char *pathfile_buf , int pathfile_bufsize , char *pathfile_format , ... );
int ReadFileLine( char *fileline_buf , int fileline_bufsize , char *pathfile_buf , int pathfile_bufsize , char *pathfile_format , ... );

/*
 * string
 */

char *TrimEnter( char *str );

void *GenerateContainerId( char *images_id , char *container_id );

/*
 * socket
 */

int writen( int sock , char *send_buffer , int send_len , int *p_sent_len );
int readn( int sock , char *recv_buffer , int recv_len , int *p_received_len );

/*
 * pts
 */

pid_t pty_fork( struct termios *p_origin_termios , struct winsize *p_origin_winsize , int *p_ptm_fd );

#ifdef __cplusplus
}
#endif

#endif

