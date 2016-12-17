/**	@file MsuPdu.cpp
 *	<TABLE>
 *	@brief Implementation of MSU Pdu
 *
 *	@copy
 *	<TABLE>
 *	<TR><TD> Copyright </TD></TR>
 *	<TR>
 *	<TD>Schneider Electric India (Pvt) Ltd.
 *	Copyright (c) 2009 - All rights reserved.
 *	</TR>
 *	<TR>
 *	<TD> No part of this document may be reproduced in any form without the express
 * 	written consent of Schneider Electric India (Pvt) Ltd.</TD>
 *	</TR>
 *	</TABLE>
 *
 *	@par
 *	@author Deepak G
 *	@par HISTORY
 *
 *	<TABLE>
 *	<TR>
 *	<TD>Author</TD>				<TD>Date</TD>		<TD>Description</TD>
 *	</TR>
 *	<TR>
 *	<TD>Deepak G</TD>	<TD>04-Oct-2011</TD>	<TD>First creation</TD>
 *	</TR>
 *
 * 	</TABLE>
 *	</TABLE>
 */

#include "MsuSocketInterface.h"
#include "MsuFileInterface.h"
#include "MsuPdu.h"
#include "MsuComplainMode.h"
#include "MsuLeech.h"


MSUSyncMessagePdu g_msuBroadcastPdu;
MSUMulticastPdu g_msuMcastPdu;
MSUDeviceInfo g_msuDeviceInfo;
MSUAuthenticationHeader g_Header;
MSUDeviceIdentification g_deviceIdentification;

MSU_COMM_PARAM   g_msuCommParam;

MSU_UINT8 gGroupID = 0;
MSU_UINT32 serverIP = 0;

MSU_EXTERN MSU_UINT32 clientIP;
MSU_EXTERN MSU_UINT8   MsuGlobalError; 

static void msuGetDeviceInfo(void);

/**  @fn msuPduInit(void)
 *   @brief msuPduInit() is used to Intialize pdu structures
 *   @details  msuPduInit() function is used Initialize pdu sync message and multicast message structures
 *             and variables. 
 *   @param[in] : None
 *   @return None
 *   @retval None
 */
void msuPduInit(void)
{   
    msuMemorySet((MSU_UINT8*)&g_msuBroadcastPdu, 0, sizeof(MSUSyncMessagePdu));
    msuMemorySet((MSU_UINT8*)&g_msuMcastPdu, 0, sizeof(MSUMulticastPdu));
	msuMemorySet((MSU_UINT8*)&g_msuDeviceInfo, 0, sizeof(MSUDeviceInfo));
	msuMemorySet((MSU_UINT8*)&g_Header, 0, sizeof(MSUAuthenticationHeader));
	msuMemorySet((MSU_UINT8*)&g_deviceIdentification, 0, sizeof(MSUDeviceIdentification));
	 
    serverIP = 0;
    gGroupID = 0;
    msuGetDeviceInfo();
}

/**  @fn isMsuVersion( MSUHeader *header)
 *   @brief isMsuVersion() is used to validate Msu Version
 *   @details  isMsuVersion() function is used to validate MSU IP version and Msu protocol version
 *   @param[in] : header - Is a pointer type of  MSUHeader structure which contains version information
 *   @return MSU_TRUE ot MSU_FALSE
 *           Error code if the operation is failed.
 *   @retval socket descriptor ( integer value) if created successfully
 *   @retval -1 if failed to create socket
 */
MSU_BOOL isMsuVersion( MSUHeader *header)
{
    MSU_BOOL result = MSU_FALSE;
	if( (header->versionInfo.versionIP == MSU_IPV4_VERSION) &&
	    (header->versionInfo.versionProtocol == MSU_PROTOCOL_VERSION))
	{
	    result = MSU_TRUE;
	}
	return result;
}

/**  @fn void msuDecryption(MSU_UINT8 *inputstring ,char *privatekeydecrypt,MSU_UINT32 lengthstring, MSU_UINT32 length_privatekey)
 *   @brief msuDecryption() is used to decrypt the encrypted data from server
 *   @details  msuGetDeviceInfo() function reads the encrypted input string 
 *             and private key string value from device.It applies the decrypted alogorithm
 *             and over writes the encrypted value of the input string with the decrypted
 *             string
 *   @param[in] : MSU_UINT8 *inputstring       Encrypted string
 *   @param[in] : char  *privatekeydecrypt Private key read from device configuration
 *   @param[in] : MSU_UINT32 lengthstring      Length of the inputstring
 *   @param[in] : MSU_UINT32 length_privatekey Length of the private key
 *   @return    : none
 */

MSU_SINT32 msuDecryption(MSU_UINT8 *inputstring ,MSU_SINT8 *privatekeydecrypt,MSU_UINT32 lengthstring, MSU_UINT32 length_privatekey)
{
	
	int i=0,j=0;
	MSU_UINT8 *stringtoascii = inputstring;
	MSU_UINT8 *privatekeytoascii = (MSU_UINT8*)privatekeydecrypt;
	
	// Loop will be executing till the length of input string and individual string will be decrypted. 	
	for (i=0;i<lengthstring; i++) 
	{		
		/* 1. When the ascii value of input string is less than private key ascii value 
		      add 128 to the input string ascii value.
		   2. Subtract the ascii value of input string from private key ascii value to get the decrypted string value*/   
		if (stringtoascii[i]<privatekeytoascii[j])
		{
			stringtoascii[i]=(MSU_UINT8)(stringtoascii[i]+(MSU_UINT8)128);
			stringtoascii[i]=(MSU_UINT8)(stringtoascii[i]-privatekeytoascii[j]);
		}		
		/* When ascii value of input string is greater than private key ascii value ,
		   Subtract the ascii value of input string from private key ascii value to get the decrypted string value*/ 
		else
		{			
			stringtoascii[i] = (MSU_UINT8) (stringtoascii[i]-privatekeytoascii[j]);
		
		}		
	
		if(++j == length_privatekey )
			j = 0;
		
	}
	return 0 ;			
}


/**  @fn void msuGetDeviceInfo(void)
 *   @brief msuGetDeviceInfo() is used to get the device information
 *   @details  msuGetDeviceInfo() function reads the device information from msu.bin file,
 *             msu.bin contains information like product name, hardware id,product Id,model name,
 *             vendor Id, firmware version number and device location information. If msu.bin is 
 *             not available in the device then it reads default configuration from device.
 *   @param[in] : None
 *   @return    : None
 */
void msuGetDeviceInfo(void)
{
        //MSU_SINT32 fileDes = FILE_DESC_ERROR;
  	MSU_UINT16 bytesToRead = 0;
  	MSU_UINT16 dataOffset = 0;
  	MSU_BOOL fileConfigStatus = MSU_FALSE;

	//Initialize the device information
  	msuMemorySet((MSU_UINT8*)&g_msuDeviceInfo, 0, sizeof(MSUDeviceInfo));
#if 0 // Need to move in other place ... this is not a core function it is platform dep.
  	//Try to get the device information from the device information binary
  	fileDes = msuFileOpen(MSU_DEVICE_INFO_FILE_NAME, eMsuFileR);
  	if(fileDes != FILE_DESC_ERROR)
  	{
  	   //File opened successfully, try to read the header information
	  	bytesToRead =  sizeof(MSUDeviceInfo);

		 //Read the authentication information data from the file
		  	if(msuFileRead(fileDes, (MSU_SINT8 *)&g_msuDeviceInfo, bytesToRead) == bytesToRead)
		  	{
			   fileConfigStatus = MSU_TRUE;
		  	}
	
	  

	  	msuFileClose(fileDes );
  	}//if(fileConfigStatus == MSU_TRUE)
#endif
  	//Load the default configuration in case the configuration extraction from the file failed
  	if(fileConfigStatus == MSU_FALSE)
  	{
  	
	  	
	   //Initialize the device information
	  	msuMemorySet((MSU_UINT8*)&g_msuDeviceInfo, 0, sizeof(MSUDeviceInfo));
	  	
	  	
		//Fill the Client/Slave status
		g_msuDeviceInfo.Client = MSU_CLIENT_STATUS;
		
		//Fill the Master status
		
		g_msuDeviceInfo.Master = MSU_MASTER_STATUS;
		
	    	
		//Fill the AutheticationTag status
		
		g_msuDeviceInfo.AutheticationTag = MSU_AUTHENTICATION_TAG;
		
		//Fill the AuthenticationLevelTag status
		
		g_msuDeviceInfo.AuthenticationLevelTag = MSU_AUTHETICATION_LEVEL_TAG;
		
		//Fill the MaxIPSupported status
		
		g_msuDeviceInfo.NumberOfIPSupported = MSU_MAX_IP_FIELDS;
		
		//Fill the MaxIPSupported status
		
		g_msuDeviceInfo.noOfFileSupported = MSU_MAX_FILES_SUPPORTED;
		
	    //Fill in only one IP fields
	   
	    g_msuDeviceInfo.IP[0].ServerIPv4=MSU_DEFAULT_SERVER_IP;
	   
	    	    
	    //Fill MAC fields	    
	    msuMemorySet(g_msuDeviceInfo.MAC,0,sizeof(g_msuDeviceInfo.MAC));
	    	
	    //Fill the Username status
		msuByteCopy((MSU_UINT8*)&g_msuDeviceInfo.Username,(MSU_UINT8*)MSU_DEFAULT_USERNAME,msuStrLen(MSU_DEFAULT_USERNAME));
	
		
		//Fill the Password status
	    msuByteCopy((MSU_UINT8*)&g_msuDeviceInfo.Password,(MSU_UINT8 *)MSU_DEFAULT_PASSWORD,msuStrLen(MSU_DEFAULT_PASSWORD));
	    
	    //Fill the Filename1 
	    msuByteCopy((MSU_UINT8*)&g_msuDeviceInfo.filename[0],(MSU_UINT8 *)MSU_DEFAULT_FILENAME,msuStrLen(MSU_DEFAULT_FILENAME));
	     
	     //Fill the Filename2
	    msuByteCopy((MSU_UINT8*)&g_msuDeviceInfo.filename[1],(MSU_UINT8 *)MSU_DEFAULT_FILENAME1,msuStrLen(MSU_DEFAULT_FILENAME1));
	     
	     //Fill the Filename3
	     
	    msuByteCopy((MSU_UINT8*)&g_msuDeviceInfo.filename[2],(MSU_UINT8 *)MSU_DEFAULT_FILENAME2,msuStrLen(MSU_DEFAULT_FILENAME2)); 
	      
	      
	     //Fill the key value
	     
	    msuByteCopy((MSU_UINT8*)&g_msuDeviceInfo.key,(MSU_UINT8 *)MSU_DEFAULT_KEY,msuStrLen(MSU_DEFAULT_KEY));  
	    
	       
	     //Fill the sequenceLimit  value
	     
	     g_msuDeviceInfo.sequenceLimit = MSU_MAX_SEQ_NUMBER;
	      
	     

   }
}


