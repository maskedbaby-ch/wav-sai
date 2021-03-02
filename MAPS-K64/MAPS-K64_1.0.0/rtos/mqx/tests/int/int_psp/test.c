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
* $Version : 3.8.18.0$
*
* Comments:
*
*   This file tests the psp specific interrupt support functions.
*
*END************************************************************************/

#include <mqx_inc.h>
#include <bsp.h>
#include <message.h>
#include <sem.h>
#include <event.h>
#include "util.h"
#include "test.h"
//#include <test_module.h>

#include "EUnit.h"
#include "eunit_crit_secs.h"

#if !MQX_CHECK_ERRORS 
    #error "MQX_CHECK_ERRORS must be set to 1 because we need to check the task errno"
#endif


#define FILE_BASENAME test

#if PSP_ENDIAN == MQX_BIG_ENDIAN
#define TIME_TICKS_LSB_INDEX MQX_NUM_TICK_FIELDS-1
#else
#define TIME_TICKS_LSB_INDEX 0
#endif


//------------------------------------------------------------------------
// Test suite function
extern void tc_1_main_task(void);//TEST # 1 - Testing _int_disable
extern void tc_2_main_task(void);//TEST # 2 - Testing _int_enable
extern void tc_3_main_task(void);//TEST # 3 - Testing _int_set_vector_table
extern void tc_4_main_task(void);//TEST # 4 - Testing _int_get_kernel_isr
extern void tc_5_main_task(void);//TEST # 5 - Testing _int_set_kernel_isr
extern void tc_6_main_task(void);//TEST # 6 - Testing _msg_send from an ISR
extern void tc_7_main_task(void);//TEST # 7 - Testing _event_set from an ISR
extern void tc_8_main_task(void);//TEST # 8 - Testing _sem_post from an ISR
extern void tc_9_main_task(void);//TEST # 9 - Testing _lwsem_post from an ISR
extern void tc_10_main_task(void);//TEST # 10 - Testing _taskq_resume from an ISR
extern void tc_11_main_task(void);//TEST # 11 - Testing _time_dequeue from an ISR
extern void tc_12_main_task(void);//TEST # 12 - Testing _int_get_previous_vector_table
extern void tc_13_main_task(void);//TEST # 13 - Testing _int_get_vector_table
extern void tc_14_main_task(void);//TEST # 14 - Testing kernel isr
//------------------------------------------------------------------------
TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
   { MAIN_TASK,    main_task,         2000, MAIN_PRIO,    "main",   MQX_AUTO_START_TASK},
   { MSGHI_TASK,   msg_higher_task,   1000, MSGHI_PRIO,   "msghi",  0},
   { MSGLO_TASK,   msg_lower_task,    1000, MSGLO_PRIO,   "msglo",  0},
   { EVENTHI_TASK, event_higher_task, 1000, EVENTHI_PRIO, "eventhi",0},
   { EVENTLO_TASK, event_lower_task,  1000, EVENTLO_PRIO, "eventlo",0},
   { SEMHI_TASK,   sem_higher_task,   1000, SEMHI_PRIO,   "semhi",  0},
   { SEMLO_TASK,   sem_lower_task,    1000, SEMLO_PRIO,   "semlo",  0},
   { LWSEMHI_TASK, lwsem_higher_task, 1000, LWSEMHI_PRIO, "lwsemhi",0},
   { LWSEMLO_TASK, lwsem_lower_task,  1000, LWSEMLO_PRIO, "lwsemlo",0},
   { TASKQHI_TASK, taskq_higher_task, 1000, TASKQHI_PRIO, "taskqhi",0},
   { TASKQLO_TASK, taskq_lower_task,  1000, TASKQLO_PRIO, "taskqlo",0},
   { TIMERHI_TASK, timer_higher_task, 1000, TIMERHI_PRIO, "timerhi",0},
   { TIMERLO_TASK, timer_lower_task,  1000, TIMERLO_PRIO, "timerlo",0},
#if PSP_MQX_CPU_IS_KINETIS_K40 || PSP_MQX_CPU_IS_KINETIS_K60
   { KISRHI_TASK,  kisr_higher_task,  1000, KISRHI_PRIO,  "kisrhi", 0},
   { KISRLO_TASK,  kisr_lower_task,   1000, KISRLO_PRIO,  "kisrlo", 0},
#endif   
   { 0,            0,                 0,     0,            0,       0}
};

static int gcount = 0;

unsigned char saved_isr[1024]; /* Saved space for kernel ISR */

volatile _pool_id  msg_pool;
volatile _queue_id lower_qid;
volatile _queue_id higher_qid;
volatile bool   lower_done;
volatile bool   higher_done;

LWSEM_STRUCT lwsem;
void        *taskq_ptr;
_task_id     higher_taskid;
_task_id     lower_taskid;

void (_CODE_PTR_       old_exception_handler)(void);

TD_STRUCT_PTR          td_ptr;
_psp_code_addr         result;
_psp_code_addr         table;
void                *event_ptr;
void                  *sem_ptr;
_mqx_uint              error;
_mqx_uint              vector;
_mqx_uint              ticks;
_mqx_uint              i;
_mqx_uint              loop_count;

void (_CODE_PTR_ old_isr)(void *);
void   *timer_isr_data;

/*ISR*-------------------------------------------------------------------
* 
* Task Name : msg_timer_isr
* Comments  :
*   replace the timer isr send a message to a task, restore the old
* isr, and chain to it
*
*END*----------------------------------------------------------------------*/

