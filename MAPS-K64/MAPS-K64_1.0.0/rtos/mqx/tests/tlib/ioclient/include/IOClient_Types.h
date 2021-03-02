/*!
    @file IOClient_Types.h
    @version 0.0.14.0

    \brief IO Client Types
    \project AUTOSAR S12X MCAL Software
    \author Petr Pomkla
    \author Freescale Semiconductor, Roznov pod Radhostem, Czech Republic

    \platform
    \peripheral
    \prerequisites
    \arversion 2.1.0
    \arrevision Rel.2.1, Rev.0017
    \swversion 2.1.0
    @lastmodusr B24638
    @lastmoddate Aug-6-2010

    Freescale Semiconductor Inc.
    (c) Copyright 2008 Freescale Semiconductor Inc.
    ALL RIGHTS RESERVED.
*/
/*! \addtogroup IOClient  @{ */

#ifndef IOCLIENT_TYPES_H
#define IOCLIENT_TYPES_H

/******************* inclusions **********************************************/

#include "Std_Types.h" /* uint8, uint16 */

/******************* type definitions ****************************************/

/*!
    \brief Type of remote function ID
*/
typedef uint16 IOClientFunctionIDType;

/*!
    \brief Type of client status functions
*/
typedef sint8 IOClientStatusType;

/*!
    \brief Type of the IOClient enumeration type
*/
typedef uint16 IOClientEnumType;

/*!
    \brief Type of integer component of IOClientNumberType
*/
typedef sint16 IOClientIntegerType;

/*!
    \brief Type of exponent component of IOClientNumberType
*/
typedef sint8 IOClientExpType;

/*!
    \brief Type of IOClient variable dimension size
*/
typedef uint8 IOClientShortUnsignedIntegerType;

/*!
    \brief Type of IOClient variable dimension size
*/
typedef uint16 IOClientUnsignedIntegerType;

/*!
    \brief Type of IOClient variable dimension size
*/
typedef sint32 IOClientDoubleIntegerType;

/*!
    \brief Type of IOClient result code
*/
typedef uint8 IOClientErrCodeType;

/*!
    \brief Type of IOClient array dimension size
*/
typedef uint8 IOClientArrDimSizeType;

/*!
    \brief Floating point number structure

    Define floating point number on small targets
    without FP unit
*/
typedef struct {
    IOClientIntegerType    integer;    /**< Integer component */
    IOClientExpType        exp;        /**< Exponent component */
} IOClientNumberType;

/*!
    \brief Type of IOClient digital signal edge type
*/
typedef enum {
    EDGE_ANY = 11,
    EDGE_RISING = 12,
    EDGE_FALLING = 13
} IOClientEdgeType;

/*!
    \brief Type of IOClient digital signal logic level type
*/
typedef enum {
    LOGIC_LOW = 0,
    LOGIC_HIGH = 1
} IOClientLogicType;

/*!
    \brief Type of IOClient waveform alignment
*/
typedef enum {
    WFM_ALIGN_LEFT = 0,
    WFM_ALIGN_CENTER,
    WFM_ALIGN_RIGHT
} IOClientWfmAlignType;

typedef enum {
    FR_POC_DEFAULT_CONFIG = 0,
    FR_POC_READY = 1,
    FR_POC_NORMAL_ACTIVE = 2,
    FR_POC_NORMAL_PASSIVE = 3,
    FR_POC_HALT = 4,
    FR_POC_MONITOR = 5,
    FR_POC_CONFIG = 15
} IOClientFrPocType;

typedef struct{
    IOClientFrPocType POC;
    uint8 chan_a_sleep;
    uint8 chan_b_sleep;
    uint8 pac;    /** passive to active count */
    uint8 ccf;    /** clock correction failed */
    uint8 fault;
    sint32 fault_code;
} IOClientFrState;

typedef enum{
    FR_CHAN_A = 0,
    FR_CHAN_B
} IOClientFrChanType;

#endif /* IOCLIENT_TYPES_H */
