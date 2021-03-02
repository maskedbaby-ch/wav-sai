
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
* $Version : 3.8.6.0$
* $Date    : Sep-6-2012$
*
* Comments:
*
*   This file contains testcase for testing printf function. Testcase redirect stdout to pipe. Then is possible to compare *   result from test_compare.h. test_compare.h is reference created with gcc.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"
#include "dma_mem_copy.h"
#include "cache_test_config.h"

#define FILE_BASENAME   test
#define MAIN_TASK  10

/* ARRAY SIZE must be align to CACHE ALIGN and biger or equal than 3xCACHE_ALIGN */
#define ARRAY_SIZE          ((uint32_t)PSP_CACHE_LINE_SIZE * 3)
#define ARRAY_FILL_CHAR_A   (0xAA)
#define ARRAY_FILL_CHAR_B   (0xBB)
#define CACHE_ALIGN         ((uint32_t)PSP_CACHE_LINE_SIZE)

#if(!PSP_HAS_DATA_CACHE)
#ERROR This chip doesnt have data cache enabled
#endif

/* Global variable definitions */
uint8_t *A, *B;

/* Local function */
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : print_array_hex
* Returned Value   : void
* Comments         :
*       temporary,can be removed
*
*END*----------------------------------------------------------------------*/
static void print_array_hex(uint8_t *array, _mem_size size)
{
    int i;
    printf("\n");
    for (i = 0; i < size; i++)
    {
        printf("0x%.8X \t",array[i]);
        if ((i % 4) == 3)
        {
            printf("\n");
        }
    }
}
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : compare_arrays
* Returned Value   : TRUE   - If arrays are identically
                   : FALSE  - Other
* Comments         :
*       This function return TRUE if arrays are the same.
*
*END*----------------------------------------------------------------------*/
static bool compare_arrays(uint8_t *a, uint8_t * b, _mem_size size)
{
    if (size == 0)
    {
       return TRUE;
    }
    else
    {
        while ((*a++ == *b++) && --size);
        return (size == 0)?TRUE:FALSE;
    }

}
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : fill_array
* Returned Value   : void
* Comments         :
*       This function clear defined size of array.
*
*END*----------------------------------------------------------------------*/
static void fill_array(uint8_t *array, _mem_size size, uint8_t data)
{
    int i;
    for (i = 0; i < size; i++)
    {
        array[i] = data;
    }
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : is_array_fill
* Returned Value   : TRUE   - If size of array is filled by character
                   : FALSE  - Other
* Comments         :
*       Check array content
*
*END*----------------------------------------------------------------------*/
static bool is_array_fill_by(uint8_t *array, _mem_size size, uint8_t fill)
{
    if (size == 0)
    {
       return TRUE;
    }
    else
    {
        while ((*array++ == fill) && --size);
        return (size == 0)?TRUE:FALSE;
    }
}

void    main_task(uint32_t);
//------------------------------------------------------------------------
// Test suite function prototype
void tc_1_main_task(void);//Test function 1"),
void tc_2_main_task(void);//Test function 2"),
void tc_3_main_task(void);//Test function 3"),
void tc_4_main_task(void);//Test function 4"),
void tc_5_main_task(void);//Test function 5"),
void tc_6_main_task(void);//Test function 6"),
void tc_7_main_task(void);//Test function 7"),
//------------------------------------------------------------------------
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
{ MAIN_TASK, main_task,  2000,   7, "Main_Task", MQX_AUTO_START_TASK},
{ 0,         0,          0,      0, 0,           0                  }
};

/******************** Begin Testcases ********************/
/*TASK*---------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1: cache working test
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    bool result;

    /* invalidate whole data cache to ensure that cache does not contain other valid data */
    _DCACHE_INVALIDATE();

    /*Clear Array B*/
    fill_array(B, ARRAY_SIZE, ARRAY_FILL_CHAR_B);
    /*Check filled array and make shure that array is cached */
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, (is_array_fill_by(A, ARRAY_SIZE, ARRAY_FILL_CHAR_A)) , "1.0 Cache working test: init A");
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, (is_array_fill_by(B, ARRAY_SIZE, ARRAY_FILL_CHAR_B)) , "1.1 Cache working test: init B");

#if ((CACHE_TYPE == WRITE_BACK) ||(CACHE_TYPE == CACHE_COMMON) || (CACHE_TYPE == WRITE_TROUGH))
    /*Need to FLUSH cache before mem_copy to write all data from cache to memory.
      For write back cache area  and write buffer in write trough type*/
    _DCACHE_FLUSH();
