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
 * $FileName: cdc_serial.c$
 * $Version : 4.0.2$
 * $Date    : Aug-20-2013$
 *
 * Comments:
 *
 *   This file is an example of device drivers for the CDC class. This example
 *   demonstrates the virtual serial port capability with abstract control model.
 *   Redirects the communication from CDC device, which is connected to the board,
 *   to the serial port ttyB.
 *
 *END************************************************************************/

#include <mqx.h>
#include <lwevent.h>
#include <string.h>

#include <bsp.h>

#include <usb.h>
#include <hostapi.h>
#include <usb_host_hub_sm.h>
#include <usb_host_cdc.h>

#include "cdc_serial.h"

#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

/***************************************
 **
 ** Globals
 */

/* Table of driver capabilities this application wants to use */
static  USB_HOST_DRIVER_INFO DriverInfoTable[] =
{
    {
        {0x00,0x00},                  /* Vendor ID per USB-IF             */
        {0x00,0x00},                  /* Product ID per manufacturer      */
        USB_CLASS_COMMUNICATION,      /* Class code                       */
        USB_SUBCLASS_COM_ABSTRACT,    /* Sub-Class code                   */
        0xFF,                         /* Protocol                         */
        0,                            /* Reserved                         */
        usb_host_cdc_acm_event        /* Application call back function   */
    },
    {
        {0x00,0x00},                  /* Vendor ID per USB-IF             */
        {0x00,0x00},                  /* Product ID per manufacturer      */
        USB_CLASS_DATA,               /* Class code                       */
        0xFF,                         /* Sub-Class code                   */
        0xFF,                         /* Protocol                         */
        0,                            /* Reserved                         */
        usb_host_cdc_data_event           /* Application call back function   */
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

/* Set the CDC device name as tty0:
 ** It is fixed in this case, so only one instance can be supported
 */
char *                device_name = "tty0:";

/* The BUF_SIZE is 200. It limits the maxmum line size is 200 characters
 ** including the new line character.
 */
char buffer_in[BUF_SIZE];
char buffer_out[BUF_SIZE];
int comp_size;

const CDC_SERIAL_INIT   usb_open_param = {
#if CDC_EXAMPLE_USE_HW_FLOW
    USB_UART_NO_BLOCKING | USB_UART_HW_FLOW
#else
        USB_UART_NO_BLOCKING
#endif
};

LWEVENT_STRUCT_PTR               cdc_serial_event;
_usb_device_instance_handle      reg_device = 0;

ACM_DEVICE_STRUCT    acm_device;
DATA_DEVICE_STRUCT   data_device;

/*
 * The following describes the scenario of this case.
 *
 * Two tasks are created in this cases.
 * UART2USB_Task services as input of CDC Devices. It verifies the CDC Host USB Send function.
 * USB2UART_Task services as output of CDC Devices. It verifies the CDC Host USB Recieve function.
 */

static F_USB_INFO_T s_f_usb_info = {NULL, 0};

void Main_Task(uint32_t);
void USB2UART_Task(uint32_t);
void UART2USB_Task(uint32_t);

#define CDC_SERIAL_DELIMITER '\r'
#define USB2UART_TASK (11)
#define UART2USB_TASK (12)
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
    {            10,     Main_Task, 1000L, 10L,     "Main",  MQX_AUTO_START_TASK },
    { USB2UART_TASK, USB2UART_Task, 2000L, 12L, "USB2UART",                   0L },
    { UART2USB_TASK, UART2USB_Task, 2000L, 12L, "UART2USB",                   0L },
    { 0L,            0L,    0L,  0L,         0L,                   0L }
};

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : Main_Task
 * Returned Value : none
 * Comments       :
 *     Execution starts here
 *
 *END*--------------------------------------------------------------------*/
void Main_Task ( uint32_t param )
{ /* Body */
    USB_STATUS       status = USB_OK;
    _usb_host_handle host_handle;
    _task_id         usb2uart_id;
    _task_id         uart2usb_id;

    printf("CDC Serial test\n");
    fflush(stdout);
    if (NULL == (cdc_serial_event = _mem_alloc(sizeof(LWEVENT_STRUCT))))
    {
        printf("\nMemory allocation failed");
        _task_block();
    }
    if (USB_OK != _lwevent_create(cdc_serial_event, LWEVENT_AUTO_CLEAR))
    {
        printf("\nlwevent create failed");
        _task_block();
    }
    if (MQX_OK != _lwevent_create(&acm_device.acm_event, LWEVENT_AUTO_CLEAR)) {
        printf("\nlwevent create failed");
        _task_block();
    }
    if (MQX_OK != _lwevent_create(&data_device.data_event, LWEVENT_AUTO_CLEAR)) {
        printf("\nlwevent create failed");
        _task_block();
    }

    /* _usb_host_init needs to be done with interrupts disabled */
    _int_disable();

    _int_install_unexpected_isr();
    if (MQX_OK != _usb_host_driver_install(USBCFG_DEFAULT_HOST_CONTROLLER)) {
        printf("\n Driver installation failed");
        _int_enable();
        _task_block();
    }

    /*
     ** It means that we are going to act like host, so we initialize the
     ** host stack. This call will allow USB system to allocate memory for
     ** data structures, it uses later (e.g pipes etc.).
     */
    status = _usb_host_init (USBCFG_DEFAULT_HOST_CONTROLLER, &host_handle);

    if (status != USB_OK)
    {
        printf("\nUSB Host Initialization failed. STATUS: %x", status);
        _int_enable();
        _task_block();
    }

    /*
     ** since we are going to act as the host driver, register the driver
     ** information for wanted class/subclass/protocols
     */
    status = _usb_host_driver_info_register (
            host_handle,
            DriverInfoTable
            );
    if (status != USB_OK) {
        printf("\nDriver Registration failed. STATUS: %x", status);
        _int_enable();
        _task_block();
    }

    _int_enable();


    printf("\nInitialization passed. Plug-in CDC device to USB port first.\n");
    printf("This example requires that the CDC device uses HW flow.\n");
    printf("If your device does not support HW flow, then set \n");
    printf("CDC_EXAMPLE_USE_HW_FLOW in cdc_serial.h to zero and rebuild example project.\n");
    printf("\nTry typing some string... Press ENTER to send to CDC device, then you will\n");
    printf("see them echoed back from the UART port of the device.\n");

    usb2uart_id = _task_create(0, USB2UART_TASK, 0);
    if (usb2uart_id == MQX_NULL_TASK_ID) {
        printf("\nCould not create USB2UART_task\n");
        printf("Case Failed\n");
        _task_block();
    }

    uart2usb_id = _task_create(0, UART2USB_TASK, 0);
    if (uart2usb_id == MQX_NULL_TASK_ID) {
        printf("Case Failed\n");
        printf("\nCould not create UART2USB_task\n");
    }

    while (1) {
        /* Wait until the USB CDC Serial Device recognized and registered */
        _lwevent_wait_ticks(cdc_serial_event, CDC_SERIAL_DEV_REGISTERED, TRUE, 0);

        /* Here the device_name already binded with CDC Serial Device by CDC register API */
        if (NULL == (s_f_usb_info.f_usb = fopen(device_name, (void *) &usb_open_param))) {
            printf("\nInternal error occured");
            printf("Case Failed\n");
            _task_block(); /* internal error occured */
        }
        s_f_usb_info.cnt++;

        /* USB CDC Serial Device is opened and ready for input from UART */
        _lwevent_set(cdc_serial_event, CDC_SERIAL_FUSB_OPENED_UART2USB | CDC_SERIAL_FUSB_OPENED_USB2UART | CDC_SERIAL_READ_DONE);
        /* Wait USB2UART or UART2USB task done, usually it is triggered by detaching CDC device */
        _lwevent_wait_ticks(cdc_serial_event, CDC_SERIAL_UART2USB_DONE | CDC_SERIAL_USB2UART_DONE, FALSE, 0);
        /* Close the CDC devices for it is not available again */
        if(s_f_usb_info.cnt) {
            if(IO_OK != fclose(s_f_usb_info.f_usb)) {
                printf("\nInternal error occured");
                printf("Case Failed\n");
                _task_block(); /* internal error occured */
            }
            s_f_usb_info.cnt--;
        }
    }

}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : USB2UART_Task
 * Returned Value : none
 * Comments       : Read content from USB Serial Device and output them to standard output
 *                 which is UART for most boards.
 *     Execution starts here
 *
 *END*--------------------------------------------------------------------*/
void USB2UART_Task ( uint32_t param )
{ /* Body */
    MQX_FILE_PTR f_uart;
    int num_done, num = 0;
    int i;
    int start_num;

    while (1) {
        /* Wait device attached and registered */
        _lwevent_wait_ticks(cdc_serial_event, CDC_SERIAL_FUSB_OPENED_USB2UART, TRUE, 0);
        _lwevent_clear(cdc_serial_event, CDC_SERIAL_DEV_DETACH_USB2UART);
        num_done = num = i = 0;

        if (NULL == (f_uart = stdout)) {
            printf("\nInternal error occured");
            printf("Case Failed\n");
            _task_block(); /* internal error occured */
        }

        /* read data from USB */
        while (1) {
            /* read data only when CDC Device send buffer is ready */
            _lwevent_wait_ticks(cdc_serial_event, CDC_SERIAL_WRITE_DONE, TRUE, 0);
            while(1){
                num_done=-1;
                while(!num_done){
                    /* read characters from USB */
                    num_done = fread(buffer_in + num, 1, sizeof(buffer_in) / sizeof(buffer_in[0]) - num, s_f_usb_info.f_usb);
                    if (IO_ERROR == num_done) {
                        if (ferror(s_f_usb_info.f_usb) != USBERR_NO_INTERFACE) {
                            printf("\nUnexpected error occured");
                            printf("Case Failed\n");
                            _task_block(); /* unexpected error occured */
                        }
                        /* Device detached */
                        _lwevent_set(cdc_serial_event, CDC_SERIAL_USB2UART_DONE);
                        break; /* device was detached */
                    }
                    num += num_done;
                }

                if(!num){
                    /* write back done, trigger UART2USB task again */
                    _lwevent_set(cdc_serial_event, CDC_SERIAL_READ_DONE);
                    break;
                }
                else{
                    start_num=0;
                    /* Comparing the send and recieve buffer, useful especially when test file transferring */
                    if(strncmp(buffer_in, buffer_out, comp_size)){
                        printf("Sent and Recieved string Not Matched, Case Failed\n");
                        _task_block();
                    }
                }
                while (num) {
                    /* Write back to stdout */
                    num_done = fwrite(buffer_in+start_num, sizeof(buffer_in[0]), num, f_uart);
                    if (IO_ERROR == num_done) {
                        printf("\nUnexpected error occured");
                        printf("Case Failed\n");
                        _task_block(); /* unexpected error occured */
                    }
                    num -= num_done;
                    start_num += num_done;
                }

                if(MQX_OK == _lwevent_wait_ticks(cdc_serial_event, CDC_SERIAL_DEV_DETACH_USB2UART, TRUE, 1)) {
                    _lwevent_set(cdc_serial_event, CDC_SERIAL_USB2UART_DONE);
                    //_lwevent_set(cdc_serial_event, CDC_SERIAL_READ_DONE);
                    break;
                }
            }
            /* if(MQX_OK == _lwevent_wait_ticks(cdc_serial_event, CDC_SERIAL_DEV_DETACH_USB2UART, TRUE, 1)) {
               _lwevent_set(cdc_serial_event, CDC_SERIAL_USB2UART_DONE);
               _lwevent_set(cdc_serial_event, CDC_SERIAL_READ_DONE);
               break;
               }*/
        }
    }
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : UART2USB_Task
 * Returned Value : none
 * Comments       :
 *     Execution starts here
 *
 *END*--------------------------------------------------------------------*/
void UART2USB_Task ( uint32_t param )
{ /* Body */
    MQX_FILE_PTR f_uart;
    int num_done, num = 0;
    char i;
    uint32_t flags;
    uint32_t read_done = 0;
    int detached=0;
    int start_num;
    while (1){
        /* Wait device attached and registered */
        _lwevent_wait_ticks(cdc_serial_event, CDC_SERIAL_FUSB_OPENED_UART2USB, TRUE, 0);
        _lwevent_clear(cdc_serial_event, CDC_SERIAL_DEV_DETACH_UART2USB);
        num_done = num = i = 0;

        if (NULL == (f_uart = stdin)) {
            printf("\nInternal error occured");
            printf("Case Failed\n");
            _task_block(); /* internal error occured */
        }

        flags = IO_SERIAL_NON_BLOCKING | IO_SERIAL_ECHO;
        if (IO_OK != ioctl(f_uart, IO_IOCTL_SERIAL_SET_FLAGS, &flags)) {
            printf("\nInternal error occured");
            printf("Case Failed\n");
            _task_block(); /* internal error occured */
        }
        /* Write data to CDC device */
        while (1) {
            /* wait the USB2UART task done to avoid corrupting the content of CDC device sending buffer */
            _lwevent_wait_ticks(cdc_serial_event, CDC_SERIAL_READ_DONE, TRUE, 0);
            while(1){
                /* read data from UART */
                num_done = fread(buffer_out + num, 1, sizeof(buffer_out) / sizeof(buffer_out[0]) - num, f_uart);
                if (IO_ERROR == num_done) {
                    printf("\nUnexpected error occured");
                    printf("Case Failed\n");
                    _task_block(); /* unexpected error occured */
                    break; /* device was detached */
                }else if(num_done) {
                    if(CDC_SERIAL_DELIMITER == *(buffer_out + num + num_done -1 ) || BUF_SIZE == num+num_done) {
                        read_done = 1;
                        start_num=0;
                        comp_size=num+num_done;
                    }
                    num += num_done;
                }

                _time_delay(1);
                /* write them to USB */
                while (num && read_done)
                {
                    num_done = fwrite(buffer_out+start_num, sizeof(buffer_out[0]), num, s_f_usb_info.f_usb);
                    if (IO_ERROR == num_done)
                    {
                        if (ferror(s_f_usb_info.f_usb) != USBERR_NO_INTERFACE)
                        {
                            printf("\nUnexpected error occured");
                            printf("Case Failed\n");
                            _task_block(); /* unexpected error occured */
                        }
                        _lwevent_set(cdc_serial_event, CDC_SERIAL_UART2USB_DONE);
                        break; /* device was detached */
                    }
                    num -= num_done;
                    start_num+=num_done;
                }
                if(!num && read_done)
                {
                    read_done=0;
                    _lwevent_set(cdc_serial_event, CDC_SERIAL_WRITE_DONE);
                    break;
                }
                if(MQX_OK == _lwevent_wait_ticks(cdc_serial_event, CDC_SERIAL_DEV_DETACH_UART2USB, TRUE, 1))
                {
                    detached=1;
                    break;
                }
                _sched_yield();
            }
            if(detached)
            {
                detached=0;
                _lwevent_set(cdc_serial_event, CDC_SERIAL_UART2USB_DONE);
                break;
            }
        }
    } /* Endbody */
}


/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_host_cdc_acm_event
 * Returned Value : None
 * Comments       :
 *     Called when acm interface has been attached, detached, etc.
 *END*--------------------------------------------------------------------*/

void usb_host_cdc_acm_event
(
 /* [IN] void * to device instance */
 _usb_device_instance_handle      dev_handle,

 /* [IN] void * to interface descriptor */
 _usb_interface_descriptor_handle intf_handle,

 /* [IN] code number for event causing callback */
 uint32_t                          event_code
 )
{ /* Body */
    INTERFACE_DESCRIPTOR_PTR   intf_ptr = (INTERFACE_DESCRIPTOR_PTR)intf_handle;

    switch (event_code) {
        case USB_CONFIG_EVENT:
            /* Drop through into attach, same processing */
        case USB_ATTACH_EVENT: {
                                   USB_CDC_DESC_ACM_PTR    acm_desc = NULL;
                                   USB_CDC_DESC_CM_PTR     cm_desc = NULL;
                                   USB_CDC_DESC_HEADER_PTR header_desc = NULL;
                                   USB_CDC_DESC_UNION_PTR  union_desc = NULL;

                                   /* finds all the descriptors in the configuration */
                                   if (USB_OK != usb_class_cdc_get_acm_descriptors(dev_handle,
                                               intf_handle,
                                               &acm_desc,
                                               &cm_desc,
                                               &header_desc,
                                               &union_desc))
                                       break;

                                   /* initialize new interface members and select this interface */
                                   if (USB_OK != _usb_hostdev_select_interface(dev_handle,
                                               intf_handle, (void *)&acm_device.CLASS_INTF))
                                   {
                                       break;
                                   }
                                   /* set all info got from descriptors to the class interface struct */
                                   usb_class_cdc_set_acm_descriptors((void *)&acm_device.CLASS_INTF,
                                           acm_desc, cm_desc, header_desc, union_desc);

                                   /* link all already registered data interfaces to this ACM control, if needed */
                                   if (USB_OK != usb_class_cdc_bind_data_interfaces(dev_handle, (void *)&acm_device.CLASS_INTF)) {
                                       break;
                                   }
                                   break;
                               }
        case USB_INTF_EVENT:
                               {
                                   CLASS_CALL_STRUCT_PTR   acm_parser;
                                   USB_STATUS              status;

                                   if (NULL == (acm_parser = usb_class_cdc_get_ctrl_interface(intf_handle)))
                                       break;
                                   if (MQX_OK != usb_class_cdc_acm_use_lwevent(acm_parser, &acm_device.acm_event))
                                       break;
                                   status = usb_class_cdc_init_ipipe(acm_parser);
                                   if ((status != USB_OK) && (status != USBERR_OPEN_PIPE_FAILED))
                                       break;
                                   break;
                               }

        case USB_DETACH_EVENT:
                               {
                                   CLASS_CALL_STRUCT_PTR acm_parser;

                                   if (NULL == (acm_parser = usb_class_cdc_get_ctrl_interface(intf_handle)))
                                       break;
                                   usb_class_cdc_unbind_data_interfaces(acm_parser);
                                   _lwevent_set(cdc_serial_event, CDC_SERIAL_DEV_DETACH_USB2UART|CDC_SERIAL_DEV_DETACH_UART2USB);
                                   break;
                               }

        default:
                               break;
    } /* EndSwitch */
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_host_cdc_data_event
 * Returned Value : None
 * Comments       :
 *     Called when data interface has been attached, detached, etc.
 *END*--------------------------------------------------------------------*/

void usb_host_cdc_data_event
(
 /* [IN] void * to device instance */
 _usb_device_instance_handle      dev_handle,

 /* [IN] void * to interface descriptor */
 _usb_interface_descriptor_handle intf_handle,

 /* [IN] code number for event causing callback */
 uint32_t                          event_code
 )
{ /* Body */
    INTERFACE_DESCRIPTOR_PTR   intf_ptr =
        (INTERFACE_DESCRIPTOR_PTR)intf_handle;

    //   fflush(stdout);
    switch (event_code) {
        case USB_CONFIG_EVENT:
            /* Drop through into attach, same processing */
        case USB_ATTACH_EVENT: {
                                   /* This data interface could be controlled by some control interface,
                                    * which could be already initialized (or not). We have to find
                                    * that interface. Then we need to bind this interface with
                                    * found control interface. */
                                   INTERFACE_DESCRIPTOR_PTR   if_desc;

                                   if (USB_OK != usb_class_cdc_get_ctrl_descriptor(dev_handle,
                                               intf_handle,
                                               &if_desc))
                                       break;

                                   /* initializes interface members and selects it */
                                   if (USB_OK != _usb_hostdev_select_interface(dev_handle,
                                               intf_handle, (void *)&data_device.CLASS_INTF))
                                   {
                                       break;
                                   }
                                   /* binds this data interface with its control interface, if possible */
                                   if (USB_OK != usb_class_cdc_bind_acm_interface((void *)&data_device.CLASS_INTF,
                                               if_desc))
                                   {
                                       break;
                                   }
                                   break;
                               }
        case USB_INTF_EVENT:
                               {
                                   CLASS_CALL_STRUCT_PTR data_parser;
                                   if (NULL == (data_parser = usb_class_cdc_get_data_interface(intf_handle)))
                                       break;
                                   if (MQX_OK != usb_class_cdc_data_use_lwevent(data_parser, &data_device.data_event))
                                       break;
                                   if (USB_OK == usb_class_cdc_install_driver(data_parser, device_name))
                                   {
                                       if (((USB_DATA_CLASS_INTF_STRUCT_PTR) (data_parser->class_intf_handle))->BOUND_CONTROL_INTERFACE != NULL) {
                                           if (reg_device == 0) {
                                               reg_device = dev_handle;
                                               _lwevent_set(cdc_serial_event, CDC_SERIAL_DEV_REGISTERED);
                                           }
                                       }
                                   }
                                   break;
                               }

        case USB_DETACH_EVENT:
                               {
                                   CLASS_CALL_STRUCT_PTR data_parser;

                                   if (NULL == (data_parser = usb_class_cdc_get_data_interface(intf_handle)))
                                       break;
                                   /* unbind data interface */
                                   if (USB_OK != usb_class_cdc_unbind_acm_interface(data_parser))
                                       break;
                                   if (USB_OK != usb_class_cdc_uninstall_driver(data_parser))
                                       break;

                                   if (reg_device == dev_handle) {
                                       reg_device = 0;
                                   }
                                   break;
                               }

        default:
                               break;
    } /* EndSwitch */
} /* Endbody */

/* EOF */
