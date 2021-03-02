/**HEADER********************************************************************
*
* Copyright (c) 2008-2013 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International;
* All Rights Reserved
*
***************************************************************************
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************
*
* $FileName: test.c$
* $Version : 4.0.1.1$
* $Date    : May-29-2013$
*
* Comments:
*
*   This file contains code for the MQX light weight log module and non-light
*   weight log module.
*
* Requirements:
*   LOG001, LOG002, LOG003, LOG004, LOG005, LOG006, LOG007, LOG008, LOG009, LOG010, LOG011, LOG012, LOG013,
*   LOG014, LOG015, LOG016, LOG017, LOG018, LOG019, LOG020, LOG021, LOG022, LOG023, LOG024, LOG025, LOG026,
*   LOG027, LOG028, LOG029, LOG030, LOG031, LOG032, LOG033, LOG034, LOG035, LOG036, LOG037, LOG038, LOG039,
*   LOG040, LOG041, LOG042, LOG043, LOG044, LOG045, LOG046, LOG047, LOG048, LOG049, LOG050, LOG051, LOG052,
*   LOG053, LOG054, LOG055 ,LOG056, LOG057, LOG058, LOG059, LOG060, LOG061, LOG062
*
*   LWLOG001, LWLOG002, LWLOG003, LWLOG004, LWLOG005, LWLOG006, LWLOG007, LWLOG008, LWLOG009, LWLOG010,
*   LWLOG011, LWLOG012, LWLOG013, LWLOG014, LWLOG015, LWLOG016, LWLOG017, LWLOG018, LWLOG019, LWLOG020,
*   LWLOG021, LWLOG022, LWLOG023, LWLOG024, LWLOG025, LWLOG026, LWLOG027, LWLOG028, LWLOG029, LWLOG030,
*   LWLOG031, LWLOG032, LWLOG033, LWLOG034, LWLOG035, LWLOG036, LWLOG037, LWLOG038, LWLOG039, LWLOG040,
*   LWLOG041, LWLOG042, LWLOG043, LWLOG044, LWLOG045, LWLOG046, LWLOG047, LWLOG048, LWLOG049, LWLOG050,
*   LWLOG051, LWLOG052, LWLOG053, LWLOG054, LWLOG055, LWLOG056, LWLOG057, LWLOG058, LWLOG059, LWLOG060,
*   LWLOG061, LWLOG062, LWLOG063, LWLOG064, LWLOG065, LWLOG066, LWLOG067, LWLOG068
*
*END************************************************************************/

#include <mqx_inc.h>
#include <bsp.h>
#include <mem_prv.h>
#include <mqx_prv.h>
#include <lwlog.h>
#include <lwlog_prv.h>
#include <log.h>
#include <log_prv.h>
#include "test.h"
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"

/*------------------------------------------------------------------------
 * Test suite function prototype
 */
void tc_1_main_task(void);  /* TEST # 1 - Testing function _(lw)log_create_component()*/
void tc_2_main_task(void);  /* TEST # 2 - Testing function _(lw)log_create()*/
void tc_3_main_task(void);  /* TEST # 3 - Testing function _(lw)log_destroy()*/
void tc_4_main_task(void);  /* TEST # 4 - Testing function _(lw)log_write()*/
void tc_5_main_task(void);  /* TEST # 5 - Testing function _(lw)log_test()*/
void tc_6_main_task(void);  /* TEST # 6 - Testing function _(lw)log_read()*/
void tc_7_main_task(void);  /* TEST # 7 - Testing function _(lw)log_reset()*/
void tc_8_main_task(void);  /* TEST # 8 - Testing function _(lw)log_disable() & _(lw)log_enable()*/
void tc_9_main_task(void);  /* TEST # 9 - Testing function _lwlog_calculate_size()*/
void tc_10_main_task(void);  /* TEST # 10 - Testing function _lwlog_create_at()*/
void tc_11_main_task(void);  /* TEST # 11 - Testing calling from ISR*/

/*------------------------------------------------------------------------
 * MQX task template
 */
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
#if PSP_MQX_CPU_IS_KINETIS_L
    { MAIN_TASK,      main_task,      1500,  MAIN_PRIORITY, "main",    MQX_AUTO_START_TASK},
#else
    { MAIN_TASK,      main_task,      3000,  MAIN_PRIORITY, "main",    MQX_AUTO_START_TASK},
#endif
    {         0,              0,         0,  0,      0,                      0}
};

/******************** Begin Test cases **********************/
#if !TEST_LWLOGS
typedef struct {
   LOG_ENTRY_STRUCT  HEADER;
   _mqx_uint         DATA[LOG_PARAM_NUMBER];
} TEST_LOG_STRUCT, * TEST_LOG_STRUCT_PTR;
#endif

#if !TEST_LWLOGS
TEST_LOG_STRUCT isr_entry;
volatile bool  ISR_test;
_mqx_uint         ISR_test_result;
void             *Old_ISR_data;
void              (_CODE_PTR_ Old_ISR)(void*);
/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : log_test_ISR
* Returned Value   : void
* Comments         :
*
*  This function is used to test calling log functions from an ISR. This
* function executes when a tick interrupt occurs, and attempts to call
* the log component functions.
*
*END*-------------------------------------------------------------------------*/
void log_test_ISR(void *user_isr_ptr)
{ /* Body */
   _mqx_uint    result;
   _mqx_uint    log_num;

   /* Chain to previous notifier */
   (*Old_ISR)(Old_ISR_data);
   if (ISR_test)
   {
	   for(log_num=0;log_num<LOG_MAXIMUM_NUMBER;log_num++)
	   {
           result = _glog_disable(log_num);
           EU_ASSERT_REF_TC_MSG(tc_11_main_task, (result == MQX_OK), "Test #11 Testing: 11.02: _log_disable() succeeds and returns MQX_OK");
           result = _glog_enable(log_num);
           EU_ASSERT_REF_TC_MSG(tc_11_main_task, result == MQX_OK, "Test #11 Testing: 11.03: _log_enable() succeeds and returns MQX_OK");
           result = _glog_reset(log_num);
           EU_ASSERT_REF_TC_MSG(tc_11_main_task, result == MQX_OK, "Test #11 Testing: 11.04: _log_reset() seccedds and returns MQX_OK");
           result=_glog_write(log_num,log_num,0);
           EU_ASSERT_REF_TC_MSG(tc_11_main_task, result == MQX_OK, "Test #11: Testing 11.05: _log_write() succeeds and returns MQX_OK");
           result = _glog_read(log_num, LOG_READ_NEWEST,isr_entry);
           EU_ASSERT_REF_TC_MSG(tc_11_main_task, result == MQX_OK, "Test #11 Testing: 11.06: _log_read() succeeds and returns MQX_OK");
           result=_glog_test_entry(log_num,&isr_entry,log_num,0);
           EU_ASSERT_REF_TC_MSG(tc_11_main_task, result == TRUE, "Test #11: Testing 11.07: _glog_test_entry() succeeds and returns TRUE");
	   }
	   ISR_test = FALSE;
   } /* Endif */
} /* Endbody */
#endif

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     :
*   Test the function: _(lw)log_create_component()
* Requirements :
*   LOG005, LOG006, LOG007, LOG052, LOG060
*   LWLOG013, LWLOG014, LWLOG015, LWLOG059
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
	/* Declarations used in this case  */
    _mqx_uint result;
    MEMORY_ALLOC_INFO_STRUCT memory_alloc_info;
    _mqx_uint in_isr_value;
    /* Test _(lw)log_create_component() which will run successfully */
    result=_glog_create_component();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.01: _(lw)log_create_component() succeeds and returns MQX_OK");
    _glog_remove_component();
    /* Test _(lw)log_create_component() which will fail if memory is full */
    memory_alloc_all(&memory_alloc_info);
    result=_glog_create_component();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OUT_OF_MEMORY, "Test #1: Testing 1.02: _(lw)log_create_component() fails and returns MQX_OUT_OF_MEMORY");
    memory_free_all(&memory_alloc_info);
