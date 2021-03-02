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
*   This file is used to test NFCNandMedia functions; including:
*                      WritePage
*                      ReadPage
*                      ReadPageMetaData
*   and infinite loop
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
#include "ddi_metadata.h"
#include "nonsequential_sectors_map.h"
#include "ddi_nand_hal_nfcphymedia.h"
#include "ddi_nand_hal.h"
#include "test.h"

using namespace nand;

void tc_1_main_task(void);
void tc_2_main_task(void);
void tc_3_main_task(void);
void tc_4_main_task(void);

//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_nfc_media)
EU_TEST_CASE_ADD(tc_1_main_task, " Test #1 - Test Write/Read/ReadMetaData function"),
EU_TEST_CASE_ADD(tc_2_main_task, " Test #2 - Test Erase function"),
EU_TEST_CASE_ADD(tc_3_main_task, " Test #3 - Test Marking blocks as bad/good block"),
EU_TEST_CASE_ADD(tc_4_main_task, " Test #4 - CopyPage in NFCNandMedia "),
EU_TEST_SUITE_END(suite_nfc_media)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
EU_TEST_SUITE_ADD(suite_nfc_media, " - Testing Nand NFC Media")
EU_TEST_REGISTRY_END()

TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
    /*  Task number, Entry point, Stack, Pri, String, Auto? */
    {MAIN_TASK,   main_task,   5000,  9,   "main", MQX_AUTO_START_TASK},
    {0,           0,           0,     0,   0,      0,                 }
};

