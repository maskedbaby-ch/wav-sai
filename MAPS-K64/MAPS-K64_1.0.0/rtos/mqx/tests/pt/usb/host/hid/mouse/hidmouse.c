/**HEADER********************************************************************
 *
 * Copyright (c) 2008-2013 Freescale Semiconductor;
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
 * $FileName: hidmouse.c$
 * $Version : 4.0.2$
 * $Date    : Jul-12-2013$
 *
 * Comments:
 *
 *   This file includes test codes of device drivers for Mouse device.
 *   It supports multi-instance dynamically.
 *   In the case, it test USB HID multi-instance supported, HID Class APIs, Hotplug
 *   and USB Mouse Event.
 *   It has been tested in Dell USB Mouse and Microsoft Basic Optical Mouse with 3
 *   button wheel Mouse.
 *   For the USB Mouse Event test part, the program queues transfers on Interrupt
 *   USB pipe and waits till the data comes back. It prints the data and queues
 *   another transfer on the same pipe.
 *   Now the Mouse report data is processed with a fixed parse method. Most mouse can
 *   be supported under current solution. Need to enhance the data process method according
 *   to the specific Report Descriptor for each HID device.
 *   See the code for details.
 *   CaseID in Testlink: MSB-11:USB-11: USB Host HID mouse user scenario test
 *
 *END************************************************************************/

#include <mqx.h>
#include <lwevent.h>
#include <bsp.h>
#include <usb.h>
#include <hostapi.h>
#include <usb_host_hid.h>
#include <usb_host_hub_sm.h>
#include <mqx_dll.h>

#include "hidmouse.h"


#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif


/************************************************************************************
 **
 ** Globals
 ************************************************************************************/

/* Event to manage APP Task creation destroy */
LWEVENT_STRUCT USB_Task_Event;
#define USB_TASK_CREATE                 (0x01)
#define USB_TASK_DESTROY                (0x02)

/* Event to manage hotplug test */
LWEVENT_STRUCT USB_Hotplug_Event;
#define USB_HOTPLUG                     (0x01)
#define HOTPLUG_TIMES                   10                  /* Force to do 10 times hotplug test */
uint8_t hotplug;

/* Event used in APP */
#define USB_EVENT_CTRL                  (0x01)
#define USB_EVENT_DATA                  (0x02)
#define USB_EVENT_DATA_CORRUPTED        (0x04)

#define MAIN_TASK                       (10)
#define MOUSE_TASK                      (11)

/* HID class request names */
char* api_array[]={"GET_PROTOCOL", "SET_PROTOCOL", "GET_REPORT", "SET_REPORT", "GET_IDLE", "SET_IDLE"};

/* Doubly-linked list to manage mutli-instance dynamically */
MQX_DLL_LIST hid_list;
MQX_DLL_NODE_PTR        temp_node;
HID_DEVICE_STRUCT_PTR   hid_instance = 0;

_usb_host_handle        host_handle;         /* global handle for calling host   */

/************************************************************************************
  Table of driver capabilities this application wants to use. See Host API document for
  details on How to define a driver info table. This table defines that this driver
  supports a HID class, boot subclass and mouse protocol.
 ************************************************************************************/

static  USB_HOST_DRIVER_INFO DriverInfoTable[] =
{
    {
        {0x00,0x00},                  /* Vendor ID per USB-IF             */
        {0x00,0x00},                  /* Product ID per manufacturer      */
        USB_CLASS_HID,                /* Class code                       */
        USB_SUBCLASS_HID_BOOT,        /* Sub-Class code                   */
        USB_PROTOCOL_HID_MOUSE,       /* Protocol                         */
        0,                            /* Reserved                         */
        usb_host_hid_mouse_event      /* Application call back function   */
    },
    /* USB 1.1 hub */
    {
        {0x00,0x00},                  /* Vendor ID per USB-IF             */
        {0x00,0x00},                  /* Product ID per manufacturer      */
        USB_CLASS_HUB,                /* Class code                       */
        USB_SUBCLASS_HUB_NONE,        /* Sub-Class code                   */
        USB_PROTOCOL_HUB_ALL,          /* Protocol                         */
        0,                            /* Reserved                         */
        usb_host_hub_device_event     /* Application call back function   */
    },
    {
        {0x00,0x00},                  /* All-zero entry terminates        */
        {0x00,0x00},                  /* driver info list.                */
        0,
        0,
        0,
        0,
        NULL
    }
};



