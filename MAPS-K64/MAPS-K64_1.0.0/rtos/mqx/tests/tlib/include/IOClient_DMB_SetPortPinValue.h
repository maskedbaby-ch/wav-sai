/*******************************************************************************
*
* Freescale Semiconductor Inc.
* (c) Copyright 2008 Freescale Semiconductor Inc.
* ALL RIGHTS RESERVED.
*
**************************************************************************** //!
*
* @file IOClient_DMB_SetPortPinValue.h
*
* @version 0.0.1.0
*
* @lastmodusr b12795
*
* @lastmoddate Dec-8-2008
*
********************************************************************************
*
*   \brief IO Client
*   \project AUTOSAR MCAL Software
*   \author Petr Novak
*   \author Freescale Semiconductor, Roznov pod Radhostem, Czech Republic
*
*   \platform
*   \peripheral
*   \prerequisites
*   \arversion 2.1.0
*   \arrevision Rel.2.1, Rev.0017
*   \swversion 2.1.0
*   \ingroup IOCLIENT_DIO
*******************************************************************************/

#ifndef IOCLIENT_DMB_SETPORTPINVALUE_H
#define IOCLIENT_DMB_SETPORTPINVALUE_H

/******************* inclusions **********************************************/

#include "IOClient_Types.h"
#include "TL_Types.h"

/******************* external function declarations **************************/

/*!*****************************************************************************
*   \brief Set the digital output value of GPIO Pin at given port expander
*
*   This function sets the output value at given channel
*   of port expander on the DMB.
*
*   \param[in] testPoint
*   \param[in] value value to set on the pin
*
*   \return 0 on success
*
*   \include ex_IOClient_DMB_SetPortPinValue.c
*
*   \ingroup IOCLIENT_DIO
*
*******************************************************************************/
uint8 IOClient_DMB_SetPortPinValue( IOClientTestPointType testPoint,
                                    TL_Max_OutputValueType  value);

#endif /* IOCLIENT_DMB_SETPORTPINVALUE_H */

