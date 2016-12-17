/*
 * msuMain.c
 *
 *  Created on: 25-Aug-2011
 *      Author: root
 */

#include <stdio.h>
#include <string.h>
#include "MsuPlatformInterface.h"
#include "MsuFileInterface.h"
#include "MsuPdu.h"
#include "MsuSocket.h"
#include "MsuLeech.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

extern MSU_UINT32 clientIP;

int main(int argc, char *argv[])
{
	MSU_BOOL paramStatus = MSU_FALSE;

	memset((char*)&clientIP, 0, sizeof(clientIP)); 
	if(argc == 0)
	{
		printf("\nNo IP address provided\n");
	}
	else if(argc > 2)
	{
		printf("\nToo many parameters\n");
	}
	else if(argc == 2)
	{
		clientIP = inet_addr((char*)argv[1]);
		printf("\nClient IP address = %X\n", clientIP);
		paramStatus = MSU_TRUE;
	}
	if(paramStatus == MSU_TRUE)
	{
		msuLeechStateMachine();
	}

	return 0;
}

