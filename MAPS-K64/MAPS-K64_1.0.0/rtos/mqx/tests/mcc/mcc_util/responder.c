/**HEADER*****************************************************************
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
* $FileName: responder.c$
* $Version : 4.0.2$
* $Date    : Aug-30-2013$
*
* Comments:
*
*   This file contains the source for responder to test application.
*
*END************************************************************************/

#if  MCC_TEST_APP == 0

#include "common.h"

#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif

/* Function declarations */
void responder_task(unsigned int dummy);
void make_err(char flag);
void de_make_err(char flag);

const TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
    /* Task Index,  Function,       Stack,  Priority,  Name,         Attributes,           Param,  Time Slice */
    { RESP_TASK,   responder_task, 10000,  10,         "Responder",  MQX_AUTO_START_TASK,  0,      0 },
    { 0 }
};

/*TASK*----------------------------------------------------------
*
* Task Name : responder_task
* Comments  :
*     This task creates a message queue then waits for a message.
*     Upon receiving the message the data is incremented before
*     the message is returned to the sender.
*END*-----------------------------------------------------------*/
void responder_task(unsigned int dummy)
{
    CONTROL_MESSAGE     msg;
    ACKNOWLEDGE_MESSAGE ack_msg;
    MCC_MEM_SIZE        num_of_received_bytes,num_of_received_control_bytes;
    CORE_MUTEX_PTR      coremutex_app_ptr;
    MCC_INFO_STRUCT     mcc_info;
    int                 ret_value, i;
    CONTROL_MESSAGE_DATA_CREATE_EP_PARAM_PTR    data_create_ep_param;
    CONTROL_MESSAGE_DATA_DESTROY_EP_PARAM_PTR   data_destroy_ep_param;
    CONTROL_MESSAGE_DATA_SEND_PARAM_PTR         data_send_param;
    CONTROL_MESSAGE_DATA_RECV_PARAM_PTR         data_recv_param;
    CONTROL_MESSAGE_DATA_MSG_AVAIL_PARAM_PTR    data_msg_avail_param;
    CONTROL_MESSAGE_DATA_GET_INFO_PARAM_PTR     data_get_info_param;
    CONTROL_MESSAGE_DATA_FREE_BUF_PARAM_PTR     data_free_buf_param;
    char*               uut_app_buffer_ptr[255];
    char*               uut_temp_buffer_ptr;
    TIME_STRUCT         uut_time;
    char*               tmp_pointer = null;
    unsigned int        err_code;
    MCC_SIGNAL          signal_t;
    MCC_RECEIVE_LIST    *list_S, *list_E;
#if PRINT_ON
    /* create core mutex used in the app. for accessing the serial console */
    coremutex_app_ptr = _core_mutex_create( 0, 2, MQX_TASK_QUEUE_FIFO );
#endif
    bookeeping_data = (MCC_BOOKEEPING_STRUCT *)MCC_BASE_ADDRESS;
    ret_value = mcc_initialize(MCC_UUT_NODE);
    ret_value = mcc_get_info(MCC_UUT_NODE, &mcc_info);
    if(MCC_SUCCESS == ret_value && (strcmp(mcc_info.version_string, MCC_VERSION_STRING) != 0)) {
#if PRINT_ON
        _core_mutex_lock(coremutex_app_ptr);
        printf("\n\n\nError, attempting to use different versions of the MCC library on each core! Application is stopped now.\n");
        _core_mutex_unlock(coremutex_app_ptr);
#endif
        mcc_destroy(MCC_UUT_NODE);
        _task_block();
    }
    ret_value = mcc_create_endpoint(&uut_control_endpoint, uut_control_endpoint.port);
#if PRINT_ON
    _core_mutex_lock(coremutex_app_ptr);
    printf("\n\n\nResponder task started, MCC version is %s\n", mcc_info.version_string);
    _core_mutex_unlock(coremutex_app_ptr);
#endif
    while (TRUE) {
        ret_value = mcc_recv_copy(&uut_control_endpoint, &msg, sizeof(CONTROL_MESSAGE), &num_of_received_control_bytes, 0xffffffff);
        if(MCC_SUCCESS != ret_value) {
#if PRINT_ON
            _core_mutex_lock(coremutex_app_ptr);
            printf("Responder task receive error: %i\n", ret_value);
            _core_mutex_unlock(coremutex_app_ptr);
#endif
        } else {
#if PRINT_ON
            _core_mutex_lock(coremutex_app_ptr);
            printf("Responder task received a msg\n");
            printf("Message: Size=0x%x, CMD = 0x%x, DATA = 0x%x 0x%x 0x%x 0x%x\n", num_of_received_control_bytes, msg.CMD, msg.DATA[0], msg.DATA[1], msg.DATA[2], msg.DATA[3]);
            _core_mutex_unlock(coremutex_app_ptr);
#endif
            if(num_of_received_control_bytes < 10) {
                num_of_received_control_bytes++;
            }
            switch(msg.CMD){
            case CTR_CMD_CREATE_EP:
                data_create_ep_param = (CONTROL_MESSAGE_DATA_CREATE_EP_PARAM_PTR)&msg.DATA;
                /* Create conditions to make cmd fail */
                make_err(msg.flag);
                /* Create the specified endpoint */
                ret_value = mcc_create_endpoint(&data_create_ep_param->uut_endpoint, data_create_ep_param->uut_endpoint.port);
                if(MCC_SUCCESS == ret_value) {
                    tmp_pointer = _mem_alloc(data_create_ep_param->uut_app_buffer_size);
                    if(tmp_pointer != null) {
                        uut_app_buffer_ptr[data_create_ep_param->uut_endpoint.port] = tmp_pointer;
                        mcc_memcpy("MEM_OK", (void*)ack_msg.RESP_DATA, 7);
                    }
                    else {
                        _mem_free(tmp_pointer);
                        mcc_memcpy("MEM_KO", (void*)ack_msg.RESP_DATA, 7);
    #if PRINT_ON
                        printf("_mem_alloc failure\n");
    #endif
                    }
                }
                /* Eliminate the effect from conditions */
                de_make_err(msg.flag);
                if(ACK_REQUIRED_YES == msg.ACK_REQUIRED) {
                    ack_msg.CMD_ACK = CTR_CMD_CREATE_EP;
                    ack_msg.RETURN_VALUE = ret_value;
                    /* Send ack_msg to tea_control_endpoint */
                    ret_value = mcc_send(&data_create_ep_param->endpoint_to_ack, &ack_msg, sizeof(ACKNOWLEDGE_MESSAGE), 0xFFFFFFFF);
                }
                break;
            case CTR_CMD_RECV:
                data_recv_param = (CONTROL_MESSAGE_DATA_RECV_PARAM_PTR)&msg.DATA;
                /* Create conditions to make cmd fail. */
                make_err(msg.flag);
                /* Use mcc_recv_copy in CMD_RECV_MODE_COPY mode */
                if(CMD_RECV_MODE_COPY == data_recv_param->mode) {
                    if(0xffffffff == data_recv_param->timeout_us) {
                        ret_value = mcc_recv_copy(&data_recv_param->uut_endpoint, uut_app_buffer_ptr[data_recv_param->uut_endpoint.port], data_recv_param->uut_app_buffer_size, &num_of_received_bytes, data_recv_param->timeout_us);
                    }
                    else if(0 == data_recv_param->timeout_us) {
                        do {
                            ret_value = mcc_recv_copy(&data_recv_param->uut_endpoint, uut_app_buffer_ptr[data_recv_param->uut_endpoint.port], data_recv_param->uut_app_buffer_size, &num_of_received_bytes, data_recv_param->timeout_us);
                        } while(MCC_ERR_TIMEOUT == ret_value);
                    }
                    else {
                        _time_get(&uut_time);
                        ack_msg.TS1_SEC = uut_time.SECONDS;
                        ack_msg.TS1_MSEC = uut_time.MILLISECONDS;
                        ret_value = mcc_recv_copy(&data_recv_param->uut_endpoint, uut_app_buffer_ptr[data_recv_param->uut_endpoint.port], data_recv_param->uut_app_buffer_size, &num_of_received_bytes, data_recv_param->timeout_us);
                        _time_get(&uut_time);
                        ack_msg.TS2_SEC = uut_time.SECONDS;
                        ack_msg.TS2_MSEC = uut_time.MILLISECONDS;
                    }
                }
                /* Use mcc_recv_copy in CMD_RECV_MODE_COPY mode */
                else if(CMD_RECV_MODE_NOCOPY == data_recv_param->mode) {
                    if(0xffffffff == data_recv_param->timeout_us) {
                        ret_value = mcc_recv_nocopy(&data_recv_param->uut_endpoint, (void**)&uut_temp_buffer_ptr, &num_of_received_bytes, data_recv_param->timeout_us);
                    }
                    else if(0 == data_recv_param->timeout_us) {
                        do {
                            ret_value = mcc_recv_nocopy(&data_recv_param->uut_endpoint, (void**)&uut_temp_buffer_ptr, &num_of_received_bytes, data_recv_param->timeout_us);
                        } while(MCC_ERR_TIMEOUT == ret_value);
                    }
                    else {
                        _time_get(&uut_time);
                        ack_msg.TS1_SEC = uut_time.SECONDS;
                        ack_msg.TS1_MSEC = uut_time.MILLISECONDS;
                        ret_value = mcc_recv_nocopy(&data_recv_param->uut_endpoint, (void**)&uut_temp_buffer_ptr, &num_of_received_bytes, data_recv_param->timeout_us);
                        _time_get(&uut_time);
                        ack_msg.TS2_SEC = uut_time.SECONDS;
                        ack_msg.TS2_MSEC = uut_time.MILLISECONDS;
                    }
                    if(MCC_SUCCESS == ret_value) {
                        mcc_memcpy((void*)uut_temp_buffer_ptr, (void*)uut_app_buffer_ptr[data_recv_param->uut_endpoint.port], (unsigned int)num_of_received_bytes);
                        mcc_free_buffer(uut_temp_buffer_ptr);
                    }
                }
                /* Eliminate the effect from conditions */
                de_make_err(msg.flag);
                if(ACK_REQUIRED_YES == msg.ACK_REQUIRED) {
                    ack_msg.CMD_ACK = CTR_CMD_RECV;
                    ack_msg.RETURN_VALUE = ret_value;
                    /* Send ack_msg to tea_control_endpoint */
                    ret_value = mcc_send(&data_recv_param->endpoint_to_ack, &ack_msg, sizeof(ACKNOWLEDGE_MESSAGE), 0xFFFFFFFF);
                }
                break;
            case CTR_CMD_SEND:
                data_send_param = (CONTROL_MESSAGE_DATA_SEND_PARAM_PTR)&msg.DATA;
                /* Create conditions to make cmd fail. */
                make_err(msg.flag);
                /* Make signal queue full */
                if(msg.flag == 4){
                    signal_t.destination = data_send_param->dest_endpoint;
                    for(i = 0; i < MCC_MAX_OUTSTANDING_SIGNALS + 1; i++){
                        signal_t.type = i;
                        mcc_queue_signal(data_send_param->dest_endpoint.core, signal_t);
                    }
                    list_S = mcc_get_endpoint_list(data_send_param->dest_endpoint);
                }
                for(i=0; i<data_send_param->repeat_count; i++) {
                    if((0 != data_send_param->timeout_us) && (0xffffffff != data_send_param->timeout_us)) {
                        _time_get(&uut_time);
                        ack_msg.TS1_SEC = uut_time.SECONDS;
                        ack_msg.TS1_MSEC = uut_time.MILLISECONDS;
                    }
                    if(0 == strncmp((const char *)data_send_param->msg, "", 1)) {
                        /* send the content of the uut_app_buffer_ptr */
                        ret_value = mcc_send(&data_send_param->dest_endpoint, uut_app_buffer_ptr[data_send_param->uut_endpoint.port], data_send_param->msg_size, data_send_param->timeout_us);
                    }
                    else {
                        /* send what received in the control command */
                        ret_value = mcc_send(&data_send_param->dest_endpoint, data_send_param->msg, data_send_param->msg_size, data_send_param->timeout_us);
                    }
                    if((0 != data_send_param->timeout_us) && (0xffffffff != data_send_param->timeout_us)) {
                        _time_get(&uut_time);
                        ack_msg.TS2_SEC = uut_time.SECONDS;
                        ack_msg.TS2_MSEC = uut_time.MILLISECONDS;
                    }
                }
                /* Eliminate the effect from conditions */
                de_make_err(msg.flag);
                if(ACK_REQUIRED_YES == msg.ACK_REQUIRED) {
                    ack_msg.CMD_ACK = CTR_CMD_SEND;
                    ack_msg.RETURN_VALUE = ret_value;
                    /* Release one buffer for mcc_send() on the uut side */
                    if(msg.flag == 3){
                        mcc_recv_copy(&data_send_param->dest_endpoint, &uut_app_buffer_ptr, UUT_APP_BUF_SIZE, &num_of_received_bytes, 0xffffffff);
                    }
                    /* Release one signal for mcc_send() on the uut side */
                    else if(msg.flag == 4){
                        list_E = mcc_get_endpoint_list(data_send_param->dest_endpoint);
                        if((list_S->head == list_E->head) && (list_S->tail == list_E->tail)){
                            ack_msg.RETURN_VALUE = ~ack_msg.RETURN_VALUE;
                        }
                        signal_t.destination = data_send_param->dest_endpoint;
                        signal_t.type = BUFFER_QUEUED;
                        mcc_dequeue_signal(data_send_param->dest_endpoint.core, &signal_t);
                     }
                    /* Send ack_msg to tea_control_endpoint*/
                    ret_value = mcc_send(&data_send_param->endpoint_to_ack, &ack_msg, sizeof(ACKNOWLEDGE_MESSAGE), 0xFFFFFFFF);
                }
                break;
            case CTR_CMD_MSG_AVAIL:
                data_msg_avail_param = (CONTROL_MESSAGE_DATA_MSG_AVAIL_PARAM_PTR)&msg.DATA;
                /* Create conditions to make cmd fail */
                make_err(msg.flag);
                ret_value = mcc_msgs_available(&data_msg_avail_param->uut_endpoint, (unsigned int*)uut_app_buffer_ptr[data_msg_avail_param->uut_endpoint.port]);
                /* Eliminate the effect from conditions */
                de_make_err(msg.flag);
                if(ACK_REQUIRED_YES == msg.ACK_REQUIRED) {
                    ack_msg.CMD_ACK = CTR_CMD_MSG_AVAIL;
                    ack_msg.RETURN_VALUE = ret_value;
                    if(MCC_SUCCESS == ret_value) {
                        /* Avoid to using mcc_memcpy() when EP port number < 0 */
                        if(msg.flag != 2){
                            mcc_memcpy((void*)uut_app_buffer_ptr[data_msg_avail_param->uut_endpoint.port], (void*)ack_msg.RESP_DATA, sizeof(unsigned int));
                        }
                    }
                    /* Send ack_msg to tea_control_endpoint */
                    ret_value = mcc_send(&data_msg_avail_param->endpoint_to_ack, &ack_msg, sizeof(ACKNOWLEDGE_MESSAGE), 0xFFFFFFFF);
                }
                break;
            case CTR_CMD_DESTROY_EP:
                data_destroy_ep_param = (CONTROL_MESSAGE_DATA_DESTROY_EP_PARAM_PTR)&msg.DATA;
                /* Create conditions to make cmd fail */
                make_err(msg.flag);
                /* Destroy the specified endpoint */
                ret_value = mcc_destroy_endpoint(&data_destroy_ep_param->uut_endpoint);
                if(MCC_SUCCESS == ret_value) {
                    /* Free the memory allocated in CTR_CMD_CREATE_EP */
                    err_code = _mem_free((void*)uut_app_buffer_ptr[data_destroy_ep_param->uut_endpoint.port]);
                    if(MQX_OK != err_code) {
#if PRINT_ON
                        printf("_mem_free failure\n");
#endif
                        mcc_memcpy("MEM_KO", (void*)ack_msg.RESP_DATA, 7);
                    }
                    else {
                        mcc_memcpy("MEM_OK", (void*)ack_msg.RESP_DATA, 7);
                    }
                }
                /* Eliminate the effect from conditions */
                de_make_err(msg.flag);
                if(ACK_REQUIRED_YES == msg.ACK_REQUIRED) {
                    ack_msg.CMD_ACK = CTR_CMD_DESTROY_EP;
                    ack_msg.RETURN_VALUE = ret_value;
                    /* Send ack_msg to tea_control_endpoint */
                    ret_value = mcc_send(&data_destroy_ep_param->endpoint_to_ack, &ack_msg, sizeof(ACKNOWLEDGE_MESSAGE), 0xFFFFFFFF);
                }
                break;
            case CTR_CMD_GET_INFO:
                data_get_info_param = (CONTROL_MESSAGE_DATA_GET_INFO_PARAM_PTR)&msg.DATA;
                /* Create conditions to make cmd fail*/
                make_err(msg.flag);
                /* Get information of the node and store them into ack_msg struct */
                ret_value = mcc_get_info(data_get_info_param->uut_endpoint.node, (MCC_INFO_STRUCT*)&ack_msg.RESP_DATA);
                /* Eliminate the effect from conditions */
                de_make_err(msg.flag);
                if(ACK_REQUIRED_YES == msg.ACK_REQUIRED) {
                    ack_msg.CMD_ACK = CTR_CMD_GET_INFO;
                    ack_msg.RETURN_VALUE = ret_value;
                    /* Send sck_msg to tea_control_nedpoint */
                    ret_value = mcc_send(&data_get_info_param->endpoint_to_ack, &ack_msg, sizeof(ACKNOWLEDGE_MESSAGE), 0xFFFFFFFF);
                }
                break;
            case CTR_CMD_FREE_BUF:
                data_free_buf_param = (CONTROL_MESSAGE_DATA_FREE_BUF_PARAM_PTR)&msg.DATA;
                data_free_buf_param->free_buf = uut_temp_buffer_ptr;
                /* Create conditions to make cmd fail. */
                make_err(msg.flag);
                /* Free the buffer */
                ret_value = mcc_free_buffer(data_free_buf_param->free_buf);
                /* Eliminate the effect from conditions */
                de_make_err(msg.flag);
                if (ACK_REQUIRED_YES == msg.ACK_REQUIRED){
                    ack_msg.CMD_ACK = CTR_CMD_FREE_BUF;
                    ack_msg.RETURN_VALUE = ret_value;
                    /* Send sck_msg to tea_control_nedpoint */
                    ret_value = mcc_send(&data_free_buf_param->endpoint_to_ack, &ack_msg, sizeof(ACKNOWLEDGE_MESSAGE), 0xFFFFFFFF);
                }
                break;
            }
        }
    }
}

void make_err( char flag){
    if (flag == 1){
    /* De-initialize the semaphore */
    mcc_deinit_semaphore(MCC_SHMEM_SEMAPHORE_NUMBER);
    }
    else if (flag == 5){
    /* Initialize the semaphore */
    mcc_init_semaphore(MCC_SHMEM_SEMAPHORE_NUMBER);
    }
}

void de_make_err(char flag){
    if (flag == 1){
    /* Initialize the semaphore for mcc_send() */
    mcc_init_semaphore(MCC_SHMEM_SEMAPHORE_NUMBER);
    }
    else if (flag == 5){
    /* Get mutex pointer corresponding to MCC_SHMEM_SEMAPHORE_NUMBER */
    CORE_MUTEX_PTR    mutex_ptr = _core_mutex_get(0,MCC_SHMEM_SEMAPHORE_NUMBER);
    /* Destroy the mutex */
    _core_mutex_destroy(mutex_ptr);
    /* Initialize the semaphore for mcc_send() */
    mcc_init_semaphore(MCC_SHMEM_SEMAPHORE_NUMBER);
    }
}
#endif
