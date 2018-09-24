#include "vhm_in.h"

int VhmShow_vtemplates( struct VhmEnvironment *vhm_env )
{
	DIR		*dir = NULL ;
	struct dirent	*dirent = NULL ;
	int		count ;
	
	dir = opendir( vhm_env->vtemplates_path_base ) ;
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
			printf( "vtemplate_name\n" );
			printf( "--------------\n" );
		}
		
		printf( "%s\n" , dirent->d_name );
		
		count++;
	}
	
	return 0;
}

