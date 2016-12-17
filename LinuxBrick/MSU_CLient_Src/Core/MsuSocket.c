#ifndef __MSU_SOCKET_CPP__
#define __MSU_SOCKET_CPP__

#include "MsuPlatformInterface.h"
#include "MsuSocketInterface.h"
#include "MsuPdu.h"
#include "MsuSocket.h"
#include "MsuComplainMode.h"
#include "MsuLeech.h"

/*Use for listening mode*/
static MSU_SOCKET msuDefaultSocket;
/*Used in update process*/
static MSU_SOCKET msuUpdateSocket;
/* Hold data in Big Endian Format*/
msuSockinfo destDeviceAddr;

MSUAuthenticationRequestPdu pAuthenticationRequestPdu;
MSU_EXTERN MSUAuthenticationRequestPdu g_Header ;
static MSU_UINT8 g_msgBuffer[MSU_MAX_PACKET_SIZE];

MSU_EXTERN void setTimeoutVal( MSU_SINT32 socketfd, MSU_UINT8 timeout_sec );
MSU_EXTERN MSU_UINT32 msuIpfromStr(MSU_SINT8* ipStr);

MSU_UINT32 clientIP;

MSU_EXTERN MSU_UINT8   MsuGlobalError; 

MSU_BOOL msuSendDiscoveryResponse(MSUDiscoveryRequestPdu *discoveryRequestPdu);
MSU_BOOL msuSendAuthenticationResponse(MSUAuthenticationRequestPdu *pAuthenticationRequestPdu,MSU_UINT32 msgSize);
/*MSU_BOOL msuAuthenticationRequest(MSU_UINT8 *pMsg,MSU_UINT32 msgSize,MSUAuthenticationRequestPdu *pAuthenticationRequest,
							  MSUAuthenticationResponsePdu *msuAuthenticationResponse, msuSockinfo *socketip );*/
							  
static void (*ftprChangeDeviceState)(EMSUState state);


/**  @fn void msuSocketInit(void)
 *   @brief msuSocketInit() function is used to intialize variables 
 *   @param[in] : None
 *   @return    : None
 */
void msuSocketInit(void)
{
   msuDefaultSocket = MSU_INVALID_SOCKET;
	msuUpdateSocket = MSU_INVALID_SOCKET;
   msuMemorySet(g_msgBuffer, 0, (sizeof(MSU_UINT8) * MSU_MAX_PACKET_SIZE));
}


/**  @fn void msuRegisterCallBackMethod(MSU_UINT8 callBackType, void (*fptr)(EMSUState),void* arg)
 *   @brief msuRegisterCallBackMethod() is used to register a function, This function will be called when ever socket 
 *          receives a abort message from server   
 *   @param[in] : MSU_UINT8 callBackType          Unused variable
 *   @param[in] : void (*fptr)(EMSUState)     function pointer which you want to register function
 *   @param[in] : void* arg                   arg not used 
 *   @return    : None
 */
void msuRegisterCallBackMethod(MSU_UINT8 callBackType, void (*fptr)(EMSUState),void* arg)
{
	ftprChangeDeviceState = fptr;
}


/**  @fn MSU_BOOL MSU_SINT32 msuDefaultSocketCreate()
 *   @details  msuDefaultSocketCreate() function is used to create a default socket with default port number
 *             and multicast Ip address.
 *   @param[in] : None
 *   @return    : MSU_SOCKET_SUCCESS, if default socket created successful, otherwise MSU_SOCKET_ERROR
 *   @retval  0  if successfu
 *   @retval  -1 if failed 
 */
MSU_SINT32 msuDefaultSocketCreate()
{
   MSU_SINT32 status = MSU_SOCKET_SUCCESS;

	msuDefaultSocket = msuSocketCreate(MSU_MCAST_SYNC_PORT,msuIpfromStr(MSU_MCAST_DEFAULT_IP));
	if(msuDefaultSocket == MSU_INVALID_SOCKET)
	{
		msuLogMsg(MSU_DEBUG_LEVEL_CRITICAL, "\nSetting up Default socket failed for port = %d\n",	MSU_MCAST_SYNC_PORT);
		status = MSU_SOCKET_ERROR;
	}
	return status;
}

