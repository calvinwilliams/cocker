#include "cocker_in.h"

int DoShow_containers( struct CockerEnvironment *cocker_env )
{
	DIR		*dir = NULL ;
	struct dirent	*dirent = NULL ;
	int		count ;
	
	dir = opendir( cocker_env->containers_path_base ) ;
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
		
		if( count == 0 )
		{
			printf( "container\n" );
			printf( "--------------\n" );
		}
		
		printf( "%s\n" , dirent->d_name );
		
		count++;
	}
	
	return 0;
}

