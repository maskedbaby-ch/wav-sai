/*!
    @file IOClient_Api.h
    @version 0.0.13.0

    \brief IO Client API
    \project AUTOSAR S12X MCAL Software
    \author Petr Pomkla
    \author Freescale Semiconductor, Roznov pod Radhostem, Czech Republic

    \platform
    \peripheral
    \prerequisites
    \arversion 2.1.0
    \arrevision Rel.2.1, Rev.0017
    \swversion 2.1.0
    @lastmodusr B12795
    @lastmoddate Nov-19-2008

    Freescale Semiconductor Inc.
    (c) Copyright 2008 Freescale Semiconductor Inc.
    ALL RIGHTS RESERVED.
*/
/*! \addtogroup IOClient  @{ */

#ifndef IOCLIENT_API_H
#define IOCLIENT_API_H

/******************* inclusions **********************************************/
#include <mqx_inc.h>
#include "IOClient_Types.h"
#include "TL_TestPoints.h"

/******************* external data *******************************************/

/*!
    \brief Helper external variable to simplify serialization of the Function data type
*/
extern IOClientFunctionIDType _ioClientFunctionID;

/*!
    \brief Helper external variable to simplify serialization of enumeration data types
*/
extern IOClientEnumType _ioClientEnum;

/******************* macros **************************************************/

/*!
    \brief Big/Little endian settings. Default: Big endian
*/
#ifdef MQX_BIG_ENDIAN
#define HIGH_BYTE_FIRST MQX_BIG_ENDIAN
#endif

#ifdef MQX_LITTLE_ENDIAN
#define LOW_BYTE_FIRST  MQX_LITTLE_ENDIAN
#endif

#ifdef PSP_ENDIAN
#define CPU_BYTE_ORDER  PSP_ENDIAN
#endif

#ifndef HIGH_BYTE_FIRST
#define HIGH_BYTE_FIRST 0
#endif

#ifndef LOW_BYTE_FIRST
#define LOW_BYTE_FIRST 1
#endif

#ifndef CPU_BYTE_ORDER
#define CPU_BYTE_ORDER HIGH_BYTE_FIRST
#endif

/*!
    \brief IOClient communication channel (defaults to channel 0)
*/
#ifndef IOCLIENT_COM_CHANNEL
#define IOCLIENT_COM_CHANNEL 0x00
#endif

/*!
    \brief Maximum number of data bytes transfered in one frame
*/
#ifndef IOCLIENT_PAYLOAD_MAX
#define IOCLIENT_PAYLOAD_MAX 255
#endif

/*!
    \brief IOCLient error codes
*/
#define IOCLIENT_SUCCESS    0x00    /**< Operation completed successfuly */
#define IOCLIENT_ERR_SIZE   0xff    /**< Argument serialization failed  */

/*!
    \brief Exponent macros
*/
#define EXP_E_9     -9
#define EXP_E_8     -8
#define EXP_E_7     -7
#define EXP_E_6     -6
#define EXP_E_5     -5
#define EXP_E_4     -4
#define EXP_E_3     -3
#define EXP_E_2     -2
#define EXP_E_1     -1
#define EXP_E0      0
#define EXP_E1      1
#define EXP_E2      2
#define EXP_E3      3
#define EXP_E4      4
#define EXP_E5      5
#define EXP_E6      6
#define EXP_E7      7
#define EXP_E8      8
#define EXP_E9      9

/*!
    \brief Helper frequency exponent macros
*/
#define EXP_Hz      EXP_E0
#define EXP_KHz     EXP_E3
#define EXP_MHz     EXP_E6
#define EXP_GHz     EXP_E9

/*!
    \brief Helper period exponent macros
*/
#define EXP_s       EXP_E0
#define EXP_ms      EXP_E_3
#define EXP_us      EXP_E_6
#define EXP_ns      EXP_E_9

/*!
    \brief Helper voltage exponent macros
*/
#define EXP_V       EXP_E0
#define EXP_mV      EXP_E_3
#define EXP_uV      EXP_E_6
#define EXP_nV      EXP_E_9

/*!
    \brief Define floating point number
    \param integer Integer component of number
    \param exp Exponent component of number
*/
#define NUMBER(param_int, param_exp) IOClient_MakeNumberParam(param_int, param_exp)

/*!
    \brief Calculate size of function parameter
    \param param function parameter
*/
#define SIZE_FNC(param) sizeof(uint16)