/**  @fn MSU_SINT32 msuUpdateSocketCreate(MSU_UINT16 port, MSU_UINT32 mIpAddr)
 *   @details  msuUpdateSocketCreate( ) function is used to create a updated socket with port number and 
 *             multicast Ip address received in Sync message
 *   @param[in] : MSU_UINT16 port     socket port number
 *   @param[in] : MSU_UINT32 mIpAddr  Multicast Ip Address
 *   @return    : MSU_SOCKET_SUCCESS, if socket created successful, otherwise MSU_SOCKET_ERROR
 *                 
 *   @retval  0  if successfu
 *   @retval  -1 if failed 
 */
MSU_SINT32 msuUpdateSocketCreate(MSU_UINT16 port, MSU_UINT32 mIpAddr)
{
   MSU_SINT32 status = MSU_SOCKET_SUCCESS;

	msuUpdateSocket = msuSocketCreate(port, mIpAddr);
	if(msuUpdateSocket == MSU_INVALID_SOCKET)
	{
		msuLogMsg( MSU_DEBUG_LEVEL_CRITICAL,"\nSetting up Updated socket failed for port = %d\n", MSU_MCAST_SYNC_PORT);
		status = MSU_SOCKET_ERROR;
	}
	else
	{
		setTimeoutVal(msuUpdateSocket,g_msuBroadcastPdu.msuSyncHeaderformat.updateTimeout);
	}
	return status;
}

/**  @fn MSU_BOOL msuSocketCheckActivity(void)
 *   @details  msuSocketCheckActivity( ) function internally calls msuSocketGetPacket() function to check activities
 *             on defualt socket.
 *   @param[in] : None
 *   @return    : MSU_TRUE or MSU_FALSE, 
 *                 
 *   @retval  1  if successful
 *   @retval  0 if failed 
 */
MSU_BOOL msuSocketCheckActivity(void)
{
	return  msuSocketGetPacket();
}


/**  @fn void msuSocketProcessDiscoveryMessage(MSU_UINT8 *pMsg, MSU_UINT32 msgSize)
 *   @details  msuSocketProcessDiscoveryMessage( ) function is responsible for handling Discovery request, validating 
 *             discovery request and sending discovery response to the server
 *   @param[in] : MSU_UINT8 *pMsg   discovery request message buffer
 *   @param[in] : MSU_UINT32 msgSize  size of discovery request message
 *   @return    : None
 *   @retval      None
 */
void msuSocketProcessDiscoveryMessage(MSU_UINT8 *pMsg, MSU_UINT32 msgSize)
{
   MSUDiscoveryRequestPdu discoverRequest;

   if (msuParseDiscoverRequest(pMsg, msgSize, &discoverRequest)== MSU_TRUE)
	   msuSendDiscoveryResponse(&discoverRequest);

}


/**  @fn MSU_BOOL msuSocketGetPacket()
 *   @details  msuSocketGetPacket( ) function uses default socket to receive messages in blocking mode, 
 *             once it receives a message it validates to check what type of message, it may be Discovery request message
 *             or Sync message or Device status update request message or group creation message, depends on message type
 *             it calls related functions to handle that type of request
 *   @param[in] : None
 *   @return    : MSU_TRUE, if it received Sync message and validtion passed,, otherwise MSU_FALSE
 *   @retval  1  if successfully  Sync message received and validation passed
 *   @retval  0  Handling other than Sync message 
 */
