#include "cocker_in.h"

int CreateContainer( struct CockerEnvironment *env , char *__image_id , char *__container_id )
{
	char			container_id[ CONTAINER_ID_LEN_MAX + 1 ] ;
	
	char			cmd[ 4096 ] ;
	
	char			pid_str[ PID_LEN_MAX + 1 ] ;
	pid_t			pid ;
	struct CockerVolume	*volume = NULL ;
	FILE			*container_volume_fp = NULL ;
	
	char			container_rwlayer_path[ PATH_MAX + 1 ] ;
	char			container_rwlayer_etc_path[ PATH_MAX + 1 ] ;
	char			container_merged_path[ PATH_MAX + 1 ] ;
	char			container_workdir_path[ PATH_MAX + 1 ] ;
	char			container_image_file[ PATH_MAX + 1 ] ;
	char			container_hostname_file[ PATH_MAX + 1 ] ;
	char			container_volume_file[ PATH_MAX + 1 ] ;
	char			container_rwlayer_net_file[ PATH_MAX + 1 ] ;
	char			container_rwlayer_netns_file[ PATH_MAX + 1 ] ;
	char			container_vip_file[ PATH_MAX + 1 ] ;
	char			container_port_mapping_file[ PATH_MAX + 1 ] ;
	
	int			nret = 0 ;
	
	/* preprocess input parameters */
	if( __image_id && __image_id[0] )
	{
		char	image_id[ IMAGES_ID_LEN_MAX + 1 ] ;
		char	*p = NULL ;
		
		memset( image_id , 0x00 , sizeof(image_id) );
		strncpy( image_id , __image_id , sizeof(image_id)-1 );
		p = strtok( image_id , ":" ) ;
		while( p )
		{
			nret = SnprintfAndCheckDir( NULL , -1 , "%s/%s/rlayer" , env->images_path_base , p ) ;
			INTER1( "*** ERROR : image[%s] not found\n" , p )
			
			p = strtok( NULL , ":" ) ;
		}
	}
	
	if( __container_id == NULL )
	{
		memset( container_id , 0x00 , sizeof(container_id) );
		GenerateContainerId( __image_id , container_id );
		__container_id = container_id ;
	}
	
	GetEthernetNames( env , __container_id );
	
	/* read pid file */
	nret = ReadFileLine( pid_str , sizeof(pid_str)-1 , NULL , -1 , "%s/%s/pid" , env->containers_path_base , __container_id ) ;
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
	nret = SnprintfAndMakeDir( env->container_path_base , sizeof(env->container_path_base)-1 , "%s/%s" , env->containers_path_base , __container_id ) ;
	INTER1( "*** ERROR : SnprintfAndMakeDir / failed[%d] , errno[%d]\n" , nret , errno )
	EIDTI( "mkdir %s ok\n" , env->container_path_base )
	
	nret = SnprintfAndMakeDir( container_rwlayer_path , sizeof(container_rwlayer_path) , "%s/rwlayer" , env->container_path_base ) ;
	INTER1( "*** ERROR : SnprintfAndMakeDir rwlayer failed[%d] , errno[%d]\n" , nret , errno )
	EIDTI( "mkdir %s ok\n" , container_rwlayer_path )
	
	nret = SnprintfAndMakeDir( container_rwlayer_etc_path , sizeof(container_rwlayer_etc_path) , "%s/rwlayer/etc" , env->container_path_base ) ;
	INTER1( "*** ERROR : SnprintfAndMakeDir /etc failed[%d] , errno[%d]\n" , nret , errno )
	EIDTI( "mkdir %s ok\n" , container_rwlayer_etc_path )
	
	nret = SnprintfAndMakeDir( container_merged_path , sizeof(container_merged_path) , "%s/merged" , env->container_path_base ) ;
	INTER1( "*** ERROR : SnprintfAndMakeDir merged failed[%d] , errno[%d]\n" , nret , errno )
	EIDTI( "mkdir %s ok\n" , container_merged_path )
	
	nret = SnprintfAndMakeDir( container_workdir_path , sizeof(container_workdir_path) , "%s/workdir" , env->container_path_base ) ;
	INTER1( "*** ERROR : SnprintfAndMakeDir workdir failed[%d] , errno[%d]\n" , nret , errno )
	EIDTI( "mkdir %s ok\n" , container_workdir_path )
	
	if( __image_id && __image_id[0] )
	{
		nret = WriteFileLine( __image_id , container_image_file , sizeof(container_image_file) , "%s/image" , env->container_path_base ) ;
		INTER1( "*** ERROR : WriteFileLine image failed[%d] , errno[%d]\n" , nret , errno )
		EIDTI( "write file %s ok\n" , container_image_file )
	}
	/*
	else
	{
		nret = WriteFileLine( "" , container_image_file , sizeof(container_image_file) , "%s/images" , env->container_path_base ) ;
		INTER1( "*** ERROR : WriteFileLine image failed[%d] , errno[%d]\n" , nret , errno )
		EIDTI( "write file %s ok\n" , container_image_file )
	}
	*/
	
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
	
	if( env->cmd_para.__host_name == NULL )
		env->cmd_para.__host_name = __container_id ;
	
	nret = WriteFileLine( env->cmd_para.__host_name , container_hostname_file , sizeof(container_hostname_file) , "%s/hostname" , env->container_path_base ) ;
	INTER1( "*** ERROR : WriteFileLine hostname failed[%d] , errno[%d]\n" , nret , errno )
	EIDTI( "write file %s ok\n" , container_hostname_file )
	
	/* create network-namespace */
	nret = WriteFileLine( env->cmd_para.__net , container_rwlayer_net_file , sizeof(container_rwlayer_net_file) , "%s/net" , env->container_path_base ) ;
	INTER1( "*** ERROR : WriteFileLine vip failed[%d] , errno[%d]\n" , nret , errno )
	EIDTI( "write file [%s] ok\n" , container_rwlayer_net_file )
	
	if( STRCMP( env->cmd_para.__net , == , "BRIDGE" ) )
	{
		nret = WriteFileLine( env->netns_name , container_rwlayer_netns_file , sizeof(container_rwlayer_netns_file) , "%s/netns" , env->container_path_base ) ;
		INTER1( "*** ERROR : WriteFileLine netns failed[%d] , errno[%d]\n" , nret , errno )
		EIDTI( "write file [%s] ok\n" , container_rwlayer_netns_file )
		
		nret = WriteFileLine( env->cmd_para.__vip , container_vip_file , sizeof(container_vip_file) , "%s/vip" , env->container_path_base ) ;
		INTER1( "*** ERROR : WriteFileLine vip failed[%d] , errno[%d]\n" , nret , errno )
		EIDTI( "write file [%s] ok\n" , container_vip_file )
		
		if( env->cmd_para.__port_mapping )
		{
			nret = WriteFileLine( env->cmd_para.__port_mapping , container_port_mapping_file , sizeof(container_port_mapping_file) , "%s/port_mapping" , env->container_path_base ) ;
			INTER1( "*** ERROR : WriteFileLine port_mapping failed[%d] , errno[%d]\n" , nret , errno )
			EIDTI( "write file [%s] ok\n" , container_port_mapping_file )
		}
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns add %s" , env->netns_name ) ;
		INTER1( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
		EIDTI( "system [%s] ok\n" , cmd )
	}
	else if( STRCMP( env->cmd_para.__net , == , "CUSTOM" ) )
	{
		nret = WriteFileLine( env->netns_name , container_rwlayer_netns_file , sizeof(container_rwlayer_netns_file) , "%s/netns" , env->container_path_base ) ;
		INTER1( "*** ERROR : WriteFileLine netns failed[%d] , errno[%d]\n" , nret , errno )
		EIDTI( "write file [%s] ok\n" , container_rwlayer_netns_file )
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns add %s" , env->netns_name ) ;
		INTER1( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
		EIDTI( "system [%s] ok\n" , cmd )
	}
	
	return 0;
}

int DoAction_create( struct CockerEnvironment *env )
{
	int		nret = 0 ;
	
	nret = CreateContainer( env , env->cmd_para.__image_id , env->cmd_para.__container_id ) ;
	if( nret == 0 )
	{
		printf( "OK\n" );
	}
	
	return nret;
}
