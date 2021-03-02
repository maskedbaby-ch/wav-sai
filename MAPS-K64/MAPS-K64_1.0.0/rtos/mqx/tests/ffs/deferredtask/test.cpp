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
*   This file is used to test deferedtask
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
#include "nandflash_wl_testing.h"
#include "testing_main_internal.h"
#include "deferred_task.h"
#include "deferred_task_queue.h"
#include "nonsequential_sectors_map.h"
#include "test.h"

void tc_1_main_task(void);
void tc_2_main_task(void);
void tc_3_main_task(void);
void tc_4_main_task(void);

//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_deferedtask)
    EU_TEST_CASE_ADD(tc_1_main_task, " Test #1 - Test initialization & de-initialization defered task queue "),
    EU_TEST_CASE_ADD(tc_2_main_task, " Test #2 - Test every post in each kTaskThreadTimeoutMSecs + 100ms"),
    EU_TEST_CASE_ADD(tc_3_main_task, " Test #3 - Test posted task run well after defered task shutdown"),
    EU_TEST_CASE_ADD(tc_4_main_task, " Test #4 - Test multiple task run well after defered task shutdown "),
EU_TEST_SUITE_END(suite_deferedtask)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_deferedtask, " - Testing Defered Task")
EU_TEST_REGISTRY_END()

using namespace nand;
DeferredTaskQueue * m_deferredTasks;

typedef struct
{
    bool isExamine;
    bool isExamineOne;
    bool isRun;
} TQ_STATUS;

static TQ_STATUS task_queue_status[100] =
{ 0 };
static uint32_t current_task_queue_status_count = 0;

class SimpleDeferredTask : public DeferredTask
{
public:

    static bool m_isExamine;
    static bool m_isExamineOne;
    static bool m_isRun;

    SimpleDeferredTask()
    : DeferredTask(10)
    {

    }
    /*
     bool examine(DeferredTaskQueue & queue)
     {
     task_queue_status[current_task_queue_status_count].isExamine = TRUE;
     return true;
     }
     */
    virtual bool getShouldExamine() const
    {

        return true;
    }
    virtual bool examineOne(DeferredTask * task)
    {
        task_queue_status[current_task_queue_status_count].isExamineOne = TRUE;
        return true;
    }
    virtual void task()
    {
        task_queue_status[current_task_queue_status_count].isRun = TRUE;
        current_task_queue_status_count++;
        return;
    }
    virtual uint32_t getTaskTypeID() const
    {
        return (('H'<<24)|('N'<<16)|('N'<<8)|('H')); /* 'HNNH' */
    }
};

TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
/*  Task number, Entry point, Stack, Pri, String, Auto? */
   {MAIN_TASK,   main_task,   2000,  9,   "main", MQX_AUTO_START_TASK},
   {0,           0,           0,     0,   0,      0,                 }
};

static bool init_defered_taks_queue();
static bool shutdown_defered_task_queue();

static bool init_defered_taks_queue()
{
    RtStatus_t error;
    media_buffer_init();
    // Create the deferred task queue.
    m_deferredTasks = new DeferredTaskQueue;
    return m_deferredTasks->init();

}

