/*!
	@file EUnit_Api.h
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
	@lastmoddate Apr-28-2011

	Freescale Semiconductor Inc.
	(c) Copyright 2007 Freescale Semiconductor Inc.
	ALL RIGHTS RESERVED.
*/
/*! \addtogroup EUnit Test Suite @{ */

#ifndef EUNIT_API_H
#define EUNIT_API_H

/******************* inclusions **********************************************/
#include "EUnit_Setting.h"
#include "EUnit_Types.h"
#ifdef ENABLE_INDEPENDENT_ASSERTIONS
	/*  Including parser generating file (only when compiling) */
	#ifndef PREPROCESS_ONLY_TEST
    #if FAKE_EUNIT 
      /* include whatever.h */
    #else
      #include "EUnit_Assertion_Ids.h"
    #endif
	#endif

#endif
#ifdef EU_EXT_STORAGE
	#include "EUnit_ExtStorage.h"
#endif

/******************* macros **************************************************/

/*!
	\brief possible test case results
*/
#define	EU_TEST_NOT_RUN		0x00	/**!< Not executed */
#define	EU_TEST_PASSED		0x01	/**!< Test case has passed */
#define	EU_TEST_FAILED		0x02	/**!< Test case has failed */
#define	EU_TEST_RESERVED	0x03	/**!< Reserved result code */