/**  @fn MSU_BOOL msuParseDiscoverRequest(MSU_UINT8 *pMsg , MSU_UINT32 msgSize, MSUDiscoveryRequestPdu *pDiscoverRequest)
 *   @brief msuParseDiscoverRequest() is used to validate device discovery request
 *   @details  When client receives Msu discovery request from server, client calls msuParseDiscoverRequest()
 *             function to validate the request and checks this device falls between the starting and ending 
 *             IP address range, if it falls then this function return true.
 *   @param[in] : MSU_UINT8 *pMsg   Device discovery request message
 *   @param[in] : MSU_UINT32 msgSize  size of discovery request message
 *   @param[in] : MSUDiscoveryRequestPdu *pDiscoverRequest    pointer to   MSUDiscoveryRequestPdu which will be
 *                updated with pMsg data  
 *   @return    : MSU_TRUE, if device falls in b/w starting and ending IP range, otherwise 0
 *                 
 *   @retval  1  if successfu
 *   @retval  0 if failed 
 */

MSU_BOOL msuParseDiscoverRequest(MSU_UINT8 *pMsg , MSU_UINT32 msgSize, MSUDiscoveryRequestPdu *pDiscoverRequest)
{
	MSU_BOOL result = MSU_FALSE;
	MSU_UINT32	 clientIPAddress = msuNtoHl(clientIP);  
   
    if (pMsg != MSU_NULL)
    {
		msuByteCopy((MSU_UINT8*)pDiscoverRequest, pMsg, sizeof(MSUDiscoveryRequestPdu));	   
   	
		if (!pDiscoverRequest->versionInfo.versionIP ) // IPv4 ==0 IPV6 =1
		{				
		
			if((msgSize == sizeof(MSUDiscoveryRequestPdu)) && (pDiscoverRequest != MSU_NULL))
			{
				
				pDiscoverRequest->ServerIpStart.ServerIPv4 = msuNtoHl(pDiscoverRequest->ServerIpStart.ServerIPv4);
				pDiscoverRequest->ServerIpEnd.ServerIPv4 = msuNtoHl(pDiscoverRequest->ServerIpEnd.ServerIPv4);

				if(((clientIPAddress >= pDiscoverRequest->ServerIpStart.ServerIPv4) && (clientIPAddress <=pDiscoverRequest-> ServerIpEnd.ServerIPv4)) ||
					 (clientIPAddress == pDiscoverRequest->ServerIpStart.ServerIPv4))
				{
					result = MSU_TRUE;
				
				}			
			}

		} // IPv4 ==0 IPV6 =1
	}
return result;
}



/**  @fn MSU_BOOL msuHandleDeviceStatusUpdateRequest(char *gMsgBuffer,  MSU_UINT32 nByte)
 *   @brief msuHandleDeviceStatusUpdateRequest() is
 *   @details  msuHandleDeviceStatusUpdateRequest() function is used to validate Device
 *             status Update request by checking the transaction Id of the MSU cycle received in sync message
 *             with received transaction Id received in Update Request, if both are equal then this function returns true.
 *             Transaction ID : Randomly generated number which uniquely represents one MSU cycle
 *   @param[in] : char *gMsgBuffer     Update request message buffer 
 *   @param[in] : MSU_UINT32 nByte         size of the message buffer 
 *   @return MSU_TRUE if successful , otherwise MSU_FALSE
 *   @retval 1 successful
 *   @retval 0 fail 
 */
MSU_BOOL msuHandleDeviceStatusUpdateRequest(MSU_SINT8 *gMsgBuffer,  MSU_UINT32 nByte)
{
   MSUUpgradeStatusPdu *statusMsg = MSU_NULL;

	if(gMsgBuffer == MSU_NULL) return MSU_FALSE;

	statusMsg = (MSUUpgradeStatusPdu *)gMsgBuffer;
	
	msuLogMsg(MSU_DEBUG_LEVEL_SUCCESS, "\nTransaction ID Status Message = %d\n",
					statusMsg->transactionID.transactionid);
	statusMsg->transactionID.transactionid = msuNtoHl(statusMsg->transactionID.transactionid);
   
	msuDecryption((MSU_UINT8*)statusMsg->transactionID.transactionencrypt,(MSU_SINT8*)&g_msuDeviceInfo.key,
                 sizeof(g_msuBroadcastPdu.msuSyncHeaderformat.transactionID.transactionid),
                 msuStrLen(g_msuDeviceInfo.key));
	
	if((g_Header.transactionid.transactionid) == (statusMsg->transactionID.transactionid))
		return MSU_TRUE;
	return MSU_FALSE;
}


/**  @fn MSU_BOOL msuHandleDeviceGroupMsg(MSU_UINT8 *pMsg , MSU_UINT32 msgSize)
 *   @brief msuHandleDeviceGroupMsg() is used to handle group creation request message
 *   @details  When client receives group creation message, it calls msuHandleDeviceGroupMsg( ) function
 *             which validates the message, if message is valid then it create a Group Id in the client device.
 *   @param[in] : MSU_UINT8 *pMsg         group creation request message buffer
 *   @param[in] : MSU_UINT32 msgSize      size of the message buffer
 *   @return MSU_TRUE, if group created successful, otherwise MSU_FALSE
 *   @retval 1  if successful
 *   @retval 0  if failed
 */
MSU_BOOL msuHandleDeviceGroupMsg(MSU_UINT8 *pMsg , MSU_UINT32 msgSize)
{
   MSU_BOOL result = MSU_TRUE;
 	MSUGroupCreationPdu *msuGroupPdu = MSU_NULL;

 	if((pMsg != MSU_NULL) && (msgSize == sizeof(MSUGroupCreationPdu)))
 	{
	   msuGroupPdu = (MSUGroupCreationPdu*)pMsg;
	 	if(msuGroupPdu->command.subcode == MSU_SUBCODE_SET_GROUP)
	 	{
		   gGroupID = msuGroupPdu->groupID;
	 	}
	 	else if(msuGroupPdu->command.subcode == MSU_SUBCODE_RESET_GROUP)
	 	{
		   gGroupID = 0;
	 	}
	 	else
	 	{
		   result =MSU_FALSE;
	 	}
 	}
 	return result;
}


/**  @fn MSU_BOOL msuSocketProcessSyncMessage(MSU_UINT8 *pMsg , MSU_UINT32 msgSize)
 *   @brief msuSocketProcessSyncMessage() is used process sync message received from server
 *   @details  When server sends Sync message with the information like file number, total number chunks, 
 *             sequence number limit,port number and multicast IP address and other parameters, this function
 *             stores all the information in local Sync message PDU and validates the pdu, if parameters are 
 *             valid then it returns MSU_TRUE 
 *   @param[in] : MSU_UINT8 *pMsg     Notification message ( Sync Message ) request message buffer
 *   @param[in] : MSU_UINT32 msgSize  size of the message buffer
 *   @return      MSU_TRUE, if validation is success, otherwise MSU_FALSE 
 *   @retval      1  for success
 *   @retval      0  for fail
 */
