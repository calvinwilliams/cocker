/*
 * cocker - Container Machine Engine
 * author	: calvin
 * email	: calvinwilliams@163.com
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#include "cocker_in.h"

int DoShow_images( struct CockerEnvironment *cocker_env )
{
	DIR		*dir = NULL ;
	struct dirent	*dirent = NULL ;
	DIR		*dir2 = NULL ;
	struct dirent	*dirent2 = NULL ;
	char		version_path_base[ PATH_MAX + 1 ] ;
	char		image_path_base[ PATH_MAX + 1 ] ;
	struct stat	dir_stat ;
	int		count ;
	char		version[ VERSION_LEN_MAX + 1 ] ;
	char		image_id[ IMAGES_ID_LEN_MAX + 1 ] ;
	struct stat	image_path_stat ;
	struct tm	image_path_modifytm ;
	char		image_path_modifytime_buf[ 32 + 1 ] ;
	char		image_rlayer_path[ PATH_MAX + 1 ] ;
	char		image_size_file[ PATH_MAX + 1 ] ;
	struct stat	image_size_file_stat ;
	int		image_size ;
	char		image_size_str[ 32 + 1 ] ;
	char		file_buffer[ 4096 ] ;
	
	int		nret = 0 ;
	
	count = 0 ;
	dir = opendir( cocker_env->images_path_base ) ;
	while( dir )
	{
		dirent = readdir( dir ) ;
		if( dirent == NULL )
			break;
		
		if( STRCMP( dirent->d_name , == , "." ) || STRCMP( dirent->d_name , == , ".." ) )
			continue;
		
		if( Snprintf( version_path_base , sizeof(version_path_base) , "%s/%s" , cocker_env->images_path_base , dirent->d_name ) == NULL )
			continue;
		memset( & dir_stat , 0x00 , sizeof(struct stat) );
		nret = stat( version_path_base , & dir_stat ) ;
		if( nret == -1 )
			continue;
		
		if( ! S_ISDIR(dir_stat.st_mode) )
			continue;
		
		Snprintf( image_id , sizeof(image_id) , "%s" , dirent->d_name );
		
		dir2 = opendir( version_path_base ) ;
		while( dir2 )
		{
			dirent2 = readdir( dir2 ) ;
			if( dirent2 == NULL )
				break;
			
			if( STRCMP( dirent2->d_name , == , "." ) || STRCMP( dirent2->d_name , == , ".." ) )
				continue;
			
			if( Snprintf( image_path_base , sizeof(image_path_base) , "%s/%s" , version_path_base , dirent2->d_name ) == NULL )
				continue;
			memset( & dir_stat , 0x00 , sizeof(struct stat) );
			nret = stat( image_path_base , & dir_stat ) ;
			if( nret == -1 )
				continue;
			if( ! S_ISDIR(dir_stat.st_mode) )
				continue;
			
			Snprintf( version , sizeof(version) , "%s" , dirent2->d_name );
			
			memset( image_path_base , 0x00 , sizeof(image_path_base) );
			Snprintf( image_path_base , sizeof(image_path_base) , "%s/%s" , version_path_base , version );
			stat( image_path_base , & image_path_stat );
			memset( image_path_modifytime_buf , 0x00 , sizeof(image_path_modifytime_buf) );
			localtime_r( & (image_path_stat.st_mtime) , & image_path_modifytm );
			strftime( image_path_modifytime_buf , sizeof(image_path_modifytime_buf) , "%Y-%m-%dT%H:%M:%S" , & image_path_modifytm );
			
			/* size */
			Snprintf( image_rlayer_path , sizeof(image_rlayer_path) , "%s/rlayer" , image_path_base );
			
			memset( file_buffer , 0x00 , sizeof(file_buffer) );
			memset( image_size_file , 0x00 , sizeof(image_size_file) );
			nret = ReadFileLine( file_buffer , sizeof(file_buffer) , image_size_file , sizeof(image_size_file) , "%s/size" , image_path_base ) ;
			if( nret )
			{
				nret = GetDirectorySize( image_rlayer_path , & image_size ) ;
				if( nret )
				{
					image_size = -1 ;
				}
				else
				{
					Snprintf( file_buffer , sizeof(file_buffer) , "%d" , image_size );
					WriteFileLine( file_buffer , image_size_file , sizeof(image_size_file) , "%s/size" , image_path_base );
				}
			}
			else
			{
				stat( image_size_file , & image_size_file_stat );
				nret = IsDirectoryNewThan( image_rlayer_path , image_size_file_stat.st_mtime ) ;
				if( nret < 0 )
				{
					image_size = -1 ;
				}
				else if( nret > 0 )
				{
					nret = GetDirectorySize( image_rlayer_path , & image_size ) ;
					if( nret )
					{
						image_size = -1 ;
					}
					else
					{
						Snprintf( file_buffer , sizeof(file_buffer) , "%d" , image_size );
						WriteFileLine( file_buffer , image_size_file , sizeof(image_size_file) , "%s/size" , image_path_base );
					}
				}
				else
				{
					image_size = atoi(file_buffer) ;
				}
			}
			
			if( image_size > 1024*1024*1024 )
				Snprintf( image_size_str , sizeof(image_size_str) , "%d GB" , image_size / (1024*1024*1024) );
			else if( image_size > 1024*1024 )
				Snprintf( image_size_str , sizeof(image_size_str) , "%d MB" , image_size / (1024*1024) );
			else if( image_size > 1024 )
				Snprintf( image_size_str , sizeof(image_size_str) , "%d KB" , image_size / 1024 );
			else if( image_size >= 0 )
				Snprintf( image_size_str , sizeof(image_size_str) , "%d B" , image_size );
			else
				Snprintf( image_size_str , sizeof(image_size_str) , "(unknow)" );
			
			/* output */
			if( count == 0 )
			{
				printf( "%-30s %-10s %-19s %-10s\n" , "image_id" , "version" , "modify_datetime" , "size" );
				printf( "--------------------------------------------------------------------\n" );
			}
			
			printf( "%-30s %-10s %-19s %s\n" , dirent->d_name , version , image_path_modifytime_buf , image_size_str );
			
			count++;
		}
	}
	
	return 0;
}

