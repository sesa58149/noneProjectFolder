/**	@file MsuLeech.cpp
 *	<TABLE>
 *	@brief Implementation of MSU Leech
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
 *	<TD>Deepak G</TD>	<TD>03-Oct-2011</TD>	<TD>First creation</TD>
 *	</TR>
 *
 * 	</TABLE>
 *	</TABLE>
 */

#ifndef MSU_LEECH_CPP
#define MSU_LEECH_CPP



#include "MsuDataTypes.h"
#include "MsuFileInterface.h"
#include "MsuPdu.h"
#include "MsuSocket.h"
#include "MsuCRC32.h"
#include "MsuComplainMode.h"
#include "MsuLeech.h"
#include "MsuFileOperation.h" 

MSU_EXTERN MSU_UINT32 g_missedSeqBitMap; 

#ifdef CHUNK_IN_RAM 
	static MSU_UINT8 *pChunkBuffer=MSU_NULL;
#endif

static MSU_UINT16 g_localSeqNumber;
static MSU_UINT32 g_localChunkNumber;
static MSU_UINT16 g_sequenceBitMap;
static MSU_UINT32 g_partialChunkSize;
static EMSUErr eMsuStatus;
static MSU_UINT32 g_chunkDataSize;
static MSU_UINT32 g_lastChunkSize;
static MSU_UINT32 noOfSeqInLastChunk;
static MSU_SINT8 filePath[MSU_MAX_FILE_PATH_SIZE];
static MsuInfo *g_info_array=MSU_NULL;
static MSU_UINT16 g_chunkSize =0;

static EMSUState eMsuState     = eMsuStateInitialize;
static EMSUState eMsuPrevState = eMsuStateRunning;
static EMSUUpgradeState deviceUpgadeStatus = eMsuUpgradeFailed;

static MSU_UINT8 firstTime = 1;
static MSU_BOOL ccmSent = MSU_FALSE;
static MSU_UINT8 ccmRetryCount = 0;
static MSU_UINT8 scmRetryCount = 0;

static MSU_UINT32 ccm_seqVal[MSU_MAX_CCM_CHUNK_NUMBER];
static MSU_SINT32 pMsuFileDes;

MSU_UINT8   MsuGlobalError = 0;


static void msuLeechInit(void);
static MSU_SINT32 msuLeechConfigure(void);
static void msuHandleMcastPacket(void);
static void msuHandleScmMcastPacket(void);
static void msuHandleCcmMcastPacket(void);
static void msuReleaseResources(void);
MSU_SINT32 msuDefaultSocketConfig();



MSU_SINT32 msuDefaultSocketConfig()
{
   return msuDefaultSocketCreate();
}


/**  @fn void msuLeechInit(void)
 *   @brief msuLeechInit() is used to initialize a variables with default values  
 *   @details  msuLeechInit() function is used intialize socket related variables, complain mode variables, 
 *             protocol data unit variables and global varables with default values.
 *             This function also creates a chunk buffer dynamically which is used to store chunk information
 *             before writing to a file.   
 *   @param[in] : None
 *   @return    : None 
 *   @retval    : None
 */            
void msuLeechInit(void)
{
 
   msuPduInit(); 
   msuSocketInit( );
   msuRegisterCallBackMethod(0,&msuChangeDeviceState,MSU_NULL);
   crcInit();
   complainModeInit();
   
   g_localSeqNumber   = 1;
   g_localChunkNumber = 1;
   g_sequenceBitMap   = 0;
   g_partialChunkSize = 0;
   g_chunkDataSize    = 0; 
   g_lastChunkSize    = 0;

   /* Set communication param to default*/
   msuSetCommunicationParameters(MSU_NULL);

   /* Set device Indentity to default*/
   setMsuDeviceIdentification(MSU_NULL);

   msuMemorySet((MSU_UINT8*)filePath, 0, MSU_MAX_FILE_PATH_SIZE);

   #ifdef CHUNK_IN_RAM 
   pChunkBuffer = (MSU_UINT8 *) msuGetMemoryFromPool(sizeof(MSU_UINT8) * MSU_MAX_SEQ_NUMBER * MSU_MAX_SEQ_SIZE);   
   if( pChunkBuffer == MSU_NULL)
   {
		msuLogMsg( MSU_DEBUG_LEVEL_CRITICAL, "\n MSU failed to get memory pool for Chunk Buffer\n" );				
		eMsuStatus = EMSUErrBufferPool;
		return;
	}
   msuMemorySet(pChunkBuffer, 0, ( sizeof(MSU_UINT8) * MSU_MAX_SEQ_NUMBER * MSU_MAX_SEQ_SIZE));
   g_info_array = (MsuInfo *)pChunkBuffer;
   #endif   
   
   eMsuStatus = EMSUErrSuccess;
}

/**  @fn MSU_SINT32 msuLeechConfigure(void)
 *   @brief msuLeechConfigure() is used to configure MSU client with the configuration received in Sync message
 *   @details  When MSU client receives Sync message from server, sync message contains configuration information 
 *             like File Number, Number of chunks,sequence number limit, Multicast port and IP address,file name,
 *             dest path name and CRC. using this parameter msuLeechConfigure( ) open a file with the specified 
 *             file name and creates a updated socket with specified Multicast IP address and port number. 
 *   @param[in] : None 
 *   @return MSU_SOCKET_SUCCESS, if the operation is successful
 *           MSU_SOCKET_ERROR code if the operation is failed.
 *   @retval 0 if created successfully
 *   @retval -1 if failed 
 */
