#ifndef __hidmouse_h__
#define __hidmouse_h__
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
* $FileName: hidmouse.h$
* $Version : 4.0.2$
* $Date    : Jul-12-2013$
*
* Comments:
*
*   This file contains mouse-application types and definitions.
*
*END************************************************************************/

#ifdef __USB_OTG__
#include "otgapi.h"
#include "devapi.h"
#else
#include "hostapi.h"
#endif
#include <mqx_dll.h>


/***************************************
**
** Application-specific definitions
*/

/* Used to initialize USB controller */
#define MAX_FRAME_SIZE           1024
#define HOST_CONTROLLER_NUMBER   USBCFG_DEFAULT_HOST_CONTROLLER

#define  USB_DEVICE_API                      (0)
#define  USB_DEVICE_INTERFACED                   (1)
#define  USB_DEVICE_INUSE                        (2)
#define  USB_DEVICE_ATTACHED                     (3)
#define  USB_DEVICE_HOTPLUG              (4)
#define  USB_DEVICE_INTERFACE_INPROGRESS                 (5)

#define  USB_DEVICE_GET_PROTOCOL                    (0)
#define  USB_DEVICE_SET_PROTOCOL                (1)
#define  USB_DEVICE_GET_REPORT                          (2)
#define  USB_DEVICE_SET_REPORT                          (3)
#define  USB_DEVICE_GET_IDLE                            (4)
#define  USB_DEVICE_SET_IDLE                            (5)

/*
** Following structs contain all states and void *s
** used by the application to control/operate devices.
*/

typedef struct hid_device_struct {
    MQX_DLL_NODE        list;
    CLASS_CALL_STRUCT                CLASS_INTF; /* Class-specific info */
   uint32_t                          DEV_STATE;  /* Attach/detach state */
   _usb_device_instance_handle      DEV_HANDLE;
   _usb_interface_descriptor_handle INTF_HANDLE;
   _task_id t_id;
   LWEVENT_STRUCT USB_Event;
   uint8_t API_STATE;
   uint8_t API_ERR;
   uint8_t Task_Flag;
} HID_DEVICE_STRUCT,  * HID_DEVICE_STRUCT_PTR;


/* Alphabetical list of Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

#if 0
    void otg_service(_usb_otg_handle, uint32_t);
#endif

void usb_host_hid_recv_callback(_usb_pipe_handle, void *, unsigned char *, uint32_t, USB_STATUS);
void usb_host_hid_ctrl_callback(_usb_pipe_handle, void *, unsigned char *, uint32_t, USB_STATUS);
void usb_host_hid_mouse_event(_usb_device_instance_handle, _usb_interface_descriptor_handle, USB_STATUS);
void process_mouse_buffer(unsigned char *, _task_id);
void check_api_result(void *, uint32_t);
void create_task();
void destroy_task();

#ifdef __cplusplus
}
#endif


#endif

/* EOF */