MSU_BOOL msuSocketProcessSyncMessage(MSU_UINT8 *pMsg , MSU_UINT32 msgSize)
{
   MSU_BOOL result = MSU_FALSE;
	MSU_UINT8 *pBuf = MSU_NULL;
   MSU_UINT32 tempPacketSize = 0;
   MSU_UINT16 tagDataSize = 0;
   MSUSyncHeaderformat *localmsuSyncHeaderformat;
   MSUSyncMessageVariableData *localmsuSyncMessageVariableData;
   
   if(pMsg!=MSU_NULL)
   {
   	
  
   
      pBuf=pMsg;
   
      localmsuSyncHeaderformat = (MSUSyncHeaderformat*)pBuf;
   
      localmsuSyncMessageVariableData=(MSUSyncMessageVariableData*)(pBuf+sizeof(MSUSyncHeaderformat));
   
      /* Reset the Global Variable*/
   
      msuMemorySet((MSU_UINT8*)&g_msuBroadcastPdu, 0, sizeof(MSUSyncMessagePdu));
   
      g_msuBroadcastPdu.msuSyncHeaderformat.transactionID.transactionid = msuNtoHl(localmsuSyncHeaderformat->transactionID.transactionid);
     /*
      msuDecryption((MSU_UINT8*)g_msuBroadcastPdu.msuSyncHeaderformat.transactionID.transactionencrypt,(char*)&g_msuDeviceInfo.key,
                 sizeof(localmsuSyncHeaderformat->transactionID.transactionid),msuStrLen(g_msuDeviceInfo.key));
  
    */
      if((g_msuBroadcastPdu.msuSyncHeaderformat.transactionID.transactionid==0) || 
        (g_msuBroadcastPdu.msuSyncHeaderformat.transactionID.transactionid !=g_Header.transactionid.transactionid))
      {
   	     MsuGlobalError = MSU_UPDATE_STATUS_TRANSACTIONID_ERROR;
   	     return result;	   		
      }
     
  
      g_msuBroadcastPdu.msuSyncHeaderformat.msuHeader.command =(localmsuSyncHeaderformat->msuHeader.command);
	   
   
      g_msuBroadcastPdu.msuSyncHeaderformat.msuHeader.fileNumber = localmsuSyncHeaderformat->msuHeader.fileNumber;
   
      if(g_msuBroadcastPdu.msuSyncHeaderformat.msuHeader.fileNumber==0)
   	 
   	  {
   	 	  MsuGlobalError = MSU_UPDATE_STATUS_FILE_VALUE_NULL;
   	   	  return result;	 
      }
	    	
   
      g_msuBroadcastPdu.msuSyncHeaderformat.msuHeader.versionInfo  = (localmsuSyncHeaderformat->msuHeader.versionInfo);
   	      	      
   
      g_msuBroadcastPdu.msuSyncHeaderformat.fileSize = msuNtoHl(localmsuSyncHeaderformat->fileSize);
   
      if(g_msuBroadcastPdu.msuSyncHeaderformat.fileSize==0)
      {
   	      MsuGlobalError = MSU_UPDATE_STATUS_FILE_SIZE_ZERO;
   	      return result;	   		
   }
	      
	      
   g_msuBroadcastPdu.msuSyncHeaderformat.chunkCount = msuNtoHl(localmsuSyncHeaderformat->chunkCount);
   
      if(g_msuBroadcastPdu.msuSyncHeaderformat.chunkCount==0)
   {
   	    MsuGlobalError = MSU_UPDATE_STATUS_CHUNKCOUNT_ZERO;
   	    return result;	   		
   }
  	  
	         
   g_msuBroadcastPdu.msuSyncHeaderformat.seqNumberLimit = msuNtoHs(localmsuSyncHeaderformat->seqNumberLimit);
   
         if(g_msuBroadcastPdu.msuSyncHeaderformat.seqNumberLimit==0)
   {
   	    MsuGlobalError = MSU_UPDATE_STATUS_SEQLIMIT_ZERO;
   	    return result;	   		
   }
   
   		if(g_msuBroadcastPdu.msuSyncHeaderformat.seqNumberLimit>MSU_MAX_SEQ_NUMBER)
   {
   	    MsuGlobalError = MSU_UPDATE_STATUS_SEQLIMIT_EXCEEDED;
   	    return result;	   		
   }
   
	      
   g_msuBroadcastPdu.msuSyncHeaderformat.seqSize = msuNtoHs(localmsuSyncHeaderformat->seqSize);
   
          if(g_msuBroadcastPdu.msuSyncHeaderformat.seqSize==0)
   {
   	    MsuGlobalError = MSU_UPDATE_STATUS_SEQSIZE_ZERO;
   	    return result;	   		
   }
        
          if(g_msuBroadcastPdu.msuSyncHeaderformat.seqSize>MSU_MAX_PACKET_SIZE )
   {
   	    MsuGlobalError = MSU_UPDATE_STATUS_SEQSIZE_EXCEEDED;
   	    return result;	   		
   }
	 
	  
	if(!g_msuBroadcastPdu.msuSyncHeaderformat.msuHeader.versionInfo.versionIP)
	{
		
		 g_msuBroadcastPdu.msuSyncHeaderformat.multicastIP.ServerIPv4=msuNtoHl(localmsuSyncHeaderformat->multicastIP.ServerIPv4);
   
          if(g_msuBroadcastPdu.msuSyncHeaderformat.multicastIP.ServerIPv4==0)
  		 {
   	         MsuGlobalError = MSU_UPDATE_STATUS_SERVER_IP_ZERO;
   	    	 return result;	   		
   	     }
	}
	else
	{
		msuByteCopy(&g_msuBroadcastPdu.msuSyncHeaderformat.multicastIP.ServerIPv6[0],&localmsuSyncHeaderformat->multicastIP.ServerIPv6[0],MAX_BYTES_IPv6_ADDRESS);
		  if(msuStrLen((MSU_SINT8*)g_msuBroadcastPdu.msuSyncHeaderformat.multicastIP.ServerIPv6)==0)
  		 {
   	         MsuGlobalError = MSU_UPDATE_STATUS_SERVER_IP_ZERO;
   	    	 return result;	   		
   	     }	
	}
	
		if(!g_msuBroadcastPdu.msuSyncHeaderformat.msuHeader.versionInfo.versionIP)
	{
		
		 g_msuBroadcastPdu.msuSyncHeaderformat.multicastCmIP.ServerIPv4=msuNtoHl(localmsuSyncHeaderformat->multicastCmIP.ServerIPv4);
   
          if(g_msuBroadcastPdu.msuSyncHeaderformat.multicastCmIP.ServerIPv4==0)
  		 {
   	         MsuGlobalError = MSU_UPDATE_STATUS_SERVER_CM_IP_ZERO;
   	    	 return result;	   		
   	     }
	}
	else
	{
		msuByteCopy(&g_msuBroadcastPdu.msuSyncHeaderformat.multicastCmIP.ServerIPv6[0],&localmsuSyncHeaderformat->multicastCmIP.ServerIPv6[0],MAX_BYTES_IPv6_ADDRESS);
		  if(msuStrLen((MSU_SINT8*)g_msuBroadcastPdu.msuSyncHeaderformat.multicastCmIP.ServerIPv6)==0)
  		 {
   	        MsuGlobalError = MSU_UPDATE_STATUS_SERVER_CM_IP_ZERO;
   	    	return result;	   		
   	     }	
	}
  

	     
   g_msuBroadcastPdu.msuSyncHeaderformat.multicastPort = msuNtoHs(localmsuSyncHeaderformat->multicastPort);
   
         if(g_msuBroadcastPdu.msuSyncHeaderformat.multicastPort==0)
   {
   	   MsuGlobalError = MSU_UPDATE_STATUS_PORT_ZERO;
   	   return result;	   		
   }
     
   g_msuBroadcastPdu.msuSyncHeaderformat.multicastCmPort = msuNtoHs(localmsuSyncHeaderformat->multicastCmPort);
    
   if(g_msuBroadcastPdu.msuSyncHeaderformat.multicastCmPort==0)
   {
   	   MsuGlobalError = MSU_UPDATE_STATUS_CM_PORT_ZERO;
   	   return result;	   		
   }

     
   g_msuBroadcastPdu.msuSyncHeaderformat.fileChecksum = msuNtoHl(localmsuSyncHeaderformat->fileChecksum);
   
          if(g_msuBroadcastPdu.msuSyncHeaderformat.fileChecksum==0)
   {
   	   MsuGlobalError = MSU_UPDATE_STATUS_CRC_ZERO;
   	   return result;	   		
   }

   g_msuBroadcastPdu.msuSyncHeaderformat.fileNameTag = (localmsuSyncHeaderformat->fileNameTag);
    
   g_msuBroadcastPdu.msuSyncHeaderformat.destPathTag = (localmsuSyncHeaderformat->destPathTag);
  
   g_msuBroadcastPdu.msuSyncHeaderformat.updateTimeout = localmsuSyncHeaderformat->updateTimeout;
   
   /* Populate the filename and the destination path fields*/
   
   if(localmsuSyncHeaderformat->fileNameTag.isFieldPresent)
   {
       msuByteCopy(g_msuBroadcastPdu.msuSyncMessageVariableData.fileName,localmsuSyncMessageVariableData->fileName,localmsuSyncHeaderformat->fileNameTag.fieldLength);
   }
   
   
   if(localmsuSyncHeaderformat->destPathTag.isFieldPresent)
   {
      msuByteCopy(g_msuBroadcastPdu.msuSyncMessageVariableData.destPath,localmsuSyncMessageVariableData->destPath,localmsuSyncHeaderformat->destPathTag.fieldLength);
   }
   
 

    result = MSU_TRUE;
    
   }
    return result;
}


/**  @fn MSU_BOOL msuPduParseMcastRequest(MSU_UINT8 *pMsg , MSU_UINT32 msgSize)
 *   @brief msuPduParseMcastRequest() function is used to validate multicast data packet
 *   @details  When client receives multicast data packet, it calls msuPduParseMcastRequest( ) function
 *             and validates the message and assign the message buffer to multicast pdu buffer.
 *             function returns true if message is valid message.
 *   @param[in] : MSU_UINT8 *pMsg     data transfer message buffer
 *   @param[in] : MSU_UINT32 msgSize  size of message buffer
 *   @return  MSU_TRUE, if message is valid, otherwise MSU_FALSE
 *   @retval 1 if success
 *   @retval 0 if fails
 */
