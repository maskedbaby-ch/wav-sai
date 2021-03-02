/**HEADER********************************************************************
*
* Copyright (c) 2008-2013 Freescale Semiconductor.
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International.
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
* $FileName: test.cpp$
* $Version : 4.0.1.2$
* $Date    : Apr-22-2013$
*
* Comments:
*
*   This file contains the source for the cplus test program.
*   includes all test cases to verify the C++ programs work properly with
*   MQX library.
*
*   mapped to test requirement: CPLUS001, CPLUS003, CPLUS004, CPLUS005, CPLUS006
*
*END************************************************************************/
#include <mqx.h>
#include <bsp.h>
#include <mem_prv.h>

#include "test.h"
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"

#ifdef __cplusplus
extern "C" {
#endif
/*------------------------------------------------------------------------
 * Required user_config settings
 */

#if !MQX_CHECK_ERRORS
    #error "MQX_CHECK_ERRORS must be enabled"
#endif
#if !MQX_CHECK_VALIDITY
    #error "MQX_CHECK_VALIDITY must be enabled"
#endif

/*------------------------------------------------------------------------
 * Test suite function prototype
 */
void tc_1_main_task(void);  /* TEST # 1 - Testing statical, local and global constructions */
void tc_2_main_task(void); /* TEST # 2 - Testing functions compatible */

/*------------------------------------------------------------------------
 * MQX task template
 */
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   { MAIN_TASK,      main_task,      3000,  9, (char *)"main",    MQX_AUTO_START_TASK},
   {         0,              0,         0,  0,      0,                      0}
};

/* Statical instance */
static Rectangle    statical(INIT_WIDTH, INIT_HEIGHT);

/******************** Begin Testcases **********************/

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1:
* This test case is to verify the Class contructor and destructor shall
* work properly in all cases.
* Requirements:
* CPLUS001, CPLUS003,CPLUS004,CPLUS006
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    Rectangle   local(INIT_WIDTH, INIT_HEIGHT);
    Rectangle   *heap;
    _mqx_uint   result;
    _mqx_uint   temp;
    _task_id    system_task_id;

    /* Get system task ID */
    system_task_id = _mqx_get_system_task_id();

    /* Verify static */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, statical.Area() == INIT_WIDTH * INIT_HEIGHT, (char *)"Test #1: Testing 1.01: Statical instance");
    /* Verify local */
    result = local.Area();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == INIT_WIDTH * INIT_HEIGHT, (char *)"Test #1: Testing 1.02: Local instance");

    /* Allocate dynamic instance */
    heap = new Rectangle();
    /* Verify dynamic */
    result = heap->Area();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == DEFAULT_WIDTH * DEFAULT_HEIGHT, (char *)"Test #1: Testing 1.03: Dynamic instance");
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, memory_get_owner(heap) == system_task_id, (char *)"Test #1: Testing 1.04 The dynamic instance belongs to system");

    /* Invalidate the memory of dynamic instance */
    temp = get_mem_valid_field(heap);
    set_mem_valid_field(heap, temp + INVALID_VALUE);
    /* Failed to deallocate dynamic instance */
    delete heap;
