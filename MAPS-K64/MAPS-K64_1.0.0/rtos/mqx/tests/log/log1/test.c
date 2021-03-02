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
* $Version : 3.8.5.1$
* $Date    : Feb-22-2012$
*
* Comments:
*
*   This file contains the source functions for the log management
*   component.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <mem_prv.h>
#include <mqx_prv.h>
#include <log.h>
#include <klog.h>


#include <mqx_inc.h>
#include <lwevent.h>
#include <lwevent_prv.h>
#include <mutex.h>


//#include <test_module.h>
#include "util.h"

#include "EUnit.h"
#include "eunit_crit_secs.h"

#define MAIN_TASK 10
#define FILE_BASENAME test

extern void main_task(_mqx_uint dummy);

#define LOG_HEADER_SIZE    (sizeof(LOG_ENTRY_STRUCT) / sizeof(_mqx_uint))

#define TEST_LOG_ENTRIES      5
#define TEST_LOG_PARAMS       2
#define TEST_LOG_ENTRY_SIZE   (LOG_HEADER_SIZE + TEST_LOG_PARAMS)
#define TEST_LOG_SIZE         (TEST_LOG_ENTRY_SIZE * TEST_LOG_ENTRIES)
#define TEST_VALUE            3

#if PSP_MQX_CPU_IS_KINETIS_L
#define BIG_LOG_ENTRIES    50
#else
#define BIG_LOG_ENTRIES    100
#endif

#define BIG_LOG_SIZE       ((LOG_HEADER_SIZE + 10) * BIG_LOG_ENTRIES)

#define PASS  TRUE
#define FAIL  FALSE


TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
   { MAIN_TASK, main_task, 3000, 8, "Main",  MQX_AUTO_START_TASK },
   { 0,         0,         0,    0, 0,       0 }
};

typedef struct {
   LOG_ENTRY_STRUCT  HEADER;
   _mqx_uint         DATA[BIG_LOG_ENTRIES];
} TEST_LOG_STRUCT, * TEST_LOG_PTR;

volatile _mqx_uint Saved_seconds[BIG_LOG_ENTRIES];
volatile _mqx_uint Saved_microseconds[BIG_LOG_ENTRIES];

volatile bool  ISR_test;
_mqx_uint         ISR_test_result;
void             *Old_ISR_data;
void              (_CODE_PTR_ Old_ISR)(void *);


_mqx_uint         i;


//-----------------------------------------------------------------------------
// Test suite function prototipe
void tc_1_main_task(void);// Test #1: Test Auto-component creation....
void tc_2_main_task(void);// Test #2: Invalid use of log before it is created....
void tc_3_main_task(void);// Test #3: Test log creation and destruction....
void tc_4_main_task(void);// Test #4: Test log read types....
void tc_5_main_task(void);// Test #5: Test log types (OVERWRITE or not)....
void tc_6_main_task(void);// Test #6: Test log reset....
void tc_7_main_task(void);// Test #7: Test log enable/disable....
void tc_8_main_task(void);// Test #8: ISR Test....
void tc_9_main_task(void);// Test #9: Variable sized read and write....
//------------------------------------------------------------------------------

/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : log_test_remove_component
* Returned Value   : bool result
* Comments         :
*
*  This function is used to remove the log component. It deallocates the log 
* component data, and returns the memory to MQX.
*
*END*-------------------------------------------------------------------------*/

void log_test_remove_component
   (
      void
   )
{ /* Body */
   KERNEL_DATA_STRUCT_PTR mqx_ptr;

   mqx_ptr = _mqx_get_kernel_data();
   _mem_free(mqx_ptr->KERNEL_COMPONENTS[KERNEL_LOG]);
   mqx_ptr->KERNEL_COMPONENTS[KERNEL_LOG] = NULL;

} /* Endbody */


/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : log_test_fn
* Returned Value   : bool result
* Comments         :
*
*  This function is used to compare the result of a function call with the
* desired result, and print an error if they don't match.
*
*  This function also calls  the _log_test function to ensure that the 
* name component is not corrupt.
*
*  This function returns FALSE if an error was detected, otherwise it returns 
* the previous result.
*
*END*-------------------------------------------------------------------------*/
#if 0
void log_test_fn
   (
      _mqx_uint   test,
      _mqx_uint   sub_test,
      _mqx_uint     result,
      _mqx_uint     desired_result
   )
{ /* Body */
   _mqx_uint p1;
   
   if (result != desired_result) {
      ktest_error("\nTest #%d.%d Failed", test, sub_test);
   } /* Endif */
   if (_log_test(&p1) != MQX_OK) {
      ktest_error("\nTest #%d.%d _log_test Failed, error_ptr = %x", 
         test, sub_test, p1);
   } /* Endif */

} /* Endbody */
#endif

/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : log_test_entry
* Returned Value   : bool result
* Comments         :
*
*  This function is used to compare a log entry with the expected log entry. 
*
*END*-------------------------------------------------------------------------*/

bool log_test_entry
(
   _mqx_uint      log,
   TEST_LOG_PTR   e_ptr,
   _mqx_uint      value,
   _mqx_uint      sequence
)
{ /* Body */
   _mqx_uint  microseconds;
   bool  result = PASS;
   
   if (e_ptr->HEADER.SIZE != TEST_LOG_ENTRY_SIZE) {
      //ktest_error("\n Error, log entry size incorrect");
      result = FAIL;
   } /* Endif */
   
   if (e_ptr->HEADER.SEQUENCE_NUMBER != (sequence + 1)) {
      //ktest_error("\n Error, log entry sequence incorrect");
      result = FAIL;
   } /* Endif */

   microseconds =   (((_mqx_uint)e_ptr->HEADER.SECONDS * 1000) +
                      (_mqx_uint)e_ptr->HEADER.MILLISECONDS)* 1000 + 
                      (_mqx_uint)e_ptr->HEADER.MICROSECONDS;
   if (e_ptr->HEADER.SECONDS < Saved_seconds[log]) {
      //ktest_error("\n Error, log entry seconds incorrect");
      result = FAIL;
   } else if (e_ptr->HEADER.SECONDS == Saved_seconds[log]) {
      if (microseconds < Saved_microseconds[log]) {
         //ktest_error("\n 1. Error, log entry milli/microseconds incorrect");
         //printf("\n    Log #                 = %d", log);
         //printf("\n    microseconds from log = %ld", microseconds);
         //printf("\n    microseconds saved    = %ld", Saved_microseconds[log]);
         result = FAIL;
      } /* Endif */
   } /* Endif */
   if (e_ptr->DATA[0] != log) {
      //ktest_error("\n Error, log entry data[0] incorrect");
      result = FAIL;
   } /* Endif */
   
   if (e_ptr->DATA[1] != value) {
      //ktest_error("\n Error, log entry data[1] incorrect");
      result = FAIL;
   } /* Endif */
   
   Saved_microseconds[log] = microseconds;
   Saved_seconds[log] = e_ptr->HEADER.SECONDS;
   
   return result;

} /* Endbody */


