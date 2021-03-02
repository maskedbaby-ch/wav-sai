
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
* $FileName: test.cpp$
* $Version : 3.8.12.0$
* $Date    : Jul-20-2012$
*
* Comments:
*
*   This file contains the source functions for the testing of the
*   nfc driver
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
#include "test.h"

//------------------------------------------------------------------------
// Test suite function prototype
void tc_1_main_task(void);
void tc_2_main_task(void);
void tc_3_main_task(void);
void tc_4_main_task(void);
void tc_5_main_task(void);
void main_task(uint32_t initial_data);
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_nand_nfc)
    EU_TEST_CASE_ADD(tc_1_main_task, " Test #1 - Test writing/reading a random physical page"),
    EU_TEST_CASE_ADD(tc_2_main_task, " Test #2 - Test writing/reading on multiple random physical pages"),
    EU_TEST_CASE_ADD(tc_3_main_task, " Test #3 - Test writing/reading metadata on a random virtual page"),
    EU_TEST_CASE_ADD(tc_4_main_task, " Test #4 - Test writing/reading metadata on multiple random virtual pages"),
    EU_TEST_CASE_ADD(tc_5_main_task, " Test #5 - Test virtual page order and factory making bytes location"),
EU_TEST_SUITE_END(suite_nand_nfc)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_nand_nfc, " - Testing Nand NFC")
EU_TEST_REGISTRY_END()

TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
/*  Task number, Entry point, Stack, Pri, String, Auto? */
   {MAIN_TASK,   main_task,   5000,  9,   "main", MQX_AUTO_START_TASK},
   {0,           0,           0,     0,   0,      0,                 }
};


//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_1_main_task
// Comments     : Test writing/reading a random physical page
//
//END---------------------------------------------------------------------

void tc_1_main_task(void) {
    MQX_FILE_PTR         nand_wl_ptr;
    uint32_t              buf_size;
    uint32_t              result;
    unsigned char *       write_buffer;
    unsigned char *       read_buffer;
    uint32_t              secnum = 25600;
    uint64_t              numberSector = 0;
    uint32_t              rand = simple_rand();
    RtStatus_t           error;

    /* Step 1: Open NANDFLASH WL*/
    nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NANDWL_OPEN_IGNORE_INIT_MEDIA);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (nand_wl_ptr != NULL), " Test #1.1: Open file failed");
    NandHalSetMqxPtr(0, nand_wl_ptr);

    /* ERASE entire nandflash chip */
    result = nandwl_erase_entire_chip(nand_wl_ptr, NANDFLASH_1ST_DATA_DRIVE_START_BLOCK, NANDFLASH_1ST_DATA_DRIVE_START_BLOCK + NANDFLASH_1ST_DATA_DRIVE_SIZE_IN_BLOCK);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, ((bool)result == TRUE), " Test #1.2: Erase entire chip");

    result = nandwl_testing_get_info(nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == IO_OK), " Test #1.2: Get information of Nand_wl ");
    
    buf_size = nandwl_phy_page_size + nandwl_phy_page_spare_size;
    
    write_buffer = allocate_buffer(buf_size, "write");
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (write_buffer != NULL), " Test #1.3: Allocate write_buffer buffer ");

    fill_continous_buff(write_buffer, buf_size);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (TRUE), " Test #1.4: Fill data to write_buffer buffer ");

    read_buffer = allocate_buffer(buf_size, "read");
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (read_buffer != NULL), " Test #1.5: Allocate read_buffer buffer ");
    zero_buff(read_buffer, buf_size );
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (TRUE), " Test #1.6: Set all data of read_buffer to zero");

    error = system_test_initialize(TRUE,nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error == SUCCESS), " Test #1.7: Nand_wl's initialization with repair if init failed ");

    error = DriveGetInfo(DRIVE_TAG_DATA, kDriveInfoSizeInSectors, &numberSector);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error == SUCCESS), " Test #1.8: Get information of Nand_wl Logical Drive  ");
    /* secnum = rand % numberSector; */

    WL_TEST_LOG("Write data on lsector %d \n", secnum);
    fseek(nand_wl_ptr, secnum, IO_SEEK_SET);

    result = ioctl(nand_wl_ptr, NANDFLASH_IOCTL_WRITE_RAW, write_buffer);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == IO_OK), " Test #1.9: Write data to Nand flash ");

    WL_TEST_LOG("Read data on lsector %d \n", secnum);
    fseek(nand_wl_ptr, secnum, IO_SEEK_SET);
    result = ioctl(nand_wl_ptr, NANDFLASH_IOCTL_READ_RAW, read_buffer) ;
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == IO_OK), " Test #1.10: Read data from Nand flash ");

    result = compare_test(write_buffer,read_buffer, nandwl_virt_page_size);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, ((bool)result == TRUE), " Test #1.11: Compare data between write_buffer & read_buffer ");

    _mem_free(write_buffer);
    _mem_free(read_buffer);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (TRUE), " Test #1.12: Free all allocated buffer ");

    error = system_test_finalize(nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error == SUCCESS), " Test #1.13: Shutdown & de-init the media buffer & data drive ");
    
    result = fclose(nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), " Test #1.14: Close nand_wl object file ");
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_2_main_task
// Comments     : Test writing/reading on multiple random physical pages
//
//END---------------------------------------------------------------------