void msg_timer_isr
   (
      void   *dummy
   )
{/* Body */
    bool     result;
    
   MESSAGE_HEADER_STRUCT_PTR msg_ptr;

   msg_ptr = _msg_alloc(msg_pool);
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, (msg_ptr != NULL), "Test #6 Testing: msg_timer_isr.1: allocate message from pool operation");

   msg_ptr->TARGET_QID = lower_qid;
   result = _msgq_send(msg_ptr);
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == TRUE), "Test #6 Testing: msg_timer_isr.2: _msgq_send from ISR operation");
   
   msg_ptr = _msg_alloc(msg_pool);
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, (msg_ptr != NULL), "Test #6 Testing: msg_timer_isr.3: allocate message from pool operation");

   msg_ptr->TARGET_QID = higher_qid;
   result = _msgq_send(msg_ptr);
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == TRUE), "Test #6 Testing: msg_timer_isr.4: _msgq_send from ISR operation");

   /* Install old isr */
   _int_install_isr(BSP_TIMER_INTERRUPT_VECTOR, old_isr, timer_isr_data);

   /* Chain to previous isr, the kernels Timer ISR */
   (*old_isr)(timer_isr_data);

}/* Endbody */


/*TASK*-------------------------------------------------------------------
* 
* Task Name    : msg_lower_task
* Comments     :
*
*END*----------------------------------------------------------------------*/

void msg_lower_task
   (
      uint32_t parameter
   )
{/* Body */
   KERNEL_DATA_STRUCT_PTR kernel_data = _mqx_get_kernel_data();
   void   *msg_ptr;

   lower_done = FALSE;
   lower_qid = _msgq_open(0,0);
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, (lower_qid != MSGQ_NULL_QUEUE_ID), "Test #6 Testing: msg_lower_task.1: _msgq_open operation");

   msg_ptr = _msgq_receive(0,0);
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, (msg_ptr != NULL), "Test #6 Testing: msg_lower_task.2: _msgq_receive operation");
   
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, (kernel_data->IN_ISR == 0), "Test #6 Testing: msg_lower_task.3: kernel_data->IN_ISR should be 0");

   lower_done = TRUE;
} /* Endbody */


/*TASK*-------------------------------------------------------------------
* 
* Task Name    : msg_higher_task
* Comments     :
*
*END*----------------------------------------------------------------------*/

void msg_higher_task
   (
      uint32_t parameter
   )
{/* Body */
   KERNEL_DATA_STRUCT_PTR kernel_data = _mqx_get_kernel_data();
   void   *msg_ptr;

   higher_done = FALSE;
   higher_qid = _msgq_open(0,0);
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, (higher_qid != MSGQ_NULL_QUEUE_ID), "Test #6 Testing: msg_higher_task.1: _msgq_open operation");

   msg_ptr = _msgq_receive(0,0);
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, (msg_ptr != NULL), "Test #6 Testing: msg_higher_task.2: _msgq_receive operation");
   
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, (kernel_data->IN_ISR == 0), "Test #6 Testing: msg_higher_task.3: kernel_data->IN_ISR should be 0");

   higher_done = TRUE;
} /* Endbody */


/*ISR*-------------------------------------------------------------------
* 
* Task Name : event_timer_isr
* Comments  :
*   replace the timer isr set events, restore the old
* isr, and chain to it
*
*END*----------------------------------------------------------------------*/

void event_timer_isr
   (
      void   *event_ptr
   )
{/* Body */
   _mqx_uint result;

   result = _event_set(event_ptr, MAX_MQX_UINT);
   EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "event_set from ISR operation");

   /* Install old isr */
   _int_install_isr(BSP_TIMER_INTERRUPT_VECTOR, old_isr, timer_isr_data);

   /* Chain to previous isr, the kernels Timer ISR */
   (*old_isr)(timer_isr_data);

}/* Endbody */


/*TASK*-------------------------------------------------------------------
* 
* Task Name    : event_lower_task
* Comments     :
*
*END*----------------------------------------------------------------------*/

void event_lower_task
   (
      uint32_t parameter
   )
{/* Body */
   KERNEL_DATA_STRUCT_PTR kernel_data = _mqx_get_kernel_data();
   void                  *event_ptr;
   _mqx_uint              result;

   lower_done = FALSE;
   result =  _event_open("event", &event_ptr);
   EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MQX_OK), "Test #7 Testing: event_lower_task.1: _event_open operation");

   result = _event_wait_all(event_ptr, 0x1, 0);
   EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MQX_OK), "Test #7 Testing: event_lower_task.2: _event_wait_all operation");

   EU_ASSERT_REF_TC_MSG(tc_7_main_task, (kernel_data->IN_ISR == 0), "Test #7 Testing: event_lower_task.3: kernel_data->IN_ISR should be 0");

   lower_done = TRUE;
} /* Endbody */


/*TASK*-------------------------------------------------------------------
* 
* Task Name    : event_higher_task
* Comments     :
*
*END*----------------------------------------------------------------------*/

void event_higher_task
   (
      uint32_t parameter
   )
{/* Body */
   KERNEL_DATA_STRUCT_PTR kernel_data = _mqx_get_kernel_data();
   void                  *event_ptr;
   _mqx_uint              result;

   higher_done = FALSE;
   result = _event_open("event", &event_ptr);
   EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MQX_OK), "Test #7 Testing: event_higher_task.1: _event_open operation");

   result = _event_wait_all(event_ptr, 0x2, 0);
   EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MQX_OK), "Test #7 Testing: event_higher_task.2: _event_wait_all operation");

   EU_ASSERT_REF_TC_MSG(tc_7_main_task, (kernel_data->IN_ISR == 0), "Test #7 Testing: event_higher_task.3: kernel_data->IN_ISR should be 0");

   higher_done = TRUE;
} /* Endbody */


/*ISR*-------------------------------------------------------------------
* 
* Task Name : sem_timer_isr
* Comments  :
*   replace the timer isr post sems, restore the old
* isr, and chain to it
*
*END*----------------------------------------------------------------------*/

void sem_timer_isr
   (
      void   *sem_ptr
   )
{/* Body */
   _mqx_uint result;

   result = _sem_post(sem_ptr);
   EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == MQX_OK), "Test #8 Testing: sem_timer_isr.1: _sem_post from ISR operation");

   result = _sem_post(sem_ptr);
   EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == MQX_OK), "Test #8 Testing: sem_timer_isr.2: _sem_post from ISR operation");

   /* Install old isr */
   _int_install_isr(BSP_TIMER_INTERRUPT_VECTOR, old_isr, timer_isr_data);

   /* Chain to previous isr, the kernels Timer ISR */
   (*old_isr)(timer_isr_data);
}/* Endbody */