/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : log_test_entry2
* Returned Value   : bool result
* Comments         :
*
*  This function is used to compare a log entry with the expected log entry. 
*
*END*-------------------------------------------------------------------------*/

bool log_test_entry2
(
   TEST_LOG_PTR   e_ptr,
   _mqx_uint      index,
   _mqx_uint      read_size,
   _mqx_uint      write_size
)
{ /* Body */
   _mqx_uint    microseconds;
   _mqx_uint  i, count = 0;
   bool    result = PASS;
   
/* printf("\nindex = %d, read = %d, write = %d", index, read_size, write_size); */

   if ((e_ptr->HEADER.SIZE - LOG_HEADER_SIZE) != index) {
      //ktest_error("\n Error, log entry size incorrect");
      result = FAIL;
   } /* Endif */

#if 0
   if (e_ptr->HEADER.SEQUENCE_NUMBER != (Saved_sequence[log] + 1)) {
      //ktest_error("\n Error, log entry sequence incorrect");
      result = FAIL;
   } /* Endif */
#endif

   microseconds = ((_mqx_uint)e_ptr->HEADER.MILLISECONDS * 1000) + 
      (_mqx_uint)e_ptr->HEADER.MICROSECONDS;

   if (e_ptr->HEADER.SECONDS < Saved_seconds[0]) {
      //ktest_error("\n Error, log entry seconds incorrect");
      result = FAIL;
   } else if (e_ptr->HEADER.SECONDS == Saved_seconds[0]) {
      if (microseconds < Saved_microseconds[0]) {
         //ktest_error("\n 2. Error, log entry milli/microseconds incorrect");
         result = FAIL;
      } /* Endif */
   } /* Endif */
   
   if (write_size < read_size) {
      count = write_size;
   } else {
      count = read_size;
   } /* Endif */
   
   for (i = 0; i < count; i++) {
      if (e_ptr->DATA[i] != (index * 10) + i + 1) {
         //ktest_error("\n Error, log entry data[%d] incorrect", i);
         result = FAIL;
      } /* Endif */
   } /* Endfor */
                           
   Saved_microseconds[0] = microseconds;
   Saved_seconds[0] = e_ptr->HEADER.SECONDS;
   
   return result;

} /* Endbody */


/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : log_test_ISR
* Returned Value   : void 
* Comments         :
*
*  This function is used to test calling log functions from an ISR.  This 
* function executes when a tick interrupt occurs, and attempts to call 
* the log component functions.
*
*END*-------------------------------------------------------------------------*/

TEST_LOG_STRUCT isr_entry;

void log_test_ISR(void *user_isr_ptr)
{ /* Body */
   _mqx_uint    result;
   _mqx_uint    p1;

   /* Chain to previous notifier */
   (*Old_ISR)(Old_ISR_data);
   
   if (ISR_test) {
      result = _log_disable(1);
      EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == MQX_OK), "Test #8 Testing: 8.2: Stop logging the log")
      EU_ASSERT_REF_TC_MSG(tc_8_main_task, (_log_test(&p1) == MQX_OK), "Test #8 Testing: 8.2: Test the log component operation")
      
      result = _log_enable(1);
      EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "Test #8 Testing: 8.3: Start logging the log")
      EU_ASSERT_REF_TC_MSG(tc_8_main_task, (_log_test(&p1) == MQX_OK), "Test #8 Testing: 8.3: Test the log component operation")
      
      result = _log_reset(1);
      EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "Test #8 Testing: 8.4: Reset the log")
      EU_ASSERT_REF_TC_MSG(tc_8_main_task, (_log_test(&p1) == MQX_OK), "Test #8 Testing: 8.4: Test the log component operation")
      
      /* PLEASE NOTE: if the log number is changed in these tests, then the third arguement 
         in the following call to _log_write MUST BE THE SAME AS THE LOG NUMBER! This is 
         checked in another test and 8.7 will fail if this is not correct */   
      result = _log_write(1, 2, 1, TEST_VALUE);
      EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "Test #8 Testing: 8.5: Write to the log")
      EU_ASSERT_REF_TC_MSG(tc_8_main_task, (_log_test(&p1) == MQX_OK), "Test #8 Testing: 8.5: Test the log component operation")
      
      result = _log_read(1, LOG_READ_NEWEST, 2, (LOG_ENTRY_STRUCT_PTR)&isr_entry);
      EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "Test #8 Testing: 8.6: Read the log")
      EU_ASSERT_REF_TC_MSG(tc_8_main_task, (_log_test(&p1) == MQX_OK), "Test #8 Testing: 8.6: Test the log component operation")
      
      result = log_test_entry(1, &isr_entry, TEST_VALUE, 0);
      EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == TRUE, "Test #8 Testing: 8.7: Test log entry")
      EU_ASSERT_REF_TC_MSG(tc_8_main_task, (_log_test(&p1) == MQX_OK), "Test #8 Testing: 8.7: Test the log component operation")
      
      ISR_test = FALSE;
   } /* Endif */

} /* Endbody */

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Test Auto-component creation.
*
*END*---------------------------------------------------------------------*/