#endif
    /* Copy Array A to array B behind view of cache */
    dma_mem_copy(A, B, ARRAY_SIZE);


    /* Now should be data in cache and data in memory different for array B "?*/
    /* If this test case failed: cache is turned off pr  arrays A and B are placed in uncached area or dma_mem_copy faild*/
    result = is_array_fill_by(B, ARRAY_SIZE, ARRAY_FILL_CHAR_B);

    EU_ASSERT_REF_TC_MSG( tc_1_main_task, result , "1.2 Cache working test");

    if (!result)  /* are A and B different? */
    {
        /* cache doesnt work and we should stop testing */
        eunit_test_stop();
        _task_block();
    }
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2: Test of data invalidate cache and type of cache
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    bool result;

    /* invalidate whole data cache to ensure that cache does not contain other valid data */
    _DCACHE_INVALIDATE();

    /*Clear Array B*/
    fill_array(B, ARRAY_SIZE, ARRAY_FILL_CHAR_B);
    /*Check filled array and make shure that array is cached */
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, (is_array_fill_by(B, ARRAY_SIZE, ARRAY_FILL_CHAR_B)) , "2.0 _DCACHE_IVALIDATE: init");
#if ((CACHE_TYPE == WRITE_BACK) ||(CACHE_TYPE == CACHE_COMMON) || (CACHE_TYPE == WRITE_TROUGH))
    /*Need to FLUSH cache before mem_copy to write all data from cache to memory.
      For write back cache area  and write buffer in write trough type*/
    _DCACHE_FLUSH();
#endif
    /* Copy Array A to array B behind view of cache */
    dma_mem_copy(A, B, ARRAY_SIZE);

    /* Now should be data in cache and data in memory different for array B */

    /* Make invalidation of data cache*/
    _DCACHE_INVALIDATE();

    result = compare_arrays(A, B, ARRAY_SIZE);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, result , "2.1 _DCACHE_IVALIDATE function test");
    /* Arrays A and B should be identically after data cache invalidate */
    if (result)
    {
#if (CACHE_TYPE == WRITE_TROUGH)
        /*if invalidate works we can test if cache use write trough type*/
        /*write data to array B*/
        fill_array(B, ARRAY_SIZE, ARRAY_FILL_CHAR_B);
        /*write trough cache type fill array in cache and in memory*/
        _DCACHE_INVALIDATE();
        /*after invalidation A != B is write trough .*/
        EU_ASSERT_REF_TC_MSG( tc_2_main_task, (is_array_fill_by(B, ARRAY_SIZE, ARRAY_FILL_CHAR_B)) , "2.2 Type test: WRITE TROUGH");
#endif
#if (CACHE_TYPE == WRITE_BACK)
        /*if invalidate works we can test if cache use write back type*/
        fill_array(B, ARRAY_SIZE, ARRAY_FILL_CHAR_B);
        /*write back cache type fill array only in cache until we call flush ,and in mamory are dirty data from older dma transfer*/
        _DCACHE_INVALIDATE();
        /*after invalidation A == B is write back*/
        EU_ASSERT_REF_TC_MSG( tc_2_main_task, (compare_arrays(A, B, ARRAY_SIZE)) , "2.2 Type test: WRITE BACK");
#endif
    }
    else
    {
        /* data cache invalidation doesnt  work and we should stop testing */
        eunit_test_stop();
        _task_block();
    }
}
/*TASK*---------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : TEST #3: Test of flush cache
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
    bool result;

    /* invalidate whole data cache to ensure that cache does not contain other valid data */
    _DCACHE_INVALIDATE();

    /*Clear Array B*/
    fill_array(B, ARRAY_SIZE, ARRAY_FILL_CHAR_B);
    /*Check filled array and make shure that array is cached */
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, (is_array_fill_by(B, ARRAY_SIZE, ARRAY_FILL_CHAR_B)) , "3.0 _DCACHE_FLUSH: init");
#if ((CACHE_TYPE == WRITE_BACK) ||(CACHE_TYPE == CACHE_COMMON) || (CACHE_TYPE == WRITE_TROUGH))
    /*Need to FLUSH cache before mem_copy to write all data from cache to memory.
      For write back cache area  and write buffer in write trough type*/
    _DCACHE_FLUSH();
