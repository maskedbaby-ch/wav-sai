/**HEADER*******************************************************************
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
* $FileName: core_mutex.c$
* $Version : 4.0.2$
* $Date    : Aug-19-2013$
*
* Comments:
*
*   This file contains the source for the core mutex program.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <fio.h>
#include <core_mutex.h>

#include "EUnit.h"
#include "eunit_crit_secs.h"
#include "util.h"

#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif

/* Function declaration */
void tc_1_main_task(void);
void tc_2_main_task(void);
void tc_3_main_task(void);
void tc_4_main_task(void);
void main_task(unsigned int);
unsigned int _core_mutex_create_internal(CORE_MUTEX_PTR,unsigned int, unsigned int, unsigned int,unsigned long);
unsigned int core_mutex_uninstall(void);

const TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   /* Task Index,   Function,   Stack,  Priority, Name,     Attributes,          Param, Time Slice */
    { 1,   main_task, 1500,   8,        "main_task",  MQX_AUTO_START_TASK, 0,     0 },
    { 0 }
};

const CORE_MUTEX_INIT_STRUCT _core_mutex_init_info = {
    BSPCFG_CORE_MUTEX_PRIORITY
};

#define MQX_TASK_TEST_POLICY    (0x1234)

/*TASK*-----------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     :
*    This task mainly tests _core_mutex_create(),_core_mutex_lock() and _core_mutex_create_internal().
* Requirements : CM004,CM005,CM006,CM007,CM008,CM009,CM010,CM012,CM013,CM014,CM015,CM016,CM033,CM034,CM035
*END*-----------------------------------------------------*/
void tc_1_main_task(void)
{
    CORE_MUTEX_PTR           cm_ptr,temp_cm_ptr;
    CORE_MUTEX_COMPONENT_PTR component_ptr;
    unsigned int             ret;

    /* When the component doesn't exist, _core_mutex_create_internal() should return MQX_COMPONENT_DOES_NOT_EXIST */

    /* Save the component pointer */
    component_ptr = _core_mutext_get_component_ptr();
    /* Set the component pointer to NULL */
    _core_mutext_set_component_ptr(NULL);
    /* Alloc system memory for a core mutex */
    temp_cm_ptr = (CORE_MUTEX_PTR)_mem_alloc_system_zero(sizeof(*temp_cm_ptr));
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, temp_cm_ptr != NULL, "TEST #1: 1.1 _mem_alloc_system_zero operation");
    /* Create a core mutex */
    ret = _core_mutex_create_internal(temp_cm_ptr, 0, 1, MQX_TASK_QUEUE_FIFO, TRUE);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, ret == MQX_COMPONENT_DOES_NOT_EXIST, "TEST #1: 1.2 _core_mutex_create_internal operation");

    /* When the component doesn't exist, _core_mutex_create() should return NULL */
    /* Create a core mutex */
    cm_ptr = _core_mutex_create( 0, 1, MQX_TASK_QUEUE_FIFO );
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, cm_ptr == NULL, "TEST #1: 1.3 _core_mutex_create operation");
    /* Get the task error code */
    ret = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, ret == MQX_COMPONENT_DOES_NOT_EXIST, "TEST #1: 1.4 Checking the task error code");
    _task_set_error(MQX_OK);

    /* When mutex_num is greater than or equal to SEMA4_NUM_GATES, _core_mutex_create_internal() should return MQX_INVALID_PARAMETER */

    /* Restore the component pointer */
    _core_mutext_set_component_ptr(component_ptr);
    /* Create a core mutex */
    ret = _core_mutex_create_internal(temp_cm_ptr, 0, 16, MQX_TASK_QUEUE_FIFO, TRUE);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, ret == MQX_INVALID_PARAMETER, "TEST #1: 1.5 _core_mutex_create_internal operation");

    /* When mutex_num is greater than or equal to SEMA4_NUM_GATES, _core_mutex_create() should return NULL */

    /* Create a core mutex */
    cm_ptr = _core_mutex_create( 0, 16, MQX_TASK_QUEUE_FIFO );
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, cm_ptr == NULL, "TEST #1: 1.6 _core_mutex_create operation");
    /* Get the task error code */
    ret = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, ret == MQX_INVALID_PARAMETER, "TEST #1: 1.7 Checking the task error code");
    _task_set_error(MQX_OK);

    /* When dev_num is greater than or equal to SEMA4_NUM_DEVICES, _core_mutex_create_internal() should return MQX_INVALID_PARAMETER */

    /* Create a core mutex */
    ret = _core_mutex_create_internal(temp_cm_ptr, SEMA4_NUM_DEVICES+1, 1, MQX_TASK_QUEUE_FIFO, TRUE);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, ret == MQX_INVALID_PARAMETER, "TEST #1: 1.8 _core_mutex_create_internal operation");

    /* When dev_num is greater than or equal to SEMA4_NUM_DEVICES, _core_mutex_create() should return NULL */

    /* Create a core mutex */
    cm_ptr = _core_mutex_create(SEMA4_NUM_DEVICES+1, 1, MQX_TASK_QUEUE_FIFO );
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, cm_ptr == NULL, "TEST #1: 1.9 _core_mutex_create operation");
    /* Get the task error code */
    ret = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, ret == MQX_INVALID_PARAMETER, "TEST #1: 1.10 Checking the task error code");
    _task_set_error(MQX_OK);

   /* When creating task queue fails, _core_mutex_create_internal() should return MQX_TASKQ_CREATE_FAILED */

    ret = _core_mutex_create_internal(temp_cm_ptr,0,1,MQX_TASK_TEST_POLICY, TRUE);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, ret == MQX_TASKQ_CREATE_FAILED, "TEST #1: 1.11 _core_mutex_create_internal operation");
    /* Get the task error code */
    ret = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, ret == MQX_INVALID_PARAMETER, "TEST #1: 1.12 Checking the task error code");
    _task_set_error(MQX_OK);

    /* When creating task queue fails, _core_mutex_create() should return NULL */

    /* Create a core mutex */
    cm_ptr = _core_mutex_create( 0, 1, MQX_TASK_TEST_POLICY );
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, cm_ptr == NULL, "TEST #1: 1.13 _core_mutex_create operation");
    /* Get the task error code */
    ret = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, ret == MQX_INVALID_PARAMETER, "TEST #1: 1.14 Checking the task error code");
    _task_set_error(MQX_OK);

    /* When the core mutex pointer is NULL, _core_mutex_create_internal() should return MQX_INVALID_PARAMETER */

    /* Free memroy for core mutex */
    _mem_free(temp_cm_ptr);
    temp_cm_ptr = NULL;
    ret = _core_mutex_create_internal(temp_cm_ptr,0,1,MQX_TASK_QUEUE_FIFO, TRUE);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, ret == MQX_INVALID_PARAMETER, "TEST #1: 1.15 _core_mutex_create_internal operation");

    /* When _core_mutex_create() works normally, it should return non-NULL */

    /* Create a core mutex */
    cm_ptr = _core_mutex_create( 0, 1, MQX_TASK_QUEUE_FIFO);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, cm_ptr != NULL, "TEST #1: 1.16 _core_mutex_create operation");

    /* When the core mutex exists, _core_mutex_create_internal() should return MQX_COMPONENT_EXISTS */

    /* Create a core mutex */
    ret = _core_mutex_create_internal(cm_ptr, 0, 1, MQX_TASK_QUEUE_FIFO, TRUE);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, ret == MQX_COMPONENT_EXISTS, "TEST #1: 1.17 _core_mutex_create_internal operation");

    /* When the core mutex exists, _core_mutex_create() should return NULL */

    /* Create a core mutex */
    temp_cm_ptr = _core_mutex_create( 0, 1, MQX_TASK_QUEUE_FIFO);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, temp_cm_ptr == NULL, "TEST #1: 1.18 _core_mutex_create operation");

    /* Destroy the core mutex */
    ret = _core_mutex_destroy(cm_ptr);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, ret == COREMUTEX_OK, "TEST #1: 1.19 _core_mutex_destroy operation");
    /* Create a core mutex */
    cm_ptr = _core_mutex_create( 0, 1, MQX_TASK_QUEUE_FIFO);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, cm_ptr != NULL, "TEST #1: 1.20 _core_mutex_create operation");

    /* When the core mutex pointer is NULL, _core_mutex_lock() should return MQX_INVALID_POINTER */

    temp_cm_ptr = NULL;
    /* Lock the core mutex */
    ret = _core_mutex_lock(temp_cm_ptr);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, ret == MQX_INVALID_POINTER, "TEST #1: 1.21 _core_mutex_lock operation");

    /* When "VALID" of the core mutex is not equal to CORE_MUTEX_VALID, _core_mutex_lock() should return MQX_INVALID_POINTER */

    temp_cm_ptr = cm_ptr;
    temp_cm_ptr->VALID = CORE_MUTEX_VALID + 1;
    /* Lock the core mutex */
    ret = _core_mutex_lock(temp_cm_ptr);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, ret == MQX_INVALID_POINTER, "TEST #1: 1.22 _core_mutex_lock operation");

    /* When succeed to lock the core mutex, _core_mutex_lock() should return COREMUTEX_OK */

    cm_ptr->VALID = CORE_MUTEX_VALID;
    /* Lock the core mutex */
    ret = _core_mutex_lock(cm_ptr);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, ret == COREMUTEX_OK, "TEST #1: 1.23 _core_mutex_lock operation");

    /* Unlock the core emutex */
    ret = _core_mutex_unlock(cm_ptr);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, ret == COREMUTEX_OK, "TEST #1: 1.24 _core_mutex_unlock operation");
    /* Destroy the core mutex */
    ret = _core_mutex_destroy(cm_ptr);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, ret == COREMUTEX_OK, "TEST #1: 1.25 _core_mutex_destroy operation");
}