void tc_2_main_task(void) {
    MQX_FILE_PTR      nand_wl_ptr;
    uint32_t           buf_size = nandwl_phy_page_size + nandwl_phy_page_spare_size;
    unsigned char *       write_buffer;
    unsigned char *       read_buffer;
    uint32_t           secnum = 25856;
    uint64_t           numberSector = 0;
    uint32_t           i = 0, j = 0, result ;
    uint32_t           num_write_on_same_sector = 100;
    uint32_t           rand;
    RtStatus_t        error;

    /* Step 1: Open NANDFLASH WL*/
    nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NANDWL_OPEN_IGNORE_INIT_MEDIA);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (nand_wl_ptr != NULL), " Test #2.1: Open file failed");
    NandHalSetMqxPtr(0, nand_wl_ptr);

    /* ERASE entire nandflash chip */
    result = nandwl_erase_entire_chip(nand_wl_ptr, NANDFLASH_1ST_DATA_DRIVE_START_BLOCK, NANDFLASH_1ST_DATA_DRIVE_START_BLOCK + NANDFLASH_1ST_DATA_DRIVE_SIZE_IN_BLOCK);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, ((bool)result == TRUE), " Test #2.2: Erase entire chip");

    write_buffer = allocate_buffer(buf_size, "write");
    fill_continous_buff(write_buffer, buf_size);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (write_buffer != NULL), " Test #2.3: Allocate new buffer (write_buffer)");

    read_buffer = allocate_buffer(buf_size, "read");
    zero_buff(read_buffer, buf_size );
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (read_buffer != NULL), " Test #2.4: Allocate new buffer (read_buffer)");

    error = system_test_initialize(TRUE,nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error == SUCCESS), " Test #2.5: Initialize Nand_wl driver");

    error = DriveGetInfo(DRIVE_TAG_DATA, kDriveInfoSizeInSectors, &numberSector);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error == SUCCESS), "Test #2.6: Get information of Logical drive");

    while (i < num_write_on_same_sector) {
        /* rand = simple_rand();         */
        /* secnum = rand % numberSector; */

        WL_TEST_LOG("Write data on lsector %d \n", secnum);
        fseek(nand_wl_ptr, secnum, IO_SEEK_SET);

        result = ioctl(nand_wl_ptr, NANDFLASH_IOCTL_WRITE_RAW, write_buffer);
        EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == IO_OK), " Test #2.7: Write data (write_buffer) to nand flash ");

        WL_TEST_LOG("Read data on lsector %d \n", secnum);
        fseek(nand_wl_ptr, secnum, IO_SEEK_SET);
        result = ioctl(nand_wl_ptr, NANDFLASH_IOCTL_READ_RAW, read_buffer);
        EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == IO_OK), "Test #2.8: Read data (read_buffer) to nand flash ");

        result = compare_test(write_buffer, read_buffer, nandwl_virt_page_size);
        EU_ASSERT_REF_TC_MSG(tc_2_main_task, ((bool)result == TRUE), " Test #2.9: Compare data between read_buffer & write_buffer ");

        i++;
    }
    _mem_free(write_buffer);
    _mem_free(read_buffer);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (TRUE), " Test #2.10: Free all allocated buffer ");
    
    error = system_test_finalize(nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error == SUCCESS), "Test #2.11: Shutdown & de-init the media buffer & data drive");
    
    result = fclose(nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #2.12: Free all allocated buffer ");
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_3_main_task
// Comments     : Test writing/reading metadata on a random virtual page
//
//END---------------------------------------------------------------------

void tc_3_main_task(void) {
    MQX_FILE_PTR                 nand_wl_ptr;
    NANDFLASH_PAGE_BUFF_STRUCT   writePage, readPage;
    unsigned char                    *pWriteData, *pWriteAux, *pReadData, *pReadAux, *metaBuffer;
    uint32_t                      buf_size;
    uint32_t                      secnum = 26112;
    uint32_t                      result;
    RtStatus_t                   error;

    /* Step 1: Open NANDFLASH WL*/
    nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NANDWL_OPEN_IGNORE_INIT_MEDIA);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (nand_wl_ptr != NULL), " Test #3.1: Open file failed");
    NandHalSetMqxPtr(0, nand_wl_ptr);
    
    nandwl_testing_get_info(nand_wl_ptr);
    buf_size = nandwl_virt_page_size + nandwl_page_metadata_size;
    
    /* ERASE entire nandflash chip */
    result = nandwl_erase_entire_chip(nand_wl_ptr, NANDFLASH_1ST_DATA_DRIVE_START_BLOCK, NANDFLASH_1ST_DATA_DRIVE_START_BLOCK + NANDFLASH_1ST_DATA_DRIVE_SIZE_IN_BLOCK);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, ((bool)result == TRUE), " Test #3.2: Erase entire chip");

    error =  system_test_initialize(TRUE,nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error == SUCCESS), " Test #3.3: Initialize Nand_wl logical drive ");

    pWriteData = allocate_buffer(nandwl_virt_page_size, "pWriteData");
    fill_continous_buff(pWriteData, nandwl_virt_page_size);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (pWriteData != NULL ), " Test #3.4: Allocate new buffer (pWriteData buffer) ");

    pWriteAux = allocate_buffer(nandwl_page_metadata_size, "pWriteAux");
    fill_continous_buff(pWriteAux, nandwl_page_metadata_size);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (pWriteAux != NULL ), " Test #3.5: Allocate new buffer (pWriteAux buffer) ");

    writePage.data = (unsigned char *)pWriteData;
    writePage.metadata = (unsigned char *)pWriteAux;

    WL_TEST_LOG("Write data on sector %d \n", secnum);
    fseek(nand_wl_ptr, secnum, IO_SEEK_SET);
    result = ioctl(nand_wl_ptr, NANDFLASH_IOCTL_WRITE_WITH_METADATA, (void *)&writePage);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_OK ), " Test #3.6: Write metadata to Nand flash ");

    pReadData = allocate_buffer(nandwl_virt_page_size, "pReadData");
    zero_buff(pReadData, nandwl_virt_page_size);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (pReadData != NULL ), " Test #3.7: Allocate new buffer (pReadData buffer) ");

    pReadAux = allocate_buffer(nandwl_page_metadata_size, "pReadAux");
    zero_buff(pReadAux, nandwl_page_metadata_size);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (pReadAux != NULL ), " Test #3.8: Allocate new buffer (pReadAux buffer) ");

    readPage.data = (unsigned char *)pReadData;
    readPage.metadata = (unsigned char *)pReadAux;

    WL_TEST_LOG("Read data on sector %d \n", secnum);
    fseek(nand_wl_ptr, secnum, IO_SEEK_SET);
    result = ioctl(nand_wl_ptr, NANDFLASH_IOCTL_READ_WITH_METADATA, (void *)&readPage);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_OK ), " Test #3.9: Read metadata from Nand flash ");

    result = compare_test(pWriteData, pReadData, nandwl_virt_page_size);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, ((bool)result == TRUE), " Test #3.10: Compare data between pWriteData & pReadData ");

    result = compare_test(pWriteAux, pReadAux, nandwl_page_metadata_size);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, ((bool)result == TRUE), " Test #3.11: Compare data between pWriteAux & pReadAux ");

    metaBuffer = allocate_buffer(nandwl_page_metadata_size, "metaBuffer");
    zero_buff(metaBuffer, nandwl_page_metadata_size );
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (metaBuffer != NULL), " Test #3.12: Allocate new buffer (metaBuffer) ");

    WL_TEST_LOG("Read metadata on sector %d \n", secnum);
    fseek(nand_wl_ptr, secnum, IO_SEEK_SET);
    result = ioctl(nand_wl_ptr, NANDFLASH_IOCTL_READ_METADATA, metaBuffer);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == IO_OK  ), " Test #3.13: Read metadata from Nand flash ");

    result = compare_test(metaBuffer, pReadAux, nandwl_page_metadata_size);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, ((bool)result == TRUE), " Test #3.14: Compare data between metaBuffer & pReadAux");

    _mem_free(pWriteData);
    _mem_free(pWriteAux);
    _mem_free(pReadData);
    _mem_free(pReadAux);
    _mem_free(metaBuffer);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (TRUE), " Test #3.15: Free all allocated buffer");

    error = system_test_finalize(nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error == SUCCESS), " Test #3.16: De-init Nand_wl drive");
    
    result = fclose(nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_OK), " Test #3.17: Close the nand_wl file");

}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_4_main_task
// Comments     : Test writing/reading metadata on multiple random virtual pages
//
//END---------------------------------------------------------------------

