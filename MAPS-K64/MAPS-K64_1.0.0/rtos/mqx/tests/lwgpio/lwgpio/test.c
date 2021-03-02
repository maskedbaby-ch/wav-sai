/**HEADER********************************************************************
*
* Copyright (c) 2013 Freescale Semiconductor;
* All Rights Reserved
*
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
* $Version : 4.1.0.0$
* $Date    : 09-23-2013$
*
* Comments:
*
*   This file contains the source for lwgpio module Test.

*END************************************************************************/
#include <mqx_inc.h>
#include <bsp.h>
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"
#include "lwgpio.h"
#include "port_pin_list.h"

#define MAIN_TASK       1

void tc_1_main_task(); // " TEST 1 - Test lwgpio functions "
void main_task(uint32_t);

TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
/*  Task number, Entry point, Stack, Pri, String, Auto? */
   {MAIN_TASK,   main_task,   2000,  9,   "main", MQX_AUTO_START_TASK},
   {0,           0,           0,     0,   0,      0,                 }
};

/*TASK*--------------------------------------------------------------
*
* Task Name : tc_1_main_task
* Comments  : Test lwgpio functions
*
*END*--------------------------------------------------------------*/
void tc_1_main_task()
{
    uint32_t            ret_value;
    LWGPIO_VALUE        gpio_value;

    ret_value = lwgpio_init(&tst_port_pin_init_list[0], tst_port_pin_id_list[0], LWGPIO_DIR_NOCHANGE, LWGPIO_VALUE_NOCHANGE);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, ret_value, "Test 1.00: Testing lwgpio init function");

    lwgpio_set_functionality(&tst_port_pin_init_list[0], tst_port_pin_gpiofunc_list[0]);

#if (PSP_MQX_CPU_IS_MPC512x || PSP_MQX_CPU_IS_MCF5441X)
    #warning lwgpio device driver does not support appropriate set/get functionality
#else
    ret_value = lwgpio_get_functionality(&tst_port_pin_init_list[0]);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, ret_value == tst_port_pin_gpiofunc_list[0], "Test 1.01: Testing lwgpio set and get functionality");
#endif

    lwgpio_set_direction(&tst_port_pin_init_list[0], LWGPIO_DIR_OUTPUT);
    lwgpio_set_value(&tst_port_pin_init_list[0], LWGPIO_VALUE_LOW);
    lwgpio_toggle_value(&tst_port_pin_init_list[0]);

    gpio_value = lwgpio_get_value(&tst_port_pin_init_list[0]);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, gpio_value == LWGPIO_VALUE_HIGH, "Test 1.02: Testing lwgpio get value");

    ret_value = lwgpio_init(&tst_port_pin_init_list[1], tst_port_pin_id_list[1], LWGPIO_DIR_NOCHANGE, LWGPIO_VALUE_NOCHANGE);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, ret_value, "Test 1.03: Testing lwgpio init function");

    ret_value = lwgpio_int_init(&tst_port_pin_init_list[1], LWGPIO_INT_MODE_RISING);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, ret_value, "Test 1.04: Testing lwgpio int init");

    ret_value = lwgpio_int_get_vector(&tst_port_pin_init_list[1]);
    lwgpio_int_enable(&tst_port_pin_init_list[1], TRUE);

    EU_ASSERT_REF_TC_MSG(tc_1_main_task, lwgpio_int_get_flag(&tst_port_pin_init_list[1]) == FALSE, "Test 1.05: Testing lwgpio get flag");
    lwgpio_int_clear_flag(&tst_port_pin_init_list[1]);
}

/******************** End Testcases ********************/

//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(lwgpio1)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST 1 - Test with lwgpio functions "),
EU_TEST_SUITE_END(lwgpio1)

// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(lwgpio1, " - lwgpio test")
EU_TEST_REGISTRY_END()
//------------------------------------------------------------------------

/*TASK*--------------------------------------------------------------
*
* Task Name : main_task
* Comments  :
*
*END*--------------------------------------------------------------*/
void main_task(uint32_t dummy)
{
    eunit_mutex_init();
    EU_RUN_ALL_TESTS(eunit_tres_array);
    eunit_test_stop();
}

/* EOF */