MSU_BOOL msuPduParseMcastRequest(MSU_UINT8 *pMsg , MSU_UINT32 msgSize)
{
    MSU_BOOL result = MSU_FALSE;
 	MSU_UINT8 *pBuf = MSU_NULL;
 	MSU_UINT32 tempMcastPacketSize = 0;

 	//Get the pdu size for fixed section of the frame
 	tempMcastPacketSize = (MSU_UINT32) (sizeof(MSUMulticastPdu) - sizeof(MSU_UINT8 *));

 	/* The message pointer should not be MSU_NULL and a minimum received bytes should be
    atleast equal to the fixed size of the multicast message pdu or Upgrade complete pdu */
 	if((pMsg != MSU_NULL) && (msgSize >= sizeof(MSUCompletePdu)))
 	{
   	    pBuf = pMsg;

   	    //Initialize the Multicast PDU
    	msuMemorySet((MSU_UINT8*)&g_msuMcastPdu, 0, sizeof(MSUMulticastPdu));

    	/* Copy header*/
    	msuByteCopy((MSU_UINT8*)&g_msuMcastPdu,pBuf,sizeof(MSUMulticastPdu));
    	/*Extract the members*/
    	g_msuMcastPdu.msuHeader.fileNumber = msuNtoHs(g_msuMcastPdu.msuHeader.fileNumber);
    	if ( (g_msuMcastPdu.msuHeader.command.subcode == MSU_SUBCODE_TRANSFER_COMPLETE) &&
    		(msuCurrentState()== eMsuStateRunning))
    	{
      	/*The packet received was upgrade complete message from the server. Extract the remaining
        	fields.*/
    		g_msuMcastPdu.fileTransferInfo = (*(FileTransferInfo *) (pBuf+4));

    		result = MSU_TRUE;
			msuLogMsg( MSU_DEBUG_LEVEL_SUCCESS,"\nUpgrade Complete Msg Received..Validation OK\n");
		}
    	else if(msgSize > tempMcastPacketSize)
    	{
      	  /*The packet received was a normal multicast message from the server. Extract the remaining
          fields.*/
    	    g_msuMcastPdu.chunkNumber = msuNtoHl(g_msuMcastPdu.chunkNumber);
    		g_msuMcastPdu.dataLength = msuNtoHs(g_msuMcastPdu.dataLength);
			
			// greater than equal to for unpacked sequence
			// variable buffer length may vary with respect to data sent by the server
    		
    		if(msgSize  >= ((MSU_UINT32) (tempMcastPacketSize + g_msuMcastPdu.dataLength)))
    		{
	    		//Point the data to the location in the buffer
    			g_msuMcastPdu.pData = pBuf+tempMcastPacketSize;
    			result = MSU_TRUE;
    		}
    	}
 	}
   return result;
}


/**  @fn MSU_BOOL msuAuthenticationRequest(MSU_UINT8 *pMsg,MSU_UINT32 msgSize,MSUAuthenticationRequestPdu *pAuthenticationRequest,
		MSUAuthenticationResponsePdu *msuAuthenticationResponse)
 *   @brief msuAuthenticationRequest parses the authentication request and constructs the response
 *   @details  When client receives the authentication request it parses the request,reads and 
 *             stores the data in the buffer.It then validates the data against the data from MSU.bin
 *             It then sends the response to the server
 *   @param[in] : MSU_UINT8 *pMsg     data transfer message buffer
 *   @param[in] : MSU_UINT32 msgSize  size of message buffer
 *   @param[in] : MSUAuthenticationRequestPdu pAuthenticationRequest: Authentication Request Pdu
 *   @param[in] : MSUAuthenticationResponsePdu msuAuthenticationResponse: Authentication Response Pdu 
 *   @return  MSU_TRUE, if message is valid and credentials valid, otherwise MSU_FALSE
 *   @retval 1 if success
 *   @retval 0 if fails
 */

