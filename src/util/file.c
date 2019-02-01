/*
 * cocker - Container Machine Engine
 * author	: calvin
 * email	: calvinwilliams@163.com
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#include "cocker_util.h"

int SnprintfAndCheckDir( char *path_buf , int path_bufsize , char *path_format , ... )
{
	char		*p_path = NULL ;
	va_list		valist ;
	struct stat	dir_stat ;
	
	int		nret = 0 ;
	
	va_start( valist , path_format );
	p_path = SnprintfV( path_buf , path_bufsize , path_format , valist ) ;
	va_end( valist );
	if( p_path == NULL )
		return -1;
	
	memset( & dir_stat , 0x00 , sizeof(struct stat) );
	nret = stat( p_path , & dir_stat ) ;
	if( nret == -1 )
		return -1;
	if( ! S_ISDIR(dir_stat.st_mode) )
		return -2;
	
	return 0;
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
	
	nret = access( path , F_OK ) ;
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

int SnprintfAndUnlink( char *path_buf , int path_bufsize , char *path_format , ... )
{
	char		*p_file = NULL ;
	va_list		valist ;
	
	int		nret = 0 ;
	
	va_start( valist , path_format );
	p_file = SnprintfV( path_buf , path_bufsize , path_format , valist ) ;
	va_end( valist );
	if( p_file == NULL )
		return -1;
	
	nret = unlink( p_file ) ;
	if( nret == -1 )
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

int SnprintfAndPopen( char *output_buf , int output_bufsize , char *cmd_buf , int cmd_bufsize , char *cmd_format , ... )
{
	char		*p_cmd = NULL ;
	FILE		*popen_fp = NULL ;
	va_list		valist ;
	
	int		nret = 0 ;
	
	va_start( valist , cmd_format );
	p_cmd = SnprintfV( cmd_buf , cmd_bufsize , cmd_format , valist ) ;
	va_end( valist );
	if( p_cmd == NULL )
		return -1;
	
	popen_fp = popen( p_cmd , "r" ) ;
	if( popen_fp == NULL )
		return -2;
	
	if( output_buf )
	{
		memset( output_buf , 0x00 , output_bufsize );
		nret = fread( output_buf , output_bufsize , 1 , popen_fp ) ;
		if( nret == -1 )
			return -3;
	}
	
	pclose( popen_fp );
	
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
		return 1;
	
	len = read( fd , fileline_buf , fileline_bufsize ) ;
	
	close( fd );
	
	if( fileline_buf[len-1] == '\n' )
		fileline_buf[len-1] = '\0' ;
	
	return 0;
}

int IsDirectoryNewThan( char *path , time_t mtime )
{
	DIR		*dir = NULL ;
	struct dirent	*dirent = NULL ;
	char		sub_path[ PATH_MAX + 1 ] ;
	struct stat	dir_stat ;
	
	int		nret = 0 ;
	
	dir = opendir( path ) ;
	if( dir == NULL )
		return -1;
	while(1)
	{
		dirent = readdir( dir ) ;
		if( dirent == NULL )
			break;
		if( STRCMP( dirent->d_name , == , "." ) || STRCMP( dirent->d_name , == , ".." ) )
			continue;
		
		if( Snprintf( sub_path , sizeof(sub_path) , "%s/%s" , path , dirent->d_name ) == NULL )
			return -2;
		
		memset( & dir_stat , 0x00 , sizeof(struct stat) );
		nret = stat( sub_path , & dir_stat ) ;
		if( nret == -1 )
			return -3;
		if( S_ISDIR(dir_stat.st_mode) )
		{
			if( dir_stat.st_mtime > mtime )
				return 1;
			
			return IsDirectoryNewThan( sub_path , mtime );
		}
	}
	
	return 0;
}

static int _GetDirectorySize( char *path , int *p_directory_size )
{
	DIR		*dir = NULL ;
	struct dirent	*dirent = NULL ;
	char		sub_path[ PATH_MAX + 1 ] ;
	struct stat	file_stat ;
	
	int		nret = 0 ;
	
	dir = opendir( path ) ;
	if( dir == NULL )
		return -1;
	
	while(1)
	{
		dirent = readdir( dir ) ;
		if( dirent == NULL )
			break;
		if( STRCMP( dirent->d_name , == , "." ) || STRCMP( dirent->d_name , == , ".." ) )
			continue;
		
		if( Snprintf( sub_path , sizeof(sub_path) , "%s/%s" , path , dirent->d_name ) == NULL )
		{
			closedir( dir );
			return -2;
		}
		
		memset( & file_stat , 0x00 , sizeof(struct stat) );
		nret = stat( sub_path , & file_stat ) ;
		if( nret == -1 )
		{
			closedir( dir );
			return -3;
		}
		
		if( S_ISDIR(file_stat.st_mode)  )
		{
			nret = _GetDirectorySize( sub_path , p_directory_size ) ;
			if( nret )
				return nret;
		}
		else if( S_ISREG(file_stat.st_mode) )
		{
			(*p_directory_size) += file_stat.st_size ;
		}
	}
	
	closedir( dir );
	
	return 0;
}

int GetDirectorySize( char *path , int *p_directory_size )
{
	(*p_directory_size) = 0 ;
	return _GetDirectorySize( path , p_directory_size );
}

int IsDirectoryEmpty( char *version_path_base )
{
	DIR		*dir = NULL ;
	struct dirent	*dirent = NULL ;
	
	dir = opendir( version_path_base ) ;
	if( dir == NULL )
		return -1;
	
	while(1)
	{
		dirent = readdir( dir ) ;
		if( dirent == NULL )
			break;
		if( STRCMP( dirent->d_name , == , "." ) || STRCMP( dirent->d_name , == , ".." ) )
			continue;
		
		closedir( dir );
		return 1;
	}
	
	closedir( dir );
	return 0;
}

