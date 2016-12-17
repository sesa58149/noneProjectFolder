#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <stdarg.h>

#include "salStackStructs.h"
#include "msuPlatformInterface.h"
#include "MsuPdu.h"

#define MSU_MAX_LOG_MSG_SIZE   80

MsuInfo g_info_array[32];
extern void msuLeechStateMachine();

extern void rtxc_printf(char* msg, ...);

void msuMemorySet( MSU_UINT8* src, MSU_UINT8 val, MSU_UINT16 len)
{
    memset(src,val,len);
    return;
}

MSU_UINT8* msuGetMemoryFromPool(MSU_UINT32 size)
{
    //return (MSU_UINT8*)malloc(size);
    return (MSU_UINT8*)g_info_array;
}

MSU_SINT32  msuReleaseMemoryToPool(MSU_UINT8 * memoryPool)
{
    free(memoryPool);
    return 0;
}
MSU_SINT32 msuByteCopy(MSU_UINT8* dst, MSU_UINT8* src, MSU_UINT32 len)
{
    memcpy(dst,src,len);
    return 0;
}
MSU_SINT32 msuStrNCmp( MSU_SINT8*  src, MSU_SINT8*  dst, MSU_UINT32 len)
{
    strncmp(src, dst, len);
    return 0;
}
MSU_SINT32 msuStrLen(MSU_SINT8* const str)
{
    return strlen(str);
}
 //-------------- Network byte order changes    
MSU_UINT32 msuHtoNl(MSU_UINT32 val)
{        
    return trHtonl(val);    
}

MSU_UINT32 msuNtoHl(MSU_UINT32 val)
{    
    return trNtohl(val);    
}

MSU_UINT16 msuHtoNs(MSU_UINT16 val)
{    
    return trNtohs(val);
    
}

MSU_UINT16 msuNtoHs(MSU_UINT16 val)
{    
    return trNtohs(val);
}

MSU_SINT32 MsuGetDeviceIdentification(MSUDeviceIdentification* devInfo)
{
    return -1;
}
void MsuEntryPoint()
{
   msuLeechStateMachine();
}
void msuLogMsg( msuEnumDebuglevel debugLevel,MSU_SINT8* msg, ...)
{
	MSU_SINT8 str[MSU_MAX_LOG_MSG_SIZE];
	va_list arglist;

	va_start( arglist, msg );
	vsnprintf( str, sizeof(str), msg, arglist );
	va_end( arglist );

	str[sizeof(str)-1] = '\0';

    rtxc_printf(str);
}