#ifdef __cplusplus
extern "C" {
#endif

#if (defined(GET_ASSERT_DETAILS))&&(!defined(PREPROCESS_ONLY_TEST))
#define EU_GET_ASSERT_DETAILS(file_id,line) EUnit_GetAssertDetails(file_id,line)
#else
#define EU_GET_ASSERT_DETAILS(file_id,line)
#endif

/* Define empty macro when EU_DO_INSIDE_ASSERTION is not specified by user */
#ifndef EU_DO_INSIDE_ASSERTION
#define EU_DO_INSIDE_ASSERTION(value,file_id,line_num,message)
#endif

#ifndef ENABLE_INDEPENDENT_ASSERTIONS
/*!
	\brief Not-fatal assertion
*/
#define EU_ASSERT(value) \
  { EUnit_AssertImpl((value)); }

/*!
	\brief Fatal assertion
*/  
#define EU_ASSERT_FATAL(value) \
  { EUnit_FatalAssertImpl((value)); } 

/*!
	\brief Independent assertion defined only if enabled
*/  
#else

	/* Utility macros to create and join strings */
	#define EU_UTIL_JOIN(a,b,c,d) EU_UTIL_JOIN_AGAIN(a,b,c,d)
	#define EU_UTIL_JOIN_AGAIN(a,b,c,d) a ## b ## c ## d
	#define EU_UTIL_QUOT(a) EU_UTIL_QUOT_AGAIN(a)
	#define EU_UTIL_QUOT_AGAIN(a) #a
	/* String describtion of an assertion */
	#define ASSERT_ID_STRING(assert_name) EU_UTIL_QUOT(EU_UTIL_JOIN(assert_name,FILE_BASENAME,_LINE_,__LINE__))

	/* Get file id (generated number by BEART parse phase) */
	#define __FILE_ID__ GET_FILE_ID(FILE_BASENAME)
	#define GET_FILE_ID(file) GET_FILE_ID_AGAIN(file)
	#define GET_FILE_ID_AGAIN(file_expanded) FILE_ID_ ## file_expanded


#ifdef SUPPRESS_ALL_ASSERTIONS
	/* Empty assertions */
	#define EU_ASSERT(value)    
	#define EU_ASSERT_FATAL(value)
	#define EU_ASSERT_REF_TC_MSG(tc_ref, value, message)
	#define EU_ASSERT_REF_TC(tc_ref, value)
#else

	/*!
		\brief Not-fatal assertion
	*/
#if FAKE_EUNIT
	#define EU_ASSERT(value) eunit_send_details(value, __LINE__, "");
#else
  #define EU_ASSERT(value) \
		{ \
			if (!(value)) \
			{ \
			EU_DO_INSIDE_ASSERTION((EU_FALSE),__FILE_ID__,__LINE__,ASSERT_ID_STRING(EU_ASSERT_AT_)); \
			EUnit_AssertFail(); \
			EU_GET_ASSERT_DETAILS(__FILE_ID__,__LINE__); \
			EU_DO_WHEN_ASSERT_FAIL; \
			} \
			else \
			{ \
			EU_DO_INSIDE_ASSERTION((EU_TRUE),__FILE_ID__,__LINE__,ASSERT_ID_STRING(EU_ASSERT_AT_)); \
			} \
		}
#endif
	/*!
		\brief Fatal assertion
	*/  
#if FAKE_EUNIT
	#define EU_ASSERT_FATAL(value) eunit_send_details(value, __LINE__, "");
#else
	#define EU_ASSERT_FATAL(value) \
		{ \
			if (!(value)) \
			{ \
			EU_DO_INSIDE_ASSERTION((EU_FALSE),__FILE_ID__,__LINE__,ASSERT_ID_STRING(EU_FATAL_ASSERT_AT_)); \
			EU_GET_ASSERT_DETAILS(__FILE_ID__,__LINE__); \
			EUnit_FatalAssertFail(); \
			EU_DO_WHEN_ASSERT_FAIL; \
			} \
			else \
			{ \
			EU_DO_INSIDE_ASSERTION((EU_TRUE),__FILE_ID__,__LINE__,ASSERT_ID_STRING(EU_FATAL_ASSERT_AT_)); \
			} \
		}
#endif
	/* assertion id magic
		composing from FILE_BASENAME - defined from Makefile
		and __LINE__ - linenumber of the assertion
		calling JOIN macro twice in order the get both parameters expanded
	*/
	#ifndef PREPROCESS_ONLY_TEST
		#define INDEP_ASSERT_ID JOIN_FILE_LINE(FILE_BASENAME, __LINE__)
		#define JOIN_FILE_LINE(file, line) JOIN_FILE_LINE_AGAIN(file, line)
		#define JOIN_FILE_LINE_AGAIN(file_expanded, line_expanded) INDEP_ASSERT_AT_ ## file_expanded ## _ ## line_expanded
	#else   
		#define INDEP_ASSERT_ID 0
	#endif
	
	/*!
		\brief Untied assertion with a message
	*/  
#if FAKE_EUNIT
	#define EU_ASSERT_REF_TC_MSG(tc_ref, value, message)     eunit_send_details(value, __LINE__, message);
#else
	#define EU_ASSERT_REF_TC_MSG(tc_ref, value, message) \
	{ \
		EU_ENTER_CRITICAL_SECTION;	\
		if ((value)) \
		{	\
			EUnit_IndepAssertImpl((INDEP_ASSERT_ID),(EU_TEST_PASSED)); \
			EU_DO_INSIDE_ASSERTION((EU_TRUE),__FILE_ID__,__LINE__,message); \
		}	\
		else \
		{	\
			EUnit_IndepAssertImpl((INDEP_ASSERT_ID),(EU_TEST_FAILED));	\
			EU_DO_INSIDE_ASSERTION((EU_FALSE),__FILE_ID__,__LINE__,message); \
			EU_DO_WHEN_ASSERT_FAIL; \
		} \
		EU_EXIT_CRITICAL_SECTION;	\
	}
#endif

	/*!
		\brief Untied assertion without a message
	*/
#if FAKE_EUNIT
	#define EU_ASSERT_REF_TC(tc_ref, value)                 eunit_send_details(value, __LINE__,"");
#else
	#define EU_ASSERT_REF_TC(tc_ref, value) \
	{ \
		EU_ENTER_CRITICAL_SECTION;	\
		if ((value)) \
		{	\
			EUnit_IndepAssertImpl((INDEP_ASSERT_ID),(EU_TEST_PASSED)); \
			EU_DO_INSIDE_ASSERTION((EU_TRUE),__FILE_ID__,__LINE__,ASSERT_ID_STRING(EU_ASSERT_REF_TC_AT_)); \
		}	\
		else \
		{	\
			EUnit_IndepAssertImpl((INDEP_ASSERT_ID),(EU_TEST_FAILED));	\
			EU_DO_INSIDE_ASSERTION((EU_FALSE),__FILE_ID__,__LINE__,ASSERT_ID_STRING(EU_ASSERT_REF_TC_AT_)); \
			EU_DO_WHEN_ASSERT_FAIL; \
		} \
		EU_EXIT_CRITICAL_SECTION;	\
	}
#endif

#endif /*  defined SUPRESS_ALL_ASSETIONS */
#endif /*  defined ENABLE_INDEPENDENT_ASSERTIONS */

#ifdef ENABLE_DUMMY_SUITES
/* 
	Empty defines when dummy suites enabled
*/
#define EU_TEST_SUITE_BEGIN(testSuiteName)
#define EU_TEST_SUITE_END(testSuiteName)
#define EU_TEST_CASE_ADD(testCaseName, testCaseDescription) \
	 { void testCaseName(void);;; }
#define EU_TEST_REGISTRY_BEGIN() 
#define EU_TEST_REGISTRY_END() 
#define EU_TEST_SUITE_ADD(testSuiteName, testSuiteDescription) 
#define EU_REGISTER_TEST_ADDRESS(base) { \
	EUnit_Init(NULL_PTR, (volatile uint8*)base); }

