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
* $Version : 3.8.12.0$
* $Date    : Jul-20-2012$
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
#include "ddi_nand_hal_internal.h"
#include "ddi_nand_hal.h"
#include "ddi_nand_hal_nfcphymedia.h"
#include "ddi_Nandhalspy.h"
#include "testing_main_internal.h"
#include "ddi_nand_hal.h"
#include "system_testing.h"
#include "system_testing_internal.h"
#include "test.h"

//------------------------------------------------------------------------
void tc_1_main_task(void);
void tc_2_main_task(void);
void tc_3_main_task(void);
void tc_4_main_task(void);
void tc_5_main_task(void);
void tc_6_main_task(void);
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_wrapper)
    EU_TEST_CASE_ADD(tc_1_main_task, " Test #1 - Test initialization & close media "),
    EU_TEST_CASE_ADD(tc_2_main_task, " Test #2 - Test build media from pre-define information"),
    EU_TEST_CASE_ADD(tc_3_main_task, " Test #3 - Test Erase DataDrive"),
    EU_TEST_CASE_ADD(tc_4_main_task, " Test #4 - Test Repair DataDrive"),
    EU_TEST_CASE_ADD(tc_5_main_task, " Test #5 - Test write/read data on a sector"),
    EU_TEST_CASE_ADD(tc_6_main_task, " Test #6 - Test write/read data on multiple sector"),
EU_TEST_SUITE_END(suite_wrapper)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_wrapper, " - Testing Wrapper")
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
*    The function is used to test initialize & close the media
*
*END------------------------------------------------------------------*/

void tc_1_main_task(void) {
   MQX_FILE_PTR     nand_wl_ptr;
   _mqx_uint        result, i;
   RtStatus_t       status;

   /* Step 1: Open NANDFLASH WL*/
   nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NANDWL_OPEN_IGNORE_INIT_MEDIA);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (nand_wl_ptr != NULL), " Test #1.1: Open file failed");
   NandHalSetMqxPtr(0, nand_wl_ptr);

   nandwl_testing_get_info(nand_wl_ptr);

   /* Step 2: Erase entire chip :   nandwl_erase_entire_chip();*/
   for (i = NANDFLASH_1ST_DATA_DRIVE_START_BLOCK; i < NANDFLASH_1ST_DATA_DRIVE_START_BLOCK+NANDFLASH_1ST_DATA_DRIVE_SIZE_IN_BLOCK; i++)
   {
      result = ioctl(nand_wl_ptr, NANDFLASH_IOCTL_ERASE_BLOCK_FORCE, (void*) i);
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1.2: Erase entire chip");
   }

   /* Step 3: Init media buffer */
   status = media_buffer_init();
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (status == SUCCESS), "Test #1.3: Init media buffer");

   /* Step 4: Initialize MEDIA */
   status = MediaInit(kInternalNandMedia);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (status == SUCCESS), "Test #1.4: Init Internal media failed");

   /* Step 5: Shutdown MEDIA */
   status = MediaShutdown(kInternalNandMedia);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, status == SUCCESS, "Test #1.5: Shutdown Internal media failed");

   /*Step 6: Close NANDFLASH WL*/
   status = media_buffer_deinit();
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (status == SUCCESS), " Test #1.6: De-init media buffer failed");
   result = fclose(nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), " Test #1.7: Close file failed");
}


