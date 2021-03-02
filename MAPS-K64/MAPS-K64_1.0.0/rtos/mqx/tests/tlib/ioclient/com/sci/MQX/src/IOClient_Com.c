/**************************************************************************
*
* Freescale Semiconductor Inc.
* (c) Copyright 2007-2008 Freescale Semiconductor Inc.
* ALL RIGHTS RESERVED.
*
************************************************************************//*!
*
*    @file IOClient_Com.c
*
* @author B22656
*
*    @version 0.0.1.0
*
* @date Oct-14-2010
*
* @brief IO Client SCI communication driver for s12sciv5
*
***************************************************************************
*

    \project V&V: Measurement Automated Framework
    \author Vaclav Hamersky
    \author Freescale Semiconductor, Roznov pod Radhostem, Czech Republic
    \platform S12X
  \addtogroup IOClient Communication Driver @{

*
******************* inclusions **********************************************/
#include <mqx.h>
#include <bsp.h>
#include <serial.h>

#include "IOClient_Com.h"
/*#include "TL_Base_Reg.h"*/

/******************* internal constants **************************************/

/******************* external data *******************************************/

/******************* external function definitions ***************************/

/*!
    \brief Initialize SCI interface
*/

static volatile MQX_FILE_PTR pUART_ptr;

IOClientErrCodeType IOClient_Com_Init(uint8 channel, uint32 baudRate, uint32 oscFreq)
{
    switch (channel)
    {
    case 0:
      pUART_ptr = fopen("ttya:", 0);
      break;

    case 1:
      pUART_ptr = fopen("ttyb:", 0);
      break;

    case 2:
      pUART_ptr = fopen("ttyc:", 0);
      break;

    case 3:
      pUART_ptr = fopen("ttyd:", 0);
      break;

    case 4:
      pUART_ptr = fopen("ttye:", 0);
      break;

    case 5:
      pUART_ptr = fopen("ttyf:", 0);
      break;

    default:
      pUART_ptr = NULL;
      break;
    }

    if (pUART_ptr == NULL) return SCI_ERR_CHANNEL;

    ioctl(pUART_ptr, IO_IOCTL_SERIAL_SET_BAUD, &baudRate);

    return SCI_SUCCESS;
}


/*!
    \brief Update communication settings
*/

IOClientErrCodeType IOClient_Com_Update(void) {

    return SCI_SUCCESS;
}

/*!
    \brief Read exactly one byte from previously initialized SCI
*/
uint8 IOClient_Com_GetChar(void) {
    uint8 data = 0x00U;
    //data = getchar();
    read(pUART_ptr, &data, 1);
    return data;
}

/*!
    \brief Write one byte to previously initialized SCI
*/
void IOClient_Com_PutChar(uint8 data) {
    /* writes a character to the terminal channel */
//    putchar(data);
    write(pUART_ptr, &data, 1);
}
