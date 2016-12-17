/**	@file MsuPdu.hpp
 *	<TABLE>
 *	@brief Implementation of MSU PDU header
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

#ifndef MSUPDUHPP
#define MSUPDUHPP

#include "MsuPlatformInterface.h"

/*   @struct msuSockinfo
*    @brief Structure defining UINT32 IP Address and UINT16 port number
*    @details Defines structure with two variables, one for the 32 bit IP Address 
*             and other for 16 bit port number
*/
typedef struct
{
	MSU_UINT32 ipAddress;
	MSU_UINT16 port;
}msuSockinfo;

/*   @struct MSU_OPCODE_UPGRADE
*    @brief Macro defining MSU Opcode for UPGRADE 1
*/
#define MSU_OPCODE_UPGRADE 1

/*   @struct MSU_OPCODE_DEVICE_DISCOVER
*    @brief Macro defining MSU Opcode for DEVICE_DISCOVER 2
*/
#define MSU_OPCODE_DEVICE_DISCOVER 2

/*   @struct MSU_OPCODE_DEVICE_GROUP
*    @brief Macro defining MSU Opcode for DEVICE_GROUP 3
*/
#define MSU_OPCODE_DEVICE_GROUP 3

/*   @struct MSU_OPCODE_USER_CMD
*    @brief Macro defining MSU Opcode for USER_CMD 4
*/
#define MSU_OPCODE_USER_CMD 4

/*   @struct MSU_SUBCODE_UPGRADE
*    @brief Macro defining MSU Sub code for UPGRADE 1
*/
#define MSU_SUBCODE_UPGRADE 1

/*   @struct MSU_SUBCODE_DOWNGRADE
*    @brief Macro defining MSU Sub code for DOWNGRADE 2
*/
#define MSU_SUBCODE_DOWNGRADE 2

/*   @struct MSU_SUBCODE_FORCE_UPGRADE
*    @brief Macro defining MSU Sub code for FORCE_UPGRAD 3
*/
#define MSU_SUBCODE_FORCE_UPGRADE 3

/*   @struct MSU_SUBCODE_DATA_TRANSFER
*    @brief Macro defining MSU Sub code for DATA_TRANSFER 4
*/
#define MSU_SUBCODE_DATA_TRANSFER 4

/*   @struct MSU_SUBCODE_SCM
*    @brief Macro defining MSU Sub code for SCM 5
*/
#define MSU_SUBCODE_SCM 5

/*   @struct MSU_SUBCODE_CCM
*    @brief Macro defining MSU Sub code for CCM 6
*/
#define MSU_SUBCODE_CCM 6

/*   @struct MSU_SUBCODE_TRANSFER_COMPLETE
*    @brief Macro defining MSU Sub code for TRANSFER_COMPLETE 7
*/
#define MSU_SUBCODE_TRANSFER_COMPLETE 7

/*   @struct MSU_SUBCODE_ABORT_TRANSFERT
*    @brief Macro defining MSU Sub code for ABORT_TRANSFERT 8
*/
#define MSU_SUBCODE_ABORT_TRANSFERT 8

/*   @struct MSU_SUBCODE_CCM_TRANSFER_COMPLETED
*    @brief Macro defining MSU Sub code for TRANSFER_COMPLETED 9
*/
#define MSU_SUBCODE_CCM_TRANSFER_COMPLETED 9


#define MSU_SUBCODE_SCM_TRANSFER_COMPLETED 10

/*   @struct MSU_SUBCODE_CLIENT_STATUS_REQUEST
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_REQUEST 10
*/
#define MSU_SUBCODE_CLIENT_STATUS_REQUEST 11

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_SUBCODE_CLIENT_STATUS_RESPONSE 12


/*   @struct MSU_OPCODE_AUTHENTICATION
*    @brief Macro defining MSU Opcode for authentication 4
*/
#define MSU_OPCODE_AUTHENTICATION 4


/*   @struct MSU_SUBCODE_DEVICE_DISCOVERY_REQUEST
*    @brief Macro defining MSU Sub code for DEVICE_DISCOVERY_REQUEST 1
*/
#define MSU_SUBCODE_DEVICE_DISCOVERY_REQUEST 1

/*   @struct MSU_SUBCODE_DEVICE_DISCOVERY_RESPONSE
*    @brief Macro defining MSU Sub code for DEVICE_DISCOVERY_RESPONSE 2
*/
#define MSU_SUBCODE_DEVICE_DISCOVERY_RESPONSE 2