void tc_4_main_task(void) {
    MQX_FILE_PTR                nand_wl_ptr;
    NANDFLASH_PAGE_BUFF_STRUCT  writePage, readPage;
    unsigned char                   *pWriteData, *pWriteAux, *pReadData, *pReadAux, *metaBuffer;
    uint32_t                     buf_size;
    uint32_t                     secnum = 26368;
    uint32_t                     rand;
    uint64_t                     numberSector;
    uint32_t                     i = 0, result;
    uint32_t                     loopCounter = 100;
    RtStatus_t                  error;

    /* Step 1: Open NANDFLASH WL*/
    nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NANDWL_OPEN_IGNORE_INIT_MEDIA);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (nand_wl_ptr != NULL), " Test #4.1: Open file failed");
    NandHalSetMqxPtr(0, nand_wl_ptr);

    nandwl_testing_get_info(nand_wl_ptr);   
    buf_size = nandwl_virt_page_size + nandwl_page_metadata_size;
    
    /* ERASE entire nandflash chip */
    result = nandwl_erase_entire_chip(nand_wl_ptr, NANDFLASH_1ST_DATA_DRIVE_START_BLOCK, NANDFLASH_1ST_DATA_DRIVE_START_BLOCK + NANDFLASH_1ST_DATA_DRIVE_SIZE_IN_BLOCK);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, ((bool)result == TRUE), " Test #4.2: Erase entire chip");

    error =  system_test_initialize(TRUE,nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (error == SUCCESS), " Test #4.3: Initialize Nand_wl logical drive ");

    pWriteData = allocate_buffer(nandwl_virt_page_size, "pWriteData");
    fill_continous_buff(pWriteData, nandwl_virt_page_size);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (pWriteData != NULL), " Test #4.4: Allocate pWriteData buffer ");

    pWriteAux = allocate_buffer(nandwl_page_metadata_size, "pWriteAux");
    fill_continous_buff(pWriteAux, nandwl_page_metadata_size);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (pWriteAux != NULL), " Test #4.5: Allocate pWriteAux buffer ");

    writePage.data = (unsigned char *)pWriteData;
    writePage.metadata = (unsigned char *)pWriteAux;

    pReadData = allocate_buffer(nandwl_virt_page_size, "pReadData");
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (pReadData != NULL), " Test #4.6: Allocate pReadData buffer ");

    pReadAux = allocate_buffer(nandwl_page_metadata_size, "pReadAux");
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (pReadData != NULL), " Test #4.7: Allocate pReadAux buffer ");

    metaBuffer = allocate_buffer(nandwl_page_metadata_size, "metaBuffer");
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (metaBuffer != NULL), " Test #4.7: Allocate metaBuffer buffer ");

    DriveGetInfo(DRIVE_TAG_DATA, kDriveInfoSizeInSectors, &numberSector);
    while (i < loopCounter) {
        /* rand = simple_rand();         */
        /* secnum = rand % numberSector; */

        WL_TEST_LOG("Write data on sector %d \n", secnum);
        fseek(nand_wl_ptr, secnum, IO_SEEK_SET);
        result = ioctl(nand_wl_ptr, NANDFLASH_IOCTL_WRITE_WITH_METADATA, (void *)&writePage);
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == IO_OK), " Test #4.8: Write metadata to Nand flash ");

        zero_buff(pReadData, nandwl_virt_page_size);
        zero_buff(pReadAux, nandwl_page_metadata_size);

        readPage.data = (unsigned char *)pReadData;
        readPage.metadata = (unsigned char *)pReadAux;

        WL_TEST_LOG("Read data on sector %d \n", secnum);
        fseek(nand_wl_ptr, secnum, IO_SEEK_SET);
        result = ioctl(nand_wl_ptr, NANDFLASH_IOCTL_READ_WITH_METADATA, (void *)&readPage);
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == IO_OK), " Test #4.9: Read metadata from Nand flash ");

        result = compare_test(pWriteData, pReadData, nandwl_virt_page_size);
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, ((bool)result == TRUE), " Test #4.10: Compare data between pWriteData & pReadData buffers ");

        result = compare_test(pWriteAux, pReadAux, nandwl_page_metadata_size);
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, ((bool)result == TRUE), " Test #4.11: Compare data between pWriteAux & pReadAux buffers ");

        zero_buff(metaBuffer, nandwl_page_metadata_size );

        WL_TEST_LOG("Read metadata on sector %d \n", secnum);
        fseek(nand_wl_ptr, secnum, IO_SEEK_SET);
        result = ioctl(nand_wl_ptr, NANDFLASH_IOCTL_READ_METADATA, metaBuffer);
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == IO_OK), " Test #4.12: Read metadata from NandFlash to metaBuffer ");

        result = compare_test(metaBuffer, pReadAux, nandwl_page_metadata_size);
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, ((bool)result == TRUE), " Test #4.13: Compare data between metaBuffer & pReadAux buffers ");

        i++;
    }

    _mem_free(pWriteData);
    _mem_free(pWriteAux);
    _mem_free(pReadData);
    _mem_free(pReadAux);
    _mem_free(metaBuffer);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (TRUE), " Test #4.14: Free all allocated buffers ");

    error = system_test_finalize(nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (error == SUCCESS), " Test #4.15: De-init & shutdown all object related Nand_wl drive ");

    result = fclose(nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_OK), " Test #4.16: Close the nand_wl file ");
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_5_main_task
// Comments     : Test virtual page order and factory making bytes location
//
//END---------------------------------------------------------------------

