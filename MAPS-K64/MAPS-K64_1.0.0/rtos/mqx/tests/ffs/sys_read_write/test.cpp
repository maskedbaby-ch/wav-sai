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
* $FileName: test.cpp$
* $Version : 4.0.2.0$
* $Date    : June-11-2013$
*
* Comments:
*
*   This file contains the funtions to test read & write system in normal case.
*
* Requirements:
*   FFS073, FFS074, FFS075.
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
#include "system_testing.h"
#include "system_testing_internal.h"
#include "simple_timer.h"
#include "mem_management.h"
#include "test.h"

/* Test suite function prototype */
void tc_1_main_task(void);
void tc_2_main_task(void);
void tc_3_main_task(void);
void tc_4_main_task(void);
void tc_5_main_task(void);
void tc_6_main_task(void);

/* Define Test Suite */
EU_TEST_SUITE_BEGIN(suite_sys_read_write)
    EU_TEST_CASE_ADD(tc_1_main_task, " Test #1: Test read/write sector with NAND WL module"),
    EU_TEST_CASE_ADD(tc_2_main_task, " Test #2: Test read/write many sectors with NAND WL module"),
    EU_TEST_CASE_ADD(tc_3_main_task, " Test #3: Test read/write DIRECTORY with MFS"),
    EU_TEST_CASE_ADD(tc_4_main_task, " Test #4: Test read/write FILE with MFS"),
    EU_TEST_CASE_ADD(tc_5_main_task, " Test #5: Test write/delete with MFS"),
    EU_TEST_CASE_ADD(tc_6_main_task, " Test #6: Test write/format with MFS"),
EU_TEST_SUITE_END(suite_sys_read_write)

/* Add test suites */
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_sys_read_write, " - Testing Read/Write")
EU_TEST_REGISTRY_END()

