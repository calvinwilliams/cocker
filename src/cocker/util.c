#include "cocker_in.h"

void GetEthernetNames( struct CockerEnvironment *env , char *container_id )
{
	memset( env->netns_name , 0x00 , sizeof(env->netns_name) );
	snprintf( env->netns_name , sizeof(env->netns_name) , "netns%s" , container_id );
	
	memset( env->netbr_name , 0x00 , sizeof(env->netbr_name) );
	snprintf( env->netbr_name , sizeof(env->netbr_name) , "cocker0" );
	
	memset( env->veth1_name , 0x00 , sizeof(env->veth1_name) );
	snprintf( env->veth1_name , sizeof(env->veth1_name) , "eth%s" , container_id );
	
	memset( env->veth0_name , 0x00 , sizeof(env->veth0_name) );
	snprintf( env->veth0_name , sizeof(env->veth0_name) , "veth%s" , container_id );
	
	memset( env->veth0_sname , 0x00 , sizeof(env->veth0_sname) );
	snprintf( env->veth0_sname , sizeof(env->veth0_sname) , "eth0" );
	
	return;
}