void tc_5_main_task(void) {
    MQX_FILE_PTR                nand_wl_ptr;
    NANDFLASH_PAGE_BUFF_STRUCT  writePage, readPage;
    unsigned char                   *pWriteData, *pWriteAux, *pageBuff, *tempBuffer;
#if defined(BSP_TWRMPC5125)
    unsigned char *                   buildInBuffer[64];
#else
    unsigned char *                   buildInBuffer[256];
#endif
    unsigned char *                   tmp1, tmp2;
    uint32_t                     i, j, secnum, virtualCount, pagenum, result;
    RtStatus_t                  error;
    

    /* Step 1: Open NANDFLASH WL*/
    nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NANDWL_OPEN_IGNORE_INIT_MEDIA);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (nand_wl_ptr != NULL), " Test #5.1: Open file failed");
    NandHalSetMqxPtr(0, nand_wl_ptr);

    nandwl_testing_get_info(nand_wl_ptr);

    /* ERASE entire nandflash chip */
    result = nandwl_erase_entire_chip(nand_wl_ptr, NANDFLASH_1ST_DATA_DRIVE_START_BLOCK, NANDFLASH_1ST_DATA_DRIVE_START_BLOCK + NANDFLASH_1ST_DATA_DRIVE_SIZE_IN_BLOCK);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, ((bool)result == TRUE), " Test #5.2: Erase entire chip");

    /* error =  system_test_initialize(TRUE,nand_wl_ptr);
       EU_ASSERT_REF_TC_MSG(tc_5_main_task, (error == SUCCESS), " Test #5.3: Initialize Nand_wl drive"); */

    pWriteData = allocate_buffer(nandwl_virt_page_size, "pWriteData");   // 54418, K70 = 512  | 5125 = 2048
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (pWriteData != NULL), " Test #5.5: Allocate pWriteData buffer");

    pWriteAux = allocate_buffer(nandwl_page_metadata_size, "pWriteAux"); // 54418, K70 = 8    | 5125 = 56
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (pWriteAux != NULL), " Test #5.5: Allocate pWriteAux buffer");

    writePage.data = (unsigned char *)pWriteData;
    writePage.metadata = (unsigned char *)pWriteAux;