/* Task template structure */
TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
    /*  Task number, Entry point, Stack, Pri, String, Auto? */
    {MAIN_TASK,   main_task,   5000,  9,   "main", MQX_AUTO_START_TASK},
    {0,           0,           0,     0,   0,      0,                 }
};

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_1_main_task
* Comments       :
*      The function is used to test read/write sector with NandWl module
* Requirements   :
*      FFS073.
*
*END------------------------------------------------------------------*/
void tc_1_main_task(void ) {
    MQX_FILE_PTR             nand_wl_ptr;
    uint32_t                  result;
    RtStatus_t               status;
   unsigned char                *write_buffer, *read_buffer;
    uint32_t                  buf_size;
    uint32_t                  secnum;
    uint32_t                  num_write_on_same_sector = 100;
    uint32_t                  i = 0;
    uint64_t                  numberSector = 23552;

    nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NANDWL_OPEN_IGNORE_INIT_MEDIA);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (nand_wl_ptr != NULL), " Test #1.1: Open file failed");
    NandHalSetMqxPtr(0, nand_wl_ptr);

    result = nandwl_erase_entire_chip(nand_wl_ptr, NANDFLASH_1ST_DATA_DRIVE_START_BLOCK, NANDFLASH_1ST_DATA_DRIVE_START_BLOCK + NANDFLASH_1ST_DATA_DRIVE_SIZE_IN_BLOCK);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, ((bool)(bool)result == TRUE), " Test #1.2: Erase entire chip");

    status =  system_test_initialize(TRUE,nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (status == SUCCESS), " Test #1.4: Init the logical media & buffer manager for nand drive");

    nandwl_testing_get_info(nand_wl_ptr);    /*Get nandwl_phy_page_size & nandwl_phy_page_spare_size*/
    buf_size = nandwl_phy_page_size + nandwl_phy_page_spare_size;

   printf("\n ============== tc_1_main_task  ================== \n");
    write_buffer = allocate_buffer(nandwl_virt_page_size, "write");
    if (NULL == write_buffer){
        EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, " Test #1.4: Allocate write_buffer buffer");
        fclose(nand_wl_ptr);
        return;
    }
    fill_continous_buff(write_buffer, nandwl_virt_page_size);

    read_buffer = allocate_buffer(nandwl_virt_page_size, "read");
    if (NULL == read_buffer){
        EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, " Test #1.5: Allocate read_buffer buffer");
        _mem_free(write_buffer);
        fclose(nand_wl_ptr);
        return;
    }
    zero_buff(read_buffer, nandwl_virt_page_size );

    // TODO: confirm that this block of secnum is not bad

    status = DriveGetInfo(DRIVE_TAG_DATA, kDriveInfoSizeInSectors, &numberSector);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (status == SUCCESS), " Test #1.6: Get Drive information");

    while (i < num_write_on_same_sector)
    {
        uint32_t rand = simple_rand();
        secnum = rand % numberSector;

        fseek(nand_wl_ptr, secnum, IO_SEEK_SET);
        result = write(nand_wl_ptr, write_buffer, 1);
        EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == 1), " Test #1.7: Write a sector to Nand flash");

        fseek(nand_wl_ptr, secnum, IO_SEEK_SET);
        result = read(nand_wl_ptr, read_buffer, 1);
        EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == 1), " Test #1.8: Read a sector from Nand flash");

        /*dump_buff(nandwl_read_buffer, nandwl_virt_page_size, "Read Buffer Test");*/
        result = compare_test(write_buffer,read_buffer, nandwl_virt_page_size);
        EU_ASSERT_REF_TC_MSG(tc_1_main_task, ((bool)result == TRUE), " Test #1.9: Compare data between write_buffer & read_buffer");

        i++;
    }

    _mem_free(write_buffer);
    _mem_free(read_buffer);
    status = system_test_finalize(nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (status == SUCCESS), " Test #1.10: Shutdown & de-init nand_wl flash system");

    result = fclose(nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), " Test #1.11: Close the nand_wl file");
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_2_main_task
* Comments       : The function is used to test read/write many sectors
* Requirements   : FFS074.
*
*END------------------------------------------------------------------*/
void tc_2_main_task(void )
{
    MQX_FILE_PTR     nand_wl_ptr;
    uint32_t          result;
    RtStatus_t       status;
    uint32_t          buf_size = nandwl_phy_page_size + nandwl_phy_page_spare_size;
   unsigned char         *write_buffer, *read_buffer;
    uint32_t          secnum;
    uint32_t          num_write_on_same_sector = 2048;
    uint32_t          i = 0;
    uint64_t          numberSector = 23552;
    uint32_t          rand = simple_rand();

    nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NANDWL_OPEN_IGNORE_INIT_MEDIA);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (nand_wl_ptr != NULL), " Test #2.1: Open file failed");
    NandHalSetMqxPtr(0, nand_wl_ptr);

    /* ERASE entire nandflash chip */
    result = nandwl_erase_entire_chip(nand_wl_ptr, NANDFLASH_1ST_DATA_DRIVE_START_BLOCK, NANDFLASH_1ST_DATA_DRIVE_START_BLOCK + NANDFLASH_1ST_DATA_DRIVE_SIZE_IN_BLOCK);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, ((bool)result == TRUE), " Test #2.1: Open file failed");

    status =  system_test_initialize(TRUE,nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (status == SUCCESS), " Test #2.3: Initialize Nand_wl drive");

    status = DriveGetInfo(DRIVE_TAG_DATA, kDriveInfoSizeInSectors, &numberSector);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (status == SUCCESS), " Test #2.4: Get information of Nand_wl drive");

    secnum = rand % numberSector;

    WL_TEST_LOG("\n ============== tc_2_main_task  ================== \n");
    write_buffer = allocate_buffer(nandwl_virt_page_size, "write");
    if (NULL == write_buffer){
        EU_ASSERT_REF_TC_MSG(tc_2_main_task, FALSE, " Test #2.5: Allocate write_buffer buffer ");
        fclose(nand_wl_ptr);
        return;
    }
    fill_continous_buff(write_buffer, nandwl_virt_page_size);


    read_buffer = allocate_buffer(nandwl_virt_page_size, "read");
    if (NULL == read_buffer){
        EU_ASSERT_REF_TC_MSG(tc_2_main_task, FALSE, " Test #2.6: Allocate read_buffer buffer ");
        _mem_free(write_buffer);
        fclose(nand_wl_ptr);
        return;
    }
    zero_buff(read_buffer, nandwl_virt_page_size );

    while (i < num_write_on_same_sector)
    {
        WL_TEST_LOG("Write data on lsector %d \n", secnum);
        fseek(nand_wl_ptr, secnum, IO_SEEK_SET);
        result = write(nand_wl_ptr, write_buffer, 1);
        EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == 1), " Test #2.7: Write a sector to Nand flash");

        WL_TEST_LOG("Read data on lsector %d \n", secnum);
        fseek(nand_wl_ptr, secnum, IO_SEEK_SET);
        result = read(nand_wl_ptr, read_buffer, 1);
        EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == 1), " Test #2.8: Read a sector from Nand flash");

        /*dump_buff(nandwl_read_buffer, nandwl_virt_page_size, "Read Buffer Test");*/
        result = compare_test(write_buffer,read_buffer, nandwl_virt_page_size);
      EU_ASSERT_REF_TC_MSG(tc_2_main_task, ((bool)(bool)result == TRUE), " Test #2.9: Compare data between write_buffer & read_buffer");      

        i++;
    }

    _mem_free(write_buffer);
    _mem_free(read_buffer);
    status = system_test_finalize(nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (status == SUCCESS), " Test #2.10: Shutdown & De-init Nand_wl drive");

    result = fclose(nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), " Test #2.11: Close the Nand_wl file");
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_3_main_task
* Comments       : The function is used to test read/write with MFS
* Requirements   : FFS075.
*
*END------------------------------------------------------------------*/
void tc_3_main_task(void){
    MQX_FILE_PTR     nand_wl_ptr;
    uint32_t          result;
    RtStatus_t       status;
    dir_arrs_type    mkdirs_arr={ 0 };
    dir_arrs_type    readdirs_arr={ 0 };
    uint32_t          num_mkdirs = 100;
    uint32_t          num_readdirs ;
    char *         fsname = "a:";
    uint32_t          i = 0;
    uint32_t          j = 0;

    nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NANDWL_OPEN_IGNORE_INIT_MEDIA);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (nand_wl_ptr != NULL), "Test #3.1: Open file failed");
    NandHalSetMqxPtr(0, nand_wl_ptr);

    /* ERASE entire nandflash chip */
    result = nandwl_erase_entire_chip(nand_wl_ptr, NANDFLASH_1ST_DATA_DRIVE_START_BLOCK, NANDFLASH_1ST_DATA_DRIVE_START_BLOCK + NANDFLASH_1ST_DATA_DRIVE_SIZE_IN_BLOCK);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, ((bool)result == TRUE), " Test #3.2: Erase entire nand flash chip");

    WL_TEST_LOG("\n ============== system_test_read_write_with_mfs_00  ================== \n");

    g_wl_log_level = WL_LOG_ERROR;
    /*g_wl_log_module_attributes= WL_MODULE_NANDWL_TESTING;*/
    g_nfc_log_enable = FALSE;

    for (j=0; j<5; j++)
    {
        WL_TEST_LOG("\n ------------> Create dir %d   ================== \n", j);
        num_mkdirs = NUM_DIR_TEST;
        for (i=0; i< num_mkdirs; i++ )
        {
            sprintf( (char *)&mkdirs_arr[i], "DIR%d%d", j,i);
        }

        result = system_test_read_write_with_mfs_mkdir_internal(nand_wl_ptr, fsname, &mkdirs_arr, num_mkdirs, false );
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, ( result == MQX_OK), " Test #3.3: Create directory with MFS internal");

        num_readdirs = 0;
        result = system_test_read_write_with_mfs_readdir_internal(nand_wl_ptr, fsname, &readdirs_arr, num_mkdirs, false );
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, ( result == MQX_OK), " Test #3.4: Read all the created directories with MFS internal");

        for (i=0; i<num_mkdirs; i++)
        {
            result = compare_test((unsigned char * ) &mkdirs_arr[i], (unsigned char * )&readdirs_arr[i],strlen((char * ) &mkdirs_arr[i]) );
            EU_ASSERT_REF_TC_MSG(tc_3_main_task, ( (bool)result == TRUE), " Test #3.5: Compare these directories which is created & read");
        }

        /*==========Create nested dir=============*/
        WL_TEST_LOG("\n ------------> Create nested dir   ================== \n");
        num_mkdirs = NUM_DIR_TEST;
        for (i=0; i< num_mkdirs; i++ )
        {
            sprintf( (char *)&mkdirs_arr[i], "DN%d%d", i,i);
        }
        zero_buff((unsigned char *)&readdirs_arr, sizeof(readdirs_arr));

        result = system_test_read_write_with_mfs_mkdir_internal(nand_wl_ptr, fsname, &mkdirs_arr, num_mkdirs, true );
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, ( result == MQX_OK), " Test #3.6: Create the nested directory");

        result = system_test_read_write_with_mfs_readdir_internal(nand_wl_ptr, fsname, &readdirs_arr, num_mkdirs, true ) ;
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_OK), " Test #3.7: Read all these nested directories");

        for (i=0; i<num_mkdirs; i++)
        {
            result = compare_test((unsigned char * ) &mkdirs_arr[i], (unsigned char * )&readdirs_arr[i],strlen((char * ) &mkdirs_arr[i]) );
            EU_ASSERT_REF_TC_MSG(tc_3_main_task, ((bool)result == TRUE), " Test #3.8: Compare all directories which are created & read");
        }
    }

    result = fclose(nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_OK), " Test #3.9: Close the nand_wl file");
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_4_main_task
* Comments       : The function is used to test read/write with MFS
* Requirements   : FFS075.
*
*END------------------------------------------------------------------*/
#define WRITE_BUFFER_MAXIMUM_SIZE 1024
void tc_4_main_task(void)
{
    uint32_t write_size[] = {
        1024*5, 1024*10, 1024*15, 1024*20, 1024*25,
        1024*30, 1024*35, 1024*40, 1024*45, 1024*50,
        1024*55, 1024*60, 1024*65, 1024*70, 1024*75,
        1024*80, 1024*85, 1024*90, 1024*95, 1024*100
    };
    unsigned char *        write_buffer = NULL;
    MQX_FILE_PTR     nand_wl_ptr = NULL;
    uint32_t          result;
    RtStatus_t       status;
    dir_arrs_type    mkdirs_arr={ 0 };
    dir_arrs_type    readdirs_arr={ 0 };
    uint32_t          num_mkdirs = 100;
    uint32_t          num_readdirs ;
    char *         fsname = "a:";
    uint32_t          i = 0;
    uint32_t          j = 0;
    uint32_t          count;
    char             fpath[PATHNAME_SIZE] = {0};
    MQX_FILE_PTR     fshandle = NULL;
    MQX_FILE_PTR     fhandle = NULL;
    char             c;
    char             c_read;

    /* Open NAND Flash device */
    nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NANDWL_OPEN_IGNORE_INIT_MEDIA);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (nand_wl_ptr != NULL), "Test #6.1: Open NAND Flash device");
    NandHalSetMqxPtr(0, nand_wl_ptr);

    /* Erase NAND Flash blocks defined */
    result = nandwl_erase_entire_chip(nand_wl_ptr, NANDFLASH_1ST_DATA_DRIVE_START_BLOCK, NANDFLASH_1ST_DATA_DRIVE_START_BLOCK + NANDFLASH_1ST_DATA_DRIVE_SIZE_IN_BLOCK);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, ((bool)result == TRUE), " Test #6.2: Erase NAND Flash blocks defined");

    /* Allocate write buffer */
    write_buffer = (unsigned char *)_mem_alloc_zero(WRITE_BUFFER_MAXIMUM_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (write_buffer != NULL), " Test #6.3: Allocate a buffer to write on NAND Flash");
    if (write_buffer == NULL)
    {
        goto error_handle;
    }

    /* Fill data to write buffer */
    c = '0';
    write_buffer[0] = 'M';
    write_buffer[1] = 'Q';
    write_buffer[2] = 'X';
    write_buffer[3] = '_';
    for (i = 4; i < WRITE_BUFFER_MAXIMUM_SIZE; i++)
    {
        write_buffer[i] = c;
        c = (c == 'z') ? '0' : c+1;
    }

    g_wl_log_level = WL_LOG_ERROR;
    // g_wl_log_module_attributes= WL_MODULE_NANDWL_TESTING;
    g_nfc_log_enable = FALSE;

    /* Initialize NAND Flash device */
    status = system_test_initialize(TRUE, nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (status == SUCCESS), " Test #6.4: Initialize NAND Flash device");
    if (status != MQX_OK)
    {
        goto error_handle;
    }

    /* Install and open MFS on NAND Flash device */
    status = system_test_mfs_open(nand_wl_ptr, fsname, &fshandle, TRUE);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (status == MFS_NO_ERROR), " Test #6.5: Install and open MFS on NAND Flash device");
    if (status != MFS_NO_ERROR)
    {
        goto error_handle;
    }

    /* Write file */
    for (i = 0; i < sizeof(write_size)/sizeof(uint32_t); i++)
    {
        count = write_size[i];
        sprintf(fpath, "%s\\FILE%d", fsname, i);
        fhandle = fopen(fpath, "a");
        fseek(fhandle, 0, IO_SEEK_CUR);
        while (count)
        {
            result = write(fhandle, (unsigned char *)(write_buffer), (count > WRITE_BUFFER_MAXIMUM_SIZE) ? WRITE_BUFFER_MAXIMUM_SIZE : count);
            EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == ((count > WRITE_BUFFER_MAXIMUM_SIZE) ? WRITE_BUFFER_MAXIMUM_SIZE : count)), " Test #6.6: Write buffer to NAND Flash");
            if (result != ((count > WRITE_BUFFER_MAXIMUM_SIZE) ? WRITE_BUFFER_MAXIMUM_SIZE : count))
            {
                break;
            }
            count -= result;
        }
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (count == 0), " Test #6.7: Write file to NAND Flash");
        fclose(fhandle);
        fhandle = NULL;
    }


    /* Close and uninstall MFS on NAND Flash device */
    status = system_test_mfs_close(fshandle, fsname);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (status == MFS_NO_ERROR), " Test #6.8: Close and uninstall MFS on NAND Flash device");
    if (status != MFS_NO_ERROR)
    {
        goto error_handle;
    }

    /* Install and open MFS on NAND Flash device */
    status = system_test_mfs_open(nand_wl_ptr, fsname, &fshandle, TRUE);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (status == MFS_NO_ERROR), " Test #6.9: Install and open MFS on NAND Flash device");
    if (status != MFS_NO_ERROR)
    {
        goto error_handle;
    }

    /* Read file */
    for (i = 0; i < sizeof(write_size)/sizeof(uint32_t); i++)
    {
        sprintf(fpath, "%s\\FILE%d", fsname, i);
        fhandle = fopen(fpath, "r");
        fseek(fhandle, 0, IO_SEEK_CUR);
        count = 0;
        while ((c_read = fgetc(fhandle)) >=0)
        {
            if ((count % WRITE_BUFFER_MAXIMUM_SIZE) == 0)
            {
                c = '0';
                if (c_read != 'M')
                {
                    break;
                }
            }
            else if ((count % WRITE_BUFFER_MAXIMUM_SIZE) == 1)
            {
                if (c_read != 'Q')
                {
                    break;
                }
            }
            else if ((count % WRITE_BUFFER_MAXIMUM_SIZE) == 2)
            {
                if (c_read != 'X')
                {
                    break;
                }
            }
            else if ((count % WRITE_BUFFER_MAXIMUM_SIZE) == 3)
            {
                if (c_read != '_')
                {
                    break;
                }
            }
            else
            {
                if (c_read != c)
                {
                    break;
                }
                c = (c == 'z') ? '0' : c+1;
            }
            count++;
        }
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (count == write_size[i]), " Test #6.10: Read file");
        fclose(fhandle);
        fhandle = NULL;
    }

