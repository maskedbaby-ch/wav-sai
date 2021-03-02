/**HEADER********************************************************************
 *
 * Copyright (c) 2008 - 2013 Freescale Semiconductor;
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
 *FileName: USB_Task.c$
 *Version : 4.0.2$
 *Date    : Jul-26-2013$
 *
 * Comments:
 *
 *   This file is the main file for usb disk filesystem demo. Note that this example
 *   is a multi tasking example and needs an operating system to run. This
 *   means that customers who are not using MQX should change the operating system
 *   dependent code. An attempt has been made to comment out the code
 *   however, a programmer must review all lines of code to ensure that
 *   it correctly compiles with their libraries of operating system and
 *   targetcompiler.
 *
 *
 *END************************************************************************/

#include "MFS_USB.h"

#include <string.h>
#include <lwmsgq.h>

#include <hostapi.h>
#include <mqx_host.h>
#include <host_dev_list.h>
#include <usb_host_msd_bo.h>
#include <usb_host_hub_sm.h>


#include "usb_task.h"
#include "usb_file.h"

#define USB_EVENT_ATTACH    (1)
#define USB_EVENT_DETACH    (2)
#define USB_EVENT_INTF      (3)

typedef struct {
    CLASS_CALL_STRUCT_PTR               ccs;     /* class call struct of MSD instance */
    uint8_t                             body;    /* message body one of USB_EVENT_xxx as defined above */
    _usb_device_instance_handle         dev_handle;     /* store device information used in APP */
    _usb_interface_descriptor_handle    intf_handle;    /* store device information used in APP */
} usb_msg_t;

LWSEM_STRUCT   USB_Stick;

/* Store mounting point used. c: is the first one, bit #0 assigned, z: is the last one, bit #25 assigned
 * If multi partitions in one u-disk, they are named like c0: c1:.
 * For MFS limitation, it can only support maximum 4 partitions&devices.
 * In the program will supported partitions and devices will be installed on mfs. But only the first four
 * partitions are accessable
 */
uint32_t              fs_mountp = 0;
unsigned char         mount_pt;

/* The granularity of message queue is one message. Its size is the multiplier of _mqx_max_type. Get that multiplier */
#define USB_TASKQ_GRANM ((sizeof(usb_msg_t) - 1) / sizeof(_mqx_max_type) + 1)
_mqx_max_type  usb_taskq[20 * USB_TASKQ_GRANM * sizeof(_mqx_max_type)]; /* prepare message queue for 20 events */

/* Table of driver capabilities this application want to use */
static const USB_HOST_DRIVER_INFO ClassDriverInfoTable[] =
{
    /* Vendor ID Product ID Class Sub-Class Protocol Reserved Application call back */
    /* Floppy drive */
    {{0x00,0x00}, {0x00,0x00}, USB_CLASS_MASS_STORAGE, USB_SUBCLASS_MASS_UFI, USB_PROTOCOL_MASS_BULK, 0, usb_host_mass_device_event },

    /* USB 2.0 hard drive */
    {{0x00,0x00}, {0x00,0x00}, USB_CLASS_MASS_STORAGE, USB_SUBCLASS_MASS_SCSI, USB_PROTOCOL_MASS_BULK, 0, usb_host_mass_device_event},

    /* USB hub */
    {{0x00,0x00}, {0x00,0x00}, USB_CLASS_HUB, USB_SUBCLASS_HUB_NONE, USB_PROTOCOL_HUB_ALL, 0, usb_host_hub_device_event},

    /* End of list */
    {{0x00,0x00}, {0x00,0x00}, 0,0,0,0, NULL}
};

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_host_mass_device_event
 * Returned Value : None
 * Comments       :
 *     called when a mass storage device has been attached, detached, etc.
 *END*--------------------------------------------------------------------*/

