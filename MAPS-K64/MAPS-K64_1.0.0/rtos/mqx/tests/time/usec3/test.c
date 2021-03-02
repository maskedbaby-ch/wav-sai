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
* $Version : 3.8.5.0$
* $Date    : Aug-3-2012$
*
* Comments:
*
*   This file contains the source functions for testing the 
*   get_microseconds (test 3)
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
//#include <EUnit_Api.h>
//#include <test_module.h>
#include "EUnit.h"
#include "eunit_crit_secs.h"


#define FILE_BASENAME test
#define MAIN_TASK      10

void tc_1_main_task(void);/* TEST #1: Testing the get_microseconds */

void main_task(uint32_t parameter);

TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
   { MAIN_TASK, main_task, 2000, 8, "Main", MQX_AUTO_START_TASK},
   { 0,         0,         0,    0, 0,      0                  }
};

/* Globals to keep track of old timer ISR */
void (_CODE_PTR_ OldHandler)(void *);

void   *OldData;

void new_tick_notifier
   (
      void   *not_used
   )
{
   /* Chain to BSP timer handler */
   (*OldHandler)(OldData);   
}

/*TASK*-------------------------------------------------------------------
* 
* Task Name    : tc_1_main_task
* Comments     :
*   Testing the get_microseconds.
*
*END*----------------------------------------------------------------------*/

void tc_1_main_task(void)
{  
   TIME_STRUCT            time;
   TIME_STRUCT            time2;
   volatile uint32_t       j, k;
	uint16_t                useconds, useconds2;
    int result;
 
   /* Install a test handler */
   OldData = _int_get_isr_data(BSP_TIMER_INTERRUPT_VECTOR);//Gets the data pointer associated with an interrupt.
   _int_disable();
	/* Installs an application-defined ISR. */
	OldHandler = _int_install_isr(BSP_TIMER_INTERRUPT_VECTOR, new_tick_notifier, (void *)0);
   _int_enable();

   /* Poll for a long time */
	for (j = 0; j < 500000; j++) 
	{
      k = k * 99;
   } /* Endfor */
   

   /* Restore old handler */
   _int_disable();
   _int_install_isr(BSP_TIMER_INTERRUPT_VECTOR, OldHandler, OldData);
   _int_enable();

   /* Test to see if timer interrupt still ticking */
   _time_get(&time);
   useconds = _time_get_microseconds();
   _time_delay(1000);
   _time_get(&time2);
   useconds2 = _time_get_microseconds();

	/* If we get here, things are ok */
    result= time2.SECONDS - time.SECONDS;
 EU_ASSERT_REF_TC_MSG( tc_1_main_task, result==1, "Test #1 Testing: Testing the get_microseconds.\n")

}

/* Define Test Suite*/  
 EU_TEST_SUITE_BEGIN(suite_1)
	EU_TEST_CASE_ADD(tc_1_main_task, " Test #1 Testing the get_microseconds.")
 EU_TEST_SUITE_END(suite_1)

/* Add test suites*/
 EU_TEST_REGISTRY_BEGIN()
        EU_TEST_SUITE_ADD(suite_1, " usec3 - Testing the get_microseconds.")
 EU_TEST_REGISTRY_END()




/*TASK*-------------------------------------------------------------------
* 
* Task Name    : main_task
* Comments     :
*   Just read and display the time.
*
*END*----------------------------------------------------------------------*/

void main_task
   (
      uint32_t parameter
   )
{	
	eunit_mutex_init();
	EU_RUN_ALL_TESTS(eunit_tres_array);
	eunit_test_stop();
} /* Endbody */

/* EOF */
