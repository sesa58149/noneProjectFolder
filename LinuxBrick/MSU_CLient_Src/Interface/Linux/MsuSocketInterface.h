/**	@file MsuSocketInterface.hpp
 *	<TABLE>
 *	@brief This file provide the Abstract socket functions which will be used in Application
 *         to create socket, transmit and receive data
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
 *	@author Rajinikanth Reddy
 *	@par HISTORY
 *
 *	<TABLE>
 *	<TR>
 *	<TD>Author</TD>		<TD>Date</TD>		<TD>Description</TD>
 *	</TR>
 *	<TR>
 *	<TD>Rajinikanth Reddy </TD>	<TD>07-Jan-2012</TD>	<TD>First creation</TD>
 *	</TR>
 *	</TABLE>
 *	</TABLE>
 *
 */

#ifndef __MSU_SOCKET_INFO_H__
#define __MSU_SOCKET_INFO_H__

#include "MsuDataTypes.h"
#include "MsuSocket.h"

/**  @fn msuSocketCreate( UINT16 port, UINT32 ipAddress  )
*    @brief Create a socket with specified port number and register with specified multicast IP Address
*/ 
MSU_SOCKET msuSocketCreate( MSU_UINT16 port, MSU_UINT32 ipAddress  );


/**  @fn msuSocketRecvFrom( SINT32 socketfd, void *buf,  UINT16 len,msuSockinfo *from)
*    @brief Used to receive data from specified socket
*/ 
MSU_SINT32  msuSocketRecvFrom( MSU_SINT32 socketfd, void *buf,  MSU_UINT16 len,msuSockinfo *from);


/**  @fn msuNonBlockingSocketRecvFrom( SINT32 socketfd, void *buf,  UINT16 len, msuSockinfo *from)
*    @brief Used to receive data from specified Non Blocking sockets 
*/ 
MSU_SINT32  msuNonBlockingSocketRecvFrom( MSU_SINT32 socketfd, void *buf,  MSU_UINT16 len, msuSockinfo *from);


/**  @fn msuSocketSendTo( SINT32 socket, const void *msg, SINT32 len, const  msuSockinfo* to )
*    @brief Used to send data to specified destination IP Address and Port number
*/ 
MSU_SINT32 msuSocketSendTo( MSU_SINT32 socket, const void *msg, MSU_SINT32 len, const  msuSockinfo* to );


/**  @fn msuSocketClose(SINT32 Socket)
*    @brief Used to close specified socket
*/ 
MSU_SINT16 msuSocketClose(MSU_SINT32 Socket);


#endif/*#ifndef __MSU_SOCKET_H__*/