void tc_1_main_task(void)
{
    _mqx_uint           result;
    _mqx_uint           p1;
    LOG_ENTRY_STRUCT    log_entry;

    result = _log_disable(0);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_COMPONENT_DOES_NOT_EXIST, "Test #1 Testing: 1.1: Stop logging to the log")
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (_log_test(&p1) == MQX_OK), "Test #1 Testing: 1.1: Test the log component operation")

    result = _log_enable(0);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_COMPONENT_DOES_NOT_EXIST, "Test #1 Testing: 1.2: Start logging to the log")
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (_log_test(&p1) == MQX_OK), "Test #1 Testing: 1.2: Test the log component operation")

    result = _log_destroy(0);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_COMPONENT_DOES_NOT_EXIST, "Test #1 Testing: 1.3: Destroy the log")
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (_log_test(&p1) == MQX_OK), "Test #1 Testing: 1.3: Test the log component operation")

    result = _log_write(0, 0, 0);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_COMPONENT_DOES_NOT_EXIST, "Test #1 Testing: 1.4: Write to the log")
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (_log_test(&p1) == MQX_OK), "Test #1 Testing: 1.4: Test the log component operation")

    result = _log_reset(0);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_COMPONENT_DOES_NOT_EXIST, "Test #1 Testing: 1.5: Reset the log to its initial state")
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (_log_test(&p1) == MQX_OK), "Test #1 Testing: 1.5: Test the log component operation")

    result = _log_read(0, 0, 0, &log_entry);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_COMPONENT_DOES_NOT_EXIST, "Test #1 Testing: 1.6: Read the information in the log")
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (_log_test(&p1) == MQX_OK), "Test #1 Testing: 1.6: Test the log component operation")

    result = _log_create(0, TEST_LOG_SIZE, 0);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1 Testing: 1.7: Create the log")
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (_log_test(&p1) == MQX_OK), "Test #1 Testing: 1.7: Test the log component operation")

    result = _log_create(0, TEST_LOG_SIZE, 0);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == LOG_EXISTS, "Test #1 Testing: 1.8: Create the log")
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (_log_test(&p1) == MQX_OK), "Test #1 Testing: 1.8: Test the log component operation")

    result = _log_destroy(0);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1 Testing: 1.9: Destroy the log")
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (_log_test(&p1) == MQX_OK), "Test #1 Testing: 1.9: Test the log component operation")

    log_test_remove_component();

    result = _log_disable(0);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_COMPONENT_DOES_NOT_EXIST, "Test #1 Testing: 1.10: Stop logging to the log")
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (_log_test(&p1) == MQX_OK), "Test #1 Testing: 1.10: Test the log component operation")

    result = _log_create_component();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1 Testing: 1.11: Create the log component")
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (_log_test(&p1) == MQX_OK), "Test #1 Testing: 1.11: Test the log component operation")

    result = _log_create_component();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1 Testing: 1.12: Create the log component")
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (_log_test(&p1) == MQX_OK), "Test #1 Testing: 1.12: Test the log component operation")
}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_2_main_task
* Comments     : TEST #2: Invalid use of log before it is created.
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    _mqx_uint           result;
    _mqx_uint           p1;
    LOG_ENTRY_STRUCT    log_entry;
      
    result = _log_disable(1);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == LOG_DOES_NOT_EXIST, "Test #2 Testing: 2.1: Stop logging to the log")
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (_log_test(&p1) == MQX_OK), "Test #2 Testing: 2.1: Test the log component operation")

    result = _log_enable(1);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == LOG_DOES_NOT_EXIST, "Test #2 Testing: 2.2: Start logging to the log")
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (_log_test(&p1) == MQX_OK), "Test #2 Testing: 2.2: Test the log component operation")

    result = _log_destroy(1);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == LOG_DOES_NOT_EXIST, "Test #2 Testing: 2.3: Destroy the log")
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (_log_test(&p1) == MQX_OK), "Test #2 Testing: 2.3: Test the log component operation")

    result = _log_write(1, 0, 0);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == LOG_DOES_NOT_EXIST, "Test #2 Testing: 2.4: Write to the log")
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (_log_test(&p1) == MQX_OK), "Test #2 Testing: 2.4: Test the log component operation")

    result = _log_reset(1);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == LOG_DOES_NOT_EXIST, "Test #2 Testing: 2.5: Reset the log to its initial state")
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (_log_test(&p1) == MQX_OK), "Test #2 Testing: 2.5: Test the log component operation")

    result = _log_read(1, 0, 0, &log_entry);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == LOG_DOES_NOT_EXIST, "Test #2 Testing: 2.6: Read the information in the log")
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (_log_test(&p1) == MQX_OK), "Test #2 Testing: 2.6: Test the log component operation")
}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_3_main_task
* Comments     : TEST #3: Test log creation & destruction.
*
*END*---------------------------------------------------------------------*/

void tc_3_main_task(void)
{
    _mqx_uint   result;
    _mqx_uint   p1;
    _mqx_uint   log;
    _mqx_uint   hi_mem;

    for (log = 1; log < LOG_MAXIMUM_NUMBER; log++) {
        result = _log_create(log, TEST_LOG_SIZE, 0);
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3 Testing: 3.1: Create the log")
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, (_log_test(&p1) == MQX_OK), "Test #3 Testing: 3.1: Test the log component operation")
    } /* Endfor */
    
    result = _log_create(LOG_MAXIMUM_NUMBER, TEST_LOG_SIZE, 0);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == LOG_INVALID, "Test #3 Testing: 3.2: Create the log should have returned LOG_INVALID")
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (_log_test(&p1) == MQX_OK), "Test #3 Testing: 3.2: Test the log component operation")
    
    /* Save our current highwater */
    hi_mem = (_mqx_uint)_mem_get_highwater();
    for (log = 1; log < LOG_MAXIMUM_NUMBER; log++) {;
        result = _log_destroy(log);
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3 Testing: 3.3: Destroy the log")
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, (_log_test(&p1) == MQX_OK), "Test #3 Testing: 3.3: Test the log component operation")
    } /* Endfor */

    result = _log_destroy(LOG_MAXIMUM_NUMBER);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == LOG_INVALID, "Test #3 Testing: 3.4: Destroy the log should have returned LOG_INVALID")
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (_log_test(&p1) == MQX_OK), "Test #3 Testing: 3.4: Test the log component operation")
    
    for (log = 1; log < LOG_MAXIMUM_NUMBER; log++) {
        result = _log_destroy(log);
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == LOG_DOES_NOT_EXIST, "Test #3 Testing: 3.5: Destroy the log should have returned LOG_DOES_NOT_EXIST")
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, (_log_test(&p1) == MQX_OK), "Test #3 Testing: 3.5: Test the log component operation")
    } /* Endfor */

    for (log = 1; log < LOG_MAXIMUM_NUMBER; log++) {
        result = _log_create(log, TEST_LOG_SIZE, 0);
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3 Testing: 3.6: Create thr log")
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, (_log_test(&p1) == MQX_OK), "Test #3 Testing: 3.6: Test the log component operation")
    } /* Endfor */

    /* Check our current highwater */
    result = (_mqx_uint)_mem_get_highwater();
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == hi_mem, "Test #3 Testing: 3.7: Get the highest memory address")
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (_log_test(&p1) == MQX_OK), "Test #3 Testing: 3.7: Test the log component operation")

    for (log = 1; log < LOG_MAXIMUM_NUMBER; log++) {
        result = _log_create(log, TEST_LOG_SIZE, 0);
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == LOG_EXISTS, "Test #3 Testing: 3.8: Create the log should have returned LOG_EXISTS")
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, (_log_test(&p1) == MQX_OK), "Test #3 Testing: 3.8: Test the log component operation")
   } /* Endfor */
}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_4_main_task
* Comments     : TEST #4: Test log read types.
*
*END*---------------------------------------------------------------------*/

