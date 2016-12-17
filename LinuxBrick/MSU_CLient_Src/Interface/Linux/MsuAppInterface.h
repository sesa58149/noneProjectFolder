/*
 * MsuAppInterface.h
 *
 *  Created on: May 15, 2015
 *      Author: SESA58149
 */

#ifndef MSUAPPINTERFACE_H_
#define MSUAPPINTERFACE_H_

#include "MsuPlatformInterface.h"

#ifdef __cplusplus
extern "C"
{
#endif
MSU_EXTERN void MsuEntryPoint();
MSU_EXTERN MSU_SINT32 setMsuCommunicationParam(MSU_COMM_PARAM * param);
MSU_EXTERN MSU_SINT32 setMsuDeviceIdentification(MSUDeviceIdentification* devInfo);

#ifdef __cplusplus
}
#endif


#endif /* MSUAPPINTERFACE_H_ */
