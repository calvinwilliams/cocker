/*
 * cocker - Container Machine Engine
 * author	: calvin
 * email	: calvinwilliams@163.com
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#include "cocker_util.h"

int writen( int sock , char *send_buffer , int send_len , int *p_sent_len )
{
	int	len , remain_len , sent_len ;
	
	remain_len = send_len ;
	sent_len = 0 ;
	
	while( remain_len > 0 )
	{
		len = write( sock , send_buffer+sent_len , remain_len ) ;
		if( len == -1 )
		{
			if( p_sent_len )
				(*p_sent_len) = sent_len ;
			return -1;
		}
		
		sent_len += len ;
		remain_len -= len ;
	}
	
	if( p_sent_len )
		(*p_sent_len) = sent_len ;
	
	return 0;
}

int readn( int sock , char *recv_buffer , int recv_len , int *p_received_len )
{
	int	len , remain_len ;
	
	remain_len = recv_len ;
	recv_len = 0 ;
	if( p_received_len )
		(*p_received_len) = 0 ;
	
	while( remain_len > 0 )
	{
		len = read( sock , recv_buffer+recv_len , remain_len ) ;
		if( len == 0 )
		{
			if( p_received_len )
				(*p_received_len) = recv_len ;
			return 1;
		}
		else if( len == -1 )
		{
			if( p_received_len )
				(*p_received_len) = recv_len ;
			return -1;
		}
		
		recv_len += len ;
		remain_len -= len ;
	}
	
	if( p_received_len )
		(*p_received_len) = recv_len ;
	
	return 0;
}