RtStatus_t CopyPagesFlagFilter::filter
(
    NandPhysicalMedia    * fromNand,
    NandPhysicalMedia    * toNand,
    uint32_t             fromPage,
    uint32_t             toPage,
    SECTOR_BUFFER        * sectorBuffer,
    SECTOR_BUFFER        * auxBuffer,
    bool                 * didModifyPage
)
{
    // Create a metadata object so we can work with flags.
    Metadata md(auxBuffer);

    // Check if the is-in-order flag is set on this page.
    if (md.isFlagSet(Metadata::kIsInLogicalOrderFlag))
    {
        // Clear the flag.
        md.clearFlag(Metadata::kIsInLogicalOrderFlag);

        // Inform the HAL that we changed the page contents.
        assert(didModifyPage);
        *didModifyPage = true;
    }

    return SUCCESS;
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_1_main_task
* Comments       :
*    The function is used to test write/read/ReadMetadata functions
*
*END------------------------------------------------------------------*/

void tc_1_main_task(void){
    MQX_FILE_PTR         nand_wl_ptr;
    _mqx_uint            result;
    RtStatus_t           status;
    NFCNandMedia         *pNand;
    unsigned char            *pWriteData, *pWriteAux, *pReadData, *pReadAux, *pMeta;
    uint32_t blocknum = 10;
    uint32_t secnum = 2560;

    nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NANDWL_OPEN_IGNORE_INIT_MEDIA);  /* Flag must be 'I' */
    NandHalSetMqxPtr(0, nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (nand_wl_ptr != NULL), " Test #1.1: Open file failed");

    /* Erase NAND Flash chip defined */
    result = nandwl_erase_entire_chip(nand_wl_ptr, NANDFLASH_1ST_DATA_DRIVE_START_BLOCK, NANDFLASH_1ST_DATA_DRIVE_START_BLOCK + NANDFLASH_1ST_DATA_DRIVE_SIZE_IN_BLOCK);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == TRUE), " Test #1.2: Erase entire chip failed");

    /*Get nandwl_phy_page_size & nandwl_phy_page_spare_size*/
    nandwl_testing_get_info(nand_wl_ptr);

    /* Init NandHal */
    status = NandHal::init();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (status == SUCCESS), " Test #1.3: Init NandHal failed");

    /* Get first NandMedia */
    pNand = (NFCNandMedia *)NandHal::getFirstNand();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (pNand != NULL), " Test #1.4: Cannot get the first NandMedia");

    /* Prepare buffer */
    pWriteData = allocate_buffer(nandwl_virt_page_size, "pWriteData");
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (pWriteData != NULL), " Test #1.5: Allocate pWriteData failed ");

    result = fill_continous_buff(pWriteData, nandwl_virt_page_size);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == TRUE), " Test #1.6: Cannot fill data to pWriteData buffer ");

    pWriteAux = allocate_buffer(nandwl_page_metadata_size, "pWriteAux");
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (pWriteAux != NULL), " Test #1.7: Allocate pWriteAux failed ");

    result = fill_continous_buff(pWriteAux, nandwl_page_metadata_size);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == TRUE), " Test #1.8: Cannot fill data to pWriteAux buffer ");

    pReadData = allocate_buffer(nandwl_virt_page_size, "pReadData");
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (pReadData != NULL), " Test #1.9: Allocated pReadData buffer failed ");

    pReadAux = allocate_buffer(nandwl_page_metadata_size, "pReadAux");
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (pReadAux != NULL), " Test #1.10: Allocate pReadAux failed ");

    pMeta = allocate_buffer(nandwl_page_metadata_size, "pMeta");
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (pMeta != NULL), " Test #1.11: Allocate pReadAux failed ");

    /* Check status of a block
    Increase block address if the block is bad */
    while (pNand->isBlockBad(blocknum, pWriteAux) == true) {
        blocknum++;
    }
    secnum = blocknum * nandwl_num_virt_pages_per_block;

    /* Write a sector */
    status = pNand->writePage(secnum, pWriteData, pWriteAux);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (status == SUCCESS), " Test #1.12: Write sector to NandFlash failed ");

    /* Read a sector */
    status = pNand->readPage(secnum, pReadData, pReadAux);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (status == SUCCESS), " Test #1.13: Cannot read sector by using pNand->readPage() function ");

    /* Read metadata of a sector */
    status = pNand->readMetadata(secnum, pMeta);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (status == SUCCESS), " Test #1.14: Read metadata of sector failed ");

    /* Compare buffers */
    result = compare_test(pWriteData, pReadData, nandwl_virt_page_size) ;
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == TRUE), " Test #1.15: The data between pWriteData & pReadData buffer are not the same ");

    result = compare_test(pWriteAux, pReadAux, nandwl_page_metadata_size);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == TRUE), " Test #1.16: The data between pWriteAux & pReadAux buffer are not the same ");

    result = compare_test(pReadAux, pMeta, nandwl_page_metadata_size);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == TRUE), " Test #1.17: The data between pReadAux & pMeta buffer are not the same ");

    /* Free all allocated buffer */
    _mem_free(pWriteData);
    _mem_free(pWriteAux);
    _mem_free(pReadData);
    _mem_free(pReadAux);
    _mem_free(pMeta);
    
    /* Shutdown NandHal */
    status = NandHal::shutdown();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (status == SUCCESS), " Test #1.18: shutdown NandHal failed");

    result = fclose(nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), " Test #1.19: Close the nand_wl file");   
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_2_main_task
* Comments       :
*    The function is used to test erase function
*
*END------------------------------------------------------------------*/

void tc_2_main_task(void){
    MQX_FILE_PTR     nand_wl_ptr;
    _mqx_uint        result;
    RtStatus_t       status;

    NFCNandMedia * pNand;
    uint32_t blkNum = 11;
    uint32_t startBlk = 12;
    uint32_t afterEndBlk = 20;
    uint32_t actualBlkCount;
    uint32_t try_count =0;
    uint32_t max_try_count =15;

    nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NANDWL_OPEN_IGNORE_INIT_MEDIA);  /* Flag must be 'I' */
    NandHalSetMqxPtr(0, nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (nand_wl_ptr != NULL), " Test #2.1: Open file failed");

    /* ERASE entire nandflash chip */
    result = nandwl_erase_entire_chip(nand_wl_ptr, NANDFLASH_1ST_DATA_DRIVE_START_BLOCK, NANDFLASH_1ST_DATA_DRIVE_START_BLOCK + NANDFLASH_1ST_DATA_DRIVE_SIZE_IN_BLOCK);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == TRUE), " Test #2.2: Erase entire chip failed");

    /* Init NandHal */
    status = NandHal::init();
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (status == SUCCESS), " Test #2.3: Init NandHal failed");

    /* Get first NandMedia */
    pNand = (NFCNandMedia *)NandHal::getFirstNand();
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (pNand != NULL), " Test #2.4: Cannot get the first NandMedia");

    /* Erase one block */
    while (try_count < max_try_count)
    {
        WL_TEST_LOG(" %d trying(max %d times) erase block %d \n",
        try_count + 1, max_try_count, blkNum);

        if (pNand->eraseBlock(blkNum) != SUCCESS) {
            WL_TEST_LOG("    Erase block %d *FAILED* \n", blkNum);
            blkNum++;
        }
        else
        {
            WL_TEST_LOG("    Erase block %d SUCCESS  \n", blkNum);
            break;
        }
        try_count++;
    }

    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (try_count != max_try_count), " Test #2.5: Cannot erase block in try_count times");

    /* Erase multiple blocks */
    WL_TEST_LOG("Erase multiple blocks %d-%d \n ", startBlk, afterEndBlk);
    status = pNand->eraseMultipleBlocks(blkNum+startBlk, blkNum+afterEndBlk, (uint32_t *)&actualBlkCount);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (status == SUCCESS), " Test #2.6: Cannot erase multiple blocks from startBlk to afterEndBlk ");

    status = NandHal::shutdown();
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (status == SUCCESS), " Test #2.7: Cannot shutdown NandHal ");

    fclose(nand_wl_ptr);
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_3_main_task
* Comments       :
*    The function is used to test marking block as bad/good block
*
*END------------------------------------------------------------------*/