/*   @struct MSU_SUBCODE_SET_GROUP
*    @brief Macro defining MSU Sub code for SET_GROUP 1
*/
#define MSU_SUBCODE_SET_GROUP 1

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_SUBCODE_RESET_GROUP 2

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_SUBCODE_ACK_GROUP 3

/*   @struct MSU_SUBCODE_AUTHENTICATION_REQUEST
*    @brief Macro defining MSU Sub code for authentication request 
*/
#define MSU_SUBCODE_AUTHENTICATION_REQUEST 1

/*   @struct MSU_SUBCODE_AUTHENTICATION_RESPONSE
*    @brief Macro defining MSU Sub code for authentication response 
*/
#define MSU_SUBCODE_AUTHENTICATION_RESPONSE 2

/*   @struct MSU_SUBCODE_DISCONNECT_REQUEST
*    @brief Macro defining MSU Sub code for disconnect request 
*/
#define MSU_SUBCODE_DISCONNECT_REQUEST 3

/*   @struct MSU_SUBCODE_DISCONNECT_RESPONSE
*    @brief Macro defining MSU Sub code for disconnect response 
*/
#define MSU_SUBCODE_DISCONNECT_RESPONSE 4


/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_SUBCODE_USER_CMD_STOP 1

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_SUBCODE_USER_CMD_INIT 2

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_SUBCODE_USER_CMD_START 3

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_UPGRADE_STATUS_PASS 0

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_UPGRADE_STATUS_FAIL 1

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_IPV4_VERSION 0

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_IPV6_VERSION 1

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_PROTOCOL_VERSION 2

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_MIN_SEQ_NUMBER  1

/*   @struct MSU_MAX_SEQ_NUMBER
*    @brief Macro defining MAX sequence Number 32
*/
#define MSU_MAX_SEQ_NUMBER  32

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_MIN_CHUNK_NUMBER  1

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_MAX_CCM_CHUNK_NUMBER  256  

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_MAX_FIELD_NAME_LENGTH 127

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_FIELD_PRESENT 1

/*   @struct MSU_MAX_PACKET_SIZE
*    @brief Macro defining MAX Packet size 1500 bytes
*/
#define MSU_MAX_PACKET_SIZE 1500

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_MAX_TAG_COUNT 2

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_DEVICE_INFO_TAG_COUNT 8

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_MAX_TAG_SIZE 127 //127 bytes excluding NULL character
#define MSU_AUTHENTICATION_TAG_COUNT 10
#define MAX_BYTES_IPv6_ADDRESS 40

#define MSU_COMMPARAM_SEQDELAY 50
#define MSU_COMMPARAM_SCMDELAY 5
#define MSU_COMMPARAM_CCMDELAY 100
#define MSU_COMMPARAM_CCMRETRY 1
#define MSU_COMMPARAM_SCMRETRY 1
#define MSU_COMMPARAM_COMMTIMEOUT 120


/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/

#define MSU_MAX_IP_FIELDS 0 //10

#define MSU_CLIENT_STATUS 1 

#define MSU_MASTER_STATUS 0 

#define MSU_AUTHENTICATION_TAG 1 

#define MSU_AUTHETICATION_LEVEL_TAG  1

#define MSU_DEFAULT_SERVER_IP 0xC0A80164 //10

#define MSU_VENDORNAME_LENGTH 20

#define MSU_HARDWAREID_LENGTH 1

#define MSU_PRODUCTCODE_LENGTH 14

#define MSU_MAJOR_MINOR_REVISION_LENGTH 4

#define MSU_MAX_FILES_SUPPORTED 3

/*********************************************************************/
				/* Adding on 5th December */
/*********************************************************************/
//#define MAX_NO_DEVICES 5
//#define COMMON_STRUCTURE_SIZE 46
#define MAC_LEN 6
#define IP_LEN 40				 

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_MAX_TAG_BUFFER_SIZE (MSU_MAX_TAG_COUNT * MSU_MAX_TAG_SIZE)

/*   @struct MSU_MAX_SEQ_SIZE
*    @brief Macro defining MAX sequence size 1460 bytes
*/
#define MSU_MAX_SEQ_SIZE 1460    

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define I_AM_VARIABLE_DATA_INFO 8 + 4 


/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_HARDWARE_ID "1"

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_PRODUCT_ID "2"

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_PRODUCT_NAME "MSU Node"

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_MODEL_NAME "MSU Client"

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_VENDOR_NAME "Schneider"

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_PRODUCT_FIRMWARE_VERSION "1.0"

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_MAJOR_MINOR_REVISION "1.01"

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_DEVICE_LOCATION MSU_FILE_DEFAULT_PATH

/*   @struct MSU_DEFAULT_PASSWORD
*    @brief Macro defining MSU default password
*/

