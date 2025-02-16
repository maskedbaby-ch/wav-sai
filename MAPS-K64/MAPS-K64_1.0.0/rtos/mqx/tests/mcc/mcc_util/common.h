/**HEADER******************************************************************
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
* $FileName: common.h$
* $Version : 4.0.2$
* $Date    : Aug-30-2013$
*
* Comments:
*
*   This file contains the source for the common definitions for the
*   MCC example
*
*END************************************************************************/
#ifndef __common_h__
#define __common_h__

#include <mqx.h>
#include <bsp.h>
#include <string.h>
#include <core_mutex.h>

#include "mcc_config.h"
#include "mcc_common.h"
#include "mcc_api.h"
#include "mcc_mqx.h"

#define PRINT_ON              (1)

#define MAIN_TASK             (10)
#define RESP_TASK             (12)

#define TEST_CNT              (10)
#define UUT_APP_BUF_SIZE      (30)
#define TEA_APP_BUF_SIZE      (50)
#define CMD_SEND_MSG_SIZE     (20)
#define CMD_SEND_TIMEOUT_US   (1000000)
#define CMD_RECV_TIMEOUT_US   (2000000)

#define CTR_CMD_CREATE_EP     (10)
#define CTR_CMD_DESTROY_EP    (11)
#define CTR_CMD_SEND          (12)
#define CTR_CMD_RECV          (13)
#define CTR_CMD_MSG_AVAIL     (14)
#define CTR_CMD_GET_INFO      (15)
#define CTR_CMD_FREE_BUF      (16)
#define CTR_CMD_DESTROY_NODE  (17)

/* recv command modes */
#define CMD_RECV_MODE_COPY     (1)
#define CMD_RECV_MODE_NOCOPY   (2)

/* acknowledge required? */
#define ACK_REQUIRED_YES       (1)
#define ACK_REQUIRED_NO        (0)

#if PSP_MQX_CPU_IS_VYBRID_A5
#if  (MCC_TEST_APP == 1)
  #define MCC_TEA_CORE         (0)
  #define MCC_TEA_NODE         (0)
  #define MCC_TEA_CTRL_EP_PORT (5)
  #define MCC_TEA_EP_PORT      (19)
  #define MCC_UUT_CORE         (1)
  #define MCC_UUT_NODE         (0)
  #define MCC_UUT_CTRL_EP_PORT (6)
  #define MCC_UUT_EP_PORT      (20)
#else
  #define MCC_TEA_CORE         (1)
  #define MCC_TEA_NODE         (0)
  #define MCC_TEA_CTRL_EP_PORT (5)
  #define MCC_TEA_EP_PORT      (19)
  #define MCC_UUT_CORE         (0)
  #define MCC_UUT_NODE         (0)
  #define MCC_UUT_CTRL_EP_PORT (6)
  #define MCC_UUT_EP_PORT      (20)
#endif
#elif PSP_MQX_CPU_IS_VYBRID_M4
#if  (MCC_TEST_APP == 0)
  #define MCC_TEA_CORE         (0)
  #define MCC_TEA_NODE         (0)
  #define MCC_TEA_CTRL_EP_PORT (5)
  #define MCC_TEA_EP_PORT      (19)
  #define MCC_UUT_CORE         (1)
  #define MCC_UUT_NODE         (0)
  #define MCC_UUT_CTRL_EP_PORT (6)
  #define MCC_UUT_EP_PORT      (20)
#else
  #define MCC_TEA_CORE         (1)
  #define MCC_TEA_NODE         (0)
  #define MCC_TEA_CTRL_EP_PORT (5)
  #define MCC_TEA_EP_PORT      (19)
  #define MCC_UUT_CORE         (0)
  #define MCC_UUT_NODE         (0)
  #define MCC_UUT_CTRL_EP_PORT (6)
  #define MCC_UUT_EP_PORT      (20)
#endif
#endif

MCC_ENDPOINT    tea_control_endpoint = {MCC_TEA_CORE,MCC_TEA_NODE,MCC_TEA_CTRL_EP_PORT};
MCC_ENDPOINT    uut_control_endpoint = {MCC_UUT_CORE,MCC_UUT_NODE,MCC_UUT_CTRL_EP_PORT};
MCC_ENDPOINT    invalid_endpoint_reserved_port = {0,0,MCC_RESERVED_PORT_NUMBER};
MCC_ENDPOINT    invalid_endpoint_not_created = {2,2,2};

