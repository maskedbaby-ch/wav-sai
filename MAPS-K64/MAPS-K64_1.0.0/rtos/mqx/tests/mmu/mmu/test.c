
/**HEADER********************************************************************
*
* Copyright (c) 20013 Freescale Semiconductor;
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
* $Version : 4.0.2$
* $Date    : Aug 20, 2013
*
* Comments: This file contain test routines for MMU testing
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "mqx_inc.h"
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"
#include "test_specific_vybrid_a5.h"
#include "mmu_cortexa5.h"

#define FILE_BASENAME   test
#define MAIN_TASK  10

#define MMU_TEST_PATERN     0xFEDCBA98

#if(!PSP_HAS_MMU)
#ERROR This chip doesnt have data cache enabled
#endif

#if(!MMU_L2_SUPPORT)
#ERROR This chip doesnt have data cache enabled
#endif

////////////////////////////////////////////////////
#define L1_TEST_ARRAY_SIZE 9
const uint32_t l1_flag_array[] =
{
     MMU_L1_1MB_RW_SO_FLAG,
     MMU_L1_1MB_RW_WT_FLAG,
     MMU_L1_1MB_RW_CN_FLAG,
     MMU_L1_1MB_RO_SO_FLAG,
     MMU_L1_1MB_RO_WT_FLAG,
     MMU_L1_1MB_RO_CN_FLAG,
     MMU_L1_1MB_NA_SO_FLAG,
     MMU_L1_1MB_NA_WT_FLAG,
     MMU_L1_1MB_NA_CN_FLAG
};
const uint32_t l1_ref_array[] =
{
     MMU_L1_1MB_RW_SO_REF,
     MMU_L1_1MB_RW_WT_REF,
     MMU_L1_1MB_RW_CN_REF,
     MMU_L1_1MB_RO_SO_REF,
     MMU_L1_1MB_RO_WT_REF,
     MMU_L1_1MB_RO_CN_REF,
     MMU_L1_1MB_NA_SO_REF,
     MMU_L1_1MB_NA_WT_REF,
     MMU_L1_1MB_NA_CN_REF
};
////////////////////////////////////////////////////
#define L2_TEST_ARRAY_SIZE 9
const uint32_t l2_flag_array[] =
{
     MMU_L2_4KB_RW_SO_FLAG,
     MMU_L2_4KB_RW_WT_FLAG,
     MMU_L2_4KB_RW_CN_FLAG,
     MMU_L2_4KB_RO_SO_FLAG,
     MMU_L2_4KB_RO_WT_FLAG,
     MMU_L2_4KB_RO_CN_FLAG,
     MMU_L2_4KB_NA_SO_FLAG,
     MMU_L2_4KB_NA_WT_FLAG,
     MMU_L2_4KB_NA_CN_FLAG
};
const uint32_t l2_ref_array[] =
{
     MMU_L2_4KB_RW_SO_REF,
     MMU_L2_4KB_RW_WT_REF,
     MMU_L2_4KB_RW_CN_REF,
     MMU_L2_4KB_RO_SO_REF,
     MMU_L2_4KB_RO_WT_REF,
     MMU_L2_4KB_RO_CN_REF,
     MMU_L2_4KB_NA_SO_REF,
     MMU_L2_4KB_NA_WT_REF,
     MMU_L2_4KB_NA_CN_REF
};
////////////////////////////////////////////////////
#define WRONG_INIT_FLAG_ARRAY_SIZE 4
const uint32_t wrong_init_flag_array[] =
{
    0x0,
    MMU_L2_4KB_RW_SO_FLAG,
    MMU_L2_16MB_RW_SO_FLAG,
    MMU_L2_64MB_RW_SO_FLAG

};
////////////////////////////////////////////////////
#define WRONG_ADD_VREGION_FLAG_ARRAY_SIZE 3
const uint32_t wrong_add_vregion_flag_array[] =
{
    0x0,
    MMU_L2_16MB_RW_SO_FLAG,
    MMU_L2_64MB_RW_SO_FLAG

};

/* Global variable definitions */
    uint32_t *l1_table;
    uint32_t *l2_table;
    _mem_size free_mem;

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : check_l1_reg
* Returned Value   : True if success, False otherwise
* Comments         :
*   This function check specified range in L1 table entries.
*
*END*----------------------------------------------------------------------*/
static bool check_l1_reg(uint32_t *table, uint32_t va, uint32_t pa, uint32_t entries_cnt, uint32_t flag_ref)
{

    for (; va < entries_cnt; va++ , pa++)
    {
        /* check flags for l1 table entry */
        if ((table[va] & MMU_L1_FLAG_MASK) != flag_ref)
        {
            return FALSE;
        }
        /* check pa adress of l1 table entry */
        if (((table[va] & MMU_L1_ADDR_MASK) >> MMU_L1_ADDR_SHIFT) != pa)
        {
            return FALSE;
        }
    }
    return TRUE;
}
/* Local function */
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : check_l1_table
* Returned Value   : True if success, False otherwise
* Comments         :
*   This function check entyre L1 table entries.
*   Also check if patern behind  L1 table is correct.
*
*END*----------------------------------------------------------------------*/
static bool check_l1_table(uint32_t *table, uint32_t flag_ref)
{
    bool result;
    result = check_l1_reg(table, 0, 0, 4096, flag_ref);
    /* check patern after table */
    if (l1_table[MMU_TRANSLATION_TABLE_ENTRIES] != MMU_TEST_PATERN)
    {
        result = FALSE;
    }
    return result;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : check_l2_reg
* Returned Value   : True if success, False otherwise
* Comments         :
*   This function check specified range in L2 table entries.
*
*END*----------------------------------------------------------------------*/
static bool check_l2_reg(uint32_t *table, uint32_t va, uint32_t pa, uint32_t entries_cnt, uint32_t flag_ref)
{

    for (; va < entries_cnt; va++ , pa++)
    {
        /* check flags for l2 table entry */
        if ((table[va] & MMU_L2_FLAG_MASK) != flag_ref)
        {
            return FALSE;
        }
        /* check pa adress of l1 table entry */
        if (((table[va] & MMU_L2_ADDR_MASK) >> MMU_L2_ADDR_SHIFT) != pa)
        {
            return FALSE;
        }
    }
    return TRUE;
}

void    main_task(uint32_t);
//------------------------------------------------------------------------
// Test suite function prototype
void tc_1_main_task(void);
void tc_2_main_task(void);
void tc_3_main_task(void);
void tc_4_main_task(void);
void tc_5_main_task(void);
void tc_6_main_task(void);

//------------------------------------------------------------------------
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
{ MAIN_TASK, main_task,  2000,   8, "Main_Task", MQX_AUTO_START_TASK},
{ 0,         0,          0,      0, 0,           0                  }
};