#if TEST_LWLOGS
    /* Test _(lw)log_create_component() which will fail if calling from ISR */
    _int_disable();
    in_isr_value = get_in_isr();
    set_in_isr(1);
    result=_glog_create_component();
    set_in_isr(in_isr_value);
    _int_enable();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR, "Test #1: Testing 1.03: _(lw)log_create_component() fails and returns MQX_CANNOT_CALL_FUNCTION_FROM_ISR");
#endif
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     :
*   Test the function: _(lw)log_create() & _(lw)log_destroy
* Requirements :
*   LOG001, LOG002, LOG003, LOG004, LOG008, LOG052, LOG054, LOG056, LOG057
*   LWLOG002, LWLOG003, LWLOG004, LWLOG005, LWLOG006, LWLOG016, LWLOG055, LWLOG068
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
	/* Declarations used in this case  */
	_mqx_uint result;
	_mqx_uint log_flag;
	_mqx_uint log_num;
	_mqx_uint valid_value;
	/* Test _(lw)log_create() when log_flag=0 */
	log_flag=0;
	/* Test _(lw)log_create() which will run successfully no matter whether component has been created*/
	for(log_num=0;log_num<LOG_MAXIMUM_NUMBER;log_num++)
	{
	    result=_glog_create(log_num,LOG_ENTRY_NUMBER,log_flag);
	    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.01: _(lw)log_create() succeeds and returns MQX_OK");
	    result=_glog_destroy(log_num);
	    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.02: _(lw)log_destroy() succeeds and returns MQX_OK");
	    _glog_remove_component();
	}
	for(log_num=0;log_num<LOG_MAXIMUM_NUMBER;log_num++)
	{
	    result=_glog_create_component();
	    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.03: _(lw)log_create_component() succeeds and returns MQX_OK");
	    result=_glog_create(log_num,LOG_ENTRY_NUMBER,log_flag);
	    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.04: _(lw)log_create() succeeds and returns MQX_OK");
	    result=_glog_destroy(log_num);
	    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.05: _(lw)log_destroy() succeeds and returns MQX_OK");
	    _glog_remove_component();
    }
	/* Test _(lw)log_create() which will fail if log number exists */
	result=_glog_create(VALID_LOG_NUMBER,LOG_ENTRY_NUMBER,log_flag);
	EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.06: _(lw)log_create() succeeds and returns MQX_OK");
	result=_glog_create(VALID_LOG_NUMBER,LOG_ENTRY_NUMBER,log_flag);
	EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == LOG_EXISTS, "Test #2: Testing 2.07: _(lw)log_create() fails and returns LOG_EXISTS");
	result=_glog_destroy(VALID_LOG_NUMBER);
	EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.08: _(lw)log_destroy() succeeds and returns MQX_OK");
	_glog_remove_component();
	/* Test _(lw)log_create() which will fail if log number is out of range */
	result=_glog_create(LOG_MAXIMUM_NUMBER,LOG_ENTRY_NUMBER,log_flag);
	EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == LOG_INVALID, "Test #2: Testing 2.09: _(lw)log_create() fails and returns LOG_INVALID");
	/* Test _(lw)log_create() which will fail if component valid is incorrect */
	result=_glog_create_component();
	EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.10: _(lw)log_create_component() succeeds and returns MQX_OK");
	valid_value=_get_valid_component();
	_set_valid_component(valid_value+1);
	result=_glog_create(VALID_LOG_NUMBER,LOG_ENTRY_NUMBER,log_flag);
	EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_INVALID_COMPONENT_BASE, "Test #2: Testing 2.11: _(lw)log_create() fails and returns MQX_INVALID_COMPONENT_BASE ");
	_set_valid_component(valid_value);
	_glog_remove_component();
	/* Test _(lw)log_create() when log_flag=LOG_OVERWRITE */
	log_flag=LOG_OVERWRITE;
	/* Test _(lw)log_create() which will run successfully no matter whether component has been created*/
	for(log_num=0;log_num<LOG_MAXIMUM_NUMBER;log_num++)
	{
		result=_glog_create(log_num,LOG_ENTRY_NUMBER,log_flag);
		EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.12: _(lw)log_create() succeeds and returns MQX_OK");
		result=_glog_destroy(log_num);
		EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.13: _(lw)log_destroy() succeeds and returns MQX_OK");
		_glog_remove_component();
	}
	for(log_num=0;log_num<LOG_MAXIMUM_NUMBER;log_num++)
	{
		result=_glog_create_component();
		EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.14: _(lw)log_create_component() succeeds and returns MQX_OK");
		result=_glog_create(log_num,LOG_ENTRY_NUMBER,log_flag);
		EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.15: _(lw)log_create() succeeds and returns MQX_OK");
		result=_glog_destroy(log_num);
		EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.16: _(lw)log_destroy() succeeds and returns MQX_OK");
		_glog_remove_component();
	}
	/* Test _(lw)log_create() which will fail if log number exists */
	result=_glog_create(VALID_LOG_NUMBER,LOG_ENTRY_NUMBER,log_flag);
	EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.17: _(lw)log_create() succeeds and returns MQX_OK");
	result=_glog_create(VALID_LOG_NUMBER,LOG_ENTRY_NUMBER,log_flag);
	EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == LOG_EXISTS, "Test #2: Testing 2.18: _(lw)log_create() fails and returns LOG_EXISTS");
	result=_glog_destroy(VALID_LOG_NUMBER);
	EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.19: _(lw)log_destroy() succeeds and returns MQX_OK");
	_glog_remove_component();
	/* Test _(lw)log_create() which will fail if log number is out of range */
	result=_glog_create(LOG_MAXIMUM_NUMBER,LOG_ENTRY_NUMBER,log_flag);
	EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == LOG_INVALID, "Test #2: Testing 2.20: _(lw)log_create() fails and returns LOG_INVALID");
	/* Test _(lw)log_create() which will fail if component valid is incorrect */
	result=_glog_create_component();
	EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.21: _(lw)log_create_component() succeeds and returns MQX_OK");
	valid_value=_get_valid_component();
	_set_valid_component(valid_value+1);
	result=_glog_create(VALID_LOG_NUMBER,LOG_ENTRY_NUMBER,log_flag);
	EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_INVALID_COMPONENT_BASE, "Test #2: Testing 2.22: _(lw)log_create() fails and returns MQX_INVALID_COMPONENT_BASE ");
	_set_valid_component(valid_value);
	_glog_remove_component();
	/* Below especially for LWLOG or LOG */
#if TEST_LWLOGS
	/*Test _lwlog_create() which will fail if maximum entry number is zero */
	result=_lwlog_create(VALID_LOG_NUMBER,0,log_flag);
	EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == LOG_INVALID_SIZE, "Test #2: Testing 2.23: _lwlog_create() fails and returns LOG_INVALID_SIZE ");
	log_flag=0;
	result=_lwlog_create(VALID_LOG_NUMBER,0,log_flag);
	EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == LOG_INVALID_SIZE, "Test #2: Testing 2.24: _lwlog_create() fails and returns LOG_INVALID_SIZE ");
	_glog_remove_component();