MSU_BOOL msuAuthenticationRequest(MSU_UINT8 *pMsg,MSU_UINT32 msgSize,MSUAuthenticationRequestPdu *pAuthenticationRequest,
		MSUAuthenticationResponsePdu *msuAuthenticationResponse,msuSockinfo *socketip )
{   
   MSU_UINT8 	*pBuf; 
   MSU_UINT8 	errcode=0;
   MSU_UINT8 	errsubcode=0;
   MSUAuthenticationHeader 	*Header;
   MSUtransactionid			decryptedtransactonid;	   
   int 		i=0,j=0;
   MSU_UINT8 	tagSize = 0;
   MSU_BOOL 	EntryFound = MSU_FALSE;
   MSU_BOOL 	retCode = MSU_FALSE;
   MSU_UINT32	length_privatekey=0;
   
   
   msuMemorySet((MSU_UINT8*)pAuthenticationRequest,0,sizeof(MSUAuthenticationRequestPdu));   
   
   /* Get MSU Authentication message header*/
   Header = (MSUAuthenticationHeader*)pMsg;
   
   
   /*The message pointer should not be MSU_NULL and a minimum received bytes should be
      atleast equal to MSU Authentication message header*/
   if((pMsg != MSU_NULL) && (msgSize >= sizeof(MSUAuthenticationHeader)))
   {	
   			
		pBuf= (MSU_UINT8 *)(pMsg+sizeof(MSUAuthenticationHeader));
		if (Header->AuthenticationLevel.Level!=g_msuDeviceInfo.AuthenticationLevelTag)
		{
			errcode = MSU_TRUE;
			errsubcode = MSU_AUTHENTICATION_ERRCODE;
			goto authenticationDone;
		}
		
		/*Decrypting transaction ID */
		length_privatekey = msuStrLen(g_msuDeviceInfo.key);
                /*
		msuDecryption((MSU_UINT8*)Header->transactionid.transactionencrypt,
					  (char*)&g_msuDeviceInfo.key,sizeof(Header->transactionid),
					  length_privatekey);
		*/
		/*For testing */
		Header->transactionid.transactionid=msuNtoHl(Header->transactionid.transactionid);
	  
		if (Header->transactionid.transactionid==0)	
		{			
			errcode = MSU_AUTHENTICATION_ERRCODE;
			errsubcode = MSU_AUTHENTICATION_ERRSUBCODE_TRANSACTIONID;
			goto authenticationDone;
		}
		
		/*File Name Tag*/
		if(Header->FileNameTag.isFieldPresent)
		{
      	   if(Header->FileNameTag.fieldLength > MSU_MAX_TAG_SIZE)
      	   {
      	   		tagSize = MSU_MAX_TAG_SIZE;		
      	   }
      	   else
      	   {
      	   	tagSize = Header->FileNameTag.fieldLength;
      	   }
      	   		
    	   msuByteCopy((MSU_UINT8*)&pAuthenticationRequest->authPayload.filename,pBuf,tagSize);
    	   pBuf+=Header->FileNameTag.fieldLength; 	   
		}
	
		/*Hardware ID Tag*/
		if(Header->hardwareIdTag.isFieldPresent)
		{
      	   if(Header->hardwareIdTag.fieldLength > MSU_MAX_TAG_SIZE)
      	   {
      	   		tagSize = MSU_MAX_TAG_SIZE;		
      	   }
      	   else
      	   {
      	   	tagSize = Header->hardwareIdTag.fieldLength;
      	   }
      	   		
    	   msuByteCopy((MSU_UINT8*)&pAuthenticationRequest->authPayload.HardwareId,pBuf,tagSize);
    	   pBuf+=Header->hardwareIdTag.fieldLength; 	   
		}
	
		/*Product ID Tag*/
		if(Header->productIdTag.isFieldPresent)
		{
      	   if(Header->productIdTag.fieldLength > MSU_MAX_TAG_SIZE)
      	   {
      	   		tagSize = MSU_MAX_TAG_SIZE;		
      	   }
      	   else
      	   {
      	   	tagSize = Header->productIdTag.fieldLength;
      	   }
      	   		
    	   msuByteCopy((MSU_UINT8*)&pAuthenticationRequest->authPayload.ProductId,pBuf,tagSize);
    	   pBuf+=Header->productIdTag.fieldLength; 	   
		}

		/*modelname Tag*/            
		if(Header->modelnameTag.isFieldPresent)
		{
      	   if(Header->modelnameTag.fieldLength > MSU_MAX_TAG_SIZE)
      	   {
      	   		tagSize = MSU_MAX_TAG_SIZE;		
      	   }
      	   else
      	   {
      	   	tagSize = Header->modelnameTag.fieldLength;
      	   }
      	   		
    	   msuByteCopy((MSU_UINT8*)&pAuthenticationRequest->authPayload.modelname,pBuf,tagSize);
    	   pBuf+=Header->modelnameTag.fieldLength; 	   
		}
	
		/*password Tag*/      
		if(Header->passwordTag.isFieldPresent)
      	{
      	   if(Header->passwordTag.fieldLength > MSU_MAX_TAG_SIZE)
      	   {
      	   		tagSize = MSU_MAX_TAG_SIZE;		
      	   }
      	   else
      	   {
      	   	tagSize = Header->passwordTag.fieldLength;
      	   }		
      	   		
    	   msuByteCopy((MSU_UINT8*)&pAuthenticationRequest->authPayload.password,pBuf,tagSize);
    	    pBuf+=Header->passwordTag.fieldLength; 	   
      	}
		
		/*username Tag*/     
		if(Header->usernameTag.isFieldPresent)
		{
      	   if(Header->usernameTag.fieldLength > MSU_MAX_TAG_SIZE)
      	   {
      	   		tagSize = MSU_MAX_TAG_SIZE;		
      	   }
      	   else
      	   {
      	   	tagSize = Header->usernameTag.fieldLength;
      	   }
      	   		
    	   msuByteCopy((MSU_UINT8*)&pAuthenticationRequest->authPayload.username,pBuf,tagSize);
    	   pBuf+=Header->usernameTag.fieldLength; 	   
      	}
	
		/*firmware version Tag*/
     	
		if(Header->firmwareversionTag.isFieldPresent)
      	{
      	   if(Header->firmwareversionTag.fieldLength > MSU_MAX_TAG_SIZE)
      	   {
      	   		tagSize = MSU_MAX_TAG_SIZE;		
      	   }
      	   else
      	   {
      	   	tagSize = Header->firmwareversionTag.fieldLength;
      	   }
      	   		
    	   msuByteCopy((MSU_UINT8*)&pAuthenticationRequest->authPayload.firmwareversion,pBuf,tagSize);
    	    pBuf+=Header->firmwareversionTag.fieldLength; 	   
      	}
	
		/*software version Tag*/     
		if(Header->softwareversionTag.isFieldPresent)
      	{
      	   if(Header->softwareversionTag.fieldLength > MSU_MAX_TAG_SIZE)
      	   {
      	   		tagSize = MSU_MAX_TAG_SIZE;		
      	   }
      	   else
      	   {
      	   	tagSize = Header->softwareversionTag.fieldLength;
      	   }
      	   		
    	   msuByteCopy((MSU_UINT8*)&pAuthenticationRequest->authPayload.softwareversion,(MSU_UINT8*)pBuf,tagSize);
    	   pBuf+=Header->softwareversionTag.fieldLength; 	   
      	}

		/*vendorid Tag*/	
		if(Header->vendorIdTag.isFieldPresent)
      	{
      	   if(Header->vendorIdTag.fieldLength > MSU_MAX_TAG_SIZE)
      	   {
      	   		tagSize = MSU_MAX_TAG_SIZE;		
      	   }
      	   else
      	   {
      	   	tagSize = Header->vendorIdTag.fieldLength;
      	   }
      	   		
    	   msuByteCopy((MSU_UINT8*)&pAuthenticationRequest->authPayload.vendorId,(MSU_UINT8*)pBuf,tagSize);
    	    pBuf+=Header->vendorIdTag.fieldLength; 	   
      	}
		
		/*product name Tag*/     	   
		if(Header->productnameTag.isFieldPresent)
      	{
      	   if(Header->productnameTag.fieldLength > MSU_MAX_TAG_SIZE)
      	   {
      	   		tagSize = MSU_MAX_TAG_SIZE;		
      	   }
      	   else
      	   {
      	   	tagSize = Header->productnameTag.fieldLength;
      	   }
      	   		
    	   msuByteCopy((MSU_UINT8*)&pAuthenticationRequest->authPayload.productname,(MSU_UINT8*)pBuf,tagSize);
    	   pBuf+=Header->productnameTag.fieldLength; 	   
      	}
	
		/* Validating the server IP with socket ip address and aginst the Whitelisted IPs */		
		if (!Header->versionInfo.versionIP) 
		{	
		
			Header->msuServerIP.ServerIPv4 = msuNtoHl(Header->msuServerIP.ServerIPv4);
			if ((Header->msuServerIP.ServerIPv4)==(socketip->ipAddress))
			{
				if(g_msuDeviceInfo.NumberOfIPSupported == 0)	
				{
					EntryFound = MSU_TRUE;
				}
				else
				{
					if(g_msuDeviceInfo.NumberOfIPSupported <= 10)
					{					
						for (i=0;i<g_msuDeviceInfo.NumberOfIPSupported;i++)
						{				
							if ((Header->msuServerIP.ServerIPv4)==(g_msuDeviceInfo.IP[i].ServerIPv4))
							{
								EntryFound = MSU_TRUE;				
								break;
							}
						}				
				
					}
				}
	
				
				
			}		
			if (EntryFound == MSU_FALSE)
			{
				errcode=MSU_TRUE;
				errsubcode=MSU_AUTHENTICATION_ERRSUBCODE_IP;
				goto authenticationDone;
			}	
			
		}//if (!Header->versionInfo.versionIP) 				
	
		/* User name and password decryption algorithm */
		if (Header->usernameTag.isFieldPresent)
		{
			
			msuDecryption((MSU_UINT8*)&pAuthenticationRequest->authPayload.username, (char*) &g_msuDeviceInfo.key,
									Header->usernameTag.fieldLength,length_privatekey);
		}
			
		if (msuStrNCmp((MSU_SINT8*)&pAuthenticationRequest->authPayload.username,(MSU_SINT8*)&g_msuDeviceInfo.Username,sizeof(pAuthenticationRequest->authPayload.username))!=0)
		{
			errcode=MSU_AUTHENTICATION_ERRCODE;
			errsubcode=MSU_AUTHENTICATION_ERRSUBCODE_LOGININFO;
			goto authenticationDone;
		}	
	
		if (Header->passwordTag.isFieldPresent  )
		{
			msuDecryption((MSU_UINT8*)&pAuthenticationRequest->authPayload.password, (char*) &g_msuDeviceInfo.key,
							Header->passwordTag.fieldLength,length_privatekey);
		}	
	
		if (msuStrNCmp((MSU_SINT8*)&pAuthenticationRequest->authPayload.password,(MSU_SINT8*)&g_msuDeviceInfo.Password,sizeof(pAuthenticationRequest->authPayload.password))!=0)
		{
			errcode=MSU_AUTHENTICATION_ERRCODE;
			errsubcode=MSU_AUTHENTICATION_ERRSUBCODE_LOGININFO;
			goto authenticationDone;
		}	
	
		EntryFound = MSU_FALSE;
		if(Header->FileNameTag.isFieldPresent )
		{	
		  if(g_msuDeviceInfo.noOfFileSupported <= 3)
			{
				for (i=0;i<g_msuDeviceInfo.noOfFileSupported;i++)
				{
								
					if(msuStrNCmp((MSU_SINT8*)&pAuthenticationRequest->authPayload.filename,(MSU_SINT8*)&g_msuDeviceInfo.filename[i],MAX_FILENAME_LEN) ==0)	//changed != /== 10/14/14		
					{
						EntryFound = MSU_TRUE;						
						break;
					}
				}							
			}				
		}      
		if (EntryFound == MSU_FALSE)
		{
			errcode=MSU_AUTHENTICATION_ERRCODE;
			errsubcode=MSU_AUTHENTICATION_ERRSUBCODE_FILENAME;
			goto authenticationDone;
		}
	
	
		if(Header->hardwareIdTag.isFieldPresent && g_deviceIdentification.hardwareIdTag.isFieldPresent  )
		{
			if (msuStrNCmp((MSU_SINT8*)&pAuthenticationRequest->authPayload.HardwareId[0],(MSU_SINT8*)&g_deviceIdentification.hardwareId[0],
							Header->hardwareIdTag.fieldLength)!=0)
			{
				errcode=MSU_AUTHENTICATION_ERRCODE;
				errsubcode=MSU_AUTHENTICATION_ERRSUBCODE_HARDWAREID;
				goto authenticationDone;
			}			
		}         	
authenticationDone:		
		/* Load the data to authentication response buffer */		
		msuAuthenticationResponse->command.opcode=MSU_OPCODE_AUTHENTICATION;
		msuAuthenticationResponse->command.subcode=MSU_SUBCODE_AUTHENTICATION_RESPONSE;
		msuAuthenticationResponse->ErrCode=errcode;
		msuAuthenticationResponse->ErrSubCode=errsubcode;		
		msuAuthenticationResponse->versionInfo.versionIP = Header->versionInfo.versionIP;
		msuAuthenticationResponse->versionInfo.versionProtocol=MSU_PROTOCOL_VERSION;	
		msuByteCopy((MSU_UINT8*)&g_Header,(MSU_UINT8*)Header,sizeof(MSUAuthenticationHeader));
		retCode = MSU_TRUE;
    }
   return retCode;
}



/**  @fn void msuCcmMsgCreate( MSUCCMPdu *msuCcmPdu, MSU_UINT32 totalChnksExpected,MSU_UINT32 *missedChnkCnt)
 *   @brief msuCcmMsgCreate() is used to construct CCM message format
 *   @details msuCcmMsgCreate()  function is used to construct CCM message format, client will use this message   
 *            and send to server to re transmit missed chunks  
 *   @param[in] : MSUCCMPdu *msuCcmPdu   pointer to CCM pdu which will be updated inside the function     
 *   @param[in] : MSU_UINT32 totalChnksExpected  total chunk number in the file
 *   @param[in] : MSU_UINT32 *missedChnkCnt   pointer which will be updated with missed chunk count inside the function
 *   @return    : None  
 *   @retval    : None 
 */