MSU_BOOL msuSocketGetPacket()
{
   MSU_BOOL status = MSU_FALSE;
 	MSU_UINT32 nByte = 0;
 	MSUCmd command = {0};
 	MSUHeader *msuHeader;

   nByte = msuSocketRecvFrom( msuDefaultSocket, g_msgBuffer, MSU_MAX_PACKET_SIZE,&destDeviceAddr);
	if (nByte != MSU_SOCKET_ERROR)
	{
	   /*Get the command type*/
		msuHeader = (MSUHeader*)g_msgBuffer;
		if(isMsuVersion(msuHeader)== MSU_FALSE) return 0;

		command = *((MSUCmd *)g_msgBuffer);
		/*Process the messages based on the message type*/
		if(command.opcode == MSU_OPCODE_DEVICE_DISCOVER && command.subcode == MSU_SUBCODE_DEVICE_DISCOVERY_REQUEST)
		{
			msuLogMsg(MSU_DEBUG_LEVEL_SUCCESS,  "\n==> Discovery request\n");
			msuSocketProcessDiscoveryMessage(g_msgBuffer, nByte);
		}
		else if (command.opcode == MSU_OPCODE_AUTHENTICATION && command.subcode == MSU_SUBCODE_AUTHENTICATION_REQUEST)
		
		{
			msuSendAuthenticationResponse(&pAuthenticationRequestPdu,nByte);
			
		}
		
	    else if(command.opcode == MSU_OPCODE_AUTHENTICATION && command.subcode == MSU_SUBCODE_DISCONNECT_REQUEST)
		{
			msuPasrseDisconnectMessage(g_msgBuffer,nByte);
		}
		
		else if ((command.opcode == MSU_OPCODE_UPGRADE) && ( (command.subcode == MSU_SUBCODE_UPGRADE)||
		   		 (command.subcode == MSU_SUBCODE_FORCE_UPGRADE) || (command.subcode == MSU_SUBCODE_DOWNGRADE))
				  )
		{
			msuLogMsg( MSU_DEBUG_LEVEL_SUCCESS, "\n==> Sync Message\n");
					 
			/*Process Sync Message*/
			status = msuSocketProcessSyncMessage(g_msgBuffer, nByte);
			if(status == MSU_TRUE)
			{	 /*Close the previously opened sockets*/
				msuSocketClose(msuDefaultSocket);
			}
		}
		else if ( (command.opcode == MSU_OPCODE_UPGRADE) && (command.subcode == MSU_SUBCODE_CLIENT_STATUS_REQUEST) )
		{
		   if (msuHandleDeviceStatusUpdateRequest((char*)g_msgBuffer, (MSU_UINT32)nByte) == MSU_TRUE)
			   msuSendUpgradeStatusMsg(MSU_TRUE);
		   
		}
		else if(command.opcode == MSU_OPCODE_DEVICE_GROUP)
		{
		   if(msuHandleDeviceGroupMsg(g_msgBuffer, (MSU_UINT32)nByte) == MSU_TRUE)
			   msuSendGroupCreationAckMsg( g_msgBuffer);
		}
	}
	return status;
}


/**  @fn MSU_BOOL msuSocketGetDataPackets( )
 *   @details  This function is used to receive data packets on updated socket in non blocking mode
 *             This received packets may be a actual data packets or Transfer complete request packet or
 *             CCM Transfer complete request or Abort request, this function calls other functions to handle 
 *             these requests. It calls msuPduParseMcastRequest( ) to handle Data transfer request, calls 
 *             msuPduHandleTransferCompleteRequest( ) for Tranfer complete request handling, calls 
 *             msuPduHandleCCMCompleteRequest() for CCM transfer complete request handling and call registered function
 *             handler for abort request
 *   @param[in] : None
 *   @return    : function returns MSU_TRUE or MSU_FALSE state from the requested calls
 */

