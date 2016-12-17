/**	@file MsuCRC32.cpp
 *	<TABLE>
 *	@brief Implementation of MSU CRC32
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

#ifndef MSU_CRC32_CPP
#define MSU_CRC32_CPP

#include "MsuCRC32.h"

#define WIDTH  (8 * sizeof(CRC32))
#define TOPBIT (1L << (WIDTH - 1))

#define POLYNOMIAL 0x04C11DB7

static CRC32  crcTable[256];

/**  @fn void crcInit(void)
 *   @details crcInit() is used to initialize crc table values
 *   @param[in] : None           
 *   @return      None
 */
void crcInit(void)
{
	CRC32  remainder;
	MSU_UINT32 dividend = 0;
	MSU_UINT8 bit;

    remainder = 0xFFFFFFFF;

    /*
     * Compute the remainder of each possible dividend.
     */
    for (dividend = 0; dividend < 256; ++dividend)
    {
        /*
         * Start with the dividend followed by zeros.
         */
        remainder = (CRC32)(dividend << (WIDTH - 8));

        /*
         * Perform modulo-2 division, a bit at a time.
         */
        for ( bit = 8; bit > 0; --bit)
        {
            /*
             * Try to divide the current data bit.
             */			
            if (remainder & TOPBIT)
            {
                remainder = (remainder << 1) ^ POLYNOMIAL;
            }
            else
            {
                remainder = (remainder << 1);
            }
        }

        /*
         * Store the result into the table.
         */
        crcTable[dividend] = remainder;
    }

}   /* crcInit() */



/**  @fn CRC32 crcFast(MSU_UINT8 const message[], MSU_SINT16 nBytes)
 *   @details  This function takes data buffer as input parameter and number of bytes as input to function 
 *             it calculate crc on these number of bytes and returns crc value
 *   @param[in] : MSU_UINT8 const message[]   Message buffer contains data
 *   @param[in] : MSU_SINT16 nBytes     number of bytes in message buffer
 *   @return    :  it returns CRC32, it is calculated crc value on buffer.
 *   @retval    :  MSU_UINT32 value
 */
CRC32 crcFast(MSU_UINT8 const message[], MSU_SINT16 nBytes)
{
   MSU_UINT8 data;
   CRC32 remainder = 0;
   int byte;
   MSU_UINT32 retCrc = 0;

    /*
     * Divide the message by the polynomial, a byte at a time.
     */
   for (byte = 0; byte < nBytes; ++byte)
   {
      data = (MSU_UINT8)(message[byte] ^ (remainder >> (WIDTH - 8)));
      remainder = (CRC32)(crcTable[data] ^ (remainder << 8));
       
      retCrc = retCrc + remainder;
      remainder = 0;
   }

   
   return (retCrc);

}   

#endif // MSU_CRC32_CPP