/*TASK*-------------------------------------------------------------------
* 
* Task Name    : sem_lower_task
* Comments     :
*
*END*----------------------------------------------------------------------*/

void sem_lower_task
   (
      uint32_t parameter
   )
{/* Body */
   KERNEL_DATA_STRUCT_PTR kernel_data = _mqx_get_kernel_data();
   void   *sem_ptr;
   _mqx_uint result;

   lower_done = FALSE;
   result =  _sem_open("sem", &sem_ptr);
   EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == MQX_OK), "Test #8 Testing: sem_lower_task.1: _sem_open operation");

   result = _sem_wait(sem_ptr, 0);
   EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == MQX_OK), "Test #8 Testing: sem_lower_task.2: _sem_wait operation");

   EU_ASSERT_REF_TC_MSG(tc_8_main_task, (kernel_data->IN_ISR == 0), "Test #8 Testing: sem_lower_task.3: kernel_data->IN_ISR should be 0");

   lower_done = TRUE;
} /* Endbody */


/*TASK*-------------------------------------------------------------------
* 
* Task Name    : sem_higher_task
* Comments     :
*
*END*----------------------------------------------------------------------*/

void sem_higher_task
   (
      uint32_t parameter
   )
{/* Body */
   KERNEL_DATA_STRUCT_PTR kernel_data = _mqx_get_kernel_data();
   void   *sem_ptr;
   _mqx_uint result;

   higher_done = FALSE;
   result = _sem_open("sem", &sem_ptr);
   EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == MQX_OK), "Test #8 Testing: sem_higher_task.1: _sem_open operation");

   result = _sem_wait(sem_ptr, 0);
   EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == MQX_OK), "Test #8 Testing: sem_higher_task.2: _sem_wait operation");

   EU_ASSERT_REF_TC_MSG(tc_8_main_task, (kernel_data->IN_ISR == 0), "Test #8 Testing: sem_higher_task.: kernel_data->IN_ISR should be 0");

   higher_done = TRUE;
} /* Endbody */


/*ISR*-------------------------------------------------------------------
* 
* Task Name : lwsem_timer_isr
* Comments  :
*   replace the timer isr post lwsems, restore the old
* isr, and chain to it
*
*END*----------------------------------------------------------------------*/

void lwsem_timer_isr
   (
      void   *dummy
   )
{/* Body */
   _mqx_uint result;

   result = _lwsem_post(&lwsem);
   EU_ASSERT_REF_TC_MSG(tc_9_main_task, (result == MQX_OK), "Test #9 Testing: lwsem_timer_isr.1: _lwsem_post from ISR opertation");

   result = _lwsem_post(&lwsem);
   EU_ASSERT_REF_TC_MSG(tc_9_main_task, (result == MQX_OK), "Test #9 Testing: lwsem_timer_isr.2: _lwsem_post from ISR operation");

   /* Install old isr */
   _int_install_isr(BSP_TIMER_INTERRUPT_VECTOR, old_isr, timer_isr_data);

   /* Chain to previous isr, the kernels Timer ISR */
   (*old_isr)(timer_isr_data);

}/* Endbody */


/*TASK*-------------------------------------------------------------------
* 
* Task Name    : lwsem_lower_task
* Comments     :
*
*END*----------------------------------------------------------------------*/

void lwsem_lower_task
   (
      uint32_t parameter
   )
{/* Body */
   KERNEL_DATA_STRUCT_PTR kernel_data = _mqx_get_kernel_data();
   _mqx_uint result;

   lower_done = FALSE;
   result = _lwsem_wait(&lwsem);
   EU_ASSERT_REF_TC_MSG(tc_9_main_task, (result == MQX_OK), "Test #9 Testing: lwsem_lower_task.1: _lwsem_wait operation");

   EU_ASSERT_REF_TC_MSG(tc_9_main_task, (kernel_data->IN_ISR == 0), "Test #9 Testing: lwsem_lower_task.: kernel_data->IN_ISR should be 0");

   lower_done = TRUE;
} /* Endbody */


/*TASK*-------------------------------------------------------------------
* 
* Task Name    : lwsem_higher_task
* Comments     :
*
*END*----------------------------------------------------------------------*/

void lwsem_higher_task
   (
      uint32_t parameter
   )
{/* Body */
   KERNEL_DATA_STRUCT_PTR kernel_data = _mqx_get_kernel_data();
   _mqx_uint result;

   higher_done = FALSE;
   result = _lwsem_wait(&lwsem);
   EU_ASSERT_REF_TC_MSG(tc_9_main_task, (result == MQX_OK), "Test #9 Testing: lwsem_higher_task.1: _lwsem_wait operation");

   EU_ASSERT_REF_TC_MSG(tc_9_main_task, (kernel_data->IN_ISR == 0), "Test #9 Testing: lwsem_higher_task.2: kernel_data->IN_ISR should be 0");

   higher_done = TRUE;
} /* Endbody */


/*ISR*-------------------------------------------------------------------
* 
* Task Name : taskq_timer_isr
* Comments  :
*   replace the timer isr resume some tasks, restore the old
* isr, and chain to it
*
*END*----------------------------------------------------------------------*/

void taskq_timer_isr
   (
      void   *taskq_ptr
   )
{/* Body */
   _mqx_uint result;

   result = _taskq_resume(taskq_ptr, TRUE);
   EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == MQX_OK), "Test #10 Testing: taskq_timer_isr.1: _taskq_resume from ISR operation");

   /* Install old isr */
   _int_install_isr(BSP_TIMER_INTERRUPT_VECTOR, old_isr, timer_isr_data);

   /* Chain to previous isr, the kernels Timer ISR */
   (*old_isr)(timer_isr_data);

}/* Endbody */


/*TASK*-------------------------------------------------------------------
* 
* Task Name    : taskq_lower_task
* Comments     :
*
*END*----------------------------------------------------------------------*/

