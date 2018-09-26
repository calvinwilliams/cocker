#include "cocker_in.h"

static char	install_bin[][10] = { "bash"
					, "ls" , "cat" , "echo" , "rm"
					, "pwd" , "cd" , "mkdir" , "rmdir"
					, "clear" , "ps" , "grep" , "more" , "id" , "uname" , "hostname" , "vi" , "vim" , "awk" , "sed" , "tr"
					, "file" , "ldd"
					, "netstat" , "ping" , "telnet" , "nc" } ;
static char	install_sbin[][10] = { "ifconfig" , "route" , "ip" } ;

int DoAction_install_test( struct CockerEnvironment *cocker_env )
{
	char		image_path_base[ PATH_MAX ] ;
	char		image_rlayer_path[ PATH_MAX ] ;
	char		image_rlayer_root_path[ PATH_MAX ] ;
	char		image_rlayer_bin_path[ PATH_MAX ] ;
	char		image_rlayer_sbin_path[ PATH_MAX ] ;
	char		image_rlayer_etc_path[ PATH_MAX ] ;
	char		image_rlayer_etc_passwd_path[ PATH_MAX ] ;
	char		image_rlayer_etc_group_path[ PATH_MAX ] ;
	char		image_rlayer_proc_path[ PATH_MAX ] ;
	char		image_rlayer_dev_path[ PATH_MAX ] ;
	char		image_rlayer_dev_pts_path[ PATH_MAX ] ;
	int		i ;
	char		cmd[ 4096 ] ;
	
	int		nret = 0 ;
	
	/* create image */
	nret = SnprintfAndMakeDir( image_path_base , sizeof(image_path_base)-1 , "%s/test" , cocker_env->images_path_base ) ;
	if( nret )
	{
		printf( "SnprintfAndMakeDir image_path_base failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	else if( cocker_env->cmd_para.__debug )
	{
		printf( "mkdir %s ok\n" , image_path_base );
	}
	
	nret = SnprintfAndMakeDir( image_rlayer_path , sizeof(image_rlayer_path)-1 , "%s/rlayer" , image_path_base ) ;
	if( nret )
	{
		printf( "SnprintfAndMakeDir rlayer failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	else if( cocker_env->cmd_para.__debug )
	{
		printf( "mkdir %s ok\n" , image_rlayer_path );
	}
	
	nret = SnprintfAndMakeDir( image_rlayer_root_path , sizeof(image_rlayer_root_path) , "%s/root" , image_rlayer_path ) ;
	if( nret )
	{
		printf( "SnprintfAndMakeDir /root failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	else if( cocker_env->cmd_para.__debug )
	{
		printf( "mkdir %s ok\n" , image_rlayer_root_path );
	}
	
	nret = SnprintfAndMakeDir( image_rlayer_bin_path , sizeof(image_rlayer_bin_path) , "%s/bin" , image_rlayer_path ) ;
	if( nret )
	{
		printf( "SnprintfAndMakeDir /bin failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	else if( cocker_env->cmd_para.__debug )
	{
		printf( "mkdir %s ok\n" , image_rlayer_bin_path );
	}
	
	for( i = 0 ; i < sizeof(install_bin)/sizeof(install_bin[0]) ; i++ )
	{
		nret = SnprintfAndSystem( cmd , sizeof(cmd)-1 , "cp /bin/%s %s/bin/" , install_bin[i] , image_rlayer_path ) ;
		if( nret )
		{
			printf( "SnprintfAndSystem[%s] failed\n" , install_bin[i] );
			return -1;
		}
		else if( cocker_env->cmd_para.__debug )
		{
			printf( "system %s ok\n" , cmd );
		}
	}
	
	SnprintfAndChangeDir( NULL , -1 , "%s/bin" , image_rlayer_path ) ;
	system( "cocker_ldd_and_cp_lib64.sh" );
	
	nret = SnprintfAndMakeDir( image_rlayer_sbin_path , sizeof(image_rlayer_sbin_path) , "%s/sbin" , image_rlayer_path ) ;
	if( nret )
	{
		printf( "SnprintfAndMakeDir /sbin failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	else if( cocker_env->cmd_para.__debug )
	{
		printf( "mkdir %s ok\n" , image_rlayer_sbin_path );
	}
	
	for( i = 0 ; i < sizeof(install_sbin)/sizeof(install_sbin[0]) ; i++ )
	{
		nret = SnprintfAndSystem( cmd , sizeof(cmd)-1 , "cp /sbin/%s %s/sbin/" , install_sbin[i] , image_rlayer_path ) ;
		if( nret )
		{
			printf( "SnprintfAndSystem[%s] failed\n" , install_bin[i] );
			return -1;
		}
		else if( cocker_env->cmd_para.__debug )
		{
			printf( "system %s ok\n" , cmd );
		}
	}
	
	SnprintfAndChangeDir( NULL , -1 , "%s/sbin" , image_rlayer_path ) ;
	system( "cocker_ldd_and_cp_lib64.sh" );
	
	nret = SnprintfAndMakeDir( image_rlayer_etc_path , sizeof(image_rlayer_etc_path) , "%s/etc" , image_rlayer_path ) ;
	if( nret )
	{
		printf( "SnprintfAndMakeDir /etc failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	else if( cocker_env->cmd_para.__debug )
	{
		printf( "mkdir %s ok\n" , image_rlayer_etc_path );
	}
	
	nret = WriteFileLine( "root:x:0:0:root:/root:/bin/bash\n"
				, image_rlayer_etc_passwd_path , sizeof(image_rlayer_etc_passwd_path) , "%s/etc/passwd" , image_rlayer_path ) ;
	if( nret )
	{
		printf( "*** ERROR : WriteFileLine profile failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	else if( cocker_env->cmd_para.__debug )
	{
		printf( "write file %s ok\n" , image_rlayer_etc_passwd_path );
	}
	
	nret = WriteFileLine( "root:x:0:\n"
				, image_rlayer_etc_group_path , sizeof(image_rlayer_etc_group_path) , "%s/etc/group" , image_rlayer_path ) ;
	if( nret )
	{
		printf( "*** ERROR : WriteFileLine profile failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	else if( cocker_env->cmd_para.__debug )
	{
		printf( "write file %s ok\n" , image_rlayer_etc_group_path );
	}
	
	nret = SnprintfAndSystem( cmd , sizeof(cmd) , "cp /bin/cocker_etc_profile_template.sh %s/etc/profile" , image_rlayer_path ) ;
	if( nret )
	{
		printf( "*** ERROR : SnprintfAndSystem /etc/profile failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	else if( cocker_env->cmd_para.__debug )
	{
		printf( "system %s ok\n" , cmd );
	}
	
	nret = SnprintfAndSystem( cmd , sizeof(cmd) , "cp /bin/cocker_profile_template.sh %s/root/.profile" , image_rlayer_path ) ;
	if( nret )
	{
		printf( "*** ERROR : SnprintfAndSystem .profile failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	else if( cocker_env->cmd_para.__debug )
	{
		printf( "system %s ok\n" , cmd );
	}
	
	nret = SnprintfAndMakeDir( image_rlayer_proc_path , sizeof(image_rlayer_proc_path) , "%s/proc" , image_rlayer_path ) ;
	if( nret )
	{
		printf( "SnprintfAndMakeDir /proc failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	else if( cocker_env->cmd_para.__debug )
	{
		printf( "mkdir %s ok\n" , image_rlayer_proc_path );
	}
	
	nret = SnprintfAndMakeDir( image_rlayer_dev_path , sizeof(image_rlayer_dev_path) , "%s/dev" , image_rlayer_path ) ;
	if( nret )
	{
		printf( "SnprintfAndMakeDir /dev failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	else if( cocker_env->cmd_para.__debug )
	{
		printf( "mkdir %s ok\n" , image_rlayer_dev_path );
	}
	
	nret = SnprintfAndMakeDir( image_rlayer_dev_pts_path , sizeof(image_rlayer_dev_pts_path) , "%s/dev/pts" , image_rlayer_path ) ;
	if( nret )
	{
		printf( "SnprintfAndMakeDir /dev/pts failed[%d] , errno[%d]\n" , nret , errno );
		return -1;
	}
	else if( cocker_env->cmd_para.__debug )
	{
		printf( "mkdir %s ok\n" , image_rlayer_dev_pts_path );
	}
	
	printf( "OK\n" );
	
	return 0;
}

