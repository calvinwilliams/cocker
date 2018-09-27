#include "cocker_in.h"

int CreateCockerEnvironment( struct CockerEnvironment **pp_cocker_env )
{
	struct CockerEnvironment	*cocker_env = NULL ;
	
	char				netbr_name[ ETHERNET_NAME_MAX + 1 ] ;
	char				cmd[ 4096 ] ;
	int				len ;
	
	int				nret = 0 ;
	
	cocker_env = (struct CockerEnvironment *)malloc( sizeof(struct CockerEnvironment) ) ;
	if( cocker_env == NULL )
	{
		printf( "*** ERROR : malloc failed , errno[%d]\n" , errno );
		return 1;
	}
	memset( cocker_env , 0x00 , sizeof(struct CockerEnvironment) );
	
	if( getenv("COCKER_HOME" ) )
	{
		nret = SnprintfAndMakeDir( cocker_env->cocker_home , sizeof(cocker_env->cocker_home)-1 , "%s" , getenv("COCKER_HOME" ) ) ;
		if( nret )
		{
			printf( "*** ERROR : SnprintfAndMakeDir[%s] failed[%d]\n" , cocker_env->cocker_home , nret );
			free( cocker_env );
			return -1;
		}
	}
	else
	{
		nret = SnprintfAndMakeDir( cocker_env->cocker_home , sizeof(cocker_env->cocker_home)-1 , "/var/cocker" ) ;
		if( nret )
		{
			printf( "*** ERROR : SnprintfAndMakeDir[%s] failed[%d]\n" , cocker_env->cocker_home , nret );
			free( cocker_env );
			return -1;
		}
	}
	
	memset( netbr_name , 0x00 , sizeof(netbr_name) );
	len = snprintf( netbr_name , sizeof(netbr_name)-1 , "cocker0" ) ;
	if( SNPRINTF_OVERFLOW(len,sizeof(netbr_name)-1) )
	{
		printf( "*** ERROR : netbr name overflow\n" );
		free( cocker_env );
		return -1;
	}
	
	nret = SnprintfAndSystem( cmd , sizeof(cmd) , "brctl show | grep -E \"^%s\" >/dev/null 2>&1" , netbr_name ) ;
	if( nret )
	{
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "brctl addbr %s" , netbr_name ) ;
		if( nret )
		{
			printf( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
			free( cocker_env );
			return -1;
		}
		else if( cocker_env->cmd_para.__debug )
		{
			printf( "system [%s] ok\n" , cmd );
		}
		
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ifconfig %s 166.88.0.1" , netbr_name ) ;
		if( nret )
		{
			printf( "*** ERROR : system [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno );
			return -1;
		}
		else if( cocker_env->cmd_para.__debug )
		{
			printf( "system [%s] ok\n" , cmd );
		}
	}
	
	(*pp_cocker_env) = cocker_env ;
	
	return 0;
}

void DestroyCockerEnvironment( struct CockerEnvironment **pp_cocker_env )
{
	free( (*pp_cocker_env) );
	(*pp_cocker_env) = NULL ;
	
	return;
}