/******************** Begin Testcases ********************/
/*TASK*---------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1:
*   Test _mmu_vinit function.
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{

/** _mmu_add_init(...) works with our L1 table . This test do nothing directly with MMU module. Just test the API **/
    uint32_t result, i;

/* We are going to alloc more space as we need to check if mmu api doesnt write over table */
    l1_table = _mem_alloc_align(MMU_TRANSLATION_TABLE_SIZE  + (1 * sizeof(int)), MMU_TRANSLATION_TABLE_ALIGN);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, (NULL != l1_table), "Test #1: Test 1.0 MMU table allocation  failed.");
    if (NULL == l1_table)
    {
        eunit_test_stop();
        _task_block();
    }
    /* Save pattern behind table */
    l1_table[MMU_TRANSLATION_TABLE_ENTRIES] = MMU_TEST_PATERN;

/* Test invalid L1 table */
    result = _mmu_vinit(MMU_L1_1MB_RW_CN_FLAG, (void *)((uint32_t)l1_table + 1));
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, (result == MQX_INVALID_PARAMETER), "Test #1: Test 1.1 MMU init when L1 table is unaligned.");
    result = _mmu_vinit(MMU_L1_1MB_RW_CN_FLAG, NULL);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, (result == MQX_INVALID_PARAMETER), "Test #1: Test 1.2 MMU init when L1 table point to NULL.");

