#include "cocker_in.h"

int CreateContainer( struct CockerEnvironment *env , char *__image_id , char *__container_id )
{
	char		container_id[ CONTAINER_NAME_MAX + 1 ] ;
	
	char		cmd[ 4096 ] ;
	
	char		pid_str[ PID_LEN_MAX + 1 ] ;
	pid_t		pid ;
	
	char		container_rwlayer_path[ PATH_MAX ] ;
	/*
	char		container_rwlayer_etc_path[ PATH_MAX ] ;
	*/
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
	if( __image_id && __image_id[0] )
	{
		nret = SnprintfAndCheckDir( NULL , -1 , "%s/%s/rlayer" , env->images_path_base , __image_id ) ;
		INTER1( "*** ERROR : image[%s] not found\n" , __image_id )
	}
	
	memset( container_id , 0x00 , sizeof(container_id) );
	if( env->cmd_para.__container_id == NULL )
	{
		GenerateContainerId( __image_id , container_id );
	}
	else
	{
		strncpy( container_id , __container_id , sizeof(container_id)-1 );
	}
	
	if( env->cmd_para.__host_name == NULL )
	{
		env->cmd_para.__host_name = container_id ;
	}
	
	/* read pid file */
	nret = ReadFileLine( pid_str , sizeof(pid_str)-1 , NULL , -1 , "%s/%s/pid" , env->containers_path_base , container_id ) ;
	if( nret == 0 )
	{
		pid = atoi(pid_str) ;
		if( pid > 0 )
		{
			nret = kill( pid , 0 ) ;
			I0TER1( "*** ERROR : container is already running\n" )
		}
	}
	
	/* create container folders and files */
	nret = SnprintfAndMakeDir( env->container_path_base , sizeof(env->container_path_base)-1 , "%s/%s" , env->containers_path_base , container_id ) ;
	INTER1( "*** ERROR : SnprintfAndMakeDir / failed[%d] , errno[%d]\n" , nret , errno )
	EIDTE( "mkdir %s ok\n" , env->container_path_base )
	
	nret = SnprintfAndMakeDir( container_rwlayer_path , sizeof(container_rwlayer_path) , "%s/rwlayer" , env->container_path_base ) ;
	INTER1( "*** ERROR : SnprintfAndMakeDir rwlayer failed[%d] , errno[%d]\n" , nret , errno )
	EIDTE( "mkdir %s ok\n" , container_rwlayer_path )
	
	/*
	nret = SnprintfAndMakeDir( container_rwlayer_etc_path , sizeof(container_rwlayer_etc_path) , "%s/rwlayer/etc" , env->container_path_base ) ;
	INTER1( "*** ERROR : SnprintfAndMakeDir /etc failed[%d] , errno[%d]\n" , nret , errno )
	EIDTE( "mkdir %s ok\n" , container_rwlayer_etc_path )
	*/
	
	nret = SnprintfAndMakeDir( container_merged_path , sizeof(container_merged_path) , "%s/merged" , env->container_path_base ) ;
	INTER1( "*** ERROR : SnprintfAndMakeDir merged failed[%d] , errno[%d]\n" , nret , errno )
	EIDTE( "mkdir %s ok\n" , container_merged_path )
	
	nret = SnprintfAndMakeDir( container_workdir_path , sizeof(container_workdir_path) , "%s/workdir" , env->container_path_base ) ;
	INTER1( "*** ERROR : SnprintfAndMakeDir workdir failed[%d] , errno[%d]\n" , nret , errno )
	EIDTE( "mkdir %s ok\n" , container_workdir_path )
	
	if( __image_id && __image_id[0] )
	{
		nret = WriteFileLine( __image_id , container_images_file , sizeof(container_images_file) , "%s/images" , env->container_path_base ) ;
		INTER1( "*** ERROR : WriteFileLine images failed[%d] , errno[%d]\n" , nret , errno )
		EIDTE( "write file %s ok\n" , container_images_file )
	}
	else
	{
		nret = WriteFileLine( "" , container_images_file , sizeof(container_images_file) , "%s/images" , env->container_path_base ) ;
		INTER1( "*** ERROR : WriteFileLine images failed[%d] , errno[%d]\n" , nret , errno )
		EIDTE( "write file %s ok\n" , container_images_file )
	}
	
	nret = WriteFileLine( env->cmd_para.__host_name , container_hostname_file , sizeof(container_hostname_file) , "%s/hostname" , env->container_path_base ) ;
	INTER1( "*** ERROR : WriteFileLine hostname failed[%d] , errno[%d]\n" , nret , errno )
	EIDTE( "write file %s ok\n" , container_hostname_file )
	
	/* create network-namespace */
	nret = WriteFileLine( env->net , container_rwlayer_net_file , sizeof(container_rwlayer_net_file) , "%s/net" , env->container_path_base ) ;
	INTER1( "*** ERROR : WriteFileLine vip failed[%d] , errno[%d]\n" , nret , errno )
	EIDTE( "write file [%s] ok\n" , container_rwlayer_net_file )
	
	if( STRCMP( env->net , == , "BRIDGE" ) )
	{
		nret = WriteFileLine( env->netns_name , container_rwlayer_netns_file , sizeof(container_rwlayer_netns_file) , "%s/netns" , env->container_path_base ) ;
		INTER1( "*** ERROR : WriteFileLine netns failed[%d] , errno[%d]\n" , nret , errno )
		EIDTE( "write file [%s] ok\n" , container_rwlayer_netns_file )
		
		nret = WriteFileLine( env->vip , container_vip_file , sizeof(container_vip_file) , "%s/vip" , env->container_path_base ) ;
		INTER1( "*** ERROR : WriteFileLine vip failed[%d] , errno[%d]\n" , nret , errno )
		EIDTE( "write file [%s] ok\n" , container_vip_file )
		
		if( env->port_mapping[0] )
		{
			nret = WriteFileLine( env->port_mapping , container_port_mapping_file , sizeof(container_port_mapping_file) , "%s/port_mapping" , env->container_path_base ) ;
			INTER1( "*** ERROR : WriteFileLine port_mapping failed[%d] , errno[%d]\n" , nret , errno )
			EIDTE( "write file [%s] ok\n" , container_port_mapping_file )
		}
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns add %s" , env->netns_name ) ;
		INTER1( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
		EIDTE( "system [%s] ok\n" , cmd )
	}
	else if( STRCMP( env->net , == , "CUSTOM" ) )
	{
		nret = WriteFileLine( env->netns_name , container_rwlayer_netns_file , sizeof(container_rwlayer_netns_file) , "%s/netns" , env->container_path_base ) ;
		INTER1( "*** ERROR : WriteFileLine netns failed[%d] , errno[%d]\n" , nret , errno )
		EIDTE( "write file [%s] ok\n" , container_rwlayer_netns_file )
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns add %s" , env->netns_name ) ;
		INTER1( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
		EIDTE( "system [%s] ok\n" , cmd )
	}
	
	printf( "OK\n" );
	
	return 0;
}

int DoAction_create( struct CockerEnvironment *env )
{
	return CreateContainer( env , env->image_id , env->container_id );
}