MSU_BOOL msuSocketGetDataPackets( )
{
   MSU_SINT32 recvStatus  = MSU_SOCKET_SUCCESS;
 	MSU_BOOL status = MSU_FALSE;
 	MSUCmd command = {0};

 	if(msuUpdateSocket == MSU_INVALID_SOCKET) return status;

 	recvStatus = msuNonBlockingSocketRecvFrom(msuUpdateSocket, g_msgBuffer, MSU_MAX_PACKET_SIZE ,&destDeviceAddr);
 
 	if ( (recvStatus != MSU_SOCKET_TIMEOUT_ERROR) && (recvStatus != MSU_SOCKET_RECEIVE_ERROR) )
 	{
 	   /*Get the command type*/
	 	command = *((MSUCmd *)g_msgBuffer);
	 	if( command.opcode == MSU_OPCODE_UPGRADE )
	 	{
	 	   /*Process the multicast packet*/
		 	switch(command.subcode)
		 	{
		 		case MSU_SUBCODE_DATA_TRANSFER:
		 		case MSU_SUBCODE_UPGRADE:
			 		status = msuPduParseMcastRequest(g_msgBuffer, (MSU_UINT32) recvStatus);
		 			break;

		 		case MSU_SUBCODE_TRANSFER_COMPLETE:
		 			status = msuPduHandleTransferCompleteRequest( );//g_msgBuffer, (MSU_UINT32) recvStatus
		 			break;


		 		case MSU_SUBCODE_SCM_TRANSFER_COMPLETED:
		 			status = msuPduHandleSCMCompleteRequest(g_msgBuffer, (MSU_UINT32) recvStatus);
		 			break;

		 		case MSU_SUBCODE_CCM_TRANSFER_COMPLETED:
		 			status = msuPduHandleCCMCompleteRequest(g_msgBuffer, (MSU_UINT32) recvStatus);
		 			break;

		 		case MSU_SUBCODE_ABORT_TRANSFERT:
			 		ftprChangeDeviceState(eMsuStateAbort);
			 		status = MSU_FALSE;
		 			break;

		 		default:
		 			status = MSU_FALSE;
		 	}
	 	}
 	}
 	else if( recvStatus == MSU_SOCKET_TIMEOUT_ERROR)
 	{
	   EMSUState currState = getCurrentMsuState( );
	   if( currState == eMsuStateRunning) 
	  	   MsuGlobalError = MSU_UPDATE_TIMEOUT;
	   if( currState == eMsuStateSCM) 
	  	   MsuGlobalError = MSU_UPDATE_SCM_FAILED;
	   if( currState == eMsuStateCCM) 
	  	   MsuGlobalError = MSU_UPDATE_CCM_FAILED;
	  
	   ftprChangeDeviceState(eMsuStateAbort);
 	}
 	else if( recvStatus == MSU_SOCKET_RECEIVE_ERROR )
 	{
 		MsuGlobalError = MSU_UPDATE_SOCKET_RX_FAILED;
 	}
 return status;
}


/**  @fn void msuSocketRelease(void)
 *   @brief msuSocketRelease() function is used to close updated socket and default socket
 *   @param[in] : None
 *   @return    : None
 */
void msuSocketRelease(void)
{
   if(msuUpdateSocket != MSU_INVALID_SOCKET)
	{
		msuSocketClose(msuUpdateSocket);
		msuUpdateSocket = MSU_INVALID_SOCKET;
	}
	if(msuDefaultSocket != MSU_INVALID_SOCKET)
	{
		msuSocketClose(msuDefaultSocket);
		msuDefaultSocket = MSU_INVALID_SOCKET;
	}
}


/**  @fn MSU_BOOL msuSendScmMsg(MSU_UINT32 bitMap, MSU_UINT32 chunkNum, MSU_UINT8 missedSeqCount  )
 *   @details This function is used to send SCM message to server, it internally calls  
 *            msuScmMsgCreate( ) to construct a message and uses  msuSocketSendTo() to 
 *            send to the server 
 *   @param[in] : MSU_UINT32 bitMap      Sequence bit map value
 *   @param[in] : MSU_UINT32 chunkNum    Chunk number
 *   @param[in] : MSU_UINT8 missedSeqCount  missed sequence count number
 *   @return    : MSU_TRUE, if sent sucessful , otherwise returns MSU_FALSE
 *   @retval  1  if successfu
 *   @retval  0 if failed 
 */
MSU_BOOL msuSendScmMsg(MSU_UINT32 bitMap, MSU_UINT32 chunkNum, MSU_UINT8 missedSeqCount  )
{
	MSU_SINT32 sentStatus =  MSU_SOCKET_SUCCESS;
    MSU_BOOL status = MSU_FALSE;
    msuSockinfo destAddr;

    MSUSCMPdu *scmMsgPdu = (MSUSCMPdu *) g_msgBuffer;

    msuScmMsgCreate( scmMsgPdu, bitMap, chunkNum, missedSeqCount );

    destAddr.port = g_msuBroadcastPdu.msuSyncHeaderformat.multicastPort;
    destAddr.ipAddress = destDeviceAddr.ipAddress;
    sentStatus = msuSocketSendTo( msuUpdateSocket, scmMsgPdu, sizeof(MSUSCMPdu), &destAddr );

    if(sentStatus != MSU_SOCKET_ERROR )
    	status = MSU_TRUE;
    else
       MsuGlobalError = MSU_UPDATE_SOCKET_TX_FAILED;

	return status;
}


