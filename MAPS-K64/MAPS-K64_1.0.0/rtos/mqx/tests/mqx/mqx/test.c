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
* $Version : 3.8.7.0$
* $Date    : Apr-17-2012$
*
* Comments:
*
*   This file contains code for the _mqx primitive verification
*   program.
*   Revision History:
*   Date          Version        Changes
*   May, 1998      2.40          Original version
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <mem_prv.h>
#include <mqx_prv.h>
#include "EUnit.h"
#include "eunit_crit_secs.h"

#include <gen_rev.h>
#include <psp_rev.h>
#include <bsp_rev.h>
#include <io_rev.h>
#include "test.h"

#define FILE_BASENAME test

#ifdef MQX_INIT_TASK_NAME
#define INIT_TASK_PRIO_DIFF     1
#define IDLE_TASK_PRIO_DIFF     2
#else
#define IDLE_TASK_PRIO_DIFF     1
#endif

extern    MQX_INITIALIZATION_STRUCT    MQX_init_struct;

//------------------------------------------------------------------------
// Test suite function prototipe
extern void tc_1_main_task(void);// Testing copyright and version primitives by inspection
extern void tc_2_main_task(void);// misc. _mqx_get primitives
extern void tc_3_main_task(void);// _mqx_get_counter
extern void tc_4_main_task(void);// _mqx_get_cpu_type, _mqx_set_cpu_type
extern void tc_5_main_task(void);// verify idle task
extern void tc_6_main_task(void);// set/get exit handler, should print ""Exit handler entered"" msg
//------------------------------------------------------------------------

extern void Exit_handler(void);


TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
    { MAIN_TASK,      main_task,     3000,  9, "main",    MQX_AUTO_START_TASK},
    { 0,              0,             0,     0, 0,         0 }
};
/*------Variable------------------------*/
bool                         error_found;
_task_id                        system_task;
char                        *str1, *str2, *str3;
_mqx_uint                       i,j,result,test[100];
_mqx_uint                       temp,idle_priority,low_prior;
_mqx_uint                       idle1,idle2,idle3,idle4;
TD_STRUCT_PTR                   td_ptr;
KERNEL_DATA_STRUCT             *kernel_ptr;
MQX_INITIALIZATION_STRUCT_PTR   init_ptr;

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : string_compare
* Returned Value   : TRUE   - If strings are the same
                   : FALSE  - Other
* Comments         :
*       This function compare 2 strings.
*
*END*----------------------------------------------------------------------*/
static bool string_compare(const char *p1, const char *p2)
{
    bool result = TRUE;
    if((p1 == NULL) || (p2 == NULL))
    {
        return FALSE;
    }
    for (; *p1 == *p2; p1++, p2++)
    {
        if (*p1 == '\0')
        {
            return TRUE;
        }
    }
    return FALSE;
}
//TASK--------------------------------------------------------------------
//
// Task Name    : tc_1_main_task
// Comments     : TEST #1: Testing copyright and version primitives by inspection
//
//END---------------------------------------------------------------------

void tc_1_main_task(void)
{
    str1 = (char*)_mqx_copyright;
    while (*str1 != 'F') str1++;

    error_found = !string_compare(str1, "Freescale Semiconductor. All rights reserved.");
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, error_found == FALSE, "Test #1 Testing: 1.1: _mqx_coppy_right operation");

    error_found = !string_compare(_mqx_generic_revision, REAL_NUM_TO_STR(GEN_REVISION));
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, error_found == FALSE, "Test #1 Testing: 1.2: _mqx_generic_revision operation");

    error_found = !string_compare(_mqx_psp_revision, REAL_NUM_TO_STR(PSP_REVISION));
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, error_found == FALSE, "Test #1 Testing: 1.3: _mqx_psp_revision operation");

    error_found = !string_compare(_mqx_bsp_revision, REAL_NUM_TO_STR(BSP_REVISION));
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, error_found == FALSE, "Test #1 Testing: 1.4: _mqx_bsp_revision operation");

    error_found = !string_compare(_mqx_io_revision, REAL_NUM_TO_STR(IO_REVISION));
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, error_found == FALSE, "Test #1 Testing: 1.5: _mqx_io_revision operation");
}

//TASK--------------------------------------------------------------------
//
// Task Name    : tc_2_main_task
// Comments     : TEST #2: misc. _mqx_get primitives
//
//END---------------------------------------------------------------------