void msuCcmMsgCreate( MSUCCMPdu *msuCcmPdu, MSU_UINT32 totalChnksExpected,MSU_UINT32 *missedChnkCnt)
{
   MSU_UINT32 *missedChunkNums = MSU_NULL;
	msuCcmPdu->msuHeader.command.opcode = MSU_OPCODE_UPGRADE;
	msuCcmPdu->msuHeader.command.subcode = MSU_SUBCODE_CCM;
	msuCcmPdu->msuHeader.fileNumber = msuHtoNs(g_msuBroadcastPdu.msuSyncHeaderformat.msuHeader.fileNumber);
	msuCcmPdu->msuHeader.versionInfo.versionIP = MSU_IPV4_VERSION;
	msuCcmPdu->msuHeader.versionInfo.versionProtocol=MSU_PROTOCOL_VERSION;

	missedChunkNums = getMissedChnkNumsArry(totalChnksExpected, missedChnkCnt);

	msuCcmPdu->chunkCount = msuHtoNl(*missedChnkCnt);

	msuCcmPdu++;

	msuByteCopy((MSU_UINT8*)msuCcmPdu, (MSU_UINT8*)missedChunkNums, (sizeof(MSU_UINT32) * *missedChnkCnt));
}

/**  @fn void msuUpgrateMsgCreate( MSUUpgradeStatusPdu *msuUpgradeStatus, MSU_SINT32 upgradeStatus, MSU_UINT8 ccmRetryVal )
 *   @brief msuUpgrateMsgCreate() is used to construct a Upgrade status message format
 *   @param[in] : MSUUpgradeStatusPdu *msuUpgradeStatus    pointer to upgrade stauts pdu, pdu data updated inside the function 
 *   @param[in] : MSU_SINT32 upgradeStatus  status of the data upgrade, it may fail or pass or in progress 
 *   @param[in] : MSU_UINT8 ccmRetryVal   Number of times CCM retrys happend to receive data 
 *   @return    : None 
 *   @retval    : None 
 */
void msuUpgrateMsgCreate( MSUUpgradeStatusPdu *msuUpgradeStatus, MSU_SINT32 upgradeStatus, MSU_UINT8 ccmRetryVal )
{
	msuMemorySet((MSU_UINT8*)msuUpgradeStatus, 0, sizeof(MSUUpgradeStatusPdu));

	msuUpgradeStatus->command.opcode = MSU_OPCODE_UPGRADE;
	msuUpgradeStatus->command.subcode = MSU_SUBCODE_CLIENT_STATUS_RESPONSE;
	msuUpgradeStatus->status = (MSU_UINT8)upgradeStatus;
    
	msuUpgradeStatus->errorCode = MsuGlobalError;
    
	msuUpgradeStatus->versionInfo.versionIP = MSU_IPV4_VERSION;
	msuUpgradeStatus->versionInfo.versionProtocol = MSU_PROTOCOL_VERSION;
	msuUpgradeStatus->transactionID.transactionid = msuHtoNl(g_msuBroadcastPdu.msuSyncHeaderformat.transactionID.transactionid);
	
	msuUpgradeStatus->deviceID = (MSU_UINT32)clientIP;	
    
	msuUpgradeStatus->ccmRetryCount = ccmRetryVal;
    
}

/**  @fn void msuDiscoveryResponseMsgCreate( MSUDiscoveryRequestPdu *discoveryRequestPdu,
					MSUDiscoveryResponsePdu *msuDiscoveryResponse,MSU_UINT16 *dataPayloadSize )
 *   @brief msuDiscoveryResponseMsgCreate() is used to construct Discovery response message format
 *   @param[in] : MSUDiscoveryRequestPdu *discoveryRequestPdu    passing msu discovery request pdu pointer
 *   @param[in] : MSUDiscoveryResponsePdu *msuDiscoveryResponse  msu discovery response pdu data will be updated inside the function
 *   @param[in] : MSU_UINT16 *dataPayloadSize  data payload size will be update by the function
 *   @return    : None 
 *   @retval None
 */
void msuDiscoveryResponseMsgCreate(MSUDiscoveryRequestPdu *discoveryRequestPdu,MSUDiscoveryResponsePdu 
*msuDiscoveryResponse,MSU_UINT16 *dataPayloadSize)
{

   MSU_UINT8 pbufvarlen=0;    
   MSU_UINT8 hardwareid='2';   
  
	MSU_UINT32 clientIPAddress = (MSU_UINT32)clientIP; // client IP to be changed to accomodate IPV6
   


   msuMemorySet((MSU_UINT8*)msuDiscoveryResponse, 0, sizeof(MSUDiscoveryResponsePdu));

   /*Writing the discovery response opcode,subcode,groupid and version information */
   msuDiscoveryResponse->command.opcode = MSU_OPCODE_DEVICE_DISCOVER;
   msuDiscoveryResponse->command.subcode = MSU_SUBCODE_DEVICE_DISCOVERY_RESPONSE;
   msuDiscoveryResponse->GroupId =gGroupID;
   msuDiscoveryResponse->versionInfo = discoveryRequestPdu->versionInfo;
   
  /* sending the device ip to the response PDU */   
  if (!discoveryRequestPdu->versionInfo.versionIP)
  {
     msuByteCopy((MSU_UINT8*)&msuDiscoveryResponse->deviceID[0],(MSU_UINT8*)&clientIPAddress,sizeof(discoveryRequestPdu->ServerIpStart.ServerIPv4));	
  }
  else
  {
     msuByteCopy((MSU_UINT8*)&msuDiscoveryResponse->deviceID,(MSU_UINT8*)&clientIPAddress,sizeof(discoveryRequestPdu->ServerIpStart.ServerIPv6));	
  }
   
	         
  /* sending the device authentication information */
  msuDiscoveryResponse->AuthenticationLevel.IsClient=g_msuDeviceInfo.Client;
  msuDiscoveryResponse->AuthenticationLevel.IsMaster=g_msuDeviceInfo.Master;
  msuDiscoveryResponse->AuthenticationLevel.IsAuthRequired=g_msuDeviceInfo.AutheticationTag;
  msuDiscoveryResponse->AuthenticationLevel.Level=g_msuDeviceInfo.AuthenticationLevelTag;
   
  /* sending the hardware id information from the structure g_deviceIdentification and incrementing the pointer
  reference to discovery response variable length buffer by the length of hardware id tag*/  
  if(g_deviceIdentification.hardwareIdTag.isFieldPresent)
  {
     msuDiscoveryResponse->hardwareIdTag.isFieldPresent=MSU_TRUE;
     msuDiscoveryResponse->hardwareIdTag.fieldLength=g_deviceIdentification.hardwareIdTag.fieldLength;
     msuByteCopy((MSU_UINT8*)&msuDiscoveryResponse->varlengthdata[pbufvarlen],(MSU_UINT8*)&g_deviceIdentification.hardwareId,msuDiscoveryResponse->hardwareIdTag.fieldLength);
     *dataPayloadSize +=msuDiscoveryResponse->hardwareIdTag.fieldLength;
     pbufvarlen=(MSU_UINT8)*dataPayloadSize;	
  }

  /* sending the product id information from the structure g_deviceIdentification and incrementing the pointer
  reference to discovery response variable length buffer by the length of product id tag*/     
  if(g_deviceIdentification.productIdTag.isFieldPresent)
  {
     msuDiscoveryResponse->productIdTag.isFieldPresent=MSU_TRUE;
     msuDiscoveryResponse->productIdTag.fieldLength=g_deviceIdentification.productIdTag.fieldLength;
     msuByteCopy((MSU_UINT8*)&msuDiscoveryResponse->varlengthdata[pbufvarlen],(MSU_UINT8*)&g_deviceIdentification.productId,
     msuDiscoveryResponse->productIdTag.fieldLength);
     *dataPayloadSize +=msuDiscoveryResponse->productIdTag.fieldLength;
     pbufvarlen=(MSU_UINT8)*dataPayloadSize;
  }

  /* sending the product name information from the structure g_deviceIdentification and incrementing the pointer
  reference to discovery response variable length buffer by the length of product name field*/
  if(g_deviceIdentification.productNameTag.isFieldPresent)
  {
    msuDiscoveryResponse->productNameTag.isFieldPresent=MSU_TRUE;
    msuDiscoveryResponse->productNameTag.fieldLength=g_deviceIdentification.productNameTag.fieldLength;
    msuByteCopy((MSU_UINT8*)&msuDiscoveryResponse->varlengthdata[pbufvarlen],(MSU_UINT8*)&g_deviceIdentification.productName,
    msuDiscoveryResponse->productNameTag.fieldLength);   
    *dataPayloadSize +=msuDiscoveryResponse->productNameTag.fieldLength;
    pbufvarlen=(MSU_UINT8)*dataPayloadSize;	
  }
   	
 /* sending the model name information from the structure g_deviceIdentification and incrementing the pointer
 reference to discovery response variable length buffer by the length of model name field */   	
 if(g_deviceIdentification.modelNameTag.isFieldPresent)
 {
   msuDiscoveryResponse->modelNameTag.isFieldPresent=MSU_TRUE;
   msuDiscoveryResponse->modelNameTag.fieldLength=g_deviceIdentification.modelNameTag.fieldLength;
   msuByteCopy((MSU_UINT8*)&msuDiscoveryResponse->varlengthdata[pbufvarlen],(MSU_UINT8*)&g_deviceIdentification.modelName,
   msuDiscoveryResponse->modelNameTag.fieldLength);      
   *dataPayloadSize +=msuDiscoveryResponse->modelNameTag.fieldLength;
   pbufvarlen=(MSU_UINT8)*dataPayloadSize;	
}
   	
/* sending the vendor id information from the structure g_deviceIdentification and incrementing the pointer
reference to discovery response variable length buffer by the length of vendor id tag */    	
if(g_deviceIdentification.vendorIdTag.isFieldPresent)
{
  msuDiscoveryResponse->vendorIdTag.isFieldPresent=MSU_TRUE;
  msuDiscoveryResponse->vendorIdTag.fieldLength=g_deviceIdentification.vendorIdTag.fieldLength;
  msuByteCopy((MSU_UINT8*)&msuDiscoveryResponse->varlengthdata[pbufvarlen],(MSU_UINT8*)&g_deviceIdentification.vendorname,
  msuDiscoveryResponse->vendorIdTag.fieldLength);
  *dataPayloadSize +=msuDiscoveryResponse->vendorIdTag.fieldLength;
  pbufvarlen=(MSU_UINT8)*dataPayloadSize;
}

/* sending the firmware version information from the structure g_deviceIdentification and incrementing the pointer
reference to discovery response variable length buffer by the length of firmware version */   
if(g_deviceIdentification.firmwareVersionTag.isFieldPresent)
{
   msuDiscoveryResponse->firmwareVersionTag.isFieldPresent=MSU_TRUE;
   msuDiscoveryResponse->firmwareVersionTag.fieldLength=g_deviceIdentification.firmwareVersionTag.fieldLength;
   msuByteCopy((MSU_UINT8*)&msuDiscoveryResponse->varlengthdata[pbufvarlen],(MSU_UINT8*)&g_deviceIdentification.firmwareVersion,
   msuDiscoveryResponse->firmwareVersionTag.fieldLength);
   *dataPayloadSize +=msuDiscoveryResponse->firmwareVersionTag.fieldLength;
   pbufvarlen=(MSU_UINT8)*dataPayloadSize;	
}

/* sending the revision information from the structure g_deviceIdentification and incrementing the pointer
reference to discovery response variable length buffer by the length of revision field */ 
if(g_deviceIdentification.revisionTag.isFieldPresent)
{
   msuDiscoveryResponse->revisionTag.isFieldPresent=MSU_TRUE;
   msuDiscoveryResponse->revisionTag.fieldLength=g_deviceIdentification.revisionTag.fieldLength;
   msuByteCopy((MSU_UINT8*)&msuDiscoveryResponse->varlengthdata[pbufvarlen],(MSU_UINT8*)&g_deviceIdentification.revision,
   msuDiscoveryResponse->firmwareVersionTag.fieldLength);
   *dataPayloadSize +=msuDiscoveryResponse->revisionTag.fieldLength;
   pbufvarlen=(MSU_UINT8)*dataPayloadSize;	
}

/* sending the devicelocation information from the structure g_deviceIdentification and incrementing the pointer
reference to discovery response variable length buffer by the length of devicelocation field  */   	
if(g_deviceIdentification.deviceLocTag.isFieldPresent)
{
   msuDiscoveryResponse->deviceLocTag.isFieldPresent=MSU_TRUE;
   msuDiscoveryResponse->deviceLocTag.fieldLength=g_deviceIdentification.deviceLocTag.fieldLength;
   msuByteCopy((MSU_UINT8*)&msuDiscoveryResponse->varlengthdata[pbufvarlen],(MSU_UINT8*)&g_deviceIdentification.deviceLoc,
   msuDiscoveryResponse->deviceLocTag.fieldLength);
   *dataPayloadSize +=msuDiscoveryResponse->deviceLocTag.fieldLength;
   pbufvarlen=(MSU_UINT8)*dataPayloadSize;
}


/* writing the device msu parameter tag information */
   msuDiscoveryResponse->MsuCommParam.isFieldPresent = MSU_TRUE;
   msuDiscoveryResponse->MsuCommParam.fieldLength    = sizeof(MSU_COMM_PARAM);
    
/* sending the device msu paramter information from the structure g_deviceIdentification and incrementing the
pointer reference to discovery response variable length buffer by the length of devicelocation field  */    
   msuByteCopy( (MSU_UINT8*)(&msuDiscoveryResponse->varlengthdata[pbufvarlen] ), (MSU_UINT8*)&g_msuCommParam,sizeof(MSU_COMM_PARAM) );
   *dataPayloadSize += msuDiscoveryResponse->MsuCommParam.fieldLength;

}

