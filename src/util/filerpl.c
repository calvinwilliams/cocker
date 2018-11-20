#include "filerpl.h"

#define _DEBUG				0

int strrpl( char **pp_buf , int *p_buf_len , int *p_buf_size , char *key , int key_len , char *value , int value_len )
{
	int		diff_len ;
	char		*p = NULL ;
	int		p_offset ;
	
	if( pp_buf == NULL || (*pp_buf) == NULL || p_buf_len == NULL || p_buf_size == NULL )
		return FILERPL_ERROR_PARAMETER;
	if( key == NULL || value == NULL )
		return FILERPL_ERROR_PARAMETER;
	
	if( key_len < 0 )
		key_len = strlen(key) ;
	if( value_len < 0 )
		value_len = strlen(value) ;
	diff_len = value_len - key_len ;
	
	p = (*pp_buf) - 1 ;
	while(1)
	{
#if _DEBUG
		printf( "strrpl - while - buf[%d/%d][%s]\n" , (*p_buf_len) , (*p_buf_size) , (*pp_buf) );
#endif
		
		p = strstr( p+1 , key ) ;
		if( p == NULL )
			break;
		
#if _DEBUG
		printf( "strrpl - while - p[%.10s...]\n" , p );
#endif
		
		if( (*p_buf_len) + diff_len > (*p_buf_size)-1 )
		{
			char	*new_buf ;
			int	new_buf_size ;
			
			p_offset = p - (*pp_buf) ;
			
			new_buf_size = (*p_buf_size) ;
			if( new_buf_size <= 1024*1024*1024 )
				new_buf_size *= 2 ;
			if( (*p_buf_len) + diff_len > new_buf_size-1 )
				new_buf_size = (*p_buf_len) + diff_len + 1 ;
			new_buf = realloc( (*pp_buf) , new_buf_size ) ;
			if( new_buf == NULL )
				return FILERPL_ERROR_REALLOC;
			
#if _DEBUG
			printf( "strrpl - while - ins_buf_size [%d]->[%d]\n" , (*p_buf_size) , new_buf_size );
#endif
			
			(*pp_buf) = new_buf ;
			(*p_buf_size) = new_buf_size ;
			
			p = (*pp_buf) + p_offset ;
		}
		
		/*
		123KEY456
		   KEY
		   VALUE
		*/
		memmove( p+value_len , p+key_len , (*p_buf_len) - ( p-(*pp_buf) + key_len ) );
		memcpy( p , value , value_len );
		(*p_buf_len) += diff_len ;
		(*pp_buf)[(*p_buf_len)] = '\0' ;
	}
	
	return 0;
}

static int _PrepareBuffer( struct stat *file_stat , char **pp_buf , int *p_buf_len , int *p_buf_size )
{
	if( (*pp_buf) == NULL )
	{
		(*p_buf_size) = file_stat->st_size + 1 ;
		(*pp_buf) = malloc( (*p_buf_size) + 1 ) ;
		if( (*pp_buf) == NULL )
			return FILERPL_ERROR_MALLOC;
		memset( (*pp_buf) , 0x00 , (*p_buf_size) );
		(*p_buf_len) = 0 ;
	}
	else if( (*p_buf_size)-1 < file_stat->st_size )
	{
		int		new_buf_size ;
		char		*new_buf = NULL ;
		
		new_buf_size = file_stat->st_size + 1 + 1 ;
		new_buf = realloc( (*pp_buf) , new_buf_size ) ;
		if( new_buf == NULL )
			return FILERPL_ERROR_REALLOC;
		memset( new_buf , 0x00 , new_buf_size );
		
		(*pp_buf) = new_buf ;
		(*p_buf_size) = new_buf_size ;
		(*p_buf_len) = 0 ;
	}
	else
	{
		memset( (*pp_buf) , 0x00 , (*p_buf_size) );
	}
	
	return 0;
}

int FreeRplBuffer( char **pp_buf , int *p_buf_len , int *p_buf_size )
{
	if( pp_buf == NULL || p_buf_len == NULL || p_buf_size == NULL )
		return FILERPL_ERROR_PARAMETER;
	
	if( (*pp_buf) == NULL )
		return 0;
	
	free( (*pp_buf) );
	(*p_buf_len) = 0 ;
	(*p_buf_size) = 0 ;
	
	return 0;
}

