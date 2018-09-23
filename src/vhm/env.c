#include "vhm_in.h"

int CreateVhmEnvironment( struct VhmEnvironment **pp_vhm_env )
{
	struct VhmEnvironment	*vhm_env = NULL ;
	
	int			nret = 0 ;
	
	vhm_env = (struct VhmEnvironment *)malloc( sizeof(struct VhmEnvironment) ) ;
	if( vhm_env == NULL )
	{
		printf( "*** ERROR : malloc failed , errno[%d]\n" , errno );
		return 1;
	}
	memset( vhm_env , 0x00 , sizeof(struct VhmEnvironment) );
	
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
		nret = SnprintfAndMakeDir( vhm_env->openvh_home , sizeof(vhm_env->openvh_home)-1 , "%s/openvh" , getenv("HOME") ) ;
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
	
	(*pp_vhm_env) = vhm_env ;
	
	return 0;
}

void DestroyVhmEnvironment( struct VhmEnvironment **pp_vhm_env )
{
	free( (*pp_vhm_env) );
	(*pp_vhm_env) = NULL ;
	
	return;
}