PACKED_STRUCT_BEGIN
struct control_message
{
    char         CMD;
    char         ACK_REQUIRED;
    char         DATA[100];
    char         flag;
} PACKED_STRUCT_END;
typedef struct control_message CONTROL_MESSAGE, * CONTROL_MESSAGE_PTR;

PACKED_STRUCT_BEGIN
struct acknowledge_message
{
    char         CMD_ACK;
    int          RETURN_VALUE;
    int          TS1_SEC;
    int          TS1_MSEC;
    int          TS2_SEC;
    int          TS2_MSEC;
    char         RESP_DATA[100];
} PACKED_STRUCT_END;
typedef struct acknowledge_message ACKNOWLEDGE_MESSAGE, * ACKNOWLEDGE_MESSAGE_PTR;

PACKED_STRUCT_BEGIN
struct control_message_data_create_ep_param
{
    MCC_ENDPOINT uut_endpoint;
    MCC_MEM_SIZE uut_app_buffer_size;
    MCC_ENDPOINT endpoint_to_ack;
} PACKED_STRUCT_END;
typedef struct control_message_data_create_ep_param CONTROL_MESSAGE_DATA_CREATE_EP_PARAM, * CONTROL_MESSAGE_DATA_CREATE_EP_PARAM_PTR;

PACKED_STRUCT_BEGIN
struct control_message_data_destroy_ep_param
{
    MCC_ENDPOINT uut_endpoint;
    MCC_ENDPOINT endpoint_to_ack;
} PACKED_STRUCT_END;
typedef struct control_message_data_destroy_ep_param CONTROL_MESSAGE_DATA_DESTROY_EP_PARAM, * CONTROL_MESSAGE_DATA_DESTROY_EP_PARAM_PTR;

PACKED_STRUCT_BEGIN
struct control_message_data_send_param
{
    MCC_ENDPOINT dest_endpoint;
    char         msg[50]; //if empty send the content of the uup_app_buffer
    MCC_MEM_SIZE msg_size;
    unsigned int timeout_us;
    MCC_ENDPOINT uut_endpoint; //in case of uup_app_buffer has to be used, specify the UUT endpoint
    MCC_ENDPOINT endpoint_to_ack;
    unsigned int repeat_count;
} PACKED_STRUCT_END;
typedef struct control_message_data_send_param CONTROL_MESSAGE_DATA_SEND_PARAM, * CONTROL_MESSAGE_DATA_SEND_PARAM_PTR;

PACKED_STRUCT_BEGIN
struct control_message_data_recv_param
{
    MCC_ENDPOINT uut_endpoint;
    MCC_MEM_SIZE uut_app_buffer_size;
    unsigned int timeout_us;
    unsigned int mode;
    MCC_ENDPOINT endpoint_to_ack;
} PACKED_STRUCT_END;
typedef struct control_message_data_recv_param CONTROL_MESSAGE_DATA_RECV_PARAM, * CONTROL_MESSAGE_DATA_RECV_PARAM_PTR;

PACKED_STRUCT_BEGIN
struct control_message_data_msg_avail_param
{
    MCC_ENDPOINT uut_endpoint;
    MCC_ENDPOINT endpoint_to_ack;
} PACKED_STRUCT_END;
typedef struct control_message_data_msg_avail_param CONTROL_MESSAGE_DATA_MSG_AVAIL_PARAM, * CONTROL_MESSAGE_DATA_MSG_AVAIL_PARAM_PTR;

PACKED_STRUCT_BEGIN
struct control_message_data_get_info_param
{
    MCC_ENDPOINT uut_endpoint;
    MCC_ENDPOINT endpoint_to_ack;
} PACKED_STRUCT_END;
typedef struct control_message_data_get_info_param CONTROL_MESSAGE_DATA_GET_INFO_PARAM, * CONTROL_MESSAGE_DATA_GET_INFO_PARAM_PTR;

PACKED_STRUCT_BEGIN
struct control_message_data_free_buf_param
{
    char*    free_buf;
    MCC_ENDPOINT endpoint_to_ack;
} PACKED_STRUCT_END;
typedef struct control_message_data_free_buf_param CONTROL_MESSAGE_DATA_FREE_BUF_PARAM, * CONTROL_MESSAGE_DATA_FREE_BUF_PARAM_PTR;
#endif