void Main_Task(uint32_t param);
void process_mouse_buffer(unsigned char * buffer, _task_id t_id);
void Mouse_Task( uint32_t param );
void check_api_result(void * p, uint32_t status);
void create_task();
void destroy_task();


TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
    { MAIN_TASK,      Main_Task,      1000L,  9L, "Main",      MQX_AUTO_START_TASK},
    { MOUSE_TASK,  Mouse_Task,  1000L,  10L,  "Mouse", 0},
    { 0L,             0L,             0L,     0L, 0L,          0L }
};

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : main (Main_Task if using MQX)
 * Returned Value : none
 * Comments       :
 *     Execution starts here
 *
 *END*--------------------------------------------------------------------*/

void Main_Task ( uint32_t param )
{
    USB_STATUS              status = USB_OK;
    uint32_t e_task;
    _lwevent_create(&USB_Task_Event, LWEVENT_AUTO_CLEAR);
    _lwevent_create(&USB_Hotplug_Event, 0);
    _mqx_dll_list_init(&hid_list);
    _int_disable();

    _int_install_unexpected_isr();
    if (MQX_OK != _usb_host_driver_install(USBCFG_DEFAULT_HOST_CONTROLLER)) {
        printf("\n Driver installation failed");
        _int_enable();
        _task_block();
    }

    status = _usb_host_init(USBCFG_DEFAULT_HOST_CONTROLLER, &host_handle);

    if (status != USB_OK) {
        printf("\nUSB Host Initialization failed. STATUS: %x", status);
        _int_enable();
        _task_block();
    } /* Endif */

    /*
     ** since we are going to act as the host driver, register the driver
     ** information for wanted class/subclass/protocols
     */
    status = _usb_host_driver_info_register(host_handle, DriverInfoTable);
    if (status != USB_OK) {
        printf("\nDriver Registration failed. STATUS: %x", status);
        _int_enable();
        _task_block();
    }

    _int_enable();

    /* Here it is APP level codes */
    printf("\n----- MQX USB HID Mouse Case -----\n");
    printf("Suggest using Golden Sample - Microsoft Basic Optical Mouse 1113\n");
    printf("Include     Hotplug test\n");
    printf("            Multi-instance test\n");
    printf("            Class-level API test\n");
    printf("            USB Mouse Event test\n");
    fflush(stdout);
    for( ; ; ){
        _lwevent_wait_ticks(&USB_Task_Event, USB_TASK_CREATE | USB_TASK_DESTROY, FALSE, 0);
        e_task = _lwevent_get_signalled();
        if (USB_TASK_CREATE == e_task) {
            /* Device plugin, create a new APP instance */
            create_task();
        }
        if (USB_TASK_DESTROY == e_task) {
            /* Device plugout, destroy the related APP task */
            destroy_task();
        }
    }
}

