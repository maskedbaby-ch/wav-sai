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
*   This file contains main initialization and functions to test driver API
*
* Requirements:
*   FFS037, FFS038, FFS039, FFS040, FFS041, FFS042.  
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"
#include "nandflash_wl.h"
#include "ddi_media.h"
#include "testing_main_internal.h"
#include "test.h"

/* Test case function prototype */
void tc_1_main_task(void);
void tc_2_main_task(void);
void tc_3_main_task(void);
void tc_4_main_task(void);

/* Define Test Suite */
EU_TEST_SUITE_BEGIN(suite_driver)
    EU_TEST_CASE_ADD(tc_1_main_task, " Test #1 - Test getting metadata size of NANDWL driver"),
    EU_TEST_CASE_ADD(tc_2_main_task, " Test #2 - Test getting spare area size of NANDWL driver"),
    EU_TEST_CASE_ADD(tc_3_main_task, " Test #3 - Test writing and reading data on a sector"),
    EU_TEST_CASE_ADD(tc_4_main_task, " Test #4 - Test writing and reading data on multiple sectors "),
EU_TEST_SUITE_END(suite_driver)

/* Add test suites */
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_driver, " - Testing driver")
EU_TEST_REGISTRY_END()

/* Task Template Structure */
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
*    FFS037, FFS038, FFS039.     
*
*END------------------------------------------------------------------*/

void tc_1_main_task(void)
{
    MQX_FILE_PTR nand_wl_ptr;
    _mqx_uint    result;
    uint32_t      i, metadataSize;
    uint32_t      numberSectors_IOCTL;
    uint32_t      numberSectors_WL;
    bool      isMediaInit = FALSE;

    /* init required data */
    nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NULL);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (nand_wl_ptr != NULL), " Test #1.1: Open file failed");

    result = nandwl_testing_get_info(nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1.2: Get nand flash information");

    /* erase entire chip */
    for (i = NANDFLASH_1ST_DATA_DRIVE_START_BLOCK; i < NANDFLASH_1ST_DATA_DRIVE_START_BLOCK+NANDFLASH_1ST_DATA_DRIVE_SIZE_IN_BLOCK; i++)
    {
        result = ioctl(nand_wl_ptr, NANDFLASH_IOCTL_ERASE_BLOCK_FORCE, (void*) i);
        EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1.3: Erase entire chip");
    }

    /* Get metadata size by using IOCTL */
    result = ioctl(nand_wl_ptr, NANDFLASH_IOCTL_GET_PAGE_METADATA_AREA_SIZE, &metadataSize);
#if defined(BSP_TWRMCF54418)
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (metadataSize == METADATA_SIZE_54418), "Test #1.4: Testing metadata size ");
#elif defined(BSP_TWRMPC5125)
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (metadataSize == METADATA_SIZE_5125), "Test #1.4: Testing metadata size ");
#elif defined(BSP_TWR_K70F120M)
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (metadataSize == METADATA_SIZE_K70), "Test #1.4: Testing metadata size ");
#elif defined(BSP_TWR_K60F120M)
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (metadataSize == METADATA_SIZE_K60), "Test #1.4: Testing metadata size ");
#endif

    result = fclose(nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), " Test #1.5: Close the nand_wl file");
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_2_main_task
* Comments       :
*    Test getting spare area size of NANDWL driver
* Requirements   : 
*    FFS037, FFS038, FFS040.       
*
*END------------------------------------------------------------------*/

void tc_2_main_task(void)
{
    MQX_FILE_PTR nand_wl_ptr;
    _mqx_uint    result;
    uint32_t      i, spareSize;

    printf("- Testing driver - tc_2_main_task - step 1");

    /* Init required data */
    nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NULL);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (nand_wl_ptr != NULL), " Test #2.1: Open file failed");

    printf("- Testing driver - tc_2_main_task - step 2");
    /* erase entire chip */
    for (i = NANDFLASH_1ST_DATA_DRIVE_START_BLOCK; i < NANDFLASH_1ST_DATA_DRIVE_START_BLOCK+NANDFLASH_1ST_DATA_DRIVE_SIZE_IN_BLOCK; i++)
    {
        result = ioctl(nand_wl_ptr, NANDFLASH_IOCTL_ERASE_BLOCK_FORCE, (void*) i);
        EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2.2: Erase entire chip");
    }

    /* Get spare size by using IOCTL */
    result = ioctl(nand_wl_ptr, NANDFLASH_IOCTL_GET_SPARE_AREA_SIZE, &spareSize);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (spareSize == SPARE_SIZE), "Test #2.3: Testing spare size ");

    result = fclose(nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), " Test #2.4: Close the nand_wl file");
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_3_main_task
* Comments       :
*    Test writing and reading data on a sector
* Requirements   :
*    FFS037, FFS038, FFS041.
*
*END------------------------------------------------------------------*/

