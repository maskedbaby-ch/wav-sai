/**HEADER********************************************************************
* 
* Copyright (c) 2008, 2013 - 2014 Freescale Semiconductor;
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
* $FileName: disk.h$
* $Version : 
* $Date    : 
*
* Comments:
*
* @brief The file contains Macro's and functions needed by the disk 
*        application
*
*****************************************************************************/


#ifndef _DISK_H
#define _DISK_H  1

#include "usb_descriptor.h"
/******************************************************************************
 * Constants - None
 *****************************************************************************/

/******************************************************************************
 * Macro's
 *****************************************************************************/
#define RAM_DISK_APP         (1)
#define SD_CARD_APP			 (0)


#if RAM_DISK_APP
    /* Length of Each Logical Address Block */
    #define LENGTH_OF_EACH_LAB              (512)    
    /* total number of logical blocks present */
    #define TOTAL_LOGICAL_ADDRESS_BLOCKS_NORMAL (48)
    /* Net Disk Size */
    #define DISK_SIZE_NORMAL (TOTAL_LOGICAL_ADDRESS_BLOCKS_NORMAL * LENGTH_OF_EACH_LAB)
#endif

#define LOGICAL_UNIT_SUPPORTED          (1)

#define SUPPORT_DISK_LOCKING_MECHANISM  (0) /*1: TRUE; 0:FALSE*/

#define MSD_RECV_BUFFER_SIZE            (512)
#define MSD_SEND_BUFFER_SIZE            (512)
/*****************************************************************************
 * Global variables
 *****************************************************************************/
 
/******************************************************************************
 * Types
 *****************************************************************************/
typedef struct _disk_variable_struct
{
    msd_handle_t app_handle;
    uint32_t start_app;
#if RAM_DISK_APP
    uint8_t storage_disk[DISK_SIZE_NORMAL];
#endif  
    uint8_t disk_lock;   
}disk_struct_t; 

/*****************************************************************************
 * Global Functions
 *****************************************************************************/
extern void TestApp_Init(void);
extern void Disk_App(void);

#endif


/* EOF */
