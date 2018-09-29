#include "cocker_in.h"

static void usage()
{
	printf( "USAGE : cocker -s images\n" );
	printf( "               -s containers\n" );
	printf( "               -a create --image-id (id) [ --container-id (id) ] [ --host-name (name) ] [ --net (BRIDGE|HOST|CUSTOM) ] [ --host-if-name (eth) ] [ --vip (ip) ] [ --port-mapping (src_port:dst_port) ]\n" );
	printf( "               -a start --container-id (id) [ --attach ]\n" );
	printf( "               -a stop --container-id (id) [ --forcely ]\n" );
	printf( "               -a destroy --container-id (id) [ --forcely ]\n" );
	printf( "               -a vip --container-id (id) --vip (ip)\n" );
	printf( "               -a port_mapping --container-id (id) --port-mapping (src_port:dst_port)\n" );
	printf( "               -a install_test\n" );
	printf( "                    --debug\n" );
	return;
}

static int ParseCommandParameters( struct CockerEnvironment *cocker_env , int argc , char *argv[] )
{
	int		i ;
	
	int		nret = 0 ;
	
	for( i = 1 ; i < argc ; i++ )
	{
		if( STRCMP( argv[i] , == , "-v" ) )
		{
			printf( "cocker v%s build %s %s\n" , _COCKER_VERSION , __DATE__ , __TIME__ );
			DestroyCockerEnvironment( & cocker_env );
			exit(0);
		}
		else if( STRCMP( argv[i] , == , "-s" ) && i + 1 < argc )
		{
			cocker_env->cmd_para._show = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "-a" ) && i + 1 < argc )
		{
			cocker_env->cmd_para._action = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "--image-id" ) && i + 1 < argc )
		{
			cocker_env->cmd_para.__image_id = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "--container-id" ) && i + 1 < argc )
		{
			cocker_env->cmd_para.__container_id = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "--host-name" ) && i + 1 < argc )
		{
			cocker_env->cmd_para.__host_name = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "--net" ) && i + 1 < argc )
		{
			cocker_env->cmd_para.__net = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "--host-eth" ) && i + 1 < argc )
		{
			cocker_env->cmd_para.__host_eth = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "--vip" ) && i + 1 < argc )
		{
			cocker_env->cmd_para.__vip = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "--port-mapping" ) && i + 1 < argc )
		{
			cocker_env->cmd_para.__port_mapping = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "--attach" ) )
		{
			cocker_env->cmd_para.__attach = argv[i] ;
		}
		else if( STRCMP( argv[i] , == , "--debug" ) )
		{
			cocker_env->cmd_para.__debug = argv[i] ;
		}
		else if( STRCMP( argv[i] , == , "--forcely" ) )
		{
			cocker_env->cmd_para.__forcely = argv[i] ;
		}
		else
		{
			printf( "*** ERROR : invalid parameter '%s'\n" , argv[i] );
			usage();
			return -7;
		}
	}
	
	memset( cocker_env->net , 0x00 , sizeof(cocker_env->net) );
	if( cocker_env->cmd_para.__image_id )
	{
		strncpy( cocker_env->image_id , cocker_env->cmd_para.__image_id , sizeof(cocker_env->image_id)-1 );
	}
	
	memset( cocker_env->net , 0x00 , sizeof(cocker_env->net) );
	if( cocker_env->cmd_para.__container_id )
	{
		strncpy( cocker_env->container_id , cocker_env->cmd_para.__container_id , sizeof(cocker_env->container_id)-1 );
	}
	
	if( cocker_env->cmd_para.__net == NULL )
		cocker_env->cmd_para.__net = "BRIDGE" ;
	
	if( ! ( STRCMP( cocker_env->cmd_para.__net , == , "BRIDGE" ) || STRCMP( cocker_env->cmd_para.__net , == , "HOST" ) || STRCMP( cocker_env->cmd_para.__net , == , "CUSTOM" ) ) )
	{
		printf( "*** ERROR : '--net' value[%s] invalid\n" , cocker_env->cmd_para.__net );
		return -7;
	}
	
	memset( cocker_env->net , 0x00 , sizeof(cocker_env->net) );
	if( cocker_env->cmd_para.__net )
	{
		strncpy( cocker_env->net , cocker_env->cmd_para.__net , sizeof(cocker_env->net)-1 );
	}
	
	if( cocker_env->cmd_para.__host_eth )
	{
		memset( cocker_env->host_eth , 0x00 , sizeof(cocker_env->host_eth) );
		strncpy( cocker_env->host_eth , cocker_env->cmd_para.__host_eth , sizeof(cocker_env->host_eth)-1 );
	}
	else
	{
		struct ifaddrs		*ifa_base = NULL ;
		struct ifaddrs		*ifa = NULL ;
		
		nret = getifaddrs( & ifa_base ) ;
		if( nret == -1 )
		{
			printf( "*** ERROR : getifaddrs failed[%d] , errno[%d]\n" , nret , errno );
			return -1;
		}
		
		memset( cocker_env->host_eth , 0x00 , sizeof(cocker_env->host_eth) );
		ifa = ifa_base ;
		while( ifa )
		{
			if( ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET )
			{
				if( STRCMP( ifa->ifa_name , != , "lo" ) && STRCMP( ifa->ifa_name , != , "cocker0" ) && STRNCMP( ifa->ifa_name , != , "veth" , 4 ) )
				{
					strncpy( cocker_env->host_eth , ifa->ifa_name , sizeof(cocker_env->host_eth)-1 );
					break;
				}
			}
			
			ifa = ifa->ifa_next ;
		}
		
		freeifaddrs( ifa_base );
		
		if( cocker_env->host_eth[0] == '\0' )
		{
			printf( "*** ERROR : host if name not found\n" );
			return -1;
		}
	}
	
	memset( cocker_env->vip , 0x00 , sizeof(cocker_env->vip) );
	if( cocker_env->cmd_para.__vip )
	{
		strncpy( cocker_env->vip , cocker_env->cmd_para.__vip , sizeof(cocker_env->vip)-1 );
	}
	
	memset( cocker_env->port_mapping , 0x00 , sizeof(cocker_env->port_mapping) );
	if( cocker_env->cmd_para.__port_mapping )
	{
		strncpy( cocker_env->port_mapping , cocker_env->cmd_para.__port_mapping , sizeof(cocker_env->port_mapping)-1 );
	}
	
	return 0;
}

