/**	@file MsuSocket.hpp
 *	<TABLE>
 *	@brief Implementation of Mass Software Upgrade Socket header
 *
 *	@copy
 *	<TABLE>
 *	<TR><TD> Copyright </TD></TR>
 *	<TR>
 *	<TD>Schneider Electric India (Pvt) Ltd.
 *	Copyright (c) 2011 - All rights reserved.
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

#ifndef __MSU_SOCKET_HPP__
#define __MSU_SOCKET_HPP__

#include "MsuLeech.h"

#define MSU_MCAST_SYNC_PORT 	567
#define MSU_MCAST_DEFAULT_IP 	"239.254.1.2"

/*Socket defines*/
#define MSU_SOCKET long
#define MSU_INVALID_SOCKET -1
#define MSU_SOCKET_ERROR -1
#define MSU_SOCKET_TIMEOUT_ERROR  -2
#define MSU_SOCKET_RECEIVE_ERROR  -3

/**  @def MSU_SOCKET_ERROR 
*    @brief Macro to be defined for Error value (-1)
*/
#define MSU_SOCKET_ERROR -1

/**  @def MSU_SOCKET_SUCCESS 
*    @brief Macro to be defined for Success value (0)
*/
#define MSU_SOCKET_SUCCESS 0

/**  @def MSU_MCAST_SYNC_PORT 
*    @brief Macro to be defined for default UDP socket port number
*/
#define MSU_MCAST_SYNC_PORT 567

/**  @def MSU_UNICAST_PORT 
*    @brief Macro to be defined for unicast socket Port number
*/
#define MSU_UNICAST_PORT 568

/**  @def MSU_SOCKET_TIMEOUT 
*    @brief Macro to be defined for max socket timeout value
*/
#define MSU_SOCKET_TIMEOUT 3600
     
    
/**  @fn void msuSocketInit(void)
*    @brief This function is used to intialize socket variables
*/ 
void msuSocketInit(void);

/**  @fn SINT32 msuDefaultSocketCreate()
*    @brief This function is used to create a default socket
*/ 
MSU_SINT32 msuDefaultSocketCreate();

/**  @fn SINT32 msuUpdateSocketCreate(UINT16 port, UINT32 mIpAddr)
*    @brief This function is used to create updated socket
*/ 
MSU_SINT32 msuUpdateSocketCreate(MSU_UINT16 port, MSU_UINT32 mIpAddr);

/**  @fn BOOL msuSocketGetPacket()
*    @brief msuSocketGetPacket( ) function uses default socket to receive messages in blocking mode
*/ 
MSU_BOOL msuSocketGetPacket();

/**  @fn BOOL msuSendScmMsg(UINT32 bitMap, UINT32 chunkNum, UINT8 seqCount  )
*    @brief This function is used to send SCM message to server
*/ 
MSU_BOOL msuSendScmMsg(MSU_UINT32 bitMap, MSU_UINT32 chunkNum, MSU_UINT8 seqCount  );

/**  @fn BOOL msuSendCcmMsg( UINT32 totalChnksExpected )
*    @brief This function is used to send CCM message to server
*/ 
MSU_BOOL msuSendCcmMsg( MSU_UINT32 totalChnksExpected );

/**  @fn BOOL msuSendUpgradeStatusMsg(BOOL upgradeStatus)
*    @brief This function is used to send Upgrade status message
*/ 
MSU_BOOL msuSendUpgradeStatusMsg(MSU_BOOL upgradeStatus);

/**  @fn BOOL msuSocketCheckActivity(void)
*    @brief This function is used to monitor activitied on updated socket
*/ 
MSU_BOOL msuSocketCheckActivity(void);

/**  @fn void msuSocketRelease(void)
*    @brief This function is used to close default and updated sockets
*/ 
void msuSocketRelease(void);

/**  @fn void msuRegisterCallBackMethod(UINT8 callBackType, void (*fptr)(EMSUState),void* arg)
*    @brief This function is used to register call back function, this call back function executes when 
*           socket receives Abort message
*/  
void msuRegisterCallBackMethod(MSU_UINT8 callBackType, void (*fptr)(EMSUState),void* arg);

/**  @fn void msuSendGroupCreationAckMsg( UINT8 *gMsg  )
*    @brief This function is used to send Group creation Ack message
*/ 
void msuSendGroupCreationAckMsg( MSU_UINT8 *gMsg  );

/**  @fn BOOL msuSendDisconnectResponse(MSUDisconnectPdu *localDisconnectpdu)
*    @brief This function is used to send Disconnect message response
*/

MSU_BOOL msuSendDisconnectResponse(MSUDisconnectPdu *localDisconnectpdu);

/**  @fn void msuPasrseDisconnectMessage( UINT8 *gMsg ,UINT16 msgSize )
*    @brief This function is used to parse the Disconnect message request
*/

void msuPasrseDisconnectMessage( MSU_UINT8 *gMsg ,MSU_UINT16 msgSize );

/**  @fn void msuSocketProcessDiscoveryMessage(UINT8 *pMsg, UINT32 msgSize)
*    @brief This function is used to handle discovery message and sends discovery response message
*/ 
void msuSocketProcessDiscoveryMessage(MSU_UINT8 *pMsg, MSU_UINT32 msgSize);

/**  @fn BOOL msuSendAuthenticationResponse(MSUAuthenticationRequestPdu *pAuthenticationRequestPdu,UINT32 msgSize)
*    @brief This function is used to handle Authetication Response
*/
MSU_BOOL msuSendAuthenticationResponse(MSUAuthenticationRequestPdu *pAuthenticationRequestPdu,MSU_UINT32 msgSize);


#endif/*#ifndef __MSU_SOCKET_HPP__*/




