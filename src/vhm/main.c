#include "vhm_in.h"

static void usage()
{
	printf( "USAGE : vhm -s vtemplates\n" );
	printf( "            -s vhosts\n" );
	printf( "            -a create [ --vtemplate (vt_name) ] --vhost (vh_name) [ --host-name (host_name) ]\n" );
	printf( "            -a destroy --vhost (vh_name)\n" );
	printf( "            -a start --vhost (vh_name)\n" );
	printf( "            -a stop --vhost (vh_name)\n" );
	printf( "            -a install_test\n" );
	return;
}

static int ParseCommandParameters( struct VhmEnvironment *vhm_env , int argc , char *argv[] )
{
	int		i ;
	
	for( i = 1 ; i < argc ; i++ )
	{
		if( STRCMP( argv[i] , == , "-v" ) )
		{
			printf( "vhm v%s build %s %s\n" , _OPENVH_VERSION , __DATE__ , __TIME__ );
			DestroyVhmEnvironment( & vhm_env );
			exit(0);
		}
		else if( STRCMP( argv[i] , == , "-s" ) && i + 1 < argc )
		{
			vhm_env->cmd_para._show = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "-a" ) && i + 1 < argc )
		{
			vhm_env->cmd_para._action = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "--vtemplate" ) && i + 1 < argc )
		{
			vhm_env->cmd_para.__vtemplate = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "--vhost" ) && i + 1 < argc )
		{
			vhm_env->cmd_para.__vhost = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "--host-name" ) && i + 1 < argc )
		{
			vhm_env->cmd_para.__host_name = argv[i+1] ;
			i++;
		}
		else
		{
			usage();
			return -7;
		}
	}
	
	if( vhm_env->cmd_para.__host_name == NULL )
		vhm_env->cmd_para.__host_name = vhm_env->cmd_para.__vhost ;
	
	return 0;
}

static int ExecuteCommandParameters( struct VhmEnvironment *vhm_env )
{
	int		nret = 0 ;
	
	if( getenv("OPENVH_HOME" ) )
	{
		nret = SnprintfAndMakeDir( vhm_env->openvh_home , sizeof(vhm_env->openvh_home)-1 , "%s" , getenv("OPENVH_HOME" ) ) ;
		if( nret )
		{
			printf( "*** ERROR : SnprintfAndMakeDir[%s] failed[%d]\n" , vhm_env->openvh_home , nret );
			return -1;
		}
	}
	else
	{
		nret = SnprintfAndMakeDir( vhm_env->openvh_home , sizeof(vhm_env->openvh_home)-1 , "/var/openvh" ) ;
		if( nret )
		{
			printf( "*** ERROR : SnprintfAndMakeDir[%s] failed[%d]\n" , vhm_env->openvh_home , nret );
			return -1;
		}
	}
	
	nret = SnprintfAndMakeDir( vhm_env->vtemplates_path_base , sizeof(vhm_env->vtemplates_path_base)-1 , "%s/vtemplate" , vhm_env->openvh_home ) ;
	if( nret )
	{
		printf( "*** ERROR : SnprintfAndMakeDir[%s] failed[%d]\n" , vhm_env->vtemplates_path_base , nret );
		return -1;
	}
	
	nret = SnprintfAndMakeDir( vhm_env->vhosts_path_base , sizeof(vhm_env->vhosts_path_base)-1 , "%s/vhost" , vhm_env->openvh_home ) ;
	if( nret )
	{
		printf( "*** ERROR : SnprintfAndMakeDir[%s] failed[%d]\n" , vhm_env->vhosts_path_base , nret );
		return -1;
	}
	
	if( vhm_env->cmd_para._show )
	{
		if( STRCMP( vhm_env->cmd_para._show , == , "vtemplates" ) )
		{
			return -VhmShow_vtemplates( vhm_env );
		}
		else if( STRCMP( vhm_env->cmd_para._show , == , "vhosts" ) )
		{
			return -VhmShow_vhosts( vhm_env );
		}
		else
		{
			printf( "*** ERROR : show[%s] invalid\n" , vhm_env->cmd_para._show );
			return -7;
		}
	}
	else if( vhm_env->cmd_para._action )
	{
		if( STRCMP( vhm_env->cmd_para._action , == , "create" ) )
		{
			if( vhm_env->cmd_para.__vhost == NULL ||  STRCMP( vhm_env->cmd_para.__vhost , == , "" ) )
			{
				printf( "*** ERROR : expect '--vhost' with action '-a create'\n" );
				return -7;
			}
			
			return -VhmAction_create( vhm_env );
		}
		else if( STRCMP( vhm_env->cmd_para._action , == , "start" ) )
		{
			if( vhm_env->cmd_para.__vhost == NULL || STRCMP( vhm_env->cmd_para.__vhost , == , "" ) )
			{
				printf( "*** ERROR : expect '--vhost' with action '-a start'\n" );
				return -7;
			}
			
			return -VhmAction_start( vhm_env );
		}
		else if( STRCMP( vhm_env->cmd_para._action , == , "stop" ) )
		{
			if( vhm_env->cmd_para.__vhost == NULL || STRCMP( vhm_env->cmd_para.__vhost , == , "" ) )
			{
				printf( "*** ERROR : expect '--vhost' with action '-a stop'\n" );
				return -7;
			}
			
			return -VhmAction_stop( vhm_env );
		}
		else if( STRCMP( vhm_env->cmd_para._action , == , "destroy" ) )
		{
			if( vhm_env->cmd_para.__vhost == NULL || STRCMP( vhm_env->cmd_para.__vhost , == , "" ) )
			{
				printf( "*** ERROR : expect '--vhost' with action '-a destroy'\n" );
				return -7;
			}
			
			return -VhmAction_destroy( vhm_env );
		}
		else if( STRCMP( vhm_env->cmd_para._action , == , "install_test" ) )
		{
			return -VhmAction_install_test( vhm_env );
		}
		else
		{
			printf( "*** ERROR : action[%s] invalid\n" , vhm_env->cmd_para._action );
			return -7;
		}
	}
	else
	{
		printf( "*** ERROR : cmd para action[%s] invalid\n" , vhm_env->cmd_para._action );
		return -7;
	}
}

int main( int argc , char *argv[] )
{
	struct VhmEnvironment	*vhm_env = NULL ;
	
	int			nret = 0 ;
	
	nret = CreateVhmEnvironment( & vhm_env ) ;
	if( nret )
	{
		printf( "*** ERROR : CreateVhmEnvironment failed[%d]\n" , nret );
		return -nret;
	}
	
	if( argc == 1 )
	{
		usage();
		return 0;
	}
	
	nret = ParseCommandParameters( vhm_env , argc , argv ) ;
	if( nret )
	{
		printf( "*** ERROR : ParseCommandParameters failed[%d]\n" , nret );
		return -nret;
	}
	
	SetLogcFile( "%s/log/vhm.log" , getenv("HOME") );
	SetLogcLevel( LOG_DEBUG );
	
	NOTICELOGC( "---  vhm start --- v%s build %s %s" , _OPENVH_VERSION , __DATE__ , __TIME__ )
	
	nret = ExecuteCommandParameters( vhm_env ) ;
	DestroyVhmEnvironment( & vhm_env );
	NOTICELOGC( "---  vhm end --- v%s build %s %s" , _OPENVH_VERSION , __DATE__ , __TIME__ )
	if( nret )
	{
		printf( "*** ERROR : ExecuteCommandParameters failed[%d]\n" , nret );
		return -nret;
	}
	
	return 0;
}

