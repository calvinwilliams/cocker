#include "cocker_in.h"

static void usage()
{
	printf( "USAGE : cocker -s images\n" );
	printf( "               -s containers\n" );
	printf( "               -a create (-m|--image) (id) [ (-c|--container) (id) ] [ --host (hostname) ] [ --net (BRIDGE|HOST|CUSTOM) ] [ --host-eth (eth) ] [ --vip (ip) ] [ --port-mapping (src_port:dst_port) ]\n" );
	printf( "               -a boot (-c|--container) (id) [ cgroup options ] [ (-t|--attach) ]\n" );
	printf( "               -a attach (-c|--container) (id)\n" );
	printf( "               -a shutdown (-c|--container) (id) [ (-f|--forcely) ]\n" );
	printf( "               -a destroy (-c|--container) (id) [ (-f|--forcely) ]\n" );
	printf( "               -a vip (-c|--container) (id) --vip (ip)\n" );
	printf( "               -a port_mapping (-c|--container) (id) --port-mapping (src_port:dst_port)\n" );
	printf( "               -a install_test\n" );
	printf( "                    (-d|--debug)\n" );
	printf( "cgroup options : [ --cpus (cpu_num,...) ] [ --cpu-quota (percent%%) ] [ --mem-limit (num|numM) ]\n" );
	return;
}

