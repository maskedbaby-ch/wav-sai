/*!
    @file IOClient.h
    @version 0.0.1.0

    \brief IO Client
    \project AUTOSAR S12X MCAL Software
    \author Petr Pomkla
    \author Freescale Semiconductor, Roznov pod Radhostem, Czech Republic

    \platform
    \peripheral
    \prerequisites
    \arversion 2.1.0
    \arrevision Rel.2.1, Rev.0017
    \swversion 2.1.0
    @lastmodusr B12796
    @lastmoddate Jan-25-2008

    Freescale Semiconductor Inc.
    (c) Copyright 2008 Freescale Semiconductor Inc.
    ALL RIGHTS RESERVED.
*/
/*! \addtogroup IOClient  @{ */

#ifndef IOCLIENT_H
#define IOCLIENT_H

/******************* inclusions **********************************************/

#include "IOClient_Types.h"
#include "IOClient_Api.h"

#if defined BSP_TWR_K70F120M || defined BSP_TWR_VF65GS10_A5 || defined BSP_TWR_VF65GS10_M4
#define IOCLIENT_SCI_PORT    2 /* "ttyc:" */

#elif defined BSP_TWRPXD10
#define IOCLIENT_SCI_PORT    1 /* "ttyb:" */

#elif defined BSP_TWRPXS20
#define IOCLIENT_SCI_PORT    0 /* "ttya:" */

#else
#define IOCLIENT_SCI_PORT    3 /* "ttyd:" */
#endif

#ifndef IOCLIENT_SCI_CHANNEL
#define IOCLIENT_SCI_CHANNEL    IOCLIENT_SCI_PORT
#endif

#endif /* IOCLIENT_H */