/**  @fn MSU_BOOL msuSendDiscoveryResponse(MSUDiscoveryRequestPdu *discoveryRequestPdu)
 *   @details msuSendDiscoveryResponse( ) function is used send discovery response message
 *            to the client, it internally calls msuDiscoveryResponseMsgCreate() to construct
 *            response message and uses msuSocketSendTo() to send to server
 *   @param[in] : MSUDiscoveryRequestPdu *discoveryRequestPdu   pointer to a discovery request Pdu
 *   @return    : MSU_TRUE, if sent sucessful , otherwise returns MSU_FALSE
 *   @retval  1  if successfu
 *   @retval  0 if failed 
 */
MSU_BOOL msuSendDiscoveryResponse(MSUDiscoveryRequestPdu *discoveryRequestPdu)
{
   MSU_SINT32 sentStatus =  MSU_SOCKET_SUCCESS;
   MSU_BOOL status = MSU_FALSE;
   MSUDiscoveryResponsePdu *msuDiscoveryResponse = (MSUDiscoveryResponsePdu *)g_msgBuffer;
   MSU_UINT16 dataPayloadSize = 0;

   msuDiscoveryResponseMsgCreate( discoveryRequestPdu, msuDiscoveryResponse,&dataPayloadSize );

   sentStatus = msuSocketSendTo( msuDefaultSocket, g_msgBuffer,
    		             				(sizeof(MSUDiscoveryResponsePdu) + dataPayloadSize), &destDeviceAddr);

   if(sentStatus == MSU_SOCKET_ERROR )
	{
		msuLogMsg( MSU_DEBUG_LEVEL_ERROR, "\n Failed to send Discovery response Socket error ");
	}
	else
	{
		msuLogMsg( MSU_DEBUG_LEVEL_SUCCESS, "\n****Discovery response Sent*** !");
		status = MSU_TRUE;
	}
	return status;
}


MSU_BOOL msuSendAuthenticationResponse(MSUAuthenticationRequestPdu *pAuthenticationRequestPdu,MSU_UINT32 msgSize)
{
	
	MSU_SINT32 sentStatus =  MSU_SOCKET_SUCCESS;
   	MSU_BOOL status = MSU_FALSE;
	MSUAuthenticationResponsePdu *pAuthenticationResponsePdu=(MSUAuthenticationResponsePdu *)g_msgBuffer;
	
	msuAuthenticationRequest(g_msgBuffer,msgSize,pAuthenticationRequestPdu,pAuthenticationResponsePdu,&destDeviceAddr);
	/*Populating the global variable with the transactionId value recieved in the Authentication request*/             				
	
	sentStatus = msuSocketSendTo( msuDefaultSocket, g_msgBuffer,
    		             		(sizeof(MSUAuthenticationResponsePdu) ), &destDeviceAddr);
    	
    if(sentStatus == MSU_SOCKET_ERROR )
	{
		msuLogMsg( MSU_DEBUG_LEVEL_ERROR, "\n Failed to send Authentication response Socket error ");
	}
	{
		msuLogMsg( MSU_DEBUG_LEVEL_SUCCESS, "\n****Authentication response Sent*** !");
		status = MSU_TRUE;	
	}         
	return status;
}


/**  @fn MSU_BOOL msuSendUpgradeStatusMsg(MSU_BOOL isDefault)
 *   @details msuSendUpgradeStatusMsg( ) function is used send upgrade status message to the server
 *            it calls msuUpgrateMsgCreate() function to construct upgrade message format
 *   @param[in] : MSU_BOOL isDefault     pass MSU_TRUE if want to send message using default socket
 *                                   pass MSU_FALSE if want to send message using Updated socket     
 *   @return    : MSU_TRUE, if sent sucessful , otherwise returns MSU_FALSE
 *   @retval  1  if successfu
 *   @retval  0 if failed 
 */
