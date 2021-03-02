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
* $Version : 3.7.4.0$
* $Date    : Mar-24-2011$
*
* Comments:
*
*   
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <log.h>
//#include <test_module.h>
#include "EUnit.h"
#include "eunit_crit_secs.h"

#define FILE_BASENAME test

#define MAIN_TASK       10
#define MY_LOG          5

#define MAX_LOG_ENTRIES 10

#define LOG_SIZE \
   ((MAX_LOG_ENTRIES * sizeof(ENTRY_STRUCT) +  sizeof(LOG_ENTRY_STRUCT)) / sizeof(_mqx_uint))


extern void main_task( uint32_t initial_data );

void tc_1_main_task(void);// Test #1 - Install the components into MQX....
void tc_2_main_task(void);// Test #2 - Read data from the log....
void tc_3_main_task(void);// Test #3 - Fill to the end of the log....
void tc_4_main_task(void);// Test #4 - Read data from the log....
void tc_5_main_task(void);// Test #5 - Re-create a log but in over-write mode....
void tc_6_main_task(void);// Test #6 - Read data from the log ....

TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
   { MAIN_TASK, main_task, 2000L, 8L, "Main", MQX_AUTO_START_TASK},
   { 0L,        0L,        0L,    0L, 0L,     0L, 0L }
};


typedef struct 
{
   LOG_ENTRY_STRUCT   HEADER;
   _mqx_uint          a;
} ENTRY_STRUCT, * ENTRY_STRUCT_PTR;

_mqx_uint    result;
_mqx_uint    i;
_mqx_uint    entries;
_mqx_uint    writes;
unsigned char        c;
ENTRY_STRUCT entry;
 
