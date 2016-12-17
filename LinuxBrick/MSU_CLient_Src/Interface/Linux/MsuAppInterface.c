
#include "MsuPdu.h"

/**  @fn MSU_SINT32 msuSetCommunicationParameters(MSU_COMM_PARAM * param)
 *   @brief This function is used to set the MSU communication Protocol parameters
 *          if param == NULL set to default value else set to input value. If application
 *          did not set this value than takes the default value.
 *   @param[in] : MSU_COMM_PARAM * param   Parameters data type *
 *   @return    : success -0 / fail -1
 */
MSU_SINT32 setMsuCommunicationParam(MSU_COMM_PARAM * param)
{
  msuSetCommunicationParameters(param);
  return MSU_SUCCESS;
}

/**@fn MsuGetDeviceIdentification(MSUDeviceIdentification* devInfo)
 * @Brief: Set the device information from application need for discovery message
 *         if not set then take default value as define in MSU core
 * @param[in] : MSUDeviceIdentification* devIdentification   Device information data type
 * @return    : success -0 / fail -1
 */
MSU_SINT32 setMsuDeviceIdentification(MSUDeviceIdentification* devIdentification)
{

    return MSU_SUCCESS;
}
/**@fn MsuEntryPoint()
 * @Brief: entry point of msu state machine. After all initialization and configuration
 *         this would be the last call for application
 */
void MsuEntryPoint()
{
    msuLeechStateMachine();
}