/* Test invalid flag parameters*/
    for (i = 0; i < WRONG_INIT_FLAG_ARRAY_SIZE; i++)
    {
        result = _mmu_vinit(wrong_init_flag_array[i], (void *)l1_table);
        EU_ASSERT_REF_TC_MSG( tc_1_main_task, (result == MQX_INVALID_PARAMETER), "Test #1: Test 1.3 MMU init when default flag is invalid.");
    }

/* Test valid flag parameters*/
    for (i = 0; i < L1_TEST_ARRAY_SIZE; i++)
    {
        result = _mmu_vinit(l1_flag_array[i], (void *)l1_table);
        EU_ASSERT_REF_TC_MSG( tc_1_main_task, (result == MQX_OK), "Test #1: Test 1.4 MMU init when default flag is valid.");
        EU_ASSERT_REF_TC_MSG( tc_1_main_task, check_l1_table(l1_table, l1_ref_array[i]), "Test #1: Test 1.5 MMU init check table after initialization.");
    }

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2:
*   Test _mmu_add_region function.
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    uint32_t result, i, size;
    uint32_t temp_entry;

    /** _mmu_add_vregion just change entries in L1 table initialized in tc_1_main_task. This test do nothing directly with MMU module. Just test the API **/

    for (size = 0; size <= MMU_ADD_REGION_SIZE; size += 0x00100000)
    {
        /* store old entry */
        temp_entry = l1_table[((MMU_ADD_REGION_START + size) >> 20)];
        /* Store patern behind tested area */
        l1_table[((MMU_ADD_REGION_START + size) >> 20)] = MMU_TEST_PATERN;
        /* Test invalid flag parameters*/
        for (i = 0; i < WRONG_ADD_VREGION_FLAG_ARRAY_SIZE; i++)
        {
            result = _mmu_add_vregion((void *)MMU_ADD_REGION_START, (void *)MMU_ADD_REGION_START, size, wrong_add_vregion_flag_array[i]);
            EU_ASSERT_REF_TC_MSG( tc_2_main_task, (result == MQX_INVALID_PARAMETER), "Test #2: Test 2.1 MMU add region when default flag is invalid.");
        }

        /* Test valid flag parameters*/
        for (i = 0; i < L1_TEST_ARRAY_SIZE; i++)
        {
            result = _mmu_add_vregion((void *)MMU_ADD_REGION_START, (void *)MMU_ADD_REGION_START, size, l1_flag_array[i]);
            EU_ASSERT_REF_TC_MSG( tc_2_main_task, (result == MQX_OK), "Test #2: Test 2.2 MMU init when default flag is valid.");
            EU_ASSERT_REF_TC_MSG( tc_2_main_task, check_l1_reg(l1_table, MMU_ADD_REGION_START, MMU_ADD_REGION_START, 4 , l1_ref_array[i]), "Test #2: Test 2.3 MMU init check region.");
        }
        /* Check patern id doesnt change */
        EU_ASSERT_REF_TC_MSG( tc_2_main_task, MMU_TEST_PATERN == l1_table[((MMU_ADD_REGION_START + size) >> 20)], "Test #2: Test 2.4 Check patern after region");
        /* restore old entry */
        l1_table[((MMU_ADD_REGION_START + size) >> 20)] = temp_entry ;
    }

}
/*TASK*---------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : TEST #3:
*   Test _mmu_add_region with L2 tables.
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
    uint32_t size, result;
    free_mem = get_free_mem();
    uint32_t temp_entry;

    /** _mmu_add_vregion just change entries in L1 and L2 tables initialized in tc_1_main_task. This test do nothing directly with MMU module. Just test the API **/

    for (size = 0; size <= MMU_ADD_REGION_SIZE; size += 0x00100000)
    {
        /* Store old entry */
        temp_entry = l1_table[((MMU_ADD_REGION_START + size) >> 20)];
        /* Store patern behind tested area */
        l1_table[((MMU_ADD_REGION_START + size) >> 20)] = MMU_TEST_PATERN;

        /* Create region with 4kB blocks*/
        result = _mmu_add_vregion((void *)MMU_ADD_REGION_START, (void *)MMU_ADD_REGION_START, size, MMU_L2_4KB_RW_CN_FLAG);

        EU_ASSERT_REF_TC_MSG( tc_3_main_task, (result == MQX_OK), "Test #2: Test 3.1 MMU init when default flag is valid.");

        /* Check patern if doesnt change */
        EU_ASSERT_REF_TC_MSG( tc_3_main_task, MMU_TEST_PATERN == l1_table[((MMU_ADD_REGION_START + size) >> 20)], "Test #3: Test 3.2 Check patern behind region.");
        /* Restore old entry */
        l1_table[((MMU_ADD_REGION_START + size) >> 20)] = temp_entry ;
    }

    /* Check if changed entries ponit to L2 Table */
    for (size = 0; size < MMU_ADD_REGION_SIZE; size += 0x00100000)
    {
        EU_ASSERT_REF_TC_MSG( tc_3_main_task, MMU_IS_ENTRY_L2_DESC(l1_table[((MMU_ADD_REGION_START + size) >> 20)]), "Test #3: Test 3.3 L1 entries are L2 descriptor base address.");
    }

    /* Changing all entries to 1MB entries shoul deallocate all L2 table */
    _mmu_add_vregion((void *)MMU_ADD_REGION_START, (void *)MMU_ADD_REGION_START, MMU_ADD_REGION_SIZE, MMU_L1_1MB_RW_SO_FLAG);

    /* Test if all l2 tables are deallocated */
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, free_mem == get_free_mem(), "Test #3: Test 3.4 Deallocation of all L2 tables");
}
/*TASK*---------------------------------------------------------------------
*
* Task Name    : tc_4_main_task
* Comments     : TEST #4:
*    Check L2 table entries
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
    uint32_t result, i, size;
    l2_table=l1_table;
    /* _mmu_add_vregion just change entries in L2 table. */
    for (size = 0; size <= MMU_ADD_REGION_SIZE; size += 0x00100000)
    {
        /* Test valid flag parameters*/
        for (i = 0; i < L2_TEST_ARRAY_SIZE; i++)
        {
            result = _mmu_add_vregion((void *)MMU_ADD_REGION_START, (void *)MMU_ADD_REGION_START, size, l2_flag_array[i]);
            EU_ASSERT_REF_TC_MSG( tc_4_main_task, (result == MQX_OK), "Test #4: Test 4.1 MMU init when default flag is valid.");
            EU_ASSERT_REF_TC_MSG( tc_4_main_task, check_l2_reg(l2_table, MMU_ADD_REGION_START, MMU_ADD_REGION_START, 4 , l2_ref_array[i]), "Test #4: Test 4.2 MMU init check region.");
        }
    }
}

