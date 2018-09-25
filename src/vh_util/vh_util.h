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
#define __USE_GNU
#include <sched.h>

#include "LOGC.h"

#ifdef __cplusplus
extern "C" {
#endif

extern char	*_OPENVH_VERSION ;

#ifndef SNPRINTF_OVERFLOW
#define SNPRINTF_OVERFLOW(_ret_,_sizeof_)	( (_ret_) == -1 || (_ret_) >= (_sizeof_) )
#endif

#ifndef OVERLAY_RET
#define OVERLAY_RET(_ret_,_inframe_ret_)	((_inframe_ret_)<0?-(_ret_)+(_inframe_ret_):(_ret_)+(_inframe_ret_))
#endif

/*
 * file
 */

char *SnprintfV( char *path_buf , int path_bufsize , char *path_format , va_list valist );

int CheckAndMakeDir( char *path );

int SnprintfAndChangeDir( char *path_buf , int path_bufsize , char *path_format , ... );
int SnprintfAndMakeDir( char *path_buf , int path_bufsize , char *path_format , ... );

int SnprintfAndUnlink( char *path_buf , int path_bufsize , char *path_format , ... );

int SnprintfAndSystem( char *cmd_buf , int cmd_bufsize , char *cmd_format , ... );

int WriteFileLine( char *fileline , char *pathfile_buf , int pathfile_bufsize , char *pathfile_format , ... );
int ReadFileLine( char *fileline_buf , int fileline_bufsize , char *pathfile_buf , int pathfile_bufsize , char *pathfile_format , ... );

#ifdef __cplusplus
}
#endif

#endif

