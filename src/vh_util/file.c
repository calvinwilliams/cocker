#include "vh_util.h"

char *SnprintfV( char *path_buf , int path_bufsize , char *path_format , va_list valist )
{
	static char		path[ PATH_MAX ] = "" ;
	
	static char		*p_path = NULL ;
	static int		path_size = 0 ;
	
	int			nret = 0 ;
	
	if( path_buf == NULL )
	{
		p_path = path ;
		path_size = sizeof(path)-1 ;
	}
	else
	{
		p_path = path_buf ;
		path_size = path_bufsize ;
	}
	
	memset( p_path , 0x00 , path_size );
	nret = vsnprintf( p_path , path_size , path_format , valist ) ;
	if( SNPRINTF_OVERFLOW( nret , path_size ) )
		return NULL;
	
	return p_path;
}

int SnprintfAndChangeDir( char *path_buf , int path_bufsize , char *path_format , ... )
{
	char		*p_path = NULL ;
	va_list		valist ;
	
	va_start( valist , path_format );
	p_path = SnprintfV( path_buf , path_bufsize , path_format , valist ) ;
	va_end( valist );
	if( p_path == NULL )
		return -1;
	
	chdir( p_path );
	
	return 0;
}

int CheckAndMakeDir( char *path )
{
	int		nret = 0 ;
	
	nret = access( path , W_OK ) ;
	if( nret == -1 )
	{
		nret = mkdir( path , 00777 ) ;
		if( nret == -1 )
			return -1;
	}
	
	return 0;
}

int SnprintfAndMakeDir( char *path_buf , int path_bufsize , char *path_format , ... )
{
	char		*p_path = NULL ;
	va_list		valist ;
	
	int		nret = 0 ;
	
	va_start( valist , path_format );
	p_path = SnprintfV( path_buf , path_bufsize , path_format , valist ) ;
	va_end( valist );
	if( p_path == NULL )
		return -1;
	
	nret = CheckAndMakeDir( p_path ) ;
	if( nret )
		return OVERLAY_RET(100,nret);
	
	return 0;
}

int SnprintfAndSystem( char *cmd_buf , int cmd_bufsize , char *cmd_format , ... )
{
	char		*p_cmd = NULL ;
	va_list		valist ;
	
	int		nret = 0 ;
	
	va_start( valist , cmd_format );
	p_cmd = SnprintfV( cmd_buf , cmd_bufsize , cmd_format , valist ) ;
	va_end( valist );
	if( p_cmd == NULL )
		return -1;
	
	nret = system( p_cmd ) ;
	if( nret )
		return OVERLAY_RET(100,nret);
	
	return 0;
}

int WriteFileLine( char *fileline , char *pathfile_buf , int pathfile_bufsize , char *pathfile_format , ... )
{
	char		*p_pathfile = NULL ;
	va_list		valist ;
	int		fd ;
	
	va_start( valist , pathfile_format );
	p_pathfile = SnprintfV( pathfile_buf , pathfile_bufsize , pathfile_format , valist ) ;
	va_end( valist );
	if( p_pathfile == NULL )
		return -1;
	
	fd = open( p_pathfile , O_CREAT|O_TRUNC|O_WRONLY , 00777 ) ;
	if( fd == -1 )
		return -1;
	
	write( fd , fileline , strlen(fileline) );
	
	close( fd );
	
	return 0;
}

int ReadFileLine( char *fileline_buf , int fileline_bufsize , char *pathfile_buf , int pathfile_bufsize , char *pathfile_format , ... )
{
	char		*p_pathfile = NULL ;
	va_list		valist ;
	int		fd ;
	int		len ;
	
	va_start( valist , pathfile_format );
	p_pathfile = SnprintfV( pathfile_buf , pathfile_bufsize , pathfile_format , valist ) ;
	va_end( valist );
	if( p_pathfile == NULL )
		return -1;
	
	fd = open( p_pathfile , O_RDONLY , 00777 ) ;
	if( fd == -1 )
		return -1;
	
	len = read( fd , fileline_buf , fileline_bufsize ) ;
	
	close( fd );
	
	if( fileline_buf[len-1] == '\n' )
		fileline_buf[len-1] = '\0' ;
	
	return 0;
}
