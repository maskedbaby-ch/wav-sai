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
* $FileName: test.c$
* $Version : 4.0.2$
* $Date    : Aug-30-2013$
*
* Comments:
*
*   This file contains the source for test of the MCC.
*
*END************************************************************************/
#if  MCC_TEST_APP == 1

#include "common.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"
#include "util.h"

#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif

/* Function declarations */
void main_task(unsigned int param);
void tc_1_main_task(void);/* TEST #1 - Testing MCC function when not successfully grap the hw semaphore */
void tc_2_main_task(void);/* TEST #2 - Testing MCC buffer and signal queue. */
void tc_3_main_task(void);/* TEST #3 - Testing MCC deinitialization. */
void tc_4_main_task(void);/* TEST #4 - Testing MCC initialization when inter-core hw semaphore has not been correctly initialized. */

const TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
    /* Task Index,  Function,       Stack,  Priority,  Name,         Attributes,           Param,  Time Slice */
    { MAIN_TASK,   main_task,      2000,   10,        "Main",       MQX_AUTO_START_TASK,  0,      0 },
    { 0 }
};

CORE_MUTEX_PTR  coremutex_app_ptr;
unsigned int    msg_counter = 0;

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Testing MCC function when not successfully grap the hw semaphore
*
* Requirement  : MCC015,MCC016,MCC017,MCC022,MCC031,MCC037,MCC043,MCC048,MCC050
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    CONTROL_MESSAGE     msg;
    ACKNOWLEDGE_MESSAGE ack_msg;
    MCC_MEM_SIZE        num_of_received_bytes;
    int                 ret_value;
    CONTROL_MESSAGE_DATA_CREATE_EP_PARAM        data_create_ep_param;
    CONTROL_MESSAGE_DATA_DESTROY_EP_PARAM       data_destroy_ep_param;
    CONTROL_MESSAGE_DATA_MSG_AVAIL_PARAM        data_msg_avail_param;
    CONTROL_MESSAGE_DATA_RECV_PARAM             data_recv_param;
    CONTROL_MESSAGE_DATA_SEND_PARAM             data_send_param;
    CONTROL_MESSAGE_DATA_GET_INFO_PARAM         data_get_info_param;
    CONTROL_MESSAGE_DATA_FREE_BUF_PARAM         data_free_buf_param;
    MCC_ENDPOINT        uut_endpoint = {MCC_UUT_CORE,MCC_UUT_NODE,MCC_UUT_EP_PORT};
    MCC_ENDPOINT        tea_endpoint = {MCC_TEA_CORE,MCC_TEA_NODE,MCC_TEA_EP_PORT};

    data_create_ep_param.uut_endpoint = uut_endpoint;
    data_create_ep_param.uut_app_buffer_size = UUT_APP_BUF_SIZE;
    data_create_ep_param.endpoint_to_ack = tea_control_endpoint;

    data_destroy_ep_param.uut_endpoint = uut_endpoint;
    data_destroy_ep_param.endpoint_to_ack = tea_control_endpoint;

    data_msg_avail_param.uut_endpoint = uut_endpoint;
    data_msg_avail_param.endpoint_to_ack = tea_control_endpoint;

    data_recv_param.uut_endpoint = uut_endpoint;
    data_recv_param.uut_app_buffer_size = UUT_APP_BUF_SIZE;
    data_recv_param.timeout_us = 0xffffffff;
    data_recv_param.mode = CMD_RECV_MODE_COPY;
    data_recv_param.endpoint_to_ack = tea_control_endpoint;

    data_send_param.dest_endpoint = tea_endpoint;
    mcc_memcpy("",(void*)data_send_param.msg,(unsigned int)4);
    data_send_param.msg_size = CMD_SEND_MSG_SIZE;
    data_send_param.timeout_us = 0xffffffff;
    data_send_param.uut_endpoint = uut_endpoint;
    data_send_param.endpoint_to_ack = tea_control_endpoint;
    data_send_param.repeat_count = 1;

    data_get_info_param.uut_endpoint = uut_endpoint;
    data_get_info_param.endpoint_to_ack = tea_control_endpoint;

    data_free_buf_param.free_buf = NULL;
    data_free_buf_param.endpoint_to_ack = tea_control_endpoint;

    msg.flag = 0;
#if PRINT_ON
    /* create core mutex used in the app. for accessing the serial console */
    coremutex_app_ptr = _core_mutex_create( 0, 2, MQX_TASK_QUEUE_FIFO );