void tc_4_main_task(void)
{
    _mqx_uint       result;
    _mqx_uint       p1;
    TEST_LOG_STRUCT entry;
    _mqx_uint       log;
    _mqx_uint       read_type;

    _mem_zero((void *)&Saved_seconds[0], sizeof(Saved_seconds));
    _mem_zero((void *)&Saved_microseconds[0], sizeof(Saved_microseconds));

    /* Test reading empty log */
    read_type = LOG_READ_NEWEST;

    for (log = 1; log < LOG_MAXIMUM_NUMBER; log++) {
        result = _log_read(log, read_type, 1, (LOG_ENTRY_STRUCT_PTR)&entry);
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == LOG_ENTRY_NOT_AVAILABLE, "Test #4 Testing: 4.1: Read the log should have returned LOG_ENTRY_NOT_AVAILABLE")
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (_log_test(&p1) == MQX_OK), "Test #4 Testing: 4.1: Test the log component operation")
    } /* Endfor */

    result = _log_write(log, 2, log, 2 * log);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == LOG_INVALID, "Test #4 Testing: 4.2: Write to the log should have returned LOG_INVALID")
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (_log_test(&p1) == MQX_OK), "Test #4 Testing: 4.2: Test the log component operation")

    /* Write TEST_LOG_ENTRIES entries to each log */
    for (log = 1; log < LOG_MAXIMUM_NUMBER; log++) {
        for (i = 0; i < TEST_LOG_ENTRIES; i++) {
            result = _log_write(log, 2, log, i);
            EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4 Testing: 4.3: Write to the log")
            EU_ASSERT_REF_TC_MSG(tc_4_main_task, (_log_test(&p1) == MQX_OK), "Test #4 Testing: 4.3: Test the log component operation")
        } /* Endfor */
    } /* Endfor */
    result = _log_write(log, 2, log, log);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == LOG_INVALID, "Test #4 Testing: 4.4: Write to the log should have returned LOG_INVALID")
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (_log_test(&p1) == MQX_OK), "Test #4 Testing: 4.4: Test the log component operation")

    /* Read next should fail, since read oldest not done */
    read_type = LOG_READ_NEXT;
    for (log = 1; log < LOG_MAXIMUM_NUMBER; log++) {
        result = _log_read(log, read_type, 2, (LOG_ENTRY_STRUCT_PTR)&entry);
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == LOG_ENTRY_NOT_AVAILABLE, "Test #4 Testing: 4.5: Read the log should have returned LOG_ENTRY_NOT_AVAILABLE")
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (_log_test(&p1) == MQX_OK), "Test #4 Testing: 4.5: Test the log component operation")
    } /* Endfor */

    /* Now read newest */
    read_type = LOG_READ_NEWEST;
    for (log = 1; log < LOG_MAXIMUM_NUMBER; log++) {
        result = _log_read(log, read_type, 2, (LOG_ENTRY_STRUCT_PTR)&entry);
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4 Testing: 4.6: Read the log")
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (_log_test(&p1) == MQX_OK), "Test #4 Testing: 4.6: Test the log component operation")

        result = log_test_entry(log, &entry, 4, 4);
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == TRUE, "Test #4 Testing: 4.7: Test the log")
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (_log_test(&p1) == MQX_OK), "Test #4 Testing: 4.7: Test the log component operation")
    } /* Endfor */

    _mem_zero((void *)&Saved_seconds[0], sizeof(Saved_seconds));
    _mem_zero((void *)&Saved_microseconds[0], sizeof(Saved_microseconds));

    /* Now read oldest */
    read_type = LOG_READ_OLDEST;
    for (log = 1; log < LOG_MAXIMUM_NUMBER; log++) {
        result = _log_read(log, read_type, 2, (LOG_ENTRY_STRUCT_PTR)&entry);
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4 Testing: 4.8: Read the log")
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (_log_test(&p1) == MQX_OK), "Test #4 Testing: 4.8: Test the log component operation")

        result = log_test_entry(log, &entry, 0, 0);
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == TRUE, "Test #4 Testing: 4.9: Test log entry")
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (_log_test(&p1) == MQX_OK), "Test #4 Testing: 4.9: Test the log component operation")
    } /* Endfor */

    /* Now read next */
    read_type = LOG_READ_NEXT;
    for (log = 1; log < LOG_MAXIMUM_NUMBER; log++) {
        for (i = 1; i < TEST_LOG_ENTRIES; i++) {
            result = _log_read(log, read_type, 2, (LOG_ENTRY_STRUCT_PTR)&entry);
            EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4 Testing: 4.10: Read the log")
            EU_ASSERT_REF_TC_MSG(tc_4_main_task, (_log_test(&p1) == MQX_OK), "Test #4 Testing: 4.10: Test the log component operation")

            result = log_test_entry(log, &entry, i, i);
            EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == TRUE, "Test #4 Testing: 4.11: Test log entry")
            EU_ASSERT_REF_TC_MSG(tc_4_main_task, (_log_test(&p1) == MQX_OK), "Test #4 Testing: 4.11: Test the log component operation")
        } /* Endfor*/
    } /* Endfor */

    _mem_zero((void *)&Saved_seconds[0], sizeof(Saved_seconds));
    _mem_zero((void *)&Saved_microseconds[0], sizeof(Saved_microseconds));

    /* Now read oldest & delete */
    read_type = LOG_READ_OLDEST_AND_DELETE;
    for (log = 1; log < LOG_MAXIMUM_NUMBER; log++) {
        for (i = 0; i < TEST_LOG_ENTRIES; i++) {
            result = _log_read(log, read_type, 2, (LOG_ENTRY_STRUCT_PTR)&entry);
            EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4 Testing: 4.12: Read the log")
            EU_ASSERT_REF_TC_MSG(tc_4_main_task, (_log_test(&p1) == MQX_OK), "Test #4 Testing: 4.12: Test the log component operation")

            result = log_test_entry(log, &entry, i, i);
            EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == TRUE, "Test #4 Testing: 4.13: Test log entry")
            EU_ASSERT_REF_TC_MSG(tc_4_main_task, (_log_test(&p1) == MQX_OK), "Test #4 Testing: 4.13: Test the log component operation")
        } /* Endfor */
    } /* Endfor */

    /* Make sure log is empty */
    read_type = LOG_READ_NEWEST;
    for (log = 1; log < LOG_MAXIMUM_NUMBER; log++) {
        result = _log_read(log, read_type, 1, (LOG_ENTRY_STRUCT_PTR)&entry);
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == LOG_ENTRY_NOT_AVAILABLE, "Test #4 Testing: 4.14: Read the log should have returned LOG_ENTRY_NOT_AVAILABLE")
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (_log_test(&p1) == MQX_OK), "Test #4 Testing: 4.14: Test the log component operation")
    } /* Endfor */
}


/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_5_main_task
* Comments     : TEST #5: Test log types (OVERWRITE or not).
*
*END*---------------------------------------------------------------------*/

