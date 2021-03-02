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
* $Version : 3.8.2.0$
* $Date    : Jun-22-2012$
*
* Comments:
*
*   This file contains the source for MPU testing.
*
*END************************************************************************/
#include <mqx.h>
#include <mqx_inc.h>
#include <bsp.h>
#include "test.h"
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"

#define FILE_BASENAME   test

TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
{ MAIN_TASK,   main_task,  1000, 9, "Main_Task",   MQX_AUTO_START_TASK  },
{ PRIV_TASK,   priv_task,  1000, 8, "Priv_Task",   0                    },
{ USER_TASK,   user_task,  1000, 8, "User_Task",   MQX_USER_TASK        },
{ 0,           0,          0,    0, 0,             0                    }
};

_mqx_uint   initial_rec, priv_result, user_result, error;
KERNEL_DATA_STRUCT_PTR  kernel_data;
USER_RO_ACCESS LWEVENT_STRUCT lwevent;
_mem_pool_id   pool_id;
USER_NO_ACCESS unsigned char g_mem[2*POOL_SIZE+sizeof(LWMEM_POOL_STRUCT)];

void priv_task(uint32_t parameter)
{
   _mqx_uint   result;
   _task_id    user_tid;
   unsigned char   *memory;
   
   priv_result = 0xFFFFFFFF;
   result = _usr_lwevent_create(&lwevent, 0xFFFF); //set autoclear
   if(result!= MQX_OK)
      _task_destroy(MQX_NULL_TASK_ID);   

   mpu_clear_rec(initial_rec);
   memory = _mem_alloc_from(pool_id, POOL_SIZE/2);
   user_tid = _task_create(0, USER_TASK, (uint32_t)memory);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, user_tid != MQX_NULL_TASK_ID, "Test #3: 3.2 Creating user task")

   result = _mem_set_pool_access(pool_id, POOL_USER_NO_ACCESS);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3: 3.3 Setting pool to USER_ON_ACCESS")
   result = _lwevent_set(&lwevent, FIRE_USER);
   if(result!= MQX_OK)
      _task_destroy(MQX_NULL_TASK_ID);
   result = _lwevent_wait_for(&lwevent, FIRE_PRIV, TRUE, NULL);
   if(result!= MQX_OK)
      _task_destroy(MQX_NULL_TASK_ID);

   mpu_clear_rec(initial_rec);
   result = _mem_set_pool_access(pool_id, POOL_USER_RO_ACCESS);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3: 3.7 Setting pool to USER_RO_ACCESS")
   result = _lwevent_set(&lwevent, FIRE_USER);
   if(result!= MQX_OK)
      _task_destroy(MQX_NULL_TASK_ID);
   result = _lwevent_wait_for(&lwevent, FIRE_PRIV, TRUE, NULL);
   if(result!= MQX_OK)
      _task_destroy(MQX_NULL_TASK_ID);

   mpu_clear_rec(initial_rec);
   result = _mem_set_pool_access(pool_id, POOL_USER_RW_ACCESS);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3: 3.11 Setting pool to USER_NO_ACCESS")
   result = _lwevent_set(&lwevent, FIRE_USER);
   if(result!= MQX_OK)
      _task_destroy(MQX_NULL_TASK_ID);
   result = _lwevent_wait_for(&lwevent, FIRE_PRIV, TRUE, NULL);
   if(result!= MQX_OK)
      _task_destroy(MQX_NULL_TASK_ID);

   priv_result = MQX_OK;
}