#if  MQX_USE_LWMEM_ALLOCATOR
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, _task_errno == MQX_LWMEM_POOL_INVALID, (char *)"Test #1: Testing 1.05 Delete a dynamic instance with invalid memory should be failed");
#else
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, _task_errno == MQX_INVALID_CHECKSUM, (char *)"Test #1: Testing 1.05 Delete a dynamic instance with invalid memory should be failed");
#endif
    _task_set_error(MQX_OK);
    /*Restore the memory of dynamic instance */
    set_mem_valid_field(heap, temp);

    /* Deallocate dynamic instance successfully */
    delete heap;
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, _task_errno == MQX_OK, (char *)"Test #1: Testing 1.06 Delete a dynamic instance in normal operation should be successfull ");
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, memory_get_owner(heap) != system_task_id, (char *)"Test #1: Testing 1.07 The dynamic instance no longer belongs to system");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2:
*   This test case is to verify the functions compatible of C++ with
*   MQX library.
* Requirements :
*   CPLUS005.
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    MQX_TICK_STRUCT start_ticks, end_ticks, test1_ticks, test2_ticks;
    Rectangle       rect1(INIT_WIDTH, INIT_HEIGHT);
    Rectangle       rect2(INIT_WIDTH, INIT_HEIGHT);
    Rectangle       rect3(INIT_WIDTH, INIT_HEIGHT);
    Rectangle       *rect;
    Polygon         *polygon;
    _mqx_uint       result;

    /* Allocate object */
    rect = new Rectangle();

    /* Verify derived and overriding */
    polygon = &rect1;
    result = polygon->Area();
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == INIT_WIDTH * INIT_HEIGHT, (char *)"Test #2: Testing 2.01 Verify derived and overriding");

    /* Verify passing arguments */
    change_attributes(rect1, &rect2, rect3);
    result = rect1.Area();
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == INIT_WIDTH * INIT_HEIGHT, (char *)"Test #2: Testing 2.02 Verify passing argument by value");
    result = rect2.Area();
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == NEW_WIDTH * INIT_HEIGHT, (char *)"Test #2: Testing 2.03 Verify passing argument by pointer");
    result = rect3.Area();
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == NEW_WIDTH * INIT_HEIGHT, (char *)"Test #2: Testing 2.04 Verify passing argument by reference");

    /* Get elapsed ticks time before delay */
    _time_get_elapsed_ticks(&start_ticks);
    /* Calling MQX API function */
    rect->TestApi(DELAY_TICKS);
    /* Get elapsed ticks time after delay */
    _time_get_elapsed_ticks(&end_ticks);
    /* Calculate the expected end ticks */
    PSP_ADD_TICKS_TO_TICK_STRUCT(&start_ticks, DELAY_TICKS, &test1_ticks);
    /* Verify accurate delay time */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, PSP_CMP_TICKS(&end_ticks, &test1_ticks) == 0, (char *)"Test #2: Testing 2.05: Verify the waiting time must be accurate");

    /* C++ method */
    _time_delay_ticks(1);
    _time_get_elapsed_ticks(&start_ticks);
    rect->TestPerf(DELAY_TICKS);
    _time_get_elapsed_ticks(&end_ticks);
    /* Calculate the difference ticks */
    _time_diff_ticks(&end_ticks, &start_ticks, &test1_ticks);

    /* C function */
    _time_delay_ticks(1);
    _time_get_elapsed_ticks(&start_ticks);
    loop_test(DELAY_TICKS);
    _time_get_elapsed_ticks(&end_ticks);

    /* Calculate the difference ticks */
    _time_diff_ticks(&end_ticks, &start_ticks, &test2_ticks);

    /* Verify the performances must be the same */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, PSP_CMP_TICKS(&test1_ticks, &test2_ticks) == 0, (char *)"Test #2: Testing 2.06: Verify the performances must be the same");
}

/*------------------------------------------------------------------------
 * Define Test Suite
 */
EU_TEST_SUITE_BEGIN(suite_1)
    EU_TEST_CASE_ADD(tc_1_main_task, "TEST # 1 - Testing static, local and global constructions"),
    EU_TEST_CASE_ADD(tc_2_main_task, "TEST # 2 - Testing functions compatible")
EU_TEST_SUITE_END(suite_1)
/* Add test suites */
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_1, " - MQX Cplus Component Test Suite, 2 Test")
EU_TEST_REGISTRY_END()
/*------------------------------------------------------------------------*/

