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
	int		count ;
	char		image_author_file[ PATH_MAX + 1 ] ;
	char		author[ AUTHOR_LEN_MAX + 1 ] ;
	char		image_create_datetime_file[ PATH_MAX + 1 ] ;
	char		create_datetime[ CREATE_DATATIME_LEN_MAX + 1 ] ;
	char		image_version_file[ PATH_MAX + 1 ] ;
	char		version[ VERSION_LEN_MAX + 1 ] ;
	char		image_rlayer_path[ PATH_MAX + 1 ] ;
	char		image_size_file[ PATH_MAX + 1 ] ;
	struct stat	image_size_file_stat ;
	int		image_size ;
	char		image_size_str[ 32 + 1 ] ;
	char		file_buffer[ 4096 ] ;
	
	int		nret = 0 ;
	
	dir = opendir( cocker_env->images_path_base ) ;
	count = 0 ;
	while(1)
	{
		dirent = readdir( dir ) ;
		if( dirent == NULL )
			break;
		
		if( STRCMP( dirent->d_name , == , "." ) || STRCMP( dirent->d_name , == , ".." ) )
			continue;
		if( dirent->d_type != DT_DIR )
			continue;
		
		/* author */
		memset( author , 0x00 , sizeof(author) );
		memset( image_author_file , 0x00 , sizeof(image_author_file) );
		nret = ReadFileLine( author , sizeof(author) , image_author_file , sizeof(image_author_file) , "%s/%s/author" , cocker_env->images_path_base , dirent->d_name ) ;
		if( nret )
		{
			memset( image_author_file , 0x00 , sizeof(image_author_file) );
			memset( author , 0x00 , sizeof(author) );
		}
		TrimEnter( author );
		
		/* create_datetime */
		memset( create_datetime , 0x00 , sizeof(create_datetime) );
		memset( image_create_datetime_file , 0x00 , sizeof(image_create_datetime_file) );
		nret = ReadFileLine( create_datetime , sizeof(create_datetime) , image_create_datetime_file , sizeof(image_create_datetime_file) , "%s/%s/create_datetime" , cocker_env->images_path_base , dirent->d_name ) ;
		if( nret )
		{
			memset( image_create_datetime_file , 0x00 , sizeof(image_create_datetime_file) );
			memset( create_datetime , 0x00 , sizeof(create_datetime) );
		}
		TrimEnter( create_datetime );
		
		/* version */
		memset( version , 0x00 , sizeof(version) );
		memset( image_version_file , 0x00 , sizeof(image_version_file) );
		nret = ReadFileLine( version , sizeof(version) , image_version_file , sizeof(image_version_file) , "%s/%s/version" , cocker_env->images_path_base , dirent->d_name ) ;
		if( nret )
		{
			memset( image_version_file , 0x00 , sizeof(image_version_file) );
			memset( version , 0x00 , sizeof(version) );
		}
		TrimEnter( version );
		
		/* size */
		Snprintf( image_rlayer_path , sizeof(image_rlayer_path) , "%s/%s/rlayer" , cocker_env->images_path_base , dirent->d_name );
		
		memset( file_buffer , 0x00 , sizeof(file_buffer) );
		memset( image_size_file , 0x00 , sizeof(image_size_file) );
		nret = ReadFileLine( file_buffer , sizeof(file_buffer) , image_size_file , sizeof(image_size_file) , "%s/%s/size" , cocker_env->images_path_base , dirent->d_name ) ;
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
				WriteFileLine( file_buffer , image_size_file , sizeof(image_size_file) , "%s/%s/size" , cocker_env->images_path_base , dirent->d_name );
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
					WriteFileLine( file_buffer , image_size_file , sizeof(image_size_file) , "%s/%s/size" , cocker_env->images_path_base , dirent->d_name );
				}
			}
			else
			{
				image_size = atoi(file_buffer) ;
			}
		}
		
		if( image_size == -1 )
			Snprintf( image_size_str , sizeof(image_size_str) , "(unknow)" );
		else if( image_size > 1024*1024*1024 )
			Snprintf( image_size_str , sizeof(image_size_str) , "%d GB" , image_size / (1024*1024*1024) );
		else if( image_size > 1024*1024 )
			Snprintf( image_size_str , sizeof(image_size_str) , "%d MB" , image_size / (1024*1024) );
		else if( image_size > 1024 )
			Snprintf( image_size_str , sizeof(image_size_str) , "%d KB" , image_size / 1024 );
		else
			Snprintf( image_size_str , sizeof(image_size_str) , "%d B" , image_size );
		
		/* output */
		if( count == 0 )
		{
			printf( "%-20s %-10s %-30s %-19s %-10s\n" , "image_id" , "version" , "author" , "create_datetime" , "size" );
			printf( "-----------------------------------------------------------------------------------------\n" );
		}
		
		printf( "%-20s %-10s %-30s %-19s %s\n" , dirent->d_name , version , author , create_datetime , image_size_str );
		
		count++;
	}
	
	return 0;
}

