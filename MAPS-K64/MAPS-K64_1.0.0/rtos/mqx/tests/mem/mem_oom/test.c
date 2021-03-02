/**HEADER********************************************************************
*
* Copyright (c) 2013 Freescale Semiconductor;
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
* $Version : 4.0.1$
* $Date    : Apr-16-2013$
*
* Comments:
*    This file contains the source for OUT_OF_MEMORY test in mem or lwmem. What the test is exactly for according to the
* specific file which specify the setting in user_config.h
*
*Requirement: MEM097, MEM004, MEM008, MEM012, MEM016, MEM020, MEM024, MEM028, MEM032, MEM036, MEM040, MEM44
* LWMEM002,LWMEM004,LWMEM006,LWMEM008,LWMEM010,LWMEM012,LWMEM014,LWMEM016,LWMEM018,LWMEM038,LWMEM041
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <mem_prv.h>
#include <mqx_prv.h>
#include "util.h"
#include "test.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"

//------------------------------------------------------------------------
// Test suite function prototype
void tc_1_main_task(void);  // TEST #1 - Testing the function _mem_alloc() or _lwmem_alloc()
void tc_2_main_task(void);  // TEST #2 - Testing the function _mem_alloc_from() or _lwmem_alloc_from()
void tc_3_main_task(void);  // TEST #3 - Testing the function _mem_alloc_zero() or _lwmem_alloc_zero()
void tc_4_main_task(void);  // TEST #4 - Testing the function _mem_alloc_zero_from() or _lwmem_alloc_zero_from()
void tc_5_main_task(void);  // TEST #5 - Testing the function _mem_alloc_system() or _lwmem_alloc_system()
void tc_6_main_task(void);  // TEST #6 - Testing the function _mem_alloc_system_from() or _lwmem_alloc_system_from()
void tc_7_main_task(void);  // TEST #7 - Testing the function _mem_alloc_system_zero() or _lwmem_alloc_system_zero()
void tc_8_main_task(void);  // TEST #8 - Testing the function _mem_alloc_system_zero_from() or _lwmem_alloc_system_zero_from()
void tc_9_main_task(void);  // TEST #9 - Testing the function _mem_alloc_align() or _lwmem_alloc_align()
void tc_10_main_task(void);  // TEST #10 - Testing the function _mem_alloc_align_from() or _lwmem_alloc_align_from()
void tc_11_main_task(void);  // TEST #11 - Testing the function _mem_alloc_at() or _lwmem_alloc_at()
//------------------------------------------------------------------------
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
{ MAIN_TASK, main_task,  MAIN_TASK_STACK,   7, "Main_Task", MQX_AUTO_START_TASK},
{ 0,         0,          0,      0, 0,           0                  }
};

//----------------- Begin Testcases --------------------------------------
_mqx_uint                   result;
MEMORY_ALLOC_INFO_STRUCT    info_ptr;
unsigned char                      *to_ptr;
unsigned char                      *tmp_ptr;
MEM_POINTER_LIST_STRUCT_PTR head_ptr;
MEM_POINTER_LIST_STRUCT_PTR temp_ptr;
unsigned char                       external_pool[MEM_EXT_POOL_SIZE];

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Test function: _mem_alloc or _lwmem_alloc
* Requirement ID : MEM097, MEM004, LWMEM002
*END*---------------------------------------------------------------------*/

