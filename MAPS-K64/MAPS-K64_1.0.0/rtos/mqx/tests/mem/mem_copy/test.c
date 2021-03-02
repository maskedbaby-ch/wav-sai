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
* $Version : 3.7.3.0$
* $Date    : Mar-24-2011$
*
* Comments:
*
*   This file contains the source functions for the loop test example.
*   The example main task creates a specified number of sub-task.
*   Each sub task creates a corresponding task, and sends a specified
*   number of messages, a specified number of times to that task.
*   In this case, the message is NOT freed/obtained from a pool.
*
*END************************************************************************/

#include <string.h>
#include <mqx.h>
#include <bsp.h>
#include "util.h"
//#include <test_module.h>
#include "EUnit.h"
#include "eunit_crit_secs.h"

#define FILE_BASENAME   test

extern void main_task(uint32_t);
//------------------------------------------------------------------------
// Test suite function
void tc_1_main_task(void);//TEST #1: Test of the function _mem_copy
//------------------------------------------------------------------------
TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
#ifdef MCF5213_PROCESSOR
   { 10,  main_task, 1000,  8, "Main",  MQX_AUTO_START_TASK},
#else
   { 10,  main_task, 8000,  8, "Main",  MQX_AUTO_START_TASK},
#endif
   { 0,   0,         0,     0, 0,       0                  }
};

#define TEST_SIZE    100//2048
#define ALLOC_SIZE   TEST_SIZE+4
#define FALLOUT_SIZE 16
#define TOTAL_SIZE ALLOC_SIZE+FALLOUT_SIZE
unsigned char From[ALLOC_SIZE];
unsigned char To[TOTAL_SIZE];

/*#define TIME_ONLY*/
/******************** Begin Testcases **********************/
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_1_main_task
// Comments     : TEST #1: Test of the function _mem_copy 
//
//END---------------------------------------------------------------------
void tc_1_main_task(void)
{
   MQX_TICK_STRUCT start_time_tick;
   MQX_TICK_STRUCT end_time_tick;
   MQX_TICK_STRUCT diff_time_tick;
   TIME_STRUCT     diff_time;
   _mqx_uint k,l, c;
   register _mqx_uint  i,j,m,n;
   register unsigned char      *to_ptr;
   register unsigned char      *from_ptr;
    
   from_ptr = &From[0];
   to_ptr   = &To[0];
   for ( i = 0; i < ALLOC_SIZE; ++i ) {
      *from_ptr++ = i;
      *to_ptr++   = 0;
   } /* Endfor */

   to_ptr = &To[ALLOC_SIZE];
   for ( i = ALLOC_SIZE; i < TOTAL_SIZE; ++i) {
      *to_ptr++ = 0xE5;
   } /* Endif */

#ifndef TIME_ONLY
   /* Test the copy command */
   for ( l = 0; l < 4; ++l ) {
      for ( k = 0; k < 4; ++k ) {
         for ( j = 0; j < TEST_SIZE; ++j ) {

            to_ptr = &To[0];
            for (c = 0; c < ALLOC_SIZE; ++c) {
               *to_ptr++ = 0;
            } /* Endfor */

            _mem_copy(&From[k], &To[l], j);
            
            /* Check to */
            for ( i = 0; i < l; i++ ) {
               EU_ASSERT_REF_TC_MSG(tc_1_main_task, To[i] == 0,"Test #1 Testing: 1.1: Check to")
               if ( To[i] != 0 ) {
                    _task_block();
               } /* Endif */
            } /* Endfor */

            i = l;
            n = j+1;
            m = From[k];
            to_ptr = &To[l];
            while ( --n ) {
               EU_ASSERT_REF_TC_MSG(tc_1_main_task, *to_ptr == (unsigned char)m,"Test #1 Testing: 1.2: Compare *to_ptr with (unsigned char)m")
               if ( *to_ptr != (unsigned char)m ) {
                    _task_block();
               } /* Endif */
               ++to_ptr;
               ++m;
               ++i;
            } /* Endwhile */

            to_ptr = &To[j+l];
            for ( i = j+l; i < TEST_SIZE; i++ ) {
               EU_ASSERT_REF_TC_MSG(tc_1_main_task, *to_ptr == 0,"Test #1 Testing: 1.3: *to_ptr should return 0 value")
               if ( *to_ptr++ != 0 ) {
                    _task_block();
               } /* Endif */
            } /* Endfor */

            /* Check fallout at end of to */
            to_ptr = &To[ALLOC_SIZE];
            for ( i = 0; i < FALLOUT_SIZE; i++ ) {
               EU_ASSERT_REF_TC_MSG(tc_1_main_task, *to_ptr == 0xE5,"Test #1 Testing: 1.4: *to_ptr should return 0 value")
               if ( *to_ptr++ != 0xE5 ) {
                    _task_block();
               } /* Endif */
            } /* Endfor */
         } /* Endfor */
      } /* Endfor */
   } /* Endfor */
#endif

   /* Time the copy command */
   _time_delay_ticks(2);
   _time_get_ticks(&start_time_tick);
   for ( l = 0; l < 4; ++l ) {
      for ( k = 0; k < 4; ++k ) {
         for ( j = 0; j < TEST_SIZE; ++j ) {
            _mem_copy(&From[k], &To[l], j);
         } /* Endfor */
      } /* Endfor */
   } /* Endfor */
   _time_get_ticks(&end_time_tick);
   _time_diff_ticks(&end_time_tick, &start_time_tick, &diff_time_tick);
   _ticks_to_time(&diff_time_tick, &diff_time);
   
   /* Time the compiler memcpy command */
   _time_delay_ticks(10);
   _time_get_ticks(&start_time_tick);
   for ( l = 0; l < 4; ++l ) {
      for ( k = 0; k < 4; ++k ) {
         for ( j = 0; j < TEST_SIZE; ++j ) {
            memcpy(&To[l], &From[k], j);
         } /* Endfor */
      } /* Endfor */
   } /* Endfor */
   _time_get_ticks(&end_time_tick);

   _time_diff_ticks(&end_time_tick, &start_time_tick, &diff_time_tick);
   _ticks_to_time(&diff_time_tick, &diff_time);
}

/******************** End Testcases ********************/

/******************** Register tests *******************/
EU_TEST_SUITE_BEGIN(suite_mem_copy)
    EU_TEST_CASE_ADD(tc_1_main_task, "- Test of the function _mem_copy")
EU_TEST_SUITE_END(suite_mem_copy)

EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_mem_copy, "Test of the function _mem_copy")
EU_TEST_REGISTRY_END()
/******************** End Register *********************/
void main_task
   (
      uint32_t dummy
   )
{ /* Body */
   _int_install_unexpected_isr();   
   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();
} /* Endbody */
/* EOF */