void tc_3_main_task(void)
{
    MQX_FILE_PTR    nand_wl_ptr;
    uint32_t         buf_size = nandwl_phy_page_size + nandwl_phy_page_spare_size;
    unsigned char *       write_buffer;
    unsigned char *       read_buffer;
    uint32_t         secnum = 0;
    uint64_t         numberSector = 0;
    uint32_t         rand = simple_rand();
    uint32_t         i, result;
    /* Init required data */
    nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NULL);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (nand_wl_ptr != NULL), " Test #3.1: Open file failed");

    /* erase entire chip */
    for (i = NANDFLASH_1ST_DATA_DRIVE_START_BLOCK; i < NANDFLASH_1ST_DATA_DRIVE_START_BLOCK+NANDFLASH_1ST_DATA_DRIVE_SIZE_IN_BLOCK; i++)
    {
        result = ioctl(nand_wl_ptr, NANDFLASH_IOCTL_ERASE_BLOCK_FORCE, (void*) i);
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3.2: Erase entire chip");
    }
    write_buffer = allocate_buffer(buf_size, "write");
    fill_continous_buff(write_buffer, buf_size);

    read_buffer = allocate_buffer(buf_size, "read");
    zero_buff(read_buffer, buf_size );

    result = ioctl(nand_wl_ptr, IO_IOCTL_GET_NUM_SECTORS, &numberSector);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_OK) && numberSector, "Test #3.3: Testing ioctl operation ");

    secnum = rand % numberSector;

    fseek(nand_wl_ptr, secnum, IO_SEEK_SET);
    result = write(nand_wl_ptr, write_buffer, 1);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == 1), "Test #3.4: Testing write 1 byte on random sector");

    fseek(nand_wl_ptr, secnum, IO_SEEK_SET);
    result = read(nand_wl_ptr, read_buffer, 1);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == 1), "Test #3.4: Testing read 1 byte on random sector");


    if (!compare_test(write_buffer,read_buffer, nandwl_virt_page_size))
    {
        dump_buff(read_buffer, nandwl_virt_page_size, "Read Buffer Test");
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == 1), "Test #3.5: Testing read on virtual page");
        fseek(nand_wl_ptr, 0, IO_SEEK_SET);
        zero_buff(read_buffer, buf_size);
        if (MQX_OK == ioctl(nand_wl_ptr, NANDFLASH_IOCTL_READ_RAW, read_buffer))
            dump_buff(read_buffer, buf_size, "Read RAW Buffer");
        else
            EU_ASSERT_REF_TC_MSG(tc_3_main_task, FALSE, "Test #3.5: Testing read RAW data");
   }

   _mem_free(write_buffer);
   _mem_free(read_buffer);

   result = fclose (nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_OK), " Test #3.6: Close the nand_wl file");
}


/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_4_main_task
* Comments       :
*       Test writing and reading data on multiple sectors
* Requirements   :
*    FFS037, FFS038, FFS042.
*
*END------------------------------------------------------------------*/

void tc_4_main_task(void)
{
    MQX_FILE_PTR     nand_wl_ptr;
    uint32_t          buf_size = nandwl_phy_page_size + nandwl_phy_page_spare_size;
    unsigned char *       write_buffer;
    unsigned char *       read_buffer;
    uint32_t          secnum = 0;
    uint32_t          i = 0;
    uint32_t          num_write_on_same_sector = 100;
    uint64_t          numberSector = 0;
    uint32_t          rand = 0;
    uint32_t          result;

    /* Init required data */
    nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NULL);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (nand_wl_ptr != NULL), " Test #4.1: Open file failed");

    /* erase entire chip */
    for (i = NANDFLASH_1ST_DATA_DRIVE_START_BLOCK; i < NANDFLASH_1ST_DATA_DRIVE_START_BLOCK+NANDFLASH_1ST_DATA_DRIVE_SIZE_IN_BLOCK; i++)
    {
        result = ioctl(nand_wl_ptr, NANDFLASH_IOCTL_ERASE_BLOCK_FORCE, (void*) i);
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4.2: Erase entire chip");
    }

      write_buffer = allocate_buffer(buf_size, "write");
      fill_continous_buff(write_buffer, buf_size);

      read_buffer = allocate_buffer(buf_size, "read");
      zero_buff(read_buffer, buf_size );

      result = ioctl(nand_wl_ptr, IO_IOCTL_GET_NUM_SECTORS, &numberSector);
      EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_OK) && numberSector, "Test #3.3: Testing ioctl operation ");

      while (i < num_write_on_same_sector)
      {
         rand = simple_rand();
         secnum = rand % numberSector;

         WL_TEST_LOG("Write data on lsector %d \n", secnum);
         fseek(nand_wl_ptr, secnum, IO_SEEK_SET);
         result = write(nand_wl_ptr, write_buffer, 1);
         EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == 1), "Test #4.4: Testing write 1 byte on random sector");


         WL_TEST_LOG("Read data on lsector %d \n", secnum);
         fseek(nand_wl_ptr, secnum, IO_SEEK_SET);
         result = read(nand_wl_ptr, read_buffer, 1);
         EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == 1), "Test #4.5: Testing read 1 byte on random sector");

         if (!compare_test(write_buffer,read_buffer, nandwl_virt_page_size))
         {
            dump_buff(read_buffer, nandwl_virt_page_size, "Read Buffer Test");
            EU_ASSERT_REF_TC_MSG(tc_4_main_task, FALSE, "Test #4.6: Testing read on virtual page");
            fseek(nand_wl_ptr, 0, IO_SEEK_SET);
            zero_buff(read_buffer, buf_size);
            if (MQX_OK == ioctl(nand_wl_ptr, NANDFLASH_IOCTL_READ_RAW, read_buffer))
                dump_buff(read_buffer, buf_size, "Read RAW Buffer");
            else
                EU_ASSERT_REF_TC_MSG(tc_4_main_task, FALSE, "Test #4.7: Testing read RAW data");
         }
         i++;
      }


      _mem_free(write_buffer);
      _mem_free(read_buffer);
      result = fclose(nand_wl_ptr);
      EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_OK), " Test #4.8: Close the nand_wl file");
}
/*TASK*-----------------------------------------------------------------
*
* Function Name  : main_task
* Comments       : 
*    This task installs nandflash_wl & runs the test suite   
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
