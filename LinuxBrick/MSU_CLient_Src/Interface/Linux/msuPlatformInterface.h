#ifndef __msuInterface__hpp
#define __msuInterface__hpp

#include "MsuDataTypes.h"

extern void msuMemorySet( MSU_UINT8* src, MSU_UINT8 val, MSU_UINT16 len);
extern MSU_UINT8* msuGetMemoryFromPool(MSU_UINT32 size);
extern MSU_SINT32 msuReleaseMemoryToPool(MSU_UINT8 * memoryPool);
extern MSU_SINT32 msuByteCopy(MSU_UINT8* dst, MSU_UINT8*  src, MSU_UINT32 len);
extern MSU_SINT32 msuStrNCmp( MSU_SINT8* src, MSU_SINT8* dst, MSU_UINT32 len);
extern MSU_SINT32 msuStrLen(MSU_SINT8* str); 

extern MSU_UINT32 msuHtoNl(MSU_UINT32);
extern MSU_UINT32 msuNtoHl(MSU_UINT32);
extern MSU_UINT16 msuHtoNs(MSU_UINT16);
extern MSU_UINT16 msuNtoHs(MSU_UINT16);

#define MSU_START_STRUCT_PACK         _Pragma("pack(1)")
#define MSU_END_STRUCT_PACK           _Pragma("pack()")

extern void msuLogMsg(msuEnumDebuglevel Dlevel, MSU_SINT8* msg, ... );		      


#define CHUNK_IN_RAM

/*   @struct MSU_MAX_SEQ_NUMBER
*    @brief Macro defining MAX sequence Number 32 ( Must be between 1 to 32)
*/
#define MSU_MAX_SEQ_NUMBER  32

/*   @struct MSU_MAX_SEQ_SIZE
*    @brief Macro defining MAX sequence size 1460 bytes
*/
#define MSU_MAX_SEQ_SIZE 1460    


#endif //__msuInterface__hpp



