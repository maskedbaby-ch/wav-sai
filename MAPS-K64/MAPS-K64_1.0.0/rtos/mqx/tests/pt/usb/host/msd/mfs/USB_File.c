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
 *FileName: USB_File.c$
 *Version : 4.0.2$
 *Date    : Jul-26-2013$
 *
 * Comments:
 *
 *   This file contains the initialization and menu source code for
 *   the USB mass storage MFS test example program using USB mass storage
 *   link driver.
 *
 *END************************************************************************/

#include "MFS_USB.h"

#include "USB_file.h"
#include "USB_task.h"

extern unsigned char mount_pt;
extern LWSEM_STRUCT   USB_Stick;
void * usb_filesystem_install(
        void *     usb_handle,
        char *    block_device_name,
        char *    partition_manager_name,
        char *    file_system_name
        )
{
    uint32_t                   partition_number;
    unsigned char              *dev_info;
    int32_t                    error_code;
    USB_FILESYSTEM_STRUCT_PTR  usb_fs_ptr;
    DEVICE_STRUCT_PTR          dev_ptr = (DEVICE_STRUCT_PTR)usb_handle;
    char                       drive[3];
    int                        p0=0; /* used to de-select partition */


    usb_fs_ptr = _mem_alloc_system_zero(sizeof(USB_FILESYSTEM_STRUCT));
    if (usb_fs_ptr==NULL)
    {
        printf("mem_allocation error\n");
        return NULL;
    }

    /* Install USB device */
    error_code = _io_usb_mfs_install(block_device_name, 0, (void *)usb_handle);
    if (error_code != MQX_OK)
    {
        printf("Error while installing USB device (0x%X)\n", error_code);
        return NULL;
    }
    usb_fs_ptr->DEV_NAME = block_device_name;

    /* Open the USB mass storage  device */
    _time_delay(500);
    usb_fs_ptr->DEV_FD_PTR = fopen(block_device_name, 0);

    if (usb_fs_ptr->DEV_FD_PTR == NULL)
    {
        printf("Unable to open USB device\n");
        usb_filesystem_uninstall(usb_fs_ptr);
        return NULL;
    }

    _io_ioctl(usb_fs_ptr->DEV_FD_PTR, IO_IOCTL_SET_BLOCK_MODE, NULL);

    /* Get the vendor information and display it */
    printf("\n************************************************************************\n");
    _io_ioctl(usb_fs_ptr->DEV_FD_PTR, IO_IOCTL_GET_VENDOR_INFO, &dev_info);
    printf("Vendor Information:     %-1.8s Mass Storage Device\n",dev_info);
    _io_ioctl(usb_fs_ptr->DEV_FD_PTR, IO_IOCTL_GET_PRODUCT_ID, &dev_info);
    printf("Product Identification: %-1.16s\n",dev_info);
    _io_ioctl(usb_fs_ptr->DEV_FD_PTR, IO_IOCTL_GET_PRODUCT_REV, &dev_info);
    printf("Product Revision Level: %-1.4s\n",dev_info);
    printf("************************************************************************\n");

    /* Try to install the partition manager */
    error_code = _io_part_mgr_install(usb_fs_ptr->DEV_FD_PTR, partition_manager_name, 0);
    if (error_code != MFS_NO_ERROR)
    {
        printf("Error while initializing partition manager: %s\n", MFS_Error_text((uint32_t)error_code));
        usb_filesystem_uninstall(usb_fs_ptr);
        return NULL;
    }
    usb_fs_ptr->PM_NAME = partition_manager_name;

    /* Open partition manager */
    usb_fs_ptr->PM_FD_PTR = fopen(partition_manager_name, NULL);
    if (usb_fs_ptr->PM_FD_PTR == NULL)
    {
        error_code = ferror(usb_fs_ptr->PM_FD_PTR);
        printf("Error while opening partition manager: %s\n", MFS_Error_text((uint32_t)error_code));
        usb_filesystem_uninstall(usb_fs_ptr);
        return NULL;
    }

    /* judge it is partitioned u-disk or not */
    partition_number = 1;
    error_code = _io_ioctl(usb_fs_ptr->PM_FD_PTR, IO_IOCTL_SEL_PART, &partition_number);

    if (error_code == MFS_NO_ERROR)
    {
        /* Partition exist for this device, try to mount all partitions */

        printf("partition exists...\n");
        printf("Installing MFS over partition...\n");

        for(usb_fs_ptr->part_num=0; mount_pt < MAX_PART; (usb_fs_ptr->part_num)++)
        {

            int j=usb_fs_ptr->part_num + 1; /* Partition begins with 1 */


            /* de-select the partition */
            _io_ioctl(usb_fs_ptr->PM_FD_PTR, IO_IOCTL_SEL_PART, &p0);
            /* Select partition */
            _io_ioctl(usb_fs_ptr->PM_FD_PTR, IO_IOCTL_SEL_PART, &j);
            error_code = _io_ioctl(usb_fs_ptr->PM_FD_PTR, IO_IOCTL_VAL_PART, NULL);
            if (error_code != MFS_NO_ERROR)
            {
                /* Error happens when install first partition, exit */
                if(!usb_fs_ptr->part_num){
                    printf("Partition %d validation Failed!\n", j);
                    usb_filesystem_uninstall(usb_fs_ptr);
                    return NULL;
                }
                else{
                    /* Two situation:
                     * S1: only one partition; S2: one of the follow partitions broken
                     * At least the first partition installed successfully, silence
                     * and untouch the left partitions.
                     */
                    break;
                }
            }

            /* install partition manage for each partition */
            char temp;
            temp='0'+usb_fs_ptr->part_num;
            strncat(dev_ptr->pm_id_group[usb_fs_ptr->part_num], partition_manager_name,3);
            strncat(dev_ptr->pm_id_group[usb_fs_ptr->part_num], &temp,1);
            strncat(dev_ptr->pm_id_group[usb_fs_ptr->part_num], partition_manager_name+3,1);
            error_code = _io_part_mgr_install(usb_fs_ptr->DEV_FD_PTR, dev_ptr->pm_id_group[usb_fs_ptr->part_num], 0);
            if (error_code != MFS_NO_ERROR)
            {
                printf("Error while initializing partition manager: %s\n", MFS_Error_text((uint32_t)error_code));
                usb_filesystem_uninstall(usb_fs_ptr);
                return NULL;
            }
            usb_fs_ptr->PM_NAME_GROUP[usb_fs_ptr->part_num] = dev_ptr->pm_id_group[usb_fs_ptr->part_num];


            /* Open partition manager */
            usb_fs_ptr->PM_FD_PTR_GROUP[usb_fs_ptr->part_num] = fopen(usb_fs_ptr->PM_NAME_GROUP[usb_fs_ptr->part_num], NULL);
            if (usb_fs_ptr->PM_FD_PTR_GROUP[usb_fs_ptr->part_num] == NULL)
            {
                error_code = ferror(usb_fs_ptr->PM_FD_PTR_GROUP[usb_fs_ptr->part_num]);
                printf("Error while opening partition manager: %s\n", MFS_Error_text((uint32_t)error_code));
                usb_filesystem_uninstall(usb_fs_ptr);
                return NULL;
            }

            /* de-select partition */
            _io_ioctl(usb_fs_ptr->PM_FD_PTR_GROUP[usb_fs_ptr->part_num], IO_IOCTL_SEL_PART, &p0);
            /* Select partition for install mfs basing one the selected partition*/
            _io_ioctl(usb_fs_ptr->PM_FD_PTR_GROUP[usb_fs_ptr->part_num], IO_IOCTL_SEL_PART, &j);

            /* dynamic generate fs name if multi-partitons exsit for the device */
            temp='0'+usb_fs_ptr->part_num;
            strncat(dev_ptr->fs_id[usb_fs_ptr->part_num], file_system_name,1);
            strncat(dev_ptr->fs_id[usb_fs_ptr->part_num], &temp,1);
            strncat(dev_ptr->fs_id[usb_fs_ptr->part_num], file_system_name+1,1);
            printf("Install mfs fs\n");
            error_code = _io_mfs_install(usb_fs_ptr->PM_FD_PTR_GROUP[usb_fs_ptr->part_num], dev_ptr->fs_id[usb_fs_ptr->part_num], j);

            if (error_code != MFS_NO_ERROR)
            {
                printf("Error initializing MFS over partition: %s\n", MFS_Error_text((uint32_t)error_code));
                usb_filesystem_uninstall(usb_fs_ptr);
                return NULL;
            }
            usb_fs_ptr->FS_NAME[usb_fs_ptr->part_num] = dev_ptr->fs_id[usb_fs_ptr->part_num];

            /* de-select the partition to prepare file open */
            _io_ioctl(usb_fs_ptr->PM_FD_PTR_GROUP[usb_fs_ptr->part_num], IO_IOCTL_SEL_PART, &p0);
            usb_fs_ptr->FS_FD_PTR[usb_fs_ptr->part_num] = fopen(dev_ptr->fs_id[usb_fs_ptr->part_num], NULL);
            error_code = ferror(usb_fs_ptr->FS_FD_PTR[usb_fs_ptr->part_num]);
            if (error_code == MFS_NOT_A_DOS_DISK)
            {
                printf("%s is NOT A DOS DISK! You must format before using it.\n", dev_ptr->fs_id[usb_fs_ptr->part_num]);
            }
            else if (error_code != MFS_NO_ERROR)
            {
                printf("Error opening filesystem: %s\n", MFS_Error_text((uint32_t)error_code));
                usb_filesystem_uninstall(usb_fs_ptr);
                return NULL;
            }
            mount_pt++;

            printf("USB device installed to %s\n", dev_ptr->fs_id[usb_fs_ptr->part_num]);
        }

    }
    else {
        if(mount_pt < MAX_PART)
        {

            printf("No partition over USB device...\n");
            printf("Installing MFS over USB device...\n");
            usb_fs_ptr->part_num=1;
            //strncat(usb_fs_ptr->FS_NAME[0], file_system_name, 2);
            strncat(dev_ptr->fs_id[0], file_system_name,3);
            error_code = _io_mfs_install(usb_fs_ptr->DEV_FD_PTR, dev_ptr->fs_id[0], 0);
            if (error_code != MFS_NO_ERROR)
            {
                printf("Error initializing MFS: %s\n", MFS_Error_text((uint32_t)error_code));
                usb_filesystem_uninstall(usb_fs_ptr);
                return NULL;
            }
            /* Open file system */
            else if (error_code == MFS_NO_ERROR)
            {
                //usb_fs_ptr->FS_NAME[0] = file_system_name;
                usb_fs_ptr->FS_NAME[0] = dev_ptr->fs_id[0];
                usb_fs_ptr->FS_FD_PTR[0] = fopen(usb_fs_ptr->FS_NAME[0], NULL);
                error_code = ferror(usb_fs_ptr->FS_FD_PTR[0]);
                if (error_code == MFS_NOT_A_DOS_DISK)
                {
                    printf("NOT A DOS DISK! You must format to continue.\n");
                }
                else if (error_code != MFS_NO_ERROR)
                {
                    printf("Error opening filesystem: %s\n", MFS_Error_text((uint32_t)error_code));
                    usb_filesystem_uninstall(usb_fs_ptr);
                    return NULL;
                }

                mount_pt++;
                printf("USB device installed to %s\n", usb_fs_ptr->FS_NAME[0]);
            }
        }
    }

    return (void *)usb_fs_ptr;
}