#define MSU_DEFAULT_PASSWORD "admin"

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU default username
*/
#define MSU_DEFAULT_USERNAME "admin"

#define MSU_DEFAULT_FILENAME "profile.bin"

#define MSU_DEFAULT_FILENAME1 "text1.txt"

#define MSU_DEFAULT_FILENAME2 "text2.txt"

#define MSU_DEFAULT_KEY "text1"

#define MSU_USERPASS_LEN 10

#define MSU_KEY_LEN 10

#define MAX_FILENAME_LEN 32

#define MAX_FILE_SUPPORTED	3

#define MSU_VENDORNAME_LENGTH 20

#define MSU_PRODUCTCODE_LENGTH 14

#define MSU_MAJOR_MINOR_REVISION_LENGTH 4


MSU_START_STRUCT_PACK

/*   @struct MSUCmd
*    @brief Structure defining MSU opcode and subCode
*    @details Defines structure with holds opcode and subcode variables, 
*              opcode occupies 4 bits and subcode occupies 4 bits 
*/
typedef struct
{
#ifdef MSU_PLATFORM_BIG_ENDIAN
 MSU_UINT8 opcode : 4;
 MSU_UINT8 subcode : 4;
#else
 MSU_UINT8 subcode : 4;
 MSU_UINT8 opcode: 4;
#endif
} MSUCmd;


/*   @struct VerInfo
*    @brief Structure defining MSU versionIP, versionProtocol and reserved fields.
*    @details Defines structure which holds versionIP, versionProtocol and reserved fields, 
*    versionIP occupies 1 bit, versionProtocol occupies 3 bits and reserved occupies 1 bit
*/
typedef struct
{
#ifdef MSU_PLATFORM_BIG_ENDIAN
 MSU_UINT8 versionIP : 1;
 MSU_UINT8 versionProtocol : 3;
 MSU_UINT8 reserved : 4;
#else
 MSU_UINT8 reserved : 4;
 MSU_UINT8 versionProtocol : 3;
 MSU_UINT8 versionIP : 1;
#endif
} VerInfo;


/*   @struct FieldNameTag
*    @brief Structure defining MSU isFieldPresent, fieldLength fields.
*    @details Defines structure which holds isFieldPresent and fieldLength fields, 
*    isFieldPresent occupies 1 bit, fieldLength occupies 73 bits 
*/
typedef struct
{
#ifdef MSU_PLATFORM_BIG_ENDIAN
 MSU_UINT8 isFieldPresent : 1;
 MSU_UINT8 fieldLength : 7;
#else
 MSU_UINT8 fieldLength : 7;
 MSU_UINT8 isFieldPresent : 1;
#endif
} FieldNameTag;


/*   @struct FileTransferInfo
*    @brief Structure defining MSU fileTransferState, chunkTransferState and reserved fields.
*    @details Defines structure which holds fileTransferState,chunkTransferState and reserved fields, 
*    fileTransferState occupies 1 bit, chunkTransferState occupies 1 bit and reserved occupies 6 bits 
*/
typedef struct
{
#ifdef MSU_PLATFORM_BIG_ENDIAN
 MSU_UINT8 fileTransferState : 1;
 MSU_UINT8 chunkTransferState : 1; 
 MSU_UINT8 reserved : 6;
#else
 MSU_UINT8 reserved :6; 
 MSU_UINT8 chunkTransferState : 1;
 MSU_UINT8 fileTransferState : 1;
#endif
} FileTransferInfo;


/*   @struct MSUHeader
*    @brief Structure defining MSU command, fileNumber and versionInfo fields.
*    @details Defines structure which holds command, fileNumber and versionInfo fields, 
*             command is a type of MSUCmd structure variable which holds opcode and subcode,
*             fileNumber which occupies UINT16 bits and versionInfo is type of VerInfo structure variable 
*             holds versionIP, versionProtocol and reserved fields
*/
typedef struct
{
 MSUCmd command;
 MSU_UINT16 fileNumber;
 VerInfo versionInfo;
} MSUHeader;

typedef union
{
	MSU_UINT32 ServerIPv4;
	MSU_UINT8  ServerIPv6[MAX_BYTES_IPv6_ADDRESS]; 
} MSUIpAddressFormat;

typedef union
{
	MSU_UINT32 transactionid;
	MSU_UINT8  transactionencrypt[4];
} MSUtransactionid;