MSU_SINT32 msuLeechConfigure(void)
{
   MSU_SINT32 status = MSU_SOCKET_ERROR;
   MSU_UINT8 pathOffset = 0;

   #ifdef CHUNK_IN_RAM 
   if(pChunkBuffer != MSU_NULL)
   {
   #endif	
      if((g_msuBroadcastPdu.msuSyncHeaderformat.destPathTag.isFieldPresent) &&
		   (g_msuBroadcastPdu.msuSyncHeaderformat.destPathTag.fieldLength != 0))
		{
		   //Copy the filepath
		   msuByteCopy((MSU_UINT8*)filePath,(MSU_UINT8*) g_msuBroadcastPdu.msuSyncMessageVariableData.destPath, g_msuBroadcastPdu.msuSyncHeaderformat.destPathTag.fieldLength);
		   pathOffset += g_msuBroadcastPdu.msuSyncHeaderformat.destPathTag.fieldLength;
		}		
		if((g_msuBroadcastPdu.msuSyncHeaderformat.fileNameTag.isFieldPresent) &&
		   (g_msuBroadcastPdu.msuSyncHeaderformat.fileNameTag.fieldLength != 0))
		{
		   //Copy the filename
		   msuByteCopy((MSU_UINT8*)&filePath[pathOffset], (MSU_UINT8*)g_msuBroadcastPdu.msuSyncMessageVariableData.fileName,g_msuBroadcastPdu.msuSyncHeaderformat.fileNameTag.fieldLength);
		}
		else
		{
			msuLogMsg( MSU_DEBUG_LEVEL_NORMAL,"\n File Opening fails-> No FileName\n");
			return( MSU_SOCKET_ERROR);
		}	
		
		msuLogMsg(MSU_DEBUG_LEVEL_SUCCESS, "\nOpening file %s\n", filePath);
		pMsuFileDes = msuFileOpen(filePath, eMsuFileW);	
		if( pMsuFileDes == FILE_DESC_ERROR)
		{
			MsuGlobalError = MSU_FILE_OPEN_FAILED;
		    msuLogMsg( MSU_DEBUG_LEVEL_CRITICAL, "\nFile open failed at %s\n", filePath);
			return( MSU_SOCKET_ERROR);			
		}

		//Calculate the last chunk size
		g_lastChunkSize = (MSU_UINT32) (g_msuBroadcastPdu.msuSyncHeaderformat.fileSize % ((MSU_UINT32)(g_msuBroadcastPdu.msuSyncHeaderformat.seqNumberLimit * g_msuBroadcastPdu.msuSyncHeaderformat.seqSize)));
			
		msuLogMsg( MSU_DEBUG_LEVEL_SUCCESS,"\nLast chunk size  = %ld\n", g_lastChunkSize);
				
		if(g_lastChunkSize%g_msuBroadcastPdu.msuSyncHeaderformat.seqSize)
		{
	       noOfSeqInLastChunk = g_lastChunkSize/g_msuBroadcastPdu.msuSyncHeaderformat.seqSize + 1;
		}
		else
		{
		   noOfSeqInLastChunk = g_lastChunkSize/g_msuBroadcastPdu.msuSyncHeaderformat.seqSize;
		}
		/* Setup socket for update process */
		g_localChunkNumber = 1;
		
		if(!g_msuBroadcastPdu.msuSyncHeaderformat.msuHeader.versionInfo.versionIP)
		{
		   status = msuUpdateSocketCreate(g_msuBroadcastPdu.msuSyncHeaderformat.multicastPort, msuNtoHl(g_msuBroadcastPdu.msuSyncHeaderformat.multicastIP.ServerIPv4) );
		}
		else
		{
		   //Handle IPv6 address
		}
	
		if(status == MSU_SOCKET_ERROR)
		{
		   MsuGlobalError = MSU_UPDATE_SOCKET_OPEN_FAILED;		   		
		   msuLogMsg( MSU_DEBUG_LEVEL_ERROR, "\nSetting up multicast socket failed for port = %d and ip = %lx\n",
		   g_msuBroadcastPdu.msuSyncHeaderformat.multicastPort,
		   g_msuBroadcastPdu.msuSyncHeaderformat.multicastIP);		   	
       	   return (status);			 
		}
#ifdef CHUNK_IN_RAM 		
   }
	else
	{
	    msuLogMsg(MSU_DEBUG_LEVEL_CRITICAL,"\n MSU can not process Notification as Memory pool failed for chunk buffer\n");			 
	}
#endif	
	
	msuMemorySet( (MSU_UINT8*)ccm_seqVal,0, sizeof( MSU_UINT32) * MSU_MAX_CCM_CHUNK_NUMBER );
	ccmRetryCount  = 0;
	MsuGlobalError = 0;
	ccmSent = MSU_FALSE;	
	eMsuPrevState = eMsuStateRunning;
	return status;
}

/**  @fn void msuReleaseResources(void)
 *   @brief msuReleaseResources() is used to release resources
 *   @details  msuReleaseResources() function releases the resources like closing the existing sockets 
 *             and closing the opened file and initialize the chunk buffer with 0
 *   @param[in] : None
 *   @return      None
 *   @retval  None
 */
void msuReleaseResources(void)
{
#ifdef CHUNK_IN_RAM 
   if(pChunkBuffer != MSU_NULL)
	{	   
	   msuReleaseMemoryToPool((MSU_UINT8*)pChunkBuffer);	 
	   pChunkBuffer = MSU_NULL;	   
	}
#endif	
   msuSocketRelease();
   complainModeInit( );

   if( pMsuFileDes != FILE_DESC_ERROR)
	msuFileClose(pMsuFileDes);

   g_localSeqNumber   = 0;
   g_localChunkNumber = 0;
   g_sequenceBitMap   = 0;
   g_partialChunkSize = 0;
   g_chunkDataSize    = 0;
   g_lastChunkSize    = 0;

   msuMemorySet((MSU_UINT8*)filePath, 0, MSU_MAX_FILE_PATH_SIZE);

   firstTime = 1;	
}


/**  @fn void  msuChangeDeviceState( EMSUState presentState )
 *   @brief msuChangeDeviceState() is used to change device state machine state
 *   @param[in] : EMSUState presentState   
 *   @return   None
 *   @retval socket descriptor ( integer value) if created successfully
 */
void  msuChangeDeviceState( EMSUState emsustate )
{
	eMsuState = emsustate;
}

/**  @fn EMSUState msuCurrentState()
 *   @brief msuCurrentState() is used to return the current state of the MSU state machine
 *   @param[in] : None
 *   @return returns EMSUState
 */

EMSUState msuCurrentState()
{
   return eMsuState;
}

/**  @fn EMSUUpgradeState  msuGetUpgradeProcessState()
 *   @brief msuGetUpgradeProcessState() is used to get the upgrade process state
 *   @param[in] : None 
 *   @return EMSUUpgradeState   upgrade state
 *   @retval eMsuUpgradePassed or eMsuUpgradeFailed or  eMsuUpgradeInProgress
 */
	
EMSUUpgradeState  msuGetUpgradeProcessState()
{
   return deviceUpgadeStatus;
}

/**  @fn void msuHandleScmMcastPacket( )
 *   @brief msuHandleScmMcastPacket() function is used to receive missed sequence packets in that particular chunk
 *   @details  After reception of each chunk, client checks whether any sequences missed in that chunk, if missed 
 *             then client enters into SCM mode and waits to received missed sequence packets from server. 
 *             This function receives missed sequence packets and write into a file. If all missed sequences are received
 *             or sequence packet received from different current chunk number other than current chunk then write the current
 *             chunk in file and changes the state to Running or CCM state depends on previous contained state
 *   @param[in] : None
 *   @return None
 */

