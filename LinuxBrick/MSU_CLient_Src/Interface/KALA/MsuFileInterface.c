/**	@file MsuFilePlatform.cpp
 *	<TABLE>
 *	@brief Implementation of MSU File Platform
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

#ifndef MSU_FILE_INTERFACE_CPP
#define MSU_FILE_INTERFACE_CPP


#include "FileSystemImpl.hpp" 
#include "MsuFileInterface.h"
#include "MsuFileOperation.h"
#include "MsuCRC32.h"
#include "LogMsg.h"
#include <stdio.h>

/**  @fn msuFileOpen(char * filePath, EMsuFileOpenMode eMode)
 *   @brief This function is used to open a file in different modes at specified path
 *   @details  msuFileOpen() function is used to open a file in read mode or write mode or readwrite mode
 *             or append mode. if the file is not availale in write or readwrite or append then it will 
 *             create a new file at specified location
 *   @param[in] : filePath - specified location of the file
 *   @param[in] : eMode - is a enum of EMsuFileOpenMode type, different enum values are  eMsuFileR,
 *                eMsuFileW,eMsuFileRW and  eMsuFileApnd.
 *   @return returns file descriptor if success or -1 if error.
 *           Error code if the operation is failed.
 *   @retval File descriptor of type Sint32, if success
 *   @retval -1 if failed to open file
  */

MSU_SINT32 msuFileOpen(MSU_SINT8 * filePath, EMsuFileOpenMode emsufileopenmode)
{

   MSU_BOOL status = MSU_FALSE;
   MSU_UINT8 operationMode = 0;
   MSU_SINT32 *fds = NULL;

   if(filePath != NULL)
   {
      if(emsufileopenmode == eMsuFileR)
	  {
	     operationMode = FILE_OPERATING_TYPE_R;
	  }
	  else if(emsufileopenmode == eMsuFileW)
	  {
	     operationMode = FILE_OPERATING_TYPE_W;
	  }
	  else if(emsufileopenmode == eMsuFileRW)
	  {
	     operationMode = FILE_OPERATING_TYPE_RW;  
	  }	
	  else if(emsufileopenmode == eMsuFileApnd)
	  {
	      operationMode = FILE_OPERATING_TYPE_A;
	  }
		  
	  fds = (MSU_SINT32 *)trfOpen(filePath, operationMode);
	  if(fds == NULL)
	  {
	     msuLogMsg(MSU_DEBUG_LEVEL_SUCCESS, "\nOpening file at the path %s with mode %s failed\n",filePath, operationMode);
		 return (-1);
	  }
	  else
	  {
    	   return ( (MSU_SINT32)fds);
	   }
	}
	else
	{
		return(-1);
	}

}



/**  @fn msuFileWrite(MSU_SINT32 fds, MSU_SINT8 * pData, MSU_UINT16 size)
 *   @brief This function is used to write data into a file
 *   @details  msuFileWrite() function is used to write a data into a file by using the specified file descriptor.
 *   @param[in] : fds - File descriptor
 *   @param[in] : pData - Data buffer which we need to write into the data
 *   @param[in] : size - size of the data to write
 *   @return returns number of bytes written successfully 
 *           Error code if the operation is failed.
 *   @retval bytes value written into the file successfully
 *   @retval TR_ERROR if failed to write into file
  */

MSU_SINT16 msuFileWrite(MSU_SINT32 fds, MSU_SINT8 * pData, MSU_UINT16 size)
{

   MSU_SINT32 retVal = TR_SUCCESS;
   MSU_SINT32 *ffds  = NULL;

  	ffds = ((MSU_SINT32 *)fds);
  	if((pData != NULL) && (ffds != NULL) && (size != 0))
  	{
		//Write the data into the already opened file	  	
	  	retVal = (MSU_SINT16)trfWrite((MSU_SINT32*)ffds, (MSU_UINT8 *)pData, (MSU_UINT32)size);
	    return (retVal);
	}
  	else
  	{
  		return(MSU_ERROR);
  	}

}