/*   @struct AuthLevel
*    @brief Structure defining Authentication.
*    @details Defines structure which holds IsClient,IsMaster,Is Authentication required,Level.
*/
typedef struct
{
 #ifdef MSU_PLATFORM_BIG_ENDIAN
 	MSU_UINT8 IsClient:1;
 	MSU_UINT8 IsMaster:1;
 	MSU_UINT8 IsAuthRequired:1;
 	MSU_UINT8 Level:1;
 	MSU_UINT8 Reserved:4;
 #else
 	MSU_UINT8 Reserved:4;
 	MSU_UINT8 Level:1;
 	MSU_UINT8 IsAuthRequired:1;
 	MSU_UINT8 IsMaster:1;
 	MSU_UINT8 IsClient:1;
 #endif	
 } AuthLevel;


/************************************************************/
	   	//Adding on Dec 6//Security Info struct
/************************************************************/
typedef struct
{
	char username[9];
	char password[9];
	char IP[1][IP_LEN];//only for testing
}SecurityInfo;

	//	char MAC[1][MAC_LEN];//only for testing
	//  char IP[MAX_NO_DEVICES][IP_LEN];//
	//  char MAC[MAX_NO_DEVICES][MAC_LEN];
 


/*   @struct MSUDeviceInfo
*    @brief Structure defining MSU Device information fields
*    @details Defines structure which holds hardwareIdTag,productIdTag,productNameTag,
*             modelNameTag,vendorIdTag,firmwareVersionTag,revisionTag,deviceLocTag information which are type of
*             FieldNameTag structure which holds isFieldPresent and fieldLength
*             it also contains deviceInfoData char arrey which holds information
*/
typedef struct
{
	MSU_UINT8 Client;
	MSU_UINT8 Master;
	MSU_UINT8 AutheticationTag;
	MSU_UINT8 AuthenticationLevelTag;
	MSU_UINT8 NumberOfIPSupported;
	MSU_UINT8 noOfFileSupported;
	MSUIpAddressFormat IP[10];
	MSU_UINT8 MAC[6];
	MSU_SINT8 Username[MSU_USERPASS_LEN];
	MSU_SINT8 Password[MSU_USERPASS_LEN];
	MSU_SINT8 filename[MAX_FILE_SUPPORTED][MAX_FILENAME_LEN];
	MSU_SINT8 key[MSU_KEY_LEN];
	MSU_UINT16 sequenceLimit;
	
}MSUDeviceInfo;





//typedef struct SecurityInfo var;//adding on Dec 09-Security Info;
// SecurityInfo var;
typedef struct //this would not be used,SecurityInfo is enough//
{
	char username[15];
	char password[15];
}login_info;


/*   @struct MSUSyncHeaderformat
*    @brief Structure defining MSU Sync message fixed frame format
*    @details Defines structure which holds MSU SyYnc Message fixed frame information, sync message contains 
*             msuHeader,fileSize,chunkCount,seqNumberLimit,seqSize,serverIP,multicastIP,multicastCmIP
*             multicastPort,multicastCmPort,transactionID,fileChecksum,fileNameTag,destPathTag,groupIDTag,
*             updateTimeout,fileName and  destPath. 
*/
typedef struct
{
 MSUHeader msuHeader;
 MSU_UINT32 fileSize;
 MSU_UINT32 chunkCount;
 MSU_UINT16 seqNumberLimit;
 MSU_UINT16 seqSize; 
 MSUIpAddressFormat multicastIP;
 MSUIpAddressFormat multicastCmIP;
 MSU_UINT16 multicastPort;
 MSU_UINT16 multicastCmPort;
 MSUtransactionid transactionID;
 MSU_UINT32 fileChecksum;
 FieldNameTag fileNameTag;
 FieldNameTag destPathTag;
 MSU_UINT8 reserveFeild;
 MSU_UINT8 updateTimeout;
 
} MSUSyncHeaderformat;


/*   @struct MSUSyncMessagePdu
*    @brief Structure defining MSU Sync message PDU
*    @details Defines structure which holds MSU SyYnc Message information, sync message contains 
*    msuHeader,fileSize,chunkCount,seqNumberLimit,seqSize,serverIP,multicastIP,multicastCmIP
*    multicastPort,multicastCmPort,transactionID,fileChecksum,fileNameTag,destPathTag,groupIDTag,
*    updateTimeout,fileName and  destPath. 
*/

typedef struct
{
 MSU_UINT8 fileName[MSU_MAX_TAG_SIZE];
 MSU_UINT8 destPath[MSU_MAX_TAG_SIZE]; 
} MSUSyncMessageVariableData;



