#include "vhm_in.h"

int VhmAction_destroy( struct VhmEnvironment *vhm_env )
{
	char		vhost_path_base[ PATH_MAX ] ;
	char		rwlayer_path[ PATH_MAX ] ;
	char		hostname_path[ PATH_MAX ] ;
	char		cmd[ PATH_MAX ] ;
	
	int		nret = 0 ;
	
	/* destroy vhost */
	nret = SnprintfAndMakeDir( vhost_path_base , sizeof(vhost_path_base)-1 , "%s/%s" , vhm_env->vhosts_path_base , vhm_env->cmd_para.__vhost ) ;
	if( nret )
	{
		ERRORLOGC( "SnprintfAndMakeDir[%s] failed[%d] , errno[%d]" , vhost_path_base , nret , errno );
		return -1;
	}
	
	nret = snprintf( rwlayer_path , sizeof(rwlayer_path)-1 , "%s/rwlayer" , vhost_path_base ) ;
	if( SNPRINTF_OVERFLOW( nret , sizeof(rwlayer_path)-1 ) )
	{
		printf( "snprintf failed\n" );
		return -1;
	}
	
	nret = snprintf( hostname_path , sizeof(hostname_path)-1 , "%s/hostname" , vhost_path_base ) ;
	if( SNPRINTF_OVERFLOW( nret , sizeof(hostname_path)-1 ) )
	{
		printf( "snprintf failed\n" );
		return -1;
	}
	
	if( access( rwlayer_path , W_OK ) == 0 && access( hostname_path , W_OK ) == 0 )
	{
		memset( cmd , 0x00 , sizeof(cmd) );
		snprintf( cmd , sizeof(cmd)-1 , "rm -rf %s" , vhost_path_base );
printf( "system[%s]\n" , cmd );
		// system( cmd );
	}
	
	return 0;
}