/*TASK*-------------------------------------------------------------------
*
* Task Name    : main_task
* Comments     :
*    This is the main task to the MQX Kernel program,  this task creates
* all defined test tasks and lets them run continuously.
* This task also checks the initialization of static class.
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
    _mqx_exit(0);
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : get_mem_valid_field
* Returned Value   : _mqx_uint.
* Comments         :
*   Get the checksum of an memory block
*   - [IN] mem_ptr: The pointer of memory block.
*
*END*--------------------------------------------------------------------*/
_mqx_uint get_mem_valid_field(void *mem_ptr)
{
    _mqx_uint result = 0;

#if MQX_USE_LWMEM_ALLOCATOR
    LWMEM_BLOCK_STRUCT_PTR  block_ptr;
    LWMEM_POOL_STRUCT_PTR   mem_pool_ptr;

    block_ptr = GET_LWMEMBLOCK_PTR(mem_ptr);
    mem_pool_ptr = (LWMEM_POOL_STRUCT_PTR) block_ptr->POOL;
    result = mem_pool_ptr->VALID;

#else
    STOREBLOCK_STRUCT_PTR block_ptr;

    block_ptr = GET_MEMBLOCK_PTR(mem_ptr);
    result = block_ptr->CHECKSUM;
#endif

    return result;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : set_mem_valid_field
* Returned Value   : None.
* Comments         :
*   Set the checksum value of an memory block
*   - [IN] mem_ptr: The pointer of memory block.
*   - [IN] checksum: The value to be set.
*
*END*--------------------------------------------------------------------*/
void set_mem_valid_field(void *mem_ptr, _mqx_uint value)
{
#if MQX_USE_LWMEM_ALLOCATOR
    LWMEM_BLOCK_STRUCT_PTR  block_ptr;
    LWMEM_POOL_STRUCT_PTR   mem_pool_ptr;

    block_ptr = GET_LWMEMBLOCK_PTR(mem_ptr);
    mem_pool_ptr = (LWMEM_POOL_STRUCT_PTR) block_ptr->POOL;
    mem_pool_ptr->VALID = value;

#else
    STOREBLOCK_STRUCT_PTR block_ptr;

    block_ptr = GET_MEMBLOCK_PTR(mem_ptr);
    block_ptr->CHECKSUM =  value;
#endif
}


/*FUNCTION*----------------------------------------------------------------\r
*
* Function Name    : change_attributes
* Returned Value   : None.
* Comments         :
*   Change the attributes of class.
*   - [IN] rect1: Rectangle instance
*   - [IN] rect2: Pointer to Rectangle object
*   - [IN] rect3: Reference of Rectangle instance
*
*END*--------------------------------------------------------------------*/
void change_attributes(Rectangle rect1, Rectangle *rect2, Rectangle &rect3)
{
	rect1.SetWidth(NEW_WIDTH);
    rect2->SetWidth(NEW_WIDTH);
    rect3.SetWidth(NEW_WIDTH);
}
/*---------------------------------------------------------------------
*
* Definitions of class methods
*
*----------------------------------------------------------------------*/
/* Polygon initialization */
void Polygon::Init(_mqx_uint new_width, _mqx_uint new_height)
{
    if ((new_width < 1000) && (new_height < 1000))
    {
        width  = new_width;
        height = new_height;
    }
    else
    {
        width  = 0;
        height = 0;
    }
}

/* Return area of Polygon */
_mqx_uint Polygon::Area(void)
{
    return 0;
}

/* Rectangle Contructor */
Rectangle::Rectangle(void)
{
	Init(DEFAULT_WIDTH, DEFAULT_HEIGHT);
}

/* Overloading constructor */
Rectangle::Rectangle(_mqx_uint new_width, _mqx_uint new_height)
{
	Init(new_width, new_height);
}

Rectangle::~Rectangle(void)
{
	//printf("Deallocating Rectangle\n");
}

/* Calculate area of Rectangle */
_mqx_uint Rectangle::Area(void)
{
	return (height * width);
}

void Rectangle::SetWidth(_mqx_uint new_width)
{
	width = new_width;
}

/* Used to test calling API function */
void Rectangle::TestApi(_mqx_uint delay)
{
	_time_delay_ticks(delay);
}

/* Used to test performance */
void Rectangle::TestPerf(_mqx_uint ticks)
{
	work_ticks(ticks);
}
#ifdef __cplusplus
}
#endif