MSU_BOOL msuSendUpgradeStatusMsg(MSU_BOOL isDefault)
{
   MSU_SINT32 sentStatus =  MSU_SOCKET_SUCCESS;
   MSU_BOOL status = MSU_FALSE;
   MSUUpgradeStatusPdu msuUpgradeStatus;
   EMSUUpgradeState upgradeStatus;
   MSU_UINT8 ccmRetryVal = 0;

   upgradeStatus = msuGetUpgradeProcessState();
   ccmRetryVal = getCcmRetryCount();
   msuUpgrateMsgCreate( &msuUpgradeStatus, (MSU_SINT32)upgradeStatus, ccmRetryVal );

   if(isDefault)
   {
      sentStatus = msuSocketSendTo( msuDefaultSocket, (MSUUpgradeStatusPdu*)&msuUpgradeStatus,
    															(sizeof(MSUUpgradeStatusPdu)), &destDeviceAddr );
   }
   else
   {
      sentStatus = msuSocketSendTo( msuUpdateSocket, (MSUUpgradeStatusPdu*)&msuUpgradeStatus,
    	    		             							(sizeof(MSUUpgradeStatusPdu)), &destDeviceAddr );
   }
   if(sentStatus == MSU_SOCKET_ERROR )
	{
		msuLogMsg( MSU_DEBUG_LEVEL_ERROR, "\n Failed to send upgrade status");		
	}
	else
	{
		msuLogMsg( MSU_DEBUG_LEVEL_SUCCESS, "\n****Upgrade status Sent*** !");
		status = MSU_TRUE;
	}
	return status;
}


/**  @fn MSU_BOOL msuSendCcmMsg( MSU_UINT32 totalChnksExpected  )
 *   @details msuSendCcmMsg( ) function is used send CCM message to server, it calls msuCcmMsgCreate( ) to 
 *            construct CCM message format and calls msuSocketSendTo( ) to send to the server
 *   @param[in] : MSU_UINT32 totalChnksExpected     total number of chunks in the transfered file
 *   @return    : MSU_TRUE if transfered successful, otherwise return MSU_FALSE
 *   @retval  1  if successfu
 *   @retval  0 if failed 
 */
MSU_BOOL msuSendCcmMsg( MSU_UINT32 totalChnksExpected  )
{
   MSU_SINT32 sentStatus =  MSU_SOCKET_SUCCESS;
	MSU_BOOL status = MSU_FALSE;
	MSU_UINT32 missedChnkCnt = 0;
	MSUCCMPdu *ccmMsgPdu = (MSUCCMPdu *)g_msgBuffer;

	msuSockinfo destAddr;

	msuCcmMsgCreate( ccmMsgPdu, totalChnksExpected, &missedChnkCnt );

	destAddr.port = g_msuBroadcastPdu.msuSyncHeaderformat.multicastPort;
	destAddr.ipAddress =destDeviceAddr.ipAddress;
	 //msuBroadcastPdu.msuSyncHeaderformat.multicastIP.ServerIPv4;
	sentStatus = msuSocketSendTo( msuUpdateSocket, (MSU_UINT8*)ccmMsgPdu, sizeof(MSUCCMPdu) + (sizeof(MSU_UINT32) * missedChnkCnt ),
																													&destAddr );

	if(sentStatus == MSU_SOCKET_ERROR )
	{
		msuLogMsg( MSU_DEBUG_LEVEL_ERROR, "\n Failed to send CCM Unicast");
		MsuGlobalError = MSU_UPDATE_SOCKET_TX_FAILED;		
	}
	else
	{
		msuLogMsg( MSU_DEBUG_LEVEL_SUCCESS, "\n****CCM Unicast Sent*** !");
		status = MSU_TRUE;
	}
	return status;
}



/**  @fn void msuSendGroupCreationAckMsg( MSU_UINT8 *gMsg  )
 *   @details msuSendGroupCreationAckMsg( ) function is used send group creation Ack message to the server,
 *            it calls msuGroupAckMsgCreate( ) f'n to construct Group creation Ack message format and calls msuSocketSendTo( ) to send to the server
 *   @param[in] : MSU_UINT32 totalChnksExpected     total number of chunks in the transfered file
 *   @return    : MSU_TRUE if transfered successful, otherwise return MSU_FALSE
 *   @retval  1  if successfu
 *   @retval  0 if failed 
 */

void msuSendGroupCreationAckMsg( MSU_UINT8 *gMsg  )
{
   MSU_SINT32 sentStatus =  MSU_SOCKET_SUCCESS;
	MSUGroupCreationPdu *gAckMsgPdu = MSU_NULL;
	MSUGroupCreationPdu *gReqPdu    = (MSUGroupCreationPdu *)gMsg;

	gAckMsgPdu = msuGroupAckMsgCreate( gReqPdu );
	sentStatus = msuSocketSendTo( msuDefaultSocket, (MSU_UINT8*)gAckMsgPdu, sizeof(MSUGroupCreationPdu), &destDeviceAddr );
	if(sentStatus == MSU_SOCKET_ERROR )
	{
		msuLogMsg( MSU_DEBUG_LEVEL_ERROR, "\n Failed to send Group Creation ACK Message ");
	}
}