void tc_2_main_task(void)
{
    /*
    ** calls: _mqx_get_kernel_data, _mqx_get_initialization, _mqx_get_system_task_id,
    **           _mqx_monitor_type
    */
    kernel_ptr = (KERNEL_DATA_STRUCT_PTR)_mqx_get_kernel_data();
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (kernel_ptr->INIT.START_OF_KERNEL_MEMORY == kernel_ptr), "Test #2 Testing: 2.1: _mqx_get_kernel_data operation");

    init_ptr = _mqx_get_initialization();
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (&kernel_ptr->INIT == init_ptr), "Test #2 Testing: 2.2: _mqx_get_initialization operation");

    /*
    ** system_task_id should be [proc_no] << 16 + SYSTEM_TASK_NUMBER
    **  check that lower word is SYSTEM_TASK_NUMBER and upper word is processor_no
    */
    system_task = _mqx_get_system_task_id();
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (((system_task & SYSTEM_TASK_NUMBER) == SYSTEM_TASK_NUMBER) && ((system_task >> 16) == kernel_ptr->INIT.PROCESSOR_NUMBER)), "Test #2 Testing: 2.3: _mqx_get_system_task_id operation");

    temp = _mqx_monitor_type;
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, ((temp == MQX_MONITOR_TYPE_NONE) || (temp == MQX_MONITOR_TYPE_BDM) || (temp == MQX_MONITOR_TYPE_MON) || (temp == MQX_MONITOR_TYPE_MON_INT) || (temp == MQX_MONITOR_TYPE_JTAG) || (temp == MQX_MONITOR_TYPE_OTHER)), "Test #2 Testing: 2.4: _mqx_monitor_type operation");
}

//TASK--------------------------------------------------------------------
//
// Task Name    : tc_3_main_task
// Comments     : TEST #3: _mqx_get_counter
//
//END---------------------------------------------------------------------

void tc_3_main_task(void)
{
    for (i=0; i < 100; i ++) {
        test[i] = _mqx_get_counter();
        for (j=0; j < i; j++) {
            EU_ASSERT_REF_TC_MSG(tc_3_main_task, (test[j] != test[i]), "Test #3 Testing: 3.1: _mqx_get_counter operation");
            EU_ASSERT_REF_TC_MSG(tc_3_main_task, (test[j] != 0), "Test #3 Testing: 3.2: _mqx_get_counter operation should not return 0");
        }
    }
    for (i=0; i < 100; i ++) {
        test[i] = _mqx_get_counter();
        for (j=0; j < i; j++) {
            EU_ASSERT_REF_TC_MSG(tc_3_main_task, (test[j] != test[i]), "Test #3 Testing: 3.3: _mqx_get_counter operation");
            EU_ASSERT_REF_TC_MSG(tc_3_main_task, (test[j] != 0), "Test #3 Testing: 3.4: _mqx_get_counter operation should not return 0");
        }
    }
}

//TASK--------------------------------------------------------------------
//
// Task Name    : tc_4_main_task
// Comments     : TEST #4: _mqx_get_cpu_type, _mqx_set_cpu_type
//
//END---------------------------------------------------------------------

void tc_4_main_task(void)
{
    temp = _mqx_get_cpu_type();
    _mqx_set_cpu_type(temp+1);
    result = _mqx_get_cpu_type();
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == temp+1), "Test #4 Testing: 4.1: _mqx_set_cpu_type and _mqx_get_cpu_type operation");

    _mqx_set_cpu_type(temp);
    result = _mqx_get_cpu_type();
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == temp), "Test #4 Testing: 4.2: _mqx_set_cpu_type and _mqx_get_cpu_type operation");

    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == kernel_ptr->CPU_TYPE), "Test #4 Testing: 4.3: _mqx_get_cpu_type should be equal to kernel_ptr->CPU_TYPE");
}

//TASK--------------------------------------------------------------------
//
// Task Name    : tc_5_main_task
// Comments     : TEST #5: verify idle task and init task
//
//END---------------------------------------------------------------------

void tc_5_main_task(void)
{
#ifdef MQX_INIT_TASK_NAME
    _mqx_uint       init_task_priority;
    TD_STRUCT_PTR   init_td_ptr;
    _task_id        init_task_id;

    low_prior = _sched_get_min_priority (0);

    init_task_priority = kernel_ptr->INIT_TASK_TEMPLATE.TASK_PRIORITY;

    EU_ASSERT_REF_TC_MSG(tc_5_main_task, init_task_priority == (low_prior + INIT_TASK_PRIO_DIFF), "Test #5 Testing: 5.0: testing priority of init task");

    init_task_id = _task_get_id_from_name(MQX_INIT_TASK_NAME);

    /* If init task is still alive */
    if (init_task_id != MQX_NULL_TASK_ID)
    {
        init_td_ptr = (TD_STRUCT_PTR)_task_get_td( init_task_id );
        result = _task_get_priority(init_td_ptr->TASK_ID, &init_task_priority);
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, init_task_priority == (low_prior + INIT_TASK_PRIO_DIFF), "Test #5 Testing: 5.1: testing priority of init task");
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (init_td_ptr->STATE == READY), "Test #5 Testing: 5.2: init task should be READY state");
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (kernel_ptr->INIT_TASK_TEMPLATE.TASK_ADDRESS == _mqx_init_task), "Test #5 Testing: 5.3: init task address should be correct");
    }
