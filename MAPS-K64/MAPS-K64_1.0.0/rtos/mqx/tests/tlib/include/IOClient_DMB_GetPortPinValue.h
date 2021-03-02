/*******************************************************************************
*
* Freescale Semiconductor Inc.
* (c) Copyright 2008 Freescale Semiconductor Inc.
* ALL RIGHTS RESERVED.
*
**************************************************************************** //!
*
* @file IOClient_DMB_GetPortPinValue.h
*
* @version 0.0.1.0
*
* @lastmodusr B12795
*
* @lastmoddate Mar-19-2009
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

#ifndef IOCLIENT_DMB_GETPORTPINVALUE_H
#define IOCLIENT_DMB_GETPORTPINVALUE_H

/******************* inclusions **********************************************/

#include "IOClient_Types.h"
#include "TL_Types.h"

/******************* external function declarations **************************/

/*!*****************************************************************************
*
*   \brief Get the digital output value of GPIO Pin at given port expander
*
*   This function gets the output value at given channel
*   of port expander on the DMB.
*
*   \param[in] testPoint testPoint identifier
*
*   \return current output value
*
*   \include ex_IOClient_DMB_GetPortPinValue.c
*
*   \ingroup IOCLIENT_DIO
*******************************************************************************/
uint8 IOClient_DMB_GetPortPinValue( IOClientTestPointType testPoint);

#endif /* IOCLIENT_DMB_GETPORTPINVALUE_H */