#else
/*!
	\brief Start of the test suite definition section
*/ 
#define EU_TEST_SUITE_BEGIN(testSuiteName) \
	static const EUnit_TestCase _##testSuiteName##List[] = { 

/*!	
	\brief End of the test suite definition section
*/	
#define EU_TEST_SUITE_END(testSuiteName) \
	}; \
	static const EUnit_TestSuite testSuiteName = { \
		sizeof(_##testSuiteName##List)/sizeof(_##testSuiteName##List[0]), _##testSuiteName##List \
	};

/*!
	\brief Add a test case to the current test suite
*/	
#define EU_TEST_CASE_ADD(testCaseName, testCaseDescription) testCaseName


/*!
	\brief Start of the test registry definition section
*/
#define EU_TEST_REGISTRY_BEGIN() \
	static const EUnit_TestSuite* _eunitTestRegistryList[] = { \

/*!
	\brief End of the test registy definition section
*/
#define EU_TEST_REGISTRY_END() \
	}; \
	static const EUnit_TestRegistry _eunitTestRegistry = { \
		sizeof(_eunitTestRegistryList)/sizeof(_eunitTestRegistryList[0]), _eunitTestRegistryList \
	};	

/*!	
	\brief Add a test suite to the test registry
*/	
#define EU_TEST_SUITE_ADD(testSuiteName, testSuiteDescription) \
	&(testSuiteName)

#endif
/*!
	\brief initialize runtime environment and run all tests
*/
#ifndef EU_EXT_STORAGE
	#define EU_RUN_ALL_TESTS(base) { \
		EUnit_Init(&_eunitTestRegistry, (volatile uint8*)base); \
		EUnit_RunAllTests(); }
#else
/* in case of external storage enable use NULL_PTR as base address */
	#define EU_RUN_ALL_TESTS(base) { \
		EUnit_Init(&_eunitTestRegistry, NULL_PTR); \
		EUnit_RunAllTests(); }
#endif

/******************* external function declarations **************************/	
	
/*!
	\fn void EUnit_Init(const EUnit_TestRegistry* const testRegistryPtr, volatile uint8* testResultBasePtr)
	\brief Initialize the EUnit test suite environment

	The function to intializes the EUnit environment and initializes
	the result array to	EU_TEST_NOT_RUN

	\param testRegistryPtr void *to collection of all test suites
	\param testResultBasePtr void *to the result array
*/

void EUnit_Init(const EUnit_TestRegistry* const testRegistryPtr, volatile uint8* testResultBasePtr);

/*!
	\fn void EUnit_RunAllTests(void)
	\brief Execute test cases in all test suites

	The function execute test cases in all test suites and stores test 
	results to test result array  specified by the EUnit_Init() function
*/

void EUnit_RunAllTests(void);

#ifndef ENABLE_INDEPENDENT_ASSERTIONS
	/*!
		\fn void EUnit_AssertImpl(uint8 value)
		\brief Fail the test case if the value parameter is 0
	
		Fails the test case if the value parameter is 0 but
		doesn't terminate the current test case
		
		\param value the test result
	*/
	void EUnit_AssertImpl(uint8 value);
	
	/*!
		\fn void EUnit_FatalAssertImpl(uint8 value)
		\brief Fail the test case if the value parameter is 0
	
		Fails the test case if the value parameter is 0 and terminates
		the current test case
		
		\param value the test result
	*/
	void EUnit_FatalAssertImpl(uint8 value);
	
	
#else
	/*!
		\fn void EUnit_IndepAssertImpl(uint16 assertionId, uint8 value)
		\brief Fail the independent assert if the value parameter is 0
		note that it sets the bits directly at address corresponding to assertionId,
		the reference to correct test case is done throug parsing/reporting perl scripts.
		
		Fails the test case if the value parameter is 0 and terminates
		the current test case
		
		\param value the test result
	*/
	void EUnit_IndepAssertImpl(uint16 assertionId, uint8 value);

	/*!
		\fn void EUnit_AssertFail()
		\brief Fail the test case
	*/
	void EUnit_AssertFail(void);
	
	/*!
		\fn void EUnit_FatalAssertFail()
		\brief Fail the test case and abort the execution
	*/
	void EUnit_FatalAssertFail(void);
	
	void EUnit_GetAssertDetails(uint8 file_id, uint16 line);

	
#endif

#ifdef __cplusplus
}
#endif

#endif /* EUNIT_API_H */
/*! @} doxygen end group definition */