#endif

    low_prior = _sched_get_min_priority (0);

    idle_priority = kernel_ptr->IDLE_TASK_TEMPLATE.TASK_PRIORITY;
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, idle_priority == (low_prior + IDLE_TASK_PRIO_DIFF), "Test #5 Testing: 5.4: testing priority of idle task");

    td_ptr = (TD_STRUCT_PTR)_task_get_td( _task_get_id_from_name(MQX_IDLE_TASK_NAME) );
    result = _task_get_priority(td_ptr->TASK_ID, &idle_priority);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (idle_priority == low_prior + IDLE_TASK_PRIO_DIFF), "Test #5 Testing: 5.5: testing priority of idle task");
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (td_ptr->STATE == READY), "Test #5 Testing: 5.6: idle task should be READY state");

    idle1 = kernel_ptr->IDLE_LOOP.IDLE_LOOP1;
    idle2 = kernel_ptr->IDLE_LOOP.IDLE_LOOP2;
    idle3 = kernel_ptr->IDLE_LOOP.IDLE_LOOP3;
    idle4 = kernel_ptr->IDLE_LOOP.IDLE_LOOP4;

    _time_delay_ticks(4);

    EU_ASSERT_REF_TC_MSG(tc_5_main_task, ((idle1 != kernel_ptr->IDLE_LOOP.IDLE_LOOP1) || (idle2 != kernel_ptr->IDLE_LOOP.IDLE_LOOP2) || (idle3 != kernel_ptr->IDLE_LOOP.IDLE_LOOP3) || (idle4 != kernel_ptr->IDLE_LOOP.IDLE_LOOP4)), "Test #5 Testing: 5.7: idle task should have ran");
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (td_ptr->STATE == READY), "Test #5 Testing: 5.8: idle task should be READY state");
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (kernel_ptr->IDLE_TASK_TEMPLATE.TASK_ADDRESS == _mqx_idle_task), "Test #5 Testing: 5.9: idle task address should be correct");
}

//TASK--------------------------------------------------------------------
//
// Task Name    : tc_6_main_task
// Comments     : TEST #6: set/get exit handler, should print ""Exit handler entered"" msg
//
//END---------------------------------------------------------------------

void tc_6_main_task(void)
{
    bool result;
    _mqx_set_exit_handler(Exit_handler);
    result = (_mqx_get_exit_handler() == Exit_handler);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == TRUE), "Test #6 Testing: 6.1: exit handler should have set correctly");
    _time_delay(2);
}


//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_1)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST # 1 - Testing copyright and version primitives by inspection"),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST # 2 - misc. _mqx_get primitives"),
    EU_TEST_CASE_ADD(tc_3_main_task, " TEST # 3 - _mqx_get_counter"),
    EU_TEST_CASE_ADD(tc_4_main_task, " TEST # 4 - _mqx_get_cpu_type, _mqx_set_cpu_type"),
    EU_TEST_CASE_ADD(tc_5_main_task, " TEST # 5 - verify idle task"),
    EU_TEST_CASE_ADD(tc_6_main_task, " TEST # 6 - set or get exit handler, should print 'Exit handler entered msg'")
EU_TEST_SUITE_END(suite_1)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_1, " - MQX _mqx primitives Test Suite")
EU_TEST_REGISTRY_END()
//------------------------------------------------------------------------

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name: Exit_handler
* Return Value : none
* Comments     :
*    dummy exit handler
*
*END*----------------------------------------------------------------------*/
void   Exit_handler(void)
{
    /*  do nothing */
}


/*TASK*-------------------------------------------------------------------
*
* Task Name    : main_task
* Comments     :
*    This is the main task of the test program
*
*END*----------------------------------------------------------------------*/

void main_task
(
    uint32_t param
    /* [IN] The MQX task parameter */
 )
{ /* Body */
   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();

   _mqx_fatal_error(10);
} /* endbody */