void Mouse_Task( uint32_t param ){
    /* Get the device instance point */
    HID_DEVICE_STRUCT_PTR           hid_device = (HID_DEVICE_STRUCT_PTR)param;
    USB_STATUS                      status = USB_OK;
    TR_INIT_PARAM_STRUCT            tr;
    HID_COMMAND                     hid_com;
    unsigned char *                       buffer;
    PIPE_STRUCT_PTR                 pipe;
    uint32_t                         e;

    /* Create USB_Event which is used just in the test Task */
    _lwevent_create(&(hid_device->USB_Event), LWEVENT_AUTO_CLEAR);

    for ( ; ; ) {
        switch ( hid_device->DEV_STATE ) {
            case USB_DEVICE_ATTACHED:
                printf("\nMouse device attached\n");
                fflush(stdout);
                status = _usb_hostdev_select_interface(hid_device->DEV_HANDLE, hid_device->INTF_HANDLE, (void *)&hid_device->CLASS_INTF);
                if (status != USB_OK) {
                    printf("\nError in _usb_hostdev_select_interface: %x", status);
                    fflush(stdout);
                    _task_block();
                } /* Endif */
                hid_device->DEV_STATE = USB_DEVICE_INTERFACE_INPROGRESS;
                break;

            case USB_DEVICE_INTERFACE_INPROGRESS:
                /* We need wait select interface finished to begin API test */
                _lwevent_wait_ticks(&(hid_device->USB_Event), USB_EVENT_CTRL, FALSE, 0);
                hid_device->DEV_STATE = USB_DEVICE_HOTPLUG;
                break;

            case USB_DEVICE_API:
                printf("\nClass level API Test\n\n");
                hid_com.CLASS_PTR = (CLASS_CALL_STRUCT_PTR)&hid_device->CLASS_INTF;
                hid_com.CALLBACK_FN = usb_host_hid_ctrl_callback;
                hid_com.CALLBACK_PARAM = (void *)hid_device;

                /* GET_PROTOCOL request verification */
                unsigned char p=0;
                hid_device->API_STATE=USB_DEVICE_GET_PROTOCOL;
                status = usb_class_hid_get_protocol(&hid_com, &p);
                check_api_result(hid_device, status);

                /* SET_PROTOCOL request verification */
                hid_device->API_STATE=USB_DEVICE_SET_PROTOCOL;
                status = usb_class_hid_set_protocol(&hid_com, USB_PROTOCOL_HID_NONE);
                check_api_result(hid_device, status);

                /* set back the default protocol */
                hid_device->API_STATE=USB_DEVICE_SET_PROTOCOL;
                status = usb_class_hid_set_protocol(&hid_com, p);
                check_api_result(hid_device, status);

                /* GET_REPORT request verification */
                unsigned char report_buf[]={0,0,0,0};
                hid_device->API_STATE=USB_DEVICE_GET_REPORT;
                status = usb_class_hid_get_report(&hid_com, 0, 3, (void *)report_buf, 4);
                check_api_result(hid_device, status);

                /* SET_PROTOCOL request verification */
                hid_device->API_STATE=USB_DEVICE_SET_REPORT;
                status = usb_class_hid_set_report(&hid_com, 0, 3, (void *)report_buf, 4);
                check_api_result(hid_device, status);

                /* GET_IDLE request verification */
                uint8_t idle_rate=0;
                hid_device->API_STATE=USB_DEVICE_GET_IDLE;
                status = usb_class_hid_get_idle(&hid_com, 0, &idle_rate);
                check_api_result(hid_device, status);

                /* SET_IDLE request verification */
                hid_device->API_STATE=USB_DEVICE_SET_IDLE;
                status = usb_class_hid_set_idle(&hid_com, 0, idle_rate/2);
                check_api_result(hid_device, status);

                /* set back the default idle rate if needed */
                if (idle_rate){
                    status = usb_class_hid_set_idle(&hid_com, 0, idle_rate);
                    check_api_result(hid_device, status);
                }
                if(hid_device->API_ERR){
                    printf("\nClass level API Test Failed\n\n");
                }
                else{
                    printf("\nClass level API Test Passed\n\n");
                }
                hid_device->DEV_STATE = USB_DEVICE_INTERFACED;
                break;

            case USB_DEVICE_HOTPLUG:
                if(hotplug < HOTPLUG_TIMES){
                    printf("\nHotplug Test, please plugout the USB Device\n");
                    _lwevent_wait_ticks(&(USB_Hotplug_Event), USB_HOTPLUG, FALSE, 0);
                }
                hid_device->DEV_STATE = USB_DEVICE_API;
                break;

            case USB_DEVICE_INTERFACED:
                /* In interfaced branch, prepare the USB device pipe for data transfer */
                pipe = _usb_hostdev_find_pipe_handle(hid_device->DEV_HANDLE, hid_device->INTF_HANDLE, USB_INTERRUPT_PIPE, USB_RECV);
                if (pipe == NULL) {
                    printf("\nError getting interrupt pipe.\n");
                    fflush(stdout);
                    _task_block();
                }
                _usb_hostdev_get_buffer(hid_device->DEV_HANDLE, pipe->MAX_PACKET_SIZE, (void * *) &buffer);
                if (buffer == NULL) {
                    printf("\nMemory allocation failed. STATUS: %x\n", status);
                    fflush(stdout);
                    _task_block();
                }
                hid_device->DEV_STATE  = USB_DEVICE_INUSE;

            case USB_DEVICE_INUSE:
                printf("\nMouse device ready, try to move the mouse to do USB mouse event test\n");
                while (1) {
                    /******************************************************************
                      Initiate a transfer request on the interrupt pipe
                     ******************************************************************/
                    usb_hostdev_tr_init(&tr, usb_host_hid_recv_callback, (void *)hid_device);
                    tr.G.RX_BUFFER = buffer;
                    tr.G.RX_LENGTH = pipe->MAX_PACKET_SIZE;

                    status = _usb_host_recv_data(host_handle, pipe, &tr);

                    if (status != USB_STATUS_TRANSFER_QUEUED) {
                        printf("\nError in _usb_host_recv_data: %x", status);
                        fflush(stdout);
                    }

                    /* Wait untill we get the data from keyboard. */
                    _lwevent_wait_ticks(&(hid_device->USB_Event), USB_EVENT_DATA | USB_EVENT_DATA_CORRUPTED, FALSE, 0);
                    e = _lwevent_get_signalled();
                    /* process only the data is valid */
                    if (USB_EVENT_DATA == e) {
                        process_mouse_buffer((unsigned char *)buffer, hid_device->t_id);
                    }
                    if(USB_EVENT_DATA_CORRUPTED == e){
                        printf("Data corrupted!\n");
                    }

                }
                break;
            default:
                printf("Unknown Mouse Device State = %d\n", hid_device->DEV_STATE);
                fflush(stdout);
                break;
        } /* Endswitch */
    } /* Endfor */
} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_host_hid_mouse_event
 * Returned Value : None
 * Comments       :
 *     Called when HID device has been attached, detached, etc
 *END*--------------------------------------------------------------------*/