/*   @struct MSUSyncMessagePdu
*    @brief Structure defining MSU Sync message PDU
*    @details Defines structure which holds MSU SyYnc Message information, sync message contains 
*    msuHeader,fileSize,chunkCount,seqNumberLimit,seqSize,serverIP,multicastIP,multicastCmIP
*    multicastPort,multicastCmPort,transactionID,fileChecksum,fileNameTag,destPathTag,groupIDTag,
*    updateTimeout,fileName and  destPath. 
*/

typedef struct  
{
  MSUSyncHeaderformat msuSyncHeaderformat;
  MSUSyncMessageVariableData msuSyncMessageVariableData;
} MSUSyncMessagePdu;


/*   @struct MSUMulticastPdu
*    @brief Structure defining MSU Multicasr PDU
*    @details Defines structure which holds MSU msuHeader,chunkNumber,seqNumber,dataLength,fileTransferInfo
*             and data pointer.
*/
typedef struct
{
 MSUHeader msuHeader;
 MSU_UINT32 chunkNumber;
 MSU_UINT8 seqNumber;
 MSU_UINT16 dataLength;
 FileTransferInfo fileTransferInfo;
 MSU_UINT8 *pData;
} MSUMulticastPdu;

/*   @struct MSUTransfer structure  */
/*typedef struct 
{
char Info[MSU_MAX_SEQ_NUMBER*MSU_MAX_SEQ_SIZE];
UINT16 chunksize;	
}MsuInfo; */
typedef struct 
{
char Info[1460];
//UINT16 seqsize;	
}MsuInfo;

/*   @struct MSUSCMPdu
*    @brief Structure defining MSU SCM PDU
*    @details Defines structure which holds MSU msuHeader,chunkNumber,seqCount,reserved and seqBitMap
*/
typedef struct
{
 MSUHeader msuHeader;
 MSU_UINT32 chunkNumber;
 MSU_UINT8 seqCount;
 MSU_UINT8 reserved[3];
 MSU_UINT32 seqBitMap;
} MSUSCMPdu;

/*   @struct MSUCCMPdu
*    @brief Structure defining MSU CCM PDU
*    @details Defines structure which holds MSU msuHeader,chunkNumber
*/
typedef struct
{
 MSUHeader msuHeader;
 MSU_UINT32 chunkCount;
} MSUCCMPdu;

/*   @struct MSUDiscoveryRequestPdu
*    @brief Structure defining MSU Discovery Request PDU
*    @details Defines structure which holds MSU command,isRange,versionInfo,IP address startRange and endRange
*/
typedef struct
{
 MSUCmd command;
 MSU_UINT8 isRange;
 MSU_UINT8 MsgType;
 VerInfo versionInfo;
 MSUIpAddressFormat ServerIpStart;
 MSUIpAddressFormat ServerIpEnd; 
 } MSUDiscoveryRequestPdu;

typedef struct
{	
MSU_UINT8 bufferdiscoveryresponse[MSU_MAX_TAG_SIZE];	
} MSUDiscoveryResponseVarlength;


/*   @struct BasicDeviceIdentification
*    @brief Structure defining BasicDeviceIdentification fields
*    @details Defines structure which holds MSU command,isRange,versionInfo,IP address startRange and endRange
*/
typedef struct
{
 FieldNameTag hardwareIdTag;
 FieldNameTag productIdTag;
 FieldNameTag productNameTag;
 FieldNameTag modelNameTag;
 FieldNameTag vendorIdTag;
 FieldNameTag firmwareVersionTag;
 FieldNameTag revisionTag;
 FieldNameTag deviceLocTag;
 MSU_UINT8 hardwareId[20];
 MSU_UINT8 productId[20];
 MSU_UINT8 productName[20];
 MSU_UINT8 vendorname[20];
 MSU_UINT8 modelName[20];
 MSU_UINT8 firmwareVersion[20];
 MSU_UINT8 revision[20];
 MSU_UINT8 deviceLoc[20];  
} MSUDeviceIdentification;



/*   @struct MSUDiscoveryResponsePdu
*    @brief Structure defining MSU Discovery Response PDU
*    @details Defines structure which holds MSU command,GroupId,versionInfo,deviceID,hardwareIdTag,productIdTag,
*             productNameTag,modelNameTag,vendorIdTag,firmwareVersionTag,revisionTag,deviceLocTag and reservedUser.
*/
typedef struct
{
 MSUCmd command;
 MSU_UINT8 GroupId;
 AuthLevel AuthenticationLevel;
 VerInfo versionInfo;
 MSU_UINT32 deviceID[10];
 FieldNameTag hardwareIdTag;
 FieldNameTag productIdTag;
 FieldNameTag productNameTag;
 FieldNameTag modelNameTag;
 FieldNameTag vendorIdTag;
 FieldNameTag firmwareVersionTag;
 FieldNameTag revisionTag;
 FieldNameTag deviceLocTag;
 FieldNameTag MsuCommParam ;
 MSU_UINT8 reservedUser[3];
 MSU_UINT8 varlengthdata[MSU_MAX_TAG_SIZE];
} MSUDiscoveryResponsePdu;


