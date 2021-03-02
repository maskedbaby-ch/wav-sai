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
* $Version : 3.8.1.0$
* $Date    : Aug-10-2012$
*
* Comments:
*
*   This file contains the source for the Structure Copy In Tasks ktest.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <fio.h>
#include <util.h>
//#include <test_module.h>
#include "EUnit.h"
#include "eunit_crit_secs.h"

/* force use of doubleing point registers for structure-to-structure copies */
#if defined(__CODEWARRIOR__)
#if (__option(ppc_no_fp_blockmove) != 0)
#pragma ppc_no_fp_blockmove off
#endif
#endif

/* adjust this macro if CodeWarrior does not generate doubleing point instructions
 * for the structure-to-structure copies.
 */
#define NUM_TEST_ARRAY_ENTRIES   8

/* Task IDs */
#define FP_TASK_0 5
#define FP_TASK_1 6
#define FILE_BASENAME test

extern void fp_task_0(uint32_t);
extern void fp_task_1(uint32_t);
void tc_1_fp_task_0(void);
void tc_1_fp_task_1(void);

TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{ 
    {FP_TASK_0, fp_task_0, 2*1024, 6, "fp0", MQX_AUTO_START_TASK, 0, 0},
    {FP_TASK_1, fp_task_1, 2*1024, 5, "fp1", 0, 0, 0},
    {0,         0,         0,    0, 0,     0, 0, 0}
};

/* used to store the original ISR and ISR data installed to the system timer */
typedef struct
{
   void   *OLD_ISR_DATA;
   void (_CODE_PTR_ OLD_ISR)(void *);
} FP_ISR_STRUCT, * FP_ISR_STRUCT_PTR;

/* used to perform structure-to-structure copies */
typedef struct
{
   uint64_t test_array[NUM_TEST_ARRAY_ENTRIES];
} FP_COPY_STRUCT, * FP_COPY_STRUCT_PTR;

volatile FP_ISR_STRUCT  fp_isr_struct;
volatile FP_COPY_STRUCT fp_struct_0 = {0};
volatile FP_COPY_STRUCT fp_struct_1;

LWSEM_STRUCT fp_sem;

/*ISR*-----------------------------------------------------------
*
* ISR Name     : new_fp_isr
* Comments     :
*    This ISR replaces the existing timer ISR, unblocks fp_task_1,
* then calls the old timer ISR.
*
*END*-----------------------------------------------------------*/
void new_fp_isr
(
   void   *user_isr_ptr
)
{
   FP_ISR_STRUCT_PTR isr_ptr;
   volatile FP_COPY_STRUCT temp_struct;
   
   isr_ptr = (FP_ISR_STRUCT_PTR)user_isr_ptr;
   
   /* unblock fp_task_1 */
   _lwsem_post (&fp_sem);
   
   /* Chain to previous notifier */
   (*isr_ptr->OLD_ISR)(isr_ptr->OLD_ISR_DATA);
}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_1_fp_task_0
* Comments     : TEST #1:
*
*END*---------------------------------------------------------------------*/
void tc_1_fp_task_0(void)
{ 
   uint32_t i, count = 0;
   _mqx_uint result;
   _task_id task_id;
   volatile FP_COPY_STRUCT task_0_struct;
    /* create the lightweight semaphore used to synchronize fp_task_1 to
    * the system clock */
   result = _lwsem_create (&fp_sem, 0);
   EU_ASSERT_REF_TC_MSG(tc_1_fp_task_0, result == MQX_OK, "Test #1: Testing 1.1: _lwsem_create() operation")

   /* create the second task */
   task_id = _task_create (0, FP_TASK_1, 0);
   EU_ASSERT_REF_TC_MSG(tc_1_fp_task_0, task_id != 0, "Test #1: Testing 1.2: _task_create() operation")
   
   /* get original ISR and ISR data */
   fp_isr_struct.OLD_ISR = _int_get_isr (BSP_TIMER_INTERRUPT_VECTOR);
   fp_isr_struct.OLD_ISR_DATA = _int_get_isr_data (BSP_TIMER_INTERRUPT_VECTOR);
   EU_ASSERT_REF_TC_MSG(tc_1_fp_task_0, fp_isr_struct.OLD_ISR != NULL, "Test #1: Testing 1.3: _int_get_isr() for BSP_TIMER_INTERRUPT_VECTOR")

   /* initialize fp_struct_1 with non-zero values */
   for (i = 0; i < NUM_TEST_ARRAY_ENTRIES; i++)
   {
      fp_struct_1.test_array[i] = (i + 1) * 0x1111111111111111;
   }
   
   /* install new ISR */
   _int_install_isr (BSP_TIMER_INTERRUPT_VECTOR, new_fp_isr, (void *)&fp_isr_struct);

   while (count != (50 * 60))
   {
      /* copy zeros into local structure using doubleing point loads/stores */
      task_0_struct = fp_struct_0;
      
      for (i = 0; i < NUM_TEST_ARRAY_ENTRIES; i++)
      {
         if (task_0_struct.test_array[i] != 0x0000000000000000)
            goto done;
      }      
      ++count;
   }
done:
    EU_ASSERT_REF_TC_MSG(tc_1_fp_task_0, (count == (50 * 60)), "Test #1: Testing 1.4: task_0_struct.test_array")
}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_1_fp_task_1
* Comments     : 
*
*END*---------------------------------------------------------------------*/
void tc_1_fp_task_1(void)
{
   volatile FP_COPY_STRUCT task_1_struct;
   _mqx_uint result;
   while (1)
   {
      /* wait for the next tick */
      result = _lwsem_wait (&fp_sem);
      EU_ASSERT_REF_TC_MSG(tc_1_fp_task_0, result == MQX_OK, "Test #1: Testing 1.5: _lwsem_wait operation")
      
      /* use doubleing point loads/stores to copy the structures */
      task_1_struct = fp_struct_1;
   }
   eunit_test_stop();
   _mqx_exit(0);
}

/******  Define Test Suite      *******/

 EU_TEST_SUITE_BEGIN(suite_double4)
    EU_TEST_CASE_ADD(tc_1_fp_task_0, " Test #1 - Test of Floating Point Structure Copies in multiple tasks")
 EU_TEST_SUITE_END(suite_double4)

/*******  Add test suites *******/
 EU_TEST_REGISTRY_BEGIN()
        EU_TEST_SUITE_ADD(suite_double4, " - Test of Floating Point Structure Copies in multiple tasks")
 EU_TEST_REGISTRY_END()
 
/*TASK*-----------------------------------------------------
* 
* Task Name    : fp_task_0
* Comments     :
*    This task performs a continual structure-to-structure copy
*
*END*-----------------------------------------------------*/
void fp_task_0
(
   uint32_t initial_data
)
{
   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();
   _mqx_exit(0);
}

/*TASK*-----------------------------------------------------
* 
* Task Name    : fp_task_1
* Comments     :
*    This task performs a structure-to-structure copy each
* time the system timer "ticks"
*
*END*-----------------------------------------------------*/
void fp_task_1
(
   uint32_t initial_data
)
{
  tc_1_fp_task_1();
}

/* EOF */
