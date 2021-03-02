/**HEADER********************************************************************
*
* Copyright (c) 2008-2013 Freescale Semiconductor;
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
* $Version : 4.0.2.0$
* $Date    : Jul-19-2013$
*
* Comments:
*
*   This file contains the source functions for testing the accuracy of the timer.
* Requirements :
*   LWMSGQ002, LWMSGQ004, LWMSGQ008, LWMSGQ010
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <lwmsgq.h>
#include <util.h>
#include "EUnit.h"
#include "eunit_crit_secs.h"


#define FILE_BASENAME   test

#define MAIN_TASK                10
#define WRITE_TASK               11
#define WRITE_TASK_BLOCK         12
#define WRITE_TASK_SEND_BLOCK     13
#define READ_TASK                14
#define READ_TASK_BLOCK          15
#define READ_TASK_TIMEOUT         16

#if PSP_MQX_CPU_IS_KINETIS_L
#define NUM_MESSAGES 16
#else
#define NUM_MESSAGES 32
#endif

#define MSG_SIZE      4


extern void main_task(uint32_t parameter);
extern void write_task(uint32_t parameter);
extern void write_taskb(uint32_t parameter);
extern void write_tasksb(uint32_t parameter);
extern void read_task(uint32_t parameter);
extern void read_taskb(uint32_t parameter);
extern void read_taskt(uint32_t parameter);

//------------------------------------------------------------------------
// Test suite function prototype
void tc_1_main_task(void);// Test #1 - Test of light weight message queues
//------------------------------------------------------------------------

TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
#if PSP_MQX_CPU_IS_KINETIS_L
   { WRITE_TASK_SEND_BLOCK,  write_tasksb, 900, 7, "", 0},
   { MAIN_TASK,              main_task,    900, 8, "Main", MQX_AUTO_START_TASK},
   { WRITE_TASK,        write_task,  900, 9, "", MQX_AUTO_START_TASK},
   { WRITE_TASK_BLOCK,  write_taskb, 900, 13, "", MQX_AUTO_START_TASK},
   { READ_TASK,         read_task,   900,  9, "", MQX_AUTO_START_TASK},
   { READ_TASK_BLOCK,   read_taskb,  900,  9, "", MQX_AUTO_START_TASK},
   { READ_TASK_TIMEOUT, read_taskt,  900,  9, "", MQX_AUTO_START_TASK},
#else
   { WRITE_TASK_SEND_BLOCK, write_tasksb,    2000,     7,     "",     0},
   { MAIN_TASK,             main_task,        2000,     8,     "Main", MQX_AUTO_START_TASK},
   { WRITE_TASK,            write_task,      2000,     9,     "",     MQX_AUTO_START_TASK},
   { WRITE_TASK_BLOCK,      write_taskb,     2000,     13, "",     MQX_AUTO_START_TASK},
   { READ_TASK,             read_task,       2000,      9,     "",     MQX_AUTO_START_TASK},
   { READ_TASK_BLOCK,       read_taskb,      2000,      9,     "",     MQX_AUTO_START_TASK},
   { READ_TASK_TIMEOUT,     read_taskt,      2000,      9,     "",     MQX_AUTO_START_TASK},
#endif
   { 0,                     0,                 0,        0,     0,      0 }
};

MQX_INITIALIZATION_STRUCT  MQX_init_struct =
{
   /* PROCESSOR_NUMBER                */  BSP_DEFAULT_PROCESSOR_NUMBER,
   /* START_OF_KERNEL_MEMORY          */  BSP_DEFAULT_START_OF_KERNEL_MEMORY,
   /* END_OF_KERNEL_MEMORY            */  BSP_DEFAULT_END_OF_KERNEL_MEMORY,
   /* INTERRUPT_STACK_SIZE            */  BSP_DEFAULT_INTERRUPT_STACK_SIZE,
   /* TASK_TEMPLATE_LIST              */  (void *)MQX_template_list,
   /* MQX_HARDWARE_INTERRUPT_LEVEL_MAX*/  BSP_DEFAULT_MQX_HARDWARE_INTERRUPT_LEVEL_MAX,
   /* MAX_MSGPOOLS                    */  BSP_DEFAULT_MAX_MSGPOOLS,
   /* MAX_MSGQS                       */  BSP_DEFAULT_MAX_MSGQS,
   /* IO_CHANNEL                      */  BSP_DEFAULT_IO_CHANNEL,
   /* IO_OPEN_MODE                    */  /*BSP_DEFAULT_IO_OPEN_MODE*/