void user_task(uint32_t memory)
{
   _mqx_uint   result;
   unsigned char       *alloc, buff;
   user_result = 0xFFFFFFFF;
   
   result = _lwevent_wait_for(&lwevent, FIRE_USER, TRUE, NULL);
   if(result!= MQX_OK) 
      _task_destroy(MQX_NULL_TASK_ID);

   //testing read, write, alloc on USER_NO_ACCESS memory pool
   error = 0;
   buff = ((volatile unsigned char *)memory)[0];
   MULTI_NOP
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, error != 0, "Test #3: 3.4 Reading from NO pool must fail")
	 
	 // dummy write - optimalization issue in read size
	 ((volatile unsigned char *)memory)[0] = buff + 1;
	 
   error = 0;
   ((unsigned char *)memory)[0] = 0xAA;
   MULTI_NOP
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, error != 0, "Test #3: 3.5 Writing to NO pool must fail")
   alloc = _mem_alloc_from((_mem_pool_id)pool_id, POOL_SIZE/2);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, alloc == NULL, "Test #3: 3.6 Allocating from NO pool must fail")

   result = _lwevent_set(&lwevent, FIRE_PRIV);
   if(result!= MQX_OK) 
      _task_destroy(MQX_NULL_TASK_ID);
   result = _lwevent_wait_for(&lwevent, FIRE_USER, TRUE, NULL);
   if(result!= MQX_OK) 
      _task_destroy(MQX_NULL_TASK_ID);

   //testing read, write, alloc on USER_RO_ACCESS memory pool   
   error = 0;
   buff = ((volatile unsigned char *)memory)[0];
   MULTI_NOP
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, error == 0, "Test #3: 3.8 Reading from RO pool must pass")

	 // dummy write - optimalization issue in read size
	 ((volatile unsigned char *)memory)[0] = buff + 1;

   error = 0;
   ((unsigned char *)memory)[0] = 0xAA;
   MULTI_NOP
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, error != 0, "Test #3: 3.9 Writing to RO pool must fail")
   alloc = _mem_alloc_from((_mem_pool_id)pool_id, POOL_SIZE/2);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, alloc == NULL, "Test #3: 3.10 Allocating from RO pool must fail")
   
   result = _lwevent_set(&lwevent, FIRE_PRIV);
   if(result!= MQX_OK)
      _task_destroy(MQX_NULL_TASK_ID);
   result = _lwevent_wait_for(&lwevent, FIRE_USER, TRUE, NULL);
   if(result!= MQX_OK)
      _task_destroy(MQX_NULL_TASK_ID);

   //testing read, write, alloc on USER_RW_ACCESS memory pool
   error = 0;
   ((unsigned char *)memory)[0] = 0xAA;
   MULTI_NOP
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, error == 0, "Test #3: 3.12 Writing to RW pool must pass")
   error = 0;
   buff = ((volatile unsigned char *)memory)[0];
   MULTI_NOP
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error == 0) && (buff == 0xAA), "Test #3: 3.13 Reading from RW pool must pass")

   alloc = _mem_alloc_from((_mem_pool_id)pool_id, POOL_SIZE/2);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, alloc != NULL, "Test #3: 3.14 Allocating from RW pool must pass")
   
   result = _lwevent_set(&lwevent, FIRE_PRIV);
   if(result!= MQX_OK)
      _task_destroy(MQX_NULL_TASK_ID);
   
   user_result = MQX_OK;   
}

void tc_1_main_task(void)
{
   _mem_pool_id   pool;
   unsigned char      *memory;
   _mqx_uint      i,result;
   
   initial_rec = mpu_get_free();
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (initial_rec != 0) && (initial_rec < CORTEX_MPU_REC) , "Test #1: 1.1 Get initial count of occupied MPU records")
   _GET_KERNEL_DATA(kernel_data);
   
   memory = _mem_alloc(POOL_SIZE);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, memory != NULL , "Test #1: 1.2 Allocate memory for pool")
   pool = _mem_create_pool(memory, POOL_SIZE);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, pool != NULL , "Test #1: 1.3 Create pool")
   
   for(i=0; i < TEST_REC_CNT; i++){
      result = _mem_set_pool_access(pool, POOL_USER_RW_ACCESS);
   }
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: 1.4 Setting MPU regions for TEST_REC_CNT times should fail")
   
}