#if defined(BSP_TWRMPC5125)
    for (i = 0; i < nandwl_block_size / nandwl_phy_page_size / 4; i++) {
#else
    /* for (i = 0; i < nandwl_block_size / nandwl_phy_page_size; i++) { */
    for (i = 0; i < 10; i++) {
#endif
        buildInBuffer[i] = allocate_buffer(nandwl_phy_page_size + nandwl_phy_page_spare_size, "buildInBuffer");  // 2048 + 64
        if (buildInBuffer[i] == NULL){
            for (j = 0; j < i; j++){
                _mem_free(buildInBuffer[j]);
            }
            EU_ASSERT_REF_TC_MSG(tc_5_main_task, FALSE, " Test #5.6: Allocate buildInBuffer buffers");
            return;
        }
        zero_buff(buildInBuffer[i], nandwl_phy_page_size + nandwl_phy_page_spare_size);
    }

    secnum = 12800;
    i = 0;

#if defined(BSP_TWRMPC5125)
    virtualCount = nandwl_num_virt_pages_per_block / 4;
    while (i < virtualCount) {
        fill_continous_buff(pWriteData, nandwl_virt_page_size);
        fill_continous_buff(pWriteAux, nandwl_page_metadata_size);

        WL_TEST_LOG("Write data on sector %d \n", secnum);
        fseek(nand_wl_ptr, secnum, IO_SEEK_SET);
        result = ioctl(nand_wl_ptr, NANDFLASH_IOCTL_WRITE_WITH_METADATA, (void *)&writePage);
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == IO_OK), " Test #5.7 (For BSP_TWRMPC5125): Write data from writePage buffer to Nand flash");

        tempBuffer = allocate_buffer(nandwl_virt_page_size + nandwl_page_metadata_size, "tempBuffer");  // 512 + 8
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (tempBuffer != NULL), " Test #5.8 (For BSP_TWRMPC5125) : Allocate tempBuffer buffer");

        _mem_copy(pWriteData, tempBuffer, nandwl_virt_page_size);
        _mem_copy(pWriteAux, tempBuffer + nandwl_virt_page_size, nandwl_page_metadata_size);
        _mem_copy(tempBuffer, buildInBuffer[i], nandwl_virt_page_size + nandwl_page_metadata_size);
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (TRUE), " Test #5.9 (For BSP_TWRMPC5125): Copy data from pWriteData & pWriteAux to buildInBuffer array ");

        _mem_free(tempBuffer);

        secnum++;
        i++;
    }