static int ExecuteCommandParameters( struct CockerEnvironment *cocker_env )
{
	if( cocker_env->cmd_para._show )
	{
		if( STRCMP( cocker_env->cmd_para._show , == , "images" ) )
		{
			return -DoShow_images( cocker_env );
		}
		else if( STRCMP( cocker_env->cmd_para._show , == , "containers" ) )
		{
			return -DoShow_containers( cocker_env );
		}
		else
		{
			printf( "*** ERROR : show[%s] invalid\n" , cocker_env->cmd_para._show );
			return -7;
		}
	}
	else if( cocker_env->cmd_para._action )
	{
		if( STRCMP( cocker_env->cmd_para._action , == , "create" ) )
		{
			if( cocker_env->image_id[0] == '\0' )
			{
				printf( "*** ERROR : expect '--image-id' with action '-a create'\n" );
				return -7;
			}
			
			if( STRCMP( cocker_env->net , == , "BRIDGE" ) && cocker_env->vip[0] == '\0' )
			{
				printf( "*** ERROR : expect '--vip' with action '-a create'\n" );
				return -7;
			}
			
			return -DoAction_create( cocker_env );
		}
		else if( STRCMP( cocker_env->cmd_para._action , == , "start" ) )
		{
			if( cocker_env->container_id[0] == '\0' )
			{
				printf( "*** ERROR : expect '--container-id' with action '-a start'\n" );
				return -7;
			}
			
			return -DoAction_start( cocker_env );
		}
		else if( STRCMP( cocker_env->cmd_para._action , == , "stop" ) )
		{
			if( cocker_env->container_id[0] == '\0' )
			{
				printf( "*** ERROR : expect '--container-id' with action '-a stop'\n" );
				return -7;
			}
			
			return -DoAction_stop( cocker_env );
		}
		else if( STRCMP( cocker_env->cmd_para._action , == , "kill" ) )
		{
			if( cocker_env->container_id[0] == '\0' )
			{
				printf( "*** ERROR : expect '--container-id' with action '-a kill'\n" );
				return -7;
			}
			
			return -DoAction_kill( cocker_env );
		}
		else if( STRCMP( cocker_env->cmd_para._action , == , "destroy" ) )
		{
			if( cocker_env->container_id[0] == '\0' )
			{
				printf( "*** ERROR : expect '--container-id' with action '-a destroy'\n" );
				return -7;
			}
			
			return -DoAction_destroy( cocker_env );
		}
		else if( STRCMP( cocker_env->cmd_para._action , == , "vip" ) )
		{
			if( cocker_env->container_id[0] == '\0' )
			{
				printf( "*** ERROR : expect '--container-id' with action '-a destroy'\n" );
				return -7;
			}
			
			if( cocker_env->vip[0] == '\0' )
			{
				printf( "*** ERROR : expect '--vip' with action '-a destroy'\n" );
				return -7;
			}
			
			return -DoAction_vip( cocker_env );
		}
		else if( STRCMP( cocker_env->cmd_para._action , == , "port_mapping" ) )
		{
			if( cocker_env->container_id[0] == '\0' )
			{
				printf( "*** ERROR : expect '--container-id' with action '-a destroy'\n" );
				return -7;
			}
			
			if( cocker_env->port_mapping[0] == '\0' )
			{
				printf( "*** ERROR : expect '--port-mapping' with action '-a destroy'\n" );
				return -7;
			}
			
			return -DoAction_port_mapping( cocker_env );
		}
		else if( STRCMP( cocker_env->cmd_para._action , == , "install_test" ) )
		{
			return -DoAction_install_test( cocker_env );
		}
		else
		{
			printf( "*** ERROR : action[%s] invalid\n" , cocker_env->cmd_para._action );
			return -7;
		}
	}
	else
	{
		printf( "*** ERROR : cmd para action[%s] invalid\n" , cocker_env->cmd_para._action );
		return -7;
	}
}

int main( int argc , char *argv[] )
{
	struct CockerEnvironment	*cocker_env = NULL ;
	
	int				nret = 0 ;
	
	nret = CreateCockerEnvironment( & cocker_env ) ;
	if( nret )
	{
		return -nret;
	}
	
	if( argc == 1 )
	{
		usage();
		return 0;
	}
	
	nret = ParseCommandParameters( cocker_env , argc , argv ) ;
	if( nret )
	{
		return -nret;
	}
	
	nret = ExecuteCommandParameters( cocker_env ) ;
	DestroyCockerEnvironment( & cocker_env );
	if( nret )
	{
		return -nret;
	}
	
	return 0;
}