void usb_host_mass_device_event
(
 /* [IN] pointer to device instance */
 _usb_device_instance_handle      dev_handle,

 /* [IN] pointer to interface descriptor */
 _usb_interface_descriptor_handle intf_handle,

 /* [IN] code number for event causing callback */
 uint32_t          event_code
 )
{
    DEVICE_STRUCT_PTR          device;
    usb_msg_t                  msg;

    switch (event_code) {
        case USB_CONFIG_EVENT:
            /* Drop through into attach, same processing */
        case USB_ATTACH_EVENT:
            /* Here, the device starts its lifetime */
            printf("\nAttaching detected\n");
            fflush(stdout);
            device = (DEVICE_STRUCT_PTR) _mem_alloc_zero(sizeof(DEVICE_STRUCT));
            if (device == NULL)
                break;
            msg.ccs = &device->ccs;
            msg.body = USB_EVENT_ATTACH;
            msg.dev_handle=dev_handle;
            msg.intf_handle=intf_handle;
            if (LWMSGQ_FULL == _lwmsgq_send(usb_taskq, (uint32_t *) &msg, 0)) {
                printf("Could not inform USB task about device attached\n");
            }
            break;

        case USB_INTF_EVENT:
            if (USB_OK != usb_class_mass_get_app(dev_handle, intf_handle, (CLASS_CALL_STRUCT_PTR *) &device))
                break;
            msg.ccs = &device->ccs;
            msg.body = USB_EVENT_INTF;
            if (LWMSGQ_FULL == _lwmsgq_send(usb_taskq, (uint32_t *) &msg, 0)) {
                printf("Could not inform USB task about device interfaced\n");
            }
            break;

        case USB_DETACH_EVENT:
            if (USB_OK != usb_class_mass_get_app(dev_handle, intf_handle, (CLASS_CALL_STRUCT_PTR *) &device))
                break;
            msg.ccs = &device->ccs;
            msg.body = USB_EVENT_DETACH;
            printf("Detach Event\n");
            /* Trigger APP detach handle which will release the memery for device_struct */
            if (LWMSGQ_FULL == _lwmsgq_send(usb_taskq, (uint32_t *) &msg, 0)) {
                printf("Could not inform USB task about device detached\n");
            }
            break;

        default:
            break;
    }
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : USB_task
 * Returned Value : None
 * Comments       :
 *     First function called. This routine install/uninstall mfs based on u-disk
 *     when u-disk attach/detach.
 *END*--------------------------------------------------------------------*/

void USB_task(uint32_t param)
{
    _usb_host_handle     host_handle;
    USB_STATUS           error;
    usb_msg_t            msg;
    char                 drive[3];
    DEVICE_STRUCT_PTR	 dev_ptr;

    mount_pt = 0;

#if DEMOCFG_USE_POOLS && defined(DEMOCFG_MFS_POOL_ADDR) && defined(DEMOCFG_MFS_POOL_SIZE)
    _MFS_pool_id = _mem_create_pool((void *)DEMOCFG_MFS_POOL_ADDR, DEMOCFG_MFS_POOL_SIZE);
#endif

    /* This event will inform other tasks that the filesystem on USB was successfully installed */
    _lwsem_create(&USB_Stick, 0);

    if (MQX_OK != _lwmsgq_init(usb_taskq, 20, USB_TASKQ_GRANM)) {
        // lwmsgq_init failed
        printf("USB_Stick initial Failed, Case Fail!\n");
        _task_block();
    }

    USB_lock();
    _int_install_unexpected_isr();
    if (MQX_OK != _usb_host_driver_install(USBCFG_DEFAULT_HOST_CONTROLLER)) {
        printf("\n Driver installation failed");
        USB_unlock();
        _task_block();
    }

    error = _usb_host_init(USBCFG_DEFAULT_HOST_CONTROLLER, &host_handle);
    if (error == USB_OK) {
        error = _usb_host_driver_info_register(host_handle, (void *)ClassDriverInfoTable);
        if (error == USB_OK) {
            error = _usb_host_register_service(host_handle, USB_SERVICE_HOST_RESUME,NULL);
        }
    }

    USB_unlock();

    if (error != USB_OK) {
        printf("Host Ininital or Driver Register Failed. Case Fail!\n");
        _task_block();
    }

    for (;;) {
        /* Wait for event sent as a message */
        _lwmsgq_receive(&usb_taskq, (_mqx_max_type *) &msg, LWMSGQ_RECEIVE_BLOCK_ON_EMPTY, 0, 0);

        if (msg.body == USB_EVENT_ATTACH) {
            /* select interface */
            if (USB_OK != _usb_hostdev_select_interface(msg.dev_handle, msg.intf_handle, msg.ccs))
            {
                printf("select interface failed, case failed\n");
                _task_block();
            }
        } else if (msg.body == USB_EVENT_INTF && fs_mountp != 0x3FFC)  { /* mountpoints c: to z: are not all already used */

            /* get free mountpoint as a string like "c:" */
            for (drive[0] = 'c'; drive[0] <= 'z'; drive[0]++)
                if (!(fs_mountp & (1 << (drive[0] - 'a'))))
                    break;
            drive[1] = ':';
            drive[2] = 0;

            dev_ptr=(DEVICE_STRUCT_PTR)msg.ccs;

            /* Must maintain device name, partion name, mfs name for each device
               dynamically because kernel data just store the string point for io
               devices. Otherwise, the second plugin u-disk will be not recognized
               any more
               */
            strcat(dev_ptr->dev_id, "USB");
            strncat(dev_ptr->dev_id, drive, 2);
            strcat(dev_ptr->pm_id, "PM");
            strncat(dev_ptr->pm_id, drive, 2);
            /* Dynamically create mfs name in the install function */
            dev_ptr->usb_fs_handle = usb_filesystem_install( (void *) msg.ccs, dev_ptr->dev_id, dev_ptr->pm_id, drive);

            if (dev_ptr->usb_fs_handle) {

                dev_ptr->mount = drive[0];

                // Mark file system as mounted
                fs_mountp |= 1 << (dev_ptr->mount - 'a');
                // Unlock the USB_Stick = signal to the application as available
                _lwsem_post(&USB_Stick);
            }
        } else if (msg.body == USB_EVENT_DETACH) {
            dev_ptr = (DEVICE_STRUCT_PTR)msg.ccs;

            if (dev_ptr->mount >= 'a' && dev_ptr->mount <= 'z') {
                // Lock the USB_Stick = mark as unavailable
                _lwsem_wait(&USB_Stick);

                // Remove the file system
                usb_filesystem_uninstall(dev_ptr->usb_fs_handle);
                // Mark file system as unmounted
                fs_mountp &= ~(1 << (dev_ptr->mount - 'a'));
            }

            /* Here, the device finishes its lifetime */
            _mem_free(dev_ptr);
        }
    }
}
