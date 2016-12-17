/**	@file MsuFilePlatform.hpp
 *	<TABLE>
 *	@brief Implementation of MSU File Platform header
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

#ifndef MSUFILEPLATFORMHPP
#define MSUFILEPLATFORMHPP

#include "MsuPlatformInterface.h"
#include "MsuFileOperation.h"    

/**  @fn setTimeoutVal( Sint32 socketfd, MSU_UINT8 timeout_sec )
*    @brief Stores the timeout value, which will be used by select system call
*/ 
MSU_SINT32 msuFileOpen(MSU_SINT8 * filePath, EMsuFileOpenMode  mode);


/**  @fn setTimeoutVal( Sint32 socketfd, MSU_UINT8 timeout_sec )
*    @brief Stores the timeout value, which will be used by select system call
*/ 
MSU_SINT16 msuFileWrite(MSU_SINT32 fds, MSU_SINT8 * pData, MSU_UINT16 size);


/**  @fn setTimeoutVal( Sint32 socketfd, MSU_UINT8 timeout_sec )
*    @brief Stores the timeout value, which will be used by select system call
*/ 
MSU_SINT16 msuFileRead(MSU_SINT32 fds, MSU_SINT8 * pBuf, MSU_UINT16 size);


/**  @fn setTimeoutVal( Sint32 socketfd, MSU_UINT8 timeout_sec )
*    @brief Stores the timeout value, which will be used by select system call
*/ 
MSU_BOOL msuFileSeek( MSU_SINT32 *fds, MSU_UINT32 position);


/**  @fn setTimeoutVal( Sint32 socketfd, MSU_UINT8 timeout_sec )
*    @brief Stores the timeout value, which will be used by select system call
*/ 
MSU_SINT16 msuFileClose(MSU_SINT32 fds);

/**  @fn setTimeoutVal( Sint32 socketfd, MSU_UINT8 timeout_sec )
*    @brief Stores the timeout value, which will be used by select system call
*/ 
MSU_UINT32 msuFileTell(MSU_SINT32 fds);


#endif/*#ifndef MSUFILEPLATFORMHPP */



