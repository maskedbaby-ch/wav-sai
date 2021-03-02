/**HEADER*******************************************************************
*
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International;
* All Rights Reserved
*
****************************************************************************
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
****************************************************************************
*
* $FileName: test.cpp$
* $Version : 3.7.2.0$
* $Date    : Mar-17-2011$
*
* Comments:
*
*
*
*END************************************************************************/
#include <mqx.h>
#include <bsp.h>
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"
#include <nandflash_wl.h>
#include "wl_common.h"
#include "testing_main_internal.h"

#include "ddi_nand_hal.h"
#include "ddi_media.h"
#include "bad_block_table.h"
#include "phy_map.h"
#include "mem_management.h"
#include "test.h"

using namespace nand;

void tc_1_main_task(void);
void tc_2_main_task(void);

//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_mapper)
    EU_TEST_CASE_ADD(tc_1_main_task, " Test #1 - test_scan_and_build_bbt"),
    EU_TEST_CASE_ADD(tc_2_main_task, " Test #2 - Test mapper"),
EU_TEST_SUITE_END(suite_mapper)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_mapper, " - Mapper Testing")
EU_TEST_REGISTRY_END()

TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
/*  Task number, Entry point, Stack, Pri, String, Auto? */
   {MAIN_TASK,   main_task,   2000,  9,   "main", MQX_AUTO_START_TASK},
   {0,           0,           0,     0,   0,      0,                 }
};

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_1_main_task
* Comments       : The function is used to test Bad Block Table generator
*
*
*END------------------------------------------------------------------*/

void tc_1_main_task(void){
   MQX_FILE_PTR     nand_wl_ptr;
   _mqx_uint        result;
   BadBlockTable    bbt;
   RtStatus_t       status;
   uint32_t          i;
   uint32_t          blockCount;
   uint8_t *       bbt_nfc = NULL;

   nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NANDWL_OPEN_IGNORE_INIT_MEDIA);  /* Flag must be 'I' */
   NandHalSetMqxPtr(0, nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (nand_wl_ptr != NULL), " Test #1.1: Open file failed");

   status = media_buffer_init();
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (status == SUCCESS), " Test #1.2: Init media buffer failed");
   status = NandHal::init();
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (status == SUCCESS), " Test #1.3: Init NandHal failed");

   /* Get bbt by using method function of BadBlockTable */
   blockCount = NandHal::getTotalBlockCount();
   bbt.allocate(blockCount);
   bbt.scanAndBuildBBT();

   /* ERASE entire nandflash chip */
   result = nandwl_erase_entire_chip(nand_wl_ptr, 0, blockCount);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == TRUE), " Test #1.4: Erase entire chip failed");

   /* Scan by using NFC function and make compare */
   bbt_nfc = (uint8_t *)_mem_alloc(blockCount);
   result = ioctl(nand_wl_ptr, NANDFLASH_IOCTL_GET_BAD_BLOCK_TABLE, bbt_nfc );
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == IO_OK), " Test #1.5: Get bad block table failed");

   for (i=0; i< blockCount; i++)
   {
      result = ((bbt_nfc[i] == 1) && (!bbt.isBlockBad(i)))  ||
               ((bbt_nfc[i] == 0) && (bbt.isBlockBad(i))) ;
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == TRUE), " Test #1.6: Checking bad block table ");
   }

   if (NULL != bbt_nfc )
   {
      _mem_free(bbt_nfc);
   }
   if (bbt.isAllocated())
   {
      bbt.release();
   }

   status = NandHal::shutdown();
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (status == SUCCESS), " Test #1.7: Cannot shutdown NandHal");

   media_buffer_deinit();
   mm_display();
   mm_display_unfree();

   result = fclose(nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), " Test #1.8: Close the nand_wl file");
   
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_2_main_task
* Comments       : The function is used to test Bad Block Table object 
*                   utilities
*
*
*END------------------------------------------------------------------*/
void tc_2_main_task(void)
{
   MQX_FILE_PTR     nand_wl_ptr;
   BadBlockTable    bbt;
   RtStatus_t       status;
   uint32_t          i;
   uint32_t          blockCount;
   uint32_t          blockNumber = 50;
   uint32_t          result;

   nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NANDWL_OPEN_IGNORE_INIT_MEDIA);  /* Flag must be 'I' */
   NandHalSetMqxPtr(0, nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (nand_wl_ptr != NULL), " Test #2.1: Open file failed");

   status = media_buffer_init();
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (status == SUCCESS), " Test #2.2: Init media buffer failed");

   status = NandHal::init();
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (status == SUCCESS), " Test #2.3: Init NandHal failed");

   /* Get bbt by using method function of BadBlockTable */
   blockCount = NandHal::getTotalBlockCount();
   bbt.allocate(blockCount);
   bbt.scanAndBuildBBT();
   bbt.markBad(blockNumber);

   /* ERASE entire nandflash chip */
   result = nandwl_erase_entire_chip(nand_wl_ptr, 0, blockCount);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == TRUE), " Test #2.4: Erase entire chip failed");
   
   i = bbt.getEntry(blockNumber) ;
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (i == 0), " Test #2.5: Get entry from bbt failed ");

   if (bbt.isAllocated())
   {
      bbt.release();
   }

   status = NandHal::shutdown();
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (status == SUCCESS), " Test #2.6: Shutdown NandHal failed");

   NandHalSetMqxPtr(0, NULL);

   media_buffer_deinit();
   mm_display();
   mm_display_unfree();

   result = fclose(nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), " Test #2.8: Close the nand_wl file");

}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : main_task
* Comments       : The task tests the mapper component
*
*
*END------------------------------------------------------------------*/

void main_task(uint32_t initial_data)
{
    if (_io_nandflash_wl_install(&_bsp_nandflash_wl_init, NAND_FLASH_WL_FILE) != MQX_OK)
    {
      /* Can not install FFS */
      _task_block();
    }
    eunit_mutex_init();
    EU_RUN_ALL_TESTS(eunit_tres_array);
    eunit_test_stop();
}/* Endbody */

/* EOF */