void msuHandleScmMcastPacket( )
{
   MSU_UINT8 noOfSeq = 0;
   
   MSU_UINT8 result=MSU_ERROR;

	/* We are in SCM mode: check whether we have received the packet within the same chunk number*/
   if(g_localChunkNumber == g_msuMcastPdu.chunkNumber)
	{	 
	   if(isSeqRecvd(g_msuMcastPdu.seqNumber) == MSU_FALSE)
	   {	      
		    msuLogMsg( MSU_DEBUG_LEVEL_SUCCESS, "\nSCM Rx : CN %d SN %d\n", g_msuMcastPdu.chunkNumber, g_msuMcastPdu.seqNumber);
            /*Copy the data into the buffer/File*/ //dataLength
 #ifdef CHUNK_IN_RAM
 	  
 	    	msuByteCopy((MSU_UINT8*)g_info_array[g_msuMcastPdu.seqNumber - 1].Info,(MSU_UINT8*)g_msuMcastPdu.pData,g_msuMcastPdu.dataLength);
 		
 			g_chunkSize+=g_msuMcastPdu.dataLength;
 			
 		
 #else			
         if( msuFileSeek(&pMsuFileDes,(MSU_UINT32)( (	(g_msuMcastPdu.chunkNumber - 1) * 
         	(g_msuBroadcastPdu.msuSyncHeaderformat.seqNumberLimit * g_msuBroadcastPdu.msuSyncHeaderformat.seqSize) ) +
          	( (g_msuMcastPdu.seqNumber - 1) * ( g_msuBroadcastPdu.msuSyncHeaderformat.seqSize) ) ) )	== MSU_TRUE )			   
			{
         
				if( msuFileWrite(pMsuFileDes,(MSU_SINT8 *)g_msuMcastPdu.pData, g_msuMcastPdu.dataLength) == MSU_SUCCESS)
				{				           	
				   msuLogMsg(MSU_DEBUG_LEVEL_SUCCESS,"\n SCM CN %ld and SN %d Writing at location from SCM %ld\n",
				   g_msuMcastPdu.chunkNumber, g_msuMcastPdu.seqNumber,f_tell((FS_FILE*)pMsuFileDes));					   
				}		
				else
				{
					MsuGlobalError = MSU_FILE_WRITE_FAILED;
					msuLogMsg( MSU_DEBUG_LEVEL_ERROR, "\nSCM =====>>FILE write error\n");
					/* Drop current sequence and move for next one*/			
					return;			
				}
			}		
 #endif  					  
			g_localSeqNumber = g_msuMcastPdu.seqNumber;
				  
			setRecvdSeqBit(g_localSeqNumber);
				  
			if( g_msuBroadcastPdu.msuSyncHeaderformat.chunkCount == g_localChunkNumber)
				noOfSeq = noOfSeqInLastChunk;
			else
				noOfSeq = g_msuBroadcastPdu.msuSyncHeaderformat.seqNumberLimit;				  
				   
		   if( getMissedSeqBitMap(noOfSeq) == 0 )				  
			   {
				   setRecvdChnkBit(g_localChunkNumber);
				#ifdef CHUNK_IN_RAM			
					msuLogMsg( MSU_DEBUG_LEVEL_SUCCESS,"\nSCM pckt CN %ld and SEQBITS %x\n",g_localChunkNumber,getSeqBitMap( ));					
					result=msuFileWrite(pMsuFileDes,(MSU_SINT8 *)g_info_array,(MSU_UINT16)g_chunkSize) ;				   
					if(result!=MSU_SUCCESS)
					{				
						MsuGlobalError = MSU_FILE_WRITE_FAILED;					   
						msuLogMsg( MSU_DEBUG_LEVEL_ERROR, "\n File write operation Fail\n");					   			
					}
					msuMemorySet((MSU_UINT8*)g_info_array,0,MSU_MAX_SEQ_NUMBER*sizeof( MsuInfo));					
					g_chunkSize=0;						
    #endif						   
				    g_localSeqNumber = 0;
				    clearSeqBitMap();
				    g_chunkDataSize=0;
				    g_localChunkNumber++;
				    if(getMissedChnkBitMap(g_msuBroadcastPdu.msuSyncHeaderformat.chunkCount) == 0)
				    {					 
					    msuLogMsg( MSU_DEBUG_LEVEL_SUCCESS,"\nMSU Done state\n");					   				
						eMsuState = eMsuStateDone;						   
				    }
				    else 
				    {					  
						eMsuState = eMsuPrevState; 
				    }
					   	
				}     		
			
			}
		}   
	   else /* Receive wrong chunk*/
	   {
		   /*Reset the parameters for new chunk*/		
			msuLogMsg( MSU_DEBUG_LEVEL_SUCCESS, "\n chunk Jump over SCM pckt CN %ld and SEQBITS %x\n",g_localChunkNumber,getSeqBitMap( ));
					
			ccm_seqVal[g_localChunkNumber -1] = getSeqBitMap( );			   
			g_localSeqNumber = 0;
			g_chunkDataSize  = 0;
			clearSeqBitMap();
			g_localChunkNumber = g_msuMcastPdu.chunkNumber;

			if(isSeqRecvd(g_msuMcastPdu.seqNumber) == MSU_FALSE)
			{
		#ifdef CHUNK_IN_RAM
		
			   msuByteCopy((MSU_UINT8*)g_info_array[g_msuMcastPdu.seqNumber-1].Info,(MSU_UINT8*)g_msuMcastPdu.pData,((sizeof(MSU_UINT8))*g_msuMcastPdu.dataLength));
			
			   g_chunkSize+=g_msuMcastPdu.dataLength;			
		
		#else
			  if( msuFileSeek(&pMsuFileDes,(MSU_UINT32)( (	(g_msuMcastPdu.chunkNumber - 1) * 
					(g_msuBroadcastPdu.msuSyncHeaderformat.seqNumberLimit * g_msuBroadcastPdu.msuSyncHeaderformat.seqSize) ) +
				 ( (g_msuMcastPdu.seqNumber - 1) * ( g_msuBroadcastPdu.msuSyncHeaderformat.seqSize) ) ) )	== MSU_TRUE )			   
				{
						
					if( msuFileWrite(pMsuFileDes,(MSU_SINT8 *)g_msuMcastPdu.pData, g_msuMcastPdu.dataLength) == MSU_SUCCESS)
					{
						msuLogMsg( MSU_DEBUG_LEVEL_SUCCESS,"\n Jump Over SCM CN %ld and SN %d Writing at location from SCM %ld\n",
						g_msuMcastPdu.chunkNumber, g_msuMcastPdu.seqNumber,f_tell((FS_FILE*)pMsuFileDes));								
						/*Set the sequence bitmask*/
						setRecvdSeqBit(g_msuMcastPdu.seqNumber);														   
						eMsuState = eMsuPrevState;									   					
					}
					else
					{
						MsuGlobalError = MSU_FILE_WRITE_FAILED;						
						msuLogMsg( MSU_DEBUG_LEVEL_ERROR, "\nSCM =====>>FILE write error\n");
						
					}
					
				}
		#endif // #ifdef CHUNK_IN_RAM    
		} // if(isSeqRecvd(g_msuMcastPdu.seqNumber) == MSU_FALSE)
   }
}


