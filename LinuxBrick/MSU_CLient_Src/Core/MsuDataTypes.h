#ifndef __MsuDataTypes__h

#define __MsuDataTypes__h

typedef unsigned char           MSU_UINT8;
typedef char                    MSU_SINT8;
typedef unsigned short          MSU_UINT16;
typedef signed short            MSU_SINT16;
typedef unsigned long           MSU_UINT32;
typedef signed long             MSU_SINT32;
typedef unsigned char           MSU_BOOL;

#define MSU_EXTERN              extern

#define MSU_TRUE                    1                        
#define MSU_FALSE                   0


#define MSU_NULL                0


#define MSU_ERROR               -1
#define MSU_SUCCESS             0

typedef enum
{
	MSU_DEBUG_LEVEL_CRITICAL,
	MSU_DEBUG_LEVEL_ERROR,
	MSU_DEBUG_LEVEL_NORMAL,
	MSU_DEBUG_LEVEL_SUCCESS	
}msuEnumDebuglevel;







#endif//__MsuDataTypes__h