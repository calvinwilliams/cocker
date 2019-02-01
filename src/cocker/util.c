/*
 * cocker - Container Machine Engine
 * author	: calvin
 * email	: calvinwilliams@163.com
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#include "cocker_in.h"

void GetEthernetNames( struct CockerEnvironment *env , char *container_id )
{
	char		ethername_postfix[ 10 + 1 ] ;
	
	memset( ethername_postfix , 0x00 , sizeof(ethername_postfix) );
	GenerateEthernamePostfix( container_id , ethername_postfix );
	
	memset( env->netns_name , 0x00 , sizeof(env->netns_name) );
	snprintf( env->netns_name , sizeof(env->netns_name) , "nns%.10s" , ethername_postfix );
	
	memset( env->netbr_name , 0x00 , sizeof(env->netbr_name) );
	snprintf( env->netbr_name , sizeof(env->netbr_name) , "cocker0" );
	
	memset( env->veth1_name , 0x00 , sizeof(env->veth1_name) );
	snprintf( env->veth1_name , sizeof(env->veth1_name) , "eth%.10s" , ethername_postfix );
	
	memset( env->veth0_name , 0x00 , sizeof(env->veth0_name) );
	snprintf( env->veth0_name , sizeof(env->veth0_name) , "vth%.10s" , ethername_postfix );
	
	memset( env->veth0_sname , 0x00 , sizeof(env->veth0_sname) );
	snprintf( env->veth0_sname , sizeof(env->veth0_sname) , "eth0" );
	
	return;
}

int SplitImageVersion( char *image_id , char *version , int version_bufsize )
{
	char	*p = NULL ;
	
	memset( version , 0x00 , version_bufsize );
	
	p = strchr( image_id , ':' ) ;
	if( p )
	{
		strncpy( version , p+1 , version_bufsize-1 );
		(*p) = '\0' ;
		
		return 1;
	}
	
	return 0;
}

int GetMaxVersionPath( char *version_path_base , char *max_version , int max_version_bufsize )
{
	DIR		*dir = NULL ;
	struct dirent	*dirent = NULL ;
	char		sub_path[ PATH_MAX + 1 ] ;
	struct stat	dir_stat ;
	int		max_v1 , max_v2 , max_v3 , max_v4 ;
	int		v1 , v2 , v3 , v4 ;
	char		version[ PATH_MAX + 1 ] = "" ;
	
	int		nret = 0 ;
	
	dir = opendir( version_path_base ) ;
	if( dir == NULL )
		return -1;
	
	max_v1 = -1 ;
	max_v2 = -1 ;
	max_v3 = -1 ;
	max_v4 = -1 ;
	while(1)
	{
		dirent = readdir( dir ) ;
		if( dirent == NULL )
			break;
		if( STRCMP( dirent->d_name , == , "." ) || STRCMP( dirent->d_name , == , ".." ) )
			continue;
		
		if( Snprintf( sub_path , sizeof(sub_path) , "%s/%s" , version_path_base , dirent->d_name ) == NULL )
		{
			closedir( dir );
			return -2;
		}
		
		memset( & dir_stat , 0x00 , sizeof(struct stat) );
		nret = stat( sub_path , & dir_stat ) ;
		if( nret == -1 )
		{
			closedir( dir );
			return -3;
		}
		if( ! S_ISDIR(dir_stat.st_mode) )
			continue;
		
		sscanf( dirent->d_name , "%d.%d.%d.%d" , & v1 , & v2 , & v3 , & v4 );
		if( v1 < max_v1 )
			continue;
		if( v2 < max_v2 )
			continue;
		if( v3 < max_v3 )
			continue;
		if( v4 < max_v4 )
			continue;
		
		strncpy( version , dirent->d_name , sizeof(version)-1 );
		max_v1 = v1 ;
		max_v2 = v2 ;
		max_v3 = v3 ;
		max_v4 = v4 ;
	}
	
	closedir( dir );
	
	if( version[0] == '0' )
	{
		return 1;
	}
	else
	{
		strncpy( max_version , version , max_version_bufsize-1 );
		return 0;
	}
}