#else
	/* Test _log_create() which will fail if memory is full */
	MEMORY_ALLOC_INFO_STRUCT memory_alloc_info;
	memory_alloc_all(&memory_alloc_info);
	result=_log_create(VALID_LOG_NUMBER,LOG_ENTRY_NUMBER,log_flag);
	EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OUT_OF_MEMORY, "Test #2: Testing 2.25: _log_create() fails and returns MQX_OUT_OF_MEMORY ");
	log_flag=0;
	result=_log_create(VALID_LOG_NUMBER,LOG_ENTRY_NUMBER,log_flag);
	EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OUT_OF_MEMORY, "Test #2: Testing 2.26: _log_create() fails and returns MQX_OUT_OF_MEMORY ");
	memory_free_all(&memory_alloc_info);
	_glog_remove_component();
#endif
}
/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     :
*   Test the function: _(lw)log_destroy()
* Requirements :
*   LOG009, LOG010, LOG011, LOG012, LOG050, LOG051, LOG053, LOG054, LOG056
*   LWLOG017, LWLOG018, LWLOG019, LWLOG020, LWLOG054, LWLOG055, LWLOG056, LWLOG057, LWLOG068
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
	_mqx_uint result,valid_value;
	/* Test _(lw)log_destroy() which will fail if the log number is out of range */
	result=_glog_destroy(LOG_MAXIMUM_NUMBER);
	EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == LOG_INVALID, "Test #3: Testing 3.01: _(lw)log_destroy() fails and returns LOG_INVALID ");
	/* Test _(lw)log_destroy() which will fail if the log component has not been created */
	_glog_remove_component();
	result=_glog_destroy(VALID_LOG_NUMBER);
	EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_COMPONENT_DOES_NOT_EXIST, "Test #3: Testing 3.02: _(lw)log_destroy() fails and returns MQX_COMPONENT_DOES_NOT_EXIST ");
	result=_glog_create_component();
	EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3: Testing 3.03: _(lw)log_create_component() succeeds and returns MQX_OK");
	/* Test _(lw)log_destroy() which will fail if log component is invalid */
	valid_value=_get_valid_component();
	_set_valid_component(valid_value+1);
	result=_glog_destroy(VALID_LOG_NUMBER);
	EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_INVALID_COMPONENT_HANDLE, "Test #3: Testing 3.04: _(lw)log_destroy() fails and returns MQX_INVALID_COMPONENT_HANDLE ");
	_set_valid_component(valid_value);
	/* Test _(lw)log_destroy() which will fail if the log number has not been created */
	result=_glog_destroy(VALID_LOG_NUMBER);
	EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == LOG_DOES_NOT_EXIST, "Test #3: Testing 3.05: _(lw)log_destroy() fails and returns LOG_DOES_NOT_EXIST ");
	_glog_remove_component();
}
/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_4_main_task
* Comments     :
*   Test the function: _(lw)log_write()
* Requirements :
*   LOG043, LOG044, LOG045, LOG046, LOG047, LOG048, LOG049, LOG050, LOG051, LOG053, LOG054, LOG055, LOG056, LOG061
*   LWLOG047, LWLOG048, LWLOG049, LWLOG050, LWLOG051, LWLOG052, LWLOG053, LWLOG054, LWLOG055, LWLOG056, LWLOG057,
*   LWLOG066, LWLOG067, LWLOG068
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
	_mqx_uint result;
	/* for simplicity, here we just limit the entry number to be written into 2 */
	_mqx_uint log_num;
	_mqx_uint entry_num;
	_mqx_uint log_flag;
	_mqx_uint valid_value;
	/* Test _(lw)log_write() which will work successfully when log flag=0 */
	log_flag=0;
	for(log_num=0;log_num<LOG_MAXIMUM_NUMBER;log_num++)
	{
		result=_glog_create(log_num,LOG_ENTRY_NUMBER,log_flag);
		EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4: Testing 4.01: _(lw)log_create() succeeds and returns MQX_OK");
		for(entry_num=0;entry_num<LOG_ENTRY_NUMBER;entry_num++)
		{
			result=_glog_write(log_num,log_num,entry_num);
			EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4: Testing 4.02: _(lw)log_write() succeeds and returns MQX_OK");
		}
		/* Write additional entry and if overwrite is not set, _(lw)log_write() will fail and return LOG_FULL */
		result=_glog_write(log_num,log_num,entry_num);
		EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == LOG_FULL, "Test #4: Testing 4.03: _(lw)log_write() fails and returns LOG_FULL");
		result=_glog_destroy(log_num);
		EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4: Testing 4.04: _(lw)log_destroy() succeeds and returns MQX_OK");
		_glog_remove_component();
	}
	/* Test _(lw)log_write() which will work successfully when log flagLOG_OVERWRITE */
	log_flag=LOG_OVERWRITE;
	for(log_num=0;log_num<LOG_MAXIMUM_NUMBER;log_num++)
	{
		result=_glog_create(log_num,LOG_ENTRY_NUMBER,log_flag);
		EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4: Testing 4.05: _(lw)log_create() succeeds and returns MQX_OK");
		for(entry_num=0;entry_num<LOG_ENTRY_NUMBER;entry_num++)
		{
			result=_glog_write(log_num,log_num,entry_num);
			EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4: Testing 4.06: _(lw)log_write() succeeds and returns MQX_OK");
		}
		/* Test _(lw)log_write() which will write an entry over the oldest one if overwrite is set */
		for(entry_num=LOG_ENTRY_NUMBER;entry_num>0;entry_num--)
		{
			result=_glog_write(log_num,log_num,entry_num);
			EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4: Testing 4.07: _(lw)log_write() succeeds and returns MQX_OK");
		}
		result=_glog_destroy(log_num);
		EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4: Testing 4.08: _(lw)log_destroy() succeeds and returns MQX_OK");
		_glog_remove_component();
	}
	/* Test _(lw)log_write() which will fail if log number is out of valid range */
	result=_glog_create_component();
	EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4: Testing 4.09: _(lw)log_create_component() succeeds and returns MQX_OK");
	result=_glog_write(LOG_MAXIMUM_NUMBER,log_num,entry_num);
	EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == LOG_INVALID, "Test #4: Testing 4.10: _(lw)log_write() fails and returns LOG_INVALID");
	_glog_remove_component();
	/* Test _(lw)log_write() which will fail if log component does not exist */
	result=_glog_write(LOG_MAXIMUM_NUMBER-1,log_num,entry_num);
	EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_COMPONENT_DOES_NOT_EXIST, "Test #4: Testing 4.11: _(lw)log_write() fails and returns MQX_COMPONENT_DOES_NOT_EXIST");
	/* Test _(lw)log_write() which will fail if component data is not valid */
	result=_glog_create_component();
	EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4: Testing 4.12: _(lw)log_create_component() succeeds and returns MQX_OK");
	valid_value=_get_valid_component();
	_set_valid_component(valid_value+1);
	result=_glog_write(LOG_MAXIMUM_NUMBER-1,log_num,entry_num);
	EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_INVALID_COMPONENT_HANDLE, "Test #4: Testing 4.13: _(lw)log_write() fails and returns MQX_INVALID_COMPONENT_HANDLE");
	/* Test _(lw)log_write() which will fail if log number is not created */
	_set_valid_component(valid_value);
	result=_glog_write(LOG_MAXIMUM_NUMBER-1,log_num,entry_num);
	EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == LOG_DOES_NOT_EXIST, "Test #4: Testing 4.14: _(lw)log_write() fails and returns LOG_DOES_NOT_EXIST");
	_glog_remove_component();
	/* Test _(lw)log_write() which will fail if log number is disabled */
	log_flag=0;
	for(log_num=0;log_num<LOG_MAXIMUM_NUMBER;log_num++)
	{
		result=_glog_create(log_num,LOG_ENTRY_NUMBER,log_flag);
		EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4: Testing 4.15: _(lw)log_create() succeeds and returns MQX_OK");
		result=_glog_disable(log_num);
		EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4: Testing 4.16: _(lw)log_disable() succeeds and returns MQX_OK");
        result=_glog_write(log_num,log_num,entry_num);
		EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == LOG_DISABLED, "Test #4: Testing 4.17: _(lw)log_write() fails and returns LOG_DISABLED");
		result=_glog_destroy(log_num);
		EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4: Testing 4.18: _(lw)log_destroy() succeeds and returns MQX_OK");
	}
	_glog_remove_component();
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_5_main_task
* Comments     :
*   Test the function: _(lw)log_test()
* Requirements :
*   LOG005, LOG040, LOG042, LOG041, LOG054, LOG056
*   LWLOG013, LWLOG044, LWLOG045, LWLOG046, LWLOG055, LWLOG068
*END*---------------------------------------------------------------------*/
void tc_5_main_task(void)
{
	_mqx_uint result;
	_mqx_uint log_error;
	_mqx_uint valid_value;
	/* Test _(lw)log_test() which will fail due to log component data is not valid */
	result=_glog_create_component();
	EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "Test #5: Testing 5.01: _(lw)log_create_component() succeeds and returns MQX_OK");
	valid_value=_get_valid_component();
	_set_valid_component(valid_value+1);
	result=_glog_test(&log_error);
	EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_INVALID_COMPONENT_BASE, "Test #5: Testing 5.02: _(lw)log_test() fails and returns MQX_INVALID_COMPONENT_BASE");
	_set_valid_component(valid_value);
	/* Test _(lw)log_test() which will run successfully if log component data is valid */
	result=_glog_create(VALID_LOG_NUMBER,LOG_ENTRY_NUMBER,0);
	EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "Test #5: Testing 5.03: _(lw)log_create() succeeds and returns MQX_OK");
	result=_glog_test(&log_error);
	EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "Test #5: Testing 5.04: _(lw)log_test() succeeds and returns MQX_OK");
	/* Test _(lw)log_test() which will fail due to log information is not valid */
	_glog_clear(VALID_LOG_NUMBER);
	result=_glog_test(&log_error);
	EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == LOG_INVALID, "Test #5: Testing 5.05: _(lw)log_test() fails and returns LOG_INVALID");
	result=_glog_destroy(VALID_LOG_NUMBER);
	EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "Test #5: Testing 5.06: _(lw)log_destroy() succeeds and returns MQX_OK");
	_glog_remove_component();
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_6_main_task
* Comments     :
*   Test the function: _(lw)log_read()
* Requirements :
*   LOG023, LOG024, LOG025, LOG026, LOG027, LOG028, LOG029, LOG030, LOG031, LOG032, LOG033, LOG034, LOG050,
*   LOG051, LOG053, LOG054, LOG055, LOG056, LOG059
*   LWLOG031, LWLOG32, LWLOG033, LWLOG034, LWLOG35, LWLOG36, LWLOG037, LWLOG038, LWLOG054, LWLOG055, LWLOG056,
*   LWLOG057, LWLOG060, LWLOG061, LWLOG062, LWLOG063, LWLOG065, LWLOG067, LWLOG068
*END*---------------------------------------------------------------------*/
void tc_6_main_task(void)
{
	_mqx_uint result;
#if TEST_LWLOGS
	LWLOG_ENTRY_STRUCT entry;
#else
	TEST_LOG_STRUCT   entry;
#endif
	_mqx_uint read_type;
	_mqx_uint log_num;
	_mqx_uint valid_value;
	_mqx_uint log_flag;
	_mqx_uint entry_num;
	log_num=LOG_MAXIMUM_NUMBER;
	read_type=LOG_READ_OLDEST_AND_DELETE;
	log_flag=0;
	/* Test _(lw)log_read() which will fail due to log is invalid */
	result=_glog_read(log_num,read_type,entry);
	EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == LOG_INVALID, "Test #6: Testing 6.01: _(lw)log_read() fails and returns LOG_INVALID");
	log_num=VALID_LOG_NUMBER;
	void *p=NULL;
	result=_glog_create_component();
	EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6: Testing 6.02: _(lw)log_create_component() succeeds and returns MQX_OK");
	/* Test _(lw)log_read() which will fail due to log entry pointer is NULL */
	result=_glog_read_1(log_num,read_type,p);
	EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_INVALID_POINTER, "Test #6: Testing 6.03: _(lw)log_read() fails and returns MQX_INVALID_POINTER");
	_glog_remove_component();
	/* Test _(lw)log_read() which will fail due to log component does not exist */
	result=_glog_read(log_num,read_type,entry);
	EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_COMPONENT_DOES_NOT_EXIST, "Test #6: Testing 6.04: _(lw)log_read() fails and returns MQX_COMPONENT_DOES_NOT_EXIST");
	result=_glog_create_component();
	EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6: Testing 6.05: _(lw)log_create_component() succeeds and returns MQX_OK");
	valid_value=_get_valid_component();
	_set_valid_component(valid_value+1);
	/* Test _(lw)log_read() which will fail due to log component data is invalid */
	result=_glog_read(log_num,read_type,entry);
	EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_INVALID_COMPONENT_HANDLE, "Test #6: Testing 6.06: _(lw)log_read() fails and returns MQX_INVALID_COMPONENT_HANDLE");
	_set_valid_component(valid_value);
	/* Test _(lw)log_read() which will fail due to log does not exist */
	result=_glog_read(log_num,read_type,entry);
	EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == LOG_DOES_NOT_EXIST, "Test #6: Testing 6.07: _(lw)log_read() fails and returns LOG_DOES_NOT_EXIST");
	result=_glog_create(log_num,LOG_ENTRY_NUMBER,log_flag);
	EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6: Testing 6.08: _(lw)log_create() succeeds and returns MQX_OK");
	/* Test _(lw)log_read() which will fail due to log entry is not available */
	result=_glog_read(log_num,read_type,entry);
	EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == LOG_ENTRY_NOT_AVAILABLE, "Test #6: Testing 6.09: _(lw)log_read() fails and returns LOG_ENTRY_NOT_AVAILABLE");
	result=_glog_write(log_num,log_num,log_num);
	EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6: Testing 6.10: _(lw)log_write() succeeds and returns MQX_OK");
	read_type=LOG_READ_NEXT;
	/* Test _(lw)log_read() which will fail due to log read type is not available */
	result=_glog_read(log_num,0,entry);
	EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == LOG_INVALID_READ_TYPE, "Test #6: Testing 6.11: _(lw)log_read() fails and returns LOG_INVALID_READ_TYPE");
	result=_glog_destroy(log_num);
	EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6: Testing 6.12: _(lw)log_destroy() succeeds and returns MQX_OK");
	/* Prepare for read test */
	for(log_num=0;log_num<LOG_MAXIMUM_NUMBER;log_num++)
	{
		result=_glog_create(log_num,LOG_ENTRY_NUMBER,log_flag);
		EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6: Testing 6.13: _(lw)log_create() succeeds and returns MQX_OK");
		for(entry_num=0;entry_num<LOG_ENTRY_NUMBER;entry_num++)
		{
			result=_glog_write(log_num,log_num,entry_num);
			EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6: Testing 6.14: _(lw)log_write() succeeds and returns MQX_OK");
		}
	}
	/* Test _log_read() which will fail due to log entry is not available */