void taskq_lower_task
   (
      uint32_t parameter
   )
{/* Body */
   KERNEL_DATA_STRUCT_PTR kernel_data = _mqx_get_kernel_data();
   _mqx_uint result;

   lower_done = FALSE;
   result = _taskq_suspend(taskq_ptr);
   EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == MQX_OK), "Test #10 Testing: taskq_lower_task.1: _taskq_suspend operation");

   EU_ASSERT_REF_TC_MSG(tc_10_main_task, (kernel_data->IN_ISR == 0), "Test #10 Testing: taskq_lower_task.2: kernel_data->IN_ISR should be 0");

   lower_done = TRUE;

} /* Endbody */


/*TASK*-------------------------------------------------------------------
* 
* Task Name    : taskq_higher_task
* Comments     :
*
*END*----------------------------------------------------------------------*/

void taskq_higher_task
   (
      uint32_t parameter
   )
{/* Body */
   KERNEL_DATA_STRUCT_PTR kernel_data = _mqx_get_kernel_data();
   _mqx_uint result;

   higher_done = FALSE;
   result = _taskq_suspend(taskq_ptr);
   EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == MQX_OK), "Test #10 Testing: taskq_higher_task.1: _taskq_suspend operation");

   EU_ASSERT_REF_TC_MSG(tc_10_main_task, (kernel_data->IN_ISR == 0), "Test #10 Testing: taskq_higher_task.2: kernel_data->IN_ISR should be 0");

   higher_done = TRUE;
} /* Endbody */


/*ISR*-------------------------------------------------------------------
* 
* Task Name : timer_timer_isr
* Comments  :
*   replace the timer isr time dequeue, restore the old
* isr, and chain to it
*
*END*----------------------------------------------------------------------*/

void timer_timer_isr
   (
      void   *dummy
   )
{/* Body */
   TD_STRUCT_PTR td_ptr;

   td_ptr = _task_get_td(higher_taskid);
   EU_ASSERT_REF_TC_MSG(tc_11_main_task, (td_ptr != NULL), "Test #11 Testing: timer_timer_isr.1: _task_get_td from ISR operation");

   _time_dequeue_td(td_ptr);
   td_ptr->STATE = BLOCKED;
   _task_ready(td_ptr);

   td_ptr = _task_get_td(lower_taskid);
   EU_ASSERT_REF_TC_MSG(tc_11_main_task, (td_ptr != NULL), "Test #11 Testing: timer_timer_isr.2: _task_get_td from ISR operation");

   _time_dequeue_td(td_ptr);
   td_ptr->STATE = BLOCKED;
   _task_ready(td_ptr);

   /* Install old isr */
   _int_install_isr(BSP_TIMER_INTERRUPT_VECTOR, old_isr, timer_isr_data);

   /* Chain to previous isr, the kernels Timer ISR */
   (*old_isr)(timer_isr_data);

}/* Endbody */


/*TASK*-------------------------------------------------------------------
* 
* Task Name    : timer_lower_task
* Comments     :
*
*END*----------------------------------------------------------------------*/

void timer_lower_task
   (
      uint32_t parameter
   )
{/* Body */
   KERNEL_DATA_STRUCT_PTR kernel_data = _mqx_get_kernel_data();


   lower_done = FALSE;
   lower_taskid = _task_get_id();
   _time_delay_ticks(1000);
   EU_ASSERT_REF_TC_MSG(tc_11_main_task, (kernel_data->IN_ISR == 0), "Test #11 Testing: timer_lower_task.1: kernel_data->IN_ISR should be 0");

   lower_done = TRUE;
} /* Endbody */


/*TASK*-------------------------------------------------------------------
* 
* Task Name    : timer_higher_task
* Comments     :
*
*END*----------------------------------------------------------------------*/

void timer_higher_task
   (
      uint32_t parameter
   )
{/* Body */
   KERNEL_DATA_STRUCT_PTR kernel_data = _mqx_get_kernel_data();

   higher_done = FALSE;
   higher_taskid = _task_get_id();
   _time_delay_ticks(1000);
   EU_ASSERT_REF_TC_MSG(tc_11_main_task, (kernel_data->IN_ISR == 0), "Test #11 Testing: timer_higher_task.1: kernel_data->IN_ISR should be 0");

   higher_done = TRUE;

} /* Endbody */

#if PSP_MQX_CPU_IS_KINETIS_K40 || PSP_MQX_CPU_IS_KINETIS_K60

void kisr_lower_task(uint32_t parameter) {
    KERNEL_DATA_STRUCT_PTR kernel_data = _mqx_get_kernel_data();
    int acount;
    PIT_MemMapPtr pit = PIT_BASE_PTR;

    lower_done = FALSE;
    lower_taskid = _task_get_id();
    
    acount = gcount;
   
    pit->MCR = 0;
    pit->CHANNEL[0].LDVAL = BSP_BUS_CLOCK / 100;
    pit->CHANNEL[0].TFLG = PIT_TFLG_TIF_MASK;
    pit->CHANNEL[0].TCTRL = PIT_TCTRL_TEN_MASK | PIT_TCTRL_TIE_MASK;
    
    _time_delay(500);
    
    EU_ASSERT_REF_TC_MSG(tc_14_main_task, (acount < gcount), "Test #14 Testing: kernel isr in low priority task (isr prio is higher as task prio)");

    lower_done = TRUE;
}

void kisr_higher_task(uint32_t parameter) {
    KERNEL_DATA_STRUCT_PTR kernel_data = _mqx_get_kernel_data();
    int acount;
    PIT_MemMapPtr pit = PIT_BASE_PTR;

    higher_done = FALSE;
    higher_taskid = _task_get_id();
    
    acount = gcount;
   
    pit->MCR = 0;
    pit->CHANNEL[0].LDVAL = BSP_BUS_CLOCK / 100;
    pit->CHANNEL[0].TFLG = PIT_TFLG_TIF_MASK;
    pit->CHANNEL[0].TCTRL = PIT_TCTRL_TEN_MASK | PIT_TCTRL_TIE_MASK;
    
    // waiting for interrupt - can't occure, because int prio is lower as task prio
    for (i = 0; i < 10000000; i++) {
        _ASM_NOP();
    }
    
    pit->CHANNEL[0].TCTRL = 0;      // disable timer
    pit->CHANNEL[0].TFLG = PIT_TFLG_TIF_MASK;       // clear flag
    
    EU_ASSERT_REF_TC_MSG(tc_14_main_task, (acount == gcount), "Test #14 Testing: kernel isr in high priority task (isr prio is lower as task prio)");

        
    higher_done = TRUE;
}