#else
    /* virtualCount = nandwl_num_virt_pages_per_block; // 256
       while (i < virtualCount) {                      */
    while (i < 40) {
        fill_continous_buff(pWriteData, nandwl_virt_page_size);
        fill_continous_buff(pWriteAux, nandwl_page_metadata_size);

        WL_TEST_LOG("Write data on sector %d \n", secnum);
        fseek(nand_wl_ptr, secnum, IO_SEEK_SET);
        result = ioctl(nand_wl_ptr, NANDFLASH_IOCTL_WRITE_WITH_METADATA, (void *)&writePage);
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == IO_OK), " Test #5.7 (For BSP_TWRK70): Copy data from pWriteData to Nand Flash ");

        tempBuffer = allocate_buffer(nandwl_virt_page_size + nandwl_page_metadata_size, "tempBuffer");  // 512 + 8
        _mem_copy(pWriteData, tempBuffer, nandwl_virt_page_size);
        _mem_copy(pWriteAux, tempBuffer + nandwl_virt_page_size, nandwl_page_metadata_size);

        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (TRUE), " Test #5.8 (For BSP_TWRK70): Copy data from pWriteData & pWriteAux to temporary buffer (tempBuffer) ");

        if (i % 4 == 0) {
            /* if (i == 0 || i == 4) {                                            */              
            /*     tmp1 = allocate_buffer(nandwl_page_metadata_size, "tmp1");     */
            /*     tmp2 = allocate_buffer(nandwl_page_metadata_size, "tmp2");     */
            /*     _mem_copy(tempBuffer + 464, tmp1, nandwl_page_metadata_size);  */
            /*     _mem_copy(tempBuffer + 512, tmp2, nandwl_page_metadata_size);  */
            /*     _mem_copy(tmp2, tempBuffer + 464, nandwl_page_metadata_size);  */
            /*     _mem_copy(tmp1, tempBuffer + 512, nandwl_page_metadata_size);  */
            /*     _mem_free(tmp1);                                               */
            /*     _mem_free(tmp2);                                               */
            /* }                                                                  */                                  

            _mem_copy(tempBuffer, buildInBuffer[i / 4] + 3 * (nandwl_virt_page_size + nandwl_page_metadata_size + 8), nandwl_virt_page_size + nandwl_page_metadata_size);
        } else if (i % 4 == 1) {
            _mem_copy(tempBuffer, buildInBuffer[i / 4] + 2 * (nandwl_virt_page_size + nandwl_page_metadata_size + 8), nandwl_virt_page_size + nandwl_page_metadata_size);
        } else if (i % 4 == 2) {
            _mem_copy(tempBuffer, buildInBuffer[i / 4] + 1 * (nandwl_virt_page_size + nandwl_page_metadata_size + 8), nandwl_virt_page_size + nandwl_page_metadata_size);
        } else {
            _mem_copy(tempBuffer, buildInBuffer[i / 4] + 0 * (nandwl_virt_page_size + nandwl_page_metadata_size + 8), nandwl_virt_page_size + nandwl_page_metadata_size);
        }

        _mem_free(tempBuffer);
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (TRUE), " Test #5.9 (For BSP_TWRK70): Copy data from temporary buffer (tempBuffer) to buildInBuffer arrary ");

        secnum++;
        i++;
    }