/**  @fn void msuHandleCcmMcastPacket(void)
 *   @brief void msuHandleCcmMcastPacket(void) function is used to receive missed chunks from the server
 *   @details  After file transfer completes, client checks whether any chunks are missed in that file, if chunks missed 
 *             then client enters into CCM mode and waits to received missed chunks from server. 
 *             After receiving each chunk it checks for missed sequences in the chunk, if any then it enters into SCM state,
 *             after receiving missed sequences it comes back to CCM state to receive other missed chunks  
 *   @param[in] : None
 *   @return    : None
 */
void msuHandleCcmMcastPacket(void)
{
   MSU_UINT32  g_missedSeqBitMap = 0;
   MSU_UINT8 missedSeqCount = 0;
   MSU_UINT8 res=0;
   

	msuLogMsg( MSU_DEBUG_LEVEL_SUCCESS,"\nRx CCM CN %ld and SN %d \n",g_msuMcastPdu.chunkNumber, g_msuMcastPdu.seqNumber);
   
	if(isChnkRecvd(g_msuMcastPdu.chunkNumber) == MSU_FALSE)
	{
	   if(firstTime)
		{
		   firstTime=0;
		   g_localChunkNumber = g_msuMcastPdu.chunkNumber;
		   /*Reset the received sequence bit map*/
		   clearSeqBitMap();
		   setSeqBitMap(ccm_seqVal[g_localChunkNumber -1] );
		}

	 	if(g_localChunkNumber != g_msuMcastPdu.chunkNumber)
		{	
		   /*Save the sequences information received for current chunk*/			
		   ccm_seqVal[g_localChunkNumber -1] = getSeqBitMap( );	
		   g_localChunkNumber = g_msuMcastPdu.chunkNumber;
			
		   /*Reset the received sequence bit map*/
		   clearSeqBitMap();
		   setSeqBitMap(ccm_seqVal[g_localChunkNumber -1] );
			
		#ifdef CHUNK_IN_RAM
 
		   g_chunkSize=0;
		   msuFileSeek(&pMsuFileDes,(MSU_UINT32)( ((g_msuMcastPdu.chunkNumber - 1) * 
		   (g_msuBroadcastPdu.msuSyncHeaderformat.seqNumberLimit * g_msuBroadcastPdu.msuSyncHeaderformat.seqSize) )) );
		
		#endif

		}
		if( isSeqRecvd( g_msuMcastPdu.seqNumber) == MSU_TRUE)
		{                   
		    msuLogMsg( MSU_DEBUG_LEVEL_NORMAL, "\nCCm Already received %ld and SN %d \n",g_msuMcastPdu.chunkNumber, g_msuMcastPdu.seqNumber); 	         
			return;		   
		}
		 

#ifdef CHUNK_IN_RAM
	
		msuByteCopy((MSU_UINT8*)g_info_array[g_msuMcastPdu.seqNumber -1].Info,(MSU_UINT8 *)g_msuMcastPdu.pData,g_msuMcastPdu.dataLength);
		
		g_chunkSize += g_msuMcastPdu.dataLength;
    	msuLogMsg( MSU_DEBUG_LEVEL_SUCCESS, "nCCM pckt CN %ld and SEQBITS %x  and the chunksize %ld \n",g_msuMcastPdu.chunkNumber,getSeqBitMap( ),g_chunkSize);
		
#else		
		msuFileSeek(&pMsuFileDes,(MSU_UINT32)( ((g_msuMcastPdu.chunkNumber - 1) *(g_msuBroadcastPdu.msuSyncHeaderformat.seqNumberLimit * g_msuBroadcastPdu.msuSyncHeaderformat.seqSize) ) ) ); 

		res=msuFileWrite(pMsuFileDes,(MSU_SINT8 *)g_msuMcastPdu.pData, g_msuMcastPdu.dataLength);		 		
		if(res!=MSU_SUCCESS)
		{
		
			MsuGlobalError = MSU_FILE_WRITE_FAILED;
			msuLogMsg( MSU_DEBUG_LEVEL_ERROR, "\nCCM =====>>FILE write error\n");
		}	
#endif 

      	/*Set the sequence bit mask*/
      	setRecvdSeqBit(g_msuMcastPdu.seqNumber);
      	if(g_msuMcastPdu.fileTransferInfo.chunkTransferState == 1)
      	{
      		g_missedSeqBitMap = getMissedSeqBitMap(g_msuMcastPdu.seqNumber);
      		if(g_missedSeqBitMap == 0)
      		{               
      			msuLogMsg(MSU_DEBUG_LEVEL_SUCCESS, "\nCCM Writing CN %ld of size %ld into file\n",g_localChunkNumber, g_chunkDataSize);

      			msuLogMsg(MSU_DEBUG_LEVEL_SUCCESS, "\nCCM pckt CN %ld and SEQBITS %x\n",g_msuMcastPdu.chunkNumber,getSeqBitMap( ));
               
#ifdef CHUNK_IN_RAM	
      			/*Set chunk bit*/
				/* Essentially for regular files only*/
				msuFileSeek(&pMsuFileDes,(MSU_UINT32)( ((g_msuMcastPdu.chunkNumber - 1) *(g_msuBroadcastPdu.msuSyncHeaderformat.seqNumberLimit * g_msuBroadcastPdu.msuSyncHeaderformat.seqSize) ) ) ); 
				
				res=msuFileWrite(pMsuFileDes,(MSU_SINT8 *)g_info_array, (MSU_UINT16)g_chunkSize);
				
				msuMemorySet((MSU_UINT8*)g_info_array,0,MSU_MAX_SEQ_NUMBER*sizeof( MsuInfo));	
						
				g_chunkSize=0;
              
				msuLogMsg( MSU_DEBUG_LEVEL_SUCCESS, "\n In-CCM  CCM CN%ld and Writing at location%ld\n",g_msuMcastPdu.chunkNumber, msuFileTell(pMsuFileDes) );
								
				if(res!=MSU_SUCCESS)
				{					
				    MsuGlobalError = MSU_FILE_WRITE_FAILED;                   
					msuLogMsg( MSU_DEBUG_LEVEL_CRITICAL, "\nCCM =====>>FILE write error\n");								
				}	
      			
#endif							
				setRecvdChnkBit(g_localChunkNumber);      		
      			
      			/* Check All the the missed chunk received*/
      			if(getMissedChnkBitMap(g_msuBroadcastPdu.msuSyncHeaderformat.chunkCount) == 0)
      				eMsuState = eMsuStateDone;
      			else
      				firstTime =1;
      		}
      		else /* Go SCM there is some sequence missed in current chunk*/
      		{  
				msuLogMsg(MSU_DEBUG_LEVEL_ERROR, "\nCCM pckt CN %ld and SEQBITS %x\n",g_msuMcastPdu.chunkNumber,getSeqBitMap( ));
			
				ccm_seqVal[g_localChunkNumber -1] = getSeqBitMap( );
				missedSeqCount = getMissedSeqCount(g_missedSeqBitMap );
				if(msuSendScmMsg(g_missedSeqBitMap,g_msuMcastPdu.chunkNumber,missedSeqCount) == MSU_TRUE)
      			{                            
			        msuLogMsg( MSU_DEBUG_LEVEL_SUCCESS, "\nUnicast SCM Message Sent\n");				   
					eMsuState = eMsuStateSCM;      			   
					msuLogMsg(MSU_DEBUG_LEVEL_SUCCESS,"\nCCM =====>> SCM\n");					
					eMsuPrevState = eMsuStateCCM;
					return;
      			}
      		}
      	}
	}
}