#endif
    bookeeping_data = (MCC_BOOKEEPING_STRUCT *)MCC_BASE_ADDRESS;
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, bookeeping_data != NULL, "TEST #1: 1.1 Bookeeping_data is not NULL");
    if(bookeeping_data == NULL)
        eunit_test_stop();

    ret_value = mcc_initialize(MCC_TEA_NODE);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ret_value == MCC_SUCCESS, "TEST #1: 1.2 Testing mcc_initialize");
    if(ret_value != MCC_SUCCESS)
        eunit_test_stop();

    /* Create TEA control endpoints */
    ret_value = mcc_create_endpoint(&tea_control_endpoint, tea_control_endpoint.port);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ret_value == MCC_SUCCESS, "TEST #1: 1.3 Creating TEA control endpoint");
    if(ret_value != MCC_SUCCESS)
        eunit_test_stop();

    /* Create tea_enpoint */
    mcc_create_endpoint(&tea_endpoint,tea_endpoint.port);
    msg.CMD = CTR_CMD_CREATE_EP;
    msg.ACK_REQUIRED = ACK_REQUIRED_NO;
    mcc_memcpy((void*)&data_create_ep_param,(void*)msg.DATA,(unsigned int)sizeof(CONTROL_MESSAGE_DATA_CREATE_EP_PARAM));
    /* Wait until the uut_control endpoint is created by the other core */
    while(MCC_ERR_ENDPOINT == mcc_send(&uut_control_endpoint,&msg,sizeof(CONTROL_MESSAGE),0xffffffff)){
    }
    /* De-initialize the semaphore before executing the command and  initialize the semaphore after executing the command on the uut side. */
    msg.flag = 1;

    /* mcc_create_endpoint() returns MCC_ERR_SEMAPHORE
     * when not sucessfull to grab the hw semaphore. */

    msg.CMD = CTR_CMD_CREATE_EP;
    msg.ACK_REQUIRED = ACK_REQUIRED_YES;
    data_create_ep_param.uut_endpoint.port++;
    mcc_memcpy((void*)&data_create_ep_param,(void*)msg.DATA,(unsigned int)sizeof(CONTROL_MESSAGE_DATA_CREATE_EP_PARAM));

    /* Send message to uut_condtrol_endpoint */
    ret_value = mcc_send(&uut_control_endpoint,&msg,sizeof(CONTROL_MESSAGE),0xffffffff);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ret_value == MCC_SUCCESS, "TEST #1: 1.4 Checking mcc_send");

    /* Receive message from uut_condtrol_endpoint */
    ret_value = mcc_recv_copy(&tea_control_endpoint,&ack_msg,sizeof(ACKNOWLEDGE_MESSAGE),&num_of_received_bytes,0xffffffff);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ret_value == MCC_SUCCESS, "TEST #1: 1.5 Checking mcc_recv_copy");
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ack_msg.RETURN_VALUE == MCC_ERR_SEMAPHORE, "TEST #1: 1.6 Checking correct CTR_CMD_CREATE_EP acknowledge message 'RETURN_VALUE' content");

    /* mcc_destroy_endpoint() returns MCC_ERR_SEMAPHORE
     * when not sucessfull to grab the hw semaphore. */

    msg.CMD = CTR_CMD_DESTROY_EP;
    msg.ACK_REQUIRED = ACK_REQUIRED_YES;
    mcc_memcpy((void*)&data_destroy_ep_param,(void*)msg.DATA,(unsigned int)sizeof(CONTROL_MESSAGE_DATA_DESTROY_EP_PARAM));

    /* Send message to uut_condtrol_endpoint */
    ret_value = mcc_send(&uut_control_endpoint,&msg,sizeof(CONTROL_MESSAGE),0xffffffff);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ret_value == MCC_SUCCESS, "TEST #1: 1.7 Checking mcc_send");

    /* Receive message from uut_condtrol_endpoint *//* Receive message from uut_condtrol_endpoint */
    ret_value = mcc_recv_copy(&tea_control_endpoint,&ack_msg,sizeof(ACKNOWLEDGE_MESSAGE),&num_of_received_bytes,0xffffffff);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ret_value == MCC_SUCCESS, "TEST #1: 1.8 Checking mcc_recv_copy");
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ack_msg.RETURN_VALUE == MCC_ERR_SEMAPHORE, "TEST #1: 1.9 Checking correct CTR_CMD_DESTROY_EP acknowledge message 'RETURN_VALUE' content");

    /* mcc_send() returns MCC_ERR_SEMAPHORE
     * when not sucessfull to grab the hw semaphore. */

    msg.CMD = CTR_CMD_SEND;
    msg.ACK_REQUIRED = ACK_REQUIRED_YES;
    mcc_memcpy((void*)&data_send_param,(void*)msg.DATA,(unsigned int)sizeof(CONTROL_MESSAGE_DATA_SEND_PARAM));

    /* Send message to uut_condtrol_endpoint */
    ret_value = mcc_send(&uut_control_endpoint,&msg,sizeof(CONTROL_MESSAGE),0xffffffff);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ret_value == MCC_SUCCESS, "TEST #1: 1.10 Checking mcc_send");

    /* Receive message from uut_condtrol_endpoint */
    ret_value = mcc_recv_copy(&tea_control_endpoint,&ack_msg,sizeof(ACKNOWLEDGE_MESSAGE),&num_of_received_bytes,0xffffffff);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ret_value == MCC_SUCCESS, "TEST #1: 1.11 Checking mcc_recv_copy");
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ack_msg.RETURN_VALUE == MCC_ERR_SEMAPHORE, "TEST #1: 1.12 Checking correct CTR_CMD_SEND acknowledge message 'RETURN_VALUE' content");

    /* mcc_recv_copy() returns MCC_ERR_SEMAPHORE
     * when not sucessfull to grab the hw semaphore. */

    msg.CMD = CTR_CMD_RECV;
    msg.ACK_REQUIRED = ACK_REQUIRED_YES;
    mcc_memcpy((void*)&data_recv_param,(void*)msg.DATA,(unsigned int)sizeof(CONTROL_MESSAGE_DATA_RECV_PARAM));

    /* Send message to uut_condtrol_endpoint */
    ret_value = mcc_send(&uut_control_endpoint,&msg,sizeof(CONTROL_MESSAGE),0xffffffff);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ret_value == MCC_SUCCESS, "TEST #1: 1.13 Checking mcc_send");

    /* Receive message from uut_condtrol_endpoint */
    ret_value = mcc_recv_copy(&tea_control_endpoint,&ack_msg,sizeof(ACKNOWLEDGE_MESSAGE),&num_of_received_bytes,0xffffffff);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ret_value == MCC_SUCCESS, "TEST #1: 1.14 Checking mcc_recv_copy");
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ack_msg.RETURN_VALUE == MCC_ERR_SEMAPHORE, "TEST #1: 1.15 Checking correct CTR_CMD_RECV(CMD_RECV_MODE_COPY) acknowledge message 'RETURN_VALUE' content");

    /* mcc_recv_nocopy() returns MCC_ERR_SEMAPHORE
     * when not sucessfull to grab the hw semaphore. */

    msg.CMD = CTR_CMD_RECV;
    msg.ACK_REQUIRED = ACK_REQUIRED_YES;
    data_recv_param.mode = CMD_RECV_MODE_NOCOPY;
    mcc_memcpy((void*)&data_recv_param,(void*)msg.DATA,(unsigned int)sizeof(CONTROL_MESSAGE_DATA_RECV_PARAM));

    /* Send message to uut_condtrol_endpoint */
    ret_value = mcc_send(&uut_control_endpoint,&msg,sizeof(CONTROL_MESSAGE),0xffffffff);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ret_value == MCC_SUCCESS, "TEST #1: 1.16 Checking mcc_send");

    /* Receive message from uut_condtrol_endpoint */
    ret_value = mcc_recv_copy(&tea_control_endpoint,&ack_msg,sizeof(ACKNOWLEDGE_MESSAGE),&num_of_received_bytes,0xffffffff);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ret_value == MCC_SUCCESS, "TEST #1: 1.17 Checking mcc_recv_copy");
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ack_msg.RETURN_VALUE == MCC_ERR_SEMAPHORE, "TEST #1: 1.18 Checking correct CTR_CMD_SEND(CMD_RECV_MODE_NOCOPY) acknowledge message 'RETURN_VALUE' content");

    /* mcc_msgs_available() returns MCC_ERR_SEMAPHORE
     * when not sucessfull to grab the hw semaphore. */

    msg.CMD = CTR_CMD_MSG_AVAIL;
    msg.ACK_REQUIRED = ACK_REQUIRED_YES;
    mcc_memcpy((void*)&data_msg_avail_param,(void*)msg.DATA,(unsigned int)sizeof(CONTROL_MESSAGE_DATA_MSG_AVAIL_PARAM));

    /* Send message to uut_condtrol_endpoint */
    ret_value = mcc_send(&uut_control_endpoint,&msg,sizeof(CONTROL_MESSAGE),0xffffffff);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ret_value == MCC_SUCCESS, "TEST #1: 1.19 Checking mcc_send");

    /* Receive message from uut_condtrol_endpoint */
    ret_value = mcc_recv_copy(&tea_control_endpoint,&ack_msg,sizeof(ACKNOWLEDGE_MESSAGE),&num_of_received_bytes,0xffffffff);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ret_value == MCC_SUCCESS, "TEST #1: 1.20 Checking mcc_recv_copy");
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ack_msg.RETURN_VALUE == MCC_ERR_SEMAPHORE, "TEST #1: 1.21 Checking correct CTR_CMD_MSG_AVAIL acknowledge message 'RETURN_VALUE' content");

    /* mcc_get_info() returns MCC_ERR_SEMAPHORE
     * when not sucessfull to grab the hw semaphore. */

    msg.CMD = CTR_CMD_GET_INFO;
    msg.ACK_REQUIRED = ACK_REQUIRED_YES;
    mcc_memcpy((void*)&data_get_info_param,(void*)msg.DATA,(unsigned int)sizeof(CONTROL_MESSAGE_DATA_GET_INFO_PARAM));

    /* Send message to uut_condtrol_endpoint */
    ret_value = mcc_send(&uut_control_endpoint,&msg,sizeof(CONTROL_MESSAGE),0xffffffff);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ret_value == MCC_SUCCESS, "TEST #1: 1.22 Checking mcc_send");

    /* Receive message from uut_condtrol_endpoint */
    ret_value = mcc_recv_copy(&tea_control_endpoint,&ack_msg,sizeof(ACKNOWLEDGE_MESSAGE),&num_of_received_bytes,0xffffffff);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ret_value == MCC_SUCCESS, "TEST #1: 1.23 Checking mcc_recv_copy");
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ack_msg.RETURN_VALUE == MCC_ERR_SEMAPHORE, "TEST #1: 1.24 Checking correct CTR_CMD_GET_INFO acknowledge message 'RETURN_VALUE' content");

    /* mcc_free_buffer() returns MCC_ERR_SEMAPHORE
     * when not sucessfull to grab the hw semaphore. */

    msg.CMD = CTR_CMD_FREE_BUF;
    msg.ACK_REQUIRED = ACK_REQUIRED_YES;
    mcc_memcpy((void*)&data_free_buf_param,(void*)msg.DATA,(unsigned int)sizeof(CONTROL_MESSAGE_DATA_FREE_BUF_PARAM));
    /* Send message to uut_condtrol_endpoint */
    ret_value = mcc_send(&uut_control_endpoint,&msg,sizeof(CONTROL_MESSAGE),0xffffffff);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ret_value == MCC_SUCCESS, "TEST #1: 1.25 Checking mcc_send");

    /* Receive message from uut_condtrol_endpoint */
    ret_value = mcc_recv_copy(&tea_control_endpoint,&ack_msg,sizeof(ACKNOWLEDGE_MESSAGE),&num_of_received_bytes,0xffffffff);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ret_value == MCC_SUCCESS, "TEST #1: 1.26 Checking mcc_recv_copy");
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ack_msg.RETURN_VALUE == MCC_ERR_SEMAPHORE, "TEST #1: 1.27 Checking correct CTR_CMD_FREE_BUF acknowledge message 'RETURN_VALUE' content");

    /* Initialize the semaphore before executing the command and  initialize the semaphore after executing the command on the uut side. */
    msg.flag = 5;

    /* mcc_create_endpoint() returns MCC_ERR_SEMAPHORE
     * when not sucessfull to grab the hw semaphore. */

    msg.CMD = CTR_CMD_CREATE_EP;
    msg.ACK_REQUIRED = ACK_REQUIRED_YES;
    data_create_ep_param.uut_endpoint.port++;
    mcc_memcpy((void*)&data_create_ep_param,(void*)msg.DATA,(unsigned int)sizeof(CONTROL_MESSAGE_DATA_CREATE_EP_PARAM));

    /* Send message to uut_condtrol_endpoint */
    ret_value = mcc_send(&uut_control_endpoint,&msg,sizeof(CONTROL_MESSAGE),0xffffffff);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ret_value == MCC_SUCCESS, "TEST #1: 1.28 Checking mcc_send");

    /* Receive message from uut_condtrol_endpoint */
    ret_value = mcc_recv_copy(&tea_control_endpoint,&ack_msg,sizeof(ACKNOWLEDGE_MESSAGE),&num_of_received_bytes,0xffffffff);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ret_value == MCC_SUCCESS, "TEST #1: 1.29 Checking mcc_recv_copy");
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ack_msg.RETURN_VALUE == MCC_ERR_SEMAPHORE, "TEST #1: 1.30 Checking correct CTR_CMD_CREATE_EP acknowledge message 'RETURN_VALUE' content");

    /* mcc_destroy_endpoint() returns MCC_ERR_SEMAPHORE
     * when not sucessfull to grab the hw semaphore. */

    msg.CMD = CTR_CMD_DESTROY_EP;
    msg.ACK_REQUIRED = ACK_REQUIRED_YES;
    mcc_memcpy((void*)&data_destroy_ep_param,(void*)msg.DATA,(unsigned int)sizeof(CONTROL_MESSAGE_DATA_DESTROY_EP_PARAM));

    /* Send message to uut_condtrol_endpoint */
    ret_value = mcc_send(&uut_control_endpoint,&msg,sizeof(CONTROL_MESSAGE),0xffffffff);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ret_value == MCC_SUCCESS, "TEST #1: 1.31 Checking mcc_send");

    /* Receive message from uut_condtrol_endpoint *//* Receive message from uut_condtrol_endpoint */
    ret_value = mcc_recv_copy(&tea_control_endpoint,&ack_msg,sizeof(ACKNOWLEDGE_MESSAGE),&num_of_received_bytes,0xffffffff);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ret_value == MCC_SUCCESS, "TEST #1: 1.32 Checking mcc_recv_copy");
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ack_msg.RETURN_VALUE == MCC_ERR_SEMAPHORE, "TEST #1: 1.33 Checking correct CTR_CMD_DESTROY_EP acknowledge message 'RETURN_VALUE' content");

    /* mcc_send() returns MCC_ERR_SEMAPHORE
     * when not sucessfull to grab the hw semaphore. */

    msg.CMD = CTR_CMD_SEND;
    msg.ACK_REQUIRED = ACK_REQUIRED_YES;
    mcc_memcpy((void*)&data_send_param,(void*)msg.DATA,(unsigned int)sizeof(CONTROL_MESSAGE_DATA_SEND_PARAM));

    /* Send message to uut_condtrol_endpoint */
    ret_value = mcc_send(&uut_control_endpoint,&msg,sizeof(CONTROL_MESSAGE),0xffffffff);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ret_value == MCC_SUCCESS, "TEST #1: 1.34 Checking mcc_send");

    /* Receive message from uut_condtrol_endpoint */
    ret_value = mcc_recv_copy(&tea_control_endpoint,&ack_msg,sizeof(ACKNOWLEDGE_MESSAGE),&num_of_received_bytes,0xffffffff);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ret_value == MCC_SUCCESS, "TEST #1: 1.35 Checking mcc_recv_copy");
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ack_msg.RETURN_VALUE == MCC_ERR_SEMAPHORE, "TEST #1: 1.36 Checking correct CTR_CMD_SEND acknowledge message 'RETURN_VALUE' content");

    /* mcc_recv_copy() returns MCC_ERR_SEMAPHORE
     * when not sucessfull to grab the hw semaphore. */

    msg.CMD = CTR_CMD_RECV;
    msg.ACK_REQUIRED = ACK_REQUIRED_YES;
    mcc_memcpy((void*)&data_recv_param,(void*)msg.DATA,(unsigned int)sizeof(CONTROL_MESSAGE_DATA_RECV_PARAM));

    /* Send message to uut_condtrol_endpoint */
    ret_value = mcc_send(&uut_control_endpoint,&msg,sizeof(CONTROL_MESSAGE),0xffffffff);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ret_value == MCC_SUCCESS, "TEST #1: 1.37 Checking mcc_send");

    /* Receive message from uut_condtrol_endpoint */
    ret_value = mcc_recv_copy(&tea_control_endpoint,&ack_msg,sizeof(ACKNOWLEDGE_MESSAGE),&num_of_received_bytes,0xffffffff);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ret_value == MCC_SUCCESS, "TEST #1: 1.38 Checking mcc_recv_copy");
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ack_msg.RETURN_VALUE == MCC_ERR_SEMAPHORE, "TEST #1: 1.39 Checking correct CTR_CMD_RECV(CMD_RECV_MODE_COPY) acknowledge message 'RETURN_VALUE' content");

    /* mcc_recv_nocopy() returns MCC_ERR_SEMAPHORE
     * when not sucessfull to grab the hw semaphore. */

    msg.CMD = CTR_CMD_RECV;
    msg.ACK_REQUIRED = ACK_REQUIRED_YES;
    data_recv_param.mode = CMD_RECV_MODE_NOCOPY;
    mcc_memcpy((void*)&data_recv_param,(void*)msg.DATA,(unsigned int)sizeof(CONTROL_MESSAGE_DATA_RECV_PARAM));

    /* Send message to uut_condtrol_endpoint */
    ret_value = mcc_send(&uut_control_endpoint,&msg,sizeof(CONTROL_MESSAGE),0xffffffff);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ret_value == MCC_SUCCESS, "TEST #1: 1.40 Checking mcc_send");

    /* Receive message from uut_condtrol_endpoint */
    ret_value = mcc_recv_copy(&tea_control_endpoint,&ack_msg,sizeof(ACKNOWLEDGE_MESSAGE),&num_of_received_bytes,0xffffffff);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ret_value == MCC_SUCCESS, "TEST #1: 1.41 Checking mcc_recv_copy");
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ack_msg.RETURN_VALUE == MCC_ERR_SEMAPHORE, "TEST #1: 1.42 Checking correct CTR_CMD_SEND(CMD_RECV_MODE_NOCOPY) acknowledge message 'RETURN_VALUE' content");

    /* mcc_msgs_available() returns MCC_ERR_SEMAPHORE
     * when not sucessfull to grab the hw semaphore. */

    msg.CMD = CTR_CMD_MSG_AVAIL;
    msg.ACK_REQUIRED = ACK_REQUIRED_YES;
    mcc_memcpy((void*)&data_msg_avail_param,(void*)msg.DATA,(unsigned int)sizeof(CONTROL_MESSAGE_DATA_MSG_AVAIL_PARAM));

    /* Send message to uut_condtrol_endpoint */
    ret_value = mcc_send(&uut_control_endpoint,&msg,sizeof(CONTROL_MESSAGE),0xffffffff);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ret_value == MCC_SUCCESS, "TEST #1: 1.43 Checking mcc_send");

    /* Receive message from uut_condtrol_endpoint */
    ret_value = mcc_recv_copy(&tea_control_endpoint,&ack_msg,sizeof(ACKNOWLEDGE_MESSAGE),&num_of_received_bytes,0xffffffff);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ret_value == MCC_SUCCESS, "TEST #1: 1.44 Checking mcc_recv_copy");
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ack_msg.RETURN_VALUE == MCC_ERR_SEMAPHORE, "TEST #1: 1.45 Checking correct CTR_CMD_MSG_AVAIL acknowledge message 'RETURN_VALUE' content");

    /* mcc_get_info() returns MCC_ERR_SEMAPHORE
     * when not sucessfull to grab the hw semaphore. */

    msg.CMD = CTR_CMD_GET_INFO;
    msg.ACK_REQUIRED = ACK_REQUIRED_YES;
    mcc_memcpy((void*)&data_get_info_param,(void*)msg.DATA,(unsigned int)sizeof(CONTROL_MESSAGE_DATA_GET_INFO_PARAM));

    /* Send message to uut_condtrol_endpoint */
    ret_value = mcc_send(&uut_control_endpoint,&msg,sizeof(CONTROL_MESSAGE),0xffffffff);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ret_value == MCC_SUCCESS, "TEST #1: 1.46 Checking mcc_send");

    /* Receive message from uut_condtrol_endpoint */
    ret_value = mcc_recv_copy(&tea_control_endpoint,&ack_msg,sizeof(ACKNOWLEDGE_MESSAGE),&num_of_received_bytes,0xffffffff);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ret_value == MCC_SUCCESS, "TEST #1: 1.47 Checking mcc_recv_copy");
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ack_msg.RETURN_VALUE == MCC_ERR_SEMAPHORE, "TEST #1: 1.48 Checking correct CTR_CMD_GET_INFO acknowledge message 'RETURN_VALUE' content");

    /* mcc_free_buffer() returns MCC_ERR_SEMAPHORE
     * when not sucessfull to grab the hw semaphore. */

    msg.CMD = CTR_CMD_FREE_BUF;
    msg.ACK_REQUIRED = ACK_REQUIRED_YES;
    mcc_memcpy((void*)&data_free_buf_param,(void*)msg.DATA,(unsigned int)sizeof(CONTROL_MESSAGE_DATA_FREE_BUF_PARAM));

    /* Send message to uut_condtrol_endpoint */
    ret_value = mcc_send(&uut_control_endpoint,&msg,sizeof(CONTROL_MESSAGE),0xffffffff);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ret_value == MCC_SUCCESS, "TEST #1: 1.49 Checking mcc_send");

    /* Receive message from uut_condtrol_endpoint */
    ret_value = mcc_recv_copy(&tea_control_endpoint,&ack_msg,sizeof(ACKNOWLEDGE_MESSAGE),&num_of_received_bytes,0xffffffff);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ret_value == MCC_SUCCESS, "TEST #1: 1.50 Checking mcc_recv_copy");
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ack_msg.RETURN_VALUE == MCC_ERR_SEMAPHORE, "TEST #1: 1.51 Checking correct CTR_CMD_FREE_BUF acknowledge message 'RETURN_VALUE' content");

    /* Destroy tea_enpoint */
    ret_value = mcc_destroy_endpoint(&tea_endpoint);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ret_value == MCC_SUCCESS, "TEST #1: 1.52 Checking mcc_destroy_endpoint");
}