#if !TEST_LWLOGS
	for(log_num=0;log_num<LOG_MAXIMUM_NUMBER;log_num++)
	{
		result=_glog_read(log_num,read_type,entry);
		EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == LOG_ENTRY_NOT_AVAILABLE, "Test #6: Testing 6.15: _log_read() fails and returns LOG_ENTRY_NOT_AVAILABLE");
	}
#endif
	/* Test _(lw)log_read() whose read type is LOG_READ_NEWEST */
	read_type=LOG_READ_NEWEST;
	for(log_num=0;log_num<LOG_MAXIMUM_NUMBER;log_num++)
	{
		result=_glog_read(log_num,read_type,entry);
		EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6: Testing 6.16: _(lw)log_read() succeeds and returns MQX_OK");
		result=_glog_test_entry(log_num,&entry,log_num,LOG_ENTRY_NUMBER-1);
		EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == TRUE, "Test #6: Testing 6.17: _glog_test_entry() succeeds and returns TRUE");
	}
	/* Test _(lw)log_read() whose read type is LOG_READ_OLDEST */
	read_type=LOG_READ_OLDEST;
	for(log_num=0;log_num<LOG_MAXIMUM_NUMBER;log_num++)
	{
		result=_glog_read(log_num,read_type,entry);
		EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6: Testing 6.18: _(lw)log_read() succeeds and returns MQX_OK");
		result=_glog_test_entry(log_num,&entry,log_num,0);
		EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == TRUE, "Test #6: Testing 6.19: _glog_test_entry() succeeds and returns TRUE");
	}
	/* Test _(lw)log_read() whose read type is LOG_READ_NEXT */
	read_type=LOG_READ_NEXT;
	for(log_num=0;log_num<LOG_MAXIMUM_NUMBER;log_num++)
	{
		for(entry_num=1;entry_num<LOG_ENTRY_NUMBER;entry_num++)
		{
			result=_glog_read(log_num,read_type,entry);
			EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6: Testing 6.20: _(lw)log_read() succeeds and returns MQX_OK");
			result=_glog_test_entry(log_num,&entry,log_num,entry_num);
			EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == TRUE, "Test #6: Testing 6.21: _glog_test_entry() succeeds and returns TRUE");
		}