void tc_1_main_task(void)
{
    /*****************************************************************
    ** The following code tests the following function(s):           *
    **                       _mem_alloc or _lwmem_alloc                     *
    ******************************************************************/
    /* test the default memory before allocating   */
    result=GENERAL_MEM_TEST;
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing: 1.1: _(lw)mem_test operation")
    /* allocate all available memory and test the memory again   */
    memory_alloc_all(&info_ptr);
    result=_task_set_error(MQX_OK);
    result=_task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing: 1.2: _task_set_error operation")
    result=GENERAL_MEM_TEST;
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing: 1.3: _(lw)mem_test operation")
    /* try to allocate which will lead to failure as expected and get error code  */
    to_ptr = (unsigned char *)_mem_alloc(TEST_ALLOC_SIZE);
    result=_task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OUT_OF_MEMORY)&&(to_ptr==NULL), "Test #1 Testing: 1.4: _(lw)mem_alloc operation for MQX_OUT_OF_MEMORY")
    result=_task_set_error(MQX_OK);
    result=_task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing: 1.5: _task_set_error operation")
    /*  free all memory allocated before and test the memory again   */
    memory_free_all(&info_ptr);
    result=GENERAL_MEM_TEST;
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing: 1.6: _(lw)mem_test operation")
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2: Test function: _mem_alloc_from or _lwmem_alloc_from
* Requirement ID : MEM097, MEM008, LWMEM012
*END*---------------------------------------------------------------------*/

void tc_2_main_task(void)
{
    /*  create a new pool and test its integrity   */
    ext_pool_id=_mem_create_pool(external_pool,MEM_EXT_POOL_SIZE);
    result=GENERAL_MEM_TEST;
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (ext_pool_id!=0), "Test #2 Testing: 2.1: _(lw)mem_create_pool operation")
     /*****************************************************************
        ** The following code tests the following function(s):           *
        **                       _mem_alloc_from  or _lwmem_alloc_from                   *
        ******************************************************************/
    /*  allocate memory from the created pool until fail and get the error code   */
    to_ptr = (unsigned char *)_mem_alloc_from(ext_pool_id,TEST_ALLOC_SIZE);
    if(to_ptr!=NULL)
    {
        head_ptr=(MEM_POINTER_LIST_STRUCT_PTR)to_ptr;
        head_ptr->PREV=NULL;
        while(TRUE)
        {
            result=GENERAL_MEM_TEST;
            if(result!=MQX_OK)
            {
                EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #2 Testing: 2.2: _(lw)mem_test operation")
                break;
            }
            to_ptr=(unsigned char *)_mem_alloc_from(ext_pool_id,TEST_ALLOC_SIZE);
            if(to_ptr==NULL)
            {
                result=_task_get_error();
                EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OUT_OF_MEMORY), "Test #2 Testing: 2.3: _(lw)mem_alloc_from operation for MQX_OUT_OF_MEMORY")
                break;
            }
            else
            {
                temp_ptr=head_ptr;
                head_ptr=(MEM_POINTER_LIST_STRUCT_PTR)to_ptr;
                head_ptr->PREV=temp_ptr;
            }
        }
    }
    else
    {
        result=_task_get_error();
        EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OUT_OF_MEMORY), "Test #2 Testing: 2.4: _(lw)mem_alloc_from operation for MQX_OUT_OF_MEMORY")
    }
    /*  free allocated memory before and test the pool's integrity again   */
    while(head_ptr)
    {
        temp_ptr=head_ptr->PREV;
        result=_mem_free(head_ptr);
        EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #2 Testing: 2.5: _(lw)mem_free operation")
        head_ptr=temp_ptr;
    }
    result=GENERAL_MEM_TEST;
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #2 Testing: 2.6: _(lw)mem_test operation")
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : TEST #3: Test function: _mem_alloc_zero or _lwmem_alloc_zero
* Requirement ID : MEM097, MEM012, LWMEM008
*END*---------------------------------------------------------------------*/

