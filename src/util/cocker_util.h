/*
 * cocker - Container Machine Engine
 * author	: calvin
 * email	: calvinwilliams@163.com
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

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
#include <poll.h>
#include "openssl/md5.h"
#define __USE_GNU
#include <sched.h>

#include "list.h"

#include "LOGC.h"
#include "filerpl.h"

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

#ifndef IMAGES_ID_LEN_MAX
#define IMAGES_ID_LEN_MAX	1024
#endif

#ifndef CREATE_DATATIME_LEN_MAX
#define CREATE_DATATIME_LEN_MAX	20
#endif

#ifndef VERSION_LEN_MAX
#define VERSION_LEN_MAX		16
#endif

#ifndef TAG_LEN_MAX
#define TAG_LEN_MAX		64
#endif

#ifndef CONTAINER_ID_LEN_MAX
#define CONTAINER_ID_LEN_MAX	64
#endif

#ifndef ETHERNET_NAME_LEN_MAX
#define ETHERNET_NAME_LEN_MAX	13
#endif

#ifndef NET_LEN_MAX
#define NET_LEN_MAX		10
#endif

#ifndef NETNS_NAME_LEN_MAX
#define NETNS_NAME_LEN_MAX	256
#endif

#ifndef IP_LEN_MAX
#define IP_LEN_MAX		20
#endif

#ifndef PORT_MAP_LEN_MAX
#define PORT_MAP_LEN_MAX	64
#endif

#ifndef PID_LEN_MAX
#define PID_LEN_MAX		20
#endif

#ifndef CGROUP_PATH
#define CGROUP_PATH		"/sys/fs/cgroup"
#endif

#ifndef COCKERIMAGE_FILE_EXTNAME
#define COCKERIMAGE_FILE_EXTNAME	"cockerimage"
#endif

#ifndef SREPO_LEN_MAX
#define SREPO_LEN_MAX		1024
#endif

/*
 * expression macro
 */

#define IS_NULL_OR_EMPTY(_p_)	( (_p_) == NULL || (_p_)[0] == '\0' )

/*
 * statement macro
 */

#define I( ... ) \
	{ \
		printf( __VA_ARGS__ ); fflush(stdout); \
		INFOLOGC( __VA_ARGS__ ) \
	} \

#define E(...) \
	{ \
		printf( __VA_ARGS__ ); fflush(stdout); \
		ERRORLOGC( __VA_ARGS__ ) \
	} \

#define ER1(...) \
	{ \
		printf( __VA_ARGS__ ); fflush(stdout); \
		ERRORLOGC( __VA_ARGS__ ) \
		return -1; \
	} \

#define INTx(_return_statement_,...) \
	if( nret ) \
	{ \
		_return_statement_; \
	} \

#define INTE(...) \
	if( nret ) \
	{ \
		printf( __VA_ARGS__ ); fflush(stdout); \
		ERRORLOGC( __VA_ARGS__ ) \
	} \

#define INTER1(...) \
	if( nret ) \
	{ \
		printf( __VA_ARGS__ ); fflush(stdout); \
		ERRORLOGC( __VA_ARGS__ ) \
		return -1; \
	} \

#define INTERX(_return_val_,...) \
	if( nret ) \
	{ \
		printf( __VA_ARGS__ ); fflush(stdout); \
		ERRORLOGC( __VA_ARGS__ ) \
		return (_return_val_); \
	} \

#define INTEx(_return_statement_,...) \
	if( nret ) \
	{ \
		printf( __VA_ARGS__ ); fflush(stdout); \
		ERRORLOGC( __VA_ARGS__ ) \
		_return_statement_; \
	} \

#define INTEFR1(...) \
	if( nret ) \
	{ \
		printf( __VA_ARGS__ ); fflush(stdout); \
		ERRORLOGC( __VA_ARGS__ ) \
		if( ! env->cmd_para.__forcely ) \
			return -1; \
	} \

#define ILTER1(...) \
	if( nret < 0 ) \
	{ \
		printf( __VA_ARGS__ ); fflush(stdout); \
		ERRORLOGC( __VA_ARGS__ ) \
		return -1; \
	} \

#define ILTx(_return_statement_) \
	if( nret < 0 ) \
	{ \
		_return_statement_; \
	} \

#define I0TI(...) \
	if( nret == 0 ) \
	{ \
		printf( __VA_ARGS__ ); fflush(stdout); \
		INFOLOGC( __VA_ARGS__ ) \
	} \

#define I0TER1(...) \
	if( nret == 0 ) \
	{ \
		printf( __VA_ARGS__ ); fflush(stdout); \
		ERRORLOGC( __VA_ARGS__ ) \
		return -1; \
	} \

