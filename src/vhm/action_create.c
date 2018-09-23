#include "vhm_in.h"

static unsigned char	stack_bottom[ 1024 * 1024 ] = "" ;

static int VHostEntry( void *p )
{
	struct VhmEnvironment	*vhm_env = (struct VhmEnvironment *)p ;
	
	sethostname( vhm_env->cmd_para.__host_name , strlen(vhm_env->cmd_para.__host_name) );
	
	
	return 0;
}

int VhmAction_Create( struct VhmEnvironment *vhm_env )
{
	char		vhost_path_base[ PATH_MAX ] ;
	char		path[ PATH_MAX ] ;
	char		path2[ PATH_MAX ] ;
	
	int		nret = 0 ;
	
	/* create vhost */
	nret = SnprintfAndMakeDir( vhost_path_base , sizeof(vhost_path_base)-1 , "%s/%s" , vhm_env->vhosts_path_base , vhm_env->cmd_para.__vhost ) ;
	if( nret )
	{
		ERRORLOGC( "SnprintfAndMakeDir[%s] failed[%d] , errno[%d]" , vhost_path_base , nret , errno );
		return -1;
	}
	
	nret = WriteFileLine( vhm_env->cmd_para.__host_name , path , sizeof(path)-1 , "%s/hostname" , vhost_path_base ) ;
	if( nret )
	{
		ERRORLOGC( "WriteFileLine hostname failed[%d] , errno[%d]" , nret , errno );
		return -1;
	}
	
	nret = SnprintfAndMakeDir( path , sizeof(path)-1 , "%s/rwlayer" , vhost_path_base ) ;
	if( nret )
	{
		ERRORLOGC( "SnprintfAndMakeDir[%s] failed[%d] , errno[%d]" , path , nret , errno );
		return -1;
	}
	
	nret = SnprintfAndMakeDir( path2 , sizeof(path2)-1 , "%s/root" , path ) ;
	if( nret )
	{
		ERRORLOGC( "SnprintfAndMakeDir[%s] failed[%d] , errno[%d]" , path2 , nret , errno );
		return -1;
	}
	
	nret = SnprintfAndMakeDir( path , sizeof(path)-1 , "%s/merged" , vhost_path_base ) ;
	if( nret )
	{
		ERRORLOGC( "SnprintfAndMakeDir[%s] failed[%d] , errno[%d]" , path , nret , errno );
		return -1;
	}
	
	nret = SnprintfAndMakeDir( path , sizeof(path)-1 , "%s/workdir" , vhost_path_base ) ;
	if( nret )
	{
		ERRORLOGC( "SnprintfAndMakeDir[%s] failed[%d] , errno[%d]" , path , nret , errno );
		return -1;
	}
	
	/* mount filesystem */
	
	/* create vhost */
	nret = clone( VHostEntry , stack_bottom+sizeof(stack_bottom) , CLONE_NEWNS|CLONE_NEWPID|CLONE_NEWIPC|CLONE_NEWUTS|CLONE_NEWNET , (void*)vhm_env ) ;
	if( nret == -1 )
	{
		FATALLOGC( "clone failed[%d] , errno[%d]" , nret , errno );
		return -1;
	}
	
	while(1)
	{
		nret = wait( NULL ) ;
		if( nret == -1 )
			break;
	}
	
	/* unmount filesystem */
	
	return 0;
}