void tc_3_main_task(void)
{
    /*****************************************************************
    ** The following code tests the following function(s):           *
    **                       _mem_alloc_zero  or _lwmem_alloc_zero                    *
    ******************************************************************/
    /*  test the  default memory before allocating  */
    result=GENERAL_MEM_TEST;
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_OK), "Test #3 Testing: 3.1: _(lw)mem_test operation")
    /*  allocate all available memory and test the memory again  */
    memory_alloc_all(&info_ptr);
    result=_task_set_error(MQX_OK);
    result=_task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_OK), "Test #3 Testing: 3.2: _task_set_error operation")
    result=GENERAL_MEM_TEST;
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_OK), "Test #3 Testing: 3.3: _(lw)mem_test operation")
    /*  try to allocate which will lead to failure as expected and get error code */
    to_ptr = (unsigned char *)_mem_alloc_zero(TEST_ALLOC_SIZE);
    result=_task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_OUT_OF_MEMORY)&&(to_ptr==NULL), "Test #3 Testing: 3.4: _(lw)mem_alloc_zero operation for MQX_OUT_OF_MEMORY")
    result=_task_set_error(MQX_OK);
    result=_task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_OK), "Test #3 Testing: 3.5: _task_set_error operation")
    /*  free all memory allocated before and test the memory again  */
    memory_free_all(&info_ptr);
    result=GENERAL_MEM_TEST;
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_OK), "Test #3 Testing: 3.6: _(lw)mem_test operation")
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_4_main_task
* Comments     : TEST #4: Test function: _mem_alloc_zero_from or _lwmem_alloc_zero_from
* Requirement ID : MEM097, MEM016, LWMEM018
*END*---------------------------------------------------------------------*/

void tc_4_main_task(void)
{
    /*  test the integrity of created pool  */
    result=GENERAL_MEM_TEST;
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_OK), "Test #4 Testing: 4.1: _(lw)mem_test operation")
         /*****************************************************************
            ** The following code tests the following function(s):           *
            **              _mem_alloc_zero_from or _lwmem_alloc_zero_from                    *
            ******************************************************************/
    /*  allocate memory from the created pool until fail and get the error code  */
        to_ptr = (unsigned char *)_mem_alloc_zero_from(ext_pool_id,TEST_ALLOC_SIZE);
        if(to_ptr!=NULL)
        {
            head_ptr=(MEM_POINTER_LIST_STRUCT_PTR)to_ptr;
            head_ptr->PREV=NULL;
            while(TRUE)
            {
                result=GENERAL_MEM_TEST;
                if(result!=MQX_OK)
                    {
                        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_OK), "Test #4 Testing: 4.2: _(lw)mem_test operation")
                        break;
                    }
                to_ptr=(unsigned char *)_mem_alloc_zero_from(ext_pool_id,TEST_ALLOC_SIZE);
                if(to_ptr==NULL)
                {
                    result=_task_get_error();
                    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_OUT_OF_MEMORY), "Test #4 Testing: 4.3: _(lw)mem_alloc_zero_from operation for MQX_OUT_OF_MEMORY")
                    break;
                }
                else
                {
                    temp_ptr=head_ptr;
                    head_ptr=(MEM_POINTER_LIST_STRUCT_PTR)to_ptr;
                    head_ptr->PREV=temp_ptr;
                }
            }
        }
        else
        {
            result=_task_get_error();
            EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_OUT_OF_MEMORY), "Test #4 Testing: 4.4: _(lw)mem_alloc_zero_from operation for MQX_OUT_OF_MEMORY")
        }
        /*  free allocated memory before and test the pool's integrity again */
        while(head_ptr)
        {
            temp_ptr=head_ptr->PREV;
            result=_mem_free(head_ptr);
            EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_OK), "Test #4 Testing: 4.5: _(lw)mem_free operation")
            head_ptr=temp_ptr;
        }
        result=GENERAL_MEM_TEST;
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_OK), "Test #4 Testing: 4.6: _(lw)mem_test operation")
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_5_main_task
* Comments     : TEST #5: Test function: _mem_alloc_system or _lwmem_alloc_system
* Requirement ID : MEM097, MEM032, LWMEM004
*END*---------------------------------------------------------------------*/

