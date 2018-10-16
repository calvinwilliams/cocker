#include "cocker_util.h"

char *SnprintfV( char *path_buf , int path_bufsize , char *path_format , va_list valist )
{
	static char		path[ 4096 ] = "" ;
	
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
	nret = vsnprintf( p_path , path_size-1 , path_format , valist ) ;
	if( SNPRINTF_OVERFLOW( nret , path_size ) )
		return NULL;
	
	return p_path;
}

char *Snprintf( char *path_buf , int path_bufsize , char *path_format , ... )
{
	char		*p_path = NULL ;
	va_list		valist ;
	
	va_start( valist , path_format );
	p_path = SnprintfV( path_buf , path_bufsize , path_format , valist ) ;
	va_end( valist );
	if( p_path == NULL )
		return NULL;
	
	return p_path;
}

char *TrimEnter( char *str )
{
	char	*ptr = NULL ;
	
	if( str == NULL )
		return NULL;
	
	for( ptr = str + strlen(str) - 1 ; ptr >= str ; ptr-- )
	{
		if( (*ptr) == '\r' || (*ptr) == '\n' )
			(*ptr) = '\0' ;
		else
			break;
	}
	
	return str;
}

void *GenerateContainerId( char *images_id , char *container_id )
{
	MD5_CTX			md5_ctx ;
	struct timeval		now ;
	char			time_buf[ 10+1+6 + 1 ] ;
	unsigned char		md5[ MD5_DIGEST_LENGTH + 1 ] ;
	int			i ;
	unsigned char		*p_md5 = NULL ;
	unsigned char		*p_md5_exp = NULL ;
	const unsigned char	hex_charset[] = "0123456789abcdef" ;
	
	MD5_Init( & md5_ctx );
	
	MD5_Update( & md5_ctx , images_id , strlen(images_id) );
	
	gettimeofday( & now , NULL );
	memset( time_buf , 0x00 , sizeof(time_buf) );
	snprintf( time_buf , sizeof(time_buf)-1 , "%6ld.%06ld" , now.tv_sec , now.tv_usec );
	MD5_Update( & md5_ctx , time_buf , strlen(time_buf) );
	
	memset( md5 , 0x00 , sizeof(md5) );
	MD5_Final( md5 , & md5_ctx );
	
	for( p_md5 = md5 , p_md5_exp = (unsigned char *)container_id , i = 0 ; i < CONTAINER_ID_LEN_MAX/2 ; p_md5++ , p_md5_exp+=2 , i++ )
	{
		p_md5_exp[0] = hex_charset[(p_md5[0]&0xF0)>>4] ;
		p_md5_exp[1] = hex_charset[p_md5[0]&0x0F] ;
	}
	
	return container_id;
}