#if TEST_LWLOGS
		result=_glog_read(log_num,read_type,entry);
		EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == LOG_ENTRY_NOT_AVAILABLE, "Test #6: Testing 6.22: _lwlog_read() fails and returns LOG_ENTRY_NOT_AVAILABLE");
#endif
	}
	/* Test _(lw)log_read() whose read type is LOG_READ_OLDEST_AND_DELETE */
	read_type=LOG_READ_OLDEST_AND_DELETE;
	for(log_num=0;log_num<LOG_MAXIMUM_NUMBER;log_num++)
	{
		for(entry_num=0;entry_num<LOG_ENTRY_NUMBER;entry_num++)
		{
			result=_glog_read(log_num,read_type,entry);
			EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6: Testing 6.23: _(lw)log_read() succeeds and returns MQX_OK");
			result=_glog_test_entry(log_num,&entry,log_num,entry_num);
			EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == TRUE, "Test #6: Testing 6.24: _glog_test_entry() succeeds and returns TRUE");
		}
	}
	/* Destroy all the created logs */
	for(log_num=0;log_num<LOG_MAXIMUM_NUMBER;log_num++)
	{
		result=_glog_destroy(log_num);
		EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6: Testing 6.25: _glog_destroy() succeeds and returns MQX_OK");
	}
	_glog_remove_component();
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_7_main_task
* Comments     :
*   Test the function: _(lw)log_reset()
* Requirements :
*   LOG035, LOG036, LOG037, LOG038, LOG039, LOG050, LOG051, LOG053, LOG054, LOG056
*   LWLOG039, LWLOG040, LWLOG041, LWLOG042, LWLOG043, LWLOG054, LWLOG055, LWLOG056, LWLOG057, LWLOG068
*END*---------------------------------------------------------------------*/
void tc_7_main_task(void)
{
	_mqx_uint result, log_num, valid_value, entry_num, read_type, log_flag;
#if TEST_LWLOGS
	LWLOG_ENTRY_STRUCT entry;
#else
	TEST_LOG_STRUCT   entry;
#endif
	/* Test _(lw)log_reset() which will fail due to log number is out of valid range */
	result=_glog_reset(LOG_MAXIMUM_NUMBER);
	EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == LOG_INVALID, "Test #7: Testing 7.01: _(lw)log_reset() fails and returns LOG_INVALID");
	/* Test _(lw)log_reset() which will fail due to log component does not exist */
	result=_glog_reset(VALID_LOG_NUMBER);
	EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_COMPONENT_DOES_NOT_EXIST, "Test #7: Testing 7.02: _(lw)log_reset() fails and returns MQX_COMPONENT_DOES_NOT_EXIST");
	/*Test _(lw)log_reset() which will fail due to log component is not valid */
	result=_glog_create_component();
	EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "Test #7: Testing 7.03: _(lw)log_create_component() succeeds and returns MQX_OK");
	valid_value=_get_valid_component();
	_set_valid_component(valid_value+1);
	result=_glog_reset(VALID_LOG_NUMBER);
	EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_INVALID_COMPONENT_HANDLE, "Test #7: Testing 7.04: _(lw)log_reset() fails and returns MQX_INVALID_COMPONENT_HANDLE");
	_set_valid_component(valid_value);
	result=_glog_reset(VALID_LOG_NUMBER);
	EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == LOG_DOES_NOT_EXIST, "Test #7: Testing 7.05: _(lw)log_reset() fails and returns LOG_DOES_NOT_EXIST");
	/* Test _(lw)log_reset() which will succeed */
	read_type=LOG_READ_NEWEST;
	log_flag=0;
	for(log_num=0;log_num<LOG_MAXIMUM_NUMBER;log_num++)
	{
		result=_glog_create(log_num,LOG_ENTRY_NUMBER,log_flag);
		EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "Test #7: Testing 7.06: _(lw)log_create() succeeds and returns MQX_OK");
		for(entry_num=0;entry_num<LOG_ENTRY_NUMBER;entry_num++)
		{
			result=_glog_write(log_num,log_num,entry_num);
			EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "Test #7: Testing 7.07: _(lw)log_write() succeeds and returns MQX_OK");
		}
		for(entry_num=0;entry_num<LOG_ENTRY_NUMBER;entry_num++)
		{
			result=_glog_read(log_num,read_type,entry);
			EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "Test #7: Testing 7.08: _(lw)log_read() succeeds and returns MQX_OK");
		}
		result=_glog_reset(log_num);
		EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "Test #7: Testing 7.09: _(lw)log_reset() succeeds and returns MQX_OK");
		for(entry_num=0;entry_num<LOG_ENTRY_NUMBER;entry_num++)
		{
			result=_glog_read(log_num,read_type,entry);
			EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == LOG_ENTRY_NOT_AVAILABLE, "Test #7: Testing 7.10: _(lw)log_read() fails and returns LOG_ENTRY_NOT_AVAILABLE");
		}
	}
	/* Destroy all the created logs */
	for(log_num=0;log_num<LOG_MAXIMUM_NUMBER;log_num++)
	{
		result=_glog_destroy(log_num);
		EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "Test #7: Testing 7.11: _glog_destroy() succeeds and returns MQX_OK");
	}
	_glog_remove_component();
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_8_main_task
* Comments     :
*   Test the function: _(lw)log_disable() & _(lw)log_enable()
* Requirements :
*   LOG013, LOG014, LOG015, LOG016, LOG017, LOG018, LOG019, LOG020, LOG021, LOG022, LOG050, LOG051, LOG053, LOG054,
*   LOG056
*   LWLOG021, LWLOG022, LWLOG023, LWLOG024, LWLOG025, LWLOG026, LWLOG027, LWLOG028, LWLOG029, LWLOG030, LWLOG054,
*   LWLOG055, LWLOG056, LWLOG057, LWLOG068
*END*---------------------------------------------------------------------*/
void tc_8_main_task(void)
{
	_mqx_uint result, log_num, entry_num, log_flag, valid_value;
	/* Test _(lw)log_disable() & _(lw)log_enabel() which will fail due to log number is out of range */
	result=_glog_disable(LOG_MAXIMUM_NUMBER);
	EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == LOG_INVALID, "Test #8: Testing 8.01: _(lw)log_disable() fails and returns LOG_INVALID");
	result=_glog_enable(LOG_MAXIMUM_NUMBER);
	EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == LOG_INVALID, "Test #8: Testing 8.02: _(lw)log_enable() fails and returns LOG_INVALID");
	/* Test _(lw)log_disable() & _(lw)log_enabel() which will fail due to log component does not exist */
	result=_glog_disable(VALID_LOG_NUMBER);
	EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_COMPONENT_DOES_NOT_EXIST, "Test #8: Testing 8.03: _(lw)log_disable() fails and returns MQX_COMPONENT_DOES_NOT_EXIST");
	result=_glog_enable(VALID_LOG_NUMBER);
	EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_COMPONENT_DOES_NOT_EXIST, "Test #8: Testing 8.04: _(lw)log_enable() fails and returns MQX_COMPONENT_DOES_NOT_EXIST");
	/*Test _(lw)log_disable() & _(lw)log_enabel() which will fail due to log component is not valid */
	result=_glog_create_component();
	EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "Test #8: Testing 8.05: _(lw)log_create_component() succeeds and returns MQX_OK");
	valid_value=_get_valid_component();
	_set_valid_component(valid_value+1);
	result=_glog_disable(VALID_LOG_NUMBER);
	EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_INVALID_COMPONENT_HANDLE, "Test #8: Testing 8.06: _(lw)log_disable() fails and returns MQX_INVALID_COMPONENT_HANDLE");
	result=_glog_enable(VALID_LOG_NUMBER);
	EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_INVALID_COMPONENT_HANDLE, "Test #8: Testing 8.07: _(lw)log_enable() fails and returns MQX_INVALID_COMPONENT_HANDLE");
	_set_valid_component(valid_value);
	result=_glog_disable(VALID_LOG_NUMBER);
	EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == LOG_DOES_NOT_EXIST, "Test #8: Testing 8.08: _(lw)log_disable() fails and returns LOG_DOES_NOT_EXIST");
	result=_glog_enable(VALID_LOG_NUMBER);
	EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == LOG_DOES_NOT_EXIST, "Test #8: Testing 8.09: _(lw)log_enable() fails and returns LOG_DOES_NOT_EXIST");
	log_flag=0;
	for(log_num=0;log_num<LOG_MAXIMUM_NUMBER;log_num++)
	{
		result=_glog_create(log_num,LOG_ENTRY_NUMBER,log_flag);
		EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "Test #8: Testing 8.10: _(lw)log_create() succeeds and returns MQX_OK");
		for(entry_num=0;entry_num<LOG_ENTRY_NUMBER;entry_num++)
		{
			result=_glog_disable(log_num);
			EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "Test #8: Testing 8.11: _(lw)log_disable() succeeds and returns MQX_OK");
			result=_glog_write(log_num,log_num,entry_num);
			EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == LOG_DISABLED, "Test #8: Testing 8.12: _(lw)log_write() fails and returns LOG_DISABLED");
			result=_glog_enable(log_num);
			EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "Test #8: Testing 8.13: _(lw)log_enable() succeeds and returns MQX_OK");
			result=_glog_write(log_num,log_num,entry_num);
			EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "Test #8: Testing 8.14: _(lw)log_write() succeeds and returns MQX_OK");
		}
	}
	/* Destroy all the created logs */
	for(log_num=0;log_num<LOG_MAXIMUM_NUMBER;log_num++)
	{
		result=_glog_destroy(log_num);
		EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "Test #8: Testing 8.15: _glog_destroy() succeeds and returns MQX_OK");
	}
	_glog_remove_component();
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_9_main_task
* Comments     :
*   Test the function: _lwlog_calculate_size()
* Requirements :
*   LWLOG001
*END*---------------------------------------------------------------------*/
void tc_9_main_task(void)
{
#if TEST_LWLOGS
	_mem_size result;
	_mqx_uint entry=2;
	result=(_mem_size) sizeof(LWLOG_HEADER_STRUCT) + (_mem_size) (entry - 1) * (_mem_size) sizeof(LWLOG_ENTRY_STRUCT);
	EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == _lwlog_calculate_size(entry), "Test #9: Testing 9.01: _lwlog_calculate_size() calculation is correct");