/*    @struct MSUAuthenticationRequestPdu
 *    @brief Structure defining MSU Authentication Request PDU
 *    @details Defines structure which holds MSU command,ipversion,serverip,transaction id,filename,hardwareid,
 *    productid,modelname,username,password,FWVersion,SWVersion,vendorid,productname
 */
typedef struct
{
 MSUCmd command;
 MSU_UINT8 reserved;
 AuthLevel AuthenticationLevel; 
 VerInfo versionInfo;
 MSUIpAddressFormat msuServerIP;
 MSUtransactionid transactionid;
 MSU_UINT8  MACAddr[6];
 FieldNameTag FileNameTag;
 FieldNameTag hardwareIdTag;
 FieldNameTag productIdTag;
 FieldNameTag modelnameTag;
 FieldNameTag passwordTag;
 FieldNameTag usernameTag;
 FieldNameTag firmwareversionTag;
 FieldNameTag softwareversionTag;
 FieldNameTag vendorIdTag;
 FieldNameTag productnameTag;
 } MSUAuthenticationHeader;
 /*    @struct MSUAuthenticationRequestPdu
 *    @brief Structure defining MSU Authentication Request PDU
 *    @details Defines structure which holds MSU command,ipversion,serverip,transaction id,filename,hardwareid,
 *    productid,modelname,username,password,FWVersion,SWVersion,vendorid,productname
 */
typedef struct
{

 MSU_UINT8 filename[MSU_MAX_TAG_SIZE];
 MSU_UINT8 HardwareId[MSU_MAX_TAG_SIZE];
 MSU_UINT8 ProductId[MSU_MAX_TAG_SIZE];
 MSU_UINT8 modelname[MSU_MAX_TAG_SIZE];
 MSU_UINT8 password[MSU_MAX_TAG_SIZE];
 MSU_UINT8 username[MSU_MAX_TAG_SIZE];
 MSU_UINT8 firmwareversion[MSU_MAX_TAG_SIZE];
 MSU_UINT8 softwareversion[MSU_MAX_TAG_SIZE];
 MSU_UINT8 vendorId[MSU_MAX_TAG_SIZE];
 MSU_UINT8 productname[MSU_MAX_TAG_SIZE];
 
 } MSUAuthenticationPayload;
 
/*    @struct MSUAuthenticationRequestPdu
 *    @brief Structure defining MSU Authentication Request PDU
 *    @details Defines structure which holds MSU command,ipversion,serverip,transaction id,filename,hardwareid,
 *    productid,modelname,username,password,FWVersion,SWVersion,vendorid,productname
 */
typedef struct
{
 	MSUAuthenticationHeader 	authHeader;
 	MSUAuthenticationPayload 	authPayload;
 } MSUAuthenticationRequestPdu;

/*    @struct MSUAuthenticationResponsePdu
 *    @brief Structure defining MSU Authentication Response PDU
 *    @details Defines structure which holds MSU command,ipversion,error code and error subcode
 */
 typedef struct
 {
  MSUCmd command;
  MSU_UINT8  ErrCode;
  MSU_UINT8  ErrSubCode;
  VerInfo versionInfo; 	
 }MSUAuthenticationResponsePdu;

/*   @struct MSUDisconnectPdu
*    @brief Structure defining MSU disconnect Pdu.
*    @details Defines structure which holds command, authenticationlevel,versionInfo,serverIp and transactionId fields, 
*             command is a type of MSUCmd structure variable which holds opcode and subcode,
*             authenticationlevel is a type ofAuthLevel structure ,versionInfo is type of VerInfo structure variable 
*             holds versionIP, versionProtocol and reserved fields,serverIp is a type of MSUIpAddressFormat structure,
*			  transactionId is a UINT32 field.
*/
typedef struct
{
 MSUCmd command;
 MSU_UINT8 reserverdfield;
 AuthLevel authenticationlevel;
 VerInfo versionInfo;
 MSUIpAddressFormat serverIp;
 MSUtransactionid transactionId;
} MSUDisconnectPdu;

/*   @struct MSUGroupCreationPdu
*    @brief Structure defining MSU Group creation PDU
*    @details Defines structure which holds MSU command,GroupId and versionInfo
*/
typedef struct
{
 MSUCmd command;
 MSU_UINT8 groupID;
 MSU_UINT8 reserved;
 VerInfo versionInfo;
 MSU_UINT32  deviceId;
} MSUGroupCreationPdu;