/*TASK*---------------------------------------------------------------------
*
* Task Name    : tc_5_main_task
* Comments     : TEST #5:
*    check different virtual regions pointing to the same PA
*    check _mmu_venable() & _mmu_vdisable()
*END*---------------------------------------------------------------------*/
void tc_5_main_task(void)
{
    uint32_t result,size;
    uint32_t *value_1, *value_2;
    size=0x00100000;
#if 0
    /* disable mmu */
    /*ENGR00277544*/
    result=_mmu_vdisable();
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, (result == MQX_OK), "Test #5: Test 5.1 MMU disable.");
    /* Test if all l2 tables are deallocated */
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, free_mem == get_free_mem(), "Test #5: Test 5.2 Deallocation of all L2 tables");
#endif
    /* Enable MMU and L1 cache */
    uint32_t *L1PageTable = _mem_alloc_align(MMU_TRANSLATION_TABLE_SIZE, MMU_TRANSLATION_TABLE_ALIGN);
    /* None cacheable is comon with strongly ordered. MMU doesnt work with another init configuration */
    result = _mmu_vinit(PSP_PAGE_TABLE_SECTION_SIZE(PSP_PAGE_TABLE_SECTION_SIZE_1MB) | PSP_PAGE_DESCR(PSP_PAGE_DESCR_ACCESS_RW_ALL) | PSP_PAGE_TYPE(PSP_PAGE_TYPE_STRONG_ORDER), (void *)L1PageTable);
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, (result == MQX_OK), "Test #5: Test 5.3 MMU init when default flag is valid.");
    /* add region in sram area */
    result = _mmu_add_vregion((void *)__INTERNAL_SRAM_BASE, (void *)__INTERNAL_SRAM_BASE, (_mem_size) 0x00100000, PSP_PAGE_TABLE_SECTION_SIZE(PSP_PAGE_TABLE_SECTION_SIZE_1MB) | PSP_PAGE_TYPE(PSP_PAGE_TYPE_CACHE_NON)   | PSP_PAGE_DESCR(PSP_PAGE_DESCR_ACCESS_RW_ALL));
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, (result == MQX_OK), "Test #5: Test 5.4 MMU init when default flag is valid.");
    /* add cached region in ddr area */
    result = _mmu_add_vregion((void *)__EXTERNAL_DDRAM_BASE, (void *)__EXTERNAL_DDRAM_BASE, __EXTERNAL_DDRAM_SIZE, PSP_PAGE_TABLE_SECTION_SIZE(PSP_PAGE_TABLE_SECTION_SIZE_1MB) | PSP_PAGE_TYPE(PSP_PAGE_TYPE_CACHE_WBNWA) | PSP_PAGE_DESCR(PSP_PAGE_DESCR_ACCESS_RW_ALL));
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, (result == MQX_OK), "Test #5: Test 5.5 MMU init when default flag is valid.");
    result = _mmu_add_vregion((void *)MMU_ADD_REGION_START, (void *)MMU_ADD_REGION_START, size, l1_flag_array[0]);
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, (result == MQX_OK), "Test #5: Test 5.6 MMU init when default flag is valid.");
    result = _mmu_add_vregion((void *)MMU_ADD_REGION_START, (void *)(MMU_ADD_REGION_START+size), size, l1_flag_array[0]);
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, (result == MQX_OK), "Test #5: Test 5.7 MMU init when default flag is valid.");
    /* enable mmu */
    result=_mmu_venable();
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, (result == MQX_OK), "Test #5: Test 5.8 MMU enable.");
    _DCACHE_ENABLE();
    _ICACHE_ENABLE();
    /* access to same PA */
    value_1=(uint32_t *)MMU_ADD_REGION_START;
    *value_1=0xFEDCBA98;
    value_2=(uint32_t *)(MMU_ADD_REGION_START+size);
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, (0xFEDCBA98 == *value_2), "Test #5: Test 5.9 MMU check when pointing to same PA.");
}

/*TASK*---------------------------------------------------------------------
*
* Task Name    : tc_6_main_task
* Comments     : TEST #6:
*
*END*---------------------------------------------------------------------*/
void tc_6_main_task(void)
{
    /* ToDo suggestion */
    /* Try to create region with different access rights(Read only or no access) and try to catch exception */
}

/******************** End Testcases ********************/

//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_mmu)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST CASE for _mmu_vinit functionality "),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST CASE for _mmu_add_region 1MB blocks functionality"),
    EU_TEST_CASE_ADD(tc_3_main_task, " TEST CASE for _mmu_add_region 4kB blocks functionality"),
    EU_TEST_CASE_ADD(tc_4_main_task, " TEST CASE for _mmu_add_vregion L2 table entry check"),
    EU_TEST_CASE_ADD(tc_5_main_task, " TEST CASE for _mmu_add_vregion when pointing to same PA"),
    EU_TEST_CASE_ADD(tc_6_main_task, " TEST CASE for _mmu_add_vregion when exception occurs"),
EU_TEST_SUITE_END(suite_mmu)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_mmu, "MMU functionality testing")
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
    _mem_free(l1_table);
    _task_block();
}/* Endbody */
/* EOF */
