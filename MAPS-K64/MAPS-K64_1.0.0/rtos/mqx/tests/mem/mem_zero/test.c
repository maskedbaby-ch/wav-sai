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
* $Version : 3.8.4.1$
* $Date    : Aug-10-2012$
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
#include <util.h>
//#include <test_module.h>
#include "EUnit.h"
#include "eunit_crit_secs.h"

#define FILE_BASENAME   test

extern void main_task(uint32_t);

void tc_1_main_task(void);

TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
#if (MCF5213_PROCESSOR || PSP_MQX_CPU_IS_KINETIS_L)
   { 10,  main_task, 1000,  8, "Main",  MQX_AUTO_START_TASK},
#else
   { 10,  main_task, 8000,  8, "Main",  MQX_AUTO_START_TASK},
#endif
   { 0,  0,         0,     0, 0,           0                  }
};

#define TEST_SIZE    50//2048
#define ALLOC_SIZE   TEST_SIZE+4
#define FALLOUT_SIZE 16

#define TOTAL_SIZE ALLOC_SIZE+FALLOUT_SIZE

unsigned char To[TOTAL_SIZE];

/******************** Begin Testcases **********************/

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_1_main_task
* Comments     :
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
   MQX_TICK_STRUCT start_time_tick;
   MQX_TICK_STRUCT end_time_tick;
   MQX_TICK_STRUCT diff_time_tick;
   TIME_STRUCT     diff_time;
   _mqx_uint l;
   register _mqx_uint i,j;
   
    EU_ASSERT(TRUE)
    
   /* Set the Fallout Area */
   for ( i = ALLOC_SIZE; i < TOTAL_SIZE; ++i ) {
      To[i] = 0xE5;
   } /* Endfor */

   for ( l = 0; l < 4; ++l ) {
      putchar('.');
      for ( j = 0; j < TEST_SIZE; ++j ) {
         for ( i = 0; i < ALLOC_SIZE; ++i ) {
            To[i] = i;
         } /* Endfor */

         _mem_zero(&To[l],j);

         /* Check to */
         for ( i = 0; i < l; i++ ) {
            EU_ASSERT_REF_TC_MSG(tc_1_main_task, To[i] == (unsigned char)i, "Test #1: Testing 1.1: Check To array")
            if ( To[i] != (unsigned char)i ) {
                 _task_block();
            } /* Endif */
         } /* Endfor */
         for ( i = l ; i < l+j ; i++ ) {
            EU_ASSERT_REF_TC_MSG(tc_1_main_task, To[i] == 0, "Test #1: Testing 1.2:Check me_zeor() operation")
            if ( To[i] != 0 ) {
                 _task_block();
            } /* Endif */
         } /* Endfor */
         for ( i = l+j; i < ALLOC_SIZE; i++ ) {
            EU_ASSERT_REF_TC_MSG(tc_1_main_task, To[i] == (unsigned char)i, "Test #1: Testing 1.3: Check To array")
            if ( To[i] != (unsigned char)i ) {
                 _task_block();
            } /* Endif */
         } /* Endfor */
         /* Check fallout at end of to */
         for ( i = ALLOC_SIZE; i < TOTAL_SIZE; i++ ) {
            EU_ASSERT_REF_TC_MSG(tc_1_main_task, To[i] == 0xE5, "Test #1: Testing 1.4: Check fallout at end of To array")
            if ( To[i] != 0xE5 ) {
                 _task_block();
            } /* Endif */
         } /* Endfor */
            
      } /* Endfor */
   } /* Endfor */

   /* Now time zeromem */
   _time_delay_ticks(2);
   _time_get_ticks(&start_time_tick);
   for ( l = 0; l < 4; ++l ) {
       for ( j = 0; j < TEST_SIZE; ++j ) {
         _mem_zero(&To[l],j);
      } /* Endfor */
   } /* Endfor */
   _time_get_ticks(&end_time_tick);
   _time_diff_ticks(&end_time_tick, &start_time_tick, &diff_time_tick);
   _ticks_to_time(&diff_time_tick, &diff_time);
   
   /* Now time memset */
   _time_delay_ticks(2);
   _time_get_ticks(&start_time_tick);
   for ( l = 0; l < 4; ++l ) {
       for ( j = 0; j < TEST_SIZE; ++j ) {
         memset(&To[l],0,j);
      } /* Endfor */
   } /* Endfor */
   _time_get_ticks(&end_time_tick);
   _time_diff_ticks(&end_time_tick, &start_time_tick, &diff_time_tick);
   _ticks_to_time(&diff_time_tick, &diff_time);
}

/******************** End Testcases ********************/

/******************** Register tests *******************/
EU_TEST_SUITE_BEGIN(suite_mem_zero)
    EU_TEST_CASE_ADD(tc_1_main_task, "- Test of the function _mem_zero")
EU_TEST_SUITE_END(suite_mem_zero)

EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_mem_zero, "Test of the function _mem_zero")
EU_TEST_REGISTRY_END()
/******************** End Register *********************/
/*TASK*-------------------------------------------------------------------
* 
* Task Name    : main_task
* Comments     :
*
*END*----------------------------------------------------------------------*/
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
