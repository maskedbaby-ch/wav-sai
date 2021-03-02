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
* $Version : 3.8.9.0$
* $Date    : Oct-2-2012$
*
* Comments:
*
*   This file contains the source functions for testing the accuracy
*   of the timer.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
//#include <EUnit_Api.h>
//#include <test_module.h>
#include "EUnit.h"
#include "eunit_crit_secs.h"

#define MAIN_TASK      10
#define FILE_BASENAME test
#define TEST_TIME   10 //time of test execution in s

extern void main_task(uint32_t parameter);
void tc_1_main_task(void); /* TEST #1: Just read the time. */

TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
   { MAIN_TASK, main_task, 2000, 8, "Main", MQX_AUTO_START_TASK},
   { 0,         0,         0,    0, 0,      0 }
};
/*TASK--------------------------------------------------------------------
* 
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Just read the time
*
*END*--------------------------------------------------------------------*/

void tc_1_main_task(void)
{  
   TIME_STRUCT		time;
   uint16_t  		useconds, count = 0;
   
   //align test execution with BSP TIMER
   _time_delay_ticks(1);
   time.SECONDS      = 0;
   time.MILLISECONDS = 0;
   _time_set( &time );
   
   count = TEST_TIME * BSP_ALARM_FREQUENCY + 1;

   while (count) {
      _time_delay_ticks(1);
      _time_get(&time);
      useconds = _time_get_microseconds();
	  
      //printf("%04ld.%03ld %04d\r", time.SECONDS, time.MILLISECONDS, useconds);	  
      count--;
      
      //break if execution time exceeded expectations
      if(time.SECONDS > TEST_TIME)
      {
         break;
      }
   } /* Endwhile */
   
   EU_ASSERT_REF_TC_MSG( tc_1_main_task, time.SECONDS == TEST_TIME, "Test #1 Testing: Checking time operation\n")
}

/* Define Test Suite*/  
 EU_TEST_SUITE_BEGIN(suite_1)
    EU_TEST_CASE_ADD(tc_1_main_task, " Test # 1 Just read the time.")
 EU_TEST_SUITE_END(suite_1)

/* Add test suites*/
 EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_1, " time - Just read the time.")
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
