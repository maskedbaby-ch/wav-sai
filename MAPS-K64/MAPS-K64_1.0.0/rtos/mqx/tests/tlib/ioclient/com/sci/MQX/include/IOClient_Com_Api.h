/**************************************************************************
*
* Freescale Semiconductor Inc.
* (c) Copyright 2007-2008 Freescale Semiconductor Inc.
* ALL RIGHTS RESERVED.
*
***********************************************************************//*!
*
*    @file IOClient_Com_Api.h
*
* @author B22656
*
*    @version 0.0.1.0
*
* @date Oct-14-2010
*
* @brief IO Client SCI communication driver
*
***************************************************************************
*

    \project V&V: Measurement Automated Framework
    \author Vaclav Hamersky
    \author Freescale Semiconductor, Roznov pod Radhostem, Czech Republic
    \platform S12X
    \peripheral
    \prerequisites

*
***************************************************************************/

/*! \addtogroup IOClient SCI Communication Driver @{ */

#ifndef IOCLIENT_COM_API_H
#define IOCLIENT_COM_API_H

/******************* inclusions **********************************************/

#include "IOClient_Types.h" /* IOClientErrCodeType, uint8, uint16, ... */

/******************* macros **************************************************/

/*!
    \brief SCI error codes
*/
#define SCI_SUCCESS         0x00    /**< Operation completed successfuly */
#define SCI_ERR_CHANNEL     0x01    /**< Invalid communication channel */
#define SCI_ERR_NULL        0x02    /**< CSI base address not selected */
#define SCI_ERR_SCM         0x03  /**< Self Clock Mode activated */

/******************* external function declarations **************************/

/*!
    \fn IOClientErrCodeType IOClient_Com_Init(uint8 channel)
    \brief Initialize SCI interface

    The function initializes the SCI hardware based on current
    clock configuration of the MCU.

    \param channel zero based physical SCI channel ID
    \param baudRate communication speed (bits per second)
    \param oscFreq oscilator frequency (Hz)

    \return 0 on success
*/
IOClientErrCodeType IOClient_Com_Init(uint8 channel, uint32 baudRate, uint32 oscFreq);

/*!
    \fn IOClientErrCodeType IOClient_If_Update(void)
    \brief Update communication settings

    Purpose of this function is to maintain correct communication
    speed when changing the MCU clock configuration (PLL, etc.). Call
    this function after MCU clock configuration change to calculate
    and set new SCI configuration.

    \return 0 on success
*/
IOClientErrCodeType IOClient_Com_Update(void);

/*!
    \fn uint8 IOClient_Com_GetChar(void)
    \brief Read exactly one byte from previously initialized SCI

    The function blocks until at least one byte is available
    in the SCI receive buffer.

    \return Read byte
*/
uint8 IOClient_Com_GetChar(void);

/*!
    \fn void IOClient_Com_PutChar(uint8 data)
    \brief Write one byte to previously initialized SCI

    The function blocks until the SCI send buffer is empty.

    \param data Byte to send
*/
void IOClient_Com_PutChar(uint8 data);

#endif /* IOCLIENT_COM_API_H */
