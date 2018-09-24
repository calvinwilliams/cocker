#include "vh_util.h"

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

int SnprintfAndMakeDir( char *path_buf , int path_bufsize , char *format , ... )
{
	va_list		valist ;
	int		nret = 0 ;
	
	va_start( valist , format );
	nret = vsnprintf( path_buf , path_bufsize , format , valist ) ;
	va_end( valist );
	if( SNPRINTF_OVERFLOW( nret , path_bufsize ) )
		return -1;
	
	nret = CheckAndMakeDir( path_buf ) ;
	if( nret )
		return OVERLAY_RET(100,nret);
	
	return 0;
}

int WriteFileLine( char *fileline , char *pathfile_format , ... )
{
	va_list		valist ;
	char		pathfilename[ PATH_MAX ] ;
	int		fd ;
	
	int		nret = 0 ;
	
	va_start( valist , pathfile_format );
	nret = vsnprintf( pathfilename , sizeof(pathfilename)-1 , pathfile_format , valist ) ;
	va_end( valist );
	if( SNPRINTF_OVERFLOW( nret , sizeof(pathfilename)-1 ) )
		return -1;
	
	fd = open( pathfilename , O_CREAT|O_TRUNC|O_WRONLY , 00777 ) ;
	if( fd == -1 )
		return -1;
	
	write( fd , fileline , strlen(fileline) );
	
	close( fd );
	
	return 0;
}

int ReadFileLine( char *fileline_buf , int fileline_bufsize , char *pathfile_format , ... )
{
	va_list		valist ;
	char		pathfilename[ PATH_MAX ] ;
	int		fd ;
	int		len ;
	
	int		nret = 0 ;
	
	va_start( valist , pathfile_format );
	nret = vsnprintf( pathfilename , sizeof(pathfilename)-1 , pathfile_format , valist ) ;
	va_end( valist );
	if( SNPRINTF_OVERFLOW( nret , sizeof(pathfilename)-1 ) )
		return -1;
	
	fd = open( pathfilename , O_RDONLY , 00777 ) ;
	if( fd == -1 )
		return -1;
	
	len = read( fd , fileline_buf , fileline_bufsize ) ;
	
	close( fd );
	
	if( fileline_buf[len-1] == '\n' )
		fileline_buf[len-1] = '\0' ;
	
	return 0;
}