void tc_5_main_task(void)
{
    _mqx_uint       result;
    _mqx_uint       p1;
    TEST_LOG_STRUCT entry;
    _mqx_uint       log;
    _mqx_uint       read_type;

    /* Write TEST_LOG_ENTRIES entries to each log */
    for (log = 1; log < LOG_MAXIMUM_NUMBER; log++) {
        for (i = 0; i < TEST_LOG_ENTRIES; i++) {
            result = _log_write(log, 2, log, i);
            EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "Test #5 Testing: 5.1: Write TEST_LOG_ENTRIES entries to the log")
            EU_ASSERT_REF_TC_MSG(tc_5_main_task, (_log_test(&p1) == MQX_OK), "Test #5 Testing: 5.1: Test the log component operation")
        } /* Endfor */
    } /* Endfor */

    /* Try to write a 6th entry to each log */
    for (log = 1; log < LOG_MAXIMUM_NUMBER; log++) {
        result = _log_write(log, 2, log, TEST_LOG_ENTRIES);
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == LOG_FULL, "Test #5 Testing: 5.2: Try to write a 6th entry to the log should have returned LOG_FULL")
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (_log_test(&p1) == MQX_OK), "Test #5 Testing: 5.2: Test the log component operation")
    } /* Endfor */

    /* Now read oldest & delete */
    read_type = LOG_READ_OLDEST_AND_DELETE;
    for (log = 1; log < LOG_MAXIMUM_NUMBER; log++) {
        for (i = 0; i < TEST_LOG_ENTRIES; i++) {
            result = _log_read(log, read_type, 2, (LOG_ENTRY_STRUCT_PTR)&entry);
            EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "Test #5 Testing: 5.3: Read the log")
            EU_ASSERT_REF_TC_MSG(tc_5_main_task, (_log_test(&p1) == MQX_OK), "Test #5 Testing: 5.3: Test the log component operation")

            result = log_test_entry(log, &entry, i, (TEST_LOG_ENTRIES + i));
            EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == TRUE, "Test #5 Testing: 5.4: Test log entry")
            EU_ASSERT_REF_TC_MSG(tc_5_main_task, (_log_test(&p1) == MQX_OK), "Test #5 Testing: 5.4: Test the log component operation")
        } /* Endfor */
    } /* Endfor */

    /* Make sure log is empty */
    read_type = LOG_READ_NEWEST;
    for (log = 1; log < LOG_MAXIMUM_NUMBER; log++) {
        result = _log_read(log, read_type, 1, (LOG_ENTRY_STRUCT_PTR)&entry);
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == LOG_ENTRY_NOT_AVAILABLE, "Test #5 Testing: 5.5: Read the log should have returned LOG_ENTRY_NOT_AVAILABLE")
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (_log_test(&p1) == MQX_OK), "Test #5 Testing: 5.5: Test the log component operation")
    } /* Endfor */

    /* Destroy & recreate with overwrite */
                              
    for (log = 1; log < LOG_MAXIMUM_NUMBER; log++) {
        result = _log_destroy(log);
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "Test #5 Testing: 5.6: Destroy the log")
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (_log_test(&p1) == MQX_OK), "Test #5 Testing: 5.6: Test the log component operation")

        result = _log_create(log, TEST_LOG_SIZE, LOG_OVERWRITE);
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "Test #5 Testing: 5.7: Recreate the log")
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (_log_test(&p1) == MQX_OK), "Test #5 Testing: 5.7: Test the log component operation")
    } /* Endfor */

    /* Write 2*TEST_LOG_ENTRIES entries to each log */
    for (log = 1; log < LOG_MAXIMUM_NUMBER; log++) {
        for (i = 0; i < (2 * TEST_LOG_ENTRIES); i++) {
            result = _log_write(log, 2, log, i);
            EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "Test #5 Testing: 5.8: Write to the log")
            EU_ASSERT_REF_TC_MSG(tc_5_main_task, (_log_test(&p1) == MQX_OK), "Test #5 Testing: 5.8: Test the log component operation")
        } /* Endfor */
    } /* Endfor */

    /* Now read oldest & delete */
    read_type = LOG_READ_OLDEST_AND_DELETE;
    for (log = 1; log < LOG_MAXIMUM_NUMBER; log++) {
        for (i = 0; i < TEST_LOG_ENTRIES; i++) {
            result = _log_read(log, read_type, 2, (LOG_ENTRY_STRUCT_PTR)&entry);
            EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "Test #5 Testing: 5.9: Read the log")
            EU_ASSERT_REF_TC_MSG(tc_5_main_task, (_log_test(&p1) == MQX_OK), "Test #5 Testing: 5.9: Test the log component operation")

            result = log_test_entry(log, &entry, (TEST_LOG_ENTRIES + i), (TEST_LOG_ENTRIES + i));
            EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == TRUE, "Test #5 Testing: 5.10: Test log entry")
            EU_ASSERT_REF_TC_MSG(tc_5_main_task, (_log_test(&p1) == MQX_OK), "Test #5 Testing: 5.10: Test the log component operation")
        } /* Endfor */
    } /* Endfor */

    /* Make sure log is empty */
    read_type = LOG_READ_NEWEST;
    for (log = 1; log < LOG_MAXIMUM_NUMBER; log++) {
        result = _log_read(log, read_type, 1, (LOG_ENTRY_STRUCT_PTR)&entry);
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == LOG_ENTRY_NOT_AVAILABLE, "Test #5 Testing: 5.11: Read the empty log should have returned LOG_ENTRY_NOT_AVAILABLE")
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (_log_test(&p1) == MQX_OK), "Test #5 Testing: 5.11: Test the log component operation")
    } /* Endfor */
}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_6_main_task
* Comments     : TEST #6: Test log reset.
*
*END*---------------------------------------------------------------------*/

