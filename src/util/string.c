#include "cocker_util.h"

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
	
	for( p_md5 = md5 , p_md5_exp = (unsigned char *)container_id , i = 0 ; i < CONTAINER_NAME_MAX/2 ; p_md5++ , p_md5_exp+=2 , i++ )
	{
		p_md5_exp[0] = hex_charset[(p_md5[0]&0xF0)>>4] ;
		p_md5_exp[1] = hex_charset[p_md5[0]&0x0F] ;
	}
	
	return container_id;
}