void count_kernel_isr(void) {
    PIT_MemMapPtr pit = PIT_BASE_PTR;
    gcount++;
    
    pit->CHANNEL[0].TCTRL = 0;      // disable timer
    pit->CHANNEL[0].TFLG = PIT_TFLG_TIF_MASK;       // clear flag
}

#endif // PSP_MQX_CPU_IS_KINETIS_K40 || PSP_MQX_CPU_IS_KINETIS_K60

/*FUNCTION*---------------------------------------------------------------
* 
* Function Name: my_kernel_isr
* Return value : none
* Comments     :
*
*END*----------------------------------------------------------------------*/

void my_kernel_isr
   (
      void
   )
{/* Body */

    _task_set_error(MQX_OK);
}/* Endbody */



/******   Test cases body definition      *******/

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_1_main_task
// Comments     : TEST # 1 - Testing _int_disable 
// tc_1_main_task(void) is increasing variable DISABLED_LEVEL from 0 to 2
// TIME.TICKS[0] and TIME.TICKS[1] depend on "ENDIAN"
// 
// EU_ASSERT is used instead EU_ASSERT_REF_TC_MSG, because EU_ASSERT_REF_TC_MSG
// modify DISABLED_LEVEL
//END---------------------------------------------------------------------
 void tc_1_main_task(void)
 {
   volatile KERNEL_DATA_STRUCT      *kernel_data = _mqx_get_kernel_data();

   _int_disable();
   EU_ASSERT_REF_TC_MSG(tc_1_main_task,td_ptr->DISABLED_LEVEL == (1+1),"Test #1 Testing: 1.1: td_ptr->DISABLED_LEVEL should be 1 after doing _int_disable() 1 time +1 additional for ASSERT");
   
   _int_disable();
   EU_ASSERT_REF_TC_MSG(tc_1_main_task,td_ptr->DISABLED_LEVEL == (2+1),"Test #1 Testing: 1.2: td_ptr->DISABLED_LEVEL should be 2 after doing _int_disable() 2 times +1 additional for ASSERT");
   
   ticks = kernel_data->TIME.TICKS[TIME_TICKS_LSB_INDEX];
   for (i = 0; i < loop_count; ++i) {
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, ticks == kernel_data->TIME.TICKS[TIME_TICKS_LSB_INDEX],"Test #1 Testing: 1.3: Interrupts should be disabled. Compare ticks with kernel_data->TIME.TICKS[]");
      if (ticks != kernel_data->TIME.TICKS[TIME_TICKS_LSB_INDEX]) 
      {
         break;
      }/* Endif */
   } /* Endfor */
 }
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_2_main_task
// Comments     : TEST # 2 - Testing _int_enable 
// So , tc_2_main_task(void) is decreasing variable DISABLED_LEVEL from 2 to 0
// TIME.TICKS[0] and TIME.TICKS[1] depend on "ENDIAN"
// 
// EU_ASSERT is used instead EU_ASSERT_REF_TC_MSG, because EU_ASSERT_REF_TC_MSG
// modify DISABLED_LEVEL
//END---------------------------------------------------------------------
 void tc_2_main_task(void)
 {
   volatile KERNEL_DATA_STRUCT      *kernel_data = _mqx_get_kernel_data();

   _int_enable();
   EU_ASSERT_REF_TC_MSG(tc_2_main_task,td_ptr->DISABLED_LEVEL == (1+1),"Test #2 Testing: 2.1: td_ptr->DISABLED_LEVEL should be 1 after doing _int_enable() 1 time +1 additional for ASSERT");
   _int_enable();
   EU_ASSERT_REF_TC_MSG(tc_2_main_task,td_ptr->DISABLED_LEVEL == (0+1),"Test #2 Testing: 2.2: td_ptr->DISABLED_LEVEL should be 0 after doing _int_enable() 2 times +1 additional for ASSERT");
   _int_enable();
   EU_ASSERT_REF_TC_MSG(tc_2_main_task,td_ptr->DISABLED_LEVEL == (0+1),"Test #2 Testing: 2.3: td_ptr->DISABLED_LEVEL should be 0 after doing _int_enable() 3 times +1 additional for ASSERT");
   
   ticks = kernel_data->TIME.TICKS[TIME_TICKS_LSB_INDEX];
   for (i = 0; i < loop_count; ++i) {
      if (ticks != kernel_data->TIME.TICKS[TIME_TICKS_LSB_INDEX]) {
         ticks = 0;
         break;
      }/* Endif */
   } /* Endfor */
   EU_ASSERT_REF_TC_MSG(tc_2_main_task,ticks == 0,"Test #2 Testing: 2.5: Interrupts should have been enabled. ticks should be 0");
}


//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_3_main_task
// Comments     : TEST # 3 - Testing _int_set_vector_table 
//
//END---------------------------------------------------------------------
 void tc_3_main_task(void)
 { 
   _int_disable();

   /* Note that on some CPUs the vector table cannot be modified */
   table  = _int_set_vector_table(0xA000);
   result = _int_get_vector_table();
   
#if defined(PSP_CPU_MPC5125)
   EU_ASSERT_REF_TC_MSG(tc_3_main_task,result != 0xA000,"Test #3 Testing: 3.1: _int_set_vector_table operation should fail");
#else
   EU_ASSERT_REF_TC_MSG(tc_3_main_task,result == 0xA000,"Test #3 Testing: 3.1: _int_set_vector_table operation");
#endif
   
   _int_set_vector_table(table);
   _int_enable();
   _task_set_error(MQX_OK);
 }
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_4_main_task
// Comments     : TEST # 4 - Testing _int_get_kernel_isr 
//
//END---------------------------------------------------------------------
 void tc_4_main_task(void)
 {
   volatile KERNEL_DATA_STRUCT      *kernel_data = _mqx_get_kernel_data();
#if !PSP_CPU_VF65GS10_A5
   _task_set_error(MQX_OK);
   vector = PSP_MAXIMUM_INTERRUPT_VECTORS;
   old_exception_handler = _int_get_kernel_isr(vector);
   /* get exception handler with too high vector number */
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, (_task_errno == MQX_INVALID_VECTORED_INTERRUPT && old_exception_handler == NULL), "Test #4 Testing: 4.1: Get exception handler with too high vector number");

   _task_set_error(MQX_OK);
   vector = PSP_MAXIMUM_INTERRUPT_VECTORS-1;
   /* get exception handler with ok high vector number */
   old_exception_handler = _int_get_kernel_isr(vector);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task,_task_errno == MQX_OK, "Test #4 Testing: 4.2: Get exception handler with ok high vector number");
