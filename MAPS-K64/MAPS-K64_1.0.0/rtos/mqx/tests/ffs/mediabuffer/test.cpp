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
#include "wl_common.h"
#include "mutex.h"
#include "testing_main_internal.h"
#include "media_buffer.h"
#include "media_buffer_manager.h"
#include "media_buffer_manager_internal.h"
#include "mem_management_internal.h"
#include "mem_management.h"
#include "test.h"

extern MediaBufferManagerContext_t g_mediaBufferManagerContext;
extern MEM_ALLOCATION_TABLE g_mem_allocation_table;

void tc_1_main_task(void);
void tc_2_main_task(void);
void tc_3_main_task(void);
void tc_4_main_task(void);
void tc_5_main_task(void);

//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_mediabuffer)
    EU_TEST_CASE_ADD(tc_1_main_task, " Test #1 - Test init&de-init media buffer "),
    EU_TEST_CASE_ADD(tc_2_main_task, " Test #2 - Test realease buffer in case that remain time-out"),
    EU_TEST_CASE_ADD(tc_3_main_task, " Test #3 - Test release temporary buffer in case that time-out is finished"),
    EU_TEST_CASE_ADD(tc_4_main_task, " Test #4 - Test release all temporary buffer in case that time-out is finished"),
    EU_TEST_CASE_ADD(tc_5_main_task, " Test #5 - Test allocated temporary buffer before release it"),
