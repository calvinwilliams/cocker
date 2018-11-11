/*
 * cocker - Container Machine Engine
 * author	: calvin
 * email	: calvinwilliams@163.com
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#include "cocker_in.h"

int DoShow_ssearch( struct CockerEnvironment *env )
{
	char		repo[ REPO_LEN_MAX + 1 ] ;
	char		repo_file[ PATH_MAX + 1 ] ;
	char		cmd[ 4096 ] ;
	FILE		*pp = NULL ;
	char		buf[ 4096 ] ;
	int		count ;
	int		image_size ;
	char		image_size_str[ 32 + 1] ;
	char		image_modify_datetime[ 32 + 1 ] ;
	char		image_id[ IMAGES_ID_LEN_MAX + 1 ] ;
	char		image_version[ VERSION_LEN_MAX + 1 ] ;
	char		*p = NULL ;
	
	int		nret = 0 ;
	
	if( env->cmd_para.__repo )
	{
		memset( repo_file , 0x00 , sizeof(repo_file) );
		nret = WriteFileLine( env->cmd_para.__repo , repo_file , sizeof(repo_file) , "%s/repo" , env->cocker_home ) ;
		I1TER1( "write %s failed\n" , repo_file )
		EIDTI( "write %s ok\n" , repo_file )
	}
	
	memset( repo , 0x00 , sizeof(repo) );
	memset( repo_file , 0x00 , sizeof(repo_file) );
	nret = ReadFileLine( repo , sizeof(repo) , repo_file , sizeof(repo_file) , "%s/repo" , env->cocker_home ) ;
	I1TER1( "read %s failed\n" , repo_file )
	EIDTI( "read %s ok\n" , repo_file )
	
	if( env->cmd_para.__match == NULL )
	{
		Snprintf( cmd , sizeof(cmd) , "ssh %s ls -l --full-time *.cockerimage 2>/dev/null" , repo ) ;
	}
	else
	{
		Snprintf( cmd , sizeof(cmd) , "ssh %s ls -l --full-time *%s*.cockerimage 2>/dev/null" , repo , env->cmd_para.__match ) ;
	}
	pp = popen( cmd , "r" ) ;
	IxTER1( (pp==NULL) , "popen [%s] failed , errno[%d]\n" , cmd , errno )
	EIDTI( "popen [%s] ok\n" , cmd )
	
	count = 0 ;
	while(1)
	{
		memset( buf , 0x00 , sizeof(buf) );
		if( fgets( buf , sizeof(buf) , pp ) == NULL )
			break;
		TrimEnter( buf );
		
		memset( image_modify_datetime , 0x00 , sizeof(image_modify_datetime) );
		memset( image_id , 0x00 , sizeof(image_id) );
		memset( image_version , 0x00 , sizeof(image_version) );
		sscanf( buf , "%*s%*s%*s%*s%d%s %[^.].%*s%*s%s" , & image_size , image_modify_datetime , image_modify_datetime+10 , image_id );
		image_modify_datetime[10] = 'T' ;
		image_id[strlen(image_id)-1-(sizeof(COCKERIMAGE_FILE_EXTNAME)-1)] = '\0' ;
		p = strchr( image_id , ':' ) ;
		if( p )
		{
			strncpy( image_version , p+1 , sizeof(image_version)-1 );
			(*p) = '\0' ;
		}
		else
		{
			strcpy( image_version , "_" );
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
		
		printf( "%-30s %-10s %-19s %s\n" , image_id , image_version , image_modify_datetime , image_size_str );
		
		count++;
	}
	
	return 0;
}

