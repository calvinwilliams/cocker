#include "cocker_in.h"

int DoAction_to_image( struct CockerEnvironment *env )
{
	char		pid_str[ PID_LEN_MAX + 1 ] ;
	pid_t		pid ;
	char		net[ NET_LEN_MAX + 1 ] ;
	char		image_rlayer_path_base[ PATH_MAX + 1 ] ;
	char		container_pid_file[ PATH_MAX + 1 ] ;
	char		container_net_file[ PATH_MAX + 1 ] ;
	char		container_author_file[ PATH_MAX + 1 ] ;
	char		container_create_datetime_file[ PATH_MAX + 1 ] ;
	char		container_version_file[ PATH_MAX + 1 ] ;
	char		cmd[ 4096 ] ;

	int		nret = 0 ;
	
	/* preprocess input parameters */
	Snprintf( env->container_path_base , sizeof(env->container_path_base)-1 , "%s/%s" , env->containers_path_base , env->cmd_para.__from_container );
	nret = access( env->container_path_base , F_OK ) ;
	I1TER1( "*** ERROR : container '%s' not found\n" , env->cmd_para.__from_container )
	
	Snprintf( env->image_path_base , sizeof(env->image_path_base)-1 , "%s/%s" , env->images_path_base , env->cmd_para.__to_image );
	nret = access( env->image_path_base , F_OK ) ;
	I0TER1( "*** ERROR : image '%s' exist\n" , env->cmd_para.__to_image )
	
	GetEthernetNames( env , env->cmd_para.__from_container );
	
	/* read pid file */
	nret = ReadFileLine( pid_str , sizeof(pid_str)-1 , container_pid_file , sizeof(container_pid_file) , "%s/%s/pid" , env->containers_path_base , env->cmd_para.__from_container ) ;
	if( nret == 0 )
	{
		pid = atoi(pid_str) ;
		if( pid > 0 )
		{
			nret = kill( pid , 0 ) ;
			I0TER1( "*** ERROR : container is already running\n" )
		}
	}
	
	/* read net file */
	nret = ReadFileLine( net , sizeof(net) , container_net_file , sizeof(container_net_file) , "%s/net" , env->container_path_base ) ;
	ILTER1( "*** ERROR : ReadFileLine net failed\n" )
	EIDTI( "read file net ok\n" )
	
	TrimEnter( net );
	
	/* destroy network-namespace */
	if( STRCMP( net , == , "BRIDGE" ) || env->cmd_para.__forcely )
	{
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns del %s" , env->netns_name ) ;
		INTEFR1( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
		EIDTI( "system [%s] ok\n" , cmd )
	}
	else if( STRCMP( net , == , "CUSTOM" ) )
	{
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ip netns del %s" , env->netns_name ) ;
		INTEFR1( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
		EIDTI( "system [%s] ok\n" , cmd )
	}
	
	/* create image */
	nret = SnprintfAndMakeDir( env->image_path_base , sizeof(env->image_path_base)-1 , "%s/%s" , env->images_path_base , env->cmd_para.__to_image ) ;
	INTER1( "*** ERROR : SnprintfAndMakeDir image_path_base failed[%d] , errno[%d]\n" , nret , errno )
	EIDTI( "mkdir %s ok\n" , env->image_path_base )
	
	nret = SnprintfAndMakeDir( image_rlayer_path_base , sizeof(image_rlayer_path_base)-1 , "%s/rlayer" , env->image_path_base ) ;
	INTER1( "*** ERROR : SnprintfAndMakeDir image_rlayer_path_base failed[%d] , errno[%d]\n" , nret , errno )
	EIDTI( "mkdir %s ok\n" , image_rlayer_path_base )
	
	nret = SnprintfAndSystem( cmd , sizeof(cmd) , "mv -f %s/rwlayer/* %s/rlayer/" , env->container_path_base , env->image_path_base ) ;
	INTER1( "*** ERROR : SnprintfAndSystem [mv -f %s/rwlayer/* %s/rlayer/] failed[%d] , errno[%d]\n" , env->container_path_base , env->image_path_base , nret , errno )
	EIDTI( "system [%s] ok\n" , cmd )
	
	Snprintf( container_author_file , sizeof(container_author_file) , "%s/author" , env->container_path_base );
	nret = access( container_author_file , F_OK ) ;
	if( nret == 0 )
	{
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "mv %s %s/" , container_author_file , env->image_path_base ) ;
		INTER1( "*** ERROR : SnprintfAndSystem [mv %s %s/] failed[%d] , errno[%d]\n" , container_author_file , env->image_path_base , nret , errno )
		EIDTE( "system [%s] ok\n" , cmd )
	}
	
	Snprintf( container_create_datetime_file , sizeof(container_create_datetime_file) , "%s/create_datetime" , env->container_path_base );
	nret = access( container_create_datetime_file , F_OK ) ;
	if( nret == 0 )
	{
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "mv %s %s/" , container_create_datetime_file , env->image_path_base ) ;
		INTER1( "*** ERROR : SnprintfAndSystem [mv %s %s/] failed[%d] , errno[%d]\n" , container_create_datetime_file , env->image_path_base , nret , errno )
		EIDTE( "system [%s] ok\n" , cmd )
	}
	
	Snprintf( container_version_file , sizeof(container_version_file) , "%s/version" , env->container_path_base );
	nret = access( container_version_file , F_OK ) ;
	if( nret == 0 )
	{
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "mv %s %s/" , container_version_file , env->image_path_base ) ;
		INTER1( "*** ERROR : SnprintfAndSystem [mv %s %s/] failed[%d] , errno[%d]\n" , container_version_file , env->image_path_base , nret , errno )
		EIDTE( "system [%s] ok\n" , cmd )
	}
	
	nret = SnprintfAndSystem( cmd , sizeof(cmd) , "rm -rf %s" , env->container_path_base ) ;
	INTER1( "*** ERROR : SnprintfAndSystem [rm -rf %s] failed[%d] , errno[%d]\n" , env->container_path_base , nret , errno )
	EIDTI( "system [%s] ok\n" , cmd )
	
	printf( "OK\n" );
	
	return 0;
}