/**  @fn MSU_SINT32 msuSetDeviceIdentification( setMsuDeviceIdentification *g_deviceIdentification)
 * @Brief: Set the device information from application need for discovery message
 *         in of case not called this function or g_deviceIdentification is NULL it will set the
 *         device information data to default as per MSU defination.
 *
 * @param[in] : MSUg_deviceIdentification* g_deviceIdentification   Device information data type
 * @return    : success -0 / fail -1
 */
  MSU_SINT32 msuSetDeviceIdentification( MSUDeviceIdentification * deviceIdentity)
  {
    if(deviceIdentity == MSU_NULL)
      {
        /* sending the hardware id information from the structure g_deviceIdentification and incrementing the pointer
        reference to discovery response variable length buffer by the length of hardware id tag*/
        g_deviceIdentification.hardwareIdTag.isFieldPresent = MSU_FALSE;

        /* sending the product id information from the structure g_deviceIdentification and incrementing the pointer
        reference to discovery response variable length buffer by the length of product id tag*/
        g_deviceIdentification.productIdTag.isFieldPresent = MSU_TRUE;
        g_deviceIdentification.productIdTag.fieldLength = sizeof(MSU_PRODUCT_ID);
        msuByteCopy((MSU_UINT8*)&g_deviceIdentification.productId,MSU_PRODUCT_ID,sizeof(MSU_PRODUCT_ID) );

        /* sending the product name information from the structure g_deviceIdentification and incrementing the pointer
        reference to discovery response variable length buffer by the length of product name field*/
        g_deviceIdentification.productNameTag.isFieldPresent = MSU_TRUE;
        g_deviceIdentification.productNameTag.fieldLength = sizeof(MSU_PRODUCT_NAME);
        msuByteCopy((MSU_UINT8*)&g_deviceIdentification.productName,MSU_PRODUCT_NAME,sizeof(MSU_PRODUCT_NAME) );

       /* sending the model name information from the structure g_deviceIdentification and incrementing the pointer
       reference to discovery response variable length buffer by the length of model name field */
        g_deviceIdentification.modelNameTag.isFieldPresent = MSU_TRUE;
        g_deviceIdentification.modelNameTag.fieldLength = sizeof(MSU_MODEL_NAME);
        msuByteCopy((MSU_UINT8*)&g_deviceIdentification.modelName,MSU_MODEL_NAME,sizeof(MSU_MODEL_NAME) );

      /* sending the vendor id information from the structure g_deviceIdentification and incrementing the pointer
      reference to discovery response variable length buffer by the length of vendor id tag */
        g_deviceIdentification.vendorIdTag.isFieldPresent = MSU_TRUE;
        g_deviceIdentification.vendorIdTag.fieldLength = sizeof(MSU_VENDOR_NAME);
        msuByteCopy((MSU_UINT8*)&g_deviceIdentification.vendorname,MSU_VENDOR_NAME,sizeof(MSU_VENDOR_NAME) );

      /* sending the firmware version information from the structure g_deviceIdentification and incrementing the pointer
      reference to discovery response variable length buffer by the length of firmware version */
        g_deviceIdentification.firmwareVersionTag.isFieldPresent = MSU_TRUE;
        g_deviceIdentification.firmwareVersionTag.fieldLength = sizeof(MSU_PRODUCT_FIRMWARE_VERSION);
        msuByteCopy((MSU_UINT8*)&g_deviceIdentification.firmwareVersion,MSU_PRODUCT_FIRMWARE_VERSION,sizeof(MSU_PRODUCT_FIRMWARE_VERSION) );

      /* sending the revision information from the structure g_deviceIdentification and incrementing the pointer
      reference to discovery response variable length buffer by the length of revision field */
        g_deviceIdentification.revisionTag.isFieldPresent = MSU_FALSE;

      /* sending the devicelocation information from the structure g_deviceIdentification and incrementing the pointer
      reference to discovery response variable length buffer by the length of devicelocation field  */
        g_deviceIdentification.deviceLocTag.isFieldPresent = MSU_FALSE;
      }
    else
      {
        msuByteCopy((MSU_UINT8*)&g_deviceIdentification, (MSU_UINT8*)deviceIdentity, sizeof(deviceIdentity));
      }
    return MSU_SUCCESS;
  }
/**  @fn MSU_SINT32 msuSetCommunicationParameters(MSU_COMM_PARAM * param)
 *   @brief This function is used to set the MSU communication Protocol parameters
 *          if param == NULL set to default value else set to input value. If application
 *          did not set this value than takes the default value.
 *   @param[in] : MSU_COMM_PARAM * param   Parameters data type *
 *   @return    : success -0 / fail -1
 */