void usb_host_hid_mouse_event
(
 /* [IN] void * to device instance */
 _usb_device_instance_handle      dev_handle,

 /* [IN] void * to interface descriptor */
 _usb_interface_descriptor_handle intf_handle,

 /* [IN] code number for event causing callback */
 uint32_t                          event_code
 )
{ /* Body */

    DEVICE_DESCRIPTOR dev_desc = ((DEV_INSTANCE_PTR)dev_handle)->dev_descriptor;
    INTERFACE_DESCRIPTOR_PTR intf_ptr = (INTERFACE_DESCRIPTOR_PTR)intf_handle;
    /*
     * VID and PID for Golden Sample Mouse Microsoft Optical Mouse - 1113
     * Record them for further strict Golden Sample case
     *
     * uint_16 t_vid = 0x045E;
     * uint_16 t_pid = 0x00CB;
     */
    uint16_t                     vid, pid;

    switch (event_code) {
        case USB_ATTACH_EVENT:
            printf("----- Attach Event -----\n");
            /* Drop through into attach, same processing */
        case USB_CONFIG_EVENT:
            vid = dev_desc.idVendor[1]<< 8 | dev_desc.idVendor[0];
            pid = dev_desc.idProduct[1] << 8 | dev_desc.idProduct[0];
            printf("  VID = 0x%04X\n", vid);
            printf("  PID = 0x%04X\n", pid);
            printf("  BCD = 0x%04X\n", dev_desc.bcdUSD[1]<< 8 | dev_desc.bcdUSD[0]);
            printf("  Class = %d", intf_ptr->bInterfaceClass);
            printf("  SubClass = %d", intf_ptr->bInterfaceSubClass);
            printf("  Protocol = %d\n", intf_ptr->bInterfaceProtocol);
            fflush(stdout);

            if (NULL != (hid_instance = USB_mem_alloc_zero(sizeof(HID_DEVICE_STRUCT))))
            {
                hid_instance->DEV_HANDLE = dev_handle;
                hid_instance->INTF_HANDLE = intf_handle;
                _mqx_dll_insert_at_head(&hid_list, &hid_instance->list);
                hid_instance -> DEV_STATE = USB_DEVICE_ATTACHED;
                _lwevent_set(&USB_Task_Event, USB_TASK_CREATE);
            }
            else
            {
                printf("Memory allocate Failed!\n");
                fflush(stdout);
            }
            break;
        case USB_INTF_EVENT:
            for(temp_node=hid_list.head_ptr; temp_node != 0; temp_node=temp_node->next_ptr)
            {
                hid_instance=(HID_DEVICE_STRUCT_PTR)temp_node;
                if(hid_instance->DEV_HANDLE == dev_handle && hid_instance->INTF_HANDLE == intf_handle)
                {
                    /* Trigger Device state to Hotplug branch */
                    _lwevent_set(&(hid_instance->USB_Event), USB_EVENT_CTRL);
                    break;
                }
            }
            if(temp_node==0){
                printf("Failed to get usb interface when interfaced, case FAIL!\n");
                fflush(stdout);
            }

            break ;
        case USB_DETACH_EVENT:
            printf("----- Detach Event -----\n");
            fflush(stdout);
            if(hotplug < HOTPLUG_TIMES + 1)
                ++hotplug;
            for(temp_node=hid_list.head_ptr; temp_node != 0; temp_node=temp_node->next_ptr)
            {
                hid_instance=(HID_DEVICE_STRUCT_PTR)temp_node;
                if(hid_instance->DEV_HANDLE == dev_handle && hid_instance->INTF_HANDLE == intf_handle)
                {
                    /* Set Task_Flag to 2 as removable flag */
                    hid_instance->Task_Flag=2;
                    if(hotplug == HOTPLUG_TIMES){
                        _lwevent_set(&USB_Hotplug_Event,  USB_HOTPLUG);
                    }
                    _lwevent_set(&USB_Task_Event, USB_TASK_DESTROY);
                    /* One device one time */
                    break;
                }
            }
            if(temp_node == 0){
                printf("Failed to get usb interface when detaching, case FAIL!\n");
                fflush(stdout);
            }
            break;
        default:
            break;
    } /* EndSwitch */
} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_host_hid_ctrl_callback
 * Returned Value : None
 * Comments       :
 *     Called when a command is completed
 *END*--------------------------------------------------------------------*/