#endif

    /* Copy Array A to array B behind view of cache */
    dma_mem_copy(A, B, ARRAY_SIZE);

    /*modify cache*/
    fill_array(B, ARRAY_SIZE, ARRAY_FILL_CHAR_A);
    fill_array(B, ARRAY_SIZE, ARRAY_FILL_CHAR_B);

    /* Now should be data in cache and data in memory different for array B */
    /*Flush data from cache to memory*/
    _DCACHE_FLUSH();

    /* Make invalidation of data cache to check flush operation*/
    _DCACHE_INVALIDATE();

    /* Arrays A and B should be different after data cache invalidate if flush works*/
    result = compare_arrays(A, B, ARRAY_SIZE);
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, !result , "3.1 _DCACHE_FLUSH function test");
    if (result)
    {
        eunit_test_stop();
        _task_block();
    }
}
/*TASK*---------------------------------------------------------------------
*
* Task Name    : tc_4_main_task
* Comments     : TEST #1:  Test of invalidate line
*
*   Try to invalidate line for every single byte in array B.then check
*   if invalidate line is invalidated and if rest of array is as was
*
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
    uint32_t i;
    uint32_t start_of_inv_line;

    /* invalidate whole data cache to ensure that cache does not contain other valid data */
    _DCACHE_INVALIDATE();

    for (i = 0; i < ARRAY_SIZE; i++)
    {
        /* Clear Array B */
        fill_array(B, ARRAY_SIZE, ARRAY_FILL_CHAR_B);
        /* Check filled array and make shure that array is cached */
        EU_ASSERT_REF_TC_MSG( tc_4_main_task, (is_array_fill_by(B, ARRAY_SIZE, ARRAY_FILL_CHAR_B)) , "4.0 _DCACHE_IVALIDATE_LINE: initialization of array");
        #if ((CACHE_TYPE == WRITE_BACK) ||(CACHE_TYPE == CACHE_COMMON) || (CACHE_TYPE == WRITE_TROUGH))
        /*Need to FLUSH cache before mem_copy to write all data from cache to memory.
          For write back cache area  and write buffer in write trough type*/
        _DCACHE_FLUSH();
        #endif
        /* Copy Array A to array B behind view of cache */
        dma_mem_copy(A, B, ARRAY_SIZE);

        /* Make invalidation of data cache line*/
        _DCACHE_INVALIDATE_LINE(&B[i]);
        /* Now should be data in cache and data in memory different for array B */
        start_of_inv_line = (i & ~(PSP_CACHE_LINE_SIZE - 1));
         /* The start of array is still different with ARRAY_FILL_CHAR_B value*/
        EU_ASSERT_REF_TC_MSG( tc_4_main_task, is_array_fill_by(B, start_of_inv_line, ARRAY_FILL_CHAR_B) , "4.1 _DCACHE_IVALIDATE_LINE: Check non-invalidated start of array");
        /* Compare invalidate line */
        EU_ASSERT_REF_TC_MSG( tc_4_main_task, compare_arrays(&A[start_of_inv_line], &B[start_of_inv_line], PSP_CACHE_LINE_SIZE ) , "4.2 _DCACHE_IVALIDATE_LINE: Compare invalidated array");
        /* The rest of array is still different with ARRAY_FILL_CHAR_B value*/
        EU_ASSERT_REF_TC_MSG( tc_4_main_task, is_array_fill_by((&B[start_of_inv_line + PSP_CACHE_LINE_SIZE]), ARRAY_SIZE - (start_of_inv_line + PSP_CACHE_LINE_SIZE), ARRAY_FILL_CHAR_B) , "4.3 _DCACHE_IVALIDATE_LINE: Check non-invalidated rest of array");
    }
}

