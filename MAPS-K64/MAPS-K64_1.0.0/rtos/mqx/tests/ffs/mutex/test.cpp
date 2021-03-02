/**HEADER*******************************************************************
*
* Copyright (c) 2013 Freescale Semiconductor;
* All Rights Reserved
*
****************************************************************************
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
****************************************************************************
*
* $FileName: test.cpp$
* $Version : 4.0.2.0$
* $Date    : June-11-2013$
*
* Comments:
*
*   This file contains main initialization and functions to test simple mutex 
*
* Requirements:
*   FFS062, FFS063.
* 
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"
#include <nandflash_wl.h>
#include "ddi_media.h"
#include "simple_mutex.h"
#include "test.h"

/* Test case function prototype */
void tc_1_main_task(void);

/* Define Test Suite */
EU_TEST_SUITE_BEGIN(suite_mutex)
    EU_TEST_CASE_ADD(tc_1_main_task, " Test #1 - Test Simple mutex "),
EU_TEST_SUITE_END(suite_mutex)

/* Add test suites */
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_mutex, " - Testing simple mutex")
EU_TEST_REGISTRY_END()

TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
/*  Task number, Entry point, Stack, Pri, String, Auto? */
   {MAIN_TASK,   main_task,   2000,  9,   "main", MQX_AUTO_START_TASK},
   {0,           0,           0,     0,   0,      0,                 }
};

WL_MUTEX     m_mutex;
_mqx_int     counter = 0;
bool      task1_exit = false;
bool      task2_exit = false;

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_1_main_task
* Comments       :
*    Test simplemutex for NAND driver
* Requirements   : 
*    FFS062, FFS063.
*
*END------------------------------------------------------------------*/

void tc_1_main_task(void)
{
    _task_id                task1;
    _task_id                task2;
    TASK_TEMPLATE_STRUCT    template_task1;
    TASK_TEMPLATE_STRUCT    template_task2;

   /* Step 1: Init Task1's template*/
    template_task1.TASK_TEMPLATE_INDEX = 10;
    template_task1.TASK_ADDRESS = (void (_CODE_PTR_)(uint32_t))task1_function;
    template_task1.TASK_STACKSIZE = WL_TASK_STACK_SIZE;
    template_task1.TASK_PRIORITY = WL_TASK_PRIORITY;
    template_task1.TASK_NAME = WL_TASK1_NAME;
    template_task1.CREATION_PARAMETER = WL_TASK_CREATION_PARAM;
    /* Verify Step 1 */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (TRUE), " Test #1.1: Init Task1's template successful");

    /* Step 2: Init Task2's template*/
    template_task2.TASK_TEMPLATE_INDEX = 11;
    template_task2.TASK_ADDRESS = (void (_CODE_PTR_)(uint32_t))task2_function;
    template_task2.TASK_STACKSIZE = WL_TASK_STACK_SIZE;
    template_task2.TASK_PRIORITY = WL_TASK_PRIORITY;
    template_task2.TASK_NAME = WL_TASK2_NAME;
    template_task2.CREATION_PARAMETER = WL_TASK_CREATION_PARAM;

    /* Verify Step 2 */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (TRUE), " Test #1.2: Init Task2's template successful");

    /* Step 3: Init Simple Mutex for NAND*/
    SimpleMutex::InitMutex(&m_mutex);
    /* Verify Step 3*/
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (TRUE), " Test #1.3: Init mutex successful");

    /* Step 4: Create Task 1*/
    task1 = _task_create(0, 0, (uint32_t)&template_task1);
    /* Verify Step 4 */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (task1 != MQX_NULL_TASK_ID), " Test #1.4: Create task1 successful");

    /* Step 5: Create Task 2*/
    task2 = _task_create(0, 0, (uint32_t)&template_task2);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (task1 != MQX_NULL_TASK_ID), " Test #1.5: Create task2 successful");

    /*Step 6: Wait until task1 & task 2 finish*/
    if (task1_exit == true && task2_exit == true) {
        SimpleMutex::DestroyMutex(&m_mutex);
        EU_ASSERT_REF_TC_MSG(tc_1_main_task, (TRUE), " Test #1.6: Task1 & Task2 exit successful ");
    }
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name  : task1_function
* Comments       :
*
*
*END------------------------------------------------------------------*/

void task1_function(uint32_t args) {

    for (uint8_t i = 0; i < 10; i++) {
        SimpleMutex sm(m_mutex);
        printf("task1 locked\n");
        counter++;
        printf("taks1 increases counter=%d\n", counter);
        _time_delay(1000);
        printf("task1 unlocked\n");
    }

    task1_exit = true;
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name  : task2_function
* Comments       :
*
*
*END------------------------------------------------------------------*/

void task2_function(uint32_t args) {

    for (uint8_t i = 0; i < 10; i++) {
        SimpleMutex sm(m_mutex);
        printf("task2 locked\n");
        counter++;
        printf("taks2 increases counter=%d\n", counter);
        _time_delay(1000);
        printf("task2 unlocked\n");
    }

    task2_exit = true;
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : main_task
* Comments       : The task is used to test simple mutex
*
*
*END------------------------------------------------------------------*/

void main_task(uint32_t initial_data)
{
    if (_io_nandflash_wl_install(&_bsp_nandflash_wl_init, NAND_FLASH_WL_FILE) != MQX_OK)
    {
      /* Can not install FFS */
      _task_block();
    }
    eunit_mutex_init();
    EU_RUN_ALL_TESTS(eunit_tres_array);
    eunit_test_stop();
}/* Endbody */

/* EOF */