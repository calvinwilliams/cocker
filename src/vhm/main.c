#include "vhm_in.h"

static void usage()
{
	printf( "USAGE : vhm -a create [ --vtemplate (vt_name) ] --vhost (vh_name) [ --host-name (host_name) ]\n" );
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
	
	if( vhm_env->cmd_para._action == NULL )
	{
		printf( "*** ERROR : expect cmd para '-a (action)'\n" );
		return -7;
	}
	else
	{
		if( STRCMP( vhm_env->cmd_para._action , == , "create" ) )
		{
			if ( STRCMP( vhm_env->cmd_para.__vhost , == , "" ) )
			{
				printf( "*** ERROR : expect '--vhost' with action '-a create'\n" );
				return -7;
			}
		}
		else if( STRCMP( vhm_env->cmd_para._action , == , "install_test" ) )
		{
			;
		}
		else
		{
			printf( "*** ERROR : cmd para action[%s] invalid\n" , vhm_env->cmd_para._action );
			return -7;
		}
	}
	
	return 0;
}

static int ExecuteCommandParameters( struct VhmEnvironment *vhm_env )
{
	if( STRCMP( vhm_env->cmd_para._action , == , "create" ) )
	{
		return -VhmAction_Create( vhm_env );
	}
	else if( STRCMP( vhm_env->cmd_para._action , == , "install_test" ) )
	{
		return -VhmAction_InstallTest( vhm_env );
	}
	else
	{
		return -9;
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
	else
	{
		printf( "OK\n" );
	}
	
	return 0;
}