MSU_SINT32 msuSetCommunicationParameters(MSU_COMM_PARAM * param)
{
    msuMemorySet((MSU_UINT8*)&g_msuCommParam, 0, sizeof(MSU_COMM_PARAM));

    if(param == (MSU_COMM_PARAM*) MSU_NULL)
      {
        g_msuCommParam.ccmDelay = MSU_COMMPARAM_CCMDELAY;
        g_msuCommParam.ccmRetry = MSU_COMMPARAM_CCMRETRY;
        g_msuCommParam.commTimeout_sec = MSU_COMMPARAM_COMMTIMEOUT;
        g_msuCommParam.scmDelay = MSU_COMMPARAM_SCMDELAY;
        g_msuCommParam.scmRetry = MSU_COMMPARAM_SCMRETRY ;
        g_msuCommParam.seqDelay = MSU_COMMPARAM_SEQDELAY;
      }
    else
      {
        g_msuCommParam.ccmDelay = param->ccmDelay;
        g_msuCommParam.ccmRetry = param->ccmRetry;
        g_msuCommParam.commTimeout_sec = param->commTimeout_sec;
        g_msuCommParam.scmDelay = param->scmDelay;
        g_msuCommParam.scmRetry = param->scmRetry;
        g_msuCommParam.seqDelay = param->seqDelay;
      }
    return MSU_SUCCESS;

}

/**  @fn void msuScmMsgCreate( MSUSCMPdu *scmMsgPdu,MSU_UINT32 bitMap, MSU_UINT32 chunkNum, MSU_UINT8 missedSeqCount)
 *   @brief msuScmMsgCreate() function is used to construct SCM message format
 *   @param[in] : MSUSCMPdu *scmMsgPdu   SCM pdu data will be updated inside fuction
 *   @param[in] : MSU_UINT32 bitMap          sequence bit map value for particular chunk
 *   @param[in] : MSU_UINT32 chunkNum        chunk number
 *   @param[in] : MSU_UINT8 missedSeqCount   total number of missed sequence count in this particular chunk
 *   @return    : None
 *   @retval    : None
 */
void msuScmMsgCreate( MSUSCMPdu *scmMsgPdu,MSU_UINT32 bitMap, MSU_UINT32 chunkNum, MSU_UINT8 missedSeqCount)
{
    scmMsgPdu->msuHeader.command.opcode = MSU_OPCODE_UPGRADE;
	scmMsgPdu->msuHeader.command.subcode = MSU_SUBCODE_SCM;
	scmMsgPdu->msuHeader.fileNumber = msuHtoNs( g_msuBroadcastPdu.msuSyncHeaderformat.msuHeader.fileNumber);
	scmMsgPdu->msuHeader.versionInfo.versionIP = MSU_IPV4_VERSION;
	scmMsgPdu->msuHeader.versionInfo.versionProtocol=MSU_PROTOCOL_VERSION;
	scmMsgPdu->chunkNumber = msuHtoNl(chunkNum);
	scmMsgPdu->seqCount = missedSeqCount;
	scmMsgPdu->seqBitMap = msuHtoNl(bitMap);
}


/**  @fn MSUGroupCreationPdu * msuGroupAckMsgCreate( MSUGroupCreationPdu *gReqPdu )
 *   @brief msuGroupAckMsgCreate() is used to construct group creation ack message format
 *   @param[in] : MSUGroupCreationPdu *gReqPdu   group creation pointer pdu data will be updated inside the function.   
 *   @return    : returns group creation ack pdu pointer
 *   @retval    : PDU pointer of type MSUGroupCreationPdu *
 */
MSUGroupCreationPdu * msuGroupAckMsgCreate( MSUGroupCreationPdu *gReqPdu )
{
    gReqPdu->command.subcode   = MSU_SUBCODE_ACK_GROUP;
	gReqPdu->groupID           = gGroupID;
	return(gReqPdu);
}

/**  @fn MSU_UINT8 msuDisconnectMsg(MSU_UINT8 *gMsg,MSU_UINT32 msgsize,MSUDisconnectPdu var)
 *   @brief msuDisconnectMsg() function is used to parse disconnect message
 *   @param[in] : MSU_UINT8 *gMsg   This points to the global buffer containing the data
 *   @param[in] : MSU_UINT32 msgsize This variable gives the length of the message read
 *  @param[in] : MSUDisconnectPdu *localDisconnectpdu This points to the disconnect PDU
 *   @return    : NONE  
 */
 MSU_UINT8 msuDisconnectMsg(MSU_UINT8 *gMsg,MSU_UINT32 msgSize,MSUDisconnectPdu *msuDisconnectpdu )
{
    
	MSU_UINT32 result =MSU_FALSE;
	MSU_UINT32 length_privatekey =0;

	/*Parse only when the message size is the same as that of the recieving pdu*/
	if(msgSize==sizeof(MSUDisconnectPdu))
	{
	
	msuByteCopy((MSU_UINT8*)msuDisconnectpdu, (MSU_UINT8*)gMsg, sizeof(MSUDisconnectPdu));
	
	
	/*Check for Server IP mismatch and set the global variable*/
		
	if(!msuDisconnectpdu->versionInfo.versionIP)
	{
	
	   if(msuNtoHl(msuDisconnectpdu->serverIp.ServerIPv4)!=g_Header.msuServerIP.ServerIPv4)
	   {
		MsuGlobalError=MSU_SERVER_IP_MISMATCH;
		result= MSU_TRUE;
		return result;
	   }
				
	}
		
	else
	
	{
	
	   if(!msuStrNCmp((MSU_SINT8*)&msuDisconnectpdu->serverIp.ServerIPv6,(MSU_SINT8*)&g_Header.msuServerIP.ServerIPv6,(MSU_UINT32)MAX_BYTES_IPv6_ADDRESS))
	   {
	
		MsuGlobalError=MSU_SERVER_IP_MISMATCH;
		result= MSU_TRUE;
		return result;
	
	   }
	
	}	
	
	/*If the first condition fails check for level and process id mismatch and set the global variable*/
	length_privatekey = msuStrLen(g_msuDeviceInfo.key);
	
    msuDisconnectpdu->transactionId.transactionid = msuNtoHl(msuDisconnectpdu->transactionId.transactionid);
           
    msuDecryption((MSU_UINT8*)msuDisconnectpdu->transactionId.transactionencrypt,(char*)&g_msuDeviceInfo.key,sizeof(msuDisconnectpdu->transactionId),length_privatekey);
	
	if(msuDisconnectpdu->transactionId.transactionid==g_Header.transactionid.transactionid)
	{
				
		
	   if(	msuDisconnectpdu->authenticationlevel.Level!=g_msuDeviceInfo.AuthenticationLevelTag)
	   {
		
		MsuGlobalError=MSU_AUTHENTICATION_LEVEL_FAILURE;
		result= MSU_TRUE;
		return result;
	   }
	}
	else
	   {
		MsuGlobalError=MSU_TRANSACTION_ID_FAILURE;
		result= MSU_TRUE;
		return result;
	   }
	
								
	}


    MsuGlobalError=0;
	
	return result;
	
}
 
 
/**  @fn void msuDisconnectMsgResponseCreate(MSUAuthenticationResponsePdu *gReqPdu)
 *   @brief msuDisconnectMsgResponseCreate() function is used to create the disconnect response
 *   @param[in] : MSUAuthenticationResponsePdu *gReqPdu This points to the global buffer containing the data
 *   @return    : void 
 */
 void msuDisconnectMsgResponseCreate( MSUAuthenticationResponsePdu *msuAuthenticationResponsePdu )
{
   
      
   msuAuthenticationResponsePdu->command.subcode   = MSU_SUBCODE_DISCONNECT_RESPONSE;
   
   /*Build the response*/
   if((MsuGlobalError==MSU_SERVER_IP_MISMATCH)||(MsuGlobalError==MSU_AUTHENTICATION_LEVEL_FAILURE)||(MsuGlobalError==MSU_TRANSACTION_ID_FAILURE))
   {
   	
   	msuAuthenticationResponsePdu->ErrCode =MSU_TRUE;
    msuAuthenticationResponsePdu->ErrSubCode =MsuGlobalError;
   	
   }
   else
   {
   		msuAuthenticationResponsePdu->ErrCode =MSU_FALSE;
   		msuAuthenticationResponsePdu->ErrSubCode =MSU_FALSE;
   		
   }

}

/**  @fn MSU_BOOL msuHandleAbortMsg(MSU_UINT8* g_msgbuffer)
 *   @brief msuHandleAbortMsg() function is used to handle abort message
 *   @param[in] : MSUAuthenticationResponsePdu *gReqPdu This points to the global buffer containing the data
 *   @return    : void 
 */
 MSU_BOOL msuHandleAbortMsg(MSU_UINT8* g_msgbuffer )
 {
 	MSU_BOOL result = MSU_FALSE; 
 	MSUAbortMsgPdu localcopy = {0};
 	MSU_UINT8 length_privatekey =0;
 	
 	msuByteCopy((MSU_UINT8*)&localcopy,(MSU_UINT8*)g_msgbuffer,sizeof(MSUAbortMsgPdu));
 	
 	length_privatekey = msuStrLen(g_msuDeviceInfo.key);	
 	
    localcopy.transactionID.transactionid = msuNtoHl(localcopy.transactionID.transactionid);         
    msuDecryption((MSU_UINT8*)localcopy.transactionID.transactionencrypt,(char*)&g_msuDeviceInfo.key,sizeof(localcopy.transactionID),length_privatekey);
    if(localcopy.transactionID.transactionid==g_Header.transactionid.transactionid)
	
	result = MSU_TRUE;
    return result;
 	
 }
 

