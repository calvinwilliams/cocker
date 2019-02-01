/*
 * cocker - Container Machine Engine
 * author	: calvin
 * email	: calvinwilliams@163.com
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#include "cocker_in.h"

int DoShow_containers( struct CockerEnvironment *cocker_env )
{
	DIR		*dir = NULL ;
	struct dirent	*dirent = NULL ;
	char		container_path_base[ PATH_MAX + 1 ] ;
	struct stat	dir_stat ;
	int		count ;
	
	char		container_image_file[ PATH_MAX + 1 ] ;
	char		image[ 4096 ] ;
	char		container_hostname_file[ PATH_MAX + 1 ] ;
	char		hostname[ HOST_NAME_MAX ] ;
	char		container_net_file[ PATH_MAX + 1 ] ;
	char		net[ NET_LEN_MAX ] ;
	char		container_netns_file[ PATH_MAX + 1 ] ;
	char		netns[ NETNS_NAME_LEN_MAX ] ;
	char		container_pid_file[ PATH_MAX + 1 ] ;
	char		pid_str[ PID_LEN_MAX ] ;
	pid_t		pid ;
	char		status[ 40 ] ;
	char		container_rwlayer_path[ PATH_MAX + 1 ] ;
	char		container_size_file[ PATH_MAX + 1 ] ;
	struct stat	container_size_file_stat ;
	int		container_size ;
	char		container_size_str[ 20 + 1 ] ;
	char		file_buffer[ 4096 ] ;
	
	int		nret = 0 ;
	
	dir = opendir( cocker_env->containers_path_base ) ;
	count = 0 ;
	while(1)
	{
		dirent = readdir( dir ) ;
		if( dirent == NULL )
			break;
		
		if( STRCMP( dirent->d_name , == , "." ) || STRCMP( dirent->d_name , == , ".." ) )
			continue;
		
		if( Snprintf( container_path_base , sizeof(container_path_base) , "%s/%s" , cocker_env->containers_path_base , dirent->d_name ) == NULL )
			continue;
		memset( & dir_stat , 0x00 , sizeof(struct stat) );
		nret = stat( container_path_base , & dir_stat ) ;
		if( nret == -1 )
			continue;
		if( ! S_ISDIR(dir_stat.st_mode) )
			continue;
		
		/* image */
		memset( image , 0x00 , sizeof(image) );
		memset( container_image_file , 0x00 , sizeof(container_image_file) );
		nret = ReadFileLine( image , sizeof(image) , container_image_file , sizeof(container_image_file) , "%s/%s/image" , cocker_env->containers_path_base , dirent->d_name ) ;
		if( nret )
		{
			memset( container_image_file , 0x00 , sizeof(container_image_file) );
			memset( image , 0x00 , sizeof(image) );
		}
		TrimEnter( image );
		
		/* hostname */
		memset( hostname , 0x00 , sizeof(hostname) );
		memset( container_hostname_file , 0x00 , sizeof(container_hostname_file) );
		nret = ReadFileLine( hostname , sizeof(hostname) , container_hostname_file , sizeof(container_hostname_file) , "%s/%s/hostname" , cocker_env->containers_path_base , dirent->d_name ) ;
		if( nret )
		{
			memset( container_hostname_file , 0x00 , sizeof(container_hostname_file) );
			memset( hostname , 0x00 , sizeof(hostname) );
		}
		TrimEnter( hostname );
		
		/* net */
		memset( net , 0x00 , sizeof(net) );
		memset( container_net_file , 0x00 , sizeof(container_net_file) );
		nret = ReadFileLine( net , sizeof(net) , container_net_file , sizeof(container_net_file) , "%s/%s/net" , cocker_env->containers_path_base , dirent->d_name ) ;
		if( nret )
		{
			memset( container_net_file , 0x00 , sizeof(container_net_file) );
			memset( net , 0x00 , sizeof(net) );
		}
		TrimEnter( net );
		
		/* netns */
		memset( netns , 0x00 , sizeof(netns) );
		memset( container_netns_file , 0x00 , sizeof(container_netns_file) );
		nret = ReadFileLine( netns , sizeof(netns) , container_netns_file , sizeof(container_netns_file) , "%s/%s/netns" , cocker_env->containers_path_base , dirent->d_name ) ;
		if( nret )
		{
			memset( container_netns_file , 0x00 , sizeof(container_netns_file) );
			memset( netns , 0x00 , sizeof(netns) );
		}
		TrimEnter( netns );
		
		/* size */
		Snprintf( container_rwlayer_path , sizeof(container_rwlayer_path) , "%s/%s/rwlayer" , cocker_env->containers_path_base , dirent->d_name );
		
		memset( file_buffer , 0x00 , sizeof(file_buffer) );
		memset( container_size_file , 0x00 , sizeof(container_size_file) );
		nret = ReadFileLine( file_buffer , sizeof(file_buffer) , container_size_file , sizeof(container_size_file) , "%s/%s/size" , cocker_env->containers_path_base , dirent->d_name ) ;
		if( nret )
		{
			nret = GetDirectorySize( container_rwlayer_path , & container_size ) ;
			if( nret )
			{
				container_size = -1 ;
			}
			else
			{
				Snprintf( file_buffer , sizeof(file_buffer) , "%d" , container_size );
				WriteFileLine( file_buffer , container_size_file , sizeof(container_size_file) , "%s/%s/size" , cocker_env->containers_path_base , dirent->d_name );
			}
		}
		else
		{
			stat( container_size_file , & container_size_file_stat );
			nret = IsDirectoryNewThan( container_rwlayer_path , container_size_file_stat.st_mtime ) ;
			if( nret < 0 )
			{
				container_size = -1 ;
			}
			else if( nret > 0 )
			{
				nret = GetDirectorySize( container_rwlayer_path , & container_size ) ;
				if( nret )
				{
					container_size = -1 ;
				}
				else
				{
					Snprintf( file_buffer , sizeof(file_buffer) , "%d" , container_size );
					WriteFileLine( file_buffer , container_size_file , sizeof(container_size_file) , "%s/%s/size" , cocker_env->containers_path_base , dirent->d_name );
				}
			}
			else
			{
				container_size = atoi(file_buffer) ;
			}
		}
		
		if( container_size > 1024*1024*1024 )
			Snprintf( container_size_str , sizeof(container_size_str) , "%d GB" , container_size / (1024*1024*1024) );
		else if( container_size > 1024*1024 )
			Snprintf( container_size_str , sizeof(container_size_str) , "%d MB" , container_size / (1024*1024) );
		else if( container_size > 1024 )
			Snprintf( container_size_str , sizeof(container_size_str) , "%d KB" , container_size / 1024 );
		else if( container_size >= 0 )
			Snprintf( container_size_str , sizeof(container_size_str) , "%d B" , container_size );
		else
			Snprintf( container_size_str , sizeof(container_size_str) , "(unknow)" );
		
		/* pid */
		memset( container_pid_file , 0x00 , sizeof(container_pid_file) );
		memset( pid_str , 0x00 , sizeof(pid_str) );
		nret = ReadFileLine( pid_str , sizeof(pid_str) , container_pid_file , sizeof(container_pid_file) , "%s/%s/pid" , cocker_env->containers_path_base , dirent->d_name ) ;
		if( nret )
		{
			memset( pid_str , 0x00 , sizeof(pid_str) );
			memset( container_pid_file , 0x00 , sizeof(container_pid_file) );
			if( nret > 0 )
			{
				Snprintf( status , sizeof(status) , "STOPED" );
			}
			else
			{
				Snprintf( status , sizeof(status) , "EXPECTION" );
			}
		}
		else
		{
			TrimEnter( pid_str );
			pid = atoi(pid_str) ;
			nret = kill( pid , 0 ) ;
			if( nret == -1 )
			{
				Snprintf( status , sizeof(status) , "EXPECTION(%s)" , pid_str );
			}
			else
			{
				Snprintf( status , sizeof(status) , "RUNNING(%s)" , pid_str );
			}
		}
		
		/* output */
		if( count == 0 )
		{
			printf( "%-20s %-20s %-10s %-10s %-16s %-10s %s\n" , "container_id" , "image" , "hostname" , "net" , "netns" , "size" , "status" );
			printf( "-----------------------------------------------------------------------------------------------------------\n" );
		}
		
		printf( "%-20s %-20s %-10s %-10s %-16s %-10s %s\n" , dirent->d_name , image , hostname , net , netns , container_size_str , status );
		
		count++;
	}
	
	return 0;
}