/**  @fn void msuHandleMcastPacket(void)
 *   @brief void msuHandleMcastPacket(void) function is used to receive all the chunks from server
 *   @details  When server Intiate data transfer, msuHandleMcastPacket( ) receives all the sequences for each chunk
 *             after reciving each chunk it checks for missed sequences in the chunk, if any then it sends SCM message to 
 *             server with missed missed sequence numbers then enters ito SCM mode. After coming back from SCM mode it set the 
 *             chunk number bit map arrey if all sequences received for that particulat chunk.
 *             Then it again starts to receive next chunk number from server till the end of file reached    
 *   @param[in] : None
 *   @return    : None
 */

void msuHandleMcastPacket(void)
{
   MSU_UINT8 missedSeqCount = 0;
   MSU_UINT16 ChunkSize_local=0;
   MSU_UINT8 result=MSU_ERROR;
   MSU_UINT8 i;   
  
	msuLogMsg( MSU_DEBUG_LEVEL_SUCCESS, "\nRx Update Mcast pckt CN %ld and SN %d\n",g_msuMcastPdu.chunkNumber,g_msuMcastPdu.seqNumber);
   
	if( g_localChunkNumber > g_msuMcastPdu.chunkNumber)
	{
		msuLogMsg( MSU_DEBUG_LEVEL_ERROR, "\n MCast CN %ld and SN %d Return\n",g_msuMcastPdu.chunkNumber, g_msuMcastPdu.seqNumber);
		return;
	}    
	
	msuLogMsg( MSU_DEBUG_LEVEL_SUCCESS,"\n Chunknumber = %ld\n",g_msuMcastPdu.chunkNumber);
	msuLogMsg( MSU_DEBUG_LEVEL_SUCCESS, "\n chunkTransferState = %d\n",g_msuMcastPdu.fileTransferInfo.chunkTransferState);
	msuLogMsg( MSU_DEBUG_LEVEL_SUCCESS, "\n fileTransferState = %d\n",g_msuMcastPdu.fileTransferInfo.fileTransferState);
	
	if(g_msuMcastPdu.chunkNumber==1 && g_msuMcastPdu.seqNumber==1)
	{
		g_chunkSize=0;   	
	}
	
	if(g_localChunkNumber != g_msuMcastPdu.chunkNumber)
	{
		ccm_seqVal[g_localChunkNumber -1] = getSeqBitMap( );		
		clearSeqBitMap();
		g_localChunkNumber = g_msuMcastPdu.chunkNumber;   		
   		msuLogMsg(MSU_DEBUG_LEVEL_ERROR, "\nChunk num not right \n");	
			
	#ifdef CHUNK_IN_RAM
		msuFileSeek(&pMsuFileDes,(MSU_UINT32)( (	(g_msuMcastPdu.chunkNumber - 1) * 
	        	(g_msuBroadcastPdu.msuSyncHeaderformat.seqNumberLimit * g_msuBroadcastPdu.msuSyncHeaderformat.seqSize) )));
		g_chunkSize=0;
	#endif
		
	}	
#ifdef CHUNK_IN_RAM 	
            
	msuByteCopy((MSU_UINT8*)g_info_array[g_msuMcastPdu.seqNumber - 1].Info,(MSU_UINT8*)g_msuMcastPdu.pData, g_msuMcastPdu.dataLength );
	
	g_chunkSize  += g_msuMcastPdu.dataLength;			
 	
#else	
	
	if(msuFileSeek(&pMsuFileDes,(MSU_UINT32)( ((g_msuMcastPdu.chunkNumber - 1) * 
		(g_msuBroadcastPdu.msuSyncHeaderformat.seqNumberLimit * g_msuBroadcastPdu.msuSyncHeaderformat.seqSize) ) + 
		( (g_msuMcastPdu.seqNumber -1) * ( g_msuBroadcastPdu.msuSyncHeaderformat.seqSize) ) ) ) == MSU_FALSE)
	{  		
		msuLogMsg(MSU_DEBUG_LEVEL_ERROR,  "Fseek fail in Sequence jump over\n");		
	}
	if( msuFileWrite(pMsuFileDes,(MSU_SINT8 *)g_msuMcastPdu.pData, g_msuMcastPdu.dataLength) == MSU_SUCCESS )
	{
		msuLogMsg( MSU_DEBUG_LEVEL_SUCCESS, "\n MCast CN %ld and SN %d Writing at location%ld\n",g_msuMcastPdu.chunkNumber,g_msuMcastPdu.seqNumber,f_tell((FS_FILE*)pMsuFileDes));
	}		
	else
	{
		MsuGlobalError = MSU_FILE_WRITE_FAILED;        
		msuLogMsg( MSU_DEBUG_LEVEL_ERROR, "\n File write operation Fail\n"); 
	}
#endif		
		/*Set the sequence bitmask*/
		setRecvdSeqBit(g_msuMcastPdu.seqNumber);	
		/*Check for chunk end state*/
		if(g_msuMcastPdu.fileTransferInfo.chunkTransferState == 1)
		{
		   /*End of chunk, check whether we need to go into SCM*/
			g_missedSeqBitMap = getMissedSeqBitMap(g_msuMcastPdu.seqNumber);
			if(g_missedSeqBitMap != 0)
			{	
				msuLogMsg( MSU_DEBUG_LEVEL_NORMAL, "\n missedSeqBitMap = %lx\n", g_missedSeqBitMap );			
				missedSeqCount = getMissedSeqCount(g_missedSeqBitMap );			
				if(msuSendScmMsg(g_missedSeqBitMap,g_msuMcastPdu.chunkNumber,missedSeqCount) == MSU_TRUE)
				{					
					msuLogMsg( MSU_DEBUG_LEVEL_NORMAL,"\n missedSeqCount = %ld\n", missedSeqCount ); 			
					msuLogMsg( MSU_DEBUG_LEVEL_NORMAL,"\nUnicast SCM Message Sent CN %ld and SN %ld\n",g_msuMcastPdu.chunkNumber,g_msuMcastPdu.seqNumber);
					
					eMsuState = eMsuStateSCM;
					msuLogMsg(MSU_DEBUG_LEVEL_NORMAL, "\nRUNNING =====>> SCM\n");						
					eMsuPrevState = eMsuStateRunning;
					return;
				}//if(msuSendScmMsg(missedSeqBitMap,g_msuMcastPdu.chunkNumber,1) == MSU_TRUE)
			}//if(missedSeqBitMap != 0)
		else
		{		
			msuLogMsg( MSU_DEBUG_LEVEL_SUCCESS, "\nMCAST pckt CN %ld and SEQBITS %x\n",g_msuMcastPdu.chunkNumber,getSeqBitMap( ));		
		    	
		#ifdef CHUNK_IN_RAM		
			result=msuFileWrite(pMsuFileDes,( MSU_SINT8 * ) g_info_array,(MSU_UINT16)g_chunkSize) ;					
			if(result!=MSU_SUCCESS)
			{									
				MsuGlobalError = MSU_FILE_WRITE_FAILED;						
				
				msuLogMsg( MSU_DEBUG_LEVEL_ERROR,"\n File write operation Fail\n"); 													
				msuLogMsg( MSU_DEBUG_LEVEL_ERROR,"\n MCast CN %ld and SN %d Writing at location %ld\n",g_msuMcastPdu.chunkNumber,g_msuMcastPdu.seqNumber,msuFileTell(pMsuFileDes));
									
			}
			msuMemorySet((MSU_UINT8*)g_info_array,0,MSU_MAX_SEQ_NUMBER*sizeof( MsuInfo));		
			g_chunkSize=0;				
		#endif	//CHUNK_IN_RAM
		
			/*set appropriate CN bit mask as received*/
			setRecvdChnkBit(g_localChunkNumber);				
			g_localChunkNumber = g_localChunkNumber + 1;
				
			clearSeqBitMap();			
			}
			
			if(g_msuMcastPdu.fileTransferInfo.fileTransferState == 1)
			{
				/*Check whether we need to go into CCM mode : This check is performed on the last packet itself
				 * instead of waiting for upgrade complete message*/
				if(getMissedChnkBitMap(g_msuBroadcastPdu.msuSyncHeaderformat.chunkCount) == 0)
				{	/*All chunks received, proceed to CRC verification*/
					eMsuState = eMsuStateDone;								
					msuLogMsg(MSU_DEBUG_LEVEL_SUCCESS, "\nREADY =====>> DONE\n");											
				}
				else
				{
					if(msuSendCcmMsg(g_msuBroadcastPdu.msuSyncHeaderformat.chunkCount) == MSU_TRUE)
					{
						/* reset the parameter for chunk retransmission */
						g_localChunkNumber = 0;
						g_chunkDataSize = 0;
						ccmRetryCount++;		
						msuLogMsg( MSU_DEBUG_LEVEL_SUCCESS, "\nREADY =====>> CCM\n");								
						eMsuState = eMsuStateCCM;
						/* Make prevState to CCM so SCM can now jump to CCM*/
						eMsuPrevState = eMsuStateCCM;
					}	
				}					
			}//if(g_msuMcastPdu.fileTransferInfo.fileTransferState == 1)
		}//if(g_msuMcastPdu.fileTransferInfo.chunkTransferState == 1)	

}

