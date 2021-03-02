/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
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
* $Version : 3.8.3.1$
* $Date    : Jul-23-2012$
*
* Comments:
*
*   This file contains the source functions for the lwlog management
*   component.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <mem_prv.h>
#include <mqx_prv.h>
#include <lwlog.h>
//#include <test_module.h>
#include <log.h>
#include "EUnit.h"
#include "eunit_crit_secs.h"

#define FILE_BASENAME   test
#define MAIN_TASK 10
#define LWLOG_HEADER_SIZE   (sizeof(LWLOG_ENTRY_STRUCT) / sizeof(_mqx_max_type))
#define TEST_LWLOG_ENTRIES      5
#define TEST_VALUE              3
#define BIG_LWLOG_ENTRIES    100

extern void main_task(uint32_t dummy);
void tc_1_main_task(void);// Test #1 - Test Auto-component creation
void tc_2_main_task(void);// Test #2 - Test Invalid use of lwlog before it is created
void tc_3_main_task(void);// Test #3 - Test lwlog creation & destruction
void tc_4_main_task(void);// Test #4 - Test lwlog read types
void tc_5_main_task(void);// Test #5 - Test lwlog types (OVERWRITE or not)
void tc_6_main_task(void);// Test #6 - Test lwlog reset
void tc_7_main_task(void);// Test #7 - Test lwlog enable/disable
void tc_8_main_task(void);// Test #8 - ISR Test
void tc_9_main_task(void);// Test #9 - Variable sized read & write

TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
#if PSP_MQX_CPU_IS_KINETIS_L
   { MAIN_TASK, main_task, 2000, 7, "Main",   MQX_AUTO_START_TASK },    
#else
   { MAIN_TASK, main_task, 5000, 7, "Main",   MQX_AUTO_START_TASK },
#endif
   { 0,        0,        0,    0, 0,       0 }
};

#if MQX_LWLOG_TIME_STAMP_IN_TICKS == 0
uint32_t Saved_seconds[BIG_LWLOG_ENTRIES];
uint32_t Saved_microseconds[BIG_LWLOG_ENTRIES];
#else
MQX_TICK_STRUCT Saved_ticks[BIG_LWLOG_ENTRIES];
#endif

volatile bool  ISR_test;
uint32_t           ISR_test_result;
void             *Old_ISR_data;
void              (_CODE_PTR_ Old_ISR)(void *);

/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : lwlog_test_remove_component
* Returned Value   : bool result
* Comments         :
*
*  This function is used to remove the lwlog component. It deallocates the lwlog 
* component data, and returns the memory to MQX.
*
*END*-------------------------------------------------------------------------*/
void lwlog_test_remove_component(void)
{
   KERNEL_DATA_STRUCT_PTR mqx_ptr;
   mqx_ptr = _mqx_get_kernel_data();
   _mem_free(mqx_ptr->KERNEL_COMPONENTS[KERNEL_LWLOG]);
   mqx_ptr->KERNEL_COMPONENTS[KERNEL_LWLOG] = NULL;
}

/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : lwlog_test_fn
* Returned Value   : bool result
* Comments         :
*
*  This function is used to compare the result of a function call with the
* desired result, and print an error if they don't match.
*
*  This function also calls  the _lwlog_test function to ensure that the 
* name component is not corrupt.
*
*  This function returns FALSE if an error was detected, otherwise it returns 
* the previous result.
*
*END*-------------------------------------------------------------------------*/

bool lwlog_test_fn
(
   bool     overall_result,
   _mqx_uint   test,
   _mqx_uint   sub_test,
   _mqx_uint   result,
   _mqx_uint   desired_result
)
{
   _mqx_uint p1;

   if (result != desired_result) {
      overall_result = FALSE;
   } /* Endif */
   if (_lwlog_test(&p1) != MQX_OK) {
      overall_result = FALSE;
   } /* Endif */

   return(overall_result);      
} /* Endbody */

/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : lwlog_test_entry
* Returned Value   : bool result
* Comments         :
*
*  This function is used to compare a lwlog entry with the expected lwlog entry. 
*
*END*-------------------------------------------------------------------------*/

bool lwlog_test_entry
(
   _mqx_uint               lwlog,
   LWLOG_ENTRY_STRUCT_PTR  e_ptr,
   _mqx_max_type           value,
   _mqx_uint               sequence
)
{ /* Body */
    bool  pass = TRUE;
#if MQX_LWLOG_TIME_STAMP_IN_TICKS == 0
    uint32_t  microseconds;
#else
    _mqx_int result;
#endif

    if (e_ptr->SEQUENCE_NUMBER != (sequence + 1)) {
        pass = FALSE;
    } /* Endif */

#if MQX_LWLOG_TIME_STAMP_IN_TICKS == 0
    microseconds = (e_ptr->MILLISECONDS * 1000) + 
        e_ptr->MICROSECONDS;
    if (e_ptr->SECONDS < Saved_seconds[lwlog]) {
        pass = FALSE;
    } else if (e_ptr->SECONDS == Saved_seconds[lwlog]) {
        if (microseconds < Saved_microseconds[lwlog]) {
            pass = FALSE;
        } /* Endif */
    } /* Endif */
#else
    result = PSP_CMP_TICKS(&e_ptr->TIMESTAMP, &Saved_ticks[lwlog]);
    if (result < 0) {
        pass = FALSE;
    } /* Endif */
#endif

    if (e_ptr->DATA[0] != (_mqx_max_type)lwlog) {
        pass = FALSE;
    } /* Endif */
   
    if (e_ptr->DATA[1] != value) {
        pass = FALSE;
    } /* Endif */
   
#if MQX_LWLOG_TIME_STAMP_IN_TICKS == 0
    Saved_microseconds[lwlog] = microseconds;
    Saved_seconds[lwlog] = e_ptr->SECONDS;
#else
    Saved_ticks[lwlog] = e_ptr->TIMESTAMP;
#endif
   
    return pass;
} /* Endbody */