void tc_6_main_task(void)
{
    _mqx_uint       result;
    _mqx_uint       p1;
    TEST_LOG_STRUCT entry;
    _mqx_uint       log;
    _mqx_uint       read_type;

    _mem_zero((void *)&Saved_seconds[0], sizeof(Saved_seconds));
    _mem_zero((void *)&Saved_microseconds[0], sizeof(Saved_microseconds));

    /* Write an entry to each log */
    for (log = 1; log < LOG_MAXIMUM_NUMBER; log++) {
        result = _log_write(log, 2, log, i);
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6 Testing: 6.1: Write to the log")
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (_log_test(&p1) == MQX_OK), "Test #6 Testing: 6.1: Test the log component operation")
    } /* Endfor */

    /* reset the log */
    for (log = 1; log < LOG_MAXIMUM_NUMBER; log++) {
        result = _log_reset(log);
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6 Testing: 6.2: Reset the log")
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (_log_test(&p1) == MQX_OK), "Test #6 Testing: 6.2: Test the log component operation")
    } /* Endfor */

    result = _log_reset(LOG_MAXIMUM_NUMBER);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == LOG_INVALID, "Test #6 Testing: 6.3: Reset log:LOG_MAXIMUM_NUMBER should have returned LOG_INVALID")
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (_log_test(&p1) == MQX_OK), "Test #6 Testing: 6.3: Test the log component operation")

    /* Make sure log is empty */
    read_type = LOG_READ_NEWEST;
    for (log = 1; log < LOG_MAXIMUM_NUMBER; log++) {
        result = _log_read(log, read_type, 1, (LOG_ENTRY_STRUCT_PTR)&entry);
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == LOG_ENTRY_NOT_AVAILABLE, "Test #6 Testing: 6.4: Read the empty log should have returned LOG_ENTRY_NOT_AVAILABLE")
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (_log_test(&p1) == MQX_OK), "Test #6 Testing: 6.4: Test the log component operation")
    } /* Endfor */

    for (log = 1; log < LOG_MAXIMUM_NUMBER; log++) {
        result = _log_write(log, 2, log, 0);
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6 Testing: 6.5: Write to the log")
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (_log_test(&p1) == MQX_OK), "Test #6 Testing: 6.5: Test the log component operation")

        result = _log_write(log, 2, log, 1);
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6 Testing: 6.6: Write to the log")
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (_log_test(&p1) == MQX_OK), "Test #6 Testing: 6.6: Test the log component operation")

        result = _log_disable(log);
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6 Testing: 6.7: Stop logging the log")
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (_log_test(&p1) == MQX_OK), "Test #6 Testing: 6.7: Test the log component operation")

        result = _log_read(log, LOG_READ_OLDEST_AND_DELETE, 2, (LOG_ENTRY_STRUCT_PTR)&entry);
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6 Testing: 6.8: Read the log")
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (_log_test(&p1) == MQX_OK), "Test #6 Testing: 6.8: Test the log component operation")

        result = log_test_entry(log, &entry, 0, 0);
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == TRUE, "Test #6 Testing: 6.9: Test log entry")
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (_log_test(&p1) == MQX_OK), "Test #6 Testing: 6.9: Test the log component operation")

        result = _log_read(log, LOG_READ_OLDEST_AND_DELETE, 2, (LOG_ENTRY_STRUCT_PTR)&entry);
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6 Testing: 6.10: Read the log")
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (_log_test(&p1) == MQX_OK), "Test #6 Testing: 6.10: Test the log component operation")

        result = log_test_entry(log, &entry, 1, 1);
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == TRUE, "Test #6 Testing: 6.11: Test log entry")
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (_log_test(&p1) == MQX_OK), "Test #6 Testing: 6.11: Test the log component operation")

        result = _log_enable(log);
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6 Testing: 6.12: Start logging the log")
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (_log_test(&p1) == MQX_OK), "Test #6 Testing: 6.12: Test the log component operation")

        result = _log_reset(log);
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6 Testing: 6.13: Reset the log")
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (_log_test(&p1) == MQX_OK), "Test #6 Testing: 6.13: Test the log component operation")
    } /* Endfor */
}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_7_main_task
* Comments     : TEST #7: Test log enable/disable.
*
*END*---------------------------------------------------------------------*/

void tc_7_main_task(void)
{
    _mqx_uint       result;
    _mqx_uint       p1;
    TEST_LOG_STRUCT entry;
    _mqx_uint       log;
    _mqx_uint       read_type;

    read_type = LOG_READ_NEWEST;
    for (log = 1; log < LOG_MAXIMUM_NUMBER; log++) {
        result = _log_write(log, 2, log, 0);
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "Test #7 Testing: 7.1: Write to the log")
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, (_log_test(&p1) == MQX_OK), "Test #7 Testing: 7.1: Test the log component operation")

        result = _log_disable(log);
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "Test #7 Testing: 7.2: Stop logging the log")
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, (_log_test(&p1) == MQX_OK), "Test #7 Testing: 7.2: Test the log component operation")

        result = _log_write(log, 2, log, 1);
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == LOG_DISABLED, "Test #7 Testing: 7.3: Write to the log should have returned LOG_DISABLED")
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, (_log_test(&p1) == MQX_OK), "Test #7 Testing: 7.3: Test the log component operation")

        result = _log_enable(log);
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "Test #7 Testing: 7.4: Start logging the log")
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, (_log_test(&p1) == MQX_OK), "Test #7 Testing: 7.4: Test the log component operation")

        result = _log_write(log, 2, log, 2);
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "Test #7 Testing: 7.5: Write to the log should have returned MQX_OK")
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, (_log_test(&p1) == MQX_OK), "Test #7 Testing: 7.5: Test the log component operation")

        result = _log_disable(log);
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "Test #7 Testing: 7.6: Stop logging the log")
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, (_log_test(&p1) == MQX_OK), "Test #7 Testing: 7.6: Test the log component operation")

        result = _log_read(log, LOG_READ_OLDEST_AND_DELETE, 2, (LOG_ENTRY_STRUCT_PTR)&entry);
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "Test #7 Testing: 7.7: Read the log")
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, (_log_test(&p1) == MQX_OK), "Test #7 Testing: 7.7: Test the log component operation")

        result = log_test_entry(log, &entry, 0, 0);
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == TRUE, "Test #7 Testing: 7.8: Test log entry")
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, (_log_test(&p1) == MQX_OK), "Test #7 Testing: 7.8: Test the log component operation")

        result = _log_enable(log);
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "Test #7 Testing: 7.9: Start logging the log")
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, (_log_test(&p1) == MQX_OK), "Test #7 Testing: 7.9: Test the log component operation")

        result = _log_read(log, LOG_READ_OLDEST_AND_DELETE, 2, (LOG_ENTRY_STRUCT_PTR)&entry);
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "Test #7 Testing: 7.10: Read the log")
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, (_log_test(&p1) == MQX_OK), "Test #7 Testing: 7.10: Test the log component operation")

        result = log_test_entry(log, &entry, 2, 1);
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == TRUE, "Test #7 Testing: 7.11: Test log entry")
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, (_log_test(&p1) == MQX_OK), "Test #7 Testing: 7.11: Test the log component operation")

        result = _log_reset(log);
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "Test #7 Testing: 7.12: Reset the log")
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, (_log_test(&p1) == MQX_OK), "Test #7 Testing: 7.12: Test the log component operation")

        result = _log_destroy(log);
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "Test #7 Testing: 7.13: Destroy the log")
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, (_log_test(&p1) == MQX_OK), "Test #7 Testing: 7.13: Test the log component operation")
    }
}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_8_main_task
* Comments     : TEST #8: ISR Test
*
*END*---------------------------------------------------------------------*/

void tc_8_main_task(void)
{
    _mqx_uint   result;
    _mqx_uint   p1;

    result = _log_create(1, TEST_LOG_SIZE, 0);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "Test #8 Testing: 8.1: Create the log")
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, (_log_test(&p1) == MQX_OK), "Test #8 Testing: 8.1: Test the log component operation")

    Old_ISR_data = _int_get_isr_data(BSP_TIMER_INTERRUPT_VECTOR);
    Old_ISR      = _int_get_isr(BSP_TIMER_INTERRUPT_VECTOR);

    ISR_test = TRUE;
    _int_install_isr(BSP_TIMER_INTERRUPT_VECTOR, log_test_ISR, 0);

    _time_delay(_time_get_resolution() * 2);

    _int_install_isr(BSP_TIMER_INTERRUPT_VECTOR, Old_ISR, Old_ISR_data);

    result = _log_destroy(1);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "Test #8 Testing: 8.8: Destroy the log")
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, (_log_test(&p1) == MQX_OK), "Test #8 Testing: 8.8: Test the log component operation")
}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_9_main_task
* Comments     : TEST #9: Variable sized read & write
*
*END*---------------------------------------------------------------------*/

void tc_9_main_task(void)
{
    _mqx_uint       result;
    _mqx_uint       p1;
    TEST_LOG_STRUCT entry;

    _mem_zero( (void *)&Saved_seconds[0], sizeof(Saved_seconds));
    _mem_zero( (void *)&Saved_microseconds[0], sizeof(Saved_microseconds));

    result = _log_disable(1);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == LOG_DOES_NOT_EXIST, "Test #9 Testing: 9.1: Stop logging the log should have returned LOG_DOES_NOT_EXIST")
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (_log_test(&p1) == MQX_OK), "Test #9 Testing: 9.1: Test the log component operation")

    result = _log_create(1, BIG_LOG_SIZE, 0);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "Test #9 Testing: 9.2: Create the log")
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (_log_test(&p1) == MQX_OK), "Test #9 Testing: 9.2: Test the log component operation")

    result = _log_write(1, 0);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "Test #9 Testing: 9.3: Write to the log with none parameter")
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (_log_test(&p1) == MQX_OK), "Test #9 Testing: 9.3: Test the log component operation")

    result = _log_write(1, 1, 11);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "Test #9 Testing: 9.4: Write to the log with 1 parameter")
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (_log_test(&p1) == MQX_OK), "Test #9 Testing: 9.4: Test the log component operation")

    result = _log_write(1, 2, 21, 22);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "Test #9 Testing: 9.5: Write to the log with 2 parameters")
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (_log_test(&p1) == MQX_OK), "Test #9 Testing: 9.5: Test the log component operation")

    result = _log_write(1, 3, 31, 32, 33);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "Test #9 Testing: 9.6: Write to the log with 3 parameters")
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (_log_test(&p1) == MQX_OK), "Test #9 Testing: 9.6: Test the log component operation")

    result = _log_write(1, 4, 41, 42, 43, 44);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "Test #9 Testing: 9.7: Write to the log with 4 parameters")
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (_log_test(&p1) == MQX_OK), "Test #9 Testing: 9.7: Test the log component operation")

    result = _log_write(1, 5, 51, 52, 53, 54, 55);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "Test #9 Testing: 9.8: Write to the log with 5 parameters")
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (_log_test(&p1) == MQX_OK), "Test #9 Testing: 9.8: Test the log component operation")

    result = _log_write(1, 6, 61, 62, 63, 64, 65, 66);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK , "Test #9 Testing: 9.9: Write to the log with 6 parameters")
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (_log_test(&p1) == MQX_OK), "Test #9 Testing: 9.9: Test the log component operation")

    result = _log_write(1, 7, 71, 72, 73, 74, 75, 76, 77);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK , "Test #9 Testing: 9.10: Write to the log with 7 parameters")
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (_log_test(&p1) == MQX_OK), "Test #9 Testing: 9.10: Test the log component operation")

    result = _log_write(1, 8, 81, 82, 83, 84, 85, 86, 87, 88);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK , "Test #9 Testing: 9.11: Write to the log with 8 parameters")
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (_log_test(&p1) == MQX_OK), "Test #9 Testing: 9.11: Test the log component operation")

    result = _log_write(1, 9, 91, 92, 93, 94, 95, 96, 97, 98, 99);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK , "Test #9 Testing: 9.12: Write to the log with 9 parameters")
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (_log_test(&p1) == MQX_OK), "Test #9 Testing: 9.12: Test the log component operation")

    result = _log_write(1, 10, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK , "Test #9 Testing: 9.13: Write to the log with 10 parameters")
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (_log_test(&p1) == MQX_OK), "Test #9 Testing: 9.13: Test the log component operation")
    
    for (i = 0; i < 11; i++) {
        result = _log_read(1, LOG_READ_OLDEST_AND_DELETE, i, (LOG_ENTRY_STRUCT_PTR)&entry);
        EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK , "Test #9 Testing: 9.14: Read the log")
        EU_ASSERT_REF_TC_MSG(tc_9_main_task, (_log_test(&p1) == MQX_OK), "Test #9 Testing: 9.14: Test the log component operation")

        result = log_test_entry2(&entry, i, i, i);
        EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == TRUE, "Test #9 Testing: 9.15: Test log entry")
        EU_ASSERT_REF_TC_MSG(tc_9_main_task, (_log_test(&p1) == MQX_OK), "Test #9 Testing: 9.15: Test the log component operation")
    } /* Endfor */

    result = _log_write(1, 0);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK , "Test #9 Testing: 9.16:  Write to the log with none parameter")
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (_log_test(&p1) == MQX_OK), "Test #9 Testing: 9.16: Test the log component operation")

    result = _log_write(1, 1, 11);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "Test #9 Testing: 9.17: Write to the log with one parameter")
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (_log_test(&p1) == MQX_OK), "Test #9 Testing: 9.17: Test the log component operation")

    result = _log_write(1, 2, 21, 22);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "Test #9 Testing: 9.18: Write to the log with 2 parameters")
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (_log_test(&p1) == MQX_OK), "Test #9 Testing: 9.18: Test the log component operation")

    result = _log_write(1, 3, 31, 32, 33);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "Test #9 Testing: 9.19: Write to the log with 3 parameters")
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (_log_test(&p1) == MQX_OK), "Test #9 Testing: 9.19: Test the log component operation")

    result = _log_write(1, 4, 41, 42, 43, 44);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK , "Test #9 Testing: 9.20: Write to the log with 4 parameters")
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (_log_test(&p1) == MQX_OK), "Test #9 Testing: 9.20: Test the log component operation")

    result = _log_write(1, 5, 51, 52, 53, 54, 55);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK , "Test #9 Testing: 9.21: Write to the log with 5 parameters")
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (_log_test(&p1) == MQX_OK), "Test #9 Testing: 9.21: Test the log component operation")

    result = _log_write(1, 6, 61, 62, 63, 64, 65, 66);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK , "Test #9 Testing: 9.22: Write to the log with 6 parameters")
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (_log_test(&p1) == MQX_OK), "Test #9 Testing: 9.22: Test the log component operation")

    result = _log_write(1, 7, 71, 72, 73, 74, 75, 76, 77);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK , "Test #9 Testing: 9.23: Write to the log with 7 parameters")
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (_log_test(&p1) == MQX_OK), "Test #9 Testing: 9.23: Test the log component operation")

    result = _log_write(1, 8, 81, 82, 83, 84, 85, 86, 87, 88);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK , "Test #9 Testing: 9.24: Write to the log with 8 parameters")
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (_log_test(&p1) == MQX_OK), "Test #9 Testing: 9.24: Test the log component operation")

    result = _log_write(1, 9, 91, 92, 93, 94, 95, 96, 97, 98, 99);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "Test #9 Testing: 9.25: Write to the log with 9 parameters")
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (_log_test(&p1) == MQX_OK), "Test #9 Testing: 9.25: Test the log component operation")

    result = _log_write(1, 10, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK , "Test #9 Testing: 9.26: Write to the log with 10 parameters")
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (_log_test(&p1) == MQX_OK), "Test #9 Testing: 9.26: Test the log component operation")

    for (i = 0; i < 11; i++) {
        result = _log_read(1, LOG_READ_OLDEST_AND_DELETE, 1, (LOG_ENTRY_STRUCT_PTR)&entry);
        EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK , "Test #9 Testing: 9.27:Read the log")
        EU_ASSERT_REF_TC_MSG(tc_9_main_task, (_log_test(&p1) == MQX_OK), "Test #9 Testing: 9.27: Test the log component operation")

        result = log_test_entry2(&entry, i, 1, i);
        EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == TRUE, "Test #9 Testing: 9.28: Test log entry")
        EU_ASSERT_REF_TC_MSG(tc_9_main_task, (_log_test(&p1) == MQX_OK), "Test #9 Testing: 9.28: Test the log component operation")
    } /* Endfor */

    /* Is this a bug in the test program? specifying 0 parameters yet passing
    ** 1. This is how the line below originally looked.
    log_test_fn(9, 29, _log_write(1, 0, 1), MQX_OK);
    */   
    result = _log_write(1, 0);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK , "Test #9 Testing: 9.29: Write to the log")
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (_log_test(&p1) == MQX_OK), "Test #9 Testing: 9.29: Test the log component operation")

    result = _log_write(1, 1, 11);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "Test #9 Testing: 9.30: Write to the log")
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (_log_test(&p1) == MQX_OK), "Test #9 Testing: 9.30: Test the log component operation")

    result = _log_write(1, 2, 21);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "Test #9 Testing: 9.31: Write to the log")
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (_log_test(&p1) == MQX_OK), "Test #9 Testing: 9.31: Test the log component operation")

    result = _log_write(1, 3, 31);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "Test #9 Testing: 9.32: Write to the log")
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (_log_test(&p1) == MQX_OK), "Test #9 Testing: 9.32: Test the log component operation")

    result = _log_write(1, 4, 41);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "Test #9 Testing: 9.33: Write to the log")
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (_log_test(&p1) == MQX_OK), "Test #9 Testing: 9.33: Test the log component operation")

    result = _log_write(1, 5, 51);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "Test #9 Testing: 9.34: Write to the log")
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (_log_test(&p1) == MQX_OK), "Test #9 Testing: 9.34: Test the log component operation")

    result = _log_write(1, 6, 61);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "Test #9 Testing: 9.35: Write to the log")
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (_log_test(&p1) == MQX_OK), "Test #9 Testing: 9.35: Test the log component operation")

    result = _log_write(1, 7, 71);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "Test #9 Testing: 9.36: Write to the log")
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (_log_test(&p1) == MQX_OK), "Test #9 Testing: 9.36: Test the log component operation")

    result = _log_write(1, 8, 81);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "Test #9 Testing: 9.37: Write to the log")
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (_log_test(&p1) == MQX_OK), "Test #9 Testing: 9.37: Test the log component operation")

    result = _log_write(1, 9, 91);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "Test #9 Testing: 9.38: Write to the log")
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (_log_test(&p1) == MQX_OK), "Test #9 Testing: 9.38: Test the log component operation")

    result = _log_write(1, 10, 101);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "Test #9 Testing: 9.39: Write to the log")
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (_log_test(&p1) == MQX_OK), "Test #9 Testing: 9.39: Test the log component operation")

    for (i = 0; i < 11; i++) {
        result = _log_read(1, LOG_READ_OLDEST_AND_DELETE, i, (LOG_ENTRY_STRUCT_PTR)&entry);
        EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "Test #9 Testing: 9.40: Read the log")
        EU_ASSERT_REF_TC_MSG(tc_9_main_task, (_log_test(&p1) == MQX_OK), "Test #9 Testing: 9.40: Test the log component operation")

        result = log_test_entry2(&entry, i, i, 1);
        EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == TRUE, "Test #9 Testing: 9.41: Test log entry")
        EU_ASSERT_REF_TC_MSG(tc_9_main_task, (_log_test(&p1) == MQX_OK), "Test #9 Testing: 9.41: Test the log component operation")
    } /* Endfor */
}

