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
* $Version : 3.8.6.0$
* $Date    : Aug-3-2012$
*
* Comments:
*
*   This file contains the source functions for testing the 
*   get microseconds kernel call.
*
*END************************************************************************/

#include <mqx_inc.h>
#include <bsp.h>
#include <util.h>
#include <timer.h>
#include <psp_prv.h>
//#include <test_module.h>
#include "EUnit.h"
#include "eunit_crit_secs.h"

#define FILE_BASENAME test
#define MAIN_TASK      10
#define SIZE_ARRAY     200

void tc_1_main_task(void);/* TEST #1, Just read the time. */

extern void main_task(uint32_t parameter);

TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
   { MAIN_TASK, main_task, 2000, 8, "Main", MQX_AUTO_START_TASK},
   { 0,         0,         0,    0, 0,      0 }
};

volatile MQX_TICK_STRUCT  tick_array[SIZE_ARRAY];
volatile uint32_t          usec_array[SIZE_ARRAY];
volatile _mqx_uint        array_index;


/*TASK*-------------------------------------------------------------------
* 
* Task Name    : time_gatherer
* Comments     :
*   Get and store the time.
*
*END*----------------------------------------------------------------------*/

void time_gatherer
   (
      void
   )
{
   /* Store microsecond time in array */
	if ( array_index < SIZE_ARRAY ) 
	{
      /* get times */
      _time_get_ticks((MQX_TICK_STRUCT_PTR)(&tick_array[array_index]));
      array_index++;
   }/* Endif */
}  /* Endbody */ 



/*TASK*-------------------------------------------------------------------
* 
* Task Name    : tc_1_main_task
* Comments     :
*   Test case number one.
*
*END*----------------------------------------------------------------------*/

void tc_1_main_task(void)
{  
   KERNEL_DATA_STRUCT_PTR   kernel_data;
   bool                  overflow;
   volatile _mqx_uint       j, k, l, m,result=1;

   array_index = 0;

   _GET_KERNEL_DATA(kernel_data);
   _int_disable();
   _time_delay_ticks(2);
   kernel_data->TIMER_COMPONENT_ISR = time_gatherer; 

	for ( j = 0; j <= 100; j++ ) 
	{
      _int_enable();
		for ( k = 0; k <= 10000; k++ ) 
		{
      } /* Endfor */

		_int_disable();
		if ( array_index < SIZE_ARRAY ) 
		{
         _time_get_ticks((MQX_TICK_STRUCT_PTR)(&tick_array[array_index]));
         array_index++;
		} 
		else 
		{
         break;
      }/* Endif */
   } /* Endfor */

   kernel_data->TIMER_COMPONENT_ISR = 0;
   _int_enable();

   /* Convert all ticks to microseconds */
	for ( l = 0; l < array_index; l++ ) 
	{
      usec_array[l] = _time_diff_microseconds(
         (MQX_TICK_STRUCT_PTR)(&tick_array[l]), 
         (MQX_TICK_STRUCT_PTR)(&_mqx_zero_tick_struct), &overflow);
   } /* Endfor */

   /* Check all times in array to make sure they are sequential */
	for ( l = 0; l < array_index - 1; l++ ) 
	{
      m = l + 1;
		if ( usec_array[l] > usec_array[m] ) 
		{
         result=0;
         break;
      }/* Endif */
   } /* Endfor */

 EU_ASSERT_REF_TC_MSG( tc_1_main_task, result==1, "Test #1 Testing: Get and store time.\n")

}

/* Define Test Suite*/  
 EU_TEST_SUITE_BEGIN(suite_1)
    EU_TEST_CASE_ADD(tc_1_main_task, " Test # 1 Get and store time.")
 EU_TEST_SUITE_END(suite_1)

/* Add test suites*/
 EU_TEST_REGISTRY_BEGIN()
        EU_TEST_SUITE_ADD(suite_1, " usec2 - Get and store time.")
 EU_TEST_REGISTRY_END()














/*TASK*-------------------------------------------------------------------
* 
* Task Name    : main_task
* Comments     :
*   Get and store time, check and display any errors.
*
*END*----------------------------------------------------------------------*/

void main_task
   (
      uint32_t parameter
   )
{ /* Body */
	 eunit_mutex_init();
     EU_RUN_ALL_TESTS(eunit_tres_array);
     eunit_test_stop();
} /* Endbody */

/* EOF */
