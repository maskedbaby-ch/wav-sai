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
* $Date    : Oct-1-2012$
*
* Comments:
*
*   This file contains the source functions for testing the 
*   get microseconds kernel call for V&V test system.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
//#include <EUnit_Api.h>
//#include <test_module.h>
#include "EUnit.h"
#include "eunit_crit_secs.h"

#define LOOP_COUNT    	1000
#define MAIN_TASK    	10
#define NUM_SAMPLES 	128

#define FILE_BASENAME test


extern void main_task(uint32_t parameter);

void tc_1_main_task(void); /* TEST #1: Just read the time.  */
void tc_2_main_task(void); /* TEST #2: Read time with delay */
void tc_3_main_task(void); /* TEST #3: Read time in the ISR */

TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
   { MAIN_TASK, main_task, 2400, 8, "Main", MQX_AUTO_START_TASK},
   { 0,         0,         0,    0, 0,      0 }
};

volatile uint32_t enabled_data[NUM_SAMPLES];
volatile uint32_t disabled_data[NUM_SAMPLES];
volatile _mqx_uint enabled_index;
volatile _mqx_uint disabled_index;
void (_CODE_PTR_ old_isr)(void *);
void  	*old_isr_param = NULL;

void new_tick_notifier
	(
		void   *not_used
	)
{
	volatile _mqx_uint j, k;
   
	(*old_isr)(old_isr_param);
	while (disabled_index < 5) {
		for (j = 0; j < 5; j++) {
			k = k * 99;
		} /* Endfor */
		disabled_data[disabled_index++] = _time_get_microseconds();
	} /* Endwhile */
 return;
}

/*TASK--------------------------------------------------------------------
* 
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Just read the time
*
*END*--------------------------------------------------------------------*/
void tc_1_main_task(void)
{  
    uint32_t result =1;

    _int_disable();
    enabled_index  = 0;
    _time_delay_ticks(2); /* suspend the active task for the specified number of ticks */
    _int_enable();
   
    while (enabled_index < NUM_SAMPLES) {
        enabled_data[enabled_index++] = (uint32_t)_time_get_microseconds(); /* Gets the calculated number //of microseconds, since the //last periodic timer interrupt. */
        if(enabled_data[enabled_index] == enabled_data[enabled_index-1])
        {
            result =0;
        }
    }   /* Endwhile */

    EU_ASSERT_REF_TC_MSG( tc_1_main_task, result==1, "Test #1 Testing: Gets the calculated number of microseconds...\n")
}

/*TASK--------------------------------------------------------------------
* 
* Task Name    : tc_2_main_task
* Comments     : TEST #2: Read time with delay
*
*END*--------------------------------------------------------------------*/
void tc_2_main_task(void)
{  
    uint32_t                msec,result =0;
    TIME_STRUCT            time;
    volatile _mqx_uint     j,k;

    _int_disable();
    enabled_index  = 0;
    _time_delay_ticks(2);    //suspend the active task for the specified number of ticks 
    _time_get_elapsed(&time);//Gets the second/millisecond time that has elapsed,
                             //since the application started on this processor.
    msec = time.MILLISECONDS;
    _int_enable();

    while (enabled_index < NUM_SAMPLES) {
        enabled_data[enabled_index++] = (uint32_t)_time_get_microseconds();
        for (j = 0; j < LOOP_COUNT; j++) {
            k = k * 99;
        } /* Endfor */
        _time_get_elapsed(&time);
        if (time.MILLISECONDS != msec) {
            /* A timer tick has occurred */
            result = 1; 
            break;
        } /* Endif */
    } /* Endwhile */
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, result, "Test #2 Testing: Read time with delay \n")
}

/*TASK--------------------------------------------------------------------
* 
* Task Name    : tc_3_main_task
* Comments     : TEST #3: Read time in the ISR
*
*END*--------------------------------------------------------------------*/
void tc_3_main_task(void) 
{  
    volatile _mqx_uint	j,k;
    uint32_t	 result = 1;
	
	enabled_index = 0;
	disabled_index = 0;
	
	for (j = 0; j < NUM_SAMPLES; j++)
	{
		disabled_data[j] = 0;
	}
	
	_int_disable();
    old_isr_param = _int_get_isr_data(BSP_TIMER_INTERRUPT_VECTOR);
    old_isr = _int_install_isr(BSP_TIMER_INTERRUPT_VECTOR, new_tick_notifier, NULL );
	_int_enable();

	/* wait for timer interrupt occurs */
	_time_delay_ticks(3);

	/* restore original timer routine */
	_int_disable();	    
    _int_install_isr(BSP_TIMER_INTERRUPT_VECTOR, old_isr, old_isr_param);		
	_int_enable();

	for (j = 0; j < disabled_index; j++)
	{
		if (disabled_data[j] == 0)
		{
			result = 0;
			break;
		}
	}
		
	EU_ASSERT_REF_TC_MSG( tc_3_main_task, (disabled_index > 0)&&(result == 1), "Test #3 Test reading time from the ISR")    
}

/* Define Test Suite*/  
 EU_TEST_SUITE_BEGIN(suite_1)
    EU_TEST_CASE_ADD(tc_1_main_task, " Test #1: Gets the calculated number of microseconds."),
    EU_TEST_CASE_ADD(tc_2_main_task, " Test #2: Read time with delay."),
    EU_TEST_CASE_ADD(tc_3_main_task, " Test #3: Read time  in the ISR.")
 EU_TEST_SUITE_END(suite_1)

/* Add test suites*/
 EU_TEST_REGISTRY_BEGIN()
        EU_TEST_SUITE_ADD(suite_1, " usec1 - get microseconds kernel call.")
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
   
    _int_install_unexpected_isr();

 //test_initialize();
 //EU_RUN_ALL_TESTS(test_result_array); 
 //test_stop();
   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();

} /* Endbody */

/* EOF */
