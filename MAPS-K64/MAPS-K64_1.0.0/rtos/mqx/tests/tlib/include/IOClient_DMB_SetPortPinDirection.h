/*******************************************************************************
*
* Freescale Semiconductor Inc.
* (c) Copyright 2008 Freescale Semiconductor Inc.
* ALL RIGHTS RESERVED.
*
**************************************************************************** //!
*
* @file IOClient_DMB_SetPortPinDirection.h
*
* @version 0.0.1.0
*
* @lastmodusr b12795
*
* @lastmoddate Dec-8-2008
*
********************************************************************************
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

#ifndef IOCLIENT_DMB_SETPORTPINDIRECTION_H
#define IOCLIENT_DMB_SETPORTPINDIRECTION_H

/******************* inclusions **********************************************/

#include "IOClient_Types.h"
#include "TL_Types.h"

/******************* external function declarations **************************/

/*!*****************************************************************************
*   \brief Set the analog output voltage
*
*   This function sets the direction at given channel
*   of port expander on the DMB.
*
*   \param[in] testPoint TestPoint identifier
*   \param[in] direction Pin direction
*
*   \return 0 on success
*
*   \include IOClient_DMB_SetPortPinDirection.c
*
*   \ingroup IOCLIENT_DIO
*******************************************************************************/
uint8 IOClient_DMB_SetPortPinDirection( IOClientTestPointType testPoint,
                                        TL_Max_DirectionType  direction);

#endif /* IOCLIENT_DMB_SETPORTPINDIRECTION_H */