/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : lwlog_test_entry2
* Returned Value   : bool result
* Comments         :
*
*  This function is used to compare a lwlog entry with the expected lwlog entry. 
*
*END*-------------------------------------------------------------------------*/
bool lwlog_test_entry2
(
   LWLOG_ENTRY_STRUCT_PTR  e_ptr,
   _mqx_uint               index,
   _mqx_uint               read_size,
   _mqx_uint               write_size
)
{ /* Body */
    bool  pass = TRUE;
#if MQX_LWLOG_TIME_STAMP_IN_TICKS == 0
    uint32_t  microseconds;
#else
    _mqx_int result;
#endif
    _mqx_uint  i, count = 0;
   
#if MQX_LWLOG_TIME_STAMP_IN_TICKS == 0
    microseconds = (e_ptr->MILLISECONDS * 1000) + 
        e_ptr->MICROSECONDS;

    if (e_ptr->SECONDS < Saved_seconds[0]) {
        pass = FALSE;
    } else if (e_ptr->SECONDS == Saved_seconds[0]) {
        if (microseconds < Saved_microseconds[0]) {
         pass = FALSE;
        } /* Endif */
    } /* Endif */
#else
    result = PSP_CMP_TICKS(&e_ptr->TIMESTAMP, &Saved_ticks[0]);
    if (result < 0) {
        pass = FALSE;
    } /* Endif */
#endif   

    if (write_size < read_size) {
        count = write_size;
    } else {
        count = read_size;
    } /* Endif */
   
    for (i = 0; i < count; i++) {
        if (e_ptr->DATA[i] != (_mqx_max_type)((index * 10) + i + 1)) {
            pass = FALSE;
        } /* Endif */
    } /* Endfor */
                           
#if MQX_LWLOG_TIME_STAMP_IN_TICKS == 0
    Saved_microseconds[0] = microseconds;
    Saved_seconds[0] = e_ptr->SECONDS;
#else
    Saved_ticks[0] = e_ptr->TIMESTAMP;
#endif
   
   return pass;
} /* Endbody */

/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : lwlog_test_print_result
* Returned Value   : void 
* Comments         :
*
*  This function prints the test name and number, and the result of the test.
*
*END*-------------------------------------------------------------------------*/

void lwlog_test_print_result
(
   _mqx_uint  test,
   _mqx_uint  result,
   char   *test_lwlog
)
{
    // printf("\nTest #%d, %s %s", test, test_lwlog, (result ? "Passed" : "Failed"));
}

/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : lwlog_test_ISR
* Returned Value   : void 
* Comments         :
*
*  This function is used to test calling lwlog functions from an ISR.  This 
* function executes when a tick interrupt occurs, and attempts to call 
* the lwlog component functions.
*
*END*-------------------------------------------------------------------------*/

LWLOG_ENTRY_STRUCT isr_entry;

void lwlog_test_ISR(void *user_isr_ptr)
{ /* Body */

    bool           passed;
   
    if (ISR_test) {
        passed = TRUE;
        passed = lwlog_test_fn(passed, 8, 2, _lwlog_disable(1), MQX_OK);
        EU_ASSERT_REF_TC_MSG(tc_8_main_task, (passed == TRUE), "Test #8 Testing: 8.2: _lwlog_disable operation")

        passed = lwlog_test_fn(passed, 8, 3, _lwlog_enable(1), MQX_OK);
        EU_ASSERT_REF_TC_MSG(tc_8_main_task, (passed == TRUE), "Test #8 Testing: 8.3: _lwlog_enable operation")

        passed = lwlog_test_fn(passed, 8, 4, _lwlog_reset(1), MQX_OK);
        EU_ASSERT_REF_TC_MSG(tc_8_main_task, (passed == TRUE), "Test #8 Testing: 8.4: _lwlog_reset operation")

        /* PLEASE NOTE: if the lwlog number is changed in these tests, then the third arguement 
         in the following call to _lwlog_write MUST BE THE SAME AS THE LWLOG NUMBER! This is 
         checked in another test and 8.7 will fail if this is not correct */
        passed = lwlog_test_fn(passed, 8, 5, _lwlog_write(1, 1, TEST_VALUE, 0, 0, 0, 0, 0), MQX_OK);
        EU_ASSERT_REF_TC_MSG(tc_8_main_task, (passed == TRUE), "Test #8 Testing: 8.5: _lwlog_write operation")

        passed = lwlog_test_fn(passed, 8, 6, _lwlog_read(1, LOG_READ_NEWEST, &isr_entry), MQX_OK);
        EU_ASSERT_REF_TC_MSG(tc_8_main_task, (passed == TRUE), "Test #8 Testing: 8.6: _lwlog_read operation")

        passed = lwlog_test_fn(passed, 8, 7, lwlog_test_entry(1, &isr_entry, TEST_VALUE, 0), TRUE);
        EU_ASSERT_REF_TC_MSG(tc_8_main_task, (passed == TRUE), "Test #8 Testing: 8.7: lwlog_test_entry operation")

        ISR_test_result = passed;
        ISR_test = FALSE;
    } /* Endif */

    /* Chain to previous notifier */
    (*Old_ISR)(Old_ISR_data);
} /* Endbody */


//----------------- Begin Testcases --------------------------------------
bool     all_passed;

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_1_main_task
// Comments     : TEST #1: Test Auto-component creation
//
//END---------------------------------------------------------------------
void tc_1_main_task(void)
{
    bool             passed;
    LWLOG_ENTRY_STRUCT  entry;

    passed = TRUE;
    passed = lwlog_test_fn(passed, 1, 1, _lwlog_disable(0), MQX_COMPONENT_DOES_NOT_EXIST);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (passed == TRUE), "Test #1 Testing: 1.1: _lwlog_disable should have returned MQX_COMPONENT_DOES_NOT_EXIST")

    passed = lwlog_test_fn(passed, 1, 2, _lwlog_enable(0), MQX_COMPONENT_DOES_NOT_EXIST);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (passed == TRUE), "Test #1 Testing: 1.2: _lwlog_enable should have returned MQX_COMPONENT_DOES_NOT_EXIST")

    passed = lwlog_test_fn(passed, 1, 3, _lwlog_destroy(0), MQX_COMPONENT_DOES_NOT_EXIST);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (passed == TRUE), "Test #1 Testing: 1.3: _lwlog_destroy should have returned MQX_COMPONENT_DOES_NOT_EXIST")

    passed = lwlog_test_fn(passed, 1, 4, _lwlog_write(0, 0, 0, 0, 0, 0, 0, 0), MQX_COMPONENT_DOES_NOT_EXIST);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (passed == TRUE), "Test #1 Testing: 1.4: _lwlog_write should have returned MQX_COMPONENT_DOES_NOT_EXIST")

    passed = lwlog_test_fn(passed, 1, 5, _lwlog_reset(0), MQX_COMPONENT_DOES_NOT_EXIST);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (passed == TRUE), "Test #1 Testing: 1.5: _lwlog_reset should have returned MQX_COMPONENT_DOES_NOT_EXIST")

    passed = lwlog_test_fn(passed, 1, 6, _lwlog_read(0, 0, &entry), MQX_COMPONENT_DOES_NOT_EXIST);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (passed == TRUE), "Test #1 Testing: 1.6: _lwlog_read should have returned MQX_COMPONENT_DOES_NOT_EXIST")

    passed = lwlog_test_fn(passed, 1, 7, _lwlog_create(0, TEST_LWLOG_ENTRIES, 0), MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (passed == TRUE), "Test #1 Testing: 1.7: _lwlog_create operation")

    passed = lwlog_test_fn(passed, 1, 8, _lwlog_create(0, TEST_LWLOG_ENTRIES, 0), LOG_EXISTS);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (passed == TRUE), "Test #1 Testing: 1.8: _lwlog_create should have returned LOG_EXISTS")

    passed = lwlog_test_fn(passed, 1, 9, _lwlog_destroy(0), MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (passed == TRUE), "Test #1 Testing: 1.9: _lwlog_destroy operation")

    lwlog_test_remove_component();

    passed = lwlog_test_fn(passed, 1, 10, _lwlog_disable(0), MQX_COMPONENT_DOES_NOT_EXIST);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (passed == TRUE), "Test #1 Testing: 1.10: _lwlog_disable should have returned MQX_COMPONENT_DOES_NOT_EXIST")

    passed = lwlog_test_fn(passed, 1, 11, _lwlog_create_component(), MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (passed == TRUE), "Test #1 Testing: 1.11: _lwlog_create_component operation")

    passed = lwlog_test_fn(passed, 1, 12, _lwlog_create_component(), MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (passed == TRUE), "Test #1 Testing: 1.12: _lwlog_create_component operation")

    lwlog_test_print_result(1, passed, "component creation");
    all_passed = all_passed && passed;
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_2_main_task
// Comments     : TEST #2: Test Invalid use of lwlog before it is created
//
//END---------------------------------------------------------------------