/**  @fn void msuLeechStateMachine(void)
 *   @brief void msuLeechStateMachine(void) function is the main state machine function to handle MSU client
 *   @details  This is the main MSU client function , which follows state machine, initially it enters into initialize state
 *             to initalize all the variables related to client then it enters into configure state where it creates default
 *             multicast socket and enters into listen state where it blocks to receive multicast packet it may be discovery
 *             messag request ot update status request or sync message request, once it receives sync message it enters into 
 *             eMsuStateStart state and creates update server socket with the specified configuration received from sync
 *             message then it enters into eMsuStateRunning state to receive all chunks, depends on complain mode it enters 
 *             into SCM or CCM states to receive missed sequences or chunks. if everything is received it enters into done state
 *             and does check sum validation on the received file, sends status message to server, then it enters into loopback 
 *             state and releases all resources and goes back to configure state. if any problem occured then it enters into
 *             abort state from there it enter into loopback then reeases resources then enters into configure state
 *
 *             
 *   @param[in] : None
 *   @return    : None
 */

void msuLeechStateMachine(void)
{
	MSU_BOOL status = MSU_FALSE;

	while(eMsuStatus == EMSUErrSuccess)
	{
		switch(eMsuState)
		{
			case eMsuStateInitialize:
				/*Initialize MSU Leech at start of each cycle*/
				msuLeechInit();						
				msuLogMsg(MSU_DEBUG_LEVEL_SUCCESS, "\nMSU Leech Initialized\n");
				eMsuState = eMsuStateConfigure;
				break;

			case  eMsuStateConfigure:
				if (msuDefaultSocketConfig() == MSU_SOCKET_SUCCESS )
					eMsuState = eMsuStateListen;
				break;

			case  eMsuStateListen:
				/*Listen to the sockets until sync message is received*/
				msuLogMsg(MSU_DEBUG_LEVEL_SUCCESS,"\nMsu listening on default socket\n");
				if(msuSocketCheckActivity() == MSU_TRUE)
				{
					eMsuState = eMsuStateStart;
				}
				break;

			case eMsuStateStart:
				msuLogMsg(MSU_DEBUG_LEVEL_SUCCESS,"\nConfigurating MSU Leech\n");
				/*Set up the MSU Leech according to the sync message paramaters*/
				if(msuLeechConfigure() == MSU_SOCKET_SUCCESS)
				{
					/*Update the configuration state to configured*/
					eMsuState = eMsuStateRunning;								
					msuLogMsg(MSU_DEBUG_LEVEL_SUCCESS,"\nMSU Ready to receive file from server\n");							
				}
				else
				{
					eMsuState = eMsuStateListen;
				}
          	break;

			case eMsuStateRunning:
				/*Listen for Multicast packets from the server*/
				status = msuSocketGetDataPackets();
				if(status == MSU_TRUE)
				{
					msuHandleMcastPacket();
				}
            break;

			case eMsuStateSCM:
				status = msuSocketGetDataPackets();
				if(status == MSU_TRUE)
				{
					msuHandleScmMcastPacket();
				}	
				break;

			case eMsuStateCCM:
				status = msuSocketGetDataPackets();
				if(status == MSU_TRUE)
				{
					msuHandleCcmMcastPacket();
				}
				break;

			case eMsuStateAbort:
				/*Discard the file here*/
				msuLogMsg(MSU_DEBUG_LEVEL_SUCCESS, "\nAborting file transfer\n");
				eMsuState = eMsuStateLoopBack;
				deviceUpgadeStatus = eMsuUpgradeFailed;
				MsuGlobalError = MsuGlobalError | MSU_UPDATE_STATUS_ABORT; 
				break;

			case eMsuStateDone:
				msuLogMsg(MSU_DEBUG_LEVEL_SUCCESS,"\nClosing file\n");				
				/*Close the previously opened file*/
				msuFileClose(pMsuFileDes);				
				if(getMissedChnkBitMap(g_msuBroadcastPdu.msuSyncHeaderformat.chunkCount) == 0)
				{
					msuLogMsg(MSU_DEBUG_LEVEL_SUCCESS,"\nFile received Successful\n");							
					deviceUpgadeStatus = eMsuUpgradePassed;			
					/*Perform checksum validation of the file*/
					if(msuPlatformComputeCheckSum(filePath, g_msuBroadcastPdu.msuSyncHeaderformat.fileChecksum) == MSU_TRUE)
					{
						msuLogMsg( MSU_DEBUG_LEVEL_SUCCESS, "\nChecksum Verification Successful\n");
						deviceUpgadeStatus = eMsuUpgradePassed;  	
					}
					else
					{
						msuLogMsg(MSU_DEBUG_LEVEL_ERROR, "\n**!!**!!Checksum Verification Failed **!!**!!\n");
						MsuGlobalError = MSU_UPDATE_FILE_CRC_FAILED;
						deviceUpgadeStatus = eMsuUpgradeFailed;
						/*Discard the file here*/
					}			
				}			
				else
				{
					msuLogMsg( MSU_DEBUG_LEVEL_ERROR,"\n Failed To Receive Complete File \n");
					deviceUpgadeStatus = eMsuUpgradeFailed;
					/*Discard the file here*/
				}
				/*Loop back and keep listening*/
				msuSendUpgradeStatusMsg(MSU_FALSE);
				eMsuState = eMsuStateLoopBack;
				break;

				case eMsuStateLoopBack:
				/*Release All resources*/
				msuReleaseResources();
				/*Loop back and keep listening*/
				eMsuState = eMsuStateConfigure;
  	
				break;

			default :
				eMsuStatus = EMSUErrUndefinedState;
				break;
		}//switch(eMsuState)

	}//while(eMsuStatus == EMSUErrSuccess)

	if(eMsuStatus != EMSUErrSuccess)
	{
		/*Release all resources*/
		msuReleaseResources();
	}//if(eMsuStatus != EMSUErrSuccess)
}

