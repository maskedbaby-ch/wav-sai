/*!
    @file IOClient_If_Api.h
    @version 0.0.3.0

    \brief IO Client Interface driver
    \project FSL MCAL ATS
    \author Petr Pomkla
    \author Freescale Semiconductor, Roznov pod Radhostem, Czech Republic

    \platform
    \peripheral
    \prerequisites
    \arversion
    \arrevision
    \swversion 1.0.0
    @lastmodusr zcz01vav01-svc
    @lastmoddate May-13-2011

    Freescale Semiconductor Inc.
    (c) Copyright 2007 Freescale Semiconductor Inc.
    ALL RIGHTS RESERVED.
*/
/*! \addtogroup IOClient SCI Interface Driver @{ */

#ifndef IOCLIENT_IF_API_H
#define IOCLIENT_IF_API_H

/******************* inclusions **********************************************/

#include "IOClient_Types.h" /* IOClientErrCodeType, uint8, uint16, ... */

/******************* macros **************************************************/

/*!
    \brief The Start and end character of the request and ack frame
*/
#define SCI_IF_STX 0x02
#define SCI_IF_ETX 0x03
#define SCI_IF_DUMMY 0xAA

/*!
    \brief SCI interface error codes
*/
#define SCI_IF_SUCCESS    0x00    /**< Operation completed successfuly */
#define SCI_IF_ERR_ETX    0x10    /**< ETX character expected */
#define SCI_IF_ERR_SUM    0x11    /**< Invalid check sum */
#define SCI_IF_ERR_SIZE   0x12    /**< Unexpected frame size */

/******************* external function declarations **************************/

/*!
    \fn IOClientErrCodeType IOClient_If_Init(uint8 channel)
    \brief Initialize SCI interface

    The function initializes the SCI hardware based on current
    clock configuration of the MCU.

    \param channel zero based physical SCI channel number
    \param baudRate communication speed (BPS)
    \param oscFreq oscilator frequency (Hz)

    \return 0 on success
*/
IOClientErrCodeType IOClient_If_Init(uint8 channel, uint32 baudRate, uint32 oscFreq);

/*!
    \fn IOClientErrCodeType IOClient_If_Update(void)
    \brief Update communication settings

    Purpose of this function is to maintain correct communication
    speed when changing the MCU clock configuration (PLL, etc.). Call
    this function after MCU clock configuration change to calculate
    and set new SCI configuration.

    \return 0 on success
*/
IOClientErrCodeType IOClient_If_Update(void);

/*!
    \fn IOClientErrCodeType IOClient_If_Call(uint8 const * const inDataPtr, uint8 inDataSize, uint8 * outDataPtr, uint8 outDataSize)
    \brief Call function on IO Server

    \param inDataPtr Pointer to param buffer
    \param inDataSize Size of param buffer
    \param outDataPtr Pointer to response buffer
    \param outDataSize Size of response buffer

    \return 0 on success
*/
IOClientErrCodeType IOClient_If_Call(uint8 const * const inDataPtr, uint8 inDataSize, uint8 * outDataPtr, uint8 outDataSize);

#endif /* IOCLIENT_IF_API_H */