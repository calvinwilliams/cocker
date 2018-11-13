/*
 * cocker - Container Machine Engine
 * author	: calvin
 * email	: calvinwilliams@163.com
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#include "cocker_in.h"

void GetEthernetNames( struct CockerEnvironment *env , char *container_id )
{
	char		ethername_postfix[ 10 + 1 ] ;
	
	memset( ethername_postfix , 0x00 , sizeof(ethername_postfix) );
	GenerateEthernamePostfix( container_id , ethername_postfix );
	
	memset( env->netns_name , 0x00 , sizeof(env->netns_name) );
	snprintf( env->netns_name , sizeof(env->netns_name) , "nns%.10s" , ethername_postfix );
	
	memset( env->netbr_name , 0x00 , sizeof(env->netbr_name) );
	snprintf( env->netbr_name , sizeof(env->netbr_name) , "cocker0" );
	
	memset( env->veth1_name , 0x00 , sizeof(env->veth1_name) );
	snprintf( env->veth1_name , sizeof(env->veth1_name) , "eth%.10s" , ethername_postfix );
	
	memset( env->veth0_name , 0x00 , sizeof(env->veth0_name) );
	snprintf( env->veth0_name , sizeof(env->veth0_name) , "vth%.10s" , ethername_postfix );
	
	memset( env->veth0_sname , 0x00 , sizeof(env->veth0_sname) );
	snprintf( env->veth0_sname , sizeof(env->veth0_sname) , "eth0" );
	
	return;
}

int SplitImageVersion( char *image_id , char *version , int version_bufsize )
{
	char	*p = NULL ;
	
	memset( version , 0x00 , version_bufsize );
	
	p = strchr( image_id , ':' ) ;
	if( p )
	{
		strncpy( version , p+1 , version_bufsize-1 );
		(*p) = '\0' ;
		
		return 1;
	}
	
	return 0;
}