static bool shutdown_defered_task_queue()
{
    RtStatus_t error;
    // Shut down the deferred tasks.
    error = m_deferredTasks->drain();

    delete m_deferredTasks;

    media_buffer_deinit();

    return error;

}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_1_main_task
* Comments       :
*    The function is used to test initalize & de-init deferedtask queue
*
*END------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    MQX_FILE_PTR nand_wl_ptr;
    _mqx_uint    result;
    int i = 0;
    RtStatus_t error;

    nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NANDWL_OPEN_IGNORE_INIT_MEDIA);  /* Flag must be 'I' */
    NandHalSetMqxPtr(0, nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (nand_wl_ptr != NULL), " Test #1.1: Open file failed");

    error = init_defered_taks_queue();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error == SUCCESS), " Test #1.2: Init defered task queue failed");

    error = shutdown_defered_task_queue();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error == SUCCESS), " Test #1.3: Shutdown defered task queue failed");

    result = fclose(nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), " Test #1.4: Close the nand_wl file");
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_2_main_task
* Comments       :
*    This function is used to test every post in each kTaskThreadTimeoutMSecs + 100ms
*
*END------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    MQX_FILE_PTR      nand_wl_ptr;
    int               i = 0;
    _mqx_uint         result;
    SimpleDeferredTask * task = new SimpleDeferredTask();
    RtStatus_t        error;

    nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NANDWL_OPEN_IGNORE_INIT_MEDIA);  /* Flag must be 'I' */
    NandHalSetMqxPtr(0, nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (nand_wl_ptr != NULL), " Test #2.1: Open file failed");

    error = init_defered_taks_queue();
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error == SUCCESS), " Test #2.2: Init defered task queue failed");

    // RelocateVirtualBlockTask * task = new RelocateVirtualBlockTask(NULL, 0);
    current_task_queue_status_count = 0;
    m_deferredTasks->post(task);

    WL_TEST_LOG("Post *task* to deferred task queue. Wait %d ms \n",
            DeferredTaskQueue::kTaskThreadTimeoutMSecs+100);

    /* Delay time*/
    _time_delay(DeferredTaskQueue::kTaskThreadTimeoutMSecs+100 );

     result = m_deferredTasks->isEmpty() && task_queue_status[0].isRun;
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == TRUE), " Test #2.4: Task is not run");

    error = shutdown_defered_task_queue();
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error == SUCCESS), " Test #2.5: Shutdown defered task queue failed");

    result = fclose(nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), " Test #2.6: Close the nand_wl file");

}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_3_main_task
* Comments       :
*    The function is used to test posted tasks run well after deferedtask
*    shutdown
*
*END------------------------------------------------------------------*/
void tc_3_main_task(void)
{
    MQX_FILE_PTR         nand_wl_ptr;
    int                  i = 0;
    SimpleDeferredTask * task = new SimpleDeferredTask;
    RtStatus_t           error;
    _mqx_uint            result ;

    nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NANDWL_OPEN_IGNORE_INIT_MEDIA);  /* Flag must be 'I' */
    NandHalSetMqxPtr(0, nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (nand_wl_ptr != NULL), " Test #3.1: Open file failed");

    error = init_defered_taks_queue();
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error == SUCCESS), " Test #3.2: Init defered task queue failed");

    current_task_queue_status_count = 0;
    // RelocateVirtualBlockTask * task = new RelocateVirtualBlockTask(NULL, 0);

    m_deferredTasks->post(task);

    WL_TEST_LOG("Post *task* to deferred task queue. Wait %d ms \n",
            DeferredTaskQueue::kTaskThreadTimeoutMSecs/2);
    _time_delay(DeferredTaskQueue::kTaskThreadTimeoutMSecs/2);

    error = shutdown_defered_task_queue();
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error == SUCCESS), " Test #3.4: Shutdown defered task queue failed");

    result =  task_queue_status[0].isRun;
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result  == TRUE), " Test #3.5: Task queue is not run");

    result = fclose(nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_OK), " Test #3.6: Close the nand_wl file");
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_4_main_task
* Comments       :
*    The function is used to test multiple task run well after deferedtask
*    shutdown
*
*END------------------------------------------------------------------*/
void tc_4_main_task(void)
{
    MQX_FILE_PTR       nand_wl_ptr;
    RtStatus_t         error;
    _mqx_uint          result;
    _mqx_int           i = 0;
    SimpleDeferredTask * task;

    nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NANDWL_OPEN_IGNORE_INIT_MEDIA);  /* Flag must be 'I' */
    NandHalSetMqxPtr(0, nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (nand_wl_ptr != NULL), " Test #4.1: Open file failed");

    error = init_defered_taks_queue();
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (error == SUCCESS), " Test #4.2: Init defered task queue failed");

    current_task_queue_status_count = 0;

    while (i++ < 10)
    {
        // RelocateVirtualBlockTask * task = new RelocateVirtualBlockTask(NULL, 0);
        task = new SimpleDeferredTask();
        m_deferredTasks->post(task);
        WL_TEST_LOG("Post *task* to deferred task queue. Wait %d ms \n",
                DeferredTaskQueue::kTaskThreadTimeoutMSecs/2);
        _time_delay(DeferredTaskQueue::kTaskThreadTimeoutMSecs/2);
    }

    error = shutdown_defered_task_queue();
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (error == SUCCESS), " Test #4.3: Shutdown defered task queue failed");

    for (i = 0; i < current_task_queue_status_count; i++)
    {
        if (
                /* (!task_queue_status[i].isExamine) ||
                 !(task_queue_status[i].isExamineOne) ||*/
                !(task_queue_status[i].isRun))
        break;
    }

    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (i == current_task_queue_status_count), " Test #4.4: It has at lease one task queue cannot run");

    result = fclose(nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_OK), " Test #4.5: Close the nand_wl file");
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : main_task
* Comments       : This task is used to test deferedtask component
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
