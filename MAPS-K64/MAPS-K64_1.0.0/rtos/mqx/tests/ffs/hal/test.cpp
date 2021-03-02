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
* Comments:
*
*   This file contains main initialization for your application
*   and infinite loop
*
*END************************************************************************/


#include <mqx.h>
#include <bsp.h>
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"
#include <nandflash_wl.h>
#include "ddi_media.h"
#include "Ddi_nand_hal_internal.h"
#include "Ddi_nand_hal.h"
#include "Ddi_nand_hal_nfcphymedia.h"
#include "test.h"

extern NandHalContext_t g_nandHalContext;

void main_task (uint32_t);

/* Test suite functions*/
void tc_1_main_task(void); /* TEST #1 - Test NAND Hardware Abstract Layer */

//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_hal)
    EU_TEST_CASE_ADD(tc_1_main_task, " Test #1 - Test Nand Hal"),
EU_TEST_SUITE_END(suite_hal)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_hal, " - Testing nandhal")
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
* Comments       :
*    Testing all parameter of NandHalContext
*
*END------------------------------------------------------------------*/
void tc_1_main_task(void)
{
   MQX_FILE_PTR            nand_wl_ptr;
   _mqx_uint               result;
   RtStatus_t              status;
   NFCNandMedia            *pNand;
   IO_DEVICE_STRUCT_PTR    io_dev_ptr = NULL;
   IO_NANDFLASH_STRUCT_PTR handle_ptr = NULL;
   uint32_t                 metaSize = 0;

   /* Open nand_wl_ptr */
   nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NANDWL_OPEN_IGNORE_INIT_MEDIA);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (nand_wl_ptr != NULL), " Test #1.1: Open file failed");
   NandHalSetMqxPtr(0, nand_wl_ptr);

   io_dev_ptr = nand_wl_ptr->DEV_PTR;
   handle_ptr = (IO_NANDFLASH_STRUCT_PTR)io_dev_ptr->DRIVER_INIT_PTR;

   /* Verify that NandHal Initiliazation */
   status =  NandHal::init();
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, ( status == SUCCESS ), " Test #1.2: NandHal::init failed");

   /* Verify g_nandHalContext.nands[0]*/
   pNand = static_cast<NFCNandMedia *>(g_nandHalContext.nands[0]);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (pNand != NULL), " Test #1.3: Cannot get g_nandHalContext.nands[0] ");

   /* Verify g_nandHalContext.parameters.pageDataSize */
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (handle_ptr->VIRTUAL_PAGE_SIZE == g_nandHalContext.parameters.pageDataSize), " Test #1.4:  pageDataSize is not equal VIRTUAL_PAGE_SIZE ");

   /* get metaSize */
   result = ioctl(nand_wl_ptr, NANDFLASH_IOCTL_GET_PAGE_METADATA_AREA_SIZE, (_mqx_uint_ptr) &metaSize);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK)," Test #1.5:  Get metaSize with NANDFLASH_IOCTL_GET_PAGE_METADATA_AREA_SIZE command: failed");

   /* Verify g_nandHalContext.parameters.pageTotalSize */
   EU_ASSERT_REF_TC_MSG(tc_1_main_task,((handle_ptr->VIRTUAL_PAGE_SIZE + metaSize)== g_nandHalContext.parameters.pageTotalSize), " Test #1.6: pageTotalSize is not equal the total of metaSize and VIRTUAL_PAGE_SIZE ");

   /* Verify wPagesPerBlock*/
   EU_ASSERT_REF_TC_MSG(tc_1_main_task,((handle_ptr->NUM_VIRTUAL_PAGES / handle_ptr->NANDFLASH_INFO_PTR->NUM_BLOCKS) == g_nandHalContext.parameters.wPagesPerBlock), " Test #1.7: wPagesPerBlock is not equal the division of NUM_VIRTUAL_PAGES/NUM_BLOCKS ");

   /* Call NandHal::shutdown() */
   status = NandHal::shutdown();
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (status == SUCCESS)," Test #1.2: NandHal::shutdown failed");

   /* Close the nand_wl_ptr */
   result = fclose(nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), " Test #1.8: Close file failed");
}


/*TASK*-----------------------------------------------------------------
*
* Function Name  : main_task
* Comments       : The task test the Nand Hal component
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