void tc_5_main_task(void)
{
    /*****************************************************************
    ** The following code tests the following function(s):           *
    **                       _mem_alloc_system or _lwmem_alloc_system                      *
    ******************************************************************/
    /*  test the default memory before allocating  */
    result=GENERAL_MEM_TEST;
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_OK), "Test #5 Testing: 5.1: _(lw)mem_test operation")
    /*  allocate all available memory and test the memory again  */
    memory_alloc_all(&info_ptr);
    result=_task_set_error(MQX_OK);
    result=_task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_OK), "Test #5 Testing: 5.2: _task_set_error operation")
    result=GENERAL_MEM_TEST;
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_OK), "Test #5 Testing: 5.3: _(lw)mem_test operation")
    /*  try to allocate which will lead to failure as expected and get error code  */
    to_ptr = (unsigned char *)_mem_alloc_system(TEST_ALLOC_SIZE);
    result=_task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_OUT_OF_MEMORY)&&(to_ptr==NULL), "Test #5 Testing: 5.4: _(lw)mem_alloc_system operation for MQX_OUT_OF_MEMORY")
    result=_task_set_error(MQX_OK);
    result=_task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_OK), "Test #5 Testing: 5.5: _task_set_error operation")
    /*  free all memory allocated before and test the memory again  */
    memory_free_all(&info_ptr);
    result=GENERAL_MEM_TEST;
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_OK), "Test #5 Testing: 5.6: _(lw)mem_test operation")
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_6_main_task
* Comments     : TEST #6: Test function: _mem_alloc_system_from or _lwmem_alloc_system_from
* Requirement ID : MEM097, MEM036, LWMEM014
*END*---------------------------------------------------------------------*/

void tc_6_main_task(void)
{
    /*  test the integrity of created pool  */
    result=GENERAL_MEM_TEST;
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == MQX_OK), "Test #6 Testing: 6.1: _(lw)mem_test operation")
     /*****************************************************************
        ** The following code tests the following function(s):           *
        **                       _mem_alloc_system_from or _lwmem_alloc_system_from                     *
        ******************************************************************/
    /* allocate memory from the created pool until fail and get the error code */
    to_ptr = (unsigned char *)_mem_alloc_system_from(ext_pool_id,TEST_ALLOC_SIZE);
    if(to_ptr!=NULL)
    {
        head_ptr=(MEM_POINTER_LIST_STRUCT_PTR)to_ptr;
        head_ptr->PREV=NULL;
        while(TRUE)
        {
            result=GENERAL_MEM_TEST;
            if(result!=MQX_OK)
                    {
                        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == MQX_OK), "Test #6 Testing: 6.2: _(lw)mem_test operation")
                        break;
                    }
            to_ptr=(unsigned char *)_mem_alloc_system_from(ext_pool_id,TEST_ALLOC_SIZE);
            if(to_ptr==NULL)
            {
                result=_task_get_error();
                EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == MQX_OUT_OF_MEMORY), "Test #6 Testing: 6.3: _(lw)mem_alloc_system_from operation for MQX_OUT_OF_MEMORY")
                break;
            }
            else
            {
                temp_ptr=head_ptr;
                head_ptr=(MEM_POINTER_LIST_STRUCT_PTR)to_ptr;
                head_ptr->PREV=temp_ptr;
            }
        }
    }
    else
    {
        result=_task_get_error();
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == MQX_OUT_OF_MEMORY), "Test #6 Testing: 6.4: _(lw)mem_alloc_system_from operation for MQX_OUT_OF_MEMORY")
    }
    /*  free allocated memory before and test the pool's integrity again  */
    while(head_ptr)
    {
        temp_ptr=head_ptr->PREV;
        result=_mem_free(head_ptr);
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == MQX_OK), "Test #6 Testing: 6.5: _(lw)mem_free operation")
        head_ptr=temp_ptr;
    }
    result=GENERAL_MEM_TEST;
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == MQX_OK), "Test #6 Testing: 6.6: _(lw)mem_test operation")
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_7_main_task
* Comments     : TEST #7: Test function: _mem_alloc_system_zero or _lwmem_alloc_system_zero
* Requirement ID : MEM097, MEM040, LWMEM006
*END*---------------------------------------------------------------------*/

