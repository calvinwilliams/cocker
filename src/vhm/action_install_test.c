#include "vhm_in.h"

int VhmAction_InstallTest( struct VhmEnvironment *vhm_env )
{
	char		vtemplate_path_base[ PATH_MAX ] ;
	char		path[ PATH_MAX ] ;
	
	int		nret = 0 ;
	
	/* create vtemplate */
	nret = SnprintfAndMakeDir( vtemplate_path_base , sizeof(vtemplate_path_base)-1 , "%s/test" , vhm_env->vtemplates_path_base ) ;
	if( nret )
	{
		ERRORLOGC( "SnprintfAndMakeDir[%s] failed[%d] , errno[%d]" , vtemplate_path_base , nret , errno );
		return -1;
	}
	
	nret = SnprintfAndMakeDir( path , sizeof(path)-1 , "%s/rlayer" , vtemplate_path_base ) ;
	if( nret )
	{
		ERRORLOGC( "SnprintfAndMakeDir[%s] failed[%d] , errno[%d]" , path , nret , errno );
		return -1;
	}
	
	return 0;
}