/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_2_main_task
* Comments       :
*    This function is used to test build media from pre-define information
*
*END------------------------------------------------------------------*/
void tc_2_main_task(void) {
   MQX_FILE_PTR     nand_wl_ptr;
   _mqx_uint        result;
   RtStatus_t       status;

   /* Step 1: Open NANDFLASH WL*/
   nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NANDWL_OPEN_IGNORE_INIT_MEDIA);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (nand_wl_ptr != NULL), " Test #2.1: Open file failed");
   NandHalSetMqxPtr(0, nand_wl_ptr);

   /* Step 2: Init media buffer */
   status = media_buffer_init();
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (status == SUCCESS), "Test #2.2: Init media buffer failed");

   /* Step 3: Initialize MEDIA */
   status = MediaInit(kInternalNandMedia);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (status == SUCCESS), "Test #2.3: Init Internal media failed");

   /* Step 4: Call MediaBuildFromPreDefine() */
   status = MediaBuildFromPreDefine();
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (status == SUCCESS), "Test #2.4: MediaBuildFromPreDefine() failed");

   /* Step 5: Initialize DATADRIVE */
   status = DriveInit(DRIVE_TAG_DATA);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (status == SUCCESS), "Test #2.5: Init DataDrive failed");

   /* Step 6: Shutdown MEDIA */
   status = MediaShutdown(kInternalNandMedia);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (status == SUCCESS), "Test #2.6: Shutdown Internal media failed");

   /* Step 7: Deinit media buffer*/
   status = media_buffer_deinit();
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (status == SUCCESS), "Test #2.7: Deinit media buffer failed");

   /* Step 8: */
   result = fclose(nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), " Test #2.8: Close file failed");

}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_3_main_task
* Comments       :
*    This function is used to test erase DataDrive
*
*END------------------------------------------------------------------*/
void tc_3_main_task(void) {
   MQX_FILE_PTR     nand_wl_ptr;
   _mqx_uint        result;
   RtStatus_t       status;

   /* Step 1: Open NANDFLASH WL*/
   nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NANDWL_OPEN_IGNORE_INIT_MEDIA);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (nand_wl_ptr != NULL), " Test #3.1: Open file failed");
   NandHalSetMqxPtr(0, nand_wl_ptr);

   /* Step 2: Init media buffer */
   status = media_buffer_init();
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (status == SUCCESS), "Test #3.2: Init media buffer failed");


   /* Step 3: Initialize MEDIA */
   status = MediaInit(kInternalNandMedia);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, status == SUCCESS, "Test #3.3: Init Internal media failed");

   /* Step 4: Call MediaBuildFromPreDefine() */
   status = MediaBuildFromPreDefine();
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, status == SUCCESS, "Test #3.4: MediaBuildFromPreDefine() failed");

   /* Step 5: Initialize DATADRIVE */
   status = DriveInit(DRIVE_TAG_DATA);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, status == SUCCESS, "Test #3.5: Init DataDrive failed");

   /* Step 6: Erase DATADRIVE */
   status = DriveErase(DRIVE_TAG_DATA, 0 /* Currently unsused */);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, status == SUCCESS, "Test #3.6:Erase DataDrive failed");

   /* Step 7: Shutdown MEDIA */
   status = MediaShutdown(kInternalNandMedia);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, status == SUCCESS, "Test #3.7: Shutdown Internal media failed");

   /* Step 8: Deinit media buffer */
   status = media_buffer_deinit();
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (status == SUCCESS) , "Test #3.8: Deinit media buffer");

   /* Step 9: Close NAND WL */
   result = fclose(nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_OK), " Test #3.9: Close file failed");

}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_4_main_task
* Comments       :
*   This function is used to test Repair DataDrive
*
*END------------------------------------------------------------------*/
void tc_4_main_task(void) {
   MQX_FILE_PTR     nand_wl_ptr;
   _mqx_uint        result;
   RtStatus_t       status;

   /* Step 1: Open NANDFLASH WL*/
   nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NANDWL_OPEN_IGNORE_INIT_MEDIA);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, (nand_wl_ptr != NULL), " Test #4.1: Open file failed");
   NandHalSetMqxPtr(0, nand_wl_ptr);

   /* Step 2: Init media buffer */
   status = media_buffer_init();
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, (status == SUCCESS), "Test #4.2: Init media buffer failed");

   /* Step 3: Initialize MEDIA */
   status = MediaInit(kInternalNandMedia);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, status == SUCCESS, "Test #4.3: Init Internal media failed");

   /* Step 4: Call MediaBuildFromPreDefine() */
   status = MediaBuildFromPreDefine();
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, status == SUCCESS, "Test #4.4: MediaBuildFromPreDefine() failed");

   /* Step 5: Initialize DATADRIVE */
   status = DriveInit(DRIVE_TAG_DATA);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, status == SUCCESS, "Test #4.5: Init DataDrive failed");

   /* Step 6: Repair DATADRIVE */
   status = DriveRepair(DRIVE_TAG_DATA, 0 /* Currently unsused */, FALSE);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, (status == SUCCESS || status == ERROR_DDI_LDL_LDRIVE_FS_FORMAT_REQUIRED), "Test #4.6: Repair DataDrive failed");

  /* Step 7: Shutdown MEDIA */
   result = MediaShutdown(kInternalNandMedia);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == SUCCESS, "Test #4.7: Shutdown Internal media failed");

   /* Step 8: Deinit media buffer */
   status = media_buffer_deinit();
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, (status == SUCCESS), "Test #4.8: Deinit media buffer");

   /* Step 9: Close NAND WL */
   result = fclose(nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_OK), " Test #4.9: Close file failed");
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_5_main_task
* Comments       :
*    This function is used to test Read/write a sector
*
*END------------------------------------------------------------------*/
void tc_5_main_task(void) {

    MQX_FILE_PTR    nand_wl_ptr;
    _mqx_uint       result;
    RtStatus_t      status;
    unsigned char        * writeBuffer;
    unsigned char        * readBuffer;
    uint64_t         numberSector;
    uint32_t         secnum;
    uint32_t         rand = simple_rand();

    /* Step 1: Open NANDFLASH WL*/
    nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NANDWL_OPEN_IGNORE_INIT_MEDIA);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (nand_wl_ptr != NULL), " Test #5.1: Open file failed");
    NandHalSetMqxPtr(0, nand_wl_ptr);

    /* Step 2: Init media buffer */
    status = media_buffer_init();
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (status == SUCCESS), "Test #5.2: Init media buffer failed");

    /* Step 3: Initialize MEDIA */
    status = MediaInit(kInternalNandMedia);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, status == SUCCESS, "Test #5.3: Init Internal media failed");

    /* Step 4: Call MediaBuildFromPreDefine() */
    status = MediaBuildFromPreDefine();
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, status == SUCCESS, "Test #5.4: MediaBuildFromPreDefine() failed");

    /* Step 5: Initialize DATADRIVE */
    status = DriveInit(DRIVE_TAG_DATA);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, status == SUCCESS, "Test #5.5: Init DataDrive failed");

    /* Step 6: Get numberSector of DATADRIVE */
    status = DriveGetInfo(DRIVE_TAG_DATA, kDriveInfoSizeInSectors, &numberSector);
    secnum = rand % numberSector;
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (status == SUCCESS), "Test #5.6: Get Info from DATADRIVE failed");

    /* Step 7: allocate writeBuffer */
    writeBuffer = allocate_buffer(nandwl_virt_page_size, "writeBuffer");
    fill_continous_buff(writeBuffer, nandwl_virt_page_size);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, writeBuffer != NULL, "Test #5.7: Allocated writeBuffer failed");

    /* Step 8: Write this allocated buffer to DATADRIVE */
    status = DriveWriteSector(DRIVE_TAG_DATA, secnum, writeBuffer);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, status == SUCCESS, "Test #5.8: Write to DATADRIVE failed");


    /* Step 9: allocate readBuffer */
    readBuffer = allocate_buffer(nandwl_virt_page_size, "readBuffer");
    zero_buff(readBuffer, nandwl_virt_page_size);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, readBuffer != NULL, "Test #5.9: Allocated readBuffer failed");

    /*Step 10: Read sector at secnum locations from DATADRIVE */
    status = DriveReadSector(DRIVE_TAG_DATA, secnum, readBuffer);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, status == SUCCESS, "Test #5.10: Read from DATADRIVE failed");

    /*Step 11: Compare readBuffer & writeBuffer */
    result = memcmp(writeBuffer, readBuffer, nandwl_virt_page_size);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == 0, "Test #5.11: Readbuffer & writeBuffer are not equal.");

    /* Step 12: Shutdown MEDIA */
    status = MediaShutdown(kInternalNandMedia);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, status == SUCCESS, "Test #5.12: Shutdown Internal media failed");

    /* Step 13: Deinit media buffer */
    status = media_buffer_deinit();
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, status == SUCCESS, "Test #5.13: Deinit media buffer");

    /* Step 14: Close NAND WL */
    result = fclose(nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_OK), " Test #5.14: Close file failed");

    /* Step 15: free all allocated buffer*/
    _mem_free(writeBuffer);
    _mem_free(readBuffer);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (TRUE), " Test #5.15: Cannot free allocated memory ");

}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_6_main_task
* Comments       :
*    This function is used to test write/read data on a sector
*
*END------------------------------------------------------------------*/
void tc_6_main_task(void) {
   MQX_FILE_PTR nand_wl_ptr;
   RtStatus_t status;
   unsigned char        * writeBuffer;
   unsigned char        * readBuffer;
   uint64_t numberSector;
   uint32_t secnum, rand, i, counter = 100;
   uint32_t result;

   /* Step 1: Open NANDFLASH WL*/
   nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NANDWL_OPEN_IGNORE_INIT_MEDIA);
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, (nand_wl_ptr != NULL), " Test #6.1: Open file failed");
   NandHalSetMqxPtr(0, nand_wl_ptr);

   /* Step 2: Erase entire chip :   nandwl_erase_entire_chip();*/
   for (i = NANDFLASH_1ST_DATA_DRIVE_START_BLOCK; i < NANDFLASH_1ST_DATA_DRIVE_START_BLOCK+NANDFLASH_1ST_DATA_DRIVE_SIZE_IN_BLOCK; i++)
   {
      result = ioctl(nand_wl_ptr, NANDFLASH_IOCTL_ERASE_BLOCK_FORCE, (void*) i);
      EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6.2: Erase entire chip");
   }

   /* Step 2: Init media buffer */
   status = media_buffer_init();
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, (status == SUCCESS), "Test #6.3: Init media buffer failed");

   /* Step 3: Initialize MEDIA */
   status = MediaInit(kInternalNandMedia);
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, (status == SUCCESS), "Test #6.4: Init Internal media failed");

   /* Step 4: Call MediaBuildFromPreDefine() */
   status = MediaBuildFromPreDefine();
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, (status == SUCCESS), "Test #6.5: MediaBuildFromPreDefine() failed");

   /* Step 5: Initialize DATADRIVE */
   status = DriveInit(DRIVE_TAG_DATA);
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, (status == SUCCESS), "Test #6.6: Init DataDrive failed");

   writeBuffer = allocate_buffer(nandwl_virt_page_size, "writeBuffer");
   if (NULL == writeBuffer) {
      EU_ASSERT_REF_TC_MSG(tc_6_main_task, FALSE, "Test #6.7: Allocate Write Buffer failed");
      _task_block();
   }
   readBuffer = allocate_buffer(nandwl_virt_page_size, "readBuffer");
   if (NULL == readBuffer) {
         EU_ASSERT_REF_TC_MSG(tc_6_main_task, FALSE, "Test #6.8: Allocate Read Buffer failed");
         _task_block();
   }
   for (i = 0; i < counter; i++) {

      /* WRITE */
      DriveGetInfo(DRIVE_TAG_DATA, kDriveInfoSizeInSectors, &numberSector);
      rand = simple_rand();
      secnum = rand % numberSector;

      fill_continous_buff(writeBuffer, nandwl_virt_page_size);

      status = DriveWriteSector(DRIVE_TAG_DATA, secnum, writeBuffer);
      EU_ASSERT_REF_TC_MSG(tc_6_main_task, (status == SUCCESS), "Test #6.9: Driver Write Sector failed");

      /* READ */
      zero_buff(readBuffer, nandwl_virt_page_size);

      status = DriveReadSector(DRIVE_TAG_DATA, secnum, readBuffer);
      EU_ASSERT_REF_TC_MSG(tc_6_main_task, (status == SUCCESS), "Test #6.10: Driver Read Sector failed");

      /* Compare */
      result = compare_test(writeBuffer,readBuffer, nandwl_virt_page_size);
      EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == TRUE, "Test #6.11: Comapare between writeBuffer & readBuffer");
   }

   status = MediaShutdown(kInternalNandMedia);
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, status == SUCCESS, "Test #6.12: Shutdown Internal media failed");

   /* Step 13: Deinit media buffer */
   status = media_buffer_deinit();
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, (status == SUCCESS), "Test #6.13: Deinit media buffer");

   /* Step 14: Close NAND WL */
   result = fclose(nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == MQX_OK), " Test #6.14: Close file failed");

   _mem_free(writeBuffer);
   _mem_free(readBuffer);
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : main_task
* Comments       :
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
