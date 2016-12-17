#ifndef __MSU_FILE_OPERATION_H
#define __MSU_FILE_OPERATION_H

#define MSU_FILE  FILE

/**  @def MSU_SOCKET_ERROR 
*    @brief Macro to be defined for Error value (-1)
*/
#define MSU_MAX_FILE_PATH_SIZE (MSU_MAX_TAG_SIZE * 2 + 1)


#define MSU_MAX_FILE_MODE_LENGTH 2

#define MSU_FILE_READ "r"
#define MSU_FILE_WRITE "w"
#define MSU_FILE_READ_WRITE "r+"
#define MSU_FILE_WRITE_CREATE "w+"
#define MSU_FILE_APPEND "a"
#define MSU_FILE_READ_APPEND "a+"

#define FILE_DESC_ERROR -1
/**  @def MSU_SOCKET_ERROR 
*    @brief Macro to be defined for Error value (-1)
*/
#define MSU_FILE_DEFAULT_PATH "flash0:/fw/"

/**  @def MSU_SOCKET_ERROR 
*    @brief Macro to be defined for Error value (-1)
*/
#define MSU_FILE_DEFAULT_NAME "MsuFile.txt"

/**  @def MSU_SOCKET_ERROR 
*    @brief Macro to be defined for Error value (-1)
*/
#define MSU_DEVICE_INFO_FILE_NAME "flash0:/fw/msu.bin"

/**  @def MSU_FILE_SEEK_START 
*    @brief Macro to be defined for file reading from Start
*/

/**  @def MSU_FILE_SEEK_CURRENT 
*    @brief Macro to be defined for file reading from current position
*/

/**  @def MSU_FILE_SEEK_END 
*    @brief Macro to be defined for file reading from end of the file position
*/

typedef enum file_seek
{
    MSU_FILE_SEEK_UNKOWN        =-1,
    MSU_FILE_SEEK_START         =0,
    MSU_FILE_SEEK_CURRENT       =1,
    MSU_FILE_SEEK_END           =2    
}MSU_FILE_SEEK_OPERATION;

#define MSU_FILE_CRC_BUFFER_SIZE 255

/*   @struct EMsuFileOpenMode
*    @brief enum defining file operation mode
*    
*/
typedef enum
{
 eMsuFileR = 1,
 eMsuFileW = 2,
 eMsuFileRW = 3,
 eMsuFileApnd = 4
}EMsuFileOpenMode;



#endif //__MSU_FILE_OPERATION_H