void tc_2_main_task(void)
{
    bool             passed;
    LWLOG_ENTRY_STRUCT  entry;

    passed = TRUE;

    passed = lwlog_test_fn(passed, 2, 1, _lwlog_disable(1), LOG_DOES_NOT_EXIST);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (passed == TRUE), "Test #2 Testing: 2.1: _lwlog_disable should have returned LOG_DOES_NOT_EXIST")

    passed = lwlog_test_fn(passed, 2, 2, _lwlog_enable(1), LOG_DOES_NOT_EXIST);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (passed == TRUE), "Test #2 Testing: 2.2: _lwlog_enable should have returned LOG_DOES_NOT_EXIST")

    passed = lwlog_test_fn(passed, 2, 3, _lwlog_destroy(1), LOG_DOES_NOT_EXIST);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (passed == TRUE), "Test #2 Testing: 2.3: _lwlog_destroy should have returned LOG_DOES_NOT_EXIST")

    passed = lwlog_test_fn(passed, 2, 4, _lwlog_write(1, 0, 0, 0, 0, 0, 0, 0), LOG_DOES_NOT_EXIST);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (passed == TRUE), "Test #2 Testing: 2.4: _lwlog_write should have returned LOG_DOES_NOT_EXIST")

    passed = lwlog_test_fn(passed, 2, 5, _lwlog_reset(1), LOG_DOES_NOT_EXIST);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (passed == TRUE), "Test #2 Testing: 2.5: _lwlog_reset should have returned LOG_DOES_NOT_EXIST")

    passed = lwlog_test_fn(passed, 2, 6, _lwlog_read(1, 0, &entry), LOG_DOES_NOT_EXIST);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (passed == TRUE), "Test #2 Testing: 2.6: _lwlog_read should have returned LOG_DOES_NOT_EXIST")

    lwlog_test_print_result(2, passed, "use before create");
    all_passed = all_passed && passed;
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_3_main_task
// Comments     : TEST #3: Test lwlog creation & destruction
//
//END---------------------------------------------------------------------
void tc_3_main_task(void)
{
    _mqx_uint           lwlog, hi_mem;
    bool             passed;

    passed = TRUE;

    for (lwlog = 1; lwlog < LOG_MAXIMUM_NUMBER; lwlog++) {
        passed = lwlog_test_fn(passed, 3, 1, _lwlog_create(lwlog, TEST_LWLOG_ENTRIES, 0), MQX_OK);
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, (passed == TRUE), "Test #3 Testing: 3.1: _lwlog_create operation")
    } /* Endfor */
    passed = lwlog_test_fn(passed, 3, 2, _lwlog_create(LOG_MAXIMUM_NUMBER,
        TEST_LWLOG_ENTRIES, 0), LOG_INVALID);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (passed == TRUE), "Test #3 Testing: 3.2: _lwlog_create with maximum log number should have returned LOG_INVALID")


    /* Save our current highwater */
    hi_mem = (uint32_t)_mem_get_highwater();

    for (lwlog = 1; lwlog < LOG_MAXIMUM_NUMBER; lwlog++) {
        passed = lwlog_test_fn(passed, 3, 3, _lwlog_destroy(lwlog), MQX_OK);
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, (passed == TRUE), "Test #3 Testing: 3.3: _lwlog_destroy operation")
    } /* Endfor */

    passed = lwlog_test_fn(passed, 3, 4, _lwlog_destroy(LOG_MAXIMUM_NUMBER), LOG_INVALID);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (passed == TRUE), "Test #3 Testing: 3.4: _lwlog_destroy with maximum log number should have returned LOG_INVALID")

    for (lwlog = 1; lwlog < LOG_MAXIMUM_NUMBER; lwlog++) {
        passed = lwlog_test_fn(passed, 3, 5, _lwlog_destroy(lwlog), LOG_DOES_NOT_EXIST);
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, (passed == TRUE), "Test #3 Testing: 3.5: _lwlog_destroy with destroyed logs should have returned LOG_DOES_NOT_EXIST")
    } /* Endfor */

    for (lwlog = 1; lwlog < LOG_MAXIMUM_NUMBER; lwlog++) {
        passed = lwlog_test_fn(passed, 3, 6, _lwlog_create(lwlog, TEST_LWLOG_ENTRIES, 0), MQX_OK);
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, (passed == TRUE), "Test #3 Testing: 3.6: _lwlog_create operation")
    } /* Endfor */

    /* Check our current highwater */
    passed = lwlog_test_fn(passed, 3, 7, (uint32_t)_mem_get_highwater(), hi_mem);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (passed == TRUE), "Test #3 Testing: 3.7: check our current highwater")

    for (lwlog = 1; lwlog < LOG_MAXIMUM_NUMBER; lwlog++) {
        passed = lwlog_test_fn(passed, 3, 8, _lwlog_create(lwlog, TEST_LWLOG_ENTRIES, 0), LOG_EXISTS);
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, (passed == TRUE), "Test #3 Testing: 3.8: _lwlog_create should have returned LOG_EXISTS")
    } /* Endfor */

    lwlog_test_print_result(3, passed, "creation/destruction");
    all_passed = all_passed && passed;
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_4_main_task
// Comments     : TEST #4: Test lwlog read types
//
//END---------------------------------------------------------------------
void tc_4_main_task(void)
{
    _mqx_uint           i, lwlog, read_type;
    bool             passed;
    LWLOG_ENTRY_STRUCT  entry;

    passed = TRUE;
#if MQX_LWLOG_TIME_STAMP_IN_TICKS == 0
    _mem_zero(&Saved_seconds[0], sizeof(Saved_seconds));
    _mem_zero(&Saved_microseconds[0], sizeof(Saved_microseconds));
#else
    _mem_zero(&Saved_ticks[0], sizeof(Saved_ticks));
#endif

    /* Test reading empty lwlog */
    read_type = LOG_READ_NEWEST;

    for (lwlog = 1; lwlog < LOG_MAXIMUM_NUMBER; lwlog++) {
        passed = lwlog_test_fn(passed, 4, 1, _lwlog_read(lwlog, read_type, &entry),
            LOG_ENTRY_NOT_AVAILABLE);
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (passed == TRUE), "Test #4 Testing: 4.1: _lwlog_read should have returned LOG_ENTRY_NOT_AVAILABLE")
    } /* Endfor */

    passed = lwlog_test_fn(passed, 4, 2, _lwlog_write(lwlog,
        (_mqx_max_type)lwlog, (_mqx_max_type)(2 * lwlog), 0, 0, 0, 0, 0), LOG_INVALID);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (passed == TRUE), "Test #4 Testing: 4.2: _lwlog_write should have returned LOG_INVALID")

    /* Write TEST_LWLOG_ENTRIES entries to each lwlog */
    for (lwlog = 1; lwlog < LOG_MAXIMUM_NUMBER; lwlog++) {
        for (i = 0; i < TEST_LWLOG_ENTRIES; i++) {
            passed = lwlog_test_fn(passed, 4, 3, _lwlog_write(lwlog,  
                (_mqx_max_type)lwlog, (_mqx_max_type)i, 0, 0, 0, 0, 0), MQX_OK);
            EU_ASSERT_REF_TC_MSG(tc_4_main_task, (passed == TRUE), "Test #4 Testing: 4.3: _lwlog_write operation")
        } /* Endfor */
    } /* Endfor */
    passed = lwlog_test_fn(passed, 4, 4, _lwlog_write(lwlog,
        (_mqx_max_type)lwlog, (_mqx_max_type)lwlog, 0, 0, 0, 0, 0), LOG_INVALID);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (passed == TRUE), "Test #4 Testing: 4.4: _lwlog_write should have returned LOG_INVALID")

    /* Now read newest */
    read_type = LOG_READ_NEWEST;
    for (lwlog = 1; lwlog < LOG_MAXIMUM_NUMBER; lwlog++) {
        passed = lwlog_test_fn(passed, 4, 6, _lwlog_read(lwlog, read_type, &entry), MQX_OK);
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (passed == TRUE), "Test #4 Testing: 4.6: _lwlog_read operation")

        passed = lwlog_test_fn(passed, 4, 7, lwlog_test_entry(lwlog, &entry, 4, 4), TRUE );
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (passed == TRUE), "Test #4 Testing: 4.7: lwlog_test_entry operation")
    } /* Endfor */

#if MQX_LWLOG_TIME_STAMP_IN_TICKS == 0
    _mem_zero(&Saved_seconds[0], sizeof(Saved_seconds));
    _mem_zero(&Saved_microseconds[0], sizeof(Saved_microseconds));
#else
    _mem_zero(&Saved_ticks[0], sizeof(Saved_ticks));
#endif

    /* Now read oldest */
    read_type = LOG_READ_OLDEST;
    for (lwlog = 1; lwlog < LOG_MAXIMUM_NUMBER; lwlog++) {
        passed = lwlog_test_fn(passed, 4, 8, _lwlog_read(lwlog, read_type, (LWLOG_ENTRY_STRUCT_PTR)&entry), MQX_OK);
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (passed == TRUE), "Test #4 Testing: 4.8: _lwlog_read operation")

        passed = lwlog_test_fn(passed, 4, 9, lwlog_test_entry(lwlog, &entry, 0, 0), TRUE );
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (passed == TRUE), "Test #4 Testing: 4.9: lwlog_test_entry operation")
    } /* Endfor */

    /* Now read next */
    read_type = LOG_READ_NEXT;
    for (lwlog = 1; lwlog < LOG_MAXIMUM_NUMBER; lwlog++) {
        for (i = 1; i < TEST_LWLOG_ENTRIES; i++) {
            passed = lwlog_test_fn(passed, 4, 10, _lwlog_read(lwlog, read_type, (LWLOG_ENTRY_STRUCT_PTR)&entry), MQX_OK);
            EU_ASSERT_REF_TC_MSG(tc_4_main_task, (passed == TRUE), "Test #4 Testing: 4.10: _lwlog_read operation")

            passed = lwlog_test_fn(passed, 4, 11, lwlog_test_entry(lwlog, &entry, i, i), TRUE );
            EU_ASSERT_REF_TC_MSG(tc_4_main_task, (passed == TRUE), "Test #4 Testing: 4.11: lwlog_test_entry operation")
        } /* Endfor */
    } /* Endfor */
   
#if MQX_LWLOG_TIME_STAMP_IN_TICKS == 0
    _mem_zero(&Saved_seconds[0], sizeof(Saved_seconds));
    _mem_zero(&Saved_microseconds[0], sizeof(Saved_microseconds));
#else
    _mem_zero(&Saved_ticks[0], sizeof(Saved_ticks));