/**  @fn BOOL msuPduHandleTransferCompleteRequest( )
 *   @brief msuPduHandleTransferCompleteRequest() is used to handle transfer complete request
 *   @details  When client receives transfer complete request, it calls msuPduHandleTransferCompleteRequest() function,
 *             it checks whether all chunks received successfully or not , if some chunks are missing then it sends 
 *             CCM message with missed chunk numbers to server and chnges state to CCM state.
 *             if all the chunks received then it enters to done state. 
 *   @return    always MSU_FALSE because int the state machine MSU_TRUE only if data packet
 *   @retval   returns 1 or 0
  */

MSU_BOOL msuPduHandleTransferCompleteRequest( )//MSU_UINT8 *pMsg , MSU_UINT32 msgSize
{
   if( ccmSent == MSU_TRUE)
      return MSU_FALSE;
   
	if(getMissedChnkBitMap(g_msuBroadcastPdu.msuSyncHeaderformat.chunkCount) != 0)
	{
		if(msuSendCcmMsg(g_msuBroadcastPdu.msuSyncHeaderformat.chunkCount) == MSU_TRUE)
		{
			/* reset the parameter for chunk retransmission */
			g_localChunkNumber = 0;
			g_chunkDataSize = 0;
			g_localSeqNumber = 0;
			ccmRetryCount++; 
			msuLogMsg(MSU_DEBUG_LEVEL_SUCCESS, "\nREADY =====>> CCM\n");
			eMsuState = eMsuStateCCM;
			eMsuPrevState = eMsuStateCCM;
			ccmSent = MSU_TRUE;
		}
	}
	else
	{
		eMsuState = eMsuStateDone;
		msuLogMsg(MSU_DEBUG_LEVEL_SUCCESS, "\nREADY =====>> File Receiption DONE\n");				
   }
	return MSU_FALSE;
}
/**  @fn BOOL msuPduHandleCCMCompleteRequest(MSU_UINT8 *pMsg , MSU_UINT32 msgSize)
 *   @brief msuPduHandleCCMCompleteRequest( ) is used to handle CCM transfer complete request
 *   @details  When client receives CCM transfer complete request, first it checks if CCM retry is set in the request packet, 
 *             if not then it enters into abort state. if CCM retry is set in the request packet then it checks for missed chunks in the 
 *             file, if any then it sends CCM request message to server and enters into CCM state.
 *             If all the chunks received then enters into done state.
 *   @param[in] : MSU_UINT8 *pMsg           CCM transfer complete request message
 *   @param[in] : MSU_UINT32 msgSize        message size
 *   @return    always MSU_FALSE because int the state machine MSU_TRUE only if data packet
 *   @retval   returns 1 or 0
  */