/*TASK*---------------------------------------------------------------------
*
* Task Name    : tc_5_main_task
* Comments     : TEST #8: Test of invalidate m lines
*
*   One block of data is moved byte by byte. There is check if invalidated
*   lines are really invalidated and rest of array is as was.
*
*END*---------------------------------------------------------------------*/
void tc_5_main_task(void)
{
    uint32_t i, t_size;
    uint32_t start_of_inv_line;
    uint32_t beyond_inv_line;

    /* invalidate whole data cache to ensure that cache does not contain other valid data */
    _DCACHE_INVALIDATE();

    /* Tested for block of data  equal to line size, bigger than line size and smaller than line size */
    for (t_size = PSP_CACHE_LINE_SIZE - 1; t_size <= PSP_CACHE_LINE_SIZE + 1; t_size++)
    {
        for (i = 0; i < (ARRAY_SIZE - t_size); i++)
        {
            /*Clear Array B*/
            fill_array(B, ARRAY_SIZE, ARRAY_FILL_CHAR_B);
            /*Check filled array and make shure that array is cached */
            EU_ASSERT_REF_TC_MSG( tc_5_main_task, (is_array_fill_by(B, ARRAY_SIZE, ARRAY_FILL_CHAR_B)) , "5.0 _DCACHE_IVALIDATE_MLINES: initialization of arrays");
            #if ((CACHE_TYPE == WRITE_BACK) ||(CACHE_TYPE == CACHE_COMMON) || (CACHE_TYPE == WRITE_TROUGH))
            /*Need to FLUSH cache before mem_copy to write all data from cache to memory.
              For write back cache area  and write buffer in write trough type*/
            _DCACHE_FLUSH();
            #endif
            /* Copy Array A to array B behind view of cache */
            dma_mem_copy(A, B, ARRAY_SIZE);
            /* Make invalidation of data cache line/s*/
            _DCACHE_INVALIDATE_MLINES(&B[i], t_size);
            /* Now should be data in cache and data in memory different for array B */

            start_of_inv_line = (i & ~(PSP_CACHE_LINE_SIZE - 1)); //start of invalidated area
            beyond_inv_line   = ((i + t_size + PSP_CACHE_LINE_SIZE - 1) & ~(PSP_CACHE_LINE_SIZE - 1));
             /* the start of array is still different with ARRAY_FILL_CHAR_B value*/
            EU_ASSERT_REF_TC_MSG( tc_5_main_task, is_array_fill_by(B, start_of_inv_line, ARRAY_FILL_CHAR_B) , "5.2 _DCACHE_IVALIDATE_MLINES: Check non-invalidated start of array");
            /* compare invalidate line */
            EU_ASSERT_REF_TC_MSG( tc_5_main_task, compare_arrays(&A[start_of_inv_line], &B[start_of_inv_line], (beyond_inv_line - start_of_inv_line)) , "5.1 _DCACHE_IVALIDATE_MLINES: Compare invalidated array");
            /* the rest of array is still different with ARRAY_FILL_CHAR_B value*/
            EU_ASSERT_REF_TC_MSG( tc_5_main_task, is_array_fill_by((&B[beyond_inv_line]), ARRAY_SIZE - beyond_inv_line, ARRAY_FILL_CHAR_B) , "5.2 _DCACHE_IVALIDATE_MLINES: Check non-invalidated rest of array");
        }
    }
}

