#ifndef _H_LOGC_
#define _H_LOGC_

/*
 * iLOG3Lite - log function library written in c
 * author	: calvin
 * email	: calvinwilliams@163.com
 * LastVersion	: v1.0.15
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#if ( defined _WIN32 )
#include <windows.h>
#include <share.h>
#include <io.h>
#include <fcntl.h>
#elif ( defined __unix ) || ( defined _AIX ) || ( defined __linux__ ) || ( defined __hpux )
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <syslog.h>
#include <pthread.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* 公共宏 */
#ifndef MAXLEN_FILENAME
#define MAXLEN_FILENAME			256
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

#ifndef MEMCMP
#define MEMCMP(_a_,_C_,_b_,_n_) ( memcmp(_a_,_b_,_n_) _C_ 0 )
#endif

/* 跨平台宏 */
#if ( defined __linux__ ) || ( defined __unix ) || ( defined _AIX )
#define TLS		__thread
#define VSNPRINTF	vsnprintf
#define SNPRINTF	snprintf
#define OPEN		open
#define READ		read
#define WRITE		write
#define CLOSE		close
#define PROCESSID	(unsigned long)getpid()
#define THREADID	(unsigned long)pthread_self()
#define NEWLINE		"\n"
#elif ( defined _WIN32 )
#define TLS		__declspec( thread )
#define VSNPRINTF	_vsnprintf
#define SNPRINTF	_snprintf
#define OPEN		_open
#define READ		_read
#define WRITE		_write
#define CLOSE		_close
#define PROCESSID	(unsigned long)GetCurrentProcessId()
#define THREADID	(unsigned long)GetCurrentThreadId()
#define NEWLINE		"\r\n"
#endif

/* 日志等级 */
#define LOGCLEVEL_INVALID	-1
#define LOGCLEVEL_DEBUG		0
#define LOGCLEVEL_INFO		1
#define LOGCLEVEL_NOTICE	2
#define LOGCLEVEL_WARN		3
#define LOGCLEVEL_ERROR		4
#define LOGCLEVEL_FATAL		5
#define LOGCLEVEL_NOLOG		6

/* 设置日志属性 */
void SetLogcFile( char *format , ... );
void SetLogcFileV( char *format , va_list valist );
char *GetLogcFilePtr();
void SetLogcLevel( int log_level );
int GetLogcLevel();

#define LOGC_MAXLEN_CUST_LABEL	64

void SetCustLabel1( char *cust_label );
void SetCustLabel2( char *cust_label );
void SetCustLabel3( char *cust_label );
void SetCustLabel4( char *cust_label );
void SetCustLabel5( char *cust_label );

/* 输出日志 */
#if ( defined __STDC_VERSION__ ) && ( __STDC_VERSION__ >= 199901 )

int WriteLogcBaseV( int log_level , char *c_filename , long c_fileline , char *format , va_list valist );
int WriteLogcBase( int log_level , char *c_filename , long c_fileline , char *format , ... );

extern TLS int _g_logc_level ;

#define WriteLevelLogc(_log_level_,_c_filename_,_c_fileline_,...) \
	{ if( (_log_level_) >= _g_logc_level ) WriteLogcBase( _log_level_ , _c_filename_ , _c_fileline_ , __VA_ARGS__ ); }

#define WriteFatalLogc(_c_filename_,_c_fileline_,...) \
	{ if( LOGCLEVEL_FATAL >= _g_logc_level ) WriteLogcBase( LOGCLEVEL_FATAL , _c_filename_ , _c_fileline_ , __VA_ARGS__ ); }

#define WriteErrorLogc(_c_filename_,_c_fileline_,...) \
	{ if( LOGCLEVEL_ERROR >= _g_logc_level ) WriteLogcBase( LOGCLEVEL_ERROR , _c_filename_ , _c_fileline_ , __VA_ARGS__ ); }