/* workaround to force close unhandled file handle
   by hotplug during data transferring or careless
   APP or user
 */
static void mfs_force_close( MQX_FILE_PTR file_ptr)
{
    MFS_DRIVE_STRUCT_PTR      drive_ptr;
    MFS_lock(file_ptr, &drive_ptr);
    while(_queue_get_size(&drive_ptr->HANDLE_LIST) > 0){
        MFS_Free_handle(drive_ptr,  (MFS_HANDLE_PTR)_queue_head(&drive_ptr->HANDLE_LIST));
    }
    MFS_unlock(drive_ptr, FALSE);
}

void usb_filesystem_uninstall( USB_FILESYSTEM_STRUCT_PTR  usb_fs_ptr)
{
    int32_t  error_code;

    if (usb_fs_ptr == NULL) {
        return;
    }
    _int_disable();

    /*
     * close the fs level files for each partition
     */
    for(int i=0; i<usb_fs_ptr->part_num; i++)
    {
        /* Force to close unclosed file handles caused by hotplug or careless user */
        if (usb_fs_ptr->FS_FD_PTR[i] != NULL)
        {
            mfs_force_close(usb_fs_ptr->FS_FD_PTR[i]);
        }
        /* Close the filesystem */
        if ((usb_fs_ptr->FS_FD_PTR[i] != NULL) && (MQX_OK != fclose(usb_fs_ptr->FS_FD_PTR[i])))
        {
            printf("Error while closing filesystem.\n");
        }

        /* Uninstall MFS */
        error_code = _io_dev_uninstall(usb_fs_ptr->FS_NAME[i]);
        if (error_code != MFS_NO_ERROR)
        {
            printf("Error while uninstalling filesystem.\n");
        }

        /* Close partition mounted with mfs */
        if ((usb_fs_ptr->PM_FD_PTR_GROUP[i] != NULL) && (MQX_OK != fclose(usb_fs_ptr->PM_FD_PTR_GROUP[i])))
        {
            printf("Error while closing partition.\n");
        }

        /* Uninstall partition manager */
        if ((usb_fs_ptr->PM_NAME_GROUP[i] != NULL) && (MFS_NO_ERROR != _io_dev_uninstall(usb_fs_ptr->PM_NAME_GROUP[i])))
        {
            printf("Error while uninstalling partition manager.\n");
        }

        mount_pt--;
    }

    /* Close partition management used to judge whether there is partition */
    if ((usb_fs_ptr->PM_FD_PTR != NULL) && (MQX_OK != fclose(usb_fs_ptr->PM_FD_PTR)))
    {
        printf("Error while closing partition.\n");
    }

    /* Uninstall partition manager */
    error_code = _io_dev_uninstall(usb_fs_ptr->PM_NAME);
    if (error_code != MFS_NO_ERROR)
    {
        printf("Error while uninstalling partition manager.\n");
    }

    /* Close USB device */
    if ((usb_fs_ptr->DEV_FD_PTR != NULL) && (MQX_OK != fclose(usb_fs_ptr->DEV_FD_PTR)))
    {
        printf("Unable to close USB device.\n");
    }

    _int_enable();

    /* Uninstall USB device */
    error_code = _io_dev_uninstall(usb_fs_ptr->DEV_NAME);
    if (error_code != MQX_OK)
    {
        printf("Error while uninstalling USB device.\n");
    }

    _mem_free(usb_fs_ptr);

    printf("USB device removed.\n");
}



/* EOF */
