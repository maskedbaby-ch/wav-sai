/**HEADER*******************************************************************
*
* Copyright (c) 2013 Freescale Semiconductor;
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
* $Version : 4.0.2.0$
* $Date    : June-11-2013$
*
* Comments:
*
*   This file contains main initialization and functions to test ioctl 
*
* Requirements:
*   FFS010, FFS089.
* 
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"
#include "nandflash_wl.h"
#include "ddi_media.h"
#include "ddi_nand_hal.h"
#include "test.h"

/* Test suite function prototype */
void tc_1_main_task(void); /* TEST #1 - Test reading number of sector */
void tc_2_main_task(void); /* TEST #2 - Test reading sector size */

/* Define Test Suite */
EU_TEST_SUITE_BEGIN(suite_ioctl)
    EU_TEST_CASE_ADD(tc_1_main_task, " Test #1 - Test reading number of sector"),
    EU_TEST_CASE_ADD(tc_2_main_task, " Test #2 - Test reading sector size"),
EU_TEST_SUITE_END(suite_ioctl)

/* Add test suites */
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_ioctl, " - Testing ioctl")
EU_TEST_REGISTRY_END()

/* Task template structure */
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
*    Test reading number of sector
* Requirements   : 
*    FFS010.
*
*END------------------------------------------------------------------*/

void tc_1_main_task(void)
{
    MQX_FILE_PTR    nand_wl_ptr;
    _mqx_uint       result;
    uint32_t         i, temp;
    uint32_t         numberSectors_IOCTL;
    uint32_t         numberSectors_WL;
    bool         isMediaInit = FALSE;

    /* init required data */
    nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NULL);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (nand_wl_ptr != NULL), " Test #1.1: Open file failed");

    /* erase entire chip */
    for (i = NANDFLASH_1ST_DATA_DRIVE_START_BLOCK; i < NANDFLASH_1ST_DATA_DRIVE_START_BLOCK+NANDFLASH_1ST_DATA_DRIVE_SIZE_IN_BLOCK; i++)
    {
        result = ioctl(nand_wl_ptr, NANDFLASH_IOCTL_ERASE_BLOCK_FORCE, (void*) i);
        EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1.2: Erase entire chip");
    }

    /* Get number sectors by using IOCTL */
    result = ioctl(nand_wl_ptr, IO_IOCTL_GET_NUM_SECTORS, &temp);
    numberSectors_IOCTL = temp;
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK) && numberSectors_IOCTL, "Test #1.3: Testing ioctl operation ");

    /* Get number sectors by using WL module */
    result = MediaGetInfo(kInternalNandMedia, kMediaInfoIsInitialized, &isMediaInit);
    if ((SUCCESS != result) || !isMediaInit)
    {
        return;
    }

    result = DriveGetInfo(DRIVE_TAG_DATA, kDriveInfoSizeInSectors, &temp);
    if (SUCCESS != result)
    {
        return;
    }
    numberSectors_WL = temp;

    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (numberSectors_IOCTL == numberSectors_WL), "Test 1.4#: Check if number of sector is correct");
    result = fclose(nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), " Test #1.5: Close the nand_wl file");
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_2_main_task
* Comments       :
*    Test getting sector size.
* Requirements   :
*    FFS089.
*
*END------------------------------------------------------------------*/

void tc_2_main_task(void)
{
    MQX_FILE_PTR    nand_wl_ptr;
    _mqx_uint       result;
    uint32_t         i, sectorSize_IOCTL;
    uint32_t         sectorSize_WL;

    /* init required data */
    nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NULL);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (nand_wl_ptr != NULL), " Test #2.1 Open file failed");

    /* erase entire chip */
    for (i = NANDFLASH_1ST_DATA_DRIVE_START_BLOCK; i < NANDFLASH_1ST_DATA_DRIVE_START_BLOCK+NANDFLASH_1ST_DATA_DRIVE_SIZE_IN_BLOCK; i++)
    {
        result = ioctl(nand_wl_ptr, NANDFLASH_IOCTL_ERASE_BLOCK_FORCE, (void*) i);
        EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2.2: Erase entire chip ");
    }

    /* Get sector size by using IOCTL */
    result = ioctl(nand_wl_ptr, IO_IOCTL_GET_BLOCK_SIZE, &sectorSize_IOCTL);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK) && sectorSize_IOCTL, "Test #2.3: Testing ioctl operation ");

    /* Get sector size by using NFC */
    result = ioctl(nand_wl_ptr, NANDFLASH_IOCTL_GET_VIRT_PAGE_SIZE, &sectorSize_WL);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (sectorSize_IOCTL == sectorSize_WL), "Test #2.4: Check if size of sector is correct");

    result = fclose(nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), " Test #2.5: Close the nand_wl file");
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : main_task
* Comments       : This task test ioclt component
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