void tc_2_main_task(void)
{
   _mem_pool_id   pool[TEST_REC_CNT];
   unsigned char      *memory[TEST_REC_CNT];
   _mqx_uint      i,result;
   
   mpu_clear_all_rec();
   
   for(i=0; i< TEST_REC_CNT; i++){
      memory[i] = _mem_alloc(POOL_SIZE);
      pool[i] = _mem_create_pool(memory[i], POOL_SIZE);
      result = _mem_set_pool_access(pool[i], POOL_USER_RW_ACCESS);
   }
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result != MQX_OK, "Test #2: 2.1 Setting MPU regions for TEST_REC_CNT times should fail")
}

void tc_3_main_task(void)
{
   _task_id       priv_tid;
   
   mpu_clear_all_rec();
   pool_id = _mem_create_pool(g_mem, 2*POOL_SIZE);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, pool_id != NULL, "Test #3: 3.1 Creating global memory pool")
   priv_tid = _task_create(0, PRIV_TASK, 0);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, priv_tid != MQX_NULL_TASK_ID, "Test #3: 3.2 Creating privilege task")
   
   //testing tasks priv_task and user_task should be finished
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (user_result == MQX_OK) && (priv_result == MQX_OK), "Test #3: 3.15 Test tasks finished correctly")
}
//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_3)
   EU_TEST_CASE_ADD(tc_1_main_task, " Setting MPU on one memory pool" ),
   EU_TEST_CASE_ADD(tc_2_main_task, " Setting MPU on multiple memory pools" ),
   EU_TEST_CASE_ADD(tc_3_main_task, " Testing MPU settings" )
EU_TEST_SUITE_END(suite_3)
// Add test suites

EU_TEST_REGISTRY_BEGIN()
   EU_TEST_SUITE_ADD(suite_3, " Testing MPU")
EU_TEST_REGISTRY_END()
//------------------------------------------------------------------------


/*TASK*--------------------------------------------------------------
* 
* Task Name : main_task
* Comments  :
*
*END*--------------------------------------------------------------*/

void main_task
   (
      uint32_t dummy
  )
{
   _int_install_unexpected_isr();

   _int_install_isr(4, fault_isr, NULL);
   _int_install_isr(5, fault_isr, NULL);  
   SCB_SHCSR |= SCB_SHCSR_MEMFAULTENA_MASK | SCB_SHCSR_BUSFAULTENA_MASK;
      
   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();

}/* Endbody */


//get index of first free MPU record
_mqx_uint mpu_get_free(void)
{
   _mqx_uint   i;
   
   for(i=1; i<CORTEX_MPU_REC; i++){
      if( MPU_WORD(i,3) != MPU_WORD_VLD_MASK)
         return i;
   }
   return 0;
}

//clear MPU record with given index
void mpu_clear_rec(_mqx_uint index)
{
   MPU_WORD(index,3) = 0;
}

//clear all MPU records used by test application
//MPU records created by system are preserved, but initial_rec must be setted first, 
//using mpu_get_free at the beggining of test application
void mpu_clear_all_rec(void)
{
   _mqx_uint   i;
   for(i= initial_rec; i < CORTEX_MPU_REC; i++)
      MPU_WORD(i,3) = 0;
}

//modify access control for region rescribed by MPU record with given index
void mpu_modify_rec(_mqx_uint index, _mqx_uint flags)
{
   MPU_RGDAAC(index) = flags;
}

void fault_isr
(
  /*  [IN]  the parameter passed by the kernel */
  void   *parameter
)
{
   uint32_t *p;
    
   error++;
    
   if (SCB_CFSR & SCB_CFSR_PRECISERR_MASK) {
      p = (uint32_t*)__get_PSP();
      p[6] += 2;      // PC in stack, temporary solution, estimate 16bit instruction
   }
    
   SCB_CFSR |= (SCB_CFSR_PRECISERR_MASK | SCB_CFSR_IMPRECISERR_MASK);
}
/* EOF */