MSU_BOOL msuPduHandleCCMCompleteRequest(MSU_UINT8 *pMsg , MSU_UINT32 msgSize)
{
   MSUCCMCompletePdu  *ccmCompletedReq;
  
   ccmCompletedReq = (MSUCCMCompletePdu  *)pMsg;
  
   eMsuPrevState = eMsuStateCCM;
   if( ccmCompletedReq->ccmRetry == MSU_FALSE)
   {
     	eMsuState = eMsuStateAbort;
     	if(getMissedChnkBitMap(g_msuBroadcastPdu.msuSyncHeaderformat.chunkCount) != 0)
     		MsuGlobalError = MSU_UPDATE_EXCEED_CCM_RETRY;
   	return MSU_FALSE;
   }
   
	if(getMissedChnkBitMap(g_msuBroadcastPdu.msuSyncHeaderformat.chunkCount) != 0)
	{
		if(msuSendCcmMsg(g_msuBroadcastPdu.msuSyncHeaderformat.chunkCount) == MSU_TRUE)
		{
			/* reset the parameter for chunk retransmission */
			g_localChunkNumber = 0;
			g_chunkDataSize = 0;
			ccmRetryCount++;
			msuLogMsg(MSU_DEBUG_LEVEL_SUCCESS,"\nREADY =====>> CCM\n");
			eMsuState = eMsuStateCCM;
		}
	}
	else
	{
		eMsuState = eMsuStateDone;
		msuLogMsg(MSU_DEBUG_LEVEL_SUCCESS, "\nREADY =====>> File Receiption DONE\n");		
	}
	return MSU_FALSE;
}

/**  @fn BOOL msuPduHandleSCMCompleteRequest(MSU_UINT8 *pMsg , MSU_UINT32 msgSize)
 *   @brief msuPduHandleSCMCompleteRequest( ) is used to handle SCM transfer complete request
 *   @details  When client receives SCM transfer complete request, first it checks if SCM retry is 
 *					set in the request packet, if not then it will not do any thing device will go as
 *					per state machine .
 *					if SCM retry is set in the request packet then it checks for missed sequence of current chunk 
 *             if any then it sends SCM request message to server and enters into SCM state.
            
 *   @param[in] : MSU_UINT8 *pMsg           SCM transfer complete request message
 *   @param[in] : MSU_UINT32 msgSize        message size
 *   @return    always MSU_FALSE because int the state machine MSU_TRUE only if data packet
 *   @retval   returns 1 or 0
  */
MSU_BOOL msuPduHandleSCMCompleteRequest(MSU_UINT8 *pMsg , MSU_UINT32 msgSize)
{
   MSU_UINT8 missedSeqCount = 0;
   MSU_UINT32 missedSepBitmap = 0;
   MSUSCMCompletePdu  *scmCompletedReq;
  
   scmCompletedReq = (MSUSCMCompletePdu  *)pMsg;
  
   if( scmCompletedReq->scmRetry == MSU_FALSE)
   {
		eMsuState = eMsuPrevState ;
     	msuLogMsg( MSU_DEBUG_LEVEL_SUCCESS,"\n SCM retry 0  In SCM Complete request \n");
		return MSU_FALSE;
   }
   missedSepBitmap = getMissedSeqBitMap(g_msuBroadcastPdu.msuSyncHeaderformat.seqNumberLimit);
	if( missedSepBitmap != 0 )
	{
	   missedSeqCount = getMissedSeqCount(missedSepBitmap );	
		if(msuSendScmMsg(missedSepBitmap,g_msuMcastPdu.chunkNumber,missedSeqCount) == MSU_TRUE)
		{
			scmRetryCount++;
			msuLogMsg( MSU_DEBUG_LEVEL_SUCCESS,"\nREADY =====>> SCM from SCM Complete Request \n");
			msuLogMsg( MSU_DEBUG_LEVEL_SUCCESS,"\n sequence count %d \n",scmRetryCount);
			eMsuState = eMsuStateSCM;		
		}
	}
	else
	{
		eMsuState = eMsuPrevState ;      
		msuLogMsg(MSU_DEBUG_LEVEL_SUCCESS, "\n No Seq missed In SCM Complete request \n");	
	}
	return MSU_FALSE;
}

/**  @fn MSU_UINT32 getCcmRetryCount()
 *   @brief getCcmRetryCount() is used to get the CCM Retry count
 *   @details  This function used to get the CCM retry count value, i,e how many times client sends the CCM Request 
 *             to receive missed chunks from server
 *   @param[in] : None
 *   @return MSU_UINT32  CCM retry count value
 *   @retval MSU_UINT32 value
  */

MSU_UINT32 getCcmRetryCount()
{
	return(ccmRetryCount);
}

/**  @fn EMSUState getCurrentMsuState( )
 *   @brief getCurrentMsuState() is used to get the current state machine state
 *   @param[in] : None
 *   @return EMSUState   msu state machine current state
 *   @retval EMSUState type
  */

EMSUState getCurrentMsuState( )
{
	return (eMsuState);
}

/**  @fn BOOL msuPlatformComputeCheckSum(MSU_SINT8 * filePath, MSU_UINT32 fileCheckSum)
 *   @brief msuPlatformComputeCheckSum( ) function is used to calculate the checksum
 *   @details  This function is used to calculate checksum of the specified file and compare the checksum
 *             with received check sum to confirm whether we received complete file successfully or not
 *   @param[in] : filePath - complete location of the file
 *   @param[in] : fileCheckSum - checksum value received from server
 *   @return returns true if file received successfully
 *   @retval MSU_TRUE if  successful
 *   @retval MSU_FALSE if failed 
  */

MSU_BOOL msuPlatformComputeCheckSum(MSU_SINT8 * filePath, MSU_UINT32 fileCheckSum)
{
	MSU_BOOL result = MSU_FALSE;
	MSU_UINT8 fileBuff[1460];   //MSU_FILE_CRC_BUFFER_SIZE
	MSU_SINT16 bytesRead = 0;
	MSU_UINT32 tempCheckSum = 0;

	MSU_SINT32 fileDes = 0;

	if( (filePath != MSU_NULL) && (fileCheckSum != 0))
	{
      //Open the file in read only mode
		fileDes = msuFileOpen((char *)filePath, eMsuFileR);
		if( fileDes != -1)
		{
			msuLogMsg( MSU_DEBUG_LEVEL_SUCCESS, "\nFile opened in read mode\n");
			do
		   {
			   //Read from the file
			   bytesRead = (MSU_UINT16)msuFileRead(fileDes,(MSU_SINT8 *)fileBuff, 1460);	
			   if(bytesRead > 0)
			   {
			      tempCheckSum += crcFast(fileBuff, (MSU_SINT16)bytesRead);
			   }

			}while(bytesRead > 0);
			msuLogMsg( MSU_DEBUG_LEVEL_SUCCESS, "\nCalculated checksum = %lx\n", tempCheckSum);
			//Close the file
			msuFileClose(fileDes);

			//Compare the checksums calculated
			if(tempCheckSum == fileCheckSum)
			{
        		result = MSU_TRUE;
			}
		}
		else
		{
			MsuGlobalError = MSU_FILE_OPEN_FAILED;
		}
	}

	return result;
}


#endif /*#ifndef MSU_LEECH_CPP */
