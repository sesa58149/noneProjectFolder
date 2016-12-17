#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include "msuPlatformInterface.h"
#include "MsuPdu.h"

#include <sys/types.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdarg.h>

#define MSU_MAX_LOG_MSG_SIZE   80
void msuMemorySet( MSU_UINT8* src, MSU_UINT8 val, MSU_UINT16 len)
{
    memset(src,val,len);
    return;
}

MSU_UINT8* msuGetMemoryFromPool(MSU_UINT32 size)
{
    return (MSU_UINT8*)malloc(size);	
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
	return 0;
}
 //-------------- Network byte order changes    
MSU_UINT32 msuHtoNl(MSU_UINT32 val)
{        
    return htonl(val); 
	
}

MSU_UINT32 msuNtoHl(MSU_UINT32 val)
{    
    return htonl(val);    
	
}

MSU_UINT16 msuHtoNs(MSU_UINT16 val)
{    
    return htons(val);
	
    
}

MSU_UINT16 msuNtoHs(MSU_UINT16 val)
{    
    return htons(val);
	
}


void msuLogMsg( msuEnumDebuglevel debugLevel, MSU_SINT8* msg, ...)
{
	MSU_SINT8 str[MSU_MAX_LOG_MSG_SIZE];
	va_list arglist;

	va_start( arglist, msg );
	vsnprintf( str, sizeof(str), msg, arglist );
	va_end( arglist );

	str[sizeof(str)-1] = '\0';
	
    printf(msg);
}