/*TASK*-----------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     :
*    This task mainly tests _core_mutex_destroy(),_core_mutex_trylock(),_core_mutex_unlock() and _core_mutex_owner()
* Requirements : CM024,CM025,CM026,CM027,CM028,CM038,CM039,CM040,CM043,CM044,CM045,CM047,CM048,CM049
*END*-----------------------------------------------------*/
void tc_2_main_task(void)
{
    CORE_MUTEX_PTR           cm_ptr, cm_ptr_temp;
    CORE_MUTEX_COMPONENT_PTR component_ptr;
    unsigned int             corenum = _psp_core_num();
    unsigned int             ret;

    //_DCACHE_DISABLE();

    /* Create a core mutex */
    cm_ptr = _core_mutex_create( 0, 1, MQX_TASK_QUEUE_FIFO );
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, cm_ptr != NULL, "TEST #2: 2.1 _core_mutex_create operation");
    /* Save the core mutex */
    cm_ptr_temp = cm_ptr;

    /* When the core mutex pointer is NULL, _core_mutex_trylock() should return MQX_INVALID_POINTER */

    cm_ptr = NULL;
    /* Lock the core mutex */
    ret = _core_mutex_trylock(cm_ptr);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, ret == MQX_INVALID_POINTER, "TEST #2: 2.2 _core_mutex_trylock operation");

    /* When the core mutex pointer is NULL, _core_mutex_unlock() should return MQX_INVALID_POINTER */

    /* Unlock the core mutex */
    ret = _core_mutex_unlock(cm_ptr);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, ret == MQX_INVALID_POINTER, "TEST #2: 2.3 _core_mutex_unlock operation");

    /* When the core mutex pointer is NULL, _core_mutex_owner() should return MQX_INVALID_POINTER */

    /* Get core id of mutex owner */
    ret = _core_mutex_owner(cm_ptr);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, ret == MQX_INVALID_POINTER, "TEST #2: 2.4 _core_mutex_owner operation");

    /* When the core mutex pointer is NULL, _core_mutex_destroy() should return MQX_INVALID_POINTER */

    /* Destroy the core mutex */
    ret = _core_mutex_destroy(cm_ptr);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, ret == MQX_INVALID_POINTER, "TEST #2: 2.5 _core_mutex_destroy operation");

    /* When "VALID" of the core mutex is not equal to CORE_MUTEX_VALID, _core_mutex_trylock() should return MQX_INVALID_POINTER */

    /* Restore the core mutex */
    cm_ptr = cm_ptr_temp;
    cm_ptr->VALID = 0;
    /* Lock the core mutex */
    ret = _core_mutex_trylock(cm_ptr);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, ret == MQX_INVALID_POINTER, "TEST #2: 2.6 _core_mutex_trylock operation");

    /* When "VALID" of the core mutex is not equal to CORE_MUTEX_VALID, _core_mutex_unlock() should return MQX_INVALID_POINTER */

    /* Unlock the core mutex */
    ret = _core_mutex_unlock(cm_ptr);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, ret == MQX_INVALID_POINTER, "TEST #2: 2.7 _core_mutex_unlock operation");

    /* When "VALID" of the core mutex is not equal to CORE_MUTEX_VALID, _core_mutex_owner() should return MQX_INVALID_POINTER */

    /* Get the core id of mutex owner */
    ret = _core_mutex_owner(cm_ptr);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, ret == MQX_INVALID_POINTER, "TEST #2: 2.8 _core_mutex_owner operation");

    /* When "VALID" of the core mutex is not equal to CORE_MUTEX_VALID, _core_mutex_destroy() should return MQX_INVALID_POINTER */

    /* Destroy the core mutex */
    ret = _core_mutex_destroy(cm_ptr);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, ret == MQX_INVALID_POINTER, "TEST #2: 2.9 _core_mutex_destroy operation");

    /* Set cm_ptr->VALID to 0 */
    cm_ptr->VALID = CORE_MUTEX_VALID;

    /* When succeed to lock the core mutex, _core_mutex_trylock() should return COREMUTEX_LOCKED */

    /* Lock the core mutex */
    ret = _core_mutex_trylock(cm_ptr);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, ret == COREMUTEX_LOCKED, "TEST #2: 2.10 _core_mutex_trylock operation");

    /* When succeed to get core id of the core mutex, _core_mutex_owner() should return corenum */

    /* Get the core id of the core mutex */
    ret = _core_mutex_owner(cm_ptr);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, ret == corenum, "TEST #2: 2.11 _core_mutex_owner operation");

    /* When succeed to unlock the core mutex, _core_mutex_unlock() should return COREMUTEX_OK */

    /* Unlock the core mutex */
    ret = _core_mutex_unlock(cm_ptr);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, ret == COREMUTEX_OK, "TEST #2: 2.12 _core_mutex_unlock operation");

    /* When the component doesn't exist, _core_mutex_destroy() should return MQX_COMPONENT_DOES_NOT_EXIST */

    /* Get the component pointer */
    component_ptr = _core_mutext_get_component_ptr();
    /* Set the component pointer to NULL */
    _core_mutext_set_component_ptr(NULL);
    /* Destroy the core mutex */
    ret = _core_mutex_destroy(cm_ptr);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, ret == MQX_COMPONENT_DOES_NOT_EXIST, "TEST #2: 2.13 _core_mutex_destroy operation");
    /* Restore the component pointer */
    _core_mutext_set_component_ptr(component_ptr);

    /* When component_ptr->DEVICE[0].MUTEX_PTR[mutex_num] is NULL, _core_mutex_destroy() should return MQX_COMPONENT_DOES_NOT_EXIST */

    /* Set MUTEX_PTR[mutex_num] to NULL */
    component_ptr->DEVICE[0].MUTEX_PTR[cm_ptr->GATE_NUM] = NULL;
    /* Destroy the core mutex */
    ret = _core_mutex_destroy(cm_ptr);
    component_ptr->DEVICE[0].MUTEX_PTR[cm_ptr->GATE_NUM] = cm_ptr;
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, ret == MQX_COMPONENT_DOES_NOT_EXIST, "TEST #2: 2.14 _core_mutex_destroy operation");

    /* When _core_mutex_destroy() works normally, it should return COREMUTEX_OK */

    /* Destroy the core mutex */
    ret = _core_mutex_destroy(cm_ptr);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, ret == COREMUTEX_OK, "TEST #2: 2.15 _core_mutex_destroy operation");
}

