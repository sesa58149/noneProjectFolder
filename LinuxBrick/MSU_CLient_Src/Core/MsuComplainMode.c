/**	@file complainMode.cpp
 *	<TABLE>
 *	@brief Implementation of MSU Complain Mode
 *
 *	@copy
 *	<TABLE>
 *	<TR><TD> Copyright </TD></TR>
 *	<TR>
 *	<TD>Schneider Electric India (Pvt) Ltd.
 *	Copyright (c) 2009 - All rights reserved.
 * India Patent Application Number:1448/CHE/2010
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
 
#ifndef _MSU_COMPLAIN_CPP__
#define _MSU_COMPLAIN_CPP__

#include "MsuPlatformInterface.h"
#include "MsuComplainMode.h"

MSU_UINT32 g_missedSeqBitMap = 0;
static MSU_UINT32 recvdSeqBitMap = 0;
static MSU_UINT32 recvdChnkArr[MAX_NUM_CHNKS];
static MSU_UINT32 missedChnkArr[MAX_NUM_CHNKS];
static MSU_UINT32 missedChnkNums[MAX_NUM_CHNKS];

#define SEQUENCE_MASK 0xFFFFFFFF
#define CHUNK_NUM_DIV 32

/**  @fn void complainModeInit(void)
 *   @brief This function is used to initialize complain mode global variables
 *   @details  This function is used to initialize reeived chunk arrey, missed chunk arrey and
 *             missed chunk numbers arrey's with 0 
 *   @param[in] : None
 *   @return    : None
 */
void complainModeInit(void)
{
   g_missedSeqBitMap = 0;
   recvdSeqBitMap = 0;
   msuMemorySet((MSU_UINT8*)recvdChnkArr, 0, sizeof(MSU_UINT32) * MAX_NUM_CHNKS);
   msuMemorySet((MSU_UINT8*)missedChnkArr, 0, sizeof(MSU_UINT32) * MAX_NUM_CHNKS);
   msuMemorySet((MSU_UINT8*)missedChnkNums, 0, sizeof(MSU_UINT32) * MAX_NUM_CHNKS);  
}


/**  @fn MSU_BOOL setRecvdChnkBit(MSU_UINT32 chnkNum)
 *   @brief This function is used to set the chunk bit for the particular chunk number
 *   @details  MSU server divides the file into number of chunks, each chunk contains n number sequences,
 *             when all the sequences received in the partucular chunk , we set the particular chunk number
 *             in chunk bit arrey
 *   @param[in] : Chunk Number
 *   @return MSU_TRUE if the operation is successful
 *           MSU_FALSE if the operation is failed.
 *   @retval 1 if chunk bit set successfully
 *   @retval 0 if failed to set chunk bit 
 */

MSU_BOOL setRecvdChnkBit(MSU_UINT32 chnkNum)
{
   MSU_UINT16 index = 0;
   MSU_BOOL retVal = 1;

	if( (MIN_NUM_CHNKS > chnkNum) || (chnkNum > MAX_NUM_CHUNKS_SUPPPORTED) )
 	{
		msuLogMsg(MSU_DEBUG_LEVEL_SUCCESS, "\nOut of proper chunk range\n");
		return 0; 
 	}

 	index = (MSU_UINT16)((chnkNum -1)/32);

 	recvdChnkArr[index] |=  (0x01 << (((chnkNum-1)%32) ));
	msuLogMsg( MSU_DEBUG_LEVEL_SUCCESS, "\nSet Recvd Chunk Bit for CN %ld\n", chnkNum);
	return retVal;
}



/**  @fn MSU_UINT32* getMissedChnkNumsArry(MSU_UINT32 totalChnksExpected,MSU_UINT32 *totalNumOfChnksMissed )
 *   @brief This function returns missed chunk numbers arrey pointer, which points to missed chunk numbers
 *   @details   Msu client once receive the chunks from server, it checks all the chunks recieved or not,
 *              if any chunks are missed then client uses this function to get the missed chunk numbers and
 *              sends message to server to get the missed chunk packets from server 
 *   @param[in] : MSU_UINT32 totalChnksExpected        total number of chunks sent from server      
 *   @param[in] : MSU_UINT32 *totalNumOfChnksMissed    total number of chunks missed in client updated by function
 *   @return    MSU_UINT32* pointer which points to missed chunk numbers arrey
 *   @retval    pointer to MSU_UINT32* arrey
 */
