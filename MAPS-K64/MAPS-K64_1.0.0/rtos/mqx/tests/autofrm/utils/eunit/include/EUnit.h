/*!
	@file EUnit.h
	@version 0.0.2.0
	
	\brief EUnit Test Suite
	\project AUTOSAR S12X MCAL Software
	\author Petr Pomkla
	\author Freescale Semiconductor, Roznov pod Radhostem, Czech Republic

	\platform
	\peripheral
	\prerequisites
	\arversion 2.1.0
	\arrevision Rel.2.1, Rev.0017
	\swversion 2.1.0
	@lastmodusr zcz01vav01-svc
	@lastmoddate Apr-7-2011

	Freescale Semiconductor Inc.
	(c) Copyright 2007 Freescale Semiconductor Inc.
	ALL RIGHTS RESERVED.
*/
/*! \addtogroup EUnit Test Suite @{ */

#ifndef EUNIT_H
#define EUNIT_H

/******************* inclusions **********************************************/
#include "EUnit_Setting.h"

#ifdef EU_CCOV_ENABLE
extern void VCAST_DUMP_COVERAGE_DATA (void);
#endif

#if (defined(ENABLE_INDEPENDENT_ASSERTIONS) && (!(defined(PREPROCESS_ONLY_TEST))))
  #if FAKE_EUNIT 
    /* include whatever.h */
  #else
    #include "EUnit_Assertion_Ids.h"
  #endif
#endif
#include "EUnit_Types.h"
#include "EUnit_Api.h"
	
#endif /* EUNIT_H */
/*! @} doxygen end group definition */
