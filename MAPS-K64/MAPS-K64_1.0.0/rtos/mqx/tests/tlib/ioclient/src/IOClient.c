/*!
    @file IOClient.c
    @version 0.0.4.0

    \brief IO Client
    \project AUTOSAR S12X MCAL Software
    \author Petr Pomkla
    \author Freescale Semiconductor, Roznov pod Radhostem, Czech Republic

    \platform
    \peripheral
    \prerequisites
    \arversion 2.1.0
    \arrevision Rel.2.1, Rev.0017
    \swversion 2.1.0
    @lastmodusr B12796
    @lastmoddate May-19-2008

    Freescale Semiconductor Inc.
    (c) Copyright 2008 Freescale Semiconductor Inc.
    ALL RIGHTS RESERVED.
*/
/*! \addtogroup IOClient  @{ */

/******************* inclusions **********************************************/

#include "IOClient.h"
#include "IOClient_If.h"

/******************* external data *******************************************/

/*!
    \brief Helper variable to simplify serialization of IOClientFunctionIDType
*/
IOClientFunctionIDType _ioClientFunctionID;

/*!
    \brief Helper external variable to simplify serialization of enumeration data types
*/
IOClientEnumType _ioClientEnum;

/******************* external function definitions ***************************/

/*!
    \brief Initialize IOClient communication interface
*/
IOClientErrCodeType IOClient_Init(uint8 channel, uint32 baudRate, uint32 oscFreq) {
    return IOClient_If_Init(channel, baudRate, oscFreq);
}

/*!
    \brief Update communication settings
*/
IOClientErrCodeType IOClient_Update(void) {
    return IOClient_If_Update();
}

/*!
    \brief Call an IOServer function
*/
IOClientErrCodeType IOClient_Call(    uint8 const * const inDataPtr,
                                    uint8 inDataSize,
                                    uint8 * outDataPtr,
                                    uint8 outDataSize) {
#ifdef IOCLIENT_IF_CALL_DISABLE
    /* stub mode to test API. Doesn't try to connect and returns 0 as ok */
    /* Also fills receive buffer with zeros, to fake good return codes */
    uint8 * TmpPtr;
    for(TmpPtr = outDataPtr; TmpPtr < (outDataPtr + outDataSize); TmpPtr++ ) {
        *TmpPtr = 0;
    }
    return 0;
#else
    return IOClient_If_Call(inDataPtr, inDataSize, outDataPtr, outDataSize);
#endif
}

#if (CPU_BYTE_ORDER==LOW_BYTE_FIRST)

/*!
    \brief Serialize integer parameter to write buffer
*/
void IOClient_SerializeInteger(uint8 ** const it, uint8 const * dataPtr, uint8 dataSize) {
    uint8 const *endPtr = dataPtr + dataSize - 1;
    for(;dataPtr-1 != endPtr; --endPtr) {
        *(*it)++ = *endPtr;
    }
}

/*!
    \brief Deserialize integer parameter from read buffer
*/
void IOClient_DeserializeInteger(uint8 ** const it, uint8 * dataPtr, uint8 dataSize) {
    uint8 * const beginPtr = dataPtr;
    dataPtr = dataPtr + dataSize -1;
    for(;dataPtr != beginPtr-1; --dataPtr) {
        *dataPtr = *(*it)++;
    }
    dataPtr = beginPtr + dataSize;
}

#elif (CPU_BYTE_ORDER==HIGH_BYTE_FIRST)

/*!
    \brief Serialize integer parameter to write buffer
*/
void IOClient_SerializeInteger(uint8 ** const it, uint8 const * dataPtr, uint8 dataSize) {
    uint8 const * const endPtr = dataPtr + dataSize;
    for(;dataPtr != endPtr; ++dataPtr) {
        *(*it)++ = *dataPtr;
    }
}

/*!
    \brief Deserialize integer parameter from read buffer
*/
void IOClient_DeserializeInteger(uint8 ** const it, uint8 * dataPtr, uint8 dataSize) {
    uint8 const * const endPtr = dataPtr + dataSize;
    for(;dataPtr != endPtr; ++dataPtr) {
        *dataPtr = *(*it)++;
    }
}

#endif

/*!
    \brief Make floating point number
*/
IOClientNumberType IOClient_MakeNumberParam(IOClientIntegerType param_int, IOClientExpType param_exp) {
    IOClientNumberType number;
    return number.integer = param_int, number.exp = param_exp, number;
}