#if MQX_USE_IO_OLD
   /* IO_OPEN_MODE                    */
   (void *)(IO_SERIAL_TRANSLATION | IO_SERIAL_ECHO)
#else
   /* IO_OPEN_MODE                    */  0
#endif
};

uint32_t my_queue[sizeof(LWMSGQ_STRUCT)/sizeof(uint32_t) +
   NUM_MESSAGES * MSG_SIZE];

uint32_t write_task_delay = 0;
uint32_t write_task_ok = 0;
uint32_t write_task_error = 0;
uint32_t write_taskb_ok = 0;
uint32_t write_taskb_error = 0;
uint32_t write_tasksb_ok = 0;
uint32_t write_tasksb_error = 0;

uint32_t read_task_delay = 0;
uint32_t read_task_ok = 0;
uint32_t read_task_error = 0;

uint32_t read_taskb_ok = 0;
uint32_t read_taskb_error = 0;

uint32_t read_taskt_timeout = 0;
uint32_t read_taskt_ok = 0;
uint32_t read_taskt_error = 0;

void write_task
   (
      uint32_t parameter
   )
{/* Body */
    uint32_t msg[MSG_SIZE];
    uint32_t i,j;
    uint32_t result;

    while (1) {
        i = 0;
        for (j = 0; j < MSG_SIZE; j++) {
            msg[j] = i++;
        }/* Endfor */
        result = _lwmsgq_send((void *)my_queue, msg, 0);
        if (result == MQX_OK) {
            write_task_ok++;
        } else {
            EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == LWMSGQ_FULL), "Test #1 Testing: write_task: _lwmsgq_send should have returned LWMSGQ_FULL when failed to do")
            if (result == LWMSGQ_FULL) {
                write_task_delay++;
                _time_delay_ticks(100);
            } else {
                write_task_error++;
            } /* Endif */
        } /* Endif */
    } /* Endwhile */

} /* Endbody */

void write_taskb
   (
      uint32_t parameter
   )
{/* Body */
    uint32_t msg[MSG_SIZE];
    uint32_t i,j;
    uint32_t result;

    while (1) {
        i = 0x10;
        for (j = 0; j < MSG_SIZE; j++) {
            msg[j] = i++;
        }/* Endfor */
        result = _lwmsgq_send((void *)my_queue, msg, LWMSGQ_SEND_BLOCK_ON_FULL);
        EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing: write_taskb: _lwmsgq_send with LWMSGQ_SEND_BLOCK_ON_FULL operation")
        if (result == MQX_OK) {
            write_taskb_ok++;
        } else {
            write_taskb_error++;
        } /* Endif */
    } /* Endwhile */

} /* Endbody */

void write_tasksb
   (
      uint32_t parameter
   )
{/* Body */
    uint32_t msg[MSG_SIZE];
    uint32_t i,j;
    uint32_t result;

    i = 0x20;
    for (j = 0; j < MSG_SIZE; j++) {
        msg[j] = i++;
    }/* Endfor */
    result = _lwmsgq_send((void *)my_queue, msg, LWMSGQ_SEND_BLOCK_ON_SEND);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing: write_tasksb:_lwmsgq_send with LWMSGQ_SEND_BLOCK_ON_SEND operation")
    if (result == MQX_OK) {
        write_tasksb_ok++;
    } else {
        write_tasksb_error++;
    } /* Endif */

} /* Endbody */