//------------------------------------------------------------------------
// Define Test Suite    
 EU_TEST_SUITE_BEGIN(suite_1)
    EU_TEST_CASE_ADD(tc_1_main_task, " Test #1: Test Auto-component creation...."),
    EU_TEST_CASE_ADD(tc_2_main_task, " Test #2: Invalid use of log before it is created...."),
    EU_TEST_CASE_ADD(tc_3_main_task, " Test #3: Test log creation and destruction...."),
    EU_TEST_CASE_ADD(tc_4_main_task, " Test #4: Test log read types...."),
    EU_TEST_CASE_ADD(tc_5_main_task, " Test #5: Test log types (OVERWRITE or not)...."),
    EU_TEST_CASE_ADD(tc_6_main_task, " Test #6: Test log reset...."),
    EU_TEST_CASE_ADD(tc_7_main_task, " Test #7: Test log enable/disable...."),
    EU_TEST_CASE_ADD(tc_8_main_task, " Test #8: ISR Test...."),
    EU_TEST_CASE_ADD(tc_9_main_task, " Test #9: Variable sized read and write....")
 EU_TEST_SUITE_END(suite_1)
// Add test suites
 EU_TEST_REGISTRY_BEGIN()
        EU_TEST_SUITE_ADD(suite_1, " - Testing the Log Component")
 EU_TEST_REGISTRY_END()
//------------------------------------------------------------------------

/*TASK*-------------------------------------------------------------------
* 
* Task Name    : Main_task
* Comments     :
*
*END*----------------------------------------------------------------------*/

void main_task
   (
      _mqx_uint dummy
   )
{ /* Body */
   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();
} /* Endbody */

/* EOF */