void usb_host_hid_ctrl_callback
(
 /* [IN] void * to pipe */
 _usb_pipe_handle  pipe_handle,

 /* [IN] user-defined parameter */
 void *           user_parm,

 /* [IN] buffer address */
 unsigned char *         buffer,

 /* [IN] length of data transferred */
 uint32_t           buflen,

 /* [IN] status, hopefully USB_OK or USB_DONE */
 USB_STATUS        status
 )
{ /* Body */
    HID_DEVICE_STRUCT_PTR hid_device = (HID_DEVICE_STRUCT_PTR)user_parm;
    if (status == USBERR_ENDPOINT_STALLED) {
        printf("HID %s Request is not supported!\n", api_array[hid_device->API_STATE]);
    }
    else if (status) {
        printf("HID %s Request failed!: 0x%x ... END!\n", api_array[hid_device->API_STATE], status);
    } /* Endif */
    fflush(stdout);
    /* notify application that status has changed */
    _lwevent_set(&hid_device->USB_Event, USB_EVENT_CTRL);
} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_host_hid_recv_callback
 * Returned Value : None
 * Comments       :
 *     Called when data is received on a pipe
 *END*--------------------------------------------------------------------*/

void usb_host_hid_recv_callback
(
 /* [IN] void * to pipe */
 _usb_pipe_handle  pipe_handle,

 /* [IN] user-defined parameter */
 void *           user_parm,

 /* [IN] buffer address */
 unsigned char *         buffer,

 /* [IN] length of data transferred */
 uint32_t           buflen,

 /* [IN] status, hopefully USB_OK or USB_DONE */
 USB_STATUS        status
 )
{ /* Body */
    if (status == USB_OK) {
        /* notify application that data are available */
        _lwevent_set(&(((HID_DEVICE_STRUCT_PTR)user_parm)->USB_Event), USB_EVENT_DATA);

    }
    else {
        /* notify application that data are available */
        _lwevent_set(&(((HID_DEVICE_STRUCT_PTR)user_parm)->USB_Event), USB_EVENT_DATA_CORRUPTED);
    }
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : process_mouse_buffer
 * Returned Value : None
 * Comments       :
 *   Process the data from mouse buffer
 *END*--------------------------------------------------------------------*/
void process_mouse_buffer(unsigned char * buffer, _task_id t_id)
{
    printf("In task %d", t_id);
    if (buffer[0] & 0x01)
        printf(" Left Click ");
    if (buffer[0] & 0x02)
        printf(" Right Click ");
    if (buffer[0] & 0x04)
        printf(" Middle Click ");
    if(buffer[1]){
        if(buffer[1] > 127)
            printf(" Left  ");
        else
            printf(" Right ");
    }
    if(buffer[2]){
        if(buffer[2] > 127)
            printf(" UP   ");
        else
            printf(" Down ");
    }
    if(buffer[3]){
        if(buffer[3] > 127)
            printf(" Wheel Down");
        else
            printf(" Wheel UP  ");
    }
    if(buffer[0] == 0 && buffer[1] == 0 && buffer[2] == 0 && buffer[3] == 0){
        printf(" Click released");
    }
    printf("\n");
    fflush(stdout);
}

void check_api_result(void * p, uint32_t status)
{
    /* wait for transition done */
    _lwevent_wait_ticks(&((HID_DEVICE_STRUCT_PTR)p)->USB_Event, USB_EVENT_CTRL, FALSE, 0);
    /*
     * According to API RM, the right status should be USB_OK if API returns successfully.
     * Now it is USB_STATUS_TRANSFER_QUEUED, current Stack codes are not aligned with API RM.
     * A ticeket has already been created for this issue.
     * Test codes will be updated after this issue resolved.
     */
    if (status != USB_STATUS_TRANSFER_QUEUED) {
        printf("\nError to pass and handle %s Request: %x\n", api_array[((HID_DEVICE_STRUCT_PTR)p)->API_STATE], status);
        ((HID_DEVICE_STRUCT_PTR)p)->API_ERR=1;
    }
    else{
        printf("The USB_CLASS_HID_%s command successfully!\n", api_array[((HID_DEVICE_STRUCT_PTR)p)->API_STATE], status);
    }
    fflush(stdout);
}

void create_task(){
    /* always create new task from list head */
    hid_instance=(HID_DEVICE_STRUCT_PTR)hid_list.head_ptr;
    if(hid_instance){
        hid_instance->t_id=_task_create(0, MOUSE_TASK, (uint32_t) hid_instance);
        hid_instance->Task_Flag=1;
        printf("Task created=%d\n",hid_instance->t_id);
    }
    else{
        printf("No Device instance, Test Failed!\n");
    }
}

void destroy_task(){
    temp_node=hid_list.head_ptr;
    MQX_DLL_NODE_PTR temp_node_2;
    while(temp_node)
    {
        hid_instance=(HID_DEVICE_STRUCT_PTR)temp_node;
        if(hid_instance->Task_Flag == 2)
        {
            uint32_t re=_lwevent_destroy(&hid_instance->USB_Event);
            _task_destroy(hid_instance->t_id);
            printf("Destroy Task %d \n",hid_instance->t_id);
            temp_node_2 = temp_node->next_ptr;
            _mqx_dll_remove_node(&hid_list, temp_node);
            USB_mem_free(hid_instance);
            /* go on destroying the detached task */
            temp_node=temp_node_2;

        }
        else{
            temp_node=temp_node->next_ptr;
        }
    }
}
