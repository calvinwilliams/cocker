#include "cocker_in.h"

int DoAction_vip( struct CockerEnvironment *cocker_env )
{
	char		cmd[ 4096 ] ;
	
	char		container_vip_file[ PATH_MAX + 1 ] ;
	
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
	
	/*
	nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns exec %s ifconfig %s up" , cocker_env->netns_name , cocker_env->veth_sname ) ;
	if( nret )
	{
		printf( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
		return -1;
	}
	else if( cocker_env->cmd_para.__debug )
	{
		printf( "system [%s] ok\n" , cmd );
	}
	*/
	
	/* modify vip */
	nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns exec %s ifconfig %s %s" , cocker_env->netns_name , cocker_env->veth_sname , cocker_env->vip ) ;
	if( nret )
	{
		printf( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
		return -1;
	}
	else if( cocker_env->cmd_para.__debug )
	{
		printf( "system [%s] ok\n" , cmd );
	}
	
	nret = WriteFileLine( cocker_env->vip , container_vip_file , sizeof(container_vip_file) , "%s/vip" , cocker_env->container_path_base ) ;
	if( nret )
	{
		printf( "*** ERROR : WriteFileLine vip failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	else if( cocker_env->cmd_para.__debug )
	{
		printf( "write file [%s] ok\n" , container_vip_file );
	}
	
	return 0;	
}