#endif
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_10_main_task
* Comments     :
*   Test the function: _lwlog_create_at()
* Requirements :
*   LWLOG007, LWLOG008, LWLOG009, LWLOG010, LWLOG011, LWLOG012, LWLOG055, LWLOG068
*END*---------------------------------------------------------------------*/
void tc_10_main_task(void)
{
#if TEST_LWLOGS
	/* Declarations used in this case  */
	_mqx_uint result;
	_mqx_uint log_flag;
	_mqx_uint log_num;
	_mqx_uint valid_value;
	void     *location;
	void     *p=NULL;
	_mem_size step;
	step=_lwlog_calculate_size(LOG_ENTRY_NUMBER);
	location=_mem_alloc_system_zero(step);
	/* Test _lwlog_create_at() when log_flag=0 */
	log_flag=0;
	/* Test _lwlog_create_at() which will run successfully no matter whether component has been created*/
	for(log_num=0;log_num<LOG_MAXIMUM_NUMBER;log_num++)
	{
	    result=_lwlog_create_at(log_num,LOG_ENTRY_NUMBER,log_flag,location);
	    EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == MQX_OK, "Test #10: Testing 10.01: _lwlog_create_at() succeeds and returns MQX_OK");
	    /* Test _lwlog_create_at() which will fail due to log number exists */
	    result=_lwlog_create_at(log_num,LOG_ENTRY_NUMBER,log_flag,location);
	    EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == LOG_EXISTS, "Test #10: Testing 10.02: _lwlog_create_at() fails and returns LOG_EXISTS");
	    result=_glog_destroy(log_num);
	    EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == MQX_OK, "Test #10: Testing 10.03: _(lw)log_destroy() succeeds and returns MQX_OK");
	    _glog_remove_component();
	}
	for(log_num=0;log_num<LOG_MAXIMUM_NUMBER;log_num++)
	{
	    result=_glog_create_component();
	    EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == MQX_OK, "Test #10: Testing 10.04: _(lw)log_create_component() succeeds and returns MQX_OK");
	    result=_lwlog_create_at(log_num,LOG_ENTRY_NUMBER,log_flag,location);
	    EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == MQX_OK, "Test #10: Testing 10.05: _lwlog_create_at() succeeds and returns MQX_OK");
	    /* Test _lwlog_create_at() which will fail due to log number exists */
	    result=_lwlog_create_at(log_num,LOG_ENTRY_NUMBER,log_flag,location);
	    EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == LOG_EXISTS, "Test #10: Testing 10.06: _lwlog_create_at() fails and returns LOG_EXISTS");
	    result=_glog_destroy(log_num);
	    EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == MQX_OK, "Test #10: Testing 10.07: _(lw)log_destroy() succeeds and returns MQX_OK");
	    _glog_remove_component();
    }
	/* Test _lwlog_create_at() which will fail due to maximum number is zero */
	result=_lwlog_create_at(VALID_LOG_NUMBER,0,log_flag,location);
	EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == LOG_INVALID_SIZE, "Test #10: Testing 10.08: _lwlog_create_at() fails and returns LOG_INVALID_SIZE");
	/* Test _lwlog_create_at() which will fail due to NULL pointer */
	result=_lwlog_create_at(VALID_LOG_NUMBER,LOG_ENTRY_NUMBER,log_flag,p);
	EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == MQX_INVALID_POINTER, "Test #10: Testing 10.09: _lwlog_create_at() fails and returns MQX_INVALID_POINTER");
	/* Test _lwlog_create_at() which will fail if log number is out of valid range */
	result=_lwlog_create_at(LOG_MAXIMUM_NUMBER,LOG_ENTRY_NUMBER,log_flag,location);
	EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == LOG_INVALID, "Test #10: Testing 10.10: _lwlog_create_at() fails and returns LOG_INVALID");
	/* Test _lwlog_create_at() which will fail if component valid is incorrect */
	result=_glog_create_component();
	EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == MQX_OK, "Test #10: Testing 10.11: _(lw)log_create_component() succeeds and returns MQX_OK");
	valid_value=_get_valid_component();
	_set_valid_component(valid_value+1);
	result=_lwlog_create_at(VALID_LOG_NUMBER,LOG_ENTRY_NUMBER,log_flag,location);
	EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == MQX_INVALID_COMPONENT_BASE, "Test #10: Testing 10.12: _lwlog_create_at() fails and returns MQX_INVALID_COMPONENT_BASE ");
	_set_valid_component(valid_value);
	_glog_remove_component();
	/* Test _(lw)log_create() when log_flag=LOG_OVERWRITE */
	log_flag=LOG_OVERWRITE;
	/* Test _lwlog_create_at() which will run successfully no matter whether component has been created*/
	for(log_num=0;log_num<LOG_MAXIMUM_NUMBER;log_num++)
	{
		result=_lwlog_create_at(log_num,LOG_ENTRY_NUMBER,log_flag,location);
		EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == MQX_OK, "Test #10: Testing 10.13: _lwlog_create_at() succeeds and returns MQX_OK");
		result=_glog_destroy(log_num);
		EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == MQX_OK, "Test #10: Testing 10.14: _(lw)log_destroy() succeeds and returns MQX_OK");
		_glog_remove_component();
	}
	for(log_num=0;log_num<LOG_MAXIMUM_NUMBER;log_num++)
	{
		result=_glog_create_component();
		EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == MQX_OK, "Test #10: Testing 10.15: _(lw)log_create_component() succeeds and returns MQX_OK");
		result=_lwlog_create_at(log_num,LOG_ENTRY_NUMBER,log_flag,location);
		EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == MQX_OK, "Test #10: Testing 10.16: _lwlog_create_at() succeeds and returns MQX_OK");
		result=_glog_destroy(log_num);
		EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == MQX_OK, "Test #10: Testing 10.17: _(lw)log_destroy() succeeds and returns MQX_OK");
		_glog_remove_component();
	}
	/* Test _lwlog_create_at() which will fail due to maximum number is zero */
	result=_lwlog_create_at(VALID_LOG_NUMBER,0,log_flag,location);
	EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == LOG_INVALID_SIZE, "Test #10: Testing 10.18: _lwlog_create_at() fails and returns LOG_INVALID_SIZE");
	/* Test _lwlog_create_at() which will fail due to NULL pointer */
	result=_lwlog_create_at(VALID_LOG_NUMBER,LOG_ENTRY_NUMBER,log_flag,p);
	EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == MQX_INVALID_POINTER, "Test #10: Testing 10.19: _lwlog_create_at() fails and returns MQX_INVALID_POINTER");
	/* Test _lwlog_create_at() which will fail if log number is out of valid range */
	result=_lwlog_create_at(LOG_MAXIMUM_NUMBER,LOG_ENTRY_NUMBER,log_flag,location);
	EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == LOG_INVALID, "Test #10: Testing 10.20: _lwlog_create_at() fails and returns LOG_INVALID");
	/* Test _(lw)log_create() which will fail if component valid is incorrect */
	result=_glog_create_component();
	EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == MQX_OK, "Test #10: Testing 10.21: _(lw)log_create_component() succeeds and returns MQX_OK");
	valid_value=_get_valid_component();
	_set_valid_component(valid_value+1);
	result=_lwlog_create_at(VALID_LOG_NUMBER,LOG_ENTRY_NUMBER,log_flag,location);
	EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == MQX_INVALID_COMPONENT_BASE, "Test #10: Testing 10.22: _lwlog_create_at() fails and returns MQX_INVALID_COMPONENT_BASE ");
	_set_valid_component(valid_value);
	_glog_remove_component();
	_mem_free(location);
