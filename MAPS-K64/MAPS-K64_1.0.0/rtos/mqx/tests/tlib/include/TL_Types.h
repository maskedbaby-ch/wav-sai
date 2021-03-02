/*!
    @file TL_Types.h
    @version 0.0.2.0

    \brief Test Library Types
    \project AUTOSAR S12X MCAL Software
    \author Petr Pomkla
    \author Freescale Semiconductor, Roznov pod Radhostem, Czech Republic

    \platform
    \peripheral
    \prerequisites
    \arversion 2.1.0
    \arrevision Rel.2.1, Rev.0017
    \swversion 2.1.0
    @lastmodusr B12795
    @lastmoddate Jan-14-2009

    Freescale Semiconductor Inc.
    (c) Copyright 2008 Freescale Semiconductor Inc.
    ALL RIGHTS RESERVED.
*/
/*! \addtogroup IOClient  @{ */

#ifndef TL_TYPES_H
#define TL_TYPES_H

/******************* inclusions **********************************************/

#include "Std_Types.h" /* uint8, uint16 */

/******************* type definitions ****************************************/

/*!
    \brief Direction of the port expander pins.
*/
typedef enum
{
    /**< The Port pins are set as input */
    TL_max_input = 0,
    /**< The Port pins are set as output */
    TL_max_output
} TL_Max_DirectionType;

/*!
    \brief Output value of the port expander pins.
*/
typedef enum
{
    /**< The Port pins are set to std_low */
    TL_max_low = 0,
    /**< The Port pins are set to std_high */
    TL_max_high
} TL_Max_OutputValueType;

/*!
    \brief Can control value (ON/OFF).
*/
typedef enum
{
    /**< The Port pins are set to std_low */
    TL_can_off = 0,
    /**< The Port pins are set to std_high */
    TL_can_on
} TL_Can_ControlValueType;

/*!
    \brief Lin control value (ON/OFF).
*/
typedef enum
{
    /**< The Port pins are set to std_low */
    TL_lin_off = 0,
    /**< The Port pins are set to std_high */
    TL_lin_on
} TL_Lin_ControlValueType;

/*!
    \brief Fr control value (ON/OFF).
*/
typedef enum
{
    /**< The Port pins are set to std_low */
    TL_fr_off = 0,
    /**< The Port pins are set to std_high */
    TL_fr_on
} TL_Fr_ControlValueType;

/*!
    \brief Signal polarity type for PWM and ICU functions.
*/
typedef enum {
    TL_POLARITY_LOW = 0,
    TL_POLARITY_HIGH = 1
} IOClientPolarityType;

#endif /* TL_TYPES_H */