/**  @fn msuFileRead(MSU_SINT32 fds, MSU_SINT8 * pBuf, MSU_UINT16 size)
 *   @brief This function is used to read data into a file
 *   @details  msuFileRead() function is used to read a data into a file by using the specified file descriptor.
 *   @param[in] : fds - File descriptor
 *   @param[in] : pBuf - empty data buffer used to receive data
 *   @param[in] : size - size of the data to read
 *   @return returns number of bytes read successfully 
 *           Error code if the operation is failed.
 *   @retval bytes value read from the file successfully
 *   @retval TR_ERROR if failed to write into file
  */

MSU_SINT16 msuFileRead(MSU_SINT32 fds, MSU_SINT8 * pBuf, MSU_UINT16 size)
{

   MSU_SINT32 retVal = 0;
   MSU_SINT32 *ffds = NULL;

   ffds = ((MSU_SINT32 *)fds);
	if((pBuf != NULL) && (ffds != NULL) && (size != 0))
  	{
        
            retVal = (MSU_SINT16)trfRead((MSU_SINT32 *)ffds, (MSU_UINT8 *)pBuf, (MSU_UINT32)size);
	    return(retVal);
		 msuLogMsg( MSU_DEBUG_LEVEL_SUCCESS, "\nRead operation: read %d bytes of data\n", retVal);

  	}
  	else
  	{
  		return(MSU_ERROR);
  	}

}



/**  @fn msuFileSeek(MSU_SINT32 *fds, MSU_UINT32 position)
 *   @brief This function is used to move the file pointer to the specified position
 *   @details  msuFileRead() function is used to move the file pointer to the specified location from the 
 *             starting position of the file.  
 *   @param[in] : fds - File descriptor, updates the descriptor if moved successfully
 *   @param[in] : position - number of bytes want to move from the file starting position
 *   @return returns true if file seek successful 
 *           Error code if the operation is failed.
 *   @retval MSU_TRUE if file seek successful
 *   @retval MSU_FALSE if failed to seek
  */

MSU_BOOL msuFileSeek(MSU_SINT32 *fds, MSU_UINT32 position)
{

   MSU_BOOL result = MSU_FALSE;
   MSU_SINT32 *ffds = NULL;

   ffds = ((MSU_SINT32 *)(*fds));

    msuLogMsg( MSU_DEBUG_LEVEL_SUCCESS,"\nSeeking to position %ld\n", position);
     

   if(ffds != NULL)
   {
       if(trfSeek((MSU_SINT32 *)ffds, (long)position, (long)MSU_FILE_SEEK_START) == -1)
       if(1)
       {
		   msuLogMsg( MSU_DEBUG_LEVEL_SUCCESS, "\nSeek operation failed\n" );
 
       	   return(MSU_FALSE);
     	}
     	else
     	{
		   	 msuLogMsg( MSU_DEBUG_LEVEL_SUCCESS, "\nSeek operation Successful\n");
			
	        *fds = (MSU_SINT32)ffds;
	        result = MSU_TRUE;
     	}
	}
	return result;

}





/**  @fn msuFileClose(MSU_SINT32 fds )
 *   @brief This function is used to close the file 
 *   @details  This function is used to close specified file 
 *   @param[in] : fds - file descriptor
 *   @return returns 0 if file closed successfully or -1
 *   @retval 0 if  successful
 *   @retval -1 if failed 
  */

MSU_SINT16 msuFileClose(MSU_SINT32 fds )
{

   MSU_SINT16 retVal = 0;
   MSU_SINT32 *ffds = NULL;

   ffds = ((MSU_SINT32 *)fds);
   if(ffds != NULL)
   {
      retVal = (MSU_SINT16)trfClose((Sint32*)ffds);	    
      msuLogMsg( MSU_DEBUG_LEVEL_SUCCESS, "\nFile close Error Code= %d\n", retVal);	 
      return(retVal);
   }
   else
   {
   	return(-1);
   }

}

MSU_UINT32 msuFileTell(MSU_SINT32 fds)
{
    return 0;//f_tell((FS_FILE*)fds);
}

#endif/*#ifndef MSU_FILE_PLATFORM_CPP */