void tc_7_main_task(void)
{
    /*****************************************************************
    ** The following code tests the following function(s):           *
    **                       _mem_alloc_system_zero or _lwmem_alloc_system_zero                      *
    ******************************************************************/
    /*  test the default memory before allocating  */
    result=GENERAL_MEM_TEST;
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MQX_OK), "Test #7 Testing: 7.1: _(lw)mem_test operation")
    /*  allocate all available memory and test the memory again */
    memory_alloc_all(&info_ptr);
    result=_task_set_error(MQX_OK);
    result=_task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MQX_OK), "Test #7 Testing: 7.2: _task_set_error operation")
    result=GENERAL_MEM_TEST;
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MQX_OK), "Test #7 Testing: 7.3: _(lw)mem_test operation")
    /* try to allocate which will lead to failure as expected and get error code */
    to_ptr = (unsigned char *)_mem_alloc(TEST_ALLOC_SIZE);
    result=_task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MQX_OUT_OF_MEMORY)&&(to_ptr==NULL), "Test #7 Testing: 7.4: _(lw)mem_alloc_system_zero operation for MQX_OUT_OF_MEMORY")
    result=_task_set_error(MQX_OK);
    result=_task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MQX_OK), "Test #7 Testing: 7.5: _task_set_error operation")
    /*  free all memory allocated before and test the memory again  */
    memory_free_all(&info_ptr);
    result=GENERAL_MEM_TEST;
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MQX_OK), "Test #7 Testing: 7.6: _(lw)mem_test operation")
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_8_main_task
* Comments     : TEST #8: Test function: _mem_alloc_system_zero_from or _lwmem_alloc_system_zero_from
* Requirement ID : MEM097, MEM044, LWMEM016
*END*---------------------------------------------------------------------*/

void tc_8_main_task(void)
{
    /*  test the integrity of created pool */
    result=GENERAL_MEM_TEST;
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == MQX_OK), "Test #8 Testing: 8.1: _(lw)mem_test operation")
         /*****************************************************************
            ** The following code tests the following function(s):           *
            **                       _mem_alloc_zero_from or _lwmem_alloc_system_zero_from                    *
            ******************************************************************/
    /*  allocate memory from the created pool until fail and get the error code  */
        to_ptr = (unsigned char *)_mem_alloc_system_zero_from(ext_pool_id,TEST_ALLOC_SIZE);
        if(to_ptr!=NULL)
        {
            head_ptr=(MEM_POINTER_LIST_STRUCT_PTR)to_ptr;
            head_ptr->PREV=NULL;
            while(TRUE)
            {
                result=GENERAL_MEM_TEST;
                if(result!=MQX_OK)
                    {
                        EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == MQX_OK), "Test #8 Testing: 8.2: _(lw)mem_test operation")
                        break;
                    }
                to_ptr=(unsigned char *)_mem_alloc_system_zero_from(ext_pool_id,TEST_ALLOC_SIZE);
                if(to_ptr==NULL)
                {
                    result=_task_get_error();
                    EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == MQX_OUT_OF_MEMORY), "Test #8 Testing: 8.3: _(lw)mem_alloc_system_zero_from operation for MQX_OUT_OF_MEMORY")
                    break;
                }
                else
                {
                    temp_ptr=head_ptr;
                    head_ptr=(MEM_POINTER_LIST_STRUCT_PTR)to_ptr;
                    head_ptr->PREV=temp_ptr;
                }
            }
        }
        else
        {
            result=_task_get_error();
            EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == MQX_OUT_OF_MEMORY), "Test #8 Testing: 8.4: _(lw)mem_alloc_system_zero_from operation for MQX_OUT_OF_MEMORY")
        }
        /*  free allocated memory before and test the pool's integrity again  */
        while(head_ptr)
        {
            temp_ptr=head_ptr->PREV;
            result=_mem_free(head_ptr);
            EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == MQX_OK), "Test #8 Testing: 8.5: _(lw)mem_free operation")
            head_ptr=temp_ptr;
        }
        result=GENERAL_MEM_TEST;
        EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == MQX_OK), "Test #8 Testing: 8.6: _(lw)mem_test operation")
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_9_main_task
* Comments     : TEST #9: Test function: _mem_alloc_align
* Requirement ID : MEM097, MEM020
*END*---------------------------------------------------------------------*/