static int ParseCommandParameters( struct CockerEnvironment *env , int argc , char *argv[] )
{
	int		i ;
	
	int		nret = 0 ;
	
	for( i = 1 ; i < argc ; i++ )
	{
		if( STRCMP( argv[i] , == , "-v" ) )
		{
			printf( "cocker v%s build %s %s\n" , _COCKER_VERSION , __DATE__ , __TIME__ );
			DestroyCockerEnvironment( & env );
			exit(0);
		}
		else if( STRCMP( argv[i] , == , "-s" ) && i + 1 < argc )
		{
			env->cmd_para._show = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "-a" ) && i + 1 < argc )
		{
			env->cmd_para._action = argv[i+1] ;
			i++;
		}
		else if( ( STRCMP( argv[i] , == , "-m" ) || STRCMP( argv[i] , == , "--image" ) ) && i + 1 < argc )
		{
			env->cmd_para.__image_id = argv[i+1] ;
			i++;
		}
		else if( ( STRCMP( argv[i] , == , "-c" ) || STRCMP( argv[i] , == , "--container" ) ) && i + 1 < argc )
		{
			env->cmd_para.__container_id = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "--host" ) && i + 1 < argc )
		{
			env->cmd_para.__host_name = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "--net" ) && i + 1 < argc )
		{
			env->cmd_para.__net = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "--host-eth" ) && i + 1 < argc )
		{
			env->cmd_para.__host_eth = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "--vip" ) && i + 1 < argc )
		{
			env->cmd_para.__vip = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "--port-mapping" ) && i + 1 < argc )
		{
			env->cmd_para.__port_mapping = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "--cpus" ) && i + 1 < argc )
		{
			env->cmd_para.__cpus = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "--cpu-quota" ) && i + 1 < argc )
		{
			env->cmd_para.__cpu_quota = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "--mem-limit" ) && i + 1 < argc )
		{
			env->cmd_para.__mem_limit = argv[i+1] ;
			i++;
		}
		else if( ( STRCMP( argv[i] , == , "-t" ) || STRCMP( argv[i] , == , "--attach" ) ) )
		{
			env->cmd_para.__attach = argv[i] ;
		}
		else if( STRCMP( argv[i] , == , "--from-container" ) && i + 1 < argc )
		{
			env->cmd_para.__from_container = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "--to-image" ) && i + 1 < argc )
		{
			env->cmd_para.__to_image = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "--from-image" ) && i + 1 < argc )
		{
			env->cmd_para.__from_image = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "--to-container" ) && i + 1 < argc )
		{
			env->cmd_para.__to_container = argv[i+1] ;
			i++;
		}
		else if( ( STRCMP( argv[i] , == , "-d" ) || STRCMP( argv[i] , == , "--debug" ) ) )
		{
			env->cmd_para.__debug = argv[i] ;
		}
		else if( ( STRCMP( argv[i] , == , "-f" ) || STRCMP( argv[i] , == , "--forcely" ) ) )
		{
			env->cmd_para.__forcely = argv[i] ;
		}
		else
		{
			printf( "*** ERROR : invalid parameter '%s'\n" , argv[i] );
			usage();
			return -7;
		}
	}
	
	memset( env->net , 0x00 , sizeof(env->net) );
	if( env->cmd_para.__image_id )
	{
		strncpy( env->image_id , env->cmd_para.__image_id , sizeof(env->image_id)-1 );
	}
	
	memset( env->net , 0x00 , sizeof(env->net) );
	if( env->cmd_para.__container_id )
	{
		strncpy( env->container_id , env->cmd_para.__container_id , sizeof(env->container_id)-1 );
	}
	
	if( env->cmd_para.__net == NULL )
		env->cmd_para.__net = "BRIDGE" ;
	
	if( ! ( STRCMP( env->cmd_para.__net , == , "BRIDGE" ) || STRCMP( env->cmd_para.__net , == , "HOST" ) || STRCMP( env->cmd_para.__net , == , "CUSTOM" ) ) )
	{
		printf( "*** ERROR : '--net' value[%s] invalid\n" , env->cmd_para.__net );
		return -7;
	}
	
	memset( env->net , 0x00 , sizeof(env->net) );
	if( env->cmd_para.__net )
	{
		strncpy( env->net , env->cmd_para.__net , sizeof(env->net)-1 );
	}
	
	if( env->cmd_para.__host_eth )
	{
		memset( env->host_eth_name , 0x00 , sizeof(env->host_eth_name) );
		strncpy( env->host_eth_name , env->cmd_para.__host_eth , sizeof(env->host_eth_name)-1 );
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
		
		memset( env->host_eth_name , 0x00 , sizeof(env->host_eth_name) );
		ifa = ifa_base ;
		while( ifa )
		{
			if( ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET )
			{
				if( STRCMP( ifa->ifa_name , != , "lo" ) && STRCMP( ifa->ifa_name , != , "cocker0" ) && STRNCMP( ifa->ifa_name , != , "veth" , 4 ) )
				{
					strncpy( env->host_eth_name , ifa->ifa_name , sizeof(env->host_eth_name)-1 );
					break;
				}
			}
			
			ifa = ifa->ifa_next ;
		}
		
		freeifaddrs( ifa_base );
		
		if( env->host_eth_name[0] == '\0' )
		{
			printf( "*** ERROR : host if name not found\n" );
			return -1;
		}
	}
	
	memset( env->vip , 0x00 , sizeof(env->vip) );
	if( env->cmd_para.__vip )
	{
		strncpy( env->vip , env->cmd_para.__vip , sizeof(env->vip)-1 );
	}
	
	memset( env->port_mapping , 0x00 , sizeof(env->port_mapping) );
	if( env->cmd_para.__port_mapping )
	{
		strncpy( env->port_mapping , env->cmd_para.__port_mapping , sizeof(env->port_mapping)-1 );
	}
	
	memset( env->netns_name , 0x00 , sizeof(env->netns_name) );
	snprintf( env->netns_name , sizeof(env->netns_name) , "netns%s" , env->container_id );
	
	memset( env->netbr_name , 0x00 , sizeof(env->netbr_name) );
	snprintf( env->netbr_name , sizeof(env->netbr_name) , "cocker0" );
	
	memset( env->veth1_name , 0x00 , sizeof(env->veth1_name) );
	snprintf( env->veth1_name , sizeof(env->veth1_name) , "eth%s" , env->container_id );
	
	memset( env->veth0_name , 0x00 , sizeof(env->veth0_name) );
	snprintf( env->veth0_name , sizeof(env->veth0_name) , "veth%s" , env->container_id );
	
	memset( env->veth0_sname , 0x00 , sizeof(env->veth0_sname) );
	snprintf( env->veth0_sname , sizeof(env->veth0_sname) , "eth0" );
	
	if( env->cmd_para.__cpus || env->cmd_para.__cpu_quota || env->cmd_para.__mem_limit )
		env->cgroup_enable = 1 ;
	
	return 0;
}

