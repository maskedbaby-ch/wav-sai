/*!
	@file EUnit.c
	@version 0.0.10.0
	
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

/******************* inclusions **********************************************/

#include "EUnit.h"

/******************* internal data *******************************************/

static EUnit_RTE gEUnitRTE;	/**!< Embedded Unit Runtime Environment */

/******************* external function definitions ***************************/

/*!
	\brief Initialize the EUnit test suite and clear the result array
*/
void EUnit_Init(const EUnit_TestRegistry * const testRegistryPtr, volatile uint8* testResultBasePtr) {
#ifndef EU_EXT_STORAGE	
	uint16 totalTestCaseCnt = 0;
	volatile uint8* testResultIt = testResultBasePtr;
	volatile uint8* testResultEndPtr; 
#endif

#ifdef GET_ASSERT_DETAILS
	gEUnitRTE.datailsBasePtr = (volatile uint8*)(GET_ASSERT_DETAILS_PTR+2);
	gEUnitRTE.failuresPtr = (volatile uint16*)(GET_ASSERT_DETAILS_PTR);;
	gEUnitRTE.maxFailures = TOTAL_DETAILS_TO_STORE;
	gEUnitRTE.exceedFailures = EU_FALSE;
#endif

	gEUnitRTE.testRegistryPtr = testRegistryPtr;
	gEUnitRTE.resultBasePtr = testResultBasePtr;
#ifndef EU_EXT_STORAGE	
	/*! \note count all test cases */
	if(gEUnitRTE.testRegistryPtr && gEUnitRTE.resultBasePtr) {
		uint16 suiteIdx;
		for(suiteIdx = 0; suiteIdx < testRegistryPtr->testSuiteCnt; ++suiteIdx) {
			if(testRegistryPtr->testSuitePtrPtr && testRegistryPtr->testSuitePtrPtr[suiteIdx]) {
				totalTestCaseCnt += testRegistryPtr->testSuitePtrPtr[suiteIdx]->testCaseCnt;
			}
		}
	}
	/*! \note clear the result array (don't use memset() so we don't need ansi lib) */	
	testResultEndPtr = testResultIt + (totalTestCaseCnt / 4) + (totalTestCaseCnt % 4 ? 1 : 0);
	for(;testResultIt != testResultEndPtr; ++testResultIt) {
		*testResultIt = EU_TEST_NOT_RUN;
	}
#else
	EU_EXT_STORAGE_EUNIT_INIT(testRegistryPtr);
#endif
}

/*!
	\brief Execute all tests
*/
void EUnit_RunAllTests(void) {
	/*!  \note get pointer  to the test registry structure */
	const EUnit_TestRegistry* testRegistryPtr = gEUnitRTE.testRegistryPtr;
	if(NULL_PTR != testRegistryPtr) {
		uint16 suiteIdx;
		uint16 totalTestCaseIdx = 0;
		
		/*! \note iterate over all test suites */
		#ifndef EU_EXT_STORAGE
		for(suiteIdx = 0; suiteIdx < testRegistryPtr->testSuiteCnt; ++suiteIdx)
		#else
		while ( (suiteIdx=EU_EXT_STORAGE_GET_SUITE_ID()) < testRegistryPtr->testSuiteCnt)
		#endif
		{
		
			/*! \note iterate over all test cases */
			if(testRegistryPtr->testSuitePtrPtr && testRegistryPtr->testSuitePtrPtr[suiteIdx]) {
				uint16 caseIdx;
				
				const EUnit_TestSuite* testSuitePtr = testRegistryPtr->testSuitePtrPtr[suiteIdx];
				#ifndef EU_EXT_STORAGE
				for(caseIdx = 0; caseIdx < testSuitePtr->testCaseCnt; ++caseIdx) 
				#else
				while ( (caseIdx=EU_EXT_STORAGE_GET_CASE_ID())< testSuitePtr->testCaseCnt )
				#endif
				{
					
					/*! \note execute test case */
					if(testSuitePtr->testCasePtr && testSuitePtr->testCasePtr[caseIdx]) {
						gEUnitRTE.pass = EU_TRUE;
						#ifndef EU_DISABLE_ANSILIB_CALLS
						if(!setjmp(gEUnitRTE.jmpEnv)) 
						#endif
						{
							testSuitePtr->testCasePtr[caseIdx]();
						}
						/*! \note call custom defined macro just before storing test results */
						#ifdef EU_DO_ON_CASE_END
							EU_DO_ON_CASE_END;
						#endif
						/*! \note update test result from NotRun to Pass or Fail */
						#ifndef EU_EXT_STORAGE
						*(gEUnitRTE.resultBasePtr + (totalTestCaseIdx / 4)) |= (gEUnitRTE.pass ? EU_TEST_PASSED : EU_TEST_FAILED) << ((totalTestCaseIdx % 4) << 0x01);
						#else
						EU_EXT_STORAGE_ASSERT((gEUnitRTE.pass ? EU_TEST_PASSED : EU_TEST_FAILED));
						#endif
						/*! \note call custom defined macro after each test case */
						#ifdef EU_DO_AFTER_EACH_CASE
							EU_DO_AFTER_EACH_CASE;
						#endif
					}
					
					/*! \note advance the global test case counter and go to next test case */
					++totalTestCaseIdx;
				}
				#ifdef EU_EXT_STORAGE
				EU_EXT_STORAGE_SUITE_END();
				#endif
			}
		}
		#ifdef EU_CCOV_ENABLE
		VCAST_DUMP_COVERAGE_DATA();
		#endif
	#ifdef EU_EXT_STORAGE
	EU_EXT_STORAGE_END();
	#endif
	}
}