#endif

    /* Now read oldest & delete */
    read_type = LOG_READ_OLDEST_AND_DELETE;
    for (lwlog = 1; lwlog < LOG_MAXIMUM_NUMBER; lwlog++) {
        for (i = 0; i < TEST_LWLOG_ENTRIES; i++) {
            passed = lwlog_test_fn(passed, 4, 12, _lwlog_read(lwlog, read_type, (LWLOG_ENTRY_STRUCT_PTR)&entry), MQX_OK);
            EU_ASSERT_REF_TC_MSG(tc_4_main_task, (passed == TRUE), "Test #4 Testing: 4.12: _lwlog_read operation")

            passed = lwlog_test_fn(passed, 4, 13, lwlog_test_entry(lwlog, &entry, i, i), TRUE);
            EU_ASSERT_REF_TC_MSG(tc_4_main_task, (passed == TRUE), "Test #4 Testing: 4.13: lwlog_test_entry operation")
        } /* Endfor */
    } /* Endfor */

    /* Make sure lwlog is empty */
    read_type = LOG_READ_NEWEST;
    for (lwlog = 1; lwlog < LOG_MAXIMUM_NUMBER; lwlog++) {
        passed = lwlog_test_fn(passed, 4, 14, _lwlog_read(lwlog, read_type, (LWLOG_ENTRY_STRUCT_PTR)&entry),
            LOG_ENTRY_NOT_AVAILABLE);
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (passed == TRUE), "Test #4 Testing: 4.14: _lwlog_read should have returned LOG_ENTRY_NOT_AVAILABLE")
    } /* Endfor */

    lwlog_test_print_result(4, passed, "read/write");
    all_passed = all_passed && passed;
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_5_main_task
// Comments     : TEST #5: Test lwlog types (OVERWRITE or not)
//
//END---------------------------------------------------------------------
void tc_5_main_task(void)
{
    _mqx_uint           i, lwlog, read_type;
    bool             passed;
    LWLOG_ENTRY_STRUCT  entry;

    passed = TRUE;

    /* Write TEST_LWLOG_ENTRIES entries to each lwlog */
    for (lwlog = 1; lwlog < LOG_MAXIMUM_NUMBER; lwlog++) {
        for (i = 0; i < TEST_LWLOG_ENTRIES; i++) {
            passed = lwlog_test_fn(passed, 5, 1, _lwlog_write(lwlog,
                (_mqx_max_type)lwlog, (_mqx_max_type)i, 0, 0, 0, 0, 0), MQX_OK);
            EU_ASSERT_REF_TC_MSG(tc_5_main_task, (passed == TRUE), "Test #5 Testing: 5.1: _lwlog_write operation")
        } /* Endfor */
    } /* Endfor */

    /* Try to write a 6th entry to each lwlog */
    for (lwlog = 1; lwlog < LOG_MAXIMUM_NUMBER; lwlog++) {
        passed = lwlog_test_fn(passed, 5, 2, _lwlog_write(lwlog,
            (_mqx_max_type)lwlog, TEST_LWLOG_ENTRIES, 0, 0, 0, 0, 0), LOG_FULL);
            EU_ASSERT_REF_TC_MSG(tc_5_main_task, (passed == TRUE), "Test #5 Testing: 5.2: _lwlog_write should have returned LOG_FULL")
    } /* Endfor */

    /* Now read oldest & delete */
    read_type = LOG_READ_OLDEST_AND_DELETE;
    for (lwlog = 1; lwlog < LOG_MAXIMUM_NUMBER; lwlog++) {
        for (i = 0; i < TEST_LWLOG_ENTRIES; i++) {
            passed = lwlog_test_fn(passed, 5, 3, _lwlog_read(lwlog, read_type, (LWLOG_ENTRY_STRUCT_PTR)&entry), MQX_OK);
            EU_ASSERT_REF_TC_MSG(tc_5_main_task, (passed == TRUE), "Test #5 Testing: 5.3: _lwlog_read operation")
         
            passed = lwlog_test_fn(passed, 5, 4, lwlog_test_entry(lwlog, &entry, i,
                (TEST_LWLOG_ENTRIES + i)), TRUE);
            EU_ASSERT_REF_TC_MSG(tc_5_main_task, (passed == TRUE), "Test #5 Testing: 5.4: lwlog_test_entry operation")
        } /* Endfor */
    } /* Endfor */

    /* Make sure lwlog is empty */
    read_type = LOG_READ_NEWEST;
    for (lwlog = 1; lwlog < LOG_MAXIMUM_NUMBER; lwlog++) {
        passed = lwlog_test_fn(passed, 5, 5, _lwlog_read(lwlog, read_type, (LWLOG_ENTRY_STRUCT_PTR)&entry),
            LOG_ENTRY_NOT_AVAILABLE);
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (passed == TRUE), "Test #5 Testing: 5.5: _lwlog_read should have returned LOG_ENTRY_NOT_AVAILABLE")
    } /* Endfor */

    /* Destroy & recreate with overwrite */
                              
    for (lwlog = 1; lwlog < LOG_MAXIMUM_NUMBER; lwlog++) {
        passed = lwlog_test_fn(passed, 5, 6, _lwlog_destroy(lwlog), MQX_OK);
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (passed == TRUE), "Test #5 Testing: 5.6: _lwlog_destroy operation")

        passed = lwlog_test_fn(passed, 5, 7, _lwlog_create(lwlog, TEST_LWLOG_ENTRIES,
            LOG_OVERWRITE), MQX_OK);
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (passed == TRUE), "Test #5 Testing: 5.7: _lwlog_create operation")
    } /* Endfor */

    /* Write 2*TEST_LWLOG_ENTRIES entries to each lwlog */
    for (lwlog = 1; lwlog < LOG_MAXIMUM_NUMBER; lwlog++) {
        for (i = 0; i < (2 * TEST_LWLOG_ENTRIES); i++) {
            passed = lwlog_test_fn(passed, 5, 8, _lwlog_write(lwlog, 
                (_mqx_max_type)lwlog, (_mqx_max_type)i, 0, 0, 0, 0, 0), MQX_OK);
            EU_ASSERT_REF_TC_MSG(tc_5_main_task, (passed == TRUE), "Test #5 Testing: 5.8: _lwlog_write operation")
        } /* Endfor */
    } /* Endfor */

    /* Now read oldest & delete */
    read_type = LOG_READ_OLDEST_AND_DELETE;
    for (lwlog = 1; lwlog < LOG_MAXIMUM_NUMBER; lwlog++) {
        for (i = 0; i < TEST_LWLOG_ENTRIES; i++) {
            passed = lwlog_test_fn(passed, 5, 9, _lwlog_read(lwlog, read_type, (LWLOG_ENTRY_STRUCT_PTR)&entry), MQX_OK);
            EU_ASSERT_REF_TC_MSG(tc_5_main_task, (passed == TRUE), "Test #5 Testing: 5.9: _lwlog_read operation")

            passed = lwlog_test_fn(passed, 5, 10, lwlog_test_entry(lwlog, &entry, (TEST_LWLOG_ENTRIES + i),
                (TEST_LWLOG_ENTRIES + i)), TRUE);
            EU_ASSERT_REF_TC_MSG(tc_5_main_task, (passed == TRUE), "Test #5 Testing: 5.10: lwlog_test_entry operation")
        } /* Endfor */
    } /* Endfor */

    /* Make sure lwlog is empty */
    read_type = LOG_READ_NEWEST;
    for (lwlog = 1; lwlog < LOG_MAXIMUM_NUMBER; lwlog++) {
        passed = lwlog_test_fn(passed, 5, 11, _lwlog_read(lwlog, read_type, (LWLOG_ENTRY_STRUCT_PTR)&entry),
            LOG_ENTRY_NOT_AVAILABLE);
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (passed == TRUE), "Test #5 Testing: 5.11: _lwlog_read should have returned LOG_ENTRY_NOT_AVAILABLE")
    } /* Endfor */

    lwlog_test_print_result(5, passed, "overwrite");
    all_passed = all_passed && passed;
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_6_main_task
// Comments     : TEST #6: Test lwlog reset
//
//END---------------------------------------------------------------------
void tc_6_main_task(void)
{
    _mqx_uint           lwlog, read_type;
    bool             passed;
    LWLOG_ENTRY_STRUCT  entry;

#if MQX_LWLOG_TIME_STAMP_IN_TICKS == 0
    _mem_zero(&Saved_seconds[0], sizeof(Saved_seconds));
    _mem_zero(&Saved_microseconds[0], sizeof(Saved_microseconds));
#else
    _mem_zero(&Saved_ticks[0], sizeof(Saved_ticks));
#endif

    passed = TRUE;
    /* Write an entry to each lwlog */
    for (lwlog = 1; lwlog < LOG_MAXIMUM_NUMBER; lwlog++) {
        passed = lwlog_test_fn(passed, 6, 1, _lwlog_write(lwlog, 
            (_mqx_max_type)lwlog, (_mqx_max_type)lwlog, 0, 0, 0, 0, 0), MQX_OK);
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (passed == TRUE), "Test #6 Testing: 6.1: _lwlog_write operation")
    } /* Endfor */

    /* reset the lwlog */
    for (lwlog = 1; lwlog < LOG_MAXIMUM_NUMBER; lwlog++) {
        passed = lwlog_test_fn(passed, 6, 2, _lwlog_reset(lwlog), MQX_OK);
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (passed == TRUE), "Test #6 Testing: 6.2: _lwlog_reset operation")
    } /* Endfor */

    passed = lwlog_test_fn(passed, 6, 3, _lwlog_reset(LOG_MAXIMUM_NUMBER), LOG_INVALID);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (passed == TRUE), "Test #6 Testing: 6.3: _lwlog_reset operation")

    /* Make sure lwlog is empty */
    read_type = LOG_READ_NEWEST;
    for (lwlog = 1; lwlog < LOG_MAXIMUM_NUMBER; lwlog++) {
        passed = lwlog_test_fn(passed, 6, 4, _lwlog_read(lwlog, read_type, (LWLOG_ENTRY_STRUCT_PTR)&entry),
            LOG_ENTRY_NOT_AVAILABLE);
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (passed == TRUE), "Test #6 Testing: 6.4: _lwlog_read should have returned LOG_ENTRY_NOT_AVAILABLE")
    } /* Endfor */

    for (lwlog = 1; lwlog < LOG_MAXIMUM_NUMBER; lwlog++) {
        passed = lwlog_test_fn(passed, 6, 5, _lwlog_write(lwlog, 
            (_mqx_max_type)lwlog, 0, 0, 0, 0, 0, 0), MQX_OK);
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (passed == TRUE), "Test #6 Testing: 6.5: _lwlog_write operation")

        passed = lwlog_test_fn(passed, 6, 6, _lwlog_write(lwlog, 
            (_mqx_max_type)lwlog, 1, 0, 0, 0, 0, 0), MQX_OK);
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (passed == TRUE), "Test #6 Testing: 6.6: _lwlog_write operation")

        passed = lwlog_test_fn(passed, 6, 7, _lwlog_disable(lwlog), MQX_OK);
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (passed == TRUE), "Test #6 Testing: 6.7: _lwlog_disable operation")

        passed = lwlog_test_fn(passed, 6, 8, _lwlog_read(lwlog, LOG_READ_OLDEST_AND_DELETE,
            (LWLOG_ENTRY_STRUCT_PTR)&entry), MQX_OK);
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (passed == TRUE), "Test #6 Testing: 6.8: _lwlog_read operation")

        passed = lwlog_test_fn(passed, 6, 9, lwlog_test_entry(lwlog, &entry, 0, 0), TRUE);
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (passed == TRUE), "Test #6 Testing: 6.9: lwlog_test_entry operation")

        passed = lwlog_test_fn(passed, 6, 10, _lwlog_read(lwlog, LOG_READ_OLDEST_AND_DELETE,
            (LWLOG_ENTRY_STRUCT_PTR)&entry), MQX_OK);
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (passed == TRUE), "Test #6 Testing: 6.10: _lwlog_read operation")

        passed = lwlog_test_fn(passed, 6, 11, lwlog_test_entry(lwlog, &entry, 1, 1), TRUE);
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (passed == TRUE), "Test #6 Testing: 6.11: lwlog_test_entry operation")

        passed = lwlog_test_fn(passed, 6, 12, _lwlog_enable(lwlog), MQX_OK);
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (passed == TRUE), "Test #6 Testing: 6.12: _lwlog_enable operation")

        passed = lwlog_test_fn(passed, 6, 13, _lwlog_reset(lwlog), MQX_OK);
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (passed == TRUE), "Test #6 Testing: 6.13: _lwlog_reset operation")
    } /* Endfor */

    lwlog_test_print_result(6, passed, "reset");
    all_passed = all_passed && passed;
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_7_main_task
// Comments     : TEST #7: Test lwlog enable/disable
//
//END---------------------------------------------------------------------
void tc_7_main_task(void)
{
    _mqx_uint           lwlog, read_type;
    bool             passed;
    LWLOG_ENTRY_STRUCT  entry;

    passed = TRUE;
    read_type = LOG_READ_NEWEST;
    for (lwlog = 1; lwlog < LOG_MAXIMUM_NUMBER; lwlog++) {
        passed = lwlog_test_fn(passed, 7, 1, _lwlog_write(lwlog, 
            (_mqx_max_type)lwlog, 0, 0, 0, 0, 0, 0), MQX_OK);
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, (passed == TRUE), "Test #7 Testing: 7.1: _lwlog_write operation")

        passed = lwlog_test_fn(passed, 7, 2, _lwlog_disable(lwlog), MQX_OK );
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, (passed == TRUE), "Test #7 Testing: 7.2: _lwlog_disable operation")

        passed = lwlog_test_fn(passed, 7, 3, _lwlog_write(lwlog, 
            (_mqx_max_type)lwlog, 1, 0, 0, 0, 0, 0), LOG_DISABLED);
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, (passed == TRUE), "Test #7 Testing: 7.3: _lwlog_write should have returned LOG_DISABLED")

        passed = lwlog_test_fn(passed, 7, 4, _lwlog_enable(lwlog), MQX_OK );
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, (passed == TRUE), "Test #7 Testing: 7.4: _lwlog_enable operation")

        passed = lwlog_test_fn(passed, 7, 5, _lwlog_write(lwlog,
            (_mqx_max_type)lwlog, 2, 0, 0, 0, 0, 0), MQX_OK);
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, (passed == TRUE), "Test #7 Testing: 7.5: _lwlog_write operation")

        passed = lwlog_test_fn(passed, 7, 6, _lwlog_disable(lwlog), MQX_OK );
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, (passed == TRUE), "Test #7 Testing: 7.6: _lwlog_disable operation")

        passed = lwlog_test_fn(passed, 7, 7, _lwlog_read(lwlog, LOG_READ_OLDEST_AND_DELETE,
            (LWLOG_ENTRY_STRUCT_PTR)&entry), MQX_OK);
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, (passed == TRUE), "Test #7 Testing: 7.7: _lwlog_read operation")

        passed = lwlog_test_fn(passed, 7, 8, lwlog_test_entry(lwlog, &entry, 0, 0), TRUE);
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, (passed == TRUE), "Test #7 Testing: 7.8: lwlog_test_entry operation")

        passed = lwlog_test_fn(passed, 7, 9, _lwlog_enable(lwlog), MQX_OK);
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, (passed == TRUE), "Test #7 Testing: 7.9: _lwlog_enable operation")

        passed = lwlog_test_fn(passed, 7, 10, _lwlog_read(lwlog, LOG_READ_OLDEST_AND_DELETE,
            (LWLOG_ENTRY_STRUCT_PTR)&entry), MQX_OK);
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, (passed == TRUE), "Test #7 Testing: 7.10: _lwlog_read operation")

        passed = lwlog_test_fn(passed, 7, 11, lwlog_test_entry(lwlog, &entry, 2, 1), TRUE);
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, (passed == TRUE), "Test #7 Testing: 7.11: lwlog_test_entry operation")

        passed = lwlog_test_fn(passed, 7, 12, _lwlog_reset(lwlog), MQX_OK);
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, (passed == TRUE), "Test #7 Testing: 7.12: _lwlog_reset operation")

        passed = lwlog_test_fn(passed, 7, 13, _lwlog_destroy(lwlog), MQX_OK);
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, (passed == TRUE), "Test #7 Testing: 7.13: _lwlog_destroy operation")
    }
    lwlog_test_print_result(7, passed, "enable/disable");
    all_passed = all_passed && passed;
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_8_main_task
// Comments     : TEST #8: ISR Test
//
//END---------------------------------------------------------------------
void tc_8_main_task(void)
{
    bool             passed;

    passed = TRUE;
    ISR_test_result = FALSE;

    passed = lwlog_test_fn(passed, 8, 1, _lwlog_create(1, TEST_LWLOG_ENTRIES, 0), MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, (passed == TRUE), "Test #8 Testing: 8.1: _lwlog_create operation")

    Old_ISR_data = _int_get_isr_data(BSP_TIMER_INTERRUPT_VECTOR);
    Old_ISR      = _int_get_isr(BSP_TIMER_INTERRUPT_VECTOR);

    ISR_test = TRUE;
    _int_install_isr(BSP_TIMER_INTERRUPT_VECTOR, lwlog_test_ISR, 0);

    _time_delay(_time_get_resolution() * 2);

    _int_install_isr(BSP_TIMER_INTERRUPT_VECTOR, Old_ISR, Old_ISR_data);

    passed = lwlog_test_fn(passed, 8, 8, _lwlog_destroy(1), MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, (passed == TRUE), "Test #8 Testing: 8.8: _lwlog_destroy operation")

    passed = passed && ISR_test_result;
    lwlog_test_print_result(8, passed, "calling from an ISR");
    all_passed = all_passed && passed;
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_9_main_task
// Comments     : TEST #9: Variable sized read & write
//
//END---------------------------------------------------------------------
void tc_9_main_task(void)
{
    _mqx_uint           i;
    bool             passed;
    LWLOG_ENTRY_STRUCT  entry;

    passed = TRUE;
#if MQX_LWLOG_TIME_STAMP_IN_TICKS == 0
    _mem_zero(&Saved_seconds[0], sizeof(Saved_seconds));
    _mem_zero(&Saved_microseconds[0], sizeof(Saved_microseconds));
#else
    _mem_zero(&Saved_ticks[0], sizeof(Saved_ticks));
#endif

    passed = lwlog_test_fn(passed, 9, 1, _lwlog_disable(1), LOG_DOES_NOT_EXIST);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (passed == TRUE), "Test #9 Testing: 9.1: _lwlog_disable should have returned LOG_DOES_NOT_EXIST")

    passed = lwlog_test_fn(passed, 9, 2, _lwlog_create(1, BIG_LWLOG_ENTRIES, 0), MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (passed == TRUE), "Test #9 Testing: 9.2: _lwlog_create operation")

    passed = lwlog_test_fn(passed, 9, 3, _lwlog_write(1, 0,  0,  0,  0,  0,  0,  0),  MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (passed == TRUE), "Test #9 Testing: 9.3: _lwlog_write operation")

    passed = lwlog_test_fn(passed, 9, 4, _lwlog_write(1, 11, 0,  0,  0,  0,  0,  0),  MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (passed == TRUE), "Test #9 Testing: 9.4: _lwlog_write operation")

    passed = lwlog_test_fn(passed, 9, 5, _lwlog_write(1, 21, 22, 0,  0,  0,  0,  0),  MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (passed == TRUE), "Test #9 Testing: 9.5: _lwlog_write operation")

    passed = lwlog_test_fn(passed, 9, 6, _lwlog_write(1, 31, 32, 33, 0,  0,  0,  0),  MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (passed == TRUE), "Test #9 Testing: 9.6: _lwlog_write operation")

    passed = lwlog_test_fn(passed, 9, 7, _lwlog_write(1, 41, 42, 43, 44, 0,  0,  0),  MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (passed == TRUE), "Test #9 Testing: 9.7: _lwlog_write operation")

    passed = lwlog_test_fn(passed, 9, 8, _lwlog_write(1, 51, 52, 53, 54, 55, 0,  0),  MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (passed == TRUE), "Test #9 Testing: 9.8: _lwlog_write operation")

    passed = lwlog_test_fn(passed, 9, 9, _lwlog_write(1, 61, 62, 63, 64, 65, 66, 0),  MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (passed == TRUE), "Test #9 Testing: 9.9: _lwlog_write operation")

    passed = lwlog_test_fn(passed, 9, 9, _lwlog_write(1, 71, 72, 73, 74, 75, 76, 77), MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (passed == TRUE), "Test #9 Testing: 9.9: _lwlog_write operation")

    for (i = 0; i < 8; i++) {
        passed = lwlog_test_fn(passed, 9, 14, _lwlog_read(1, LOG_READ_OLDEST_AND_DELETE, (LWLOG_ENTRY_STRUCT_PTR)&entry), MQX_OK);
        EU_ASSERT_REF_TC_MSG(tc_9_main_task, (passed == TRUE), "Test #9 Testing: 9.14: _lwlog_read operation")
      
        passed = lwlog_test_fn(passed, 9, 15, lwlog_test_entry2(&entry, i, i, i), TRUE);
        EU_ASSERT_REF_TC_MSG(tc_9_main_task, (passed == TRUE), "Test #9 Testing: 9.15: lwlog_test_entry2 operation")
    } /* Endfor */


    passed = lwlog_test_fn(passed, 9, 16, _lwlog_write(1, 0, 0,  0,  0,  0,   0,  0), MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (passed == TRUE), "Test #9 Testing: 9.16: _lwlog_write operation")

    passed = lwlog_test_fn(passed, 9, 17, _lwlog_write(1, 11, 0,  0,  0,  0,  0,  0), MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (passed == TRUE), "Test #9 Testing: 9.17: _lwlog_write operation")

    passed = lwlog_test_fn(passed, 9, 18, _lwlog_write(1, 21, 22, 0,  0,  0,  0,  0), MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (passed == TRUE), "Test #9 Testing: 9.18: _lwlog_write operation")

    passed = lwlog_test_fn(passed, 9, 19, _lwlog_write(1, 31, 32, 33, 0,  0,  0,  0), MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (passed == TRUE), "Test #9 Testing: 9.19: _lwlog_write operation")

    passed = lwlog_test_fn(passed, 9, 20, _lwlog_write(1, 41, 42, 43, 44, 0,  0,  0), MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (passed == TRUE), "Test #9 Testing: 9.20: _lwlog_write operation")

    passed = lwlog_test_fn(passed, 9, 21, _lwlog_write(1, 51, 52, 53, 54, 55, 0,  0), MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (passed == TRUE), "Test #9 Testing: 9.21: _lwlog_write operation")

    passed = lwlog_test_fn(passed, 9, 22, _lwlog_write(1, 61, 62, 63, 64, 65, 66, 0), MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (passed == TRUE), "Test #9 Testing: 9.22: _lwlog_write operation")

    passed = lwlog_test_fn(passed, 9, 22, _lwlog_write(1, 71, 72, 73, 74, 75, 76, 77), MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (passed == TRUE), "Test #9 Testing: 9.22: _lwlog_write operation")

    for (i = 0; i < 8; i++) {
        passed = lwlog_test_fn(passed, 9, 27, _lwlog_read(1, LOG_READ_OLDEST_AND_DELETE,
            (LWLOG_ENTRY_STRUCT_PTR)&entry), MQX_OK);
        EU_ASSERT_REF_TC_MSG(tc_9_main_task, (passed == TRUE), "Test #9 Testing: 9.27: _lwlog_read operation")
      
        passed = lwlog_test_fn(passed, 9, 28, lwlog_test_entry2(&entry, i, 1, i), TRUE);
        EU_ASSERT_REF_TC_MSG(tc_9_main_task, (passed == TRUE), "Test #9 Testing: 9.28: lwlog_test_entry2 operation")
    } /* Endfor */

    passed = lwlog_test_fn(passed, 9, 29, _lwlog_write(1, 0,   0, 0, 0, 0, 0, 0), MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (passed == TRUE), "Test #9 Testing: 9.29: _lwlog_write operation")

    passed = lwlog_test_fn(passed, 9, 30, _lwlog_write(1, 11,  0, 0, 0, 0, 0, 0), MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (passed == TRUE), "Test #9 Testing: 9.30: _lwlog_write operation")

    passed = lwlog_test_fn(passed, 9, 31, _lwlog_write(1, 21,  0, 0, 0, 0, 0, 0), MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (passed == TRUE), "Test #9 Testing: 9.31: _lwlog_write operation")

    passed = lwlog_test_fn(passed, 9, 32, _lwlog_write(1, 31,  0, 0, 0, 0, 0, 0), MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (passed == TRUE), "Test #9 Testing: 9.32: _lwlog_write operation")

    passed = lwlog_test_fn(passed, 9, 33, _lwlog_write(1, 41,  0, 0, 0, 0, 0, 0), MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (passed == TRUE), "Test #9 Testing: 9.33: _lwlog_write operation")

    passed = lwlog_test_fn(passed, 9, 34, _lwlog_write(1, 51,  0, 0, 0, 0, 0, 0), MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (passed == TRUE), "Test #9 Testing: 9.34: _lwlog_write operation")

    passed = lwlog_test_fn(passed, 9, 35, _lwlog_write(1, 61,  0, 0, 0, 0, 0, 0), MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (passed == TRUE), "Test #9 Testing: 9.35: _lwlog_write operation")

    passed = lwlog_test_fn(passed, 9, 36, _lwlog_write(1, 71,  0, 0, 0, 0, 0, 0), MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (passed == TRUE), "Test #9 Testing: 9.36: _lwlog_write operation")

    passed = lwlog_test_fn(passed, 9, 37, _lwlog_write(1, 81,  0, 0, 0, 0, 0, 0), MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (passed == TRUE), "Test #9 Testing: 9.37: _lwlog_write operation")

    passed = lwlog_test_fn(passed, 9, 38, _lwlog_write(1, 91,  0, 0, 0, 0, 0, 0), MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (passed == TRUE), "Test #9 Testing: 9.38: _lwlog_write operation")

    passed = lwlog_test_fn(passed, 9, 39, _lwlog_write(1, 101, 0, 0, 0, 0, 0, 0), MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (passed == TRUE), "Test #9 Testing: 9.39: _lwlog_write operation")

    for (i = 0; i < 11; i++) {
        passed = lwlog_test_fn(passed, 9, 40, _lwlog_read(1, LOG_READ_OLDEST_AND_DELETE,
            (LWLOG_ENTRY_STRUCT_PTR)&entry), MQX_OK);
        EU_ASSERT_REF_TC_MSG(tc_9_main_task, (passed == TRUE), "Test #9 Testing: 9.40: _lwlog_read operation")

        passed = lwlog_test_fn(passed, 9, 41, lwlog_test_entry2(&entry, i, i, 1), TRUE);
        EU_ASSERT_REF_TC_MSG(tc_9_main_task, (passed == TRUE), "Test #9 Testing: 9.41: lwlog_test_entry2 operation")
    } /* Endfor */

    lwlog_test_print_result(9, passed, "Variable sized read/write");
    all_passed = all_passed && passed;
}

