/*!
    @file IOClient_DMB_GetPortPinValue.c
    @version 0.0.1.0

    \brief Get IO Client GPIO pin value
    \project AUTOSAR MCAL Software
    \author Petr Novak
    \author Freescale Semiconductor, Roznov pod Radhostem, Czech Republic

    \platform
    \peripheral
    \prerequisites
    \arversion 2.1.0
    \arrevision Rel.2.1, Rev.0017
    \swversion 2.1.0
    @lastmodusr B12795
    @lastmoddate Mar-19-2009

    Freescale Semiconductor Inc.
    (c) Copyright 2008 Freescale Semiconductor Inc.
    ALL RIGHTS RESERVED.
*/
/*! \addtogroup IOClient  Remote Functions @{ */

/******************* inclusions **********************************************/
#include <mqx_inc.h>
#include "IOClient_Api.h"
#include "IOClient_FunctionID.h"
#include "IOClient_DMB_GetPortPinValue.h"

/******************* external function definitions ***************************/

/*!
    \brief Test whether digtal signal has given period and duty cycle
*/
uint8 IOClient_DMB_GetPortPinValue(IOClientTestPointType testPoint)
{
    /*! \note result error code */
    uint8 result;
    uint8 Status;
    uint8 PinValue;

    uint8 inSize = SIZE_FNC(IOClient_DMB_SetPortPinValue_ID) +
                SIZE_ENUM(testPoint);

    uint8 outSize = SIZE_INT(Status) + SIZE_INT(PinValue);

    /*! \note Define read buffer */
    uint8 *outData = (uint8*) _mem_alloc(outSize);

    /*! \note Define write buffer */
    uint8 *inData = (uint8 *) _mem_alloc(inSize);

    uint8 *it = inData;

    /*! \note Serialize remote function ID */
    SERIALIZE_FNC(it, IOClient_DMB_GetPortPinValue_ID);

    /*! \note Serialize input parameters */
    SERIALIZE_ENUM(it, testPoint);

    /*! \note Call remote function */
    result = IOClient_Call(inData, inSize, outData, outSize);

    if(result == 0 ) /* message correctly received */
    {
        it = outData;
        /*! \note Deserialize Status */
        DESERIALIZE_INT(it, Status);
        if(Status == E_IOCLIENT_OK)
        {
            /*! \note Deserialize output parameters */
            DESERIALIZE_INT(it, PinValue);
            /* return the value of the pin */
            result = PinValue;
        } else
        {   /* If status is not E_IOCLIENT_OK, return it's value */
            result = Status;
        }
    }

    _mem_free(outData);
    _mem_free(inData);

    return result;
}

/* EOF */
