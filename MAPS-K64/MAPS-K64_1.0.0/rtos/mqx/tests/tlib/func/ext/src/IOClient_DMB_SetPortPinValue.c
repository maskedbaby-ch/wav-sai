/*!
    @file IOClient_DMB_SetPortPinValue.c
    @version 0.0.1.0

    \brief IO Client GPIO pin value
    \project AUTOSAR MCAL Software
    \author Petr Novak
    \author Freescale Semiconductor, Roznov pod Radhostem, Czech Republic

    \platform
    \peripheral
    \prerequisites
    \arversion 2.1.0
    \arrevision Rel.2.1, Rev.0017
    \swversion 2.1.0
    @lastmodusr b12795
    @lastmoddate Dec-8-2008

    Freescale Semiconductor Inc.
    (c) Copyright 2008 Freescale Semiconductor Inc.
    ALL RIGHTS RESERVED.
*/
/*! \addtogroup IOClient  Remote Functions @{ */

/******************* inclusions **********************************************/
#include <mqx_inc.h>
#include "IOClient_Api.h"
#include "IOClient_FunctionID.h"
#include "IOClient_DMB_SetPortPinValue.h"

/******************* external function definitions ***************************/

/*!
    \brief Test whether digtal signal has given period and duty cycle
*/
uint8 IOClient_DMB_SetPortPinValue
(
    IOClientTestPointType       testPoint,
    TL_Max_OutputValueType      value
)
{

    /*! \note result error code */
    uint8 result;

    uint8 outSize = SIZE_INT(result);
    uint8 inSize = SIZE_FNC(IOClient_DMB_SetPortPinValue_ID) +
                SIZE_ENUM(testPoint) +
                SIZE_ENUM(value);

    /*! \note Define read buffer */
    uint8 *outData = (uint8*) _mem_alloc( outSize );

        /*! \note Define write buffer */
    uint8 *inData = (uint8*) _mem_alloc( inSize );

    uint8 *it = inData;

    /*! \note Serialize remote function ID */
    SERIALIZE_FNC(it, IOClient_DMB_SetPortPinValue_ID);

    /*! \note Serialize input parameters */
    SERIALIZE_ENUM(it, testPoint);
    SERIALIZE_ENUM(it, value);

    /*! \note Call remote function */
    result = IOClient_Call(inData, inSize, outData, outSize);

    if(!result)
    {
        it = outData;

        /*! \note Deserialize output parameters */
        DESERIALIZE_INT(it, result)
    }

    _mem_free(inData);
    _mem_free(outData);
    return result;
}

/* EOF */
