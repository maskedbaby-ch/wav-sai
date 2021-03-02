#ifndef __usb_task_h__
#define __usb_task_h__
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
 *FileName: USB_Task.h$
 *Version : 4.0.2$
 *Date    : Jul-26-2012$
 *
 * Comments:
 *
 *   This file contains the Test specific data structures and defines
 *
 *END************************************************************************/

#define  USB_DEVICE_IDLE                   (0)
#define  USB_DEVICE_ATTACHED               (1)
#define  USB_DEVICE_CONFIGURED             (2)
#define  USB_DEVICE_SET_INTERFACE_STARTED  (3)
#define  USB_DEVICE_INTERFACED             (4)
#define  USB_DEVICE_DETACHED               (5)
#define  USB_DEVICE_OTHER                  (6)

#define  MAX_ID                            (10)


typedef struct device_struct {
    /* This must be the first member of this structure, because sometimes we can use it
     ** as pointer to CLASS_CALL_STRUCT, other time as a pointer to DEVICE_STRUCT
     */
    CLASS_CALL_STRUCT                ccs;
    /* Mount point, 'c' for "c:" */
    char                             mount;
    char                             dev_id[MAX_ID];
    char                             pm_id[MAX_ID];
    char                             fs_id[4][MAX_ID];
    char                             pm_id_group[4][MAX_ID];
    void *                           usb_fs_handle;  /* handle to save the usb_fs structure */
    char                             part_num; /* store the partition numbers */

} DEVICE_STRUCT,  * DEVICE_STRUCT_PTR;


extern void usb_host_device_event(_usb_device_instance_handle,
        _usb_interface_descriptor_handle, uint32_t);
extern void usb_host_mass_device_event(_usb_device_instance_handle, _usb_interface_descriptor_handle, uint32_t);
extern void USB_task( uint32_t param);

#endif
