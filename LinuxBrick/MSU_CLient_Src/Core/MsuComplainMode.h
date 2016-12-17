/**	@file complainMode.hpp
 *	<TABLE>
 *	@brief Implementation of MSU complainMode header
 *
 *	@copy
 *	<TABLE>
 *	<TR><TD> Copyright </TD></TR>
 *	<TR>
 *	<TD>Schneider Electric India (Pvt) Ltd.
 *	Copyright (c) 2011 - All rights reserved.
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
 *	<TD>Author</TD>			<TD>Date</TD>			<TD>Description</TD>
 *	</TR>
 *	<TR>
 *	<TD>Deepak G</TD>	<TD>03-Oct-2011</TD>	<TD>First creation</TD>
 *	</TR>
 *
 *	</TABLE>
 *	</TABLE>
 */

#ifndef _MSU_COMPLAIN_HPP__
#define _MSU_COMPLAIN_HPP__


#include "MsuDataTypes.h"



/**  @def MSU_MAX_NUM_OF_SEQ 
*    @brief Macro to be defined for Max number of sequences in each chunk
*/
#define MSU_MAX_NUM_OF_SEQ 32

/**  @def MAX_NUM_CHNKS 
*    @brief Macro to be defined for Max number of chunks value 128
*/
#define MAX_NUM_CHNKS 128

/**  @def MIN_NUM_CHNKS 
*    @brief Macro to be defined for Min Chunk number value 1
*/
#define MIN_NUM_CHNKS 1

/**  @def MAX_NUM_CNUKS_SUPPPORTED 
*    @brief Macro to be defined for Max number of chunks supported value 3968
*/
#define MAX_NUM_CHUNKS_SUPPPORTED 3968


/**  @fn void setRecvdSeqBitMap(UINT8 index)
*    @brief This function is used to set the received sequence bit map 
*/ 
void setRecvdSeqBit(MSU_UINT8 index);


/**  @fn UINT32 getMissedSeqBitMap(UINT8 numOfSeqns);
*    @brief This function is used to get the missed sequence bit maps in the current chunk
*/ 
MSU_UINT32 getMissedSeqBitMap(MSU_UINT8 numOfSeqns);


/**  @fn void clearSeqBitMap(void)
*    @brief Create a socket with specified port number and register with specified multicast IP Address
*/ 
void clearSeqBitMap(void);

/**  @fn BOOL isSeqBitMapSet(UINT8 seqNum)
*    @brief This function is used to check for particulat sequence number is already received or not
*/ 
MSU_BOOL isSeqRecvd(MSU_UINT8 seqNum);

/**  @fn void complainModeInit(void)
*    @brief This function is used to initialize complain mode global variables
*/ 
void complainModeInit(void);

/**  @fn BOOL setRecvdChnkBit(UINT32 chnkNum)
*    @brief This function is used to set the chunk bit for the particular chunk number
*/ 
MSU_BOOL setRecvdChnkBit(MSU_UINT32 chnkNum);

/**  @fn mBOOL clrRecvdChnkBit(UINT32 chnkNum)
*    @brief This function is used to clear the particular chunk number bit
*/ 
MSU_BOOL clrRecvdChnkBit(MSU_UINT32 chnkNum);

/**  @fn void clrRecvdChnkBitMap(void)
*    @brief This function is used to clear entire chunk bit map arrey
*/ 
void clrRecvdChnkBitMap(void);

/**  @fn BOOL isChnRecvd(UINT32 chnkNum)
*    @brief This function is used to check particulat chunk number is received or not
*/ 
MSU_BOOL isChnkRecvd(MSU_UINT32 chnkNum);

/**  @fn UINT32 getMissedChnkBitMap(UINT32 totalChnksExpected)
*    @brief This function is used to return total number of missed chunks in the bit map arrey
*/ 
MSU_UINT32 getMissedChnkBitMap(MSU_UINT32 totalChnksExpected);

/**  @fn UINT32* getMissedChnkNumsArry(UINT32 totalChnksExpected,UINT32 *totalNumOfChnksMissed  )
*    @brief This function returns missed chunk numbers arrey pointer, which points to missed chunk numbers
*/ 
MSU_UINT32* getMissedChnkNumsArry(MSU_UINT32 totalChnksExpected,MSU_UINT32 *totalNumOfChnksMissed  );

/**  @fn void clrMissedChnkBitMap(void)
*    @brief This function is used to clear missed chunk bitmap arrey
*/ 
void clrMissedChnkBitMap(void);

/**  @fn UINT32 getSeqBitMap( )
*    @brief This function is used to get the current chunk sequence bit map value
*/ 
MSU_UINT32 getSeqBitMap( );

/**  @fn void setSeqBitMap(UINT32 seqbitmapVal )
*    @brief This function is used to set the current chunk sequence bit map value
*/ 
void setSeqBitMap(MSU_UINT32 seqbitmapVal );


MSU_UINT8 getMissedSeqCount(MSU_UINT32 seqBitmapVal );



#endif //_MSU_COMPLAIN_H__