/*TASK*-----------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     :
*    This task mainly tests _core_mutex_create_at() and _core_mutex_get().
* Requirements : CM017,CM018,CM019,CM020,CM021,CM022,CM023,CM029,CM030,CM031,CM032
*END*-----------------------------------------------------*/
void tc_3_main_task(void)
{
    CORE_MUTEX_PTR           cm_ptr, temp_cm_ptr;
    CORE_MUTEX_COMPONENT_PTR component_ptr;
    unsigned int             ret;

    //_DCACHE_DISABLE();

    /* Create a core mutex */
    cm_ptr = _core_mutex_create( 0, 1, MQX_TASK_QUEUE_FIFO );
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, cm_ptr != NULL, "TEST #3: 3.1 _core_mutex_create operation");

    /* When component doesn't exist, _core_mutex_get() should return MQX_COMPONENT_NOT_EXIST. */
    _task_set_error(MQX_OK);
    /* Get the component pointer */
    component_ptr = _core_mutext_get_component_ptr();
    /* Set the component pointer to NULL */
    _core_mutext_set_component_ptr(NULL);
    /* Get the core mutex poniter */
    temp_cm_ptr = _core_mutex_get(0,1);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, temp_cm_ptr == NULL, "TEST #3: 3.2 _core_mutex_get operation");
    /* Get the task error code */
    ret = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, ret == MQX_COMPONENT_DOES_NOT_EXIST, "TEST #3: 3.3 _task_get_error operation");
    _task_set_error(MQX_OK);

    /* When dev_num is greater than or equal to SEMA4_NUM_DEVICES, _core_mute_get() should return MQX_INVALID_PARAMETER */

    /* Restore the component pointer */
    _core_mutext_set_component_ptr(component_ptr);
    /* Get the core mutex */
    temp_cm_ptr = _core_mutex_get(SEMA4_NUM_DEVICES+1,1);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, temp_cm_ptr == NULL, "TEST #3: 3.4 _core_mutex_get operation");
    /* Get the task error code */
    ret = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, ret == MQX_INVALID_PARAMETER, "TEST #3: 3.5 _task_get_error operation");
    _task_set_error(MQX_OK);

    /* When mutex_num is greater than or equal to SEMA4_NUM_GATES, _core_mutex_get should return MQX_INVALID_PARAMETER */

    /* Get the core mutex */
    temp_cm_ptr = _core_mutex_get(0,16);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, temp_cm_ptr == NULL, "TEST #3: 3.6 _core_mutex_get operation");
    /* Get the task error code */
    ret = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, ret == MQX_INVALID_PARAMETER, "TEST #3: 3.7 _task_get_error operation");
    _task_set_error(MQX_OK);

    /* When _core_mutex_get() work normally, it should return a core mutex pointer */

    /* Get the core mutex */
    temp_cm_ptr = _core_mutex_get(0,1);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, temp_cm_ptr == cm_ptr, "TEST #3: 3.8 _core_mutex_get operation");

    /* Destroy the core mutex */
    ret = _core_mutex_destroy(cm_ptr);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, ret == COREMUTEX_OK, "TEST #3: 3.9 _core_mutex_destroy operation");

    /* Alloc memory for core mutex */
    cm_ptr = _mem_alloc_system_zero(sizeof(*cm_ptr));
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, cm_ptr != NULL, "TEST #3: 3.10 _mem_alloc_system_zero operation");

    /* When the component doesn't exist, _core_mutex_create_at() should return MQX_COMPONENT_DOES_NOT_EXIST */

    /* Get the component pointer */
    component_ptr = _core_mutext_get_component_ptr();
    /* Set the component pointer to NULL */
    _core_mutext_set_component_ptr(NULL);
    /* Create a core mutex */
    ret = _core_mutex_create_at(temp_cm_ptr, 0, 1, MQX_TASK_QUEUE_FIFO );
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, ret == MQX_COMPONENT_DOES_NOT_EXIST, "TEST #3: 3.11 _core_mutex_create_at operation");

    /* When the core mutex pointer is NULL, _core_mutex_create_at() should return MQX_INVALID_PARAMETER */

    /* Restore the component pointer */
    _core_mutext_set_component_ptr(component_ptr);
    /* Set the core mutex pointer to NULL */
    temp_cm_ptr = NULL;
    /* Create a core mutex */
    ret = _core_mutex_create_at(temp_cm_ptr, 0, 1, MQX_TASK_QUEUE_FIFO);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, ret == MQX_INVALID_PARAMETER, "TEST #3: 3.12 _core_mutex_create_at operation");

    /* When dev_num is greater than or equal to SEMA4_NUM_DEVICES, _core_mutex_create_at() should return MQX_INVALID_PARAMETER */

    /* Create a cor emutex */
    ret = _core_mutex_create_at(cm_ptr, SEMA4_NUM_DEVICES+1, 1, MQX_TASK_QUEUE_FIFO);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, ret == MQX_INVALID_PARAMETER, "TEST #3: 3.13 _core_mutex_create_at operation");

    /* When mutex_num is greater than or equal to SEMA4_NUM_GATES, _core_mutex_create_at() should return MQX_INVALID_PARAMETER */

    /* Create a core mutex */
    ret = _core_mutex_create_at(cm_ptr, 0, 16, MQX_TASK_QUEUE_FIFO);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, ret == MQX_INVALID_PARAMETER, "TEST #3: 3.14 _core_mutex_create_at operation");

    /* When creating task queue fails, _core_mutex_create_at() should return MQX_TASKQ_CREATE_FAILED */

    /* Create a core mutex */
    ret = _core_mutex_create_at(cm_ptr, 0, 1, MQX_TASK_TEST_POLICY);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, ret == MQX_TASKQ_CREATE_FAILED, "TEST #3: 3.15 _core_mutex_create_at operation");
    /* Get the task error code */
    ret = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, ret == MQX_INVALID_PARAMETER, "TEST #3: 3.16 _core_mutex_lock operation");
    _task_set_error(MQX_OK);

    /* When _core_mutex_create_at() works normally, it should return COREMUTEX_OK */

    /* Create a core mutex */
    ret = _core_mutex_create_at(cm_ptr, 0, 1, MQX_TASK_QUEUE_FIFO);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, ret == COREMUTEX_OK, "TEST #3: 3.17 _core_mutex_create_at operation");

    /* when the core mutex has been created, _core_mutex_create_at() should return MQX_COMPONENT_EXISTS */

    /* Create a core mutex */
    ret = _core_mutex_create_at(cm_ptr, 0, 1, MQX_TASK_QUEUE_FIFO);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, ret == MQX_COMPONENT_EXISTS, "TEST #3: 3.18 _core_mutex_create_at operation");

    /* Destroy the core mutex*/
    ret = _core_mutex_destroy(cm_ptr);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, ret == COREMUTEX_OK, "TEST #3: 3.19 _core_mutex_create operation");
}