#endif
   _task_set_error(MQX_OK);
 }
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_5_main_task
// Comments     : TEST # 5 - Testing _int_set_kernel_isr 
//
//END---------------------------------------------------------------------
void tc_5_main_task(void)
{
    volatile KERNEL_DATA_STRUCT      *kernel_data = _mqx_get_kernel_data();
#if !PSP_CPU_VF65GS10_A5
    //vector = PSP_MAXIMUM_INTERRUPT_VECTORS;
    //b23362 , PSP_MAXIMUM_INTERRUPT_VECTORS = 256 ????
    //b23362 , MQX_ROM_VECTORS = 0
    vector = PSP_MAXIMUM_INTERRUPT_VECTORS;
    //b23362
    
    _task_set_error(MQX_OK);
    old_exception_handler = _int_install_kernel_isr(vector, my_kernel_isr);

    /* set exception handler with too high vector number */
    #if MQX_ROM_VECTORS == 0
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (_task_errno == MQX_INVALID_VECTORED_INTERRUPT && old_exception_handler == NULL), "Test #5 Testing: 5.1: Set exception handler with too high vector number");
    #else
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (_task_errno == MQX_INVALID_CONFIGURATION && old_exception_handler == NULL), "Test #5 Testing: 5.1: _int_install_kernel_isr must failed because MQX_ROM_VECTORS = 1");
    #endif

    _task_set_error(MQX_OK);

    vector = PSP_MAXIMUM_INTERRUPT_VECTORS-1;
    /* set exception handler with ok high vector number */
    old_exception_handler = _int_get_kernel_isr(vector);

    _int_disable();

    old_exception_handler = _int_install_kernel_isr(vector, my_kernel_isr);

    #if MQX_ROM_VECTORS == 0
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (_task_errno == MQX_OK), "Test #5 Testing: 5.2: Set exception handler with ok high vector number");
    #else
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (_task_errno == MQX_INVALID_CONFIGURATION && old_exception_handler == NULL), "Test #5 Testing: 5.2: _int_install_kernel_isr must failed because MQX_ROM_VECTORS = 1");
    #endif

    _int_install_kernel_isr(vector, old_exception_handler);
    _int_enable();