int LoadRplTemplateFile( char *tpl_pathfilename , char **pp_tpl_buf , int *p_tpl_buf_len , int *p_tpl_buf_size )
{
	int		fd ;
	struct stat	file_stat ;
	
	int		nret = 0 ;
	
	if( pp_tpl_buf == NULL )
		return FILERPL_ERROR_PARAMETER;
	
	fd = open( tpl_pathfilename , O_RDONLY ) ;
	if( fd == -1 )
		return FILERPL_ERROR_OPEN;
	
	nret = fstat( fd , & file_stat ) ;
	if( nret == -1 )
	{
		close( fd );
		return FILERPL_ERROR_FSTAT;
	}
	
	nret = _PrepareBuffer( & file_stat , pp_tpl_buf , p_tpl_buf_len , p_tpl_buf_size ) ;
	if( nret )
	{
		close( fd );
		return nret;
	}
	
	(*p_tpl_buf_len) = read( fd , (*pp_tpl_buf) , file_stat.st_size ) ;
	if( (*p_tpl_buf_len) < file_stat.st_size )
	{
		close( fd );
		return FILERPL_ERROR_READ;
	}
	
	close( fd );
	
	return 0;
}

int LoadRplMappingFile( char *map_pathfilename , char **pp_map_buf , int *p_map_buf_len , int *p_map_buf_size )
{
	int		fd ;
	struct stat	file_stat ;
	char		*base = NULL ;
	char		*p1 = NULL ;
	char		*p_key_begin = NULL ;
	int		key_len ;
	char		*p_value_begin = NULL ;
	int		value_len ;
	char		*p2 = NULL ;
	
	int		nret = 0 ;
	
	if( pp_map_buf == NULL )
		return FILERPL_ERROR_PARAMETER;
	
	fd = open( map_pathfilename , O_RDONLY ) ;
	if( fd == -1 )
		return FILERPL_ERROR_OPEN;
	
	nret = fstat( fd , & file_stat ) ;
	if( nret == -1 )
	{
		close( fd );
		return FILERPL_ERROR_FSTAT;
	}
	
	nret = _PrepareBuffer( & file_stat , pp_map_buf , p_map_buf_len , p_map_buf_size ) ;
	if( nret )
	{
		close( fd );
		return nret;
	}
	
	base = mmap( NULL , file_stat.st_size , PROT_READ , MAP_SHARED , fd , 0 ) ;
	if( base == NULL )
	{
		close( fd );
		return FILERPL_ERROR_MMAP;
	}
	
	close( fd );
	
	p1 = base ;
	p2 = (*pp_map_buf) ;
	while(1)
	{
		/*
		key value\n
		 key value\n
		key val ue\n
		*/
		
		/* before key */
		do
		{
			if( (*p1) != ' ' && (*p1) != '\t' )
			{
				break;
			}
			else if( (*p1) == '\0' )
			{
				break;
			}
			else if( (*p1) == '\r' || (*p1) == '\n' )
			{
				break;
			}
		}
		while( p1++ );
		if( (*p1) == '\0' )
			break;
		if( (*p1) == '\r' || (*p1) == '\n' )
		{
			p1++;
			continue;
		}
		
		p_key_begin = p1 ;
		
		/* key */
		do
		{
			if( (*p1) == ' ' || (*p1) == '\t'  )
			{
				break;
			}
			else if( (*p1) == '\0' )
			{
				munmap( base , 0 );
				return FILERPL_ERROR_MAP_INVALID_1;
			}
			else if( (*p1) == '\r' || (*p1) == '\n' )
			{
				munmap( base , 0 );
				return FILERPL_ERROR_MAP_INVALID_2;
			}
		}
		while( p1++ );
		
		key_len = p1 - p_key_begin ;
		if( key_len <= 0 )
		{
			munmap( base , 0 );
			return FILERPL_ERROR_MAP_INVALID_3;
		}
		memcpy( p2 , p_key_begin , key_len );
		p2 += key_len + 1 ;
		(*p_map_buf_len) += key_len + 1 ;
		
		/* before value */
		do
		{
			if( (*p1) != ' ' && (*p1) != '\t' )
			{
				break;
			}
			else if( (*p1) == '\0' )
			{
				munmap( base , 0 );
				return FILERPL_ERROR_MAP_INVALID_4;
			}
			else if( (*p1) == '\r' || (*p1) == '\n' )
			{
				munmap( base , 0 );
				return FILERPL_ERROR_MAP_INVALID_5;
			}
		}
		while( p1++ );
		
		p_value_begin = p1 ;
		
		/* value */
		do
		{
			if( (*p1) == ' ' || (*p1) == '\t'  )
			{
				break;
			}
			else if( (*p1) == '\0' )
			{
				break;
			}
			else if( (*p1) == '\r' || (*p1) == '\n' )
			{
				break;
			}
		}
		while( p1++ );
		
		value_len = p1 - p_value_begin ;
		if( value_len > 0 )
			memcpy( p2 , p_value_begin , value_len );
		p2 += value_len + 1 ;
		(*p_map_buf_len) += value_len + 1 ;
		
		if( (*p1) == '\0' )
			break;
	}
	
	munmap( base , 0 );
	
	return 0;
}