/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2: Testing MCC buffer and signal queue.
*
* Requirement ID: MCC025,MCC026
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    CONTROL_MESSAGE     msg;
    ACKNOWLEDGE_MESSAGE ack_msg;
    MCC_MEM_SIZE        num_of_received_bytes;
    CONTROL_MESSAGE_DATA_CREATE_EP_PARAM     data_create_ep_param;
    int             ret_value;

    CONTROL_MESSAGE_DATA_SEND_PARAM    data_send_param;
    MCC_ENDPOINT        uut_endpoint = {MCC_UUT_CORE,MCC_UUT_NODE,MCC_UUT_EP_PORT};
    MCC_ENDPOINT        tea_endpoint = {MCC_TEA_CORE,MCC_TEA_NODE,MCC_TEA_EP_PORT};
    char            tea_app_buffer[TEA_APP_BUF_SIZE];

    data_create_ep_param.uut_endpoint = uut_endpoint;
    data_create_ep_param.uut_app_buffer_size = UUT_APP_BUF_SIZE;
    data_create_ep_param.endpoint_to_ack = tea_control_endpoint;

    data_send_param.dest_endpoint = tea_endpoint;
    mcc_memcpy("XXX",(void*)data_send_param.msg,(unsigned int)4);
    data_send_param.msg_size = CMD_SEND_MSG_SIZE;
    data_send_param.timeout_us = 0xffffffff;
    data_send_param.uut_endpoint = uut_endpoint;
    data_send_param.endpoint_to_ack = tea_control_endpoint;
    data_send_param.repeat_count = 1;

    /* Set default value(0) for mag.flag */
    msg.flag = 0;

    /* Create tea_enpoint */
    ret_value = mcc_create_endpoint(&tea_endpoint,tea_endpoint.port);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, ret_value == MCC_SUCCESS, "TEST #2: 2.1 Checking mcc_create_endpoint");

    msg.CMD = CTR_CMD_CREATE_EP;
    msg.ACK_REQUIRED = ACK_REQUIRED_NO;
    mcc_memcpy((void*)&data_create_ep_param,(void*)msg.DATA,(unsigned int)sizeof(CONTROL_MESSAGE_DATA_CREATE_EP_PARAM));
    /*wait until the uut_control endpoint is created by the other core*/
    while(MCC_ERR_ENDPOINT == mcc_send(&uut_control_endpoint,&msg,sizeof(CONTROL_MESSAGE),0xffffffff)){
    }

    /* MCC_ERR_NOMEM has to be returned when no free buffer available
     * and the non-blocking call has been issued (timeout_us set to 0). */

    msg.CMD = CTR_CMD_SEND;
    msg.ACK_REQUIRED = ACK_REQUIRED_YES;
    /* Release one buffer for mcc_send() on the uut side */
    msg.flag = 3;
    data_send_param.repeat_count = 11;
    data_send_param.timeout_us = 0;
    mcc_memcpy((void*)&data_send_param,(void*)msg.DATA,(unsigned int)sizeof(CONTROL_MESSAGE_DATA_SEND_PARAM));

    /* Send message to uut_condtrol_endpoint */
    ret_value = mcc_send(&uut_control_endpoint,&msg,sizeof(CONTROL_MESSAGE),0xffffffff);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, ret_value == MCC_SUCCESS, "TEST #2: 2.2 Checking mcc_send");

    /* Receive message from uut_condtrol_endpoint */
    ret_value = mcc_recv_copy(&tea_control_endpoint,&ack_msg,sizeof(ACKNOWLEDGE_MESSAGE),&num_of_received_bytes,0xffffffff);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, ret_value == MCC_SUCCESS, "TEST #2: 2.3 Checking mcc_recv_copy");
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, ack_msg.RETURN_VALUE == MCC_ERR_NOMEM, "TEST #2: 2.4 Checking correct CTR_CMD_SEND acknowledge message 'RETURN_VALUE' content");

    /* Release all messages sent to the TEA EP */
    while (MCC_SUCCESS == mcc_recv_copy(&tea_endpoint, &tea_app_buffer, TEA_APP_BUF_SIZE, &num_of_received_bytes, 0)) {
        EU_ASSERT_REF_TC_MSG( tc_2_main_task, 0 == strncmp(tea_app_buffer, "XXX", 3), "TEST #2: 2.5 Checking correct response message content");
    }

    /* MCC_ERR_SQ_FULL has to be returned when signal queue is full.
     * Check that no buffer has been enqueued to the destination EP buffer at this errror condition. */

    msg.CMD = CTR_CMD_SEND;
    msg.ACK_REQUIRED = ACK_REQUIRED_YES;
    /* Make signal queue full and release one signal for mcc_send() on the uut side */
    msg.flag = 4;
    data_send_param.repeat_count = 1;
    data_send_param.timeout_us = 0xffffffff;
    mcc_memcpy((void*)&data_send_param,(void*)msg.DATA,(unsigned int)sizeof(CONTROL_MESSAGE_DATA_SEND_PARAM));

    /* Send message to uut_condtrol_endpoint */
    ret_value = mcc_send(&uut_control_endpoint,&msg,sizeof(CONTROL_MESSAGE),0xffffffff);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, ret_value == MCC_SUCCESS, "TEST #2: 2.6 Checking mcc_send");

    /* Receive message from uut_condtrol_endpoint */
    mcc_recv_copy(&tea_control_endpoint,&ack_msg,sizeof(ACKNOWLEDGE_MESSAGE),&num_of_received_bytes,0xffffffff);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, ret_value == MCC_SUCCESS, "TEST #2: 2.7 Checking mcc_recv_copy");
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, ~ack_msg.RETURN_VALUE == MCC_ERR_SQ_FULL, "TEST #2: 2.8 Checking correct CTR_CMD_SEND acknowledge message 'RETURN_VALUE' content");

    /* Destroy tea_enpoint */
    ret_value = mcc_destroy_endpoint(&tea_endpoint);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, ret_value == MCC_SUCCESS, "TEST #2: 2.9 Checking mcc_destroy_endpoint");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : TEST #3: Testing MCC deinitialization
