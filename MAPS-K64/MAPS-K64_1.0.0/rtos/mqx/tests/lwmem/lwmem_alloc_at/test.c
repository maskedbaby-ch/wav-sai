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
* $Date    : Jun-22-2012$
*
* Comments:
*
*   This file contains the source for the lwmem testing.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <mem_prv.h>
#include <mqx_prv.h>
#include <lwmem.h>
#include <lwmem_prv.h>
#include "util.h"
//#include <test_module.h>
#include "EUnit.h"
#include "eunit_crit_secs.h"


//------------------------------------------------------------------------
// required user_config settings
//------------------------------------------------------------------------
#if !MQX_USE_LWMEM_ALLOCATOR
    #error "Required MQX_USE_LWMEM_ALLOCATOR"
#endif


#define FILE_BASENAME   test

/* General defines */
#define MAIN_TASK  10

#define TEST_ALLOC_SIZE (200)


extern void    main_task(uint32_t);

void tc_1_main_task(void);// Test #1 - Testing the function _lwmem_alloc_at()
                         

TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
{ MAIN_TASK, main_task,  2000,   7, "Main_Task", MQX_AUTO_START_TASK},
{ 0,         0,          0,      0, 0,           0                  }
};

//----------------- Begin Testcases --------------------------------------
void 	                *ptr_0;
void      	            *ptr_1;
void       	            *ptr_2;
void         	            *ptr_alloc;

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_1_main_task
* Comments     : TEST 1: Test function: _lwmem_alloc_at
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
   _mqx_uint        result , size_min;
   uint32	local_address=0;
   
  
   //allocate 3 memory blocks
   ptr_0= _lwmem_alloc( TEST_ALLOC_SIZE );
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, ptr_0 != NULL, "Test 1: Testing 1.0: _lwmem_alloc operation")   
   if(!ptr_0){
   	return;
   }
   ptr_1= _lwmem_alloc( TEST_ALLOC_SIZE );
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, ptr_1 != NULL, "Test 1: Testing 1.1: _lwmem_alloc operation")   
   if(!ptr_1){
   		return;
   }
   ptr_2= _lwmem_alloc( TEST_ALLOC_SIZE );
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, ptr_2 != NULL, "Test 1: Testing 1.2: _lwmem_alloc operation")   
   if(!ptr_2){
   		return;
   }
    //deallocate ptr_1
   result = _lwmem_free(ptr_1);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == 0, "Test 1: Testing 1.3: _lwmem_free operation")   
   if(result != 0){
   		return;
   }
   /*allocation mem block smaller that the deallocated memory block inside the deallocated memory block*/
   size_min= LWMEM_MIN_MEMORY_STORAGE_SIZE;
   //calcullate start address for allocation
   local_address = (uint32)ptr_1+(uint32)(size_min+1);
   //allocate new block "ptr_alloc" inside "ptr_1" memory block
   ptr_alloc = _lwmem_alloc_at(TEST_ALLOC_SIZE-size_min-20,(void *)local_address);   
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, ptr_alloc != NULL, "Test 1: Testing 1.4: _lwmem_alloc_at operation allocate")   
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (uint32)ptr_alloc == local_address, "Test 1: Testing 1.5: _lwmem_alloc_at operation, allocate address")   
   if(ptr_alloc !=NULL){
		   result = _lwmem_free(ptr_alloc);   
           EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == 0, "Test 1: Testing 1.6: _lwmem_free operation")   
   }
   /*allocation mem block bigger that the deallocated memory block inside the deallocated memory block*/
   //allocate new block "ptr_alloc" inside "ptr_1" memory block
   ptr_alloc = _lwmem_alloc_at(TEST_ALLOC_SIZE,(void *)local_address);   
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, ptr_alloc == NULL, "Test 1: Testing 1.7: _lwmem_alloc_at operation allocate")   
   if(ptr_alloc !=NULL){
		return;		
   }
   /*allocation mem block inside allocadet memory block*/
   //allocate new block "ptr_alloc" inside "ptr_1" memory block
   local_address = (uint32)ptr_1-(uint32)(size_min+1);
   ptr_alloc = _lwmem_alloc_at(TEST_ALLOC_SIZE,(void *)local_address );   
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, ptr_alloc == NULL, "Test 1: Testing 1.8: _lwmem_alloc_at operation allocate")   
   if(ptr_alloc !=NULL){
		return;		
   }
   /*allocation mem block smaller that the deallocated memory block from the start+(size_min -1) of the deallocated memory block*/
   //calcullate start address for allocation
   local_address = (uint32)ptr_1+(uint32)(size_min-1);
   //allocate new block "ptr_alloc" inside "ptr_1" memory block
   ptr_alloc = _lwmem_alloc_at(TEST_ALLOC_SIZE-size_min-20,(void *)local_address);   
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, ptr_alloc == NULL, "Test 1: Testing 1.9: _lwmem_alloc_at operation allocate")   
   if(ptr_alloc !=NULL){
		return;
  }

   
   
}

//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_1)
    EU_TEST_CASE_ADD(tc_1_main_task, " Test #1 - Testing the function _lwmem_alloc_at()"),
EU_TEST_SUITE_END(suite_1)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_1, " - Test of _lwmem_alloc_at")
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


   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();
}

/* EOF */
