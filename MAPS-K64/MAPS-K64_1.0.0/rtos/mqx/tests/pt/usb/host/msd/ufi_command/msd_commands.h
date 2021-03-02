#ifndef __msc_commands_h__
#define __msc_commands_h__
/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
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
* $FileName: msd_commands.h$
* $Version : 3.8.7.0$
* $Date    : Jul-2-2012$
*
* Comments:
*
*   This file contains mass storage class application types and definitions.
*
*END************************************************************************/

#include "host_dev_list.h"
#include "host_common.h"

/* Application-specific definitions */
#define  MAX_PENDING_TRANSACTIONS      16
#define  MAX_FRAME_SIZE                1024
#define  HOST_CONTROLLER_NUMBER        USBCFG_DEFAULT_HOST_CONTROLLER
#define  MAX_MASS_DEVICES              (1)
#define  TEST_WRITE                    (1)

#define  BUFF_SIZE                     (0x2000)

#define  USB_DEVICE_IDLE                   (0)
#define  USB_DEVICE_ATTACHED               (1)
#define  USB_DEVICE_CONFIGURED             (2)
#define  USB_DEVICE_SET_INTERFACE_STARTED  (3)
#define  USB_DEVICE_INTERFACED             (4)
#define  USB_DEVICE_DETACHED               (5)
#define  USB_DEVICE_OTHER                  (6)

/*
** Following structs contain all states and pointers
** used by the application to control/operate devices.
*/

typedef struct device_struct
{
   uint32_t                          dev_state;  /* Attach/detach state */
   PIPE_BUNDLE_STRUCT_PTR           pbs;        /* Device & pipes */
   _usb_device_instance_handle      dev_handle;
   _usb_interface_descriptor_handle intf_handle;
   CLASS_CALL_STRUCT                class_intf; /* Class-specific info */
} DEVICE_STRUCT, * DEVICE_STRUCT_PTR;


/* Alphabetical list of Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

static void usb_host_mass_device_event(_usb_device_instance_handle, _usb_interface_descriptor_handle, uint32_t);
static void usb_host_mass_ctrl_callback(_usb_pipe_handle, void *, unsigned char *, uint32_t, uint32_t);
static void usb_host_mass_bulk_callback(USB_STATUS, void *, void *, uint32_t);
static void usb_host_mass_test_storage();

#ifdef __cplusplus
}
#endif


#endif

/* EOF */
