#include "vhm_in.h"

static char	install_bin[][10] = { "bash" , "clear" , "ps" , "grep" , "more"
					, "ls" , "cat" , "echo" , "rm"
					, "pwd" , "cd" , "mkdir" , "rmdir" } ;

int VhmAction_install_test( struct VhmEnvironment *vhm_env )
{
	char		vtemplate_path_base[ PATH_MAX ] ;
	char		vtemplate_rlayer_path[ PATH_MAX ] ;
	int		i ;
	char		cmd[ 4096 ] ;
	
	int		nret = 0 ;
	
	/* create vtemplate */
	nret = SnprintfAndMakeDir( vtemplate_path_base , sizeof(vtemplate_path_base)-1 , "%s/test" , vhm_env->vtemplates_path_base ) ;
	if( nret )
	{
		printf( "SnprintfAndMakeDir vtemplate_path_base failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	
	nret = SnprintfAndMakeDir( vtemplate_rlayer_path , sizeof(vtemplate_rlayer_path)-1 , "%s/rlayer" , vtemplate_path_base ) ;
	if( nret )
	{
		printf( "SnprintfAndMakeDir rlayer failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	
	nret = SnprintfAndMakeDir( NULL , -1 , "%s/root" , vtemplate_rlayer_path ) ;
	if( nret )
	{
		printf( "SnprintfAndMakeDir /root failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	
	nret = SnprintfAndMakeDir( NULL , -1 , "%s/bin" , vtemplate_rlayer_path ) ;
	if( nret )
	{
		printf( "SnprintfAndMakeDir /bin failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	
	for( i = 0 ; i < sizeof(install_bin)/sizeof(install_bin[0]) ; i++ )
	{
		nret = SnprintfAndSystem( cmd , sizeof(cmd)-1 , "cp /bin/%s %s/bin/" , install_bin[i] , vtemplate_rlayer_path ) ;
		if( nret )
		{
			printf( "SnprintfAndSystem[%s] failed\n" , install_bin[i] );
			return -1;
		}
	}
	
	SnprintfAndChangeDir( NULL , -1 , "%s/bin" , vtemplate_rlayer_path ) ;
	system( "vhm_ldd_and_cp_lib64.sh" );
	
	nret = SnprintfAndMakeDir( NULL , -1 , "%s/etc" , vtemplate_rlayer_path ) ;
	if( nret )
	{
		printf( "SnprintfAndMakeDir /etc failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	
	nret = WriteFileLine( "#!/bin/bash\n"
				"export PATH=/bin\n"
				"export LC_ALL=C\n"
				"export LANG=C\n"
				, NULL , -1 , "%s/etc/profile" , vtemplate_rlayer_path ) ;
	if( nret )
	{
		printf( "*** ERROR : WriteFileLine profile failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	
	nret = SnprintfAndMakeDir( NULL , -1 , "%s/proc" , vtemplate_rlayer_path ) ;
	if( nret )
	{
		printf( "SnprintfAndMakeDir /proc failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	
	nret = SnprintfAndMakeDir( NULL , -1 , "%s/dev" , vtemplate_rlayer_path ) ;
	if( nret )
	{
		printf( "SnprintfAndMakeDir /dev failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	
	nret = SnprintfAndMakeDir( NULL , -1 , "%s/dev/pts" , vtemplate_rlayer_path ) ;
	if( nret )
	{
		printf( "SnprintfAndMakeDir /dev/pts failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	
	return 0;
}

