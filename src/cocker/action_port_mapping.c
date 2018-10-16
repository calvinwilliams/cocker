#include "cocker_in.h"

int DoAction_port_mapping( struct CockerEnvironment *env )
{
	char		container_port_mapping_file[ PATH_MAX + 1 ] ;
	
	int		nret = 0 ;
	
	/* preprocess input parameters */
	Snprintf( env->container_path_base , sizeof(env->container_path_base)-1 , "%s/%s" , env->containers_path_base , env->cmd_para.__container_id );
	nret = access( env->container_path_base , F_OK ) ;
	I1TER1( "*** ERROR : container '%s' not found\n" , env->cmd_para.__container_id )
	
	/* modify port mapping */
	nret = WriteFileLine( env->cmd_para.__port_mapping , container_port_mapping_file , sizeof(container_port_mapping_file) , "%s/port_mapping" , env->container_path_base ) ;
	INTER1( "*** ERROR : WriteFileLine port_mapping failed[%d] , errno[%d]\n" , nret , errno )
	EIDTE( "write file [%s] ok\n" , container_port_mapping_file )
	
	return 0;	
}

