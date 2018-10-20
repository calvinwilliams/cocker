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
		
		memset( image_author_file , 0x00 , sizeof(image_author_file) );
		memset( author , 0x00 , sizeof(author) );
		nret = ReadFileLine( author , sizeof(author) , image_author_file , sizeof(image_author_file) , "%s/%s/author" , cocker_env->images_path_base , dirent->d_name ) ;
		if( nret )
		{
			printf( "*** ERROR : ReadFileLine author failed[%d]\n" , nret );
			memset( image_author_file , 0x00 , sizeof(image_author_file) );
			memset( author , 0x00 , sizeof(author) );
		}
		TrimEnter( author );
		
		memset( image_create_datetime_file , 0x00 , sizeof(image_create_datetime_file) );
		memset( create_datetime , 0x00 , sizeof(create_datetime) );
		nret = ReadFileLine( create_datetime , sizeof(create_datetime) , image_create_datetime_file , sizeof(image_create_datetime_file) , "%s/%s/create_datetime" , cocker_env->images_path_base , dirent->d_name ) ;
		if( nret )
		{
			printf( "*** ERROR : ReadFileLine create_datetime failed[%d]\n" , nret );
			memset( image_create_datetime_file , 0x00 , sizeof(image_create_datetime_file) );
			memset( create_datetime , 0x00 , sizeof(create_datetime) );
		}
		TrimEnter( create_datetime );
		
		memset( image_version_file , 0x00 , sizeof(image_version_file) );
		memset( version , 0x00 , sizeof(version) );
		nret = ReadFileLine( version , sizeof(version) , image_version_file , sizeof(image_version_file) , "%s/%s/version" , cocker_env->images_path_base , dirent->d_name ) ;
		if( nret )
		{
			printf( "*** ERROR : ReadFileLine author failed[%d]\n" , nret );
			memset( image_version_file , 0x00 , sizeof(image_version_file) );
			memset( version , 0x00 , sizeof(version) );
		}
		TrimEnter( version );
		
		if( count == 0 )
		{
			printf( "%-10s %-32s %-19s %-16s\n" , "image_id" , "author" , "create_datetime" , "version" );
			printf( "-----------------------------------------------------------------------\n" );
		}
		
		printf( "%-10s %-32s %-19s %-16s\n" , dirent->d_name , author , create_datetime , version );
		
		count++;
	}
	
	return 0;
}

