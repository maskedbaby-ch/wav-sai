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
*   This file contains the source for the Structure Copy In ISR ktest.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <fio.h>
#include "util.h"
//#include <test_module.h>
#include "EUnit.h"
#include "eunit_crit_secs.h"


#define FILE_BASENAME test
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
#define FP_TASK 5

extern void fp_task(uint32_t);
//------------------------------------------------------------------------
// Test suite function
extern void tc_1_fp_task(void);// Test #1 - Test of Floating Point Structure Copies in ISRs and tasks
//------------------------------------------------------------------------
TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{ 
    {FP_TASK, fp_task, 1024, 5, "fp", MQX_AUTO_START_TASK, 0, 0},
    {0,         0,         0,    0, 0,      0,                   0, 0}
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

/*ISR*-----------------------------------------------------------
*
* ISR Name     : new_fp_isr
* Comments     :
*    This ISR replaces the existing timer ISR, performs a structure-
* to-structure copy, then calls the old timer ISR.
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
   
   /* perform structure to structure copy - doubleing point registers
    * should be used here for CodeWarrior if the ppc_no_fp_blockmove
    * pragma is off
    */
   temp_struct = fp_struct_1;
   
   /* Chain to previous notifier */
   (*isr_ptr->OLD_ISR)(isr_ptr->OLD_ISR_DATA);
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_1_fp_task
// Comments     : 
//
//END---------------------------------------------------------------------
void tc_1_fp_task(void)
 {
   uint32_t i, count = 0;
   volatile FP_COPY_STRUCT copy_struct;

   /* get original ISR and ISR data */
   fp_isr_struct.OLD_ISR = _int_get_isr (BSP_TIMER_INTERRUPT_VECTOR);
   fp_isr_struct.OLD_ISR_DATA = _int_get_isr_data (BSP_TIMER_INTERRUPT_VECTOR);
   EU_ASSERT_REF_TC_MSG(tc_1_fp_task,fp_isr_struct.OLD_ISR != NULL,"Test #1 Testing: 1.1: _int_get_isr should not returned NULL")
   if (fp_isr_struct.OLD_ISR == NULL)
   {
      eunit_test_stop();
      _mqx_exit(1);
   }

   /* initialize fp_struct_1 with non-zero values */
   for (i = 0; i < NUM_TEST_ARRAY_ENTRIES; i++)
   {
      fp_struct_1.test_array[i] = (i + 1) * 0x1111111111111111;
   }
   
   /* install new ISR */
   _int_install_isr (BSP_TIMER_INTERRUPT_VECTOR, new_fp_isr, (void *)&fp_isr_struct);
   
   while (count != (50 * 600))
   {
      /* copy zeros into local structure using doubleing point load/stores */
      copy_struct = fp_struct_0;      
      for (i = 0; i < NUM_TEST_ARRAY_ENTRIES; i++)
      {
         if (copy_struct.test_array[i] != 0x0000000000000000)
          goto done;
      }      
      ++count;
   }
done:
   /* check if all test passed */
   EU_ASSERT_REF_TC_MSG(tc_1_fp_task, (count == (50 * 600)),"Test #1 Testing: 1.2: copy_struct operation")
 }
//------------------------------------------------------------------------
// Define Test Suite
 EU_TEST_SUITE_BEGIN(suite_double3)
    EU_TEST_CASE_ADD(tc_1_fp_task, " Test #1 - Test of Floating Point Structure Copies in ISRs and tasks")
 EU_TEST_SUITE_END(suite_double3)
// Add test suites
 EU_TEST_REGISTRY_BEGIN()
        EU_TEST_SUITE_ADD(suite_double3, " - Test of structure copy in ISR")
 EU_TEST_REGISTRY_END()
/*TASK*-----------------------------------------------------
* 
* Task Name    : fp_task
* Comments     :
*    This task detects doubleing point errors when structure
* to structure copies occur from interrupt level.  You should
* verify that CodeWarrior is using doubleing point loads/stores
* to perform the structure-to-structure copies.
*
*END*-----------------------------------------------------*/
void fp_task
(
   uint32_t initial_data
)
{
   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();
   _mqx_exit(0);
}
/* EOF */