void tc_9_main_task(void)
{
    /*****************************************************************
    ** The following code tests the following function(s):           *
    **                       _mem_alloc_align                      *
    ******************************************************************/
    /*  test the default memory before allocating  */
    result=GENERAL_MEM_TEST;
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (result == MQX_OK), "Test #9 Testing: 9.1: _(lw)mem_test operation")
    /* allocate all available memory and test the memory again  */
    memory_alloc_all(&info_ptr);
    result=_task_set_error(MQX_OK);
    result=_task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (result == MQX_OK), "Test #9 Testing: 9.2: _task_set_error operation")
    result=GENERAL_MEM_TEST;
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (result == MQX_OK), "Test #9 Testing: 9.3: _(lw)mem_test operation")
    /*  try to allocate which will lead to failure as expected and get error code  */
    to_ptr = (unsigned char *)_mem_alloc_align(TEST_ALLOC_SIZE, TEST_ALLOC_ALIGN_SIZE);
    result=_task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (result == MQX_OUT_OF_MEMORY)&&(to_ptr==NULL), "Test #9 Testing: 9.4: _(lw)mem_alloc operation for MQX_OUT_OF_MEMORY")
    result=_task_set_error(MQX_OK);
    result=_task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (result == MQX_OK), "Test #9 Testing: 9.5: _task_set_error operation")
    /*  free all memory allocated before and test the memory again  */
    memory_free_all(&info_ptr);
    result=GENERAL_MEM_TEST;
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (result == MQX_OK), "Test #9 Testing: 9.6: _(lw)mem_test operation")
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_10_main_task
* Comments     : TEST #10: Test function: _mem_alloc_align_from
* Requirement ID : MEM097, MEM024
*END*---------------------------------------------------------------------*/

void tc_10_main_task(void)
{
    /*  test the integrity of created pool  */
    result=GENERAL_MEM_TEST;
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == MQX_OK), "Test #10 Testing: 10.1: _(lw)mem_test operation")
     /*****************************************************************
        ** The following code tests the following function(s):           *
        **                       _mem_alloc_align_from                     *
        ******************************************************************/
    /*  allocate memory from the created pool until fail and get the error code  */
    to_ptr = (unsigned char *)_mem_alloc_align_from(ext_pool_id,TEST_ALLOC_SIZE, TEST_ALLOC_ALIGN_SIZE);
    if(to_ptr!=NULL)
    {
        head_ptr=(MEM_POINTER_LIST_STRUCT_PTR)to_ptr;
        head_ptr->PREV=NULL;
        while(TRUE)
        {
            result=GENERAL_MEM_TEST;
            if(result!=MQX_OK)
                    {
                        EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == MQX_OK), "Test #10 Testing: 10.2: _(lw)mem_test operation")
                        break;
                    }
            to_ptr=(unsigned char *)_mem_alloc_align_from(ext_pool_id,TEST_ALLOC_SIZE, TEST_ALLOC_ALIGN_SIZE);
            if(to_ptr==NULL)
            {
                result=_task_get_error();
                EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == MQX_OUT_OF_MEMORY), "Test #10 Testing: 10.3: _(lw)mem_alloc_align_from operation for MQX_OUT_OF_MEMORY")
                break;
            }
            else
            {
                temp_ptr=head_ptr;
                head_ptr=(MEM_POINTER_LIST_STRUCT_PTR)to_ptr;
                head_ptr->PREV=temp_ptr;
            }
        }
    }
    else
    {
        result=_task_get_error();
        EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == MQX_OUT_OF_MEMORY), "Test #10 Testing: 10.4: _(lw)mem_alloc_align_from operation for MQX_OUT_OF_MEMORY")
    }
    /*  free allocated memory before and test the pool's integrity again  */
    while(head_ptr)
    {
        temp_ptr=head_ptr->PREV;
        result=_mem_free(head_ptr);
        EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == MQX_OK), "Test #10 Testing: 10.5: _(lw)mem_free operation")
        head_ptr=temp_ptr;
    }
    result=GENERAL_MEM_TEST;
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == MQX_OK), "Test #10 Testing: 10.6: _(lw)mem_test operation")
}


