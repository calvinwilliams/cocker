#include "cocker_in.h"

static void usage()
{
	printf( "USAGE : cocker -s images\n" );
	printf( "               -s containers\n" );
	printf( "               -a create [ --image (name) ] --container (name) [ --host-name (name) ]\n" );
	printf( "               -a start --container (name)\n" );
	printf( "               -a stop --container (name) [ --forcely ]\n" );
	printf( "               -a destroy --container (name) [ --forcely ]\n" );
	printf( "               -a install_test\n" );
	return;
}

static int ParseCommandParameters( struct CockerEnvironment *cocker_env , int argc , char *argv[] )
{
	int		i ;
	
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
		else if( STRCMP( argv[i] , == , "--image" ) && i + 1 < argc )
		{
			cocker_env->cmd_para.__image = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "--container" ) && i + 1 < argc )
		{
			cocker_env->cmd_para.__container = argv[i+1] ;
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
		else if( STRCMP( argv[i] , == , "--nat-postrouting" ) && i + 1 < argc )
		{
			cocker_env->cmd_para.__nat_postrouting = argv[i+1] ;
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
			usage();
			return -7;
		}
	}
	
	if( cocker_env->cmd_para.__net == NULL )
		cocker_env->cmd_para.__net = "none" ;
	
	if( ! ( STRCMP( cocker_env->cmd_para.__net , == , "none" ) || STRCMP( cocker_env->cmd_para.__net , == , "host" ) || STRCMP( cocker_env->cmd_para.__net , == , "bridge" ) ) )
	{
		printf( "*** ERROR : '--net' value[%s] invalid\n" , cocker_env->cmd_para.__net );
		return -7;
	}
	
	return 0;
}

static int ExecuteCommandParameters( struct CockerEnvironment *cocker_env )
{
	int		nret = 0 ;
	
	nret = SnprintfAndMakeDir( cocker_env->images_path_base , sizeof(cocker_env->images_path_base)-1 , "%s/images" , cocker_env->cocker_home ) ;
	if( nret )
	{
		printf( "*** ERROR : SnprintfAndMakeDir[%s] failed[%d]\n" , cocker_env->images_path_base , nret );
		return -1;
	}
	else if( cocker_env->cmd_para.__debug )
	{
		printf( "mkdir %s ok\n" , cocker_env->images_path_base );
	}
	
	nret = SnprintfAndMakeDir( cocker_env->containers_path_base , sizeof(cocker_env->containers_path_base)-1 , "%s/containers" , cocker_env->cocker_home ) ;
	if( nret )
	{
		printf( "*** ERROR : SnprintfAndMakeDir[%s] failed[%d]\n" , cocker_env->containers_path_base , nret );
		return -1;
	}
	else if( cocker_env->cmd_para.__debug )
	{
		printf( "mkdir %s ok\n" , cocker_env->containers_path_base );
	}
	
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
			if( cocker_env->cmd_para.__container == NULL ||  STRCMP( cocker_env->cmd_para.__container , == , "" ) )
			{
				printf( "*** ERROR : expect '--container' with action '-a create'\n" );
				return -7;
			}
			
			return -DoAction_create( cocker_env );
		}
		else if( STRCMP( cocker_env->cmd_para._action , == , "start" ) )
		{
			if( cocker_env->cmd_para.__container == NULL || STRCMP( cocker_env->cmd_para.__container , == , "" ) )
			{
				printf( "*** ERROR : expect '--container' with action '-a start'\n" );
				return -7;
			}
			
			return -DoAction_start( cocker_env );
		}
		else if( STRCMP( cocker_env->cmd_para._action , == , "stop" ) )
		{
			if( cocker_env->cmd_para.__container == NULL || STRCMP( cocker_env->cmd_para.__container , == , "" ) )
			{
				printf( "*** ERROR : expect '--container' with action '-a stop'\n" );
				return -7;
			}
			
			return -DoAction_stop( cocker_env );
		}
		else if( STRCMP( cocker_env->cmd_para._action , == , "destroy" ) )
		{
			if( cocker_env->cmd_para.__container == NULL || STRCMP( cocker_env->cmd_para.__container , == , "" ) )
			{
				printf( "*** ERROR : expect '--container' with action '-a destroy'\n" );
				return -7;
			}
			
			return -DoAction_destroy( cocker_env );
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
	
	int			nret = 0 ;
	
	nret = CreateCockerEnvironment( & cocker_env ) ;
	if( nret )
	{
		printf( "*** ERROR : CreateCockerEnvironment failed[%d]\n" , nret );
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
		printf( "*** ERROR : ParseCommandParameters failed[%d]\n" , nret );
		return -nret;
	}
	
	nret = ExecuteCommandParameters( cocker_env ) ;
	DestroyCockerEnvironment( & cocker_env );
	if( nret )
	{
		printf( "*** ERROR : ExecuteCommandParameters failed[%d]\n" , nret );
		return -nret;
	}
	
	return 0;
}

