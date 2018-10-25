#include "cocker_in.h"

int DoAction_volume( struct CockerEnvironment *env )
{
	char			container_volume_file[ PATH_MAX + 1 ] ;
	FILE			*container_volume_fp = NULL ;
	struct CockerVolume	*volume = NULL ;
	
	int			nret = 0 ;
	
	/* preprocess input parameters */
	Snprintf( env->container_path_base , sizeof(env->container_path_base)-1 , "%s/%s" , env->containers_path_base , env->cmd_para.__container_id );
	nret = access( env->container_path_base , F_OK ) ;
	I1TER1( "*** ERROR : container '%s' not found\n" , env->cmd_para.__container_id )
	
	/* modify volume */
	if( ! list_empty( & (env->cmd_para.volume_list) ) )
	{
		Snprintf( container_volume_file , sizeof(container_volume_file) , "%s/volume" , env->container_path_base ) ;
		container_volume_fp = fopen( container_volume_file , "w" ) ;
		IxTER1( (container_volume_fp==NULL) , "*** ERROR : WriteFileLine volume failed[%d] , errno[%d]\n" , nret , errno )
		
		list_for_each_entry( volume , & (env->cmd_para.volume_list) , struct CockerVolume , volume_node )
		{
			fprintf( container_volume_fp , "%.*s:%s\n" , volume->host_path_len , volume->host_path , volume->container_path );
			IDTI( "write file %s [%.*s:%s]\n" , container_volume_file , volume->host_path_len , volume->host_path , volume->container_path )
		}
		
		fclose( container_volume_fp );
	}
	
	printf( "OK\n" );
	
	return 0;	
}