/*TASK*---------------------------------------------------------------------
*
* Task Name    : tc_6_main_task
* Comments     : TEST #6: flush line
*
*   Try to flush line for every single byte in array B. Then check
*   if invalidate line is invalidated and if rest of array is as was.
*
*END*---------------------------------------------------------------------*/
void tc_6_main_task(void)
{
    uint32_t i;
    uint32_t start_of_flush_line;

    /* invalidate whole data cache to ensure that cache does not contain other valid data */
    _DCACHE_INVALIDATE();

    for (i = 0; i < ARRAY_SIZE; i++)
    {
        /*Clear Array B*/
        fill_array(B, ARRAY_SIZE, ARRAY_FILL_CHAR_B);
        /*Check filled array and make shure that array is cached */
        EU_ASSERT_REF_TC_MSG( tc_6_main_task, (is_array_fill_by(B, ARRAY_SIZE, ARRAY_FILL_CHAR_B)) , "6.0 _DCACHE_FLUSH_LINE: initialization of array");
        #if ((CACHE_TYPE == WRITE_BACK) ||(CACHE_TYPE == CACHE_COMMON) || (CACHE_TYPE == WRITE_TROUGH))
        /*Need to FLUSH cache before mem_copy to write all data from cache to memory.
          For write back cache area  and write buffer in write trough type*/
        _DCACHE_FLUSH();
        #endif
        /* Copy Array A to array B behind view of cache */
        dma_mem_copy(A, B, ARRAY_SIZE);
        /*modify cache line*/
        /*We have to modify all alements of array for writte trough cache*/

        start_of_flush_line = (i & ~(PSP_CACHE_LINE_SIZE - 1)); //start of invalidated area
        /*need toadd testcases specific for write back (changing only one element of array)*/
        fill_array(&B[start_of_flush_line], PSP_CACHE_LINE_SIZE, ARRAY_FILL_CHAR_A);
        fill_array(&B[start_of_flush_line], PSP_CACHE_LINE_SIZE, ARRAY_FILL_CHAR_B);
        /* Now should be data in cache and data in memory different for array B */
        /* Flush one line */
        _DCACHE_FLUSH_LINE(&B[i]);
        /*Invalidation of  ARRAY B*/
        _DCACHE_INVALIDATE_MLINES(B,ARRAY_SIZE);
        /* First 16 bytes of area A and B should be identicaly */
        EU_ASSERT_REF_TC_MSG( tc_6_main_task, compare_arrays(A, B , start_of_flush_line) , "6.1 _DCACHE_FLUSH_LINE: check first unflushed line/s of  data cache");
        /* Second 32 bytes of B array (2 lines) should be identically to  second 32 byte of A array */
        EU_ASSERT_REF_TC_MSG( tc_6_main_task, is_array_fill_by(&B[start_of_flush_line], PSP_CACHE_LINE_SIZE, ARRAY_FILL_CHAR_B) , "6.2 _DCACHE_FLUSH_LINE: chceck flushed  line of data cache");
        /* Rest of  B array should be differenr and aqual to ARRAY_FILL_CHAR_B */
        EU_ASSERT_REF_TC_MSG( tc_6_main_task, compare_arrays(&A[start_of_flush_line + PSP_CACHE_LINE_SIZE], &B[start_of_flush_line + PSP_CACHE_LINE_SIZE], (ARRAY_SIZE - (start_of_flush_line + PSP_CACHE_LINE_SIZE))) , "6.3 _DCACHE_FLUSH_LINE: chceck rest of  flushed  lines of data cache");
    }
}

/*TASK*---------------------------------------------------------------------
*
* Task Name    : tc_7_main_task
* Comments     : TEST #7: flush mline
*
*   One block of data is moved byte by byte. There is check if invalidated
*   lines are really invalidated and rest of array is as was.
*
*END*---------------------------------------------------------------------*/
void tc_7_main_task(void)
{
    uint32_t i, t_size;
    uint32_t start_of_flush_line;
    uint32_t beyond_flush_line;

    /* invalidate whole data cache to ensure that cache does not contain other valid data */
    _DCACHE_INVALIDATE();

    /* Tested for block of data  equal to line size, bigger than line size and smaller than line size */
    for (t_size = PSP_CACHE_LINE_SIZE - 1; t_size <= PSP_CACHE_LINE_SIZE + 1; t_size++)
    {
        for (i = 0; i < (ARRAY_SIZE - PSP_CACHE_LINE_SIZE); i++)
        {
            /*Clear Array B*/
            fill_array(B, ARRAY_SIZE, ARRAY_FILL_CHAR_B);
            /*Check filled array and make shure that array is cached */
            EU_ASSERT_REF_TC_MSG( tc_6_main_task, (is_array_fill_by(B, ARRAY_SIZE, ARRAY_FILL_CHAR_B)) , "7.0 _DCACHE_FLUSH_MINES: initialization of array");
            #if ((CACHE_TYPE == WRITE_BACK) ||(CACHE_TYPE == CACHE_COMMON) || (CACHE_TYPE == WRITE_TROUGH))
            /*Need to FLUSH cache before mem_copy to write all data from cache to memory.
              For write back cache area  and write buffer in write trough type*/
            _DCACHE_FLUSH();
            #endif
            /* Copy Array A to array B behind view of cache */
            dma_mem_copy(A, B, ARRAY_SIZE);
            /*modify cache line*/
            /*We have to modify all alements of array for writte trough cache*/

            start_of_flush_line = (i & ~(PSP_CACHE_LINE_SIZE - 1)); //start of invalidated area
            beyond_flush_line   = ((i + t_size + PSP_CACHE_LINE_SIZE - 1) & ~(PSP_CACHE_LINE_SIZE - 1));
            /*need toadd testcases specific for write back (changing only one element of array)*/
            fill_array(&B[start_of_flush_line], (beyond_flush_line - start_of_flush_line), ARRAY_FILL_CHAR_A);
            fill_array(&B[start_of_flush_line], (beyond_flush_line - start_of_flush_line), ARRAY_FILL_CHAR_B);
            /* Now should be data in cache and data in memory different for array B */
            /* Flush one line */
            _DCACHE_FLUSH_MLINES(&B[i], t_size);
            /*Invalidation of  ARRAY B*/
            _DCACHE_INVALIDATE_MLINES(B, ARRAY_SIZE);
            /* First 16 bytes of area A and B should be identicaly */
            EU_ASSERT_REF_TC_MSG( tc_6_main_task, compare_arrays(A, B , start_of_flush_line) , "7.1 _DCACHE_FLUSH_MLINES: check first unflushed line/s of  data cache");
            /* Second 32 bytes of B array (2 lines) should be identically to  second 32 byte of A array */
            EU_ASSERT_REF_TC_MSG( tc_6_main_task, is_array_fill_by(&B[start_of_flush_line], (beyond_flush_line - start_of_flush_line ), ARRAY_FILL_CHAR_B) , "7.2 _DCACHE_FLUSH_MLINES: chceck flushed  line of data cache");
            /* Rest of  B array should be differenr and aqual to ARRAY_FILL_CHAR_B */
            EU_ASSERT_REF_TC_MSG( tc_6_main_task, compare_arrays(&A[beyond_flush_line], &B[beyond_flush_line], (ARRAY_SIZE - (beyond_flush_line))) , "7.3 _DCACHE_FLUSH_MLINES: chceck rest of  flushed  lines of data cache");
        }
    }
}
/******************** End Testcases ********************/