EU_TEST_SUITE_END(suite_mediabuffer)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_mediabuffer, " - Testing media buffer")
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
* Comments       : Test init&de-init media buffer
*    
*
*END------------------------------------------------------------------*/
void tc_1_main_task (void)
{
    MQX_FILE_PTR            nand_wl_ptr;
    volatile RtStatus_t     error = SUCCESS;
    _mqx_uint               result;

    /* Step 1: Open NANDFLASH WL*/
    nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NANDWL_OPEN_IGNORE_INIT_MEDIA);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (nand_wl_ptr != NULL), " Test #1.1: Open file failed");
    NandHalSetMqxPtr(0, nand_wl_ptr);

    error = media_buffer_init();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error == SUCCESS), " Test #1.2: Initialize media buffer  ");

    media_buffer_report_stats();
    mm_display();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (TRUE), " Test #1.3: Display the memory statistic");

    error = media_buffer_deinit();
    mm_display();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error == SUCCESS), " Test #1.4: De-init media buffer ");

   result = fclose(nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), " Test #1.5: Close file failed");
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_2_main_task
* Comments       : Test realease buffer in case that remain time-out
*    
*
*END------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    MQX_FILE_PTR           nand_wl_ptr;
    volatile RtStatus_t error = SUCCESS;
    SECTOR_BUFFER * pBuf;
    MediaBufferType_t type = kMediaBufferType_Sector;
    uint32_t i, result;

    /* Step 1: Open NANDFLASH WL*/
    nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NANDWL_OPEN_IGNORE_INIT_MEDIA);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (nand_wl_ptr != NULL), " Test #2.1: Open file failed");
    NandHalSetMqxPtr(0, nand_wl_ptr);

    error = media_buffer_init();
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error == SUCCESS), " Test #2.2: Initialize media buffer successfull ");

    media_buffer_report_stats();
    mm_display();
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (TRUE), " Test #2.3: Display the memory statistic");

    /* Debug Media Buffer */
    /* kMediaBufferType_Auxiliary */
    error = media_buffer_acquire(type, kMediaBufferFlag_None, &pBuf);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error == SUCCESS), " Test #2.4: Obtain the free buffer failed");

    error = media_buffer_release(pBuf);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error == SUCCESS), " Test #2.5: Release the media buffer");

    /* Confirm the pBuf is not really free */
    error = !(SUCCESS);

    for (i = 0; i < g_mediaBufferManagerContext.slotCount; ++i)
    {
        MediaBufferInfo_t * info = &g_mediaBufferManagerContext.buffers[i];

        if (info->data == pBuf)
        {
            error = SUCCESS;
        }
    }
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error == SUCCESS), " Test #2.6: Verify that pBuf is not really free");

    error = media_buffer_deinit();
    mm_display();
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error == SUCCESS), " Test #2.7: De-init media buffer");

   result = fclose(nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), " Test #2.8: Close file failed");
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_3_main_task
* Comments       : Test release temporary buffer in case that time-out is finished
*    
*
*END------------------------------------------------------------------*/
void tc_3_main_task(void)
{
    MQX_FILE_PTR           nand_wl_ptr;
    volatile RtStatus_t    error = SUCCESS;
    SECTOR_BUFFER *        pBuf;
    MediaBufferType_t      type = kMediaBufferType_Sector;
    uint32_t                i, result;

    /* Step 1: Open NANDFLASH WL*/
    nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NANDWL_OPEN_IGNORE_INIT_MEDIA);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (nand_wl_ptr != NULL), " Test #3.1: Open file failed");
    NandHalSetMqxPtr(0, nand_wl_ptr);

    error = media_buffer_init();
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error == SUCCESS), " Test #3.2: Initialize media buffer failed ");

    media_buffer_report_stats();
    mm_display();
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (TRUE), " Test #3.3: Display the memory statistic");

    /* Debug Media Buffer */
    /* kMediaBufferType_Auxiliary */
    error = media_buffer_acquire(type, kMediaBufferFlag_None, &pBuf);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error == SUCCESS), " Test #3.4: Obtain the free buffer failed");

    error = media_buffer_release(pBuf);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error == SUCCESS), " Test #3.5: Release the media buffer");

    _time_delay(TEMPORARY_BUFFER_TIMEOUT_MS);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (TRUE), " Test #3.6: Waiting TEMPORARY_BUFFER_TIMEOUT_MS ms");

    /* Confirm the pBuf is not really free */

    for (i = 0; (i < g_mediaBufferManagerContext.slotCount)
            && (g_mediaBufferManagerContext.buffers[i].data != pBuf); i++)
    {}
    /* Check for whether this buffer is really freed */
    error = !(SUCCESS);
    if (i == g_mediaBufferManagerContext.slotCount)
    {
        error = SUCCESS;
    }
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error == SUCCESS), " Test #3.7: Verify that pBuf is really free");

    error = media_buffer_deinit();
    mm_display();
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error == SUCCESS), " Test #3.8: De-init media buffer");

   result = fclose(nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_OK), " Test #3.9: Close file failed");
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_4_main_task
* Comments       : Test release all temporary buffer in case that time-out is finished
*    
*
*END------------------------------------------------------------------*/
void tc_4_main_task(void)
{
    MQX_FILE_PTR           nand_wl_ptr;
    volatile RtStatus_t    error;
    SECTOR_BUFFER *        pBuf[100];
    MediaBufferType_t      type = kMediaBufferType_Sector;
    uint32_t                i, result;

    /* Step 1: Open NANDFLASH WL*/
    nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NANDWL_OPEN_IGNORE_INIT_MEDIA);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (nand_wl_ptr != NULL), " Test #4.1: Open file failed");
    NandHalSetMqxPtr(0, nand_wl_ptr);

    error = media_buffer_init();
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (error == SUCCESS), " Test #4.2: Initialize media buffer failed ");

    /* Debug Media Buffer */
    /* kMediaBufferType_Auxiliary */
    for (i = 0; i < MAX_BUFFER_COUNT; i++)
    {
        error = media_buffer_acquire(type, kMediaBufferFlag_None, &pBuf[i]);
        if (SUCCESS != error)
        {
            media_buffer_deinit();
        }
    }

    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (error == SUCCESS), " Test #4.3: Obtain free buffer by media_buffer_acquire() failed");

    error = media_buffer_deinit();
    if ((g_mediaBufferManagerContext.slotCount == 0) && (error == SUCCESS))
    {
        error = SUCCESS;
    }
    mm_display();
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (error == SUCCESS), " Test #4.4: De-init media buffer");

    result = fclose(nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_OK), " Test #4.5: Close file failed");
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_5_main_task
* Comments       : Test allocated temporary buffer before release it
*    
*
*END------------------------------------------------------------------*/
void tc_5_main_task(void)
{
    MQX_FILE_PTR            nand_wl_ptr;
    volatile RtStatus_t     error = SUCCESS;
    SECTOR_BUFFER           * pBuf1;
    SECTOR_BUFFER           * pBuf2;
    MediaBufferType_t       type = kMediaBufferType_Sector;
    uint32_t                 i,result;
    MediaBufferInfo_t       *info = NULL;

    /* Step 1: Open NANDFLASH WL*/
    nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NANDWL_OPEN_IGNORE_INIT_MEDIA);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (nand_wl_ptr != NULL), " Test #5.1: Open file failed");
    NandHalSetMqxPtr(0, nand_wl_ptr);

    error = media_buffer_init();
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (error == SUCCESS), " Test #5.2: Initialize media buffer failed ");

    media_buffer_report_stats();
    mm_display();
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (TRUE), " Test #5.3: Display memory statistic ");

    error = media_buffer_acquire(type, kMediaBufferFlag_None, &pBuf1);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (error == SUCCESS), " Test #5.4: Acquire pBuf1 buffer by calling media_buffer_acquire() function ");

    error = media_buffer_release(pBuf1);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (error == SUCCESS), " Test #5.5: Release the pBuf1 buffer ");

    error = media_buffer_acquire(type, kMediaBufferFlag_None, &pBuf2);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (error == SUCCESS), " Test #5.6: Acquire pBuf2 buffer  ");

    error = media_buffer_release(pBuf2);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (error == SUCCESS), " Test #5.7: Release the pBuf2 buffer ");

    for (i = 0; (i < g_mediaBufferManagerContext.slotCount)
            && (g_mediaBufferManagerContext.buffers[i].data != pBuf2); i++)
    {
        break;
    }

    /* Check for whether the timer is set or not */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (i == g_mediaBufferManagerContext.nextTimeout), " Test #5.8: Check the time out release is set");

    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (pBuf1 == pBuf2), " Test #5.9: Verify that pBuf2 is equal pBuf1");

    error =  media_buffer_deinit();
    mm_display();
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (error == SUCCESS), " Test #5.10: De-init the media buffer");

    /* Step 9: Close NAND WL */
    result = fclose(nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_OK), " Test #5.11: Close file failed");
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : main_task
* Comments       : The task is used to test the mediabuffer component
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