MSU_UINT32* getMissedChnkNumsArry(MSU_UINT32 totalChnksExpected,MSU_UINT32 *totalNumOfChnksMissed )
{
   MSU_UINT32 chnkNum;
	MSU_UINT32 numofMissedChnks = 0;
    MSU_UINT16 i = 0;

	numofMissedChnks = getMissedChnkBitMap(totalChnksExpected);
	*totalNumOfChnksMissed = numofMissedChnks;

	for( chnkNum = 1; chnkNum <= totalChnksExpected;  chnkNum++ )
	{
		if ( isChnkRecvd(chnkNum) == 0)
		{
			missedChnkNums[i] = msuHtoNl(chnkNum);
			msuLogMsg( MSU_DEBUG_LEVEL_SUCCESS, "\nCN missed = %ld\n", chnkNum);
			i++;
		}
	}
	return missedChnkNums;
}



/**  @fn MSU_UINT32* MSU_UINT32 getMissedChnkBitMap(MSU_UINT32 totalChnksExpected)
 *   @brief This function is used to return total number of missed chunks in the bit map arrey
 *   @details   This f'n checks all the chunks recieved or not, if any chunks are missed in the client, 
 *              this function returns the missed chunks count 
 *   @param[in] : MSU_UINT32 totalChnksExpected        total number of chunks sent from server      
 *   @return    MSU_UINT32   missed chunk numbers count
 */
MSU_UINT32 getMissedChnkBitMap(MSU_UINT32 totalChnksExpected)
{
	MSU_UINT16 chnkNum ;
	MSU_UINT16 index = 0;
	MSU_UINT32 numofMissedChnks =0;

	for( chnkNum = 1; chnkNum <= totalChnksExpected;  chnkNum++ )
	{
		if ( isChnkRecvd(chnkNum) == 0)
		{
			index = (MSU_UINT16)((chnkNum-1)/32 );
			missedChnkArr[index] |=  ( 1 << ((chnkNum-1)%32));
			numofMissedChnks++;
		}
	}
	msuLogMsg( MSU_DEBUG_LEVEL_SUCCESS, "\nMissed chunk count = %ld\n", numofMissedChnks);
	return numofMissedChnks;
}

/**  @fn void clrRecvdChnkBitMap(void)
 *   @brief This function is used to clear entire chunk bit map arrey
 *   @param[in] : None
 *   @return    : None
 */
void clrRecvdChnkBitMap(void)
{
   msuMemorySet ((MSU_UINT8*)recvdChnkArr,0,sizeof(recvdChnkArr));
}


/**  @fn void clrMissedChnkBitMap(void)
 *   @brief This function is used to clear missed chunk bit map arrey
 *   @param[in] : None
 *   @return    : None
 */
void clrMissedChnkBitMap(void)
{
   msuMemorySet ((MSU_UINT8*)missedChnkArr,0,sizeof(missedChnkArr));
}


/**  @fn MSU_BOOL isChnRecvd(MSU_UINT32 chnkNum)
 *   @brief This function is used to check particulat chunk number is received or not
 *   @param[in] : MSU_UINT32 Chunk Number
 *   @return    : MSU_TRUE if particular chunk received bit set in the received chunk arrey
 *                MSU_FALSE if particular chunk received bit is not set in the received chunk arrey 
 *   @retval    : 1 if success, 0 if failure

 */
MSU_BOOL isChnkRecvd(MSU_UINT32 chnkNum)
{
   MSU_UINT16 index = 0;
   MSU_UINT32 mask = 0;
   MSU_BOOL retVal = 0;

 	if( (MIN_NUM_CHNKS > chnkNum) || (chnkNum > MAX_NUM_CHUNKS_SUPPPORTED) )
	   return retVal; 

 	index = (MSU_UINT16)( (chnkNum -1)/CHUNK_NUM_DIV );
 	mask = 1ul << ((chnkNum-1)%CHUNK_NUM_DIV);
 	if(recvdChnkArr[index] & mask)
 	{
		 retVal = 1;
 	}
 	return retVal;
}


/**  @fn MSU_BOOL clrRecvdChnkBit(MSU_UINT32 chnkNum)
 *   @brief This function is used to clear particulat chunk number in the received chunk bit arrey
 *   @param[in] : MSU_UINT32 Chunk Number
 *   @return    : MSU_TRUE if particular chunk received bit is cleared in the received chunk arrey
 *                MSU_FALSE if particular chunk received bit is not cleared in the received chunk arrey 
 *   @retval    : 1 if success, 0 if failure

 */