#endif
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_11_main_task
* Comments     :
*   Test calling from ISR
* Requirements :
*   LWLOG060
*END*---------------------------------------------------------------------*/
void tc_11_main_task(void)
{
#if !TEST_LWLOGS
	_mqx_uint   result;
    _mqx_uint   log_num;
    /* Prepare for test */
    for(log_num=0;log_num<LOG_MAXIMUM_NUMBER;log_num++)
    {
	    result = _glog_create(log_num, LOG_ENTRY_NUMBER, 0);
	    EU_ASSERT_REF_TC_MSG(tc_11_main_task, result == MQX_OK, "Test #11 Testing: 11.01: _log_create() succeeds and returns MQX_OK");
    }
	Old_ISR_data = _int_get_isr_data(BSP_TIMER_INTERRUPT_VECTOR);
	Old_ISR      = _int_get_isr(BSP_TIMER_INTERRUPT_VECTOR);
	ISR_test = TRUE;
	_int_install_isr(BSP_TIMER_INTERRUPT_VECTOR, log_test_ISR, &log_num);
	_time_delay(_time_get_resolution() * 2);
	_int_install_isr(BSP_TIMER_INTERRUPT_VECTOR, Old_ISR, Old_ISR_data);
	/* Destroy created logs */
	for(log_num=0;log_num<LOG_MAXIMUM_NUMBER;log_num++)
	{
	    result = _glog_destroy(log_num);
	    EU_ASSERT_REF_TC_MSG(tc_11_main_task, result == MQX_OK, "Test #11 Testing: 11.08: _log_destroy() succeeds and returns MQX_OK");
    }
	_glog_remove_component();
#endif
}

/*------------------------------------------------------------------------
 * Define Test Suite
 */