error_handle:
    /* Close and uninstall MFS on NAND Flash device */
    if (fshandle != NULL)
    {
        status = system_test_mfs_close(fshandle, fsname);
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (status == MFS_NO_ERROR), " Test #6.11: Close and uninstall MFS on NAND Flash device");
    }

    /* De-initialize NAND Flash device */
    system_test_finalize(nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (status == SUCCESS), " Test #6.12: De-initialize NAND Flash device");

    if (write_buffer != NULL)
    {
        _mem_free(write_buffer);
    }
    if (nand_wl_ptr != NULL)
    {
        fclose(nand_wl_ptr);
    }
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_5_main_task
* Comments       : The function is used to test write/del with MFS
* Requirements   : FFS075.
*
*END------------------------------------------------------------------*/
#define STRESS_TEST_LOOP_NUMBER         (10)
#define STRESS_TEST_FILE_SIZE           (10000000)
#define STRESS_TEST_MAXIMUM_BUFFER_SIZE (1024)
void tc_5_main_task(void)
{
    unsigned char *    write_buffer = NULL;
    uint32_t      write_size = 0;
    MQX_FILE_PTR nand_wl_ptr = NULL;
    MQX_FILE_PTR fshandle = NULL;
    MQX_FILE_PTR fhandle = NULL;
    char         fpath[PATHNAME_SIZE] = {0};
    char *     fsname = "a:";
    char *     _argv[3];
    uint32_t      i = 0;
    uint32_t      result;
    uint32_t      status;
    uint32_t      count;
    uint32_t      error;

    nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NANDWL_OPEN_IGNORE_INIT_MEDIA);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (nand_wl_ptr != NULL), " Test #5.1: Open file failed");
    NandHalSetMqxPtr(0, nand_wl_ptr);

    /* ERASE entire nandflash chip */
    result = nandwl_erase_entire_chip(nand_wl_ptr, NANDFLASH_1ST_DATA_DRIVE_START_BLOCK, NANDFLASH_1ST_DATA_DRIVE_START_BLOCK + NANDFLASH_1ST_DATA_DRIVE_SIZE_IN_BLOCK);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, ((bool)result == TRUE), " Test #5.2: Erase entire nand flash chip");

    WL_TEST_LOG("\n ============== tc_5_main_task  ================== \n");
    g_wl_log_level = WL_LOG_ERROR;
    /* g_wl_log_module_attributes= WL_MODULE_NANDWL_TESTING; */
    g_nfc_log_enable = FALSE;

    status =  system_test_initialize(TRUE,nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (status == SUCCESS), " Test #5.3: Initialize Nand_wl drive");

    status = system_test_mfs_open(nand_wl_ptr, fsname, &fshandle, TRUE);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (status == SUCCESS), " Test #5.4: Open MFS on NAND Flash");

    write_buffer = allocate_buffer(STRESS_TEST_MAXIMUM_BUFFER_SIZE, "write");
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (write_buffer != NULL), " Test #5.5: Allocate buffer for write_buffer");
    fill_continous_buff(write_buffer, STRESS_TEST_MAXIMUM_BUFFER_SIZE);

    /* Format NAND Flash storage */
    error = ioctl(fshandle, IO_IOCTL_DEFAULT_FORMAT,  NULL);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (error == MFS_NO_ERROR), " Test #5.6: Format NAND Flash");

    /* Get size of NAND Flash storage == free space after formatting */
    ioctl(fshandle, IO_IOCTL_FREE_SPACE, &write_size);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (write_size != 0), " Test #5.7: Get size of NAND Flash");

    for (i = 0; i < STRESS_TEST_LOOP_NUMBER; i++)
    {
        sprintf(fpath, "%s\\f", fsname);
        fhandle = fopen(fpath, "a");
        fseek(fhandle, 0, IO_SEEK_SET);
        count = write_size * 0.8;
        while (count)
        {
            result = write(fhandle, (unsigned char *)(write_buffer), (count > STRESS_TEST_MAXIMUM_BUFFER_SIZE) ? STRESS_TEST_MAXIMUM_BUFFER_SIZE : count);
            if (result != ((count > STRESS_TEST_MAXIMUM_BUFFER_SIZE) ? STRESS_TEST_MAXIMUM_BUFFER_SIZE : count))
            {
                break;
            }
            count -= result;
        }
        fclose(fhandle);
        fhandle = NULL;
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (!count), " Test #5.8: Write file");
        if (!count)
        {
            result = ioctl(fshandle, IO_IOCTL_DELETE_FILE, (void *)fpath);
            EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_OK), " Test #5.9: Delete file");
            if (result != MQX_OK)
            {
                break;
            }
        }
        else
        {
            break;
        }
    }

    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (i == STRESS_TEST_LOOP_NUMBER), " Test #5.10: Write/Delete loop runs successfully");

    _mem_free(write_buffer);

    status = system_test_mfs_close(fshandle, fsname);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (status == SUCCESS), " Test #5.11: Close MFS on NAND Flash");

    status = system_test_finalize(nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (status == SUCCESS), " Test #5.12: Shutdown & De-init Nand_wl drive");

    result = fclose(nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_OK), " Test #5.13: Close the nand_wl file");
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_6_main_task
* Comments       : The function is used to test read/write with MFS
* Requirements   : FFS075.
*
*END------------------------------------------------------------------*/
void tc_6_main_task(void)
{
    uint32_t      write_size = 0;
    unsigned char *    write_buffer = NULL;
    char *     fsname = "a:";
    char         fpath[PATHNAME_SIZE] = {0};
    MQX_FILE_PTR nand_wl_ptr = NULL;
    MQX_FILE_PTR fshandle = NULL;
    MQX_FILE_PTR fhandle = NULL;
    char         c;
    char         c_read;
    uint32_t      i = 0;
    uint32_t      count;
    uint32_t      error = 0;
    uint32_t      free_space;
    uint32_t      result;
    RtStatus_t   status;

    /* Open NAND Flash device */
    nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NANDWL_OPEN_IGNORE_INIT_MEDIA);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (nand_wl_ptr != NULL), "Test #6.1: Open NAND Flash device");
    NandHalSetMqxPtr(0, nand_wl_ptr);

    /* Erase NAND Flash blocks defined */
    result = nandwl_erase_entire_chip(nand_wl_ptr, NANDFLASH_1ST_DATA_DRIVE_START_BLOCK, NANDFLASH_1ST_DATA_DRIVE_START_BLOCK + NANDFLASH_1ST_DATA_DRIVE_SIZE_IN_BLOCK);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, ((bool)result == TRUE), " Test #6.2: Erase NAND Flash blocks defined");

    /* Allocate write buffer */
    write_buffer = (unsigned char *)_mem_alloc_zero(WRITE_BUFFER_MAXIMUM_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (write_buffer != NULL), " Test #6.3: Allocate a buffer to write on NAND Flash");
    if (write_buffer == NULL)
    {
        goto error_handle;
    }

    /* Fill data to write buffer */
    c = '0';
    write_buffer[0] = 'M';
    write_buffer[1] = 'Q';
    write_buffer[2] = 'X';
    write_buffer[3] = '_';
    for (i = 4; i < WRITE_BUFFER_MAXIMUM_SIZE; i++)
    {
        write_buffer[i] = c;
        c = (c == 'z') ? '0' : c+1;
    }

    g_wl_log_level = WL_LOG_ERROR;
    /* g_wl_log_module_attributes= WL_MODULE_NANDWL_TESTING; */
    g_nfc_log_enable = FALSE;

    /* Initialize NAND Flash device */
    status = system_test_initialize(TRUE, nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (status == SUCCESS), " Test #6.4: Initialize NAND Flash device");
    if (status != MQX_OK)
    {
        goto error_handle;
    }

    /* Install and open MFS on NAND Flash device */
    status = system_test_mfs_open(nand_wl_ptr, fsname, &fshandle, TRUE);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (status == MFS_NO_ERROR), " Test #6.5: Install and open MFS on NAND Flash device");
    if (status != MFS_NO_ERROR)
    {
        goto error_handle;
    }

    /* Get size of NAND Flash storage == free space after formatting */
    ioctl(fshandle, IO_IOCTL_FREE_SPACE, &write_size);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (write_size != 0), " Test #6.6: Get size of NAND Flash");

    for (i = 0; i < STRESS_TEST_LOOP_NUMBER; i++)
    {
        /* Format NAND Flash storage */
        error = ioctl(fshandle, IO_IOCTL_DEFAULT_FORMAT,  NULL);
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (error == MFS_NO_ERROR), " Test #6.7: Format NAND Flash");

        /* Write file with size is size of NAND Flash storage */
        count = write_size;
        sprintf(fpath, "%s\\FILE_FULL", fsname);
        fhandle = fopen(fpath, "a");
        fseek(fhandle, 0, IO_SEEK_CUR);
        while (count)
        {
            result = write(fhandle, (unsigned char *)(write_buffer), (count > WRITE_BUFFER_MAXIMUM_SIZE) ? WRITE_BUFFER_MAXIMUM_SIZE : count);
            //EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == ((count > WRITE_BUFFER_MAXIMUM_SIZE) ? WRITE_BUFFER_MAXIMUM_SIZE : count)), " Test #6.6: Write buffer to NAND Flash");
            if (result != ((count > WRITE_BUFFER_MAXIMUM_SIZE) ? WRITE_BUFFER_MAXIMUM_SIZE : count))
            {
                break;
            }
            count -= result;
        }
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (count == 0), " Test #6.8: Write a file to fill NAND Flash fully");
        fclose(fhandle);
        fhandle = NULL;

        /* Write file with size is 0 byte */
        sprintf(fpath, "%s\\FILE_0", fsname);
        fhandle = fopen(fpath, "a");
        fseek(fhandle, 0, IO_SEEK_CUR);
        result = write(fhandle, (unsigned char *)(write_buffer), 0);
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == 0), " Test #6.9: Write a file with size as 0 byte to NAND Flash");
        fclose(fhandle);
        fhandle = NULL;

        /* Write file with size is 1 byte */
        sprintf(fpath, "%s\\FILE_1", fsname);
        fhandle = fopen(fpath, "a");
        fseek(fhandle, 0, IO_SEEK_CUR);
        result = write(fhandle, (unsigned char *)(write_buffer), 1);
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result != 1), " Test #6.10: Write a file with size as 1 byte to NAND Flash");
        error = ferror(fhandle);
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (error == MFS_DISK_FULL), " Test #6.11: NAND Flash is full");
        fclose(fhandle);
        fhandle = NULL;

        /* ioctl(fshandle, IO_IOCTL_FREE_SPACE, &free_space);
           EU_ASSERT_REF_TC_MSG(tc_6_main_task, (free_space == 0), " Test #6.12: Get free space of NAND Flash"); */

        /* Format NAND Flash storage */
        error = ioctl(fshandle, IO_IOCTL_DEFAULT_FORMAT,  NULL);
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (error == MFS_NO_ERROR), " Test #6.13: Format NAND Flash");

        /* Write file with size is size of NAND Flash storage + 1 */
        count = write_size + 1;
        sprintf(fpath, "%s\\FILE_OVER", fsname);
        fhandle = fopen(fpath, "a");
        fseek(fhandle, 0, IO_SEEK_CUR);
        while (count)
        {
            result = write(fhandle, (unsigned char *)(write_buffer), (count > WRITE_BUFFER_MAXIMUM_SIZE) ? WRITE_BUFFER_MAXIMUM_SIZE : count);
            if (result != ((count > WRITE_BUFFER_MAXIMUM_SIZE) ? WRITE_BUFFER_MAXIMUM_SIZE : count))
            {
                break;
            }
            count -= result;
        }
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (count == 1), " Test #6.14: Write a file to fill NAND Flash fully");
        error = ferror(fhandle);
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (error == MFS_DISK_FULL), " Test #6.15: NAND Flash is full");
        fclose(fhandle);
        fhandle = NULL;

        /* Delete file */
        result = ioctl(fshandle, IO_IOCTL_DELETE_FILE, (void *)fpath);
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == MQX_OK), " Test #6.16: Delete file");

        /* Write file with size is size of NAND Flash storage */
        count = write_size;
        sprintf(fpath, "%s\\FILE_FULL", fsname);
        fhandle = fopen(fpath, "a");
        fseek(fhandle, 0, IO_SEEK_CUR);
        while (count)
        {
            result = write(fhandle, (unsigned char *)(write_buffer), (count > WRITE_BUFFER_MAXIMUM_SIZE) ? WRITE_BUFFER_MAXIMUM_SIZE : count);
            if (result != ((count > WRITE_BUFFER_MAXIMUM_SIZE) ? WRITE_BUFFER_MAXIMUM_SIZE : count))
            {
                break;
            }
            count -= result;
        }
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (count == 0), " Test #6.17: Write a file to fill NAND Flash fully");
        fclose(fhandle);
        fhandle = NULL;
    }

error_handle:
    /* Close and uninstall MFS on NAND Flash device */
    if (fshandle != NULL)
    {
        status = system_test_mfs_close(fshandle, fsname);
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (status == MFS_NO_ERROR), " Test #6.18: Close and uninstall MFS on NAND Flash device");
    }

    /* De-initialize NAND Flash device */
    system_test_finalize(nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (status == SUCCESS), " Test #6.19: De-initialize NAND Flash device");

    if (write_buffer != NULL)
    {
        _mem_free(write_buffer);
    }
    if (nand_wl_ptr != NULL)
    {
        fclose(nand_wl_ptr);
    }
}

/*******************************************************************************************************************/

/*TASK*-----------------------------------------------------------------
*
* Function Name  : Main_task
* Comments       : The task is used to test the read/write function
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