//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_1)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST CASE for Data cache function "),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST CASE for _DCACHE_INVALIDATE"),
    EU_TEST_CASE_ADD(tc_3_main_task, " TEST CASE for _DCACHE_FLUSH"),
    EU_TEST_CASE_ADD(tc_4_main_task, " TEST CASE for _DCACHE_INVALIDATE_LINE"),
    EU_TEST_CASE_ADD(tc_5_main_task, " TEST CASE for _DCACHE_INVALIDATE_MLINES"),
    EU_TEST_CASE_ADD(tc_6_main_task, " TEST CASE for _DCACHE_FLUSH_LINE"),
    EU_TEST_CASE_ADD(tc_7_main_task, " TEST CASE for _DCACHE_FLUSH_MLINES"),
EU_TEST_SUITE_END(suite_1)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_1, " - Test for printf")
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
    if (dma_mem_copy_init())
    {
        printf("DMA initialization failed\n");
        _task_block();
    }
#ifdef TEST_CACHED_AREA_START /*This value should be specified at user_config.h */
    /* Create pool at cached area */
    _mem_pool_id cached_pool = _mem_create_pool((void *)TEST_CACHED_AREA_START, TEST_CACHED_AREA_SIZE);
    if (cached_pool == NULL)
    {
        printf("Create pool failed !\n");
        _task_block();
    }
    else
    {
        /* We should use _mem_alloc_from and align should be hidden */
        B = _mem_alloc_align_from(cached_pool, ARRAY_SIZE, PSP_CACHE_LINE_SIZE);
        if (B == NULL)
        {
            printf("Array B allocation failed!\n");
            _task_block();
        }
    }
#else
/*If test cached area is not specified we are allocating from actual pool. This work only for cached targets (DDR targets)*/
#if ((CACHE_TYPE == WRITE_BACK) ||(CACHE_TYPE == CACHE_COMMON))
#error Issue when code is running from cached area with using writeback caches.
#else
    B = _mem_alloc(ARRAY_SIZE);
    if (B == NULL)
    {
        printf("Array B allocation failed!\n");
        _task_block();
    }
#endif /* (CACHE_TYPE == WRITE_BACK) ||(CACHE_TYPE == CACHE_COMMON) */
#endif /* TEST_CACHED_AREA_START */
    /* Array A can be at uncached area */
    A = _mem_alloc(ARRAY_SIZE);
    if (A == NULL)
    {
        printf("Array A allocation failed!\n");
        _task_block();
    }
    fill_array(A, ARRAY_SIZE, ARRAY_FILL_CHAR_A);
    /*for write back and write trough write allocate cache we need to have the same content of array A in cache and in memory for dma transfer*/
    _DCACHE_FLUSH();

    eunit_mutex_init();
    EU_RUN_ALL_TESTS(eunit_tres_array);
    eunit_test_stop();
    _task_block();
}/* Endbody */
/* EOF */
