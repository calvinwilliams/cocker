#include "cocker_in.h"

void GetEthernetName( char *container_id , struct CockerEnvironment *cocker_env )
{
	memset( cocker_env->netns_name , 0x00 , sizeof(cocker_env->netns_name) );
	snprintf( cocker_env->netns_name , sizeof(cocker_env->netns_name) , "netns%s" , cocker_env->container_id );
	
	memset( cocker_env->netbr_name , 0x00 , sizeof(cocker_env->netbr_name) );
	snprintf( cocker_env->netbr_name , sizeof(cocker_env->netbr_name) , "cocker0" );
	
	memset( cocker_env->eth_name , 0x00 , sizeof(cocker_env->eth_name) );
	snprintf( cocker_env->eth_name , sizeof(cocker_env->eth_name) , "eth%s" , cocker_env->container_id );
	
	memset( cocker_env->veth_name , 0x00 , sizeof(cocker_env->veth_name) );
	snprintf( cocker_env->veth_name , sizeof(cocker_env->veth_name) , "veth%s" , cocker_env->container_id );
	
	memset( cocker_env->veth_sname , 0x00 , sizeof(cocker_env->veth_sname) );
	snprintf( cocker_env->veth_sname , sizeof(cocker_env->veth_sname) , "eth0" );
	
	return;
}