#endif    
    _task_set_error(MQX_OK);
}
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_6_main_task
// Comments     : TEST # 6 - Testing _msg_send from an ISR 
//
//END---------------------------------------------------------------------
 void tc_6_main_task(void)
 {
   volatile KERNEL_DATA_STRUCT      *kernel_data = _mqx_get_kernel_data();

   lower_done  = FALSE;
   higher_done = FALSE;
   msg_pool = _msgpool_create(sizeof(MESSAGE_HEADER_STRUCT), 2, 0, 2);
   /* Create the tasks */
   _task_create(0, MSGHI_TASK, 0);
   _task_create(0, MSGLO_TASK, 0);
   /* Let them run and create queues, and wait for a message */
   _sched_yield();
   _int_disable();
   /* Install interrupt handler that sends message */
   timer_isr_data = _int_get_isr_data(BSP_TIMER_INTERRUPT_VECTOR);
   old_isr = _int_install_isr(BSP_TIMER_INTERRUPT_VECTOR, msg_timer_isr, 0);
   EU_ASSERT_REF_TC_MSG(tc_6_main_task,old_isr != NULL,"Test #6 Testing: 6.1: Install interrupt handler that sends message");

   _int_enable();
   while (!lower_done) {
      _sched_yield();
   } /* Endwhile */ 
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, higher_done, "Test #6 Testing: 6.2: Higher priority task should return TRUE");
}
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_7_main_task
// Comments     : TEST # 7 - Testing _event_set from an ISR 
//
//END---------------------------------------------------------------------
 void tc_7_main_task(void)
 {
   volatile KERNEL_DATA_STRUCT      *kernel_data = _mqx_get_kernel_data();

   lower_done  = FALSE;
   higher_done = FALSE;
   error = _event_create("event");
   EU_ASSERT_REF_TC_MSG(tc_7_main_task,error == MQX_OK,"Test #7 Testing: 7.1: Create event");

   error = _event_open("event", &event_ptr);
   EU_ASSERT_REF_TC_MSG(tc_7_main_task,error == MQX_OK,"Test #7 Testing: 7.2: Open event");

   /* Create the tasks */
   _task_create(0, EVENTHI_TASK, 0);
   _task_create(0, EVENTLO_TASK, 0);
   /* Let them run and get ready */
   _sched_yield();
   _int_disable();
   /* Install interrupt handler that sends message */
   timer_isr_data = _int_get_isr_data(BSP_TIMER_INTERRUPT_VECTOR);
   old_isr = _int_install_isr(BSP_TIMER_INTERRUPT_VECTOR, event_timer_isr, event_ptr);
   EU_ASSERT_REF_TC_MSG(tc_7_main_task,old_isr != NULL,"Test #7 Testing: 7.3: Install interrupt handler that sends message");

   _int_enable();
   while (!lower_done) {
      _sched_yield();
   } /* Endwhile */ 
   EU_ASSERT_REF_TC_MSG(tc_7_main_task, higher_done, "Test #7 Testing: 7.4: Higher priority task should return TRUE");
}
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_8_main_task
// Comments     : TEST # 8 - Testing _sem_post from an ISR
//
//END---------------------------------------------------------------------
 void tc_8_main_task(void)
 {
   volatile KERNEL_DATA_STRUCT      *kernel_data = _mqx_get_kernel_data();

   lower_done  = FALSE;
   higher_done = FALSE;
   error = _sem_create("sem", 0, 0);
   EU_ASSERT_REF_TC_MSG(tc_8_main_task,error == MQX_OK,"Test #8 Testing: 8.1: Create sem");

   error = _sem_open("sem", &sem_ptr);
   EU_ASSERT_REF_TC_MSG(tc_8_main_task,error == MQX_OK,"Test #8 Testing: 8.2: Open sem");

   /* Create the tasks */
   _task_create(0, SEMHI_TASK, 0);
   _task_create(0, SEMLO_TASK, 0);
   /* Let them run and get ready */
   _sched_yield();
   _int_disable();
   /* Install interrupt handler that sends message */
   timer_isr_data = _int_get_isr_data(BSP_TIMER_INTERRUPT_VECTOR);
   old_isr = _int_install_isr(BSP_TIMER_INTERRUPT_VECTOR, sem_timer_isr, sem_ptr);
   EU_ASSERT_REF_TC_MSG(tc_8_main_task,old_isr != NULL,"Test #8 Testing: 8.3: Install interrupt handler that sends message");

   _int_enable();
   while (!lower_done) {
      _sched_yield();
   } /* Endwhile */ 
   EU_ASSERT_REF_TC_MSG(tc_8_main_task, higher_done, "Test #8 Testing: 8.4: Higher priority task should return TRUE");
}
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_9_main_task
// Comments     : TEST # 9 - Testing _lwsem_post from an ISR 
//
//END---------------------------------------------------------------------
 void tc_9_main_task(void)
 {
   volatile KERNEL_DATA_STRUCT      *kernel_data = _mqx_get_kernel_data();

   lower_done  = FALSE;
   higher_done = FALSE;
   error = _lwsem_create(&lwsem, 0);
   EU_ASSERT_REF_TC_MSG(tc_9_main_task,error == MQX_OK,"Test #9 Testing: 9.1: Create lwsem");

   /* Create the tasks */
   _task_create(0, LWSEMHI_TASK, 0);
   _task_create(0, LWSEMLO_TASK, 0);
   /* Let them run and get ready */
   _sched_yield();
   _int_disable();
   /* Install interrupt handler that sends message */
   timer_isr_data = _int_get_isr_data(BSP_TIMER_INTERRUPT_VECTOR);
   old_isr = _int_install_isr(BSP_TIMER_INTERRUPT_VECTOR, lwsem_timer_isr, 0);
   EU_ASSERT_REF_TC_MSG(tc_9_main_task,old_isr != NULL,"Test #9 Testing: 9.2: Install interrupt handler that sends message");

   _int_enable();
   while (!lower_done) {
      _sched_yield();
   } /* Endwhile */ 
   EU_ASSERT_REF_TC_MSG(tc_9_main_task, higher_done, "Test #9 Testing: 9.3: Higher priority task should return TRUE");
}
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_10_main_task
// Comments     : TEST # 10 - Testing _taskq_resume from an ISR 
//
//END---------------------------------------------------------------------
 void tc_10_main_task(void)
 {
   volatile KERNEL_DATA_STRUCT      *kernel_data = _mqx_get_kernel_data();

   lower_done  = FALSE;
   higher_done = FALSE;
   taskq_ptr = _taskq_create(0);
   EU_ASSERT_REF_TC_MSG(tc_10_main_task,taskq_ptr != NULL,"Test #10 Testing: 10.1: Create taskq");

   /* Create the tasks */
   _task_create(0, TASKQHI_TASK, 0);
   _task_create(0, TASKQLO_TASK, 0);
   /* Let them run and get ready */
   _sched_yield();
   _int_disable();
   /* Install interrupt handler that sends message */
   timer_isr_data = _int_get_isr_data(BSP_TIMER_INTERRUPT_VECTOR);
   old_isr = _int_install_isr(BSP_TIMER_INTERRUPT_VECTOR, taskq_timer_isr, taskq_ptr);
   EU_ASSERT_REF_TC_MSG(tc_10_main_task,old_isr != NULL,"Test #10 Testing: 10.2: Install interrupt handler that sends message");

   _int_enable();
   while (!lower_done) {
      _sched_yield();
   } /* Endwhile */ 
   EU_ASSERT_REF_TC_MSG(tc_10_main_task,higher_done,"Test #10 Testing: 10.3: Higher priority task should return TRUE");
}
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_11_main_task
// Comments     : TEST # 11 - Testing _time_dequeue from an ISR 
//
//END---------------------------------------------------------------------
 void tc_11_main_task(void)
 {
   volatile KERNEL_DATA_STRUCT      *kernel_data = _mqx_get_kernel_data();

   lower_done  = FALSE;
   higher_done = FALSE;
   /* Create the tasks */
   _task_create(0, TIMERHI_TASK, 0);
   _task_create(0, TIMERLO_TASK, 0);
   /* Let them run and get ready */
   _sched_yield();
   _int_disable();
   /* Install interrupt handler that dequeues them */
   timer_isr_data = _int_get_isr_data(BSP_TIMER_INTERRUPT_VECTOR);
   old_isr = _int_install_isr(BSP_TIMER_INTERRUPT_VECTOR, timer_timer_isr, NULL);
   EU_ASSERT_REF_TC_MSG(tc_11_main_task,old_isr != NULL,"Test #11 Testing: 11.1: Install interrupt handler that dequeues them");

   _int_enable();
   while (!lower_done) {
      _sched_yield();
   } /* Endwhile */ 
   EU_ASSERT_REF_TC_MSG(tc_11_main_task,higher_done,"Test #11 Testing: 11.2: Higher priority task should retrun TRUE");
}
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_12_main_task
// Comments     : TEST # 12 - Testing _int_get_previous_vector_table 
//
//END---------------------------------------------------------------------