/******   Test cases body definition      *******/
/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Install the components into MQX
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task()
{
    /* Install the components into MQX */
    result = _log_create_component();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1 Testing: 1.1: Create log component")
    if (result != MQX_OK) {
        eunit_test_stop();
        _mqx_exit(0);  
    }

    /* Create a log */
    result = _log_create(MY_LOG, LOG_SIZE, 0);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1 Testing: 1.2: Create the log")
    if (result != MQX_OK) {
        eunit_test_stop();
        _mqx_exit(0);  
    }

    writes = 1;
    result = _log_write(MY_LOG, 1, writes);
    entries = 1;
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1 Testing: 1.3: Write to the log")

    writes++;
    result = _log_write(MY_LOG, 1, writes);
    entries++;
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1 Testing: 1.4: Write to the log")
     
    writes++;
    result = _log_write(MY_LOG, 1, writes);
    entries++;   
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1 Testing: 1.5: Write to the log")

    writes++;
    result = _log_write(MY_LOG, 1, writes);
    entries++;
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1 Testing: 1.6: Write to the log")
}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_2_main_task
* Comments     : TEST #2: Read data from the log.
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task()
{
    /* Read data from the log */
   
   result = _log_read(MY_LOG, LOG_READ_OLDEST, 1, (LOG_ENTRY_STRUCT_PTR) &entry);
   i = 1;
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test # Testing: 2.1: Read data from the log")
   if (result == MQX_OK) {
      while ((result == MQX_OK) && (i < (entries * 3))) {
         result = _log_read(MY_LOG, LOG_READ_NEXT, 1, 
            (LOG_ENTRY_STRUCT_PTR)&entry);
         i++;
         EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test # Testing: 2.2: Read data from the log")
      }
   } 
   
   result = _log_read(MY_LOG, LOG_READ_OLDEST_AND_DELETE, 1, (LOG_ENTRY_STRUCT_PTR) &entry);
   i = 1;
   entries--;
   if (result == MQX_OK) {
      result = _log_read(MY_LOG, LOG_READ_OLDEST, 1, (LOG_ENTRY_STRUCT_PTR) &entry);
      EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test # Testing: 2.3: Read data from the log")
      if (result == MQX_OK) {
         while ((result == MQX_OK) && (i < (entries * 3))) {
            result = _log_read(MY_LOG, LOG_READ_NEXT, 1, 
               (LOG_ENTRY_STRUCT_PTR)&entry);
            i++;
            EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test # Testing: 2.4: Read data from the log")
            
         }
      }
   }
}
/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_3_main_task
* Comments     : TEST #3: Fill to the end of the log.
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task()
{
    /* Fill to the end of the log - we wrote 4 entries already */
   for (i = 0; i < (MAX_LOG_ENTRIES - 4); i++) {
      writes++;
      result = _log_write(MY_LOG, 1, writes);
      entries++;
      EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3 Testing: 3.1: Write to the log")
   }
   /* Read data from the log */
   
   result = _log_read(MY_LOG, LOG_READ_OLDEST, 1, (LOG_ENTRY_STRUCT_PTR) &entry);
   i = 1;
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3 Testing: 3.2: Read data from the log")
   if (result == MQX_OK) {
      while ((result == MQX_OK) && (i < (entries * 3))) {
         result = _log_read(MY_LOG, LOG_READ_NEXT, 5, 
            (LOG_ENTRY_STRUCT_PTR)&entry);
         i++;
         EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3 Testing: 3.3: Read data from the log")
         
      }
   }
}
/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_4_main_task
* Comments     : TEST #4: Read data from the log.
*
*END*---------------------------------------------------------------------*/
void tc_4_main_task()
{
   result = _log_read(MY_LOG, LOG_READ_OLDEST_AND_DELETE, 1, (LOG_ENTRY_STRUCT_PTR) &entry);
   i = 1;
   entries--;
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4 Testing: 4.1: Read data from the log")
   if (result == MQX_OK) {
      result = _log_read(MY_LOG, LOG_READ_OLDEST, 1, (LOG_ENTRY_STRUCT_PTR) &entry);
      EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4 Testing: 4.2: Read data from the log")
      if (result == MQX_OK) {
         while ((result == MQX_OK) && (i < (entries * 3))) {
            result = _log_read(MY_LOG, LOG_READ_NEXT, 1, 
               (LOG_ENTRY_STRUCT_PTR)&entry);
            i++;
            EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4 Testing: 4.3: Read data from the log")
         }
      }
   } 
   
   writes++;
   result = _log_write(MY_LOG, 1, writes);
   entries++;
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4 Testing: 4.4: Write to the log")

   /* Read data from the log */
   result = _log_read(MY_LOG, LOG_READ_OLDEST, 1, (LOG_ENTRY_STRUCT_PTR) &entry);
   i = 1;
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4 Testing: 4.5: Read data from the log")
   if (result == MQX_OK) {
      while ((result == MQX_OK) && (i < (entries * 3))) {
         result = _log_read(MY_LOG, LOG_READ_NEXT, 1, 
            (LOG_ENTRY_STRUCT_PTR)&entry);
         i++;
         EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4 Testing: 4.6: Read data from the log")
         
      }
   }
   for (i = 0; i < entries; i++) {
      result = _log_read(MY_LOG, LOG_READ_OLDEST_AND_DELETE, 1, 
         (LOG_ENTRY_STRUCT_PTR)&entry);
      EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4 Testing: 4.7: Read data from the log")
         
   }
}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_5_main_task
* Comments     : TEST #5: Re-create a log but in over-write mode.
*
*END*---------------------------------------------------------------------*/
void tc_5_main_task()
{
    /* Delete the log */
    _log_destroy( MY_LOG );

    /* Re-create a log but in over-write mode */
    result = _log_create(MY_LOG, LOG_SIZE, LOG_OVERWRITE);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "Test #5 Testing: 5.1: Create a log in over-write mode")
    if (result != MQX_OK) {
        eunit_test_stop();
        _mqx_exit(0);
    }

    writes  = 0;
    entries = 0;

    for (i = 0; i < (MAX_LOG_ENTRIES + 1); i++) {
        result = _log_write(MY_LOG, 1, writes+1);
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "Test #5 Testing: 5.2: Write to the log")
        if (result == MQX_OK) {
            writes++;
            entries++;
        }
    }
    entries--;
}
/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_6_main_task
* Comments     : TEST #6: Read data from the log.
*
*END*---------------------------------------------------------------------*/
void tc_6_main_task()
{
    /* Read data from the log */
   result = _log_read(MY_LOG, LOG_READ_OLDEST, 1, (LOG_ENTRY_STRUCT_PTR) &entry);
   i = 1;
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6 Testing: 6.1: Read data from the log")
   if (result == MQX_OK) {
      while ((result == MQX_OK) && (i < (entries * 3))) {
         result = _log_read(MY_LOG, LOG_READ_NEXT, 1, 
            (LOG_ENTRY_STRUCT_PTR)&entry);
         i++;
         EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6 Testing: 6.2: Read data from the log")
                  
      }
   }
}

//------------------------------------------------------------------------
// Define Test Suite
 EU_TEST_SUITE_BEGIN(suite_2)
    EU_TEST_CASE_ADD(tc_1_main_task, " Test #1 - Install the components into MQX...."),
    EU_TEST_CASE_ADD(tc_2_main_task, " Test #2 - Read data from the log...."),
    EU_TEST_CASE_ADD(tc_3_main_task, " Test #3 - Fill to the end of the log...."),
    EU_TEST_CASE_ADD(tc_4_main_task, " Test #4 - Read data from the log...."),
    EU_TEST_CASE_ADD(tc_5_main_task, " Test #5 - Re-create a log but in over-write mode...."),
    EU_TEST_CASE_ADD(tc_6_main_task, " Test #6 - Read data from the log ...."),
 EU_TEST_SUITE_END(suite_2) 
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_2, " - Testing the Log Component")
EU_TEST_REGISTRY_END()
//------------------------------------------------------------------------

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : Main_task
* Comments     :
*
*END*---------------------------------------------------------------------*/
void main_task ( uint32_t initial_data )
{
    _int_install_unexpected_isr();
   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();
}