/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_11_main_task
* Comments     : TEST #11: Test function: _mem_alloc_at or _lwmem_alloc_at
* Requirement ID : MEM097, MEM028, LWMEM010
*END*---------------------------------------------------------------------*/

void tc_11_main_task(void)
{
    /*  test the default memory before allocating  */
    result=GENERAL_MEM_TEST;
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, (result == MQX_OK), "Test #11 Testing: 11.1: _(lw)mem_test operation")
    /*  allocate  memory and free it to obtain an address */
    tmp_ptr=(unsigned char *)_mem_alloc(TEST_ALLOC_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, (tmp_ptr != NULL), "Test #11 Testing: 11.2: _(lw)mem_alloc operation")
    result=_mem_free(tmp_ptr);
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, (result == MQX_OK), "Test #11 Testing: 11.3: _(lw)mem_free operation")
    /*****************************************************************
    ** The following code tests the following function(s):           *
    **                       _mem_alloc_at                      *
    ******************************************************************/
    /*  test the default memory before allocating  */
    result=GENERAL_MEM_TEST;
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, (result == MQX_OK), "Test #11 Testing: 11.4: _(lw)mem_test operation")
    /*  allocate all available memory and test the memory again  */
    memory_alloc_all(&info_ptr);
    result=_task_set_error(MQX_OK);
    result=_task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, (result == MQX_OK), "Test #11 Testing: 11.5: _task_set_error operation")
    result=GENERAL_MEM_TEST;
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, (result == MQX_OK), "Test #11 Testing: 11.6: _(lw)mem_test operation")
    /*  try to alloc which will lead to failure as expected and get error code  */
    to_ptr = (unsigned char *)_mem_alloc_at(TEST_ALLOC_SIZE, tmp_ptr);
    result=_task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, (result == MQX_OUT_OF_MEMORY)&&(to_ptr==NULL), "Test #11 Testing: 11.7: _(lw)mem_alloc_at operation for MQX_OUT_OF_MEMORY")
    result=_task_set_error(MQX_OK);
    result=_task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, (result == MQX_OK), "Test #11 Testing: 11.8: _task_set_error operation")
    /*  free all memory allocated before and test the memory again  */
    memory_free_all(&info_ptr);
    result=GENERAL_MEM_TEST;
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, (result == MQX_OK), "Test #11 Testing: 11.9: _(lw)mem_test operation")
}
//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_1)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST #1 - Testing the function _(lw)mem_alloc()"),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST #2 - Testing the function _(lw)mem_alloc_from()"),
    EU_TEST_CASE_ADD(tc_3_main_task, " TEST #3 - Testing the function _(lw)mem_alloc_zero()"),
    EU_TEST_CASE_ADD(tc_4_main_task, " TEST #4 - Testing the function _(lw)mem_alloc_zero_from()"),
    EU_TEST_CASE_ADD(tc_5_main_task, " TEST #5 - Testing the function _(lw)mem_alloc_system()"),
    EU_TEST_CASE_ADD(tc_6_main_task, " TEST #6 - Testing the function _(lw)mem_alloc_system_from()"),
    EU_TEST_CASE_ADD(tc_7_main_task, " TEST #7 - Testing the function _(lw)mem_alloc_system_zero()"),
    EU_TEST_CASE_ADD(tc_8_main_task, " TEST #8 - Testing the function _(lw)mem_alloc_system_zero_from()"),
    EU_TEST_CASE_ADD(tc_9_main_task, " TEST #9 - Testing the function _(lw)mem_alloc_align()"),
    EU_TEST_CASE_ADD(tc_10_main_task, " TEST #10 - Testing the function _(lw)mem_alloc_align_from()"),
    EU_TEST_CASE_ADD(tc_11_main_task, " TEST #11 - Testing the function _(lw)mem_alloc_at()"),
EU_TEST_SUITE_END(suite_1)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_1, " :mem_oom - Test of MQX memory functions")
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

   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();

}/* Endbody */



/* EOF */