#endif

    secnum = 12800;
    i = 0;
    pagenum = secnum / (nandwl_phy_page_size / nandwl_virt_page_size);
    pageBuff = allocate_buffer(nandwl_phy_page_size + nandwl_phy_page_spare_size, "pageBuff");
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (pageBuff != NULL), " Test #5.10: Allocate pageBuff buffer ");

    zero_buff(pageBuff, nandwl_phy_page_size + nandwl_phy_page_spare_size);

#if defined(BSP_TWRMPC5125)
    while (i < (nandwl_block_size / nandwl_phy_page_size / 4)) {
        WL_TEST_LOG("Read raw data on page %d \n", pagenum);
        fseek(nand_wl_ptr, pagenum, IO_SEEK_SET);
        result = ioctl(nand_wl_ptr, NANDFLASH_IOCTL_READ_RAW, pageBuff);
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_OK), " Test #5.11 (For BSP_TWRMPC5125): Read raw data from Nand flash to pageBuff buffer ");


        for (j = 0; j < nandwl_phy_page_size + nandwl_phy_page_spare_size; j++) {
            if (j < 2104) {
                if ((buildInBuffer[i])[j] != pageBuff[j]) {
                    result = ~(MQX_OK);
                }
            }
        }
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_OK), " Test #5.12 (For BSP_TWRMPC5125): Compare data between pageBuff (read from Nand flash) and buildInBuffer (data write to flash)  ");

        pagenum++;
        i++;
    }
