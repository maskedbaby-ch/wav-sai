/*!
	@file EUnit_Types.h
	@version 0.0.6.0
	
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
	@lastmodusr B16958
	@lastmoddate Apr-5-2011

	Freescale Semiconductor Inc.
	(c) Copyright 2007 Freescale Semiconductor Inc.
	ALL RIGHTS RESERVED.
*/
/*! \addtogroup EUnit Test Suite @{ */

#ifndef EUNIT_TYPES_H
#define EUNIT_TYPES_H

/******************* inclusions **********************************************/

#include "Std_Types.h" /* uint8, uint16 */

#ifndef EU_DISABLE_ANSILIB_CALLS
#include <setjmp.h> /* jmp_buf, setjmp(), longjmp() */
#endif

/******************* type definitions ****************************************/

/** 
	\brief Std types for EUnit to avoid collisions
 */
typedef unsigned char EUnit_bool;          /**!< EUnit bool */
#ifndef EU_TRUE
    #define EU_TRUE ((EUnit_bool) 1)
#endif
#ifndef EU_FALSE
    #define EU_FALSE ((EUnit_bool) 0)
#endif


/**
	\brief Test case function
*/
typedef void (* const EUnit_TestCase)(void);

/**
	\brief Test suite (collection of test cases)
*/
typedef struct {
	uint32						testCaseCnt;	/**!< Number of test cases */
	 EUnit_TestCase* const	testCasePtr;	/**!< Collection of test case pointers */
} EUnit_TestSuite;

/**
	\brief Test registry (collection of test suites)
*/
typedef struct {
	uint32							testSuiteCnt;		/**!< Number of test suites */
	const EUnit_TestSuite**	const	testSuitePtrPtr;	/**!< Pointer to collection of test suite pointers */
} EUnit_TestRegistry;

/**
	\brief Run-Time environment structure
*/
typedef struct {
#ifndef EU_DISABLE_ANSILIB_CALLS
	jmp_buf						jmpEnv;				/**!< setjmp(), longjmp() environment variable */
#endif
	EUnit_bool				pass;				/**!< Status of test case being executed */
	volatile uint8*				resultBasePtr;		/**!< Base pointer to the result file */
	const EUnit_TestRegistry*	testRegistryPtr; 	/**!< Pointer to collection of test suites */
#ifdef GET_ASSERT_DETAILS
	volatile uint8*				datailsBasePtr;		/**!< Base pointer to the details */
	volatile uint16*			failuresPtr;			/**!< ptr to current number of failures */
	uint32						maxFailures;		/**!< total number of configured failures */
	EUnit_bool				exceedFailures;		/**!< exceeded supported total number of failures */
#endif
} EUnit_RTE;

#endif /* EUNIT_TYPES_H */
/*! @} doxygen end group definition */