void tc_3_main_task(void){
    MQX_FILE_PTR         nand_wl_ptr;
    _mqx_uint            result;
    RtStatus_t           status;
    NFCNandMedia         * pNand;
    uint32_t              blkNum = 10;
    unsigned char *            buf = NULL;
    unsigned char *            aux = NULL;

    nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NANDWL_OPEN_IGNORE_INIT_MEDIA);  /* Flag must be 'I' */
    NandHalSetMqxPtr(0, nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (nand_wl_ptr != NULL), " Test #3.1: Open file failed");

    nandwl_testing_get_info(nand_wl_ptr);    /*Get nandwl_phy_page_size & nandwl_phy_page_spare_size*/

    /* Init NandHal */
    status = NandHal::init();
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (status == SUCCESS), " Test #3.2: Init NandHal failed");

    /* Get first NandMedia */
    pNand = (NFCNandMedia *)NandHal::getFirstNand();
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (pNand != NULL), " Test #3.3: Get first NandMedia failed");

    /* Prepare buffer */
    buf = allocate_buffer(nandwl_virt_page_size, "buf");
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (buf != NULL), " Test #3.4: Allocate buffer to buf with size = nandwl_virt_page_size failed");

    aux = allocate_buffer(nandwl_page_metadata_size, "aux");
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (aux != NULL), " Test #3.5: Allocate buffer to aux with size = nandwl_page_metadata_size failed");

    /* Check status of a block
    Increase block address if the block is bad */
    while (pNand->isBlockBad(blkNum, aux) == true) {
        blkNum++;
    }

    /* Mark the block bad */
    status = pNand->markBlockBad(blkNum, buf, aux);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (status == SUCCESS), " Test #3.6: Mark Block Bad to blkNum failed");

    /* Confirm the block was marked bad */
    result = pNand->isBlockBad(blkNum, aux);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == TRUE), " Test #3.7: Cannot confirm the block was marked bad");

    status = NandHal::shutdown();
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (status == SUCCESS), " Test #3.8: Shutdown NandHal failed ");

    /* Free all allocated buffer*/
    _mem_free(buf);
    _mem_free(aux);

    result = fclose(nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_OK), " Test #3.9: Close the nand_wl file ");
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_4_main_task
* Comments       :
*      The function is used to test copy/page in NFCNand Media
*
*END------------------------------------------------------------------*/
void tc_4_main_task(void){
    MQX_FILE_PTR         nand_wl_ptr;
    _mqx_uint            result;
    RtStatus_t           status;                        
    NFCNandMedia         * pNand;
    uint32_t              fromSecNum = 5120;
    uint32_t              toSecNum = 7680;
    uint32_t              i = 0;
    uint32_t              j = 0;
    unsigned char            *pWriteData, *pWriteAux, *pReadData, *pReadAux;
    unsigned char *            dataArr[10];
    unsigned char *            auxArr[10];
    CopyPagesFlagFilter  copyFilter;
    uint32_t             successfulCopies;


    nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NANDWL_OPEN_IGNORE_INIT_MEDIA);  /* Flag must be 'I' */
    NandHalSetMqxPtr(0, nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (nand_wl_ptr != NULL), " Test #4.1: Open file failed");

    nandwl_testing_get_info(nand_wl_ptr);    /*Get nandwl_phy_page_size & nandwl_phy_page_spare_size*/

    /* ERASE entire nandflash chip */
    result = nandwl_erase_entire_chip(nand_wl_ptr, NANDFLASH_1ST_DATA_DRIVE_START_BLOCK, NANDFLASH_1ST_DATA_DRIVE_START_BLOCK + NANDFLASH_1ST_DATA_DRIVE_SIZE_IN_BLOCK);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == TRUE), " Test #4.2: Erase entire chip failed");

    /* Init NandHal */
    status = NandHal::init();
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (status == SUCCESS), " Test #4.3: Init NandHal failed");

    /* Get first NandMedia */
    pNand = (NFCNandMedia *)NandHal::getFirstNand();
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (pNand != NULL), " Test #4.4: Cannot get the first NandMedia");

    /* Write */
    for (i = 0; i < 256; i++) {
        /* Prepare buffer */
        pWriteData = allocate_buffer(nandwl_virt_page_size, "pWriteData");
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (pWriteData != NULL), " Test #4.5: Allocate buffer with size = nandwl_virt_page_size failed");
        fill_continous_buff(pWriteData, nandwl_virt_page_size);
        pWriteData[0] = i;

        pWriteAux = allocate_buffer(nandwl_page_metadata_size, "pWriteAux");
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (pWriteAux != NULL), " Test #4.6: Allocate buffer with size = nandwl_page_metadata_size failed");
        fill_continous_buff(pWriteAux, nandwl_page_metadata_size);
        pWriteAux[0] = i;

        status = pNand->writePage(fromSecNum + i, pWriteData, pWriteAux);
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (status == SUCCESS), " Test #4.7: Write Page to sector number = fromSecNum + i failed");

        _mem_free(pWriteData);
        _mem_free(pWriteAux);
    }

    /* Copy */
    for (i = 0; i < 256; i++) {
        status = pNand->copyPages(
        (NandPhysicalMedia *)pNand,
        fromSecNum + i,
        toSecNum + i,
        1,
        pWriteData,
        pWriteAux,
        (NandCopyPagesFilter *)&copyFilter,
        &successfulCopies) ;
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (status == SUCCESS), " Test #4.8: Copy Page failed");
    }

    pReadData = allocate_buffer(nandwl_virt_page_size, "pReadData");
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (pReadData != NULL), " Test #4.9: Allocate pReadData buffer with size = nandwl_virt_page_size");

    pReadAux = allocate_buffer(nandwl_page_metadata_size, "pReadAux");
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (pReadAux != NULL), " Test #4.10: Allocate pReadAux buffer with size = nandwl_virt_page_size");
    /* Read and compare */
    for (i = 0; i < 256; i++) {
        /* Read */
        status = pNand->readPage(toSecNum + i, pReadData, pReadAux);
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (status == SUCCESS), " Test #4.11: ReadPage with sector number = toSecNum + i");
        
        /* Validate data */
        result = FALSE;
        if (pReadData[0] == i) 
        result = TRUE; 
        else 
        result = FALSE;
        for (j = 1; j < nandwl_virt_page_size; j++)
        {
            if (pReadData[j] == (j % 0xFF)) 
            result = TRUE; 
            else
            result = FALSE;
        }
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == TRUE), " Test #4.12: Compare data between pWriteData & pReadData");

        result = FALSE;
        if (pReadAux[0] == i) 
        result = TRUE; 
        else 
        result = FALSE;
        for (j = 1; j < nandwl_page_metadata_size; j++)
        {
            if (pReadAux[j] == (j % 0xFF)) 
            result = TRUE; 
            else
            result = FALSE;
        }
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == TRUE), " Test #4.12: Compare data between pWriteAux & pReadAux");
    }

    status = NandHal::shutdown();
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (status == SUCCESS), " Test #4.13: Shutdown NandHal");

    /* Free all allocated buffer*/
    _mem_free(pReadData);
    _mem_free(pReadAux);

    result = fclose(nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_OK), " Test #4.14: Close the nand_wl file");
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : main_task
* Comments       : The task is used to test media component
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