/*!
    \brief Serialize function ID parameter to write buffer
*/
#define SERIALIZE_FNC(ptr, param) IOClient_SerializeInteger(&(ptr), \
    (_ioClientFunctionID = param, ((uint8 const *)&_ioClientFunctionID)), \
    sizeof(_ioClientFunctionID));

/*!
    \brief Calculate size of enum parameter
    \param param enum parameter
*/
#define SIZE_ENUM(param) sizeof(uint16)

/*!
    \brief Serialize enum parameter to write buffer
*/
#define SERIALIZE_ENUM(ptr, param) IOClient_SerializeInteger(&(ptr), \
    (_ioClientEnum = param, ((uint8 const *)&_ioClientEnum)), \
    SIZE_ENUM(_ioClientEnum));

/*!
    \brief Deserialize the enum parameter from read buffer
*/
#define DESERIALIZE_ENUM(ptr, param, enumType) { \
    IOClient_DeserializeInteger(&(ptr), (uint8*)&(_ioClientEnum), sizeof(_ioClientEnum)); \
    *(enumType*)&(param) = (enumType)_ioClientEnum; \
}

/*!
    \brief Calculate size of integer parameter
    \param param integer parameter
*/
#define SIZE_INT(param) sizeof(param)

/*!
    \brief Serialize integer parameter to write buffer
*/
#define SERIALIZE_INT(ptr, param) IOClient_SerializeInteger(&(ptr), (uint8 const *)&(param), sizeof(param));

/*!
    \brief Deserialize integer parameter from read buffer
*/
#define DESERIALIZE_INT(ptr, param) IOClient_DeserializeInteger(&(ptr), (uint8*)&(param), sizeof(param));

/*!
    \brief Calculate size of number parameter
*/
#define SIZE_NUM(param) (sizeof((param).integer) + sizeof((param).exp))

/*!
    \brief Serialize number parameter to write buffer
    \param integer Integer component of number
    \param exp Exponent component of number
*/
#define SERIALIZE_NUM(ptr, param) { \
    IOClient_SerializeInteger(&(ptr), (uint8 const *)&(param.integer), sizeof(param.integer)); \
    IOClient_SerializeInteger(&(ptr), (uint8 const *)&(param.exp), sizeof(param.exp)); \
}

/*!
    \brief Deserialize the number parameter from read buffer
*/
#define DESERIALIZE_NUM(ptr, param) { \
    IOClient_DeserializeInteger(&(ptr), (uint8*)&((param).integer), sizeof((param).integer)); \
    IOClient_DeserializeInteger(&(ptr), (uint8*)&((param).exp), sizeof((param).exp)); \
}

/*!
    \brief Calculate size of serialized array
    \param elSize Serialization size of the array element
    \param arrSize Size of the array in elements
*/
#define SIZE_ARR(elSize, arrSize) (sizeof(IOClientArrDimSizeType) + ((elSize) * (arrSize)))

/*!
    \brief Serialize array to the write buffer
    \param itPtr Write buffer iterator
    \param arrPtr Pointer to serialized array
    \param arrSize Size of the array in elements
    \param elSerialize Name of the element serialization macro (SERIALIZE_INT,
    SERIALIZE_NUM, SERIALIZE_ENUM, etc.)
*/
#define SERIALIZE_ARR(itPtr, arrPtr, arrSize, elSerialize) { \
    IOClientArrDimSizeType _serializeArrIt = (arrSize); \
    SERIALIZE_INT(itPtr, _serializeArrIt); \
    for(_serializeArrIt = 0; _serializeArrIt < (arrSize); ++_serializeArrIt) { \
        elSerialize(itPtr, (arrPtr)[_serializeArrIt]); \
    } \
}

/*!
    \brief Deserialize array from the read buffer
    \param itPtr Read buffer iterator
    \param arrPtr Pointer to deserialized array
    \param arrSizePtr Pointer to size of the array in elements. Should be inialised with maximum
    expected value. Will be replaced by actual value received. Size and array output parameters
    will not be modified if received value is greater than expected value
    \param elDeserialize Name of the element deserialization macro (DESERIALIZE_INT,
    DESERIALIZE_NUM, etc ). Warning: does not work with ENUM. Use DESERIALIZE_ARR_ENUM instead.
*/
#define DESERIALIZE_ARR(itPtr, arrPtr, arrSizePtr, elDeserialize) { \
    IOClientArrDimSizeType _deserializeArrIt; \
    DESERIALIZE_INT(itPtr, _deserializeArrIt); \
    if(*(arrSizePtr) >= _deserializeArrIt) { \
        *(arrSizePtr) = _deserializeArrIt; \
    } \
    for(_deserializeArrIt = 0; _deserializeArrIt < *(arrSizePtr); ++_deserializeArrIt) { \
        elDeserialize(itPtr, (arrPtr)[_deserializeArrIt]); \
    } \
}