/**  @fn void msuPasrseDisconnectMessage( MSU_UINT8 *gmsgBuffer,MSU_UINT16 msgSize  )
 *   @details msuPasrseDisconnectMessage( ) function is used parse disconnect message from the server server,
 *   @param[in] : MSU_UINT8 *g_msgBuffer     This points to the global buffer
 *   @param[in] : MSU_UINT16 msgSize    The size of the message read into the buffer
 *   @return    : MSU_TRUE if transfered successful, otherwise return MSU_FALSE
 *   @retval    : NONE
 */

void msuPasrseDisconnectMessage( MSU_UINT8 *gmsgBuffer ,MSU_UINT16 msgSize )
{
    
    MSUDisconnectPdu msuDisconnectpdu = {0};
    MSU_UINT8 returnStatus=0;
    /*Parse only if the global buffer pointer is not NULL*/
	if(gmsgBuffer!=MSU_NULL)
	{
		/*Parse the Disconnect Message and return the result*/
		returnStatus=msuDisconnectMsg(gmsgBuffer,msgSize,&msuDisconnectpdu);
		msuLogMsg( MSU_DEBUG_LEVEL_SUCCESS, "\n Tansaction Id %ld Return\n", g_Header.authHeader.transactionid.transactionid);
		/*On successful Disconnection */
		if(returnStatus!=MSU_TRUE)
		{
			//Set transaction ID to 0;
			g_Header.authHeader.transactionid.transactionid=0;
			msuLogMsg( MSU_DEBUG_LEVEL_SUCCESS, "\n Tansaction Id %ld Return\n",	g_Header.authHeader.transactionid.transactionid);
	
		}
		/*Return the response irrescpective of the Parsing outcome*/
		msuSendDisconnectResponse(&msuDisconnectpdu);
	}

}



/**  @fn MSU_BOOL msuSendDisconnectResponse( MSU_UINT8 *gMsg,MSU_UINT16 msgSize  )
 *   @details msuSendGroupCreationAckMsg( ) function is used send group creation Ack message to the server,
 *            it calls msuGroupAckMsgCreate( ) f'n to construct Group creation Ack message format and calls msuSocketSendTo( ) to send to the server
 *   @param[in] : MSU_UINT32 totalChnksExpected     total number of chunks in the transfered file
 *   @return    : MSU_TRUE if transfered successful, otherwise return MSU_FALSE
 *   @retval  1  if successfu
 *   @retval  0 if failed 
 */

MSU_BOOL msuSendDisconnectResponse(MSUDisconnectPdu *msuDisconnectpdu)
{	
	MSU_SINT32 sentStatus =  MSU_SOCKET_SUCCESS;
	MSUAuthenticationResponsePdu msuAutheticateresponse={0};
        MSU_BOOL retCode = MSU_TRUE;
	
	/*Set the Response PDU fields based on the Disconnect pdu*/
	msuAutheticateresponse.command.opcode=msuDisconnectpdu->command.opcode;
	msuAutheticateresponse.versionInfo.versionIP=msuDisconnectpdu->versionInfo.versionIP;
	msuAutheticateresponse.versionInfo.versionProtocol=msuDisconnectpdu->versionInfo.versionProtocol;
	
	/*Create the Response Message */
    msuDisconnectMsgResponseCreate( &msuAutheticateresponse );
    
    /*Send the response*/
	sentStatus = msuSocketSendTo( msuDefaultSocket, (MSU_UINT8*)&msuAutheticateresponse, sizeof(MSUAuthenticationResponsePdu), &destDeviceAddr );
	if(sentStatus == MSU_SOCKET_ERROR )
	{
		msuLogMsg( MSU_DEBUG_LEVEL_ERROR, "\n Failed to send Group Creation ACK Message ");
		retCode = MSU_FALSE;
	}
        return retCode;	
}

#endif /*#ifndef __MSU_SOCKET_CPP__*/