/*   @struct MSUUpgradeStatusPdu
*    @brief Structure defining MSU Upgrade status PDU
*    @details Defines structure which holds MSU command,status,errorCode,versionInfo,transactionID and deviceID
*/
typedef struct
{
 MSUCmd command;
 MSU_UINT8  status; 
 MSU_UINT8 errorCode;
 VerInfo versionInfo;
 MSUtransactionid transactionID;
 MSU_UINT32 deviceID;
 MSU_UINT8 ccmRetryCount;
 MSU_UINT8 reserve1[3];
} MSUUpgradeStatusPdu;

/*   @struct MSUGroupCreationAckPdu
*    @brief Structure defining MSU Group creation Ack PDU
*    @details Defines structure which holds MSU command,groupID,reserved and versionInfo.
*/
typedef struct
{
 MSUCmd command;
 MSU_UINT8 groupID;
 MSU_UINT8 reserved;
 VerInfo versionInfo;
 MSU_UINT32  deviceId;
} MSUGroupCreationAckPdu;


/*   @struct MSUCompletePdu
*    @brief Structure defining MSU Complete PDU
*    @details Defines structure which holds MSU msuHeader.
*/
typedef struct
{
 MSUHeader msuHeader;
 
} MSUCompletePdu;


/*   @struct MSUCCMCompletePdu
*    @brief Structure defining MSU CCM Complete PDU
*    @details Defines structure which holds MSU msuHeader.
*/
typedef struct
{
  MSUHeader msuHeader;
  MSU_BOOL ccmRetry;
  MSU_UINT8 reserved[3];
} MSUCCMCompletePdu;

/*   @struct MSUAbortMsgPdu
*    @brief Structure defining MSU Abort PDU
*    @details Defines structure which holds MSU msuHeader.
*/
typedef struct
{
  MSUHeader msuHeader;
  MSUtransactionid transactionID;

} MSUAbortMsgPdu;


typedef struct
{
  MSUHeader msuHeader;
  MSU_BOOL scmRetry;
  MSU_UINT8 reserved[3];
} MSUSCMCompletePdu;

typedef struct
{
   MSU_UINT16 seqDelay;
   MSU_UINT16 scmDelay;
   MSU_UINT16 ccmDelay;
   MSU_UINT8  ccmRetry;
   MSU_UINT8  scmRetry;
   MSU_UINT8  commTimeout_sec; 
}MSU_COMM_PARAM;


MSU_END_STRUCT_PACK

/**  @fn msuPduInit(void)
*    @brief This function Initializes the PDU data with 
*/ 
void msuPduInit(void);

/**  @fn msuSocketProcessSyncMessage(UINT8 *pMsg , UINT32 msgSize)
*    @brief This function process the sync message received from server
*/ 
MSU_BOOL msuSocketProcessSyncMessage(MSU_UINT8 *pMsg , MSU_UINT32 msgSize);

/**  @fn msuPduParseMcastRequest(UINT8 *pMsg , UINT32 msgSize)
*    @brief This function parses the multicast request received from server
*/ 
MSU_BOOL msuPduParseMcastRequest(MSU_UINT8 *pMsg , MSU_UINT32 msgSize);

/**  @fn msuParseDiscoverRequest(UINT8 *pMsg , UINT32 msgSize, MSUDiscoveryRequestPdu *pDiscoverRequest)
*    @brief This function parses the discovery request received from server
*/ 
MSU_BOOL msuParseDiscoverRequest(MSU_UINT8 *pMsg , MSU_UINT32 msgSize, MSUDiscoveryRequestPdu *pDiscoverRequest);

/**  @fn msuHandleDeviceGroupMsg(UINT8 *pMsg , UINT32 msgSize)
*    @brief This function handles device group request received from server
*/ 
MSU_BOOL msuHandleDeviceGroupMsg(MSU_UINT8 *pMsg , MSU_UINT32 msgSize);

/**  @fn isMsuVersion( MSUHeader *header)
*    @brief This function validates the MSU version
*/ 
MSU_BOOL isMsuVersion( MSUHeader *header);

/**  @fn msuHandleDeviceStatusUpdateRequest(char *gMsgBuffer,  UINT32 nByte)
*    @brief This function handles device status update request received from server
*/ 
MSU_BOOL msuHandleDeviceStatusUpdateRequest(MSU_SINT8 *gMsgBuffer,  MSU_UINT32 nByte);