#else
    /* while (i < (nandwl_block_size / nandwl_phy_page_size)) { */
    while (i < 10) {
        WL_TEST_LOG("Read raw data on page %d \n", pagenum);
        fseek(nand_wl_ptr, pagenum, IO_SEEK_SET);
        result = ioctl(nand_wl_ptr, NANDFLASH_IOCTL_READ_RAW, pageBuff);
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_OK), " Test #5.11 (For BSP_TWRK70): Read raw data from Nand flash to pageBuff buffer ");

      for (j = 0; j < nandwl_phy_page_size + nandwl_phy_page_spare_size; j++) {
        if ((j < 520) || (j >= 528 && j < 1048) || (j >= 1056 && j < 1576) || (j >= 1584 && j < 2104)) {
            if ((buildInBuffer[i])[j] != pageBuff[j]) {
                result = ~(MQX_OK);
                dump_buff(buildInBuffer[i], nandwl_phy_page_size + nandwl_phy_page_spare_size, " buildInBuffer ");
                dump_buff(pageBuff, nandwl_phy_page_size + nandwl_phy_page_spare_size, " pageBuff ");
                break;
            }
        }
      }
      EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_OK), " Test #5.11 (For BSP_TWRK70): Compare data between pageBuff (read from Nand flash) and buildInBuffer (data write to flash)  ");

      pagenum++;
      i++;
    }
#endif

    _mem_free(pWriteData);
    _mem_free(pWriteAux);
    _mem_free(buildInBuffer);
    _mem_free(pageBuff);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (TRUE), " Test #5.14: Free all allocated buffer ");

    /* error = system_test_finalize(nand_wl_ptr);
       EU_ASSERT_REF_TC_MSG(tc_5_main_task, (error == SUCCESS), " Test #5.15: De-init & shutdown all object which reletated Nand_wl drive "); */
    fclose(nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_OK), " Test #5.16: Close nand_wl_ptr file ");

}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : main_task
* Comments       : This task test the nfc component.
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