void read_task
   (
      uint32_t parameter
   )
{/* Body */
    uint32_t msg[MSG_SIZE];
    uint32_t result;
    uint32_t j;

    while (1) {
        result = _lwmsgq_receive((void *)my_queue, msg, 0, 0, 0);
        if (result == MQX_OK) {
            read_task_ok++;
            _time_delay_ticks(2);
            for (j = 1; j < MSG_SIZE; j++) {
                EU_ASSERT_REF_TC_MSG(tc_1_main_task, (msg[j] == msg[j-1]+1), "Test #1 Testing: read_task: msg data received should be correct operation")
            }/* Endfor */
        } else {
            EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == LWMSGQ_EMPTY), "Test #1 Testing: read_task: _lwmsgq_receive should have returned LWMSGQ_EMPTY when failed to do")
            if (result == LWMSGQ_EMPTY) {
                read_task_delay++;
                _time_delay_ticks(10);
            } else {
                read_task_error++;
            } /* Endif */
        } /* Endif */
    } /* Endwhile */

} /* Endbody */


void read_taskb
   (
      uint32_t parameter
   )
{/* Body */
    uint32_t msg[MSG_SIZE];
    uint32_t result;
    uint32_t j;

    while (1) {
        result = _lwmsgq_receive((void *)my_queue, msg,
            LWMSGQ_RECEIVE_BLOCK_ON_EMPTY, 0, 0);
        EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing: read_taskb: _lwmsgq_receive operation")
        if (result == MQX_OK) {
            read_taskb_ok++;
            for (j = 1; j < MSG_SIZE; j++) {
                EU_ASSERT_REF_TC_MSG(tc_1_main_task, (msg[j] == msg[j-1]+1), "Test #1 Testing: read_taskb: msg data received should be correct operation")
            }/* Endfor */
        } else {
            read_taskb_error++;
        } /* Endif */
    } /* Endwhile */

} /* Endbody */


void read_taskt
   (
      uint32_t parameter
   )
{/* Body */
    MQX_TICK_STRUCT ticks;
    MQX_TICK_STRUCT current;
    MQX_TICK_STRUCT absolute;
    uint32_t msg[MSG_SIZE];
    uint32_t result;
    uint32_t i, j;

    i = 0;
    while (1) {
        i++;
        if (i == 1) {
            result = _lwmsgq_receive((void *)my_queue, msg,
            LWMSGQ_RECEIVE_BLOCK_ON_EMPTY, 0, 0);
        } else if (i == 2) {
            result = _lwmsgq_receive((void *)my_queue, msg,
            LWMSGQ_RECEIVE_BLOCK_ON_EMPTY, 2, 0);
        } else if (i == 3) {
            _psp_msecs_to_ticks(2, (PSP_TICK_STRUCT_PTR)&ticks);
            result = _lwmsgq_receive((void *)my_queue, msg,
            LWMSGQ_RECEIVE_BLOCK_ON_EMPTY | LWMSGQ_TIMEOUT_FOR, 0, &ticks);
        } else if (i == 4) {
            _psp_msecs_to_ticks(2, (PSP_TICK_STRUCT_PTR)&ticks);
            _time_get_ticks(&current);
            _psp_add_ticks((PSP_TICK_STRUCT_PTR)&ticks,
            (PSP_TICK_STRUCT_PTR)&current, (PSP_TICK_STRUCT_PTR)&absolute);
            result = _lwmsgq_receive((void *)my_queue, msg,
            LWMSGQ_RECEIVE_BLOCK_ON_EMPTY | LWMSGQ_TIMEOUT_UNTIL, 0, &absolute);
            i = 0;
        } /* Endif */

        EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK || result == LWMSGQ_TIMEOUT), "Test #1 Testing: read_taskt: _lwmsgq_receive should have returned either MQX_OK or LWMSGQ_TIMEOUT")
        if (result == MQX_OK) {
            read_taskt_ok++;
            for (j = 1; j < MSG_SIZE; j++) {
                EU_ASSERT_REF_TC_MSG(tc_1_main_task, (msg[j] == msg[j-1]+1), "Test #1 Testing: read_taskt: msg data received should be correct operation")
            }/* Endfor */
        } else if (result == LWMSGQ_TIMEOUT) {
            read_taskt_timeout++;
        } else {
            read_taskt_error++;
        } /* Endif */
    } /* Endwhile */

} /* Endbody */