/*TASK*-----------------------------------------------------
*
* Task Name    : tc_4_main_task
* Comments     :
*    This task mainly tests _core_mutex_install().
* Requirements : CM001,CM002,CM003,CM011
*END*-----------------------------------------------------*/
void tc_4_main_task(void)
{
    CORE_MUTEX_PTR           cm_ptr;
    unsigned int             ret;
    MEMORY_ALLOC_INFO_STRUCT memory_alloc_info;

    //_DCACHE_DISABLE();

    /* When the component exists, _core_mutex_install() should return MQX_COMPONENT_EXIST */

    /* Install core mutex */
    ret = _core_mutex_install(&_core_mutex_init_info);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, ret == MQX_COMPONENT_EXISTS, "TEST #4: 4.1 _core_mutex_install operation");

    /* When succeed to install core mutex, _core_mutex_install() should return COREMUTEX_OK */

    /* Uninsatll the core mutex */
    ret = core_mutex_uninstall();
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, ret == COREMUTEX_OK, "TEST #4: 4.2 core_mutex_uninsatll operation");
    /* Install the core mutex */
    ret = _core_mutex_install(&_core_mutex_init_info);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, ret == COREMUTEX_OK, "TEST #4: 4.3 _core_mutex_install operation");

    /* Create a core mutex */
    cm_ptr = _core_mutex_create( 0, 1, MQX_TASK_QUEUE_FIFO );
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, cm_ptr != NULL, "TEST #4: 4.4 _core_mutex_create operation");

    /* Destroy the core mutex */
    ret = _core_mutex_destroy(cm_ptr);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, ret == COREMUTEX_OK, "TEST #4: 4.5 _core_mutex_destroy operation");

    /* When allocing memory fails, _core_mutex_install() should return MQX_OUT_OF_MEMORY */

    /* Uninstall the core mutex */
    ret = core_mutex_uninstall();
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, ret == COREMUTEX_OK, "TEST #4: 4.6 core_mutex_uninstall operation");
    /* Alloc all available memory */
    memory_alloc_all(&memory_alloc_info);
    /* Install the core mutex */
    ret = _core_mutex_install(&_core_mutex_init_info);
    /* Free the memory */
    memory_free_all(&memory_alloc_info);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, ret == MQX_OUT_OF_MEMORY, "TEST #4: 4.7 _core_mutex_install operation");
    /* Install the core mutex */
    ret = _core_mutex_install(&_core_mutex_init_info);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, ret == COREMUTEX_OK, "TEST #4: 4.8 _core_mutex_install operation");

    /* When allocing memory fails, _core_mutex_create() should return MQX_OUT_OF_MEMORY*/

    /* Alloc all available memory */
    memory_alloc_all(&memory_alloc_info);
    /* Create a core mutex */
    cm_ptr = _core_mutex_create( 0, 1, MQX_TASK_QUEUE_FIFO );
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, cm_ptr == NULL, "TEST #4: 4.9 _core_mutex_create operation");
    /* Get the task error code */
    ret = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, ret == MQX_OUT_OF_MEMORY, "TEST #4: 4.10 _core_mutex_create operation");
    /* Free the memory */
    memory_free_all(&memory_alloc_info);
}