#define WriteWarnLogc(_c_filename_,_c_fileline_,...) \
	{ if( LOGCLEVEL_WARN >= _g_logc_level ) WriteLogcBase( LOGCLEVEL_WARN , _c_filename_ , _c_fileline_ , __VA_ARGS__ ); }

#define WriteNoticeLogc(_c_filename_,_c_fileline_,...) \
	{ if( LOGCLEVEL_NOTICE >= _g_logc_level ) WriteLogcBase( LOGCLEVEL_NOTICE , _c_filename_ , _c_fileline_ , __VA_ARGS__ ); }

#define WriteInfoLogc(_c_filename_,_c_fileline_,...) \
	{ if( LOGCLEVEL_INFO >= _g_logc_level ) WriteLogcBase( LOGCLEVEL_INFO , _c_filename_ , _c_fileline_ , __VA_ARGS__ ); }

#define WriteDebugLogc(_c_filename_,_c_fileline_,...) \
	{ if( LOGCLEVEL_DEBUG >= _g_logc_level ) WriteLogcBase( LOGCLEVEL_DEBUG , _c_filename_ , _c_fileline_ , __VA_ARGS__ ); }

int WriteHexLogcBaseV( int log_level , char *c_filename , long c_fileline , char *buf , long buflen , char *format , va_list valist );
int WriteHexLogcBase( int log_level , char *c_filename , long c_fileline , char *buf , long buflen , char *format , ... );

#define WriteLevelHexLogc(_log_level_,_c_filename_,_c_fileline_,_buf_,_buflen_,...) \
	{ if( (_log_level_) >= _g_logc_level ) WriteHexLogcBase( _log_level_ , _c_filename_ , _c_fileline_ , _buf_ , _buflen_ , __VA_ARGS__ ); }

#define WriteFatalHexLogc(_c_filename_,_c_fileline_,_buf_,_buflen_,...) \
	{ if( LOGCLEVEL_FATAL >= _g_logc_level ) WriteHexLogcBase( LOGCLEVEL_FATAL , _c_filename_ , _c_fileline_ , _buf_ , _buflen_ , __VA_ARGS__ ); }

#define WriteErrorHexLogc(_c_filename_,_c_fileline_,_buf_,_buflen_,...) \
	{ if( LOGCLEVEL_ERROR >= _g_logc_level ) WriteHexLogcBase( LOGCLEVEL_ERROR , _c_filename_ , _c_fileline_ , _buf_ , _buflen_ , __VA_ARGS__ ); }

#define WriteWarnHexLogc(_c_filename_,_c_fileline_,_buf_,_buflen_,...) \
	{ if( LOGCLEVEL_WARN >= _g_logc_level ) WriteHexLogcBase( LOGCLEVEL_WARN , _c_filename_ , _c_fileline_ , _buf_ , _buflen_ , __VA_ARGS__ ); }

#define WriteNoticeHexLogc(_c_filename_,_c_fileline_,_buf_,_buflen_,...) \
	{ if( LOGCLEVEL_NOTICE >= _g_logc_level ) WriteHexLogcBase( LOGCLEVEL_NOTICE , _c_filename_ , _c_fileline_ , _buf_ , _buflen_ , __VA_ARGS__ ); }

#define WriteInfoHexLogc(_c_filename_,_c_fileline_,_buf_,_buflen_,...) \
	{ if( LOGCLEVEL_INFO >= _g_logc_level ) WriteHexLogcBase( LOGCLEVEL_INFO , _c_filename_ , _c_fileline_ , _buf_ , _buflen_ , __VA_ARGS__ ); }

#define WriteDebugHexLogc(_c_filename_,_c_fileline_,_buf_,_buflen_,...) \
	{ if( LOGCLEVEL_DEBUG >= _g_logc_level ) WriteHexLogcBase( LOGCLEVEL_DEBUG , _c_filename_ , _c_fileline_ , _buf_ , _buflen_ , __VA_ARGS__ ); }

#else