*
* Requirement  : MCC015
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
    int           ret_value;

    /* mcc_destroy() can return MCC_ERR_SEMAPHORE
     * when the semaphore is de-initialized */

    /* Destroy TEA control endpoint */
    ret_value = mcc_destroy_endpoint(&tea_control_endpoint);
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, ret_value == MCC_SUCCESS, "TEST #3: 3.1 Destroying TEA control endpoint");
    /* De-initialize the semaphore */
    ret_value = mcc_deinit_semaphore(MCC_SHMEM_SEMAPHORE_NUMBER);
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, ret_value == MCC_SUCCESS, "TEST #3: 3.2 De-initialize the semaphore");
    /* Test mcc_destroy() when the semaphore is de-initialized*/
    ret_value = mcc_destroy(MCC_TEA_NODE);
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, ret_value == MCC_ERR_SEMAPHORE, "TEST #3: 3.3 Checking mcc_destroy() ret_value when not sucessfull to grab the hw semaphore.");
    /* Initialize the semaphore */
    ret_value = mcc_init_semaphore(MCC_SHMEM_SEMAPHORE_NUMBER);
    /* Destroy the mcc node in normal operation */
    ret_value = mcc_destroy(MCC_TEA_NODE);
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, ret_value == MCC_SUCCESS, "TEST #3: 3.4 Destroying MCC");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_4_main_task
* Comments     : TEST #4: Testing MCC initialization when inter-core hw semaphore has not been correctly initialized
*
* Requirement  : MCC013
*
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
    int                         ret_value,temp;
    MEMORY_ALLOC_INFO_STRUCT    memory_alloc_info;
    KERNEL_DATA_STRUCT_PTR      kernel_data;
    CORE_MUTEX_COMPONENT_PTR    core_mutex_ptr;

    /* mcc_initialize() can return MCC_ERR_SEMAPHORE
     * when the function is called in ISR */

    _GET_KERNEL_DATA(kernel_data);
    /* Disable all interrupts */
    _int_disable();
    /* Save the current value of IN_ISR */
    temp = kernel_data->IN_ISR;
    kernel_data->IN_ISR = 1;
    /* Test mcc_initialize() in ISR */
    ret_value = mcc_initialize(MCC_TEA_NODE);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, ret_value == MCC_ERR_SEMAPHORE, "TEST #4: 4.1 Testing mcc_initialize in ISR");
    /* Restore IN_ISR to original value */
    kernel_data->IN_ISR = temp;
    /* Enable all interrupts */
    _int_enable();

    /* Test mcc_initialize() in normal operation */
    ret_value = mcc_initialize(MCC_TEA_NODE);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, ret_value == MCC_SUCCESS, "TEST #4: 4.2 Testing mcc_initialize");
    /* Destroy the mcc node */
    ret_value = mcc_destroy(MCC_TEA_NODE);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, ret_value == MCC_SUCCESS, "TEST #4: 4.3 Destroying MCC");

    /* mcc_initilize() can return MCC_ERR_SEMAPHORE
     * when the mutex component doesn't exist */

    /* Save pointer to mutex component */
    core_mutex_ptr = _core_mutext_get_component_ptr();
    /* Test for the mutex component doesn't exist */
    _core_mutext_set_component_ptr(NULL);
    /* Initialize the mcc node when the mutex component doesn't exist */
    ret_value = mcc_initialize(MCC_TEA_NODE);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, ret_value == MCC_ERR_SEMAPHORE, "TEST #4: 4.4 Testing mcc_initialize when the mutex component doesn't exist");
    /* Restore mutext component to original value */
    _core_mutext_set_component_ptr(core_mutex_ptr);
    _task_set_error(MQX_OK);

    /* Initialize the mcc node when the mutex component exists */
    ret_value = mcc_initialize(MCC_TEA_NODE);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, ret_value == MCC_SUCCESS, "TEST #4: 4.5 Testing mcc_initialize when the mutex component exists");
    /* Destroy the mcc node */
    ret_value = mcc_destroy(MCC_TEA_NODE);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, ret_value == MCC_SUCCESS, "TEST #4: 4.6 Destroying MCC");

    /* mcc_initilize() can return MCC_ERR_SEMAPHORE
     * when the semaphore has been initialized */

    /* Initialize the semaphore */
    ret_value = mcc_init_semaphore(MCC_SHMEM_SEMAPHORE_NUMBER);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, ret_value == MCC_SUCCESS, "TEST #4: 4.7 Testing mcc_init_semaphore");
    /* Initialize the mcc node when the semaphore has been initialized */
    ret_value = mcc_initialize(MCC_TEA_NODE);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, ret_value == MCC_ERR_SEMAPHORE, "TEST #4: 4.8 Testing mcc_initialize when the semaphore has been initialized");

    /* Get mutex pointer corresponding to MCC_SHMEM_SEMAPHORE_NUMBER*/
    CORE_MUTEX_PTR mutex_ptr = _core_mutex_get(0,MCC_SHMEM_SEMAPHORE_NUMBER);
    /* Destroy the mutex */
    ret_value = _core_mutex_destroy(mutex_ptr);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, ret_value == COREMUTEX_OK, "TEST #4: 4.9 Destroying the mutex");
    /* Initialize the mcc node */
    ret_value = mcc_initialize(MCC_TEA_NODE);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, ret_value == MCC_SUCCESS, "TEST #4: 4.10 Testing mcc_initialize when the semaphore has been initialized");
    /* Destroy the mcc node */
    ret_value = mcc_destroy(MCC_TEA_NODE);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, ret_value == MCC_SUCCESS, "TEST #4: 4.11 Destroying MCC");

    /* mcc_initilize() can return MCC_ERR_SEMAPHORE
     * when all free memory is allocated */

    /* The all free memory is allocated */
    memory_alloc_all(&memory_alloc_info);
    _task_set_error(MQX_OK);
    /* Initialize the mcc node when all free memory is allocated */
    ret_value = mcc_initialize(MCC_TEA_NODE);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, ret_value == MCC_ERR_SEMAPHORE, "TEST #4: 4.12 Testing mcc_initialize when the semaphore has been initialized");
    _task_set_error(MQX_OK);
    /* Free the allocated memory */
    memory_free_all(&memory_alloc_info);
    /* Initialize the mcc node */
    ret_value = mcc_initialize(MCC_TEA_NODE);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, ret_value == MCC_SUCCESS, "TEST #4: 4.13 Testing mcc_initialize when the semaphore has been initialized");
    /* Destroy the mcc node */
    ret_value = mcc_destroy(MCC_TEA_NODE);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, ret_value == MCC_SUCCESS, "TEST #4: 4.14 Destroying MCC");
}

//------------------------------------------------------------------------
// Define Test Suite
 EU_TEST_SUITE_BEGIN(suite_mcc)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST #1 - Testing MCC function when not successfully grap the hw semaphore"),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST #2 - Testing MCC buffer and signal queue."),
    EU_TEST_CASE_ADD(tc_3_main_task, " TEST #3 - Testing MCC deinitialization"),
    EU_TEST_CASE_ADD(tc_4_main_task, " TEST #4 - Testing MCC initialization when inter-core hw semaphore has not been correctly initialized"),
 EU_TEST_SUITE_END(suite_mcc)
// Add test suites
 EU_TEST_REGISTRY_BEGIN()
        EU_TEST_SUITE_ADD(suite_mcc, " - MCC task suite")
 EU_TEST_REGISTRY_END()

void main_task
(
    /* [IN] The MQX task parameter */
    unsigned int param
)
{   /* Body */
    eunit_mutex_init();
    EU_RUN_ALL_TESTS(eunit_tres_array);
    eunit_test_stop();
}  /* Endbody */

#endif