void tc_12_main_task(void)
{	
	KERNEL_DATA_STRUCT_PTR kernel_data = _mqx_get_kernel_data();

#if !defined(PSP_CPU_MPC5125)
    result = _int_get_previous_vector_table();
    EU_ASSERT_REF_TC_MSG(tc_12_main_task, result == (_psp_code_addr)kernel_data->USERS_VBR, "Test #12 Testing the function  _int_get_previous_vector_table");
#endif

    _task_set_error(MQX_OK);
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_13_main_task
// Comments     : TEST # 13 - Testing _int_get_vector_table 
//
//END---------------------------------------------------------------------

void tc_13_main_task(void)
{
#if !defined(PSP_CPU_MPC5125)
    result = _int_get_vector_table();
    EU_ASSERT_REF_TC_MSG(tc_13_main_task, result == table, "Test #13 Testing the function  _int_get_vector_table");
#endif

    _task_set_error(MQX_OK);
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_14_main_task
// Comments     : TEST # 14 - Testing int suite
//
//END---------------------------------------------------------------------
void tc_14_main_task(void) {
#if PSP_MQX_CPU_IS_KINETIS_K40 || PSP_MQX_CPU_IS_KINETIS_K60 // PSP_MQX_CPU_IS_KINETIS_K40 || PSP_MQX_CPU_IS_KINETIS_K60
    void (_CODE_PTR_ old_isr)(void);
    volatile KERNEL_DATA_STRUCT      *kernel_data = _mqx_get_kernel_data();

    lower_done  = FALSE;
    higher_done = FALSE;
    vector = INT_PIT0;

    _int_disable();
    /* Install interrupt handler that dequeues them */
    old_isr = _int_install_kernel_isr(vector, count_kernel_isr);

#if MQX_ROM_VECTORS == 0
    EU_ASSERT_REF_TC_MSG(tc_14_main_task,old_isr != NULL, "Test #14 Testing: _int_install_kernel_isr");
#else
    EU_ASSERT_REF_TC_MSG(tc_14_main_task, (_task_errno == MQX_INVALID_CONFIGURATION && old_isr == NULL), "Test #14 Testing: _int_install_kernel_isr must failed because MQX_ROM_VECTORS = 1");        
#endif

    if (old_isr == NULL || _task_errno != MQX_OK)
        return;
    
    _bsp_int_init(vector, 5, 0, TRUE);
    _int_enable();
    
    /* Create the tasks */
    _task_create(0, KISRLO_TASK, 0);
    
    /* Let them run and get ready */
    _sched_yield();
    
    while (!lower_done) {
        _sched_yield();
    }
    
    EU_ASSERT_REF_TC_MSG(tc_14_main_task, lower_done, "Test #14 Testing: low prio task end");
    
    _task_create(0, KISRHI_TASK, 0);

    while (!higher_done) {
        _sched_yield();
    }
    
    _bsp_int_disable(vector);
    _int_install_kernel_isr(vector, old_isr);
    
    EU_ASSERT_REF_TC_MSG(tc_14_main_task, higher_done, "Test #14 Testing: high prio task end");
#else
    EU_ASSERT_REF_TC_MSG(tc_14_main_task, TRUE, "Test #14 Testing: Test case cannot run on this platform");
#endif
}



//------------------------------------------------------------------------
// Define Test Suite 
 EU_TEST_SUITE_BEGIN(suite_int)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST # 1 - Testing _int_disable"),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST # 2 - Testing _int_enable"),
    EU_TEST_CASE_ADD(tc_3_main_task, " TEST # 3 - Testing _int_set_vector_table"),
    EU_TEST_CASE_ADD(tc_4_main_task, " TEST # 4 - Testing _int_get_kernel_isr"),
    EU_TEST_CASE_ADD(tc_5_main_task, " TEST # 5 - Testing _int_set_kernel_isr"),
    EU_TEST_CASE_ADD(tc_6_main_task, " TEST # 6 - Testing _msg_send from an ISR"),
    EU_TEST_CASE_ADD(tc_7_main_task, " TEST # 7 - Testing _event_set from an ISR"),
    EU_TEST_CASE_ADD(tc_8_main_task, " TEST # 8 - Testing _sem_post from an ISR"),
    EU_TEST_CASE_ADD(tc_9_main_task, " TEST # 9 - Testing _lwsem_post from an ISR"),
    EU_TEST_CASE_ADD(tc_10_main_task, " TEST # 10 - Testing _taskq_resume from an ISR"),
    EU_TEST_CASE_ADD(tc_11_main_task, " TEST # 11 - Testing _time_dequeue from an ISR"),
    EU_TEST_CASE_ADD(tc_12_main_task, " TEST # 12 - Testing _int_get_previous_vector_table"),
    EU_TEST_CASE_ADD(tc_13_main_task, " TEST # 13 - Testing _int_get_vector_table"),
    EU_TEST_CASE_ADD(tc_14_main_task, " TEST # 14 - Testing kernel isr")
 EU_TEST_SUITE_END(suite_int)
// Add test suites
 EU_TEST_REGISTRY_BEGIN()
        EU_TEST_SUITE_ADD(suite_int, " - Test int suite")
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
{
   volatile KERNEL_DATA_STRUCT      *kernel_data = _mqx_get_kernel_data();
   
   _int_install_unexpected_isr();

   td_ptr = kernel_data->ACTIVE_PTR;

   /* Calibrate the cpu for # loops until timer fires */
   _time_delay_ticks(2);
   ticks = kernel_data->TIME.TICKS[TIME_TICKS_LSB_INDEX];
   
   for (i = 0; i < 0x10000000; ++i) {
      if (ticks != kernel_data->TIME.TICKS[TIME_TICKS_LSB_INDEX]) {
         break;
      }
   }
   
   loop_count = i * 2;
   
   //test_initialize();      
   //EU_RUN_ALL_TESTS(test_result_array); 
   //test_stop();
   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();     
} /* Endbody */

/* EOF */
