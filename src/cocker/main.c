/*
 * cocker - Container Machine Engine
 * author	: calvin
 * email	: calvinwilliams@163.com
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#include "cocker_in.h"

static void usage()
{
	printf( "USAGE : cocker -v\n" );
	printf( "               -s images\n" );
	printf( "               -s containers\n" );
	printf( "               -a create (-m|--image) (image[:version])[,(image[:version])]... [ create options ] [ (-c|--container) (container) ] [ (-b|--boot) [ cgroup options ] [ (-t|--attach) | (-e|--exec) (cmd|\"program para1 ...\") ] ]\n" );
	printf( "               -a rplfile (-c|--container) (container) (--template-file) (template_file) (--mapping-file) (mapping_file) [ (--instance-file) (instance_file) ]\n" );
	printf( "               -a boot (-c|--container) (container) [ cgroup options ] [ (-t|--attach) | (-e|--exec) (cmd|\"program para1 ...\") ]\n" );
	printf( "               -a attach (-c|--container) (container)\n" );
	printf( "               -a run (-c|--container) (container) (--cmd) (cmd)\n" );
	printf( "               -a shutdown (-c|--container) (container) [ (-f|--forcely) ]\n" );
	printf( "               -a kill (-c|--container) (container) [ (-f|--forcely) ]\n" );
	printf( "               -a destroy (-c|--container) (container) [ (-f|--forcely) ] [ (-h|--shutdown) ]\n" );
	printf( "               -a version (-m|--image) (image[:version]) [ --version (version) ]\n" );
	printf( "               -a vip (-c|--container) (container) --vip (ip)\n" );
	printf( "               -a port_mapping (-c|--container) (container) --port-mapping (src_port:dst_port)\n" );
	printf( "               -a volume (-c|--container) (container) --volume (host_path[:container_path])[ ...]\n" );
	printf( "               -a to_image --from-container (container) [ --verion (verion) ] --to-image (image)\n" );
	printf( "               -a to_container --from-image (image[:version]) (-m|--image) (image[:version])[,(image[:version])]... [ create options ] --to-container (container)\n" );
	printf( "               -a copy_image --from-image (image[:version]) --to-image (image[:version])\n" );
	printf( "               -a del_image (-m|--image) (image[:version])\n" );
	printf( "               -a import --image-file (file)\n" );
	printf( "               -a export (-m|--image) (image[:version])\n" );
	printf( "               -s ssearch --srepo (user@host)\n" );
	printf( "               -a install_test\n" );
	printf( "create options : [ --volume (host_path:container_path) ][ --volume ... ] [ --host (hostname) ] [ --net (BRIDGE|HOST|CUSTOM) ] [ --host-eth (eth) ] [ --vip (ip) ] [ --port-mapping (src_port:dst_port) ]\n" );
	printf( "cgroup options : [ --cpus [(cpu_num,...)|(cpu_num-cpu_num2)] ] [ --cpu-quota (percent%%) ] [ --mem-limit (num|numM) ]\n" );
	printf( "  enable debug : [ (-d|--debug) ]\n" );
	return;
}

static int ParseCommandParameters( struct CockerEnvironment *env , int argc , char *argv[] )
{
	int		i ;
	char		cmd[ 4096 ] ;
	
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
		else if( STRCMP( argv[i] , == , "--version" ) && i + 1 < argc )
		{
			env->cmd_para.__version = argv[i+1] ;
			i++;
		}
		else if( ( STRCMP( argv[i] , == , "-m" ) || STRCMP( argv[i] , == , "--image" ) ) && i + 1 < argc )
		{
			env->cmd_para.__image = argv[i+1] ;
			i++;
		}
		else if( ( STRCMP( argv[i] , == , "-c" ) || STRCMP( argv[i] , == , "--container" ) ) && i + 1 < argc )
		{
			env->cmd_para.__container = argv[i+1] ;
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
		else if( STRCMP( argv[i] , == , "--volume" ) && i + 1 < argc )
		{
			struct CockerVolume	*volume = NULL ;
			char			*p = NULL ;
			
			volume = (struct CockerVolume *)malloc( sizeof(struct CockerVolume) ) ;
			if( volume == NULL )
			{
				printf( "*** ERROR : malloc failed , errno[%d]\n" , errno );
				exit(1);
			}
			memset( volume , 0x00 , sizeof(struct CockerVolume) );
			
			volume->host_path = argv[i+1] ;
			p = strchr( volume->host_path , ':' ) ;
			if( p )
			{
				volume->host_path_len = p - volume->host_path ;
				volume->container_path = p + 1 ;
			}
			else
			{
				volume->host_path_len = strlen(volume->host_path) ;
				volume->container_path = volume->host_path ;
			}
			
			list_add_tail( & (volume->volume_node) , & (env->cmd_para.volume_list) );
			
			env->cmd_para.__volume = argv[i+1] ;
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
		else if( ( STRCMP( argv[i] , == , "-b" ) || STRCMP( argv[i] , == , "--boot" ) ) )
		{
			env->cmd_para.__boot = argv[i] ;
		}
		else if( ( STRCMP( argv[i] , == , "-t" ) || STRCMP( argv[i] , == , "--attach" ) ) )
		{
			env->cmd_para.__attach = argv[i] ;
		}
		else if( ( STRCMP( argv[i] , == , "-e" ) || STRCMP( argv[i] , == , "--exec" ) ) && i + 1 < argc )
		{
			env->cmd_para.__exec = argv[i+1] ;
			i++;
		}
		else if( ( STRCMP( argv[i] , == , "-h" ) || STRCMP( argv[i] , == , "--shutdown" ) ) )
		{
			env->cmd_para.__shutdown = argv[i] ;
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
		else if( STRCMP( argv[i] , == , "--image-file" ) && i + 1 < argc )
		{
			env->cmd_para.__image_file = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "--srepo" ) && i + 1 < argc )
		{
			env->cmd_para.__srepo = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "--match" ) && i + 1 < argc )
		{
			env->cmd_para.__match = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "--cmd" ) && i + 1 < argc )
		{
			env->cmd_para.__cmd = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "--template-file" ) && i + 1 < argc )
		{
			env->cmd_para.__template_file = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "--mapping-file" ) && i + 1 < argc )
		{
			env->cmd_para.__mapping_file = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "--instance-file" ) && i + 1 < argc )
		{
			env->cmd_para.__instance_file = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "--src-file" ) && i + 1 < argc )
		{
			env->cmd_para.__src_file = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "--dst-file" ) && i + 1 < argc )
		{
			env->cmd_para.__dst_file = argv[i+1] ;
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
	
	if( env->cmd_para.__net == NULL )
		env->cmd_para.__net = "HOST" ;
	
	if( ! ( STRCMP( env->cmd_para.__net , == , "BRIDGE" ) || STRCMP( env->cmd_para.__net , == , "HOST" ) || STRCMP( env->cmd_para.__net , == , "CUSTOM" ) ) )
	{
		printf( "*** ERROR : '--net' value[%s] invalid\n" , env->cmd_para.__net );
		return -7;
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
	
	memset( cmd , 0x00 , sizeof(cmd) );
	SnprintfAndPopen( env->host_eth_ip , sizeof(env->host_eth_ip) , cmd , sizeof(cmd) , "ifconfig %s | grep -w inet | awk '{print $2}'" , env->host_eth_name );
	TrimEnter( env->host_eth_ip );
	
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
		else if( STRCMP( env->cmd_para._show , == , "container_root" ) )
		{
			INFOLOGC( "--- call DoShow_container_root ---" )
			nret = DoShow_container_root( env ) ;
			INFOLOGC( "--- DoShow_container_root return[%d] ---" , nret )
		}
		else if( STRCMP( env->cmd_para._show , == , "ssearch" ) )
		{
			INFOLOGC( "--- call DoShow_ssearch ---" )
			nret = DoShow_ssearch( env ) ;
			INFOLOGC( "--- DoShow_ssearch return[%d] ---" , nret )
		}
		else
		{
			printf( "*** ERROR : show[%s] invalid\n" , env->cmd_para._show );
			return -7;
		}
	}
	else if( env->cmd_para._action )
	{
		if( STRCMP( env->cmd_para._action , == , "install_test" ) )
		{
			return -DoAction_install_test( env );
		}
		else if( STRCMP( env->cmd_para._action , == , "create" ) )
		{
			if( IS_NULL_OR_EMPTY(env->cmd_para.__image) )
			{
				printf( "*** ERROR : expect '--image' with action '-a create'\n" );
				return -7;
			}
			
			if( env->cmd_para.__net && STRCMP( env->cmd_para.__net , == , "BRIDGE" ) )
			{
				if( ( env->cmd_para.__vip == NULL || env->cmd_para.__vip[0] == '\0' ) )
				{
					printf( "*** ERROR : expect '--vip' with action '-a create'\n" );
					return -7;
				}
			}
			
			INFOLOGC( "--- call DoAction_create ---" )
			nret = DoAction_create( env ) ;
			INFOLOGC( "--- DoAction_create return[%d] ---" , nret )
		}
		else if( STRCMP( env->cmd_para._action , == , "boot" ) )
		{
			if( IS_NULL_OR_EMPTY(env->cmd_para.__container) )
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
			if( IS_NULL_OR_EMPTY(env->cmd_para.__container) )
			{
				printf( "*** ERROR : expect '--container' with action '-a attach'\n" );
				return -7;
			}
			
			INFOLOGC( "--- call DoAction_attach ---" )
			nret = DoAction_attach( env ) ;
			INFOLOGC( "--- DoAction_attach return[%d] ---" , nret )
		}
		else if( STRCMP( env->cmd_para._action , == , "run" ) )
		{
			if( IS_NULL_OR_EMPTY(env->cmd_para.__container) )
			{
				printf( "*** ERROR : expect '--container' with action '-a run'\n" );
				return -7;
			}
			
			if( IS_NULL_OR_EMPTY(env->cmd_para.__cmd) )
			{
				printf( "*** ERROR : expect '--cmd' with action '-a run'\n" );
				return -7;
			}
			
			INFOLOGC( "--- call DoAction_run ---" )
			nret = DoAction_run( env ) ;
			INFOLOGC( "--- DoAction_run return[%d] ---" , nret )
		}
		else if( STRCMP( env->cmd_para._action , == , "rplfile" ) )
		{
			if( IS_NULL_OR_EMPTY(env->cmd_para.__container) )
			{
				printf( "*** ERROR : expect '--container' with action '-a rplfile'\n" );
				return -7;
			}
			
			if( IS_NULL_OR_EMPTY(env->cmd_para.__template_file) )
			{
				printf( "*** ERROR : expect '--template-file' with action '-a rplfile'\n" );
				return -7;
			}
			
			if( IS_NULL_OR_EMPTY(env->cmd_para.__mapping_file) )
			{
				printf( "*** ERROR : expect '--mapping-file' with action '-a rplfile'\n" );
				return -7;
			}
			
			INFOLOGC( "--- call DoAction_rplfile ---" )
			nret = DoAction_rplfile( env ) ;
			INFOLOGC( "--- DoAction_rplfile return[%d] ---" , nret )
		}
		else if( STRCMP( env->cmd_para._action , == , "putfile" ) )
		{
			if( IS_NULL_OR_EMPTY(env->cmd_para.__container) )
			{
				printf( "*** ERROR : expect '--container' with action '-a putfile'\n" );
				return -7;
			}
			
			if( IS_NULL_OR_EMPTY(env->cmd_para.__src_file) )
			{
				printf( "*** ERROR : expect '--src-file' with action '-a putfile'\n" );
				return -7;
			}
			
			if( IS_NULL_OR_EMPTY(env->cmd_para.__dst_file) )
			{
				printf( "*** ERROR : expect '--dst-file' with action '-a putfile'\n" );
				return -7;
			}
			
			INFOLOGC( "--- call DoAction_putfile ---" )
			nret = DoAction_putfile( env ) ;
			INFOLOGC( "--- DoAction_putfile return[%d] ---" , nret )
		}
		else if( STRCMP( env->cmd_para._action , == , "getfile" ) )
		{
			if( IS_NULL_OR_EMPTY(env->cmd_para.__container) )
			{
				printf( "*** ERROR : expect '--container' with action '-a getfile'\n" );
				return -7;
			}
			
			if( IS_NULL_OR_EMPTY(env->cmd_para.__src_file) )
			{
				printf( "*** ERROR : expect '--src-file' with action '-a getfile'\n" );
				return -7;
			}
			
			if( IS_NULL_OR_EMPTY(env->cmd_para.__dst_file) )
			{
				printf( "*** ERROR : expect '--dst-file' with action '-a getfile'\n" );
				return -7;
			}
			
			INFOLOGC( "--- call DoAction_getfile ---" )
			nret = DoAction_getfile( env ) ;
			INFOLOGC( "--- DoAction_getfile return[%d] ---" , nret )
		}
		else if( STRCMP( env->cmd_para._action , == , "shutdown" ) )
		{
			if( IS_NULL_OR_EMPTY(env->cmd_para.__container) )
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
			if( IS_NULL_OR_EMPTY(env->cmd_para.__container) )
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
			if( IS_NULL_OR_EMPTY(env->cmd_para.__container) )
			{
				printf( "*** ERROR : expect '--container' with action '-a destroy'\n" );
				return -7;
			}
			
			INFOLOGC( "--- call DoAction_destroy ---" )
			nret = DoAction_destroy( env ) ;
			INFOLOGC( "--- DoAction_destroy return[%d] ---" , nret )
		}
		else if( STRCMP( env->cmd_para._action , == , "version" ) )
		{
			if( IS_NULL_OR_EMPTY(env->cmd_para.__image) )
			{
				printf( "*** ERROR : expect '--image' with action '-a version'\n" );
				return -7;
			}
			
			INFOLOGC( "--- call DoAction_version ---" )
			nret = DoAction_version( env ) ;
			INFOLOGC( "--- DoAction_version return[%d] ---" , nret )
		}
		else if( STRCMP( env->cmd_para._action , == , "vip" ) )
		{
			if( IS_NULL_OR_EMPTY(env->cmd_para.__container) )
			{
				printf( "*** ERROR : expect '--container' with action '-a vip'\n" );
				return -7;
			}
			
			if( IS_NULL_OR_EMPTY(env->cmd_para.__vip) )
			{
				printf( "*** ERROR : expect '--vip' with action '-a vip'\n" );
				return -7;
			}
			
			INFOLOGC( "--- call DoAction_vip ---" )
			nret = DoAction_vip( env ) ;
			INFOLOGC( "--- DoAction_vip return[%d] ---" , nret )
		}
		else if( STRCMP( env->cmd_para._action , == , "port_mapping" ) )
		{
			if( IS_NULL_OR_EMPTY(env->cmd_para.__container) )
			{
				printf( "*** ERROR : expect '--container' with action '-a port_mapping'\n" );
				return -7;
			}
			
			if( IS_NULL_OR_EMPTY(env->cmd_para.__port_mapping) )
			{
				printf( "*** ERROR : expect '--port-mapping' with action '-a port_mapping'\n" );
				return -7;
			}
			
			INFOLOGC( "--- call DoAction_port_mapping ---" )
			nret = DoAction_port_mapping( env ) ;
			INFOLOGC( "--- DoAction_port_mapping return[%d] ---" , nret )
		}
		else if( STRCMP( env->cmd_para._action , == , "volume" ) )
		{
			if( IS_NULL_OR_EMPTY(env->cmd_para.__container) )
			{
				printf( "*** ERROR : expect '--volume' with action '-a volume'\n" );
				return -7;
			}
			
			if( IS_NULL_OR_EMPTY(env->cmd_para.__volume) )
			{
				printf( "*** ERROR : expect '--volume' with action '-a volume'\n" );
				return -7;
			}
			
			INFOLOGC( "--- call DoAction_volume ---" )
			nret = DoAction_volume( env ) ;
			INFOLOGC( "--- DoAction_volume return[%d] ---" , nret )
		}
		else if( STRCMP( env->cmd_para._action , == , "to_image" ) )
		{
			if( IS_NULL_OR_EMPTY(env->cmd_para.__from_container) )
			{
				printf( "*** ERROR : expect '--from-container' with action '-a to_image'\n" );
				return -7;
			}
			
			if( IS_NULL_OR_EMPTY(env->cmd_para.__to_image) )
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
			if( IS_NULL_OR_EMPTY(env->cmd_para.__from_image) )
			{
				printf( "*** ERROR : expect '--from-image' with action '-a to_image'\n" );
				return -7;
			}
			
			if( IS_NULL_OR_EMPTY(env->cmd_para.__to_container) )
			{
				printf( "*** ERROR : expect '--to-container' with action '-a to_container'\n" );
				return -7;
			}
			
			if( STRCMP( env->cmd_para.__net , == , "BRIDGE" ) && env->cmd_para.__vip[0] == '\0' )
			{
				printf( "*** ERROR : expect '--vip' with action '-a create'\n" );
				return -7;
			}
			
			INFOLOGC( "--- call DoAction_to_container ---" )
			nret = DoAction_to_container( env ) ;
			INFOLOGC( "--- DoAction_to_container return[%d] ---" , nret )
		}
		else if( STRCMP( env->cmd_para._action , == , "copy_image" ) )
		{
			if( IS_NULL_OR_EMPTY(env->cmd_para.__from_image) )
			{
				printf( "*** ERROR : expect '--from-image' with action '-a copy_image'\n" );
				return -7;
			}
			
			if( IS_NULL_OR_EMPTY(env->cmd_para.__to_image) )
			{
				printf( "*** ERROR : expect '--to-image' with action '-a copy_image'\n" );
				return -7;
			}
			
			INFOLOGC( "--- call DoAction_copy_image ---" )
			nret = DoAction_copy_image( env ) ;
			INFOLOGC( "--- DoAction_copy_image return[%d] ---" , nret )
		}
		else if( STRCMP( env->cmd_para._action , == , "del_image" ) )
		{
			if( IS_NULL_OR_EMPTY(env->cmd_para.__image) )
			{
				printf( "*** ERROR : expect '--image' with action '-a del_image'\n" );
				return -7;
			}
			
			INFOLOGC( "--- call DoAction_del_image ---" )
			nret = DoAction_del_image( env ) ;
			INFOLOGC( "--- DoAction_del_image return[%d] ---" , nret )
		}
		else if( STRCMP( env->cmd_para._action , == , "export" ) )
		{
			if( IS_NULL_OR_EMPTY(env->cmd_para.__image) )
			{
				printf( "*** ERROR : expect '--image' with action '-a export'\n" );
				return -7;
			}
			
			INFOLOGC( "--- call DoAction_export ---" )
			nret = DoAction_export( env ) ;
			INFOLOGC( "--- DoAction_export return[%d] ---" , nret )
		}
		else if( STRCMP( env->cmd_para._action , == , "import" ) )
		{
			if( IS_NULL_OR_EMPTY(env->cmd_para.__image_file) )
			{
				printf( "*** ERROR : expect '--image-file' with action '-a import'\n" );
				return -7;
			}
			
			INFOLOGC( "--- call DoAction_import ---" )
			nret = DoAction_import( env ) ;
			INFOLOGC( "--- DoAction_import return[%d] ---" , nret )
		}
		else if( STRCMP( env->cmd_para._action , == , "spush" ) )
		{
			if( IS_NULL_OR_EMPTY(env->cmd_para.__image) )
			{
				printf( "*** ERROR : expect '--image' with action '-a spush'\n" );
				return -7;
			}
			
			INFOLOGC( "--- call DoAction_spush ---" )
			nret = DoAction_spush( env ) ;
			INFOLOGC( "--- DoAction_spush return[%d] ---" , nret )
		}
		else if( STRCMP( env->cmd_para._action , == , "spull" ) )
		{
			if( IS_NULL_OR_EMPTY(env->cmd_para.__image) )
			{
				printf( "*** ERROR : expect '--image' with action '-a spull'\n" );
				return -7;
			}
			
			INFOLOGC( "--- call DoAction_spull ---" )
			nret = DoAction_spull( env ) ;
			INFOLOGC( "--- DoAction_spull return[%d] ---" , nret )
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