#define I1TE(...) \
	if( nret == -1 ) \
	{ \
		printf( __VA_ARGS__ ); fflush(stdout); \
		ERRORLOGC( __VA_ARGS__ ) \
	} \

#define I1TER1(...) \
	if( nret == -1 ) \
	{ \
		printf( __VA_ARGS__ ); fflush(stdout); \
		ERRORLOGC( __VA_ARGS__ ) \
		return -1; \
	} \

#define I1TERX(_return_val_,...) \
	if( nret == -1 ) \
	{ \
		printf( __VA_ARGS__ ); fflush(stdout); \
		ERRORLOGC( __VA_ARGS__ ) \
		return (_return_val_); \
	} \

#define I1TERx(_return_statement_,...) \
	if( nret == -1 ) \
	{ \
		printf( __VA_ARGS__ ); fflush(stdout); \
		ERRORLOGC( __VA_ARGS__ ) \
		_return_statement_; \
	} \

#define IxTER1(_condition_exp_,...) \
	if( (_condition_exp_) ) \
	{ \
		printf( __VA_ARGS__ ); fflush(stdout); \
		ERRORLOGC( __VA_ARGS__ ) \
		return -1; \
	} \

#define IxTEFR1(_condition_exp_,...) \
	if( (_condition_exp_) ) \
	{ \
		printf( __VA_ARGS__ ); fflush(stdout); \
		ERRORLOGC( __VA_ARGS__ ) \
		if( ! env->cmd_para.__forcely ) \
			return -1; \
	} \

#define IxTERX(_condition_exp_,_return_val_,...) \
	if( (_condition_exp_) ) \
	{ \
		printf( __VA_ARGS__ ); fflush(stdout); \
		ERRORLOGC( __VA_ARGS__ ) \
		return (_return_val_); \
	} \

#define IxTEx(_condition_exp_,_return_statement_,...) \
	if( (_condition_exp_) ) \
	{ \
		printf( __VA_ARGS__ ); fflush(stdout); \
		ERRORLOGC( __VA_ARGS__ ) \
		_return_statement_; \
	} \

#define IDTI(...) \
	if( env->cmd_para.__debug ) \
	{ \
		printf( __VA_ARGS__ ); fflush(stdout); \
		INFOLOGC( __VA_ARGS__ ) \
	} \

#define IDTE(...) \
	if( env->cmd_para.__debug ) \
	{ \
		printf( __VA_ARGS__ ); fflush(stdout); \
		ERRORLOGC( __VA_ARGS__ ) \
	} \

#define EIDTI(...) \
	else if( env->cmd_para.__debug ) \
	{ \
		printf( __VA_ARGS__ ); fflush(stdout); \
		INFOLOGC( __VA_ARGS__ ) \
	} \

#define EIDTE(...) \
	else if( env->cmd_para.__debug ) \
	{ \
		printf( __VA_ARGS__ ); fflush(stdout); \
		ERRORLOGC( __VA_ARGS__ ) \
	} \

extern char		*_COCKER_VERSION ;

/*
 * string
 */

char *SnprintfV( char *path_buf , int path_bufsize , char *path_format , va_list valist );
char *Snprintf( char *path_buf , int path_bufsize , char *path_format , ... );

char *TrimEnter( char *str );

char *GenerateContainerId( char *images_id , char *container_id );
char *GenerateEthernamePostfix( char *container_id , char *ethername_postfix );

/*
 * file
 */

int CheckAndMakeDir( char *path );

int SnprintfAndCheckDir( char *path_buf , int path_bufsize , char *path_format , ... );
int SnprintfAndChangeDir( char *path_buf , int path_bufsize , char *path_format , ... );
int SnprintfAndMakeDir( char *path_buf , int path_bufsize , char *path_format , ... );

int SnprintfAndUnlink( char *path_buf , int path_bufsize , char *path_format , ... );

int SnprintfAndSystem( char *cmd_buf , int cmd_bufsize , char *cmd_format , ... );
int SnprintfAndPopen( char *output_buf , int output_bufsize , char *cmd_buf , int cmd_bufsize , char *cmd_format , ... );

int WriteFileLine( char *fileline , char *pathfile_buf , int pathfile_bufsize , char *pathfile_format , ... );
int ReadFileLine( char *fileline_buf , int fileline_bufsize , char *pathfile_buf , int pathfile_bufsize , char *pathfile_format , ... );

int IsDirectoryNewThan( char *path , time_t mtime );
int GetDirectorySize( char *path , int *p_directory_size );
int IsDirectoryEmpty( char *version_path_base );

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