int WriteLevelLogc( int log_level , char *c_filename , long c_fileline , char *format , ... );
int WriteFatalLogc( char *c_filename , long c_fileline , char *format , ... );
int WriteErrorLogc( char *c_filename , long c_fileline , char *format , ... );
int WriteWarnLogc( char *c_filename , long c_fileline , char *format , ... );
int WriteNoticeLogc( char *c_filename , long c_fileline , char *format , ... );
int WriteInfoLogc( char *c_filename , long c_fileline , char *format , ... );
int WriteDebugLogc( char *c_filename , long c_fileline , char *format , ... );

int WriteLevelHexLogc( int log_level , char *c_filename , long c_fileline , char *buf , long buflen , char *format , ... );
int WriteFatalHexLogc( char *c_filename , long c_fileline , char *buf , long buflen , char *format , ... );
int WriteErrorHexLogc( char *c_filename , long c_fileline , char *buf , long buflen , char *format , ... );
int WriteWarnHexLogc( char *c_filename , long c_fileline , char *buf , long buflen , char *format , ... );
int WriteNoticeHexLogc( char *c_filename , long c_fileline , char *buf , long buflen , char *format , ... );
int WriteInfoHexLogc( char *c_filename , long c_fileline , char *buf , long buflen , char *format , ... );
int WriteDebugHexLogc( char *c_filename , long c_fileline , char *buf , long buflen , char *format , ... );

#endif

#if ( defined __STDC_VERSION__ ) && ( __STDC_VERSION__ >= 199901 )

#define WRITELOGC(_log_level_,...)	WriteLevelLogc( _log_level_ , __FILE__ , __LINE__ , __VA_ARGS__ );
#define FATALLOGC(...)			WriteFatalLogc( __FILE__ , __LINE__ , __VA_ARGS__ );
#define ERRORLOGC(...)			WriteErrorLogc( __FILE__ , __LINE__ , __VA_ARGS__ );
#define WARNLOGC(...)			WriteWarnLogc( __FILE__ , __LINE__ , __VA_ARGS__ );
#define NOTICELOGC(...)			WriteNoticeLogc( __FILE__ , __LINE__ , __VA_ARGS__ );
#define INFOLOGC(...)			WriteInfoLogc( __FILE__ , __LINE__ , __VA_ARGS__ );
#define DEBUGLOGC(...)			WriteDebugLogc( __FILE__ , __LINE__ , __VA_ARGS__ );

#define WRITEHEXLOGC(_log_level_,_buf_,_buflen_,...)	WriteLevelHexLogc( _log_level_ , __FILE__ , __LINE__ , _buf_ , _buflen_ , __VA_ARGS__ );
#define FATALHEXLOGC(_buf_,_buflen_,...)		WriteFatalHexLogc( __FILE__ , __LINE__ , _buf_ , _buflen_ , __VA_ARGS__ );
#define ERRORHEXLOGC(_buf_,_buflen_,...)		WriteErrorHexLogc( __FILE__ , __LINE__ , _buf_ , _buflen_ , __VA_ARGS__ );
#define WARNHEXLOGC(_buf_,_buflen_,...)			WriteWarnHexLogc( __FILE__ , __LINE__ , _buf_ , _buflen_ , __VA_ARGS__ );
#define NOTICEHEXLOGC(_buf_,_buflen_,...)		WriteNoticeHexLogc( __FILE__ , __LINE__ , _buf_ , _buflen_ , __VA_ARGS__ );
#define INFOHEXLOGC(_buf_,_buflen_,...)			WriteInfoHexLogc( __FILE__ , __LINE__ , _buf_ , _buflen_ , __VA_ARGS__ );
#define DEBUGHEXLOGC(_buf_,_buflen_,...)		WriteDebugHexLogc( __FILE__ , __LINE__ , _buf_ , _buflen_ , __VA_ARGS__ );

#endif

#ifdef __cplusplus
}
#endif

#endif