EU_TEST_SUITE_BEGIN(suite_glog1)
    EU_TEST_CASE_ADD(tc_1_main_task, "TEST # 1 - Testing function _(lw)log_create_component()"),
    EU_TEST_CASE_ADD(tc_2_main_task, "TEST # 2 - Testing function _(lw)log_create()"),
    EU_TEST_CASE_ADD(tc_3_main_task, "TEST # 3 - Testing function _(lw)log_destroy()"),
    EU_TEST_CASE_ADD(tc_4_main_task, "TEST # 4 - Testing function _(lw)log_write()"),
    EU_TEST_CASE_ADD(tc_5_main_task, "TEST # 5 - Testing function _(lw)log_test()"),
    EU_TEST_CASE_ADD(tc_6_main_task, "TEST # 6 - Testing function _(lw)log_read()"),
    EU_TEST_CASE_ADD(tc_7_main_task, "TEST # 7 - Testing function _(lw)log_reset()"),
    EU_TEST_CASE_ADD(tc_8_main_task, "TEST # 8 - Testing function _(lw)log_disable() & _(lw)log_enable()"),
    EU_TEST_CASE_ADD(tc_9_main_task, "TEST # 9 - Testing function _lwlog_calculate_size()"),
    EU_TEST_CASE_ADD(tc_10_main_task, "TEST # 10 - Testing function _lwlog_create_at()"),
    EU_TEST_CASE_ADD(tc_11_main_task, "TEST # 11 - Testing calling from ISR"),
EU_TEST_SUITE_END(suite_glog1)
/* Add test suites */
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_glog1, " - MQX (LW)log Test Suite")
EU_TEST_REGISTRY_END()
/*------------------------------------------------------------------------*/

/*TASK*-------------------------------------------------------------------
*
* Task Name    : main_task
* Comments     :
*   This is the main task to the MQX Kernel program, this task creates
*   all defined test tasks and lets them run continuously.
*
*END*----------------------------------------------------------------------*/
void main_task(uint32_t param)
{
	_int_install_unexpected_isr();
    eunit_mutex_init();
    EU_RUN_ALL_TESTS(eunit_tres_array);
    eunit_test_stop();
    _mqx_exit(0);
}

/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : lwlog_remove_component
* Returned Value   : bool result
* Comments         :
*
*  This function is used to remove the (lw)log component. It deallocates the (lw)log
* component data, and returns the memory to MQX.
*
*END*-------------------------------------------------------------------------*/
void _glog_remove_component(void)
{
   KERNEL_DATA_STRUCT_PTR mqx_ptr;
   mqx_ptr = _mqx_get_kernel_data();
#if TEST_LWLOGS
   mqx_ptr->KERNEL_COMPONENTS[KERNEL_LWLOG] = NULL;
   _mem_free(mqx_ptr->KERNEL_COMPONENTS[KERNEL_LWLOG]);
#else
   mqx_ptr->KERNEL_COMPONENTS[KERNEL_LOG] = NULL;
   _mem_free(mqx_ptr->KERNEL_COMPONENTS[KERNEL_LOG]);
#endif
}

/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : _get_valid_component
* Returned Value   : _mqx_uint result
* Comments         :
*
*  This function is used to get the valid in the (lw)log component.
*
*END*-------------------------------------------------------------------------*/
_mqx_uint _get_valid_component(void)
{
	KERNEL_DATA_STRUCT_PTR kernel_data_ptr;
	kernel_data_ptr=_mqx_get_kernel_data();
#if TEST_LWLOGS
	LWLOG_COMPONENT_STRUCT_PTR log_component_ptr;
	log_component_ptr=(LWLOG_COMPONENT_STRUCT_PTR)kernel_data_ptr->KERNEL_COMPONENTS[KERNEL_LWLOG];
#else
	LOG_COMPONENT_STRUCT_PTR log_component_ptr;
	log_component_ptr=(LOG_COMPONENT_STRUCT_PTR)kernel_data_ptr->KERNEL_COMPONENTS[KERNEL_LOG];
#endif
	return(log_component_ptr->VALID);
}

/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : _set_valid_component
* Returned Value   : void result
* Comments         :
*
*  This function is used to set the valid for the (lw)log component.
*
*END*-------------------------------------------------------------------------*/
void _set_valid_component(_mqx_uint valid_value)
{
	KERNEL_DATA_STRUCT_PTR kernel_data_ptr;
	kernel_data_ptr=_mqx_get_kernel_data();
#if TEST_LWLOGS
	LWLOG_COMPONENT_STRUCT_PTR log_component_ptr;
	log_component_ptr=(LWLOG_COMPONENT_STRUCT_PTR)kernel_data_ptr->KERNEL_COMPONENTS[KERNEL_LWLOG];
#else
	LOG_COMPONENT_STRUCT_PTR log_component_ptr;
	log_component_ptr=(LOG_COMPONENT_STRUCT_PTR)kernel_data_ptr->KERNEL_COMPONENTS[KERNEL_LOG];
#endif
	log_component_ptr->VALID=valid_value;
}

/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : _glog_test_entry
* Returned Value   : bool result
* Comments         :
*
*  This function is used to test entry after reading from a log.
*
*END*-------------------------------------------------------------------------*/
bool _glog_test_entry(_mqx_uint log_num, void *entry, _mqx_uint value, _mqx_uint seq)
{
#if TEST_LWLOGS
	LWLOG_ENTRY_STRUCT_PTR e_ptr;
	e_ptr=(LWLOG_ENTRY_STRUCT_PTR)entry;
	bool  pass = TRUE;
	if(e_ptr->SEQUENCE_NUMBER!=(seq+1) || e_ptr->DATA[0]!=value || e_ptr->DATA[1]!=seq)
	{
		pass=FALSE;
	}
	return(pass);
#else
	TEST_LOG_STRUCT_PTR   e_ptr;
	e_ptr=(TEST_LOG_STRUCT_PTR)entry;
	bool  pass = TRUE;
	if(e_ptr->HEADER.SEQUENCE_NUMBER!=(seq+1) || e_ptr->DATA[0]!=value || e_ptr->DATA[1]!=seq)
	{
		pass=FALSE;
	}
	return(pass);
#endif
}

/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : _glog_clear
* Returned Value   : void result
* Comments         :
*
*  This function is used to clear a log.
*
*END*-------------------------------------------------------------------------*/
void _glog_clear(_mqx_uint log_num)
{
#if TEST_LWLOGS
	KERNEL_DATA_STRUCT_PTR     kernel_data;
	LWLOG_COMPONENT_STRUCT_PTR log_component_ptr;
	LWLOG_HEADER_STRUCT_PTR    log_header_ptr;
	_GET_KERNEL_DATA(kernel_data);
	log_component_ptr = (LWLOG_COMPONENT_STRUCT_PTR) kernel_data->KERNEL_COMPONENTS[KERNEL_LWLOG];
	_int_disable();
	log_header_ptr = log_component_ptr->LOGS[log_num];
	log_header_ptr->READ_PTR=NULL;
	_int_enable();
#else
	KERNEL_DATA_STRUCT_PTR     kernel_data;
	LOG_COMPONENT_STRUCT_PTR log_component_ptr;
	LOG_HEADER_STRUCT_PTR    log_header_ptr;
	_GET_KERNEL_DATA(kernel_data);
	log_component_ptr = (LOG_COMPONENT_STRUCT_PTR) kernel_data->KERNEL_COMPONENTS[KERNEL_LOG];
	_int_disable();
	log_header_ptr = log_component_ptr->LOGS[log_num];
	log_header_ptr->LOG_END=NULL;
	_int_enable();
#endif
}