MSU_BOOL clrRecvdChnkBit(MSU_UINT32 chnkNum)
{
   MSU_UINT16 index = 0;
   MSU_BOOL retVal = 1;

 	if( (MIN_NUM_CHNKS > chnkNum) || (chnkNum > MAX_NUM_CHUNKS_SUPPPORTED) )
		return 0; 

 	index = (MSU_UINT16) ((chnkNum -1)/CHUNK_NUM_DIV);
 	recvdChnkArr[index] &=  ~(0x01 << ((chnkNum - 1)%CHUNK_NUM_DIV));
 	return retVal;
}





/**  @fn void setRecvdSeqBitMap(MSU_UINT8 seqNum)
 *   @brief This function is used to set particul sequence number bit in the received sequence bit map arrey
 *   @param[in] : MSU_UINT8 Sequence Number
 *   @return    : None
 *   @retval    : None
 */
void setRecvdSeqBit(MSU_UINT8 seqNum)
{
   MSU_UINT8 offSet = 32;

	recvdSeqBitMap |= (MSU_UINT32) (1L << (offSet -seqNum ));
	msuLogMsg(MSU_DEBUG_LEVEL_SUCCESS, "\n!!!! Set Bit Called for SN %d and Rx Bit Map = %lx\n",seqNum,  recvdSeqBitMap);	
}


/**  @fn MSU_UINT32 getSeqBitMap( )
 *   @brief This function is used to get the current chunk sequence bit map value
 *   @param[in] : None
 *   @return    : MSU_UINT32 received sequence bit map value
 *   @retval    : MSU_UINT32
 */
MSU_UINT32 getSeqBitMap( )
{
   return recvdSeqBitMap;
}


/**  @fn void setSeqBitMap(MSU_UINT32 seqbitmapVal )
 *   @brief This function is used to set the current chunk sequence bit map value
 *   @param[in] : MSU_UINT32 seqbitmapVal     sequence bit map value  
 *   @return    : None
 *   @retval    : None
 */
void setSeqBitMap(MSU_UINT32 seqbitmapVal )
{
   recvdSeqBitMap = seqbitmapVal;

}



/**  @fn MSU_UINT32 getMissedSeqBitMap(MSU_UINT8 numOfSeqns)
 *   @brief This function is used to get the current chunk missed sequence bit map value
 *   @param[in] : MSU_UINT8 numOfSeqns      total number of sequences in each chunk value       
 *   @return    : missed sequence bit map value
 *   @retval    : MSU_UINT32
 */

MSU_UINT32 getMissedSeqBitMap(MSU_UINT8 numOfSeqns)
{
    static MSU_UINT32 tempMissed = 0;
	MSU_UINT8 index=0 ;
	MSU_UINT32 mask = 0;

	for(index = 1; index <= numOfSeqns; index++ )
	{
		mask |= 1<<(32-index);
	}
	if((recvdSeqBitMap & mask) != 0)
	{
		tempMissed = (recvdSeqBitMap ^ SEQUENCE_MASK);
		tempMissed &= mask;
	}
	msuLogMsg( MSU_DEBUG_LEVEL_SUCCESS, "\nMissed sequence bitmap = %lx\n",(MSU_UINT32) msuNtoHl(tempMissed));
	return tempMissed;
}


/**  @fn MSU_BOOL isSeqBitMapSet(MSU_UINT8 seqNum)
 *   @brief This function is used to verify is particular sequence number is received or not
 *   @param[in] :MSU_UINT8 seqNum    sequence number      
 *   @return    : MSU_TRUE, if bit map set for passed sequence number
                  MSU_FALSE, if bit map not set for passed sequence number  
 *   @retval    : MSU_UINT32
 */

MSU_BOOL isSeqRecvd(MSU_UINT8 seqNum)
{
   MSU_BOOL result = MSU_FALSE;
   msuLogMsg( MSU_DEBUG_LEVEL_SUCCESS,"\n****Is Set Called for SN %d and Rx Bit Map = %lx\n",seqNum,  recvdSeqBitMap);
   
   if(recvdSeqBitMap & (((MSU_UINT32)1L) << (32 - seqNum)) )
 	{
	   result = MSU_TRUE;
 	}
   return result;
}

/**  @fn void clearSeqBitMap(void)
 *   @brief This function is used to clear the current chunk sequence bit map value to 0
 *   @param[in] : None
 *   @return    : None
 *   @retval    : None
 */
void clearSeqBitMap(void)
{
   recvdSeqBitMap = 0;
}


MSU_UINT8 getMissedSeqCount( MSU_UINT32 seqBitmapVal )
{
   MSU_UINT8 ctr=0; 
   for( ; seqBitmapVal!=0; seqBitmapVal>>=1 )
   { 
       if( seqBitmapVal & 1 ) 
		{
	        ctr++;
		}
	}
	return (ctr);
}


#endif //_MSU_COMPLAIN_CPP__