/**  @fn msuCcmMsgCreate( MSUCCMPdu *ccmMsgPdu, UINT32 totalChnksExpected, UINT32 *missedChnkCnt)
*    @brief This function constructs the CCM message format
*/ 
void msuCcmMsgCreate( MSUCCMPdu *ccmMsgPdu, MSU_UINT32 totalChnksExpected, MSU_UINT32 *missedChnkCnt);

/**  @fn msuUpgrateMsgCreate( MSUUpgradeStatusPdu *msuUpgradeStatus, SINT32 upgradeStatus )
*    @brief This function constructs the Upgrade message format
*/ 
void msuUpgrateMsgCreate( MSUUpgradeStatusPdu *msuUpgradeStatus, MSU_SINT32 upgradeStatus,MSU_UINT8 ccmRetryCount );


/**  @fn msuDiscoveryResponseMsgCreate(MSUDiscoveryRequestPdu *discoveryRequestPdu,
		MSUDiscoveryResponsePdu *msuDiscoveryResponse, UINT16 *dataPayloadSize )
*    @brief This function constructs the Discovery response message format
*/ 
void msuDiscoveryResponseMsgCreate(MSUDiscoveryRequestPdu *discoveryRequestPdu,
		MSUDiscoveryResponsePdu *msuDiscoveryResponse, MSU_UINT16 *dataPayloadSize );

/**  @fn msuScmMsgCreate( MSUSCMPdu *scmMsgPdu,UINT32 bitMap, UINT32 chunkNum, UINT8 missedSeqCount)
*    @brief This function constructs the SCM message format
*/ 
void msuScmMsgCreate( MSUSCMPdu *scmMsgPdu,MSU_UINT32 bitMap, MSU_UINT32 chunkNum, MSU_UINT8 missedSeqCount);

/**  @fn msuGroupAckMsgCreate( MSUGroupCreationPdu *gReqPdu )
*    @brief This function constructs the Group creation Ack message format
*/ 
MSUGroupCreationPdu * msuGroupAckMsgCreate( MSUGroupCreationPdu *gReqPdu );

/**  @fn msuAuthenticationRequest(UINT8 *pMsg,MSUAuthenticationRequestPdu *pAuthenticationRequest,
		MSUAuthenticationResponsePdu *msuAuthenticationResponse, UINT16 *dataPayloadSize )
*    @brief This function parses the authentication request and constructs the response
*/
MSU_BOOL msuAuthenticationRequest(MSU_UINT8 *pMsg,MSU_UINT32 msgSize,MSUAuthenticationRequestPdu *pAuthenticationRequest,
			      MSUAuthenticationResponsePdu *msuAuthenticationResponse, msuSockinfo *socketip );

/**  @fn msuDecryption(UINT8 *inputstring ,char *privatekeydecrypt,UINT32 lengthstring, 
                       UINT32 length_privatekey);
*    @brief This function decrypts the encrypted data from server
*/ 
MSU_SINT32 msuDecryption(MSU_UINT8 *inputstring ,MSU_SINT8 *privatekeydecrypt,MSU_UINT32 lengthstring, MSU_UINT32 length_privatekey);


/**  @fn void msuDisconnectMsg(UINT8 *gMsg,UINT32 msgsize )
 *   @brief this function is used to parse disconnect message 
 */
 MSU_UINT8 msuDisconnectMsg(MSU_UINT8 *gMsg,MSU_UINT32 msgsize,MSUDisconnectPdu *variable );
 
 /**  @fn void msuDisconnectMsgResponseCreate( MSUAuthenticationResponsePdu *msuAuthenticationResponsePdu )
 *   @brief this function is used to create the disconnect message response
 */ 
 void msuDisconnectMsgResponseCreate( MSUAuthenticationResponsePdu *msuAuthenticationResponsePdu );
 
  /**  @fn BOOL msuHandleAbortMsg(UINT8* g_msgbuffer);
 *   @brief this function is used to handle abort message
 */ 

 MSU_BOOL msuHandleAbortMsg(MSU_UINT8* g_msgbuffer );

 /**    @fn MSU_SINT32 msuSetCommunicationParameters(MSU_COMM_PARAM * param)
  *     @Brief: Set the MSU communication protocol parameters
  */
 MSU_SINT32 msuSetCommunicationParameters(MSU_COMM_PARAM * param);

 /**    @fn MSU_SINT32 msuSetDeviceIdentification( MSUDeviceIdentification *deviceidentification);
   *     @Brief: Set DEVICE information used by discover message
   */
 MSU_SINT32 msuSetDeviceIdentification( MSUDeviceIdentification *deviceidentification);
#endif/*#ifndef MSUPDUHPP */
