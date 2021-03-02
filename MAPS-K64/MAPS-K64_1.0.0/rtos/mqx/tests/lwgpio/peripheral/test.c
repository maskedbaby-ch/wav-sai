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
#include "test.h"

#define MAIN_TASK       1

void tc_1_main_task(); // " TEST 1 - Test lwgpio with external device "

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
* Comments  : Test lwgpio with external device
*
*END*--------------------------------------------------------------*/
void tc_1_main_task()
{
    uint8_t  ret_value;
    uint32_t pin_it;
    LWGPIO_VALUE gpio_value;


  /* external pins used as input */
  for(pin_it = 0; pin_it < TST_PORT_PIN_COUNT; pin_it++)
  {
      ret_value = lwgpio_init(&tst_port_pin_init_list[pin_it], tst_port_pin_id_list[pin_it], LWGPIO_DIR_NOCHANGE, LWGPIO_VALUE_NOCHANGE);
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, ret_value, "Test 1.00: Testing lwgpio_init function ");

      lwgpio_set_functionality(&tst_port_pin_init_list[pin_it], LWGPIO_PIN_MUX_GPIO);
      ret_value = lwgpio_get_functionality(&tst_port_pin_init_list[pin_it]);
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, ret_value == LWGPIO_PIN_MUX_GPIO, "Test 1.01: Testing lwgpio set and get functionality");

      ret_value = IOClient_DMB_SetPortPinDirection(tst_port_pin_maftp_list[pin_it], TL_max_input);
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, ret_value == 0, "Test 1.02: Testing set pin of external device to input direction");

      lwgpio_set_direction(&tst_port_pin_init_list[pin_it], LWGPIO_DIR_OUTPUT);
      lwgpio_set_value(&tst_port_pin_init_list[pin_it], LWGPIO_VALUE_LOW);

      ret_value = IOClient_DMB_GetPortPinValue(tst_port_pin_maftp_list[pin_it]);
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, ret_value == STD_LOW, "Test 1.03: Testing lwgpio set value");

      lwgpio_set_value(&tst_port_pin_init_list[pin_it], LWGPIO_VALUE_HIGH);
      ret_value = IOClient_DMB_GetPortPinValue(tst_port_pin_maftp_list[pin_it]);
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, ret_value == STD_HIGH, "Test 1.04: Testing lwgpio set value");

      lwgpio_toggle_value(&tst_port_pin_init_list[pin_it]);
      ret_value = IOClient_DMB_GetPortPinValue(tst_port_pin_maftp_list[pin_it]);
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, ret_value == STD_LOW, "Test 1.05: Testing lwgpio toogle value");

      lwgpio_toggle_value(&tst_port_pin_init_list[pin_it]);
      ret_value = IOClient_DMB_GetPortPinValue(tst_port_pin_maftp_list[pin_it]);
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, ret_value == STD_HIGH, "Test 1.06: Testing lwgpio toogle value");
  }

  /* external pins used as output */
  for(pin_it = 0; pin_it < TST_PORT_PIN_COUNT; pin_it++)
  {
      lwgpio_set_functionality(&tst_port_pin_init_list[pin_it], LWGPIO_PIN_MUX_GPIO);

      ret_value = lwgpio_get_functionality(&tst_port_pin_init_list[pin_it]);
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, ret_value == LWGPIO_PIN_MUX_GPIO, "Test 1.07: Testing set and get functionality");

      lwgpio_set_direction(&tst_port_pin_init_list[pin_it], LWGPIO_DIR_INPUT);

      ret_value = IOClient_DMB_SetPortPinDirection(tst_port_pin_maftp_list[pin_it], TL_max_output);
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, ret_value == 0, "Test 1.08: Testing set pin of external device to output direction");

      ret_value = IOClient_DMB_SetPortPinValue(tst_port_pin_maftp_list[pin_it], TL_max_low);
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, ret_value == 0, "Test 1.09: Testing set pin of external device to low value");

      gpio_value = lwgpio_get_value(&tst_port_pin_init_list[pin_it]);
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, gpio_value == LWGPIO_VALUE_LOW, "Test 1.10: Testing lwgpio get value");

      gpio_value = lwgpio_get_raw(&tst_port_pin_init_list[pin_it]);
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, gpio_value == LWGPIO_VALUE_LOW, "Test 1.11: Testing lwgpio get raw");

      ret_value = IOClient_DMB_SetPortPinValue(tst_port_pin_maftp_list[pin_it], TL_max_high);
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, ret_value == 0, "Test 1.12: Testing set pin of external device to high value");

      gpio_value = lwgpio_get_value(&tst_port_pin_init_list[pin_it]);
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, gpio_value == LWGPIO_VALUE_HIGH, "Test 1.13: Testing lwgpio get value");

      gpio_value = lwgpio_get_raw(&tst_port_pin_init_list[pin_it]);
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, gpio_value == LWGPIO_VALUE_HIGH, "Test 1.14: Testing lwgpio get raw");
  }

  /* set used external pins back to the inputs */
  for(pin_it = 0; pin_it < TST_PORT_PIN_COUNT; pin_it++)
  {
      ret_value = IOClient_DMB_SetPortPinDirection(tst_port_pin_maftp_list[pin_it], TL_max_input);
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, ret_value == 0, "Test 1.15: Testing set pin of external device to input direction");
  }
}

/******************** End Testcases ********************/

//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(peripheral)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST 1 - Test lwgpio with external device "),
EU_TEST_SUITE_END(peripheral)

// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(peripheral, " - lwgpio test")
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
    IOClient_Init(IOCLIENT_SCI_CHANNEL, 9600, BSP_SYSTEM_CLOCK);

    eunit_mutex_init();
    EU_RUN_ALL_TESTS(eunit_tres_array);
    eunit_test_stop();
}

/* EOF */