int ConvertRplBuffer( char **pp_ins_buf , int *p_ins_buf_len , int *p_ins_buf_size , char *p_map_buf , int map_buf_len )
{
	char		*key = NULL ;
	char		*value = NULL ;
	
	int		nret = 0 ;
	
	key = p_map_buf ;
	while( (*key) )
	{
		value = key + strlen(key) + 1 ;
		
#if _DEBUG
		printf( "ConvertRplBuffer - key[%s] value[%s] buf[%.*s]\n" , key , value , (*p_ins_buf_len) , (*pp_ins_buf) );
#endif
		
		nret = strrpl( pp_ins_buf , p_ins_buf_len , p_ins_buf_size , key , -1 , value , -1 ) ;
		if( nret )
			return nret;
		
		key = value + strlen(value) + 1 ;
	}
	
	return 0;
}

int DumpRplInstanceFile( char *p_ins_buf , int ins_buf_len , char *ins_pathfilename )
{
	int		fd ;
	
	int		nret = 0 ;
	
	if( p_ins_buf == NULL || ins_buf_len < 0 || ins_pathfilename == NULL )
		return FILERPL_ERROR_PARAMETER;
	
	fd = open( ins_pathfilename , O_CREAT|O_WRONLY , 00777 ) ;
	if( fd == -1 )
		return FILERPL_ERROR_OPEN;
	
	nret = write( fd , p_ins_buf , ins_buf_len ) ;
	if( nret < ins_buf_len )
	{
		close( fd );
		return FILERPL_ERROR_WRITE;
	}
	
	close( fd );
	
	return 0;
}

int filerpl( char *tpl_pathfilename , char *map_pathfilename , char *ins_pathfilename )
{
	char		*ins_buf = NULL ;
	int		ins_buf_len = 0 ;
	int		ins_buf_size = 0 ;
	char		*map_buf = NULL ;
	int		map_buf_len = 0 ;
	int		map_buf_size = 0 ;
	
	int		nret = 0 ;
	
#if _DEBUG
	printf( "ConvertRplFile - LoadRplTemplateFile\n" );
#endif
	
	nret = LoadRplTemplateFile( tpl_pathfilename , & ins_buf , & ins_buf_len , & ins_buf_size ) ;
	if( nret )
		return nret;
	
#if _DEBUG
	printf( "ConvertRplFile tpl_buf[%d/%d][%s]\n" , ins_buf_len , ins_buf_size , ins_buf );
#endif
	
#if _DEBUG
	printf( "ConvertRplFile - LoadRplMappingFile\n" );
#endif
	
	nret = LoadRplMappingFile( map_pathfilename , & map_buf , & map_buf_len , & map_buf_size ) ;
	if( nret )
		return nret;
	
#if _DEBUG
	printf( "ConvertRplFile - map_buf[%d/%d][%s]\n" , map_buf_len , map_buf_size , map_buf );
	
	{
	char		*key = NULL ;
	char		*value = NULL ;
	
	key = map_buf ;
	while( (*key) )
	{
		value = key + strlen(key) + 1 ;
		
		printf( "ConvertRplFile - key[%s] value[%s]\n" , key , value );
		
		key = value + strlen(value) + 1 ;
	}
	}
#endif
	
#if _DEBUG
	printf( "ConvertRplFile - ConvertRplBuffer\n" );
#endif
	
	nret = ConvertRplBuffer( & ins_buf , & ins_buf_len , & ins_buf_size , map_buf , map_buf_len ) ;
	if( nret )
		return nret;
	
#if _DEBUG
	printf( "ConvertRplFile - ins_buf[%d/%d][%s]\n" , ins_buf_len , ins_buf_size , ins_buf );
#endif
	
#if _DEBUG
	printf( "ConvertRplFile - DumpRplInstanceFile\n" );
#endif
	
	nret = DumpRplInstanceFile( ins_buf , ins_buf_len , ins_pathfilename ) ;
	if( nret )
		return nret;
	
	FreeRplBuffer( & ins_buf , & ins_buf_len , & ins_buf_size );
	FreeRplBuffer( & map_buf , & map_buf_len , & map_buf_size );
	
	return 0;
}

#if 0
int main()
{
	int		nret = 0 ;
	
	nret = ConvertRplFile( "tpl.txt" , "map.txt" , "ins.txt" ) ;
	if( nret )
	{
		printf( "ConvertRplFile failed[%d]\n" , nret );
		return 1;
	}
	else
	{
		printf( "ConvertRplFile ok\n" );
		return 0;
	}
}
#endif