//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_lwlog)
    EU_TEST_CASE_ADD(tc_1_main_task, " Test #1 - Test Auto-component creation"),
    EU_TEST_CASE_ADD(tc_2_main_task, " Test #2 - Test Invalid use of lwlog before it is created"),
    EU_TEST_CASE_ADD(tc_3_main_task, " Test #3 - Test lwlog creation and destruction"),
    EU_TEST_CASE_ADD(tc_4_main_task, " Test #4 - Test lwlog read types"),
    EU_TEST_CASE_ADD(tc_5_main_task, " Test #5 - Test lwlog types (OVERWRITE or not)"),
    EU_TEST_CASE_ADD(tc_6_main_task, " Test #6 - Test lwlog reset"),
    EU_TEST_CASE_ADD(tc_7_main_task, " Test #7 - Test lwlog enable/disable"),
    EU_TEST_CASE_ADD(tc_8_main_task, " Test #8 - ISR Test"),
    EU_TEST_CASE_ADD(tc_9_main_task, " Test #9 - Variable sized read and write")
EU_TEST_SUITE_END(suite_lwlog)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_lwlog, "- Tests the lwlog component")
EU_TEST_REGISTRY_END()
//------------------------------------------------------------------------

/*TASK*-------------------------------------------------------------------
* 
* Task Name    : Main_task
* Comments     :
*   This task tests the lwlog component.  
* The following tests are conducted:
*
* #1: Test Auto-component creation 
* #2: Invalid use of lwlog before it is created
* #3: Test lwlog creation & destruction
* #4: Test lwlog read types
* #5: Test lwlog types (OVERWRITE or not)
* #6: Test lwlog reset
* #7: Test lwlog enable/disable
* #8: ISR Test
* #9: Variable sized read & write
*
*END*----------------------------------------------------------------------*/
void main_task
   (
      uint32_t dummy
   )
{ /* Body */
   all_passed = TRUE;
   //test_initialize();
   //EU_RUN_ALL_TESTS(test_result_array);
   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   _time_delay_ticks(10);  /* waiting for all lwloggings to be printed out */
   eunit_test_stop();
   
//   _time_delay_ticks(10);  /* waiting for all lwloggings to be printed out */
//   test_stop();
   _mqx_exit(0);

} /* Endbody */
/* EOF */