/* Define Test Suite */
EU_TEST_SUITE_BEGIN(suite_coremutex)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST #1 - Mainly test _core_mutex_create(), _core_mutex_create_internal() and _core_mutex_lock()"),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST #2 - Mainly test _core_mutex_destroy(),_core_mutex_trulock(), _core_mutex_unlock() and _core_mutex_owner()"),
    EU_TEST_CASE_ADD(tc_3_main_task, " TEST #3 - Mainly test _core_mutex_create_at() and _core_mutex_get()"),
    EU_TEST_CASE_ADD(tc_4_main_task, " TEST #4 - Mainly test _core_mutex_install()")
EU_TEST_SUITE_END(suite_coremutex)
/* Add test suite */
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_coremutex, " - coremutex suite")
EU_TEST_REGISTRY_END()

void main_task(unsigned int param)
{
    eunit_mutex_init();
    EU_RUN_ALL_TESTS(eunit_tres_array);
    eunit_test_stop();
}

unsigned int core_mutex_uninstall(void)
{
    CORE_MUTEX_COMPONENT_PTR    component_ptr = _core_mutext_get_component_ptr();
    PSP_INTERRUPT_TABLE_INDEX   vector;
    unsigned int                i;

    for ( i = 0; i < SEMA4_NUM_DEVICES; i ++)
    {
        vector = _bsp_get_sema4_vector(i);
        _int_install_isr(vector, NULL, &component_ptr->DEVICE[i]);
    }
    /* Set the component pointer to NULL */
    _core_mutext_set_component_ptr(NULL);
    /* Free the requested memory for the component */
    _mem_free(component_ptr);
    return COREMUTEX_OK;
}