//TASK--------------------------------------------------------------------
//
// Task Name    : tc_1_main_task
// Comments     : TEST #1: Test of light weight message queues
// Requirements : LWMSGQ002, LWMSGQ004, LWMSGQ008, LWMSGQ010
//END---------------------------------------------------------------------

void tc_1_main_task(void)
{
    _mqx_uint result;
    _task_id  tid;
    uint32_t   msg[MSG_SIZE];

    /* Test _lwmsgq_receive() which shall return the error LWMSGQ_INVALID if the lightweight message queue pointed by pointer handle was not already created by _lwmsgq_init */
    result=_lwmsgq_receive((void*)my_queue, msg, 0, 0, 0);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == LWMSGQ_INVALID), "Test #1 Testing: 1.01: _lwmsgq_receive operation");
    /* Test _lwmsgq_receive() which shall return the error LWMSGQ_INVALID if the lightweight message queue pointed by pointer handle was not already created by _lwmsgq_init */
    result=_lwmsgq_send((void*)my_queue, msg, 0);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == LWMSGQ_INVALID), "Test #1 Testing: 1.02: _lwmsgq_send operation");

    result = _lwmsgq_init((void *)my_queue, NUM_MESSAGES, MSG_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing: 1.03: _lwmsgq_init operation")

    /* Test _lwmsgq_receive() which shall return the error MQX_EINVAL if the lightweight message queue pointed by pointer handle has been already created by _lwmsgq_init */
    result=_lwmsgq_init((void*)my_queue, NUM_MESSAGES, MSG_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_EINVAL), "Test #1 Testing: 1.04: _lwmsgq_init operation");
    /* Create task that sends and blocks */
    tid = _task_create(0, WRITE_TASK_SEND_BLOCK, 0);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (tid != MQX_NULL_TASK_ID), "Test #1 Testing: 1.05: create write task that sends and blocks operation")
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (LWMSGQ_SIZE(my_queue) == 1), "Test #1 Testing: 1.06: message queue received should have size of 1")

    _task_ready(_task_get_td(tid));

    result = _lwmsgq_receive((void *)my_queue, msg, 0, 0, 0);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing: 1.07: _lwmsgq_receive operation")
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (LWMSGQ_SIZE(my_queue) == 0), "Test #1 Testing: 1.08: message queue received should have size of 0")

    _time_delay_ticks(2000);

    result = (write_task_error == 0 && write_taskb_error == 0 && write_tasksb_error == 0 &&
        read_task_error == 0 && read_taskb_error == 0 && read_taskt_error == 0);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == TRUE), "Test #1 Testing: 1.09: write tasks and read tasks should have no error")
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (write_task_delay), "Test #1 Testing: 1.10: write task should be delayed")
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (write_task_ok), "Test #1 Testing: 1.11: write task should be ok")
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (write_taskb_ok), "Test #1 Testing: 1.12: write task b should be ok")
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (write_tasksb_ok), "Test #1 Testing: 1.13: write tasks b should be ok")
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (read_task_delay), "Test #1 Testing: 1.14: read task should be delayed")
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (read_task_ok), "Test #1 Testing: 1.15: read task should be ok")
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (read_taskb_ok), "Test #1 Testing: 1.16: read task b should be ok")
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (read_taskt_timeout), "Test #1 Testing: 1.17: read task t should be timeout")
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (read_taskt_ok), "Test #1 Testing: 1.18: read task t should be ok")
}

//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_lwmsgq)
    EU_TEST_CASE_ADD(tc_1_main_task, " Test #1 - Test of light weight message queues")
EU_TEST_SUITE_END(suite_lwmsgq)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_lwmsgq, " - Test of light weight message queues")
EU_TEST_REGISTRY_END()
//------------------------------------------------------------------------


/*TASK*-------------------------------------------------------------------
*
* Task Name    : main_task
* Comments     :
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


