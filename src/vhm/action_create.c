#include "vhm_in.h"

/* for test
$ vhm -a create --vtemplate test --vhost test --host-name test
*/

int VhmAction_create( struct VhmEnvironment *vhm_env )
{
	char		vhost_path_base[ PATH_MAX ] ;
	
	int		nret = 0 ;
	
	/* create vhost */
	nret = SnprintfAndMakeDir( vhost_path_base , sizeof(vhost_path_base)-1 , "%s/%s" , vhm_env->vhosts_path_base , vhm_env->cmd_para.__vhost ) ;
	if( nret )
	{
		printf( "*** ERROR : SnprintfAndMakeDir / failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	
	nret = SnprintfAndMakeDir( NULL , -1 , "%s/rwlayer" , vhost_path_base ) ;
	if( nret )
	{
		printf( "*** ERROR : SnprintfAndMakeDir rwlayer failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	
	nret = SnprintfAndMakeDir( NULL , -1 , "%s/rwlayer/etc" , vhost_path_base ) ;
	if( nret )
	{
		printf( "*** ERROR : SnprintfAndMakeDir /etc failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	
	nret = WriteFileLine( vhm_env->cmd_para.__host_name , NULL , -1 , "%s/rwlayer/etc/hostname" , vhost_path_base ) ;
	if( nret )
	{
		printf( "*** ERROR : WriteFileLine /etc/hostname failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	
	nret = SnprintfAndMakeDir( NULL , -1 , "%s/merged" , vhost_path_base ) ;
	if( nret )
	{
		printf( "*** ERROR : SnprintfAndMakeDir merged failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	
	nret = SnprintfAndMakeDir( NULL , -1 , "%s/workdir" , vhost_path_base ) ;
	if( nret )
	{
		printf( "*** ERROR : SnprintfAndMakeDir workdir failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	
	if( vhm_env->cmd_para.__vtemplate )
	{
		nret = WriteFileLine( vhm_env->cmd_para.__vtemplate , NULL , -1 , "%s/vtemplates" , vhost_path_base ) ;
		if( nret )
		{
			printf( "*** ERROR : WriteFileLine vtemplates failed[%d] , errno[%d]\n" , nret , errno );
			return -1;
		}
	}
	else
	{
		nret = WriteFileLine( "" , NULL , -1 , "%s/vtemplates" , vhost_path_base ) ;
		if( nret )
		{
			printf( "*** ERROR : WriteFileLine vtemplates failed[%d] , errno[%d]\n" , nret , errno );
			return -1;
		}
	}
	
	return 0;
}