/*!
    \brief Deserialize enum array from the read buffer. Specific macro because enums are not all
    of the same size
    \param itPtr Read buffer iterator
    \param arrPtr Pointer to deserialized array
    \param arrSizePtr Pointer to size of the array in elements. Should be inialised with maximum
    expected value. Will be replaced by actual value received. Size and array output parameters
    will not be modified if received value is greater than expected value
    \param enumType type of enum to deserialise
*/
#define DESERIALIZE_ARR_ENUM(itPtr, arrPtr, arrSizePtr, enumType) { \
    IOClientArrDimSizeType _deserializeArrIt; \
    DESERIALIZE_INT(itPtr, _deserializeArrIt); \
    if(*(arrSizePtr) >= _deserializeArrIt) { \
        *(arrSizePtr) = _deserializeArrIt; \
    } \
    for(_deserializeArrIt = 0; _deserializeArrIt < *(arrSizePtr); ++_deserializeArrIt) { \
        DESERIALIZE_ENUM(itPtr, (arrPtr)[_deserializeArrIt], enumType); \
    } \
}
/******************* external function declarations **************************/

/*!
    \fn uint8 IOClient_Init(uint8 channel, uint32 baudRate, uint32 oscFreq)
    \brief Initialize IOClient communication interface

    The function initializes the communication  hardware based on current
    clock configuration of the MCU.

    \param channel zero based physical communication channel number
    \param baudRate communication speed (bits per second)
    \param oscFreq oscilator frequency (Hz)

    \return 0 on success
*/
IOClientErrCodeType IOClient_Init(uint8 channel, uint32 baudRate, uint32 oscFreq);

/*!
    \fn IOClientErrCodeType IOClient_Update(void)
    \brief Update communication settings

    Purpose of this function is to maintain correct communication
    speed when changing the MCU clock configuration (PLL, etc.). Call
    this function after MCU clock configuration change to calculate
    and set a new configuration of communication interface

    \return 0 on success
*/
IOClientErrCodeType IOClient_Update(void);

/*!
    \fn IOClientErrCodeType IOClient_Call(uint8 const * const inDataPtr, uint8 inDataSize, uint8 * outDataPtr, uint8 outDataSize)
    \brief Call function on IO Server

    \param inDataPtr Pointer to param buffer
    \param inDataSize Size of param buffer
    \param outDataPtr Pointer to response buffer
    \param outDataSize Size of response buffer

    \return 0 on success
*/
IOClientErrCodeType IOClient_Call(  uint8 const * const inDataPtr,
                                    uint8 inDataSize,
                                    uint8 * outDataPtr,
                                    uint8 outDataSize);

/*!
    \fn void IOClient_SerializeInteger(uint8 ** it, uint8* dataPtr, uint8 dataSize)
    \brief Serialize integer parameter to write buffer

    \param it Pointer to pointer to serialization buffer
    \param dataPtr Pointer to integer parameter
    \param dataSize Size of integer parameter
*/
void IOClient_SerializeInteger(uint8 ** const it, uint8 const * dataPtr, uint8 dataSize);

/*!
    \fn void IOClient_DeserializeInteger(uint8 ** it, uint8* dataPtr, uint8 dataSize)
    \brief Serialize integer parameter from read buffer

    \param it Pointer to pointer to read buffer
    \param dataPtr Pointer to integer parameter
    \param dataSize Size of integer parameter
*/
void IOClient_DeserializeInteger(uint8 ** const it, uint8 * dataPtr, uint8 dataSize);

/*!
    \fn IOClientNumberType IOClient_MakeNumberParam(short param_int, uint8 param_exp)
    \brief Initialize IOClientNumber structure

    \param param_int Integer component of number
    \param param_exp Exponent component of number

    \return Floating point number structure
*/
IOClientNumberType IOClient_MakeNumberParam(IOClientIntegerType param_int, IOClientExpType param_exp);

#endif /* IOCLIENT_API_H */