/*!
	\brief Independent assertion defined only if enabled
*/  
#ifdef ENABLE_INDEPENDENT_ASSERTIONS
void EUnit_IndepAssertImpl(uint16 indepAssertId, uint8 value)
{
	/*  This assertion could be called several times, so the notion is as follows
		00 -> Not Run
		01 -> Passed
		10 -> Failed
		11 -> Failed
	*/
	/* Assertion is oring in "value" which could be EU_TEST_PASSED or EU_TEST_FAILED*/
	#ifndef EU_EXT_STORAGE
	*(gEUnitRTE.resultBasePtr + (indepAssertId / 4)) |= (value) << ((indepAssertId % 4) << 0x01);
	#else
	EU_EXT_STORAGE_INDEP_ASSERT(indepAssertId,value);
	#endif
}

	/*!
		\brief Fail the test case
	*/
	void EUnit_AssertFail(void)
	{
		gEUnitRTE.pass = EU_FALSE;
	}

	/*!
		\brief Fail the test case and abort the execution
	*/
	void EUnit_FatalAssertFail(void)
	{
		gEUnitRTE.pass = EU_FALSE;
		#ifndef EU_DISABLE_ANSILIB_CALLS
			longjmp(gEUnitRTE.jmpEnv, 1);
		#endif
	}

#else
/*!
	\brief assert (fail the test case) when the value is == 0
*/
void EUnit_AssertImpl(uint8 value) {
	if(!value) {
		gEUnitRTE.pass = EU_FALSE;
	}
}

/*!
	\brief fatal assert (fail and abort the test case) when the value is == 0
*/
void EUnit_FatalAssertImpl(uint8 value) {
	if(!value) {
		gEUnitRTE.pass = EU_FALSE;
		#ifndef EU_DISABLE_ANSILIB_CALLS
			longjmp(gEUnitRTE.jmpEnv, 1);
		#endif
	}
}
#endif

#if (defined(GET_ASSERT_DETAILS))
#warning GET_ASSERT_DETAILS is defined!
void EUnit_GetAssertDetails(uint8 file_id, uint16 line)
{
	if ((*(gEUnitRTE.failuresPtr))<0xFFFF)
	{
		if ((*(gEUnitRTE.failuresPtr)) <= gEUnitRTE.maxFailures)
		{
			*(gEUnitRTE.datailsBasePtr++) = file_id;	/* store file id*/
			*(gEUnitRTE.datailsBasePtr++) = (uint8)line; /* store lower byte of line number */
			*(gEUnitRTE.datailsBasePtr++) = (uint8)(line>>8); /* store higher byte of line number */
		}
		(*(gEUnitRTE.failuresPtr))++;
	} 
	else
	{
		gEUnitRTE.exceedFailures=EU_TRUE;
	}
}
#endif



/*! @} doxygen end group definition */
