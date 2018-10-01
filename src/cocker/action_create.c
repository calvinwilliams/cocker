#include "cocker_in.h"

int DoAction_create( struct CockerEnvironment *env )
{
	char		cmd[ 4096 ] ;
	
	char		pid_str[ PID_LEN_MAX + 1 ] ;
	
	char		container_rwlayer_path[ PATH_MAX ] ;
	char		container_rwlayer_etc_path[ PATH_MAX ] ;
	char		container_merged_path[ PATH_MAX ] ;
	char		container_workdir_path[ PATH_MAX ] ;
	char		container_images_file[ PATH_MAX ] ;
	char		container_hostname_file[ PATH_MAX ] ;
	char		container_rwlayer_net_file[ PATH_MAX ] ;
	char		container_rwlayer_netns_file[ PATH_MAX ] ;
	char		container_vip_file[ PATH_MAX ] ;
	char		container_port_mapping_file[ PATH_MAX ] ;
	
	int		nret = 0 ;
	
	/* preprocess input parameters */
	if( env->image_id[0] )
	{
		nret = SnprintfAndCheckDir( NULL , -1 , "%s/%s/rlayer" , env->images_path_base , env->image_id ) ;
		INTPR1( "*** ERROR : image[%s] not found\n" , env->image_id )
	}
	
	if( env->cmd_para.__container_id == NULL )
	{
		memset( env->container_id , 0x00 , sizeof(env->container_id) );
		GenerateContainerId( env->cmd_para.__image_id , env->container_id );
	}
	
	if( env->cmd_para.__host_name == NULL )
	{
		env->cmd_para.__host_name = env->container_id ;
	}
	
	/* read pid file */
	nret = ReadFileLine( pid_str , sizeof(pid_str)-1 , NULL , -1 , "%s/%s/pid" , env->containers_path_base , env->container_id ) ;
	IxTPR1( (nret==0) , "*** ERROR : container is already running\n" )
	TrimEnter( pid_str );
	
	/* create container folders and files */
	nret = SnprintfAndMakeDir( env->container_path_base , sizeof(env->container_path_base)-1 , "%s/%s" , env->containers_path_base , env->container_id ) ;
	INTPR1( "*** ERROR : SnprintfAndMakeDir / failed[%d] , errno[%d]\n" , nret , errno )
	EIDTP( "mkdir %s ok\n" , env->container_path_base )
	
	nret = SnprintfAndMakeDir( container_rwlayer_path , sizeof(container_rwlayer_path) , "%s/rwlayer" , env->container_path_base ) ;
	INTPR1( "*** ERROR : SnprintfAndMakeDir rwlayer failed[%d] , errno[%d]\n" , nret , errno )
	EIDTP( "mkdir %s ok\n" , container_rwlayer_path )
	
	nret = SnprintfAndMakeDir( container_rwlayer_etc_path , sizeof(container_rwlayer_etc_path) , "%s/rwlayer/etc" , env->container_path_base ) ;
	INTPR1( "*** ERROR : SnprintfAndMakeDir /etc failed[%d] , errno[%d]\n" , nret , errno )
	EIDTP( "mkdir %s ok\n" , container_rwlayer_etc_path )
	
	nret = SnprintfAndMakeDir( container_merged_path , sizeof(container_merged_path) , "%s/merged" , env->container_path_base ) ;
	INTPR1( "*** ERROR : SnprintfAndMakeDir merged failed[%d] , errno[%d]\n" , nret , errno )
	EIDTP( "mkdir %s ok\n" , container_merged_path )
	
	nret = SnprintfAndMakeDir( container_workdir_path , sizeof(container_workdir_path) , "%s/workdir" , env->container_path_base ) ;
	INTPR1( "*** ERROR : SnprintfAndMakeDir workdir failed[%d] , errno[%d]\n" , nret , errno )
	EIDTP( "mkdir %s ok\n" , container_workdir_path )
	
	if( env->image_id[0] )
	{
		nret = WriteFileLine( env->image_id , container_images_file , sizeof(container_images_file) , "%s/images" , env->container_path_base ) ;
		INTPR1( "*** ERROR : WriteFileLine images failed[%d] , errno[%d]\n" , nret , errno )
		EIDTP( "write file %s ok\n" , container_images_file )
	}
	else
	{
		nret = WriteFileLine( "" , container_images_file , sizeof(container_images_file) , "%s/images" , env->container_path_base ) ;
		INTPR1( "*** ERROR : WriteFileLine images failed[%d] , errno[%d]\n" , nret , errno )
		EIDTP( "write file %s ok\n" , container_images_file )
	}
	
	nret = WriteFileLine( env->cmd_para.__host_name , container_hostname_file , sizeof(container_hostname_file) , "%s/hostname" , env->container_path_base ) ;
	INTPR1( "*** ERROR : WriteFileLine hostname failed[%d] , errno[%d]\n" , nret , errno )
	EIDTP( "write file %s ok\n" , container_hostname_file )
	
	/* create network-namespace */
	nret = WriteFileLine( env->net , container_rwlayer_net_file , sizeof(container_rwlayer_net_file) , "%s/net" , env->container_path_base ) ;
	INTPR1( "*** ERROR : WriteFileLine vip failed[%d] , errno[%d]\n" , nret , errno )
	EIDTP( "write file [%s] ok\n" , container_rwlayer_net_file )
	
	if( STRCMP( env->net , == , "BRIDGE" ) )
	{
		nret = WriteFileLine( env->netns_name , container_rwlayer_netns_file , sizeof(container_rwlayer_netns_file) , "%s/netns" , env->container_path_base ) ;
		INTPR1( "*** ERROR : WriteFileLine netns failed[%d] , errno[%d]\n" , nret , errno )
		EIDTP( "write file [%s] ok\n" , container_rwlayer_netns_file )
		
		nret = WriteFileLine( env->vip , container_vip_file , sizeof(container_vip_file) , "%s/vip" , env->container_path_base ) ;
		INTPR1( "*** ERROR : WriteFileLine vip failed[%d] , errno[%d]\n" , nret , errno )
		EIDTP( "write file [%s] ok\n" , container_vip_file )
		
		if( env->port_mapping[0] )
		{
			nret = WriteFileLine( env->port_mapping , container_port_mapping_file , sizeof(container_port_mapping_file) , "%s/port_mapping" , env->container_path_base ) ;
			INTPR1( "*** ERROR : WriteFileLine port_mapping failed[%d] , errno[%d]\n" , nret , errno )
			EIDTP( "write file [%s] ok\n" , container_port_mapping_file )
		}
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns add %s" , env->netns_name ) ;
		INTPR1( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
		EIDTP( "system [%s] ok\n" , cmd )
	}
	else if( STRCMP( env->net , == , "CUSTOM" ) )
	{
		nret = WriteFileLine( env->netns_name , container_rwlayer_netns_file , sizeof(container_rwlayer_netns_file) , "%s/netns" , env->container_path_base ) ;
		INTPR1( "*** ERROR : WriteFileLine netns failed[%d] , errno[%d]\n" , nret , errno )
		EIDTP( "write file [%s] ok\n" , container_rwlayer_netns_file )
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns add %s" , env->netns_name ) ;
		INTPR1( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
		EIDTP( "system [%s] ok\n" , cmd )
	}
	
	printf( "OK\n" );
	
	return 0;
}

