/*!
    @file IOClient_If.c
    @version 0.0.3.0

    \brief IOClient SCI communication interface
    \project FSL MCAL ATS
    \author Petr Pomkla
    \author Freescale Semiconductor, Roznov pod Radhostem, Czech Republic

    \platform
    \peripheral
    \prerequisites
    \arversion
    \arrevision
    \swversion 1.0.0
    @lastmodusr r28107
    @lastmoddate Sep-2-2010

    Freescale Semiconductor Inc.
    (c) Copyright 2007 Freescale Semiconductor Inc.
    ALL RIGHTS RESERVED.
*/
/*! \addtogroup IOClient SCI Communication Interface @{ */

/******************* inclusions **********************************************/

#include "IOClient_If.h"
#include "IOClient_Com.h"

/******************* external function definitions ***************************/

/*!
    \brief Initialize SCI interface
*/
uint8 IOClient_If_Init(uint8 channel, uint32 baudRate, uint32 oscFreq) {
    return IOClient_Com_Init(channel, baudRate, oscFreq);
}

/*!
    \brief Update communication settings
*/
uint8 IOClient_If_Update(void) {
    return IOClient_Com_Update();
}

/*!
    \brief Build and send the request frame
    \note Request frame format: | STX(1) | SIZE(1) | PAYLOAD(0 .. 255)  | SUM(1) | ETX(1) |
*/
static void IOClient_If_Send(uint8 const * dataPtr, uint8 dataSize) {
    uint8 checkSum = dataSize + SCI_IF_STX;
    uint8 const * endPtr = dataPtr + dataSize;

    /* Send STX */
    IOClient_Com_PutChar(SCI_IF_STX);

    /* Send Data Size */
    IOClient_Com_PutChar(dataSize);

    /* Send payload */
    for(;dataPtr != endPtr; ++dataPtr) {
        checkSum += *dataPtr;
        IOClient_Com_PutChar(*dataPtr);
    }

    /* Send SUM */
    IOClient_Com_PutChar(checkSum);

    /* Send ETX */
    IOClient_Com_PutChar(SCI_IF_ETX);
}

/*!
    \brief Wait for the ack frame
    \note Response frame format: | STX(1) | SIZE(1) | PAYLOAD(0 .. 255)  | SUM(1) | ETX(1) |
*/
static uint8 IOClient_If_Receive(uint8 * dataPtr, uint8 dataSize) {
    uint8 size;
    uint8 checkSum = SCI_IF_STX;
    uint8 const * endPtr;

    /* Wait for STX */
    while(SCI_IF_STX != IOClient_Com_GetChar()) {
      ;
    }

    /* Read frame size */
    size = IOClient_Com_GetChar();

    if(size > dataSize) {
        return SCI_IF_ERR_SIZE; /* Unexpected frame size */
    }

    /* Update check sum */
    checkSum += size;

    /* Read payload */
    for(endPtr = dataPtr + size; dataPtr != endPtr; ++dataPtr) {
         *dataPtr = IOClient_Com_GetChar();
         checkSum += *dataPtr;
    }

    /* Read and verify the check sum */
    if(checkSum != IOClient_Com_GetChar()) {
        return SCI_IF_ERR_SUM;
    }

    /* Read EXT */
    return IOClient_Com_GetChar() == SCI_IF_ETX ? SCI_IF_SUCCESS : SCI_IF_ERR_ETX;
}

/*!
    \brief Call function on IO Server
*/
uint8 IOClient_If_Call(uint8 const * const inDataPtr, uint8 inDataSize, uint8 * outDataPtr, uint8 outDataSize) {
    IOClient_If_Send(inDataPtr, inDataSize);
    return IOClient_If_Receive(outDataPtr, outDataSize);
}