static int ExecuteCommandParameters( struct CockerEnvironment *env )
{
	int		nret = 0 ;
	
	SetLogcFile( "/var/cocker/cocker.log" );
	SetLogcLevel( LOGCLEVEL_INFO );
	
	if( env->cmd_para._show )
	{
		if( STRCMP( env->cmd_para._show , == , "images" ) )
		{
			INFOLOGC( "--- call DoShow_images ---" )
			nret = DoShow_images( env ) ;
			INFOLOGC( "--- DoShow_images return[%d] ---" , nret )
		}
		else if( STRCMP( env->cmd_para._show , == , "containers" ) )
		{
			INFOLOGC( "--- call DoShow_containers ---" )
			nret = DoShow_containers( env ) ;
			INFOLOGC( "--- DoShow_containers return[%d] ---" , nret )
		}
		else
		{
			printf( "*** ERROR : show[%s] invalid\n" , env->cmd_para._show );
			return -7;
		}
	}
	else if( env->cmd_para._action )
	{
		if( STRCMP( env->cmd_para._action , == , "create" ) )
		{
			if( env->image_id[0] == '\0' )
			{
				printf( "*** ERROR : expect '--image' with action '-a create'\n" );
				return -7;
			}
			
			if( STRCMP( env->net , == , "BRIDGE" ) && env->vip[0] == '\0' )
			{
				printf( "*** ERROR : expect '--vip' with action '-a create'\n" );
				return -7;
			}
			
			INFOLOGC( "--- call DoAction_create ---" )
			nret = DoAction_create( env ) ;
			INFOLOGC( "--- DoAction_create return[%d] ---" , nret )
		}
		else if( STRCMP( env->cmd_para._action , == , "boot" ) )
		{
			if( env->container_id[0] == '\0' )
			{
				printf( "*** ERROR : expect '--container' with action '-a boot'\n" );
				return -7;
			}
			
			INFOLOGC( "--- call DoAction_boot ---" )
			nret = DoAction_boot( env ) ;
			INFOLOGC( "--- DoAction_boot return[%d] ---" , nret )
		}
		else if( STRCMP( env->cmd_para._action , == , "attach" ) )
		{
			if( env->container_id[0] == '\0' )
			{
				printf( "*** ERROR : expect '--container' with action '-a attach'\n" );
				return -7;
			}
			
			INFOLOGC( "--- call DoAction_attach ---" )
			nret = DoAction_attach( env ) ;
			INFOLOGC( "--- DoAction_attach return[%d] ---" , nret )
		}
		else if( STRCMP( env->cmd_para._action , == , "shutdown" ) )
		{
			if( env->container_id[0] == '\0' )
			{
				printf( "*** ERROR : expect '--container' with action '-a shutdown'\n" );
				return -7;
			}
			
			INFOLOGC( "--- call DoAction_shutdown ---" )
			nret = DoAction_shutdown( env ) ;
			INFOLOGC( "--- DoAction_shutdown return[%d] ---" , nret )
		}
		else if( STRCMP( env->cmd_para._action , == , "kill" ) )
		{
			if( env->container_id[0] == '\0' )
			{
				printf( "*** ERROR : expect '--container' with action '-a kill'\n" );
				return -7;
			}
			
			INFOLOGC( "--- call DoAction_kill ---" )
			nret = DoAction_kill( env ) ;
			INFOLOGC( "--- DoAction_kill return[%d] ---" , nret )
		}
		else if( STRCMP( env->cmd_para._action , == , "destroy" ) )
		{
			if( env->container_id[0] == '\0' )
			{
				printf( "*** ERROR : expect '--container' with action '-a destroy'\n" );
				return -7;
			}
			
			INFOLOGC( "--- call DoAction_destroy ---" )
			nret = DoAction_destroy( env ) ;
			INFOLOGC( "--- DoAction_destroy return[%d] ---" , nret )
		}
		else if( STRCMP( env->cmd_para._action , == , "vip" ) )
		{
			if( env->container_id[0] == '\0' )
			{
				printf( "*** ERROR : expect '--container' with action '-a destroy'\n" );
				return -7;
			}
			
			if( env->vip[0] == '\0' )
			{
				printf( "*** ERROR : expect '--vip' with action '-a destroy'\n" );
				return -7;
			}
			
			INFOLOGC( "--- call DoAction_vip ---" )
			nret = DoAction_vip( env ) ;
			INFOLOGC( "--- DoAction_vip return[%d] ---" , nret )
		}
		else if( STRCMP( env->cmd_para._action , == , "port_mapping" ) )
		{
			if( env->container_id[0] == '\0' )
			{
				printf( "*** ERROR : expect '--container' with action '-a destroy'\n" );
				return -7;
			}
			
			if( env->port_mapping[0] == '\0' )
			{
				printf( "*** ERROR : expect '--port-mapping' with action '-a destroy'\n" );
				return -7;
			}
			
			INFOLOGC( "--- call DoAction_port_mapping ---" )
			nret = DoAction_port_mapping( env ) ;
			INFOLOGC( "--- DoAction_port_mapping return[%d] ---" , nret )
		}
		else if( STRCMP( env->cmd_para._action , == , "install_test" ) )
		{
			return -DoAction_install_test( env );
		}
		else if( STRCMP( env->cmd_para._action , == , "to_image" ) )
		{
			if( env->cmd_para.__from_container == NULL || env->cmd_para.__from_container[0] == '\0' )
			{
				printf( "*** ERROR : expect '--from-container' with action '-a to_image'\n" );
				return -7;
			}
			
			if( env->cmd_para.__to_image == NULL || env->cmd_para.__to_image[0] == '\0' )
			{
				printf( "*** ERROR : expect '--to-image' with action '-a to_image'\n" );
				return -7;
			}
			
			INFOLOGC( "--- call DoAction_to_image ---" )
			nret = DoAction_to_image( env ) ;
			INFOLOGC( "--- DoAction_to_image return[%d] ---" , nret )
		}
		else if( STRCMP( env->cmd_para._action , == , "to_container" ) )
		{
			if( env->cmd_para.__from_image == NULL || env->cmd_para.__from_image[0] == '\0' )
			{
				printf( "*** ERROR : expect '--from-image' with action '-a to_image'\n" );
				return -7;
			}
			
			if( env->cmd_para.__to_container == NULL || env->cmd_para.__to_container[0] == '\0' )
			{
				printf( "*** ERROR : expect '--to-container' with action '-a to_container'\n" );
				return -7;
			}
			
			INFOLOGC( "--- call DoAction_to_container ---" )
			nret = DoAction_to_container( env ) ;
			INFOLOGC( "--- DoAction_to_container return[%d] ---" , nret )
		}
		else
		{
			printf( "*** ERROR : action[%s] invalid\n" , env->cmd_para._action );
			return -7;
		}
	}
	else
	{
		printf( "*** ERROR : cmd para action[%s] invalid\n" , env->cmd_para._action );
		return -7;
	}
	
	return -nret;
}

int main( int argc , char *argv[] )
{
	struct CockerEnvironment	*env = NULL ;
	
	int				nret = 0 ;
	
	nret = CreateCockerEnvironment( & env ) ;
	if( nret )
	{
		return -nret;
	}
	
	if( argc == 1 )
	{
		usage();
		return 0;
	}
	
	nret = ParseCommandParameters( env , argc , argv ) ;
	if( nret )
	{
		return -nret;
	}
	
	nret = ExecuteCommandParameters( env ) ;
	DestroyCockerEnvironment( & env );
	if( nret )
	{
		return -nret;
	}
	
	return 0;
}

