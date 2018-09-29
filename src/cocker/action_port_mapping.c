#include "cocker_in.h"

int DoAction_port_mapping( struct CockerEnvironment *cocker_env )
{
	char		container_port_mapping_file[ PATH_MAX + 1 ] ;
	
	int		nret = 0 ;
	
	/* preprocess input parameters */
	Snprintf( cocker_env->container_path_base , sizeof(cocker_env->container_path_base)-1 , "%s/%s" , cocker_env->containers_path_base , cocker_env->cmd_para.__container_id );
	nret = access( cocker_env->container_path_base , F_OK ) ;
	if( nret == -1 )
	{
		printf( "*** ERROR : container '%s' not found\n" , cocker_env->cmd_para.__container_id );
		return -1;
	}
	
	GetEthernetName( cocker_env->container_id , cocker_env );
	
	/* modify port mapping */
	nret = WriteFileLine( cocker_env->port_mapping , container_port_mapping_file , sizeof(container_port_mapping_file) , "%s/port_mapping" , cocker_env->container_path_base ) ;
	if( nret )
	{
		printf( "*** ERROR : WriteFileLine port_mapping failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	else if( cocker_env->cmd_para.__debug )
	{
		printf( "write file [%s] ok\n" , container_port_mapping_file );
	}
	
	return 0;	
}

