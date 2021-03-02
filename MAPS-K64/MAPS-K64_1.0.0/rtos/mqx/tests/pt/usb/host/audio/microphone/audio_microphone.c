/**HEADER********************************************************************
 *
 * Copyright (c) 2011-2013 Freescale Semiconductor;
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
 * $FileName: audio_microphone.c$
 * $Version :
 * $Date    :
 *
 * Comments:
 *
 *   This file is an example of device drivers for the Audio host class. This example
 *   demonstrates the audio transfer capability of audio host class with audio devices.
 * Limitation:
 *   1. Audio control request no response now for some device.
 * The volume get/set request may never return.  In code we will check it and report it.
 * In new stack, it will get fixed.
 *   2. About the record quality. Now the recorded audio file is stored in the SD card.
 * The speed of SD card is too slow to store/save the recorded audio stream in time. So when
 * the record sample rate rises, the record quality will become bad. During test we find when
 * the record format is 48K 16bit 2 channel, the audio stream speed is 192KB/s, there will be
 * several blocks of audio stream data lost.
 *
 *END************************************************************************/

#include "audio_microphone.h"
#include "usb_host_audio.h"

#include <bsp.h>
#include "mqx_host.h"
#include "fio.h"
#include <usb_host_hub_sm.h>
#include "mfs.h"
#include "sh_mfs.h"
#include "sh_audio.h"
#include "shell.h"
#include "sdcard.h"
#include <lwevent.h>
#include <spi.h>
#include <part_mgr.h>
#include <hwtimer.h>

#if ! SHELLCFG_USES_MFS
#error This application requires SHELLCFG_USES_MFS \
    defined non-zero in user_config.h. Please recompile libraries with this option.
#endif

#if defined BSP_SDCARD_ESDHC_CHANNEL
#if ! BSPCFG_ENABLE_ESDHC
#error This application requires BSPCFG_ENABLE_ESDHC \
    defined non-zero in user_config.h. Please recompile libraries with this option.
#endif

#elif defined BSP_SDCARD_SDHC_CHANNEL

#if ! BSPCFG_ENABLE_SDHC
#error This application requires BSPCFG_ENABLE_SDHC defined non-zero in user_config.h. Please recompile libraries with this option.
#endif

#elif defined (BSP_TWRMCF51CN) || defined (BSP_TWRPXS20) || defined (BSP_TWR_K20D72M)

#if ! BSPCFG_ENABLE_SPI1
#error This application requires BSPCFG_ENABLE_SPI1 defined non-zero in user_config.h. Please recompile libraries with this option.
#endif

#else
#if ! BSPCFG_ENABLE_SPI0
#error This application requires BSPCFG_ENABLE_SPI0 \
    defined non-zero in user_config.h. Please recompile libraries with this option.
#endif

#endif

#if defined (BSP_SDCARD_SPI_CHANNEL)
#define SDCARD_COM_CHANNEL BSP_SDCARD_SPI_CHANNEL
#elif defined (BSP_SDCARD_ESDHC_CHANNEL)
#define SDCARD_COM_CHANNEL BSP_SDCARD_ESDHC_CHANNEL
#elif defined (BSP_SDCARD_SDHC_CHANNEL)
#define SDCARD_COM_CHANNEL BSP_SDCARD_SDHC_CHANNEL
#else
#error "SDCARD low level communication device not defined!"
#endif
/***************************************
 **
 ** Macros
 ****************************************/
#define USB_EVENT_CTRL 0x01
#define USB_EVENT_DATA 0x02
extern uint32_t   requests;
extern uint32_t   remainder ;

/***************************************
 **
 ** Global functions
 ****************************************/

/***************************************
 **
 ** Local functions
 ****************************************/
static void Audio_Task(uint32_t);
static void Sdcard_task(uint32_t temp);
static void Shell_task(uint32_t temp);
USB_STATUS usb_class_audio_control_get_descriptor_id
(
 /* [IN] pointer to device instance */
 _usb_device_instance_handle                dev_handle,

 /* [IN] pointer to interface descriptor */
 _usb_interface_descriptor_handle           intf_handle,

 /* [IN] AC descriptor ID */
 int8_t         id,

 /* [OUT] pointer to requested descriptor ID */
 void ** ac_desc
 );

USB_STATUS usb_class_audio_control_get_it_descriptor
(
 /* [IN] pointer to device instance */
 _usb_device_instance_handle                dev_handle,

 /* [IN] pointer to interface descriptor */
 _usb_interface_descriptor_handle           intf_handle,

 /* [IN] AC descriptor ID */
 int16_t            type,

 /* [OUT] pointer to requested descriptor ID */
 void ** it_desc
 );

USB_STATUS usb_class_audio_control_get_fu_descriptor
(
 /* [IN] pointer to device instance */
 _usb_device_instance_handle                dev_handle,

 /* [IN] pointer to interface descriptor */
 _usb_interface_descriptor_handle           intf_handle,

 /* [IN] AC descriptor ID */
 int8_t         sid,

 /* [OUT] pointer to requested descriptor ID */
 void ** fu_desc
 );

/***************************************
 **
 ** Global variables
 ****************************************/
/* buffer for receiving an audio data packet, must be cache line aligned */
char                                    *wav_recv_buff;
/* ping-pong buffer for accumulating audio data packets */
char                                    wav_sdcard_buff0[REC_BLOCK_SIZE];
/* ping-pong buffer for accumulating audio data packets */
char                                    wav_sdcard_buff1[REC_BLOCK_SIZE];
char                                    *wav_sdcard_buff = wav_sdcard_buff0;
/* audio packet size */
uint32_t                                 packet_size;
/* audio control device */
volatile AUDIO_CONTROL_DEVICE_STRUCT    audio_control = { 0 };
/* audio stream device */
volatile AUDIO_CONTROL_DEVICE_STRUCT    audio_stream  = { 0 };
/* check device type error */
//static uint8_t                           error_state   =  0;
/* audio stream descriptor */
USB_AUDIO_STREAM_DESC_FORMAT_TYPE_PTR   frm_type_desc = NULL;
/* flag to mark the target audio stream from speaker/micrphone stream */
uint8_t target=0;
/* block record ready event */
extern LWEVENT_STRUCT_PTR block_rec;
/* bytes remain for record */
extern uint32_t                          remainder;
/* final packet flag*/
extern bool                          final_packet;
/* end point descriptor */
ENDPOINT_DESCRIPTOR_PTR                 endp;
/* hwtimer handle */
HWTIMER audio_timer;

/* Global pointer to target audio stream interface */
INTERFACE_DESCRIPTOR_PTR   g_intf_ptr;
AUDIO_COMMAND audio_com;
volatile static int16_t                  cur_volume, min_volume, max_volume, res_volume;
volatile static int16_t                  physic_volume;
volatile uint8_t                         host_cur_volume = 5;
volatile int16_t                         device_volume_step;
volatile uint8_t *g_cur_mute;
volatile uint8_t *g_cur_vol;
volatile uint8_t *g_max_vol;
volatile uint8_t *g_min_vol;
volatile uint8_t *g_res_vol;
volatile uint8_t *g_cur_samp;
extern volatile uint8_t buf0_flag;
extern volatile uint8_t buf1_flag;

LWEVENT_STRUCT                   USB_Audio_FU_Request;
uint32_t                         fu_status;
#define                          FU_WAIT_TICKS      (50)

USB_AUDIO_CTRL_DESC_IT_PTR       it_desc  = NULL;
USB_AUDIO_CTRL_DESC_OT_PTR       ot_desc  = NULL;
USB_AUDIO_CTRL_DESC_FU_PTR       fu_desc  = NULL;
USB_AUDIO_CTRL_DESC_HEADER_PTR   header_desc = NULL;


/* MQX template list */
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
    {10,    Audio_Task,         2500L,    10L, "Audio Task", MQX_AUTO_START_TASK},
    {11,    Sdcard_task,        2000L,    11L, "Sdcard Task",MQX_AUTO_START_TASK},
    {12,    Shell_task,         2000L,    12L, "Shell Task", MQX_AUTO_START_TASK},
    {0L,    0L,                 0L,       0L,  0L,           0L }
};

/* Shell list */
const SHELL_COMMAND_STRUCT Shell_commands[] = {
    { "cd",        Shell_cd },
    { "del",       Shell_del },
    { "dir",       Shell_dir },
    { "help",      Shell_help },
    { "read",      Shell_read },
    { "mkdir",     Shell_mkdir },
    { "ren",       Shell_rename },
    { "rmdir",     Shell_rmdir },
    { "record",    Shell_record },
    { "mute",      Shell_mute },
    { "?",         Shell_command_list },
    { NULL,        NULL }
};

/***************************************
 **
 ** Local variables
 ****************************************/
/* device type string*/
//static char                         *device_string;

/* Transfer Types */
static const char *TransferType[TRANSFER_TYPE_NUM] =
{
    "Control",
    "Isochronous",
    "Bulk",
    "Interrupt"
};
/* Sync Types */
//static const char *SyncType[SYNC_TYPE_NUM] =
const char *SyncType[SYNC_TYPE_NUM] =
{
    "No synchronization",
    "Asynchronous",
    "Adaptive",
    "Synchrounous"
};
/* Data Types */
static const char *DataType[DATA_TYPE_NUM] =
{
    "Data endpoint",
    "Feedback endpoint",
    "Implicit feedback",
    "Reserved"
};

/* Table of driver capabilities this application wants to use */
static  USB_HOST_DRIVER_INFO DriverInfoTable[] =
{
    {
        {0x00,0x00},                  /* Vendor ID per USB-IF             */
        {0x00,0x00},                  /* Product ID per manufacturer      */
        USB_CLASS_AUDIO,              /* Class code                       */
        USB_SUBCLASS_AUD_CONTROL,     /* Sub-Class code                   */
        0xFF,                         /* Protocol                         */
        0,                            /* Reserved                         */
        usb_host_audio_control_event  /* Application call back function   */
    },
    {
        {0x00,0x00},                  /* Vendor ID per USB-IF             */
        {0x00,0x00},                  /* Product ID per manufacturer      */
        USB_CLASS_AUDIO,              /* Class code                       */
        USB_SUBCLASS_AUD_STREAMING,   /* Sub-Class code                   */
        0xFF,                         /* Protocol                         */
        0,                            /* Reserved                         */
        usb_host_audio_stream_event   /* Application call back function   */
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

/*FUNCTION*---------------------------------------------------------------
 *
 * Function Name : audio_timer_isr
 * Comments  : Callback called by hwtimer every elapsed period
 *
 *END*----------------------------------------------------------------------*/
static void audio_timer_isr(void *p)
{
    //Do the stuff only if both USB and the sync mechanism are ready
    if ((USB_DEVICE_INUSE == audio_stream.DEV_STATE) && (block_rec != NULL))
    {
        /* the attached device is microphone */
        /* Recv data */
        usb_audio_recv_data(
                (CLASS_CALL_STRUCT_PTR)&audio_control.CLASS_INTF,
                (CLASS_CALL_STRUCT_PTR)&audio_stream.CLASS_INTF,
                usb_host_audio_tr_callback,
                NULL, packet_size,
                (unsigned char *)wav_recv_buff
                );
    }
}

#ifdef BSP_SDCARD_GPIO_CS
/*FUNCTION*---------------------------------------------------------------
 *
 * Function Name : set_CS
 * Comments  : This function sets chip select signal to enable/disable memory
 *
 *END*----------------------------------------------------------------------*/

static void set_CS (uint32_t cs_mask, uint32_t logic_level, void *user_data)
{
    LWGPIO_STRUCT_PTR gpiofd = user_data;

    if (cs_mask & BSP_SDCARD_SPI_CS)
    {
        lwgpio_set_value(gpiofd, logic_level ? LWGPIO_VALUE_HIGH : LWGPIO_VALUE_LOW);
    }
}
#endif

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : USB_Audio_Get_Packet_Size
 * Returned Value : None
 * Comments       :
 *     This function gets the frequency supported by the attached device.
 *
 *END*--------------------------------------------------------------------*/
uint32_t USB_Audio_Get_Packet_Size
(
 /* [IN] Point to format type descriptor */
 USB_AUDIO_STREAM_DESC_FORMAT_TYPE_PTR format_type_desc,
 uint32_t rate
 )
{
    uint32_t packet_size_tmp;
    /* calculate packet size to send to the device each mS.*/
    /* packet_size = (sample frequency (Hz) /1000) *
     * (bit resolution/8) * number of channels */
    packet_size_tmp = (rate
            *(format_type_desc->bBitResolution/8)
            *(format_type_desc->bNrChannels)
            /1000);
    return (packet_size_tmp);
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : Audio_Task
 * Returned Value : none
 * Comments       :
 *     Execution starts here
 *
 *END*--------------------------------------------------------------------*/
static void Audio_Task ( uint32_t param )
{ /* Body */
    USB_STATUS           status = USB_OK;
    _usb_host_handle     host_handle;
    FEATURE_CONTROL_STRUCT control_feature;
    uint8_t bSamFreqType_index;

    /* _usb_host_init needs to be done with interrupts disabled */
    _int_disable();
    _int_install_unexpected_isr();
    _usb_host_driver_install(USBCFG_DEFAULT_HOST_CONTROLLER);

    /*
     ** It means that we are going to act like host, so we initialize the
     ** host stack. This call will allow USB system to allocate memory for
     ** data structures, it uses later (e.g pipes etc.).
     */
    status = _usb_host_init (
            USBCFG_DEFAULT_HOST_CONTROLLER,   /* Use value in header file */
            &host_handle);             /* Returned pointer */

    if (USB_OK != status)
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
    if (USB_OK != status) {
        printf("\nDriver Registration failed. STATUS: %x", status);
        _int_enable();
        _task_block();
    }

    if (MQX_OK !=_lwevent_create(&USB_Audio_FU_Request, LWEVENT_AUTO_CLEAR)){
        printf("\n_lwevent_create USB_Audio_FU_Request failed.\n");
    }

    _int_enable();

    if (NULL == (wav_recv_buff = _mem_alloc(MAX_ISO_PACKET_SIZE)))
    {
        printf("\nMemory allocation for ISO buffer failed.\n");
        _task_block();
    }

    /* Initialization of hwtimer */
    if (MQX_OK != hwtimer_init(&audio_timer, &BSP_HWTIMER1_DEV, BSP_HWTIMER1_ID, 5))
    {
        printf("\nHwtimer initialization failed.\n");
        _task_block();
    }
    hwtimer_set_freq(&audio_timer, BSP_HWTIMER1_SOURCE_CLK, AUDIO_MICROPHONE_FREQUENCY);
    hwtimer_callback_reg(&audio_timer,(HWTIMER_CALLBACK_FPTR)audio_timer_isr, NULL);
    if (NULL == (g_cur_mute = _mem_alloc(4)))
    {
        printf("\nMemory allocation for mute buffer failed.\n");
        _task_block();
    }
    if (NULL == (g_cur_vol = _mem_alloc(4)))
    {
        printf("\nMemory allocation for current volume buffer failed.\n");
        _task_block();
    }
    if (NULL == (g_max_vol = _mem_alloc(4)))
    {
        printf("\nMemory allocation for max volume buffer failed.\n");
        _task_block();
    }
    if (NULL == (g_min_vol = _mem_alloc(4)))
    {
        printf("\nMemory allocation for min volume buffer failed.\n");
        _task_block();
    }
    if (NULL == (g_res_vol = _mem_alloc(4)))
    {
        printf("\nMemory allocation for volume resolution failed.\n");
        _task_block();
    }
    if (NULL == (g_cur_samp = _mem_alloc(4)))
    {
        printf("\nMemory allocation for current sampling failed.\n");
        _task_block();
    }

    printf("USB Host Audio Demo\n"
            "Waiting for USB Audio Microphone Device to be attached...\n");

    for(;;)
    {
        /* Get information of audio interface */
        if (USB_DEVICE_INTERFACED == audio_stream.DEV_STATE)
        {
            audio_com.CLASS_PTR = (CLASS_CALL_STRUCT_PTR)&audio_control.CLASS_INTF;
            audio_com.CALLBACK_FN = usb_host_audio_request_ctrl_callback;
            audio_com.CALLBACK_PARAM = (void *)&control_feature;
            control_feature.FU = USB_AUDIO_CTRL_FU_VOLUME;
            /* Get min volume */
            if(USB_OK == usb_class_audio_feature_command(&audio_com,0,(void *)g_min_vol,USB_AUDIO_GET_MIN_VOLUME))
            {
                fu_status=_lwevent_wait_ticks(&USB_Audio_FU_Request, FU_VOLUME_MASK, FALSE, FU_WAIT_TICKS);
                if(USB_OK != fu_status)
                    printf("Audio volume control request no response.\n");
            }
            else
                printf("This device does not support GEG_MIN_VOLUME request.\n");
            /* Get max volume */
            if(USB_OK == usb_class_audio_feature_command(&audio_com,0,(void *)g_max_vol,USB_AUDIO_GET_MAX_VOLUME))
            {
                fu_status=_lwevent_wait_ticks(&USB_Audio_FU_Request, FU_VOLUME_MASK, FALSE, FU_WAIT_TICKS);
                if(USB_OK != fu_status)
                    printf("Audio volume control request no response.\n");
            }
            else
                printf("This device does not support GEG_MAX_VOLUME request.\n");
            control_feature.FU = USB_AUDIO_CTRL_FU_VOLUME;
            /* Get res volume */
            if(USB_OK == usb_class_audio_feature_command(&audio_com,0,(void *)g_res_vol,USB_AUDIO_GET_RES_VOLUME))
            {
                fu_status=_lwevent_wait_ticks(&USB_Audio_FU_Request, FU_VOLUME_MASK, FALSE, FU_WAIT_TICKS);
                if(USB_OK != fu_status)
                    printf("Audio volume control request no response.\n");
            }
            else
                printf("This device does not support GEG_RES_VOLUME request.\n");
            _time_delay(10);
            min_volume = (g_min_vol[1]<<8)|(g_min_vol[0]);
            max_volume = (g_max_vol[1]<<8)|(g_max_vol[0]);
            res_volume = (g_res_vol[1]<<8)|(g_res_vol[0]);
            _lwevent_clear(&USB_Audio_FU_Request, FU_VOLUME_MASK);
            control_feature.FU = 0;
            /* Synchronize host volume and device volume */
            device_volume_step = (int16_t)(((int16_t)(max_volume) - (int16_t)(min_volume))/(HOST_MAX_VOLUME - HOST_MIN_VOLUME));
            cur_volume = (int16_t)(min_volume + device_volume_step*host_cur_volume);
            /* Calculate physical volume(dB) */
            physic_volume = ((int16_t)(cur_volume)*39)/10000;
            g_cur_vol[0] = (uint8_t)((int16_t)(cur_volume)&0x00FF);
            g_cur_vol[1] = (uint8_t)((int16_t)(cur_volume)>>8);
            /* Unmute and set volume for audio device*/
            control_feature.FU = USB_AUDIO_CTRL_FU_VOLUME;
            if(USB_OK == usb_class_audio_feature_command(&audio_com,0,(void *)g_min_vol,USB_AUDIO_SET_CUR_VOLUME))
            {
                fu_status=_lwevent_wait_ticks(&USB_Audio_FU_Request, FU_VOLUME_MASK, FALSE, FU_WAIT_TICKS);
                if(USB_OK != fu_status)
                    printf("Audio volume control request no response.\n");
            }
            else
                printf("This device does not support SET_CUR_VOLUME request.\n");
            if(USB_OK == usb_class_audio_feature_command(&audio_com,0,(void *)g_max_vol,USB_AUDIO_SET_CUR_VOLUME))
            {
                fu_status=_lwevent_wait_ticks(&USB_Audio_FU_Request, FU_VOLUME_MASK, FALSE, FU_WAIT_TICKS);
                if(USB_OK != fu_status)
                    printf("Audio volume control request no response.\n");
            }
            else
                printf("This device does not support SET_CUR_VOLUME request.\n");

            /* Audio device information */
            printf("Audio device information:\n");
            //printf("   - Device type    : %s\n", device_string);
            for (bSamFreqType_index =0; bSamFreqType_index < frm_type_desc->bSamFreqType; bSamFreqType_index++)
            {
                if(0 == bSamFreqType_index)
                {
                    g_cur_samp[0] = frm_type_desc->tSamFreq[3*bSamFreqType_index + 0];
                    g_cur_samp[1] = frm_type_desc->tSamFreq[3*bSamFreqType_index + 1];
                    g_cur_samp[2] = frm_type_desc->tSamFreq[3*bSamFreqType_index + 2];
                }
                printf("   - Frequency device support      : %d Hz\n", (frm_type_desc->tSamFreq[3*bSamFreqType_index + 2] << 16) |
                        (frm_type_desc->tSamFreq[3*bSamFreqType_index + 1] << 8)  |
                        (frm_type_desc->tSamFreq[3*bSamFreqType_index + 0] << 0));
            }
            /* print device information */
            printf("   - Bit resolution : %d bits\n", frm_type_desc->bBitResolution);
            printf("   - Number of channels : %d channels\n", frm_type_desc->bNrChannels);
            printf("   - Transfer type : %s\n", TransferType[(endp->bmAttributes)&EP_TYPE_MASK]);
            printf("   - Sync type : %s\n", SyncType[(endp->bmAttributes>>2)&EP_TYPE_MASK]);
            printf("   - Usage type : %s\n", DataType[(endp->bmAttributes>>4)&EP_TYPE_MASK]);
            printf("Type \"record a:\\filename.wav 48000 5\" to record the file at sample rate 48K"\
                    " with 5 (s) length \n");
            audio_stream.DEV_STATE = USB_DEVICE_INUSE;
        }
        _time_delay(1);
    }
} /* Endbody */

/*TASK*-----------------------------------------------------------------
 *
 * Function Name  : Sdcard_task
 * Returned Value : void
 * Comments       :
 *
 *END------------------------------------------------------------------*/
static void Sdcard_task(uint32_t temp)
{
    bool      inserted = TRUE, readonly = FALSE, last = FALSE;
    _mqx_int     error_code;
    _mqx_uint    param;
    MQX_FILE_PTR com_handle, sdcard_handle, filesystem_handle, partman_handle;
    char         filesystem_name[] = "a:";
    char         partman_name[] = "pm:";
#if defined BSP_SDCARD_GPIO_DETECT
    LWGPIO_STRUCT      sd_detect;
#endif
#if defined BSP_SDCARD_GPIO_PROTECT
    LWGPIO_STRUCT      sd_protect;
#endif
#ifdef BSP_SDCARD_GPIO_CS

    LWGPIO_STRUCT          sd_cs;
    SPI_CS_CALLBACK_STRUCT callback;

#endif

    /* Open low level communication device */
    com_handle = fopen (SDCARD_COM_CHANNEL, NULL);

    if (NULL == com_handle)
    {
        printf("Error installing communication handle.\n");
        _task_block();
    }

#ifdef BSP_SDCARD_GPIO_CS

    /* Open GPIO file for SPI CS signal emulation */
    error_code = lwgpio_init(
            &sd_cs,
            BSP_SDCARD_GPIO_CS,
            LWGPIO_DIR_OUTPUT,
            LWGPIO_VALUE_NOCHANGE
            );
    if (!error_code)
    {
        printf("Initializing GPIO with associated pins failed.\n");
        _task_block();
    }
    lwgpio_set_functionality(&sd_cs,BSP_SDCARD_CS_MUX_GPIO);
    lwgpio_set_attribute(&sd_cs, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);
    /* Set CS call back */
    callback.MASK = BSP_SDCARD_SPI_CS;
    callback.CALLBACK = set_CS;
    callback.USERDATA = &sd_cs;
    if (SPI_OK != ioctl (com_handle, IO_IOCTL_SPI_SET_CS_CALLBACK, &callback))
    {
        printf ("Setting CS call back failed.\n");
        _task_block();
    }

#endif

#if defined BSP_SDCARD_GPIO_DETECT
    /* Init GPIO pins for other SD card signals */
    error_code = lwgpio_init(&sd_detect, BSP_SDCARD_GPIO_DETECT, \
            LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE);
    if (!error_code)
    {
        printf("Initializing GPIO with sdcard detect pin failed.\n");
        _task_block();
    }
    /*Set detect and protect pins as GPIO Function */
    lwgpio_set_functionality(&sd_detect,BSP_SDCARD_DETECT_MUX_GPIO);
    lwgpio_set_attribute(&sd_detect, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);
#endif

#if defined BSP_SDCARD_GPIO_PROTECT
    /* Init GPIO pins for other SD card signals */
    error_code = lwgpio_init(&sd_protect, BSP_SDCARD_GPIO_PROTECT, \
            LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE);
    if (!error_code)
    {
        printf("Initializing GPIO with sdcard protect pin failed.\n");
        _task_block();
    }
    /*Set detect and protect pins as GPIO Function */
    lwgpio_set_functionality(&sd_protect,BSP_SDCARD_PROTECT_MUX_GPIO);
    lwgpio_set_attribute(&sd_protect, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);
#endif

    /* Install SD card device */
    error_code = _io_sdcard_install("sdcard:", (void *)&_bsp_sdcard0_init, com_handle);
    if ( MQX_OK != error_code)
    {
        printf("Error installing SD card device (0x%x)\n", error_code);
        _task_block();
    }

    for (;;)
    {

#if defined BSP_SDCARD_GPIO_DETECT
        inserted = !lwgpio_get_value(&sd_detect);
#endif

#if defined BSP_SDCARD_GPIO_PROTECT
        /* Get value of protect pin */
        readonly = lwgpio_get_value(&sd_protect);
#endif
        if (last != inserted)
        {
            if (inserted)
            {
                _time_delay (200);
                /* Open the device which MFS will be installed on */
                sdcard_handle = fopen("sdcard:", 0);
                if ( sdcard_handle == NULL )
                {
                    printf("Unable to open SD card device.\n");
                    _task_block();
                }

                /* Set read only flag as needed */
                param = 0;
                if (readonly)
                {
                    param = IO_O_RDONLY;
                }
                if (IO_OK != ioctl(sdcard_handle, IO_IOCTL_SET_FLAGS, (char *) &param))
                {
                    printf("Setting device read only failed.\n");
                    _task_block();
                }

                /* Install partition manager over SD card driver */
                error_code = _io_part_mgr_install(sdcard_handle, partman_name, 0);
                if (MFS_NO_ERROR != error_code)
                {
                    printf("Error installing partition manager: %s\n", \
                            MFS_Error_text((uint32_t)error_code));
                    _task_block();
                }

                /* Open partition manager */
                partman_handle = fopen(partman_name, NULL);
                if (NULL == partman_handle)
                {
                    error_code = ferror(partman_handle);
                    printf("Error opening partition manager: %s\n", \
                            MFS_Error_text((uint32_t)error_code));
                    _task_block();
                }

                /* Validate partition 1 */
                param = 1;
                error_code = _io_ioctl(partman_handle, IO_IOCTL_VAL_PART, &param);
                if (MQX_OK == error_code)
                {

                    /* Install MFS over partition 1 */
                    error_code = _io_mfs_install(partman_handle, filesystem_name, param);
                    if (MFS_NO_ERROR != error_code)
                    {
                        printf("Error initializing MFS over partition: %s\n", \
                                MFS_Error_text((uint32_t)error_code));
                        _task_block();
                    }

                } else {

                    /* Install MFS over SD card driver */
                    error_code = _io_mfs_install(
                            sdcard_handle,
                            filesystem_name,
                            (_file_size)0
                            );
                    if (MFS_NO_ERROR != error_code)
                    {
                        printf("Error initializing MFS: %s\n",\
                                MFS_Error_text((uint32_t)error_code));
                        _task_block();
                    }

                }

                /* Open file system */
                filesystem_handle = fopen(filesystem_name, NULL);
                error_code = ferror (filesystem_handle);
                if ((MFS_NO_ERROR != error_code) && (MFS_NOT_A_DOS_DISK != error_code))
                {
                    printf("Error opening filesystem: %s\n",\
                            MFS_Error_text((uint32_t)error_code));
                    _task_block();
                }
                if ( MFS_NOT_A_DOS_DISK ==  error_code)
                {
                    printf("NOT A DOS DISK! You must format to continue.\n");
                }

                printf ("SD card installed to %s\n", filesystem_name);
                if (readonly)
                {
                    printf ("SD card is locked (read only).\n");
                }
            }
            else
            {
                /* Close the filesystem */
                if (MQX_OK != fclose (filesystem_handle))
                {
                    printf("Error closing filesystem.\n");
                    _task_block();
                }
                filesystem_handle = NULL;

                /* Uninstall MFS  */
                error_code = _io_dev_uninstall(filesystem_name);
                if (MFS_NO_ERROR != error_code)
                {
                    printf("Error uninstalling filesystem.\n");
                    _task_block();
                }

                /* Close partition manager */
                if (MQX_OK != fclose (partman_handle))
                {
                    printf("Unable to close partition manager.\n");
                    _task_block();
                }
                partman_handle = NULL;

                /* Uninstall partition manager  */
                error_code = _io_dev_uninstall(partman_name);
                if (MFS_NO_ERROR != error_code)
                {
                    printf("Error uninstalling partition manager.\n");
                    _task_block();
                }

                /* Close the SD card device */
                if (MQX_OK != fclose (sdcard_handle))
                {
                    printf("Unable to close SD card device.\n");
                    _task_block();
                }
                sdcard_handle = NULL;

                printf ("SD card uninstalled.\n");
            }
        }
        last = inserted;
        _time_delay (200);
    }
}

/*TASK*-----------------------------------------------------------------
 *
 * Function Name  : Shell_task
 * Returned Value : void
 * Comments       :
 *
 *END------------------------------------------------------------------*/
static void Shell_task(uint32_t temp)
{
    /* Run the shell on the serial port */
    Shell(Shell_commands, NULL);
    _task_block();
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_host_audio_mute_ctrl_callback
 * Returned Value : None
 * Comments       :
 *     Called when a mute request is sent successfully.
 *
 *END*--------------------------------------------------------------------*/
void usb_host_audio_mute_ctrl_callback
(
 /* [IN] pointer to pipe */
 _usb_pipe_handle  pipe_handle,

 /* [IN] user-defined parameter */
 void             *user_parm,

 /* [IN] buffer address */
 unsigned char         *buffer,

 /* [IN] length of data transferred */
 uint32_t           buflen,

 /* [IN] status, hopefully USB_OK or USB_DONE */
 uint32_t           status
 )
{
    /* Body */
    if(USB_OK == status)
    {
        printf("Successfully\n");
    }
    else
    {
        printf("Failed\n");
    }
    fflush(stdout);

} /* Endbody */
/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_host_audio_request_ctrl_callback
 * Returned Value : None
 * Comments       :
 *     Called when a mute request is sent successfully.
 *
 *END*--------------------------------------------------------------------*/
void usb_host_audio_request_ctrl_callback
(
 /* [IN] pointer to pipe */
 _usb_pipe_handle  pipe_handle,

 /* [IN] user-defined parameter */
 void             *user_parm,

 /* [IN] buffer address */
 unsigned char         *buffer,

 /* [IN] length of data transferred */
 uint32_t           buflen,

 /* [IN] status, hopefully USB_OK or USB_DONE */
 uint32_t           status
 )
{ /* Body */
#if CODING_
    static FEATURE_CONTROL_STRUCT_PTR control_feature_ptr;
    control_feature_ptr = (FEATURE_CONTROL_STRUCT_PTR)user_parm;

    switch (control_feature_ptr->FU)
    {
        case USB_AUDIO_CTRL_FU_MUTE:
            {
                if (MQX_OK !=_lwevent_set(&USB_Audio_FU_Request, FU_MUTE_MASK)){
                    printf("_lwevent_set USB_Audio_FU_Request failed.\n");
                }
                break;
            }
        case USB_AUDIO_CTRL_FU_VOLUME:
            {
                if (MQX_OK !=_lwevent_set(&USB_Audio_FU_Request, FU_VOLUME_MASK)){
                    printf("_lwevent_set USB_Audio_FU_Request failed.\n");
                }
                break;
            }
        default:
            break;
    }
    fflush(stdout);
#endif
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_host_audio_tr_callback
 * Returned Value : None
 * Comments       :
 *     Called when a ISO packet is sent/received successfully.
 *
 *END*--------------------------------------------------------------------*/
void usb_host_audio_tr_callback
(
 /* [IN] pointer to pipe */
 _usb_pipe_handle pipe_handle,

 /* [IN] user-defined parameter */
 void   *user_parm,

 /* [IN] buffer address */
 unsigned char *buffer,

 /* [IN] length of data transferred */
 uint32_t buflen,

 /* [IN] status, hopefully USB_OK or USB_DONE */
 uint32_t status
 )
{
    static uint16_t packnum = 0;
    unsigned int k;
    /* copy each packet data to the received buffer*/
    for(k = 0; k < buflen; k++)
    {
        wav_sdcard_buff[packnum++] = wav_recv_buff[k];
        if (packnum >= REC_BLOCK_SIZE) {
            /* It is normal sd card writing */
            if(wav_sdcard_buff==wav_sdcard_buff0 && buf0_flag){
                _lwevent_set(block_rec, EV_BLOCK0_W_READY);
                wav_sdcard_buff = (wav_sdcard_buff == wav_sdcard_buff0) ? wav_sdcard_buff1 : wav_sdcard_buff0;
                buf0_flag=0;
            }
            else if(wav_sdcard_buff==wav_sdcard_buff1 && buf1_flag){
                _lwevent_set(block_rec, EV_BLOCK1_W_READY);
                wav_sdcard_buff = (wav_sdcard_buff == wav_sdcard_buff0) ? wav_sdcard_buff1 : wav_sdcard_buff0;
                buf1_flag=0;
            }
            else
            {
                /* Data lost caused by low SD card writing speeed */
                requests--;
            }
            packnum = 0;
        }
        else if (final_packet && (packnum == remainder)) {
            /* Here it is the final SD card writing operation */
            /* Stop the HW timer to end audio record process */
            hwtimer_stop(&audio_timer);
            /* signal that one is ready for writing */
            _lwevent_set(block_rec, wav_sdcard_buff == wav_sdcard_buff0 ? EV_BLOCK0_W_READY : EV_BLOCK1_W_READY);
            /* reset to the default config */
            wav_sdcard_buff = wav_sdcard_buff0;
            packnum = 0;
            buf1_flag=0;
            buf0_flag=0;
        }
    }

}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_host_audio_control_event
 * Returned Value : None
 * Comments       :
 *     Called when control interface has been attached, detached, etc.
 *END*--------------------------------------------------------------------*/
void usb_host_audio_control_event
(
 /* [IN] pointer to device instance */
 _usb_device_instance_handle      dev_handle,

 /* [IN] pointer to interface descriptor */
 _usb_interface_descriptor_handle intf_handle,

 /* [IN] code number for event causing callback */
 uint32_t                          event_code
 )
{
    /* Can not support multi-instance */
    INTERFACE_DESCRIPTOR_PTR   intf_ptr =
        (INTERFACE_DESCRIPTOR_PTR)intf_handle;

    fflush(stdout);
    switch (event_code) {
        case USB_CONFIG_EVENT:
            /* Drop through into attach, same processing */
        case USB_ATTACH_EVENT:
            {
                if((USB_DEVICE_IDLE == audio_stream.DEV_STATE) || \
                        (USB_DEVICE_DETACHED == audio_stream.DEV_STATE))
                {
                    audio_control.DEV_HANDLE  = dev_handle;
                    audio_control.INTF_HANDLE = intf_handle;
                    audio_control.DEV_STATE   = USB_DEVICE_ATTACHED;
                }
                else
                {
                    printf("In AC event - Audio device already attached\n");
                    fflush(stdout);
                }

                /* Finds all the descriptors in the configuration, here just header_desc
                 * is meaningful.
                 */

                if (USB_OK != usb_class_audio_control_get_descriptors(
                            dev_handle,
                            intf_handle,
                            &header_desc,
                            &it_desc,
                            &ot_desc,
                            &fu_desc)
                   )
                {
                    printf("Failed to get audio descriptors\n");
                    break;
                };
                /* overwrite it_desc, ot_desc and fu_desc before get the proper desc */
                it_desc = NULL;
                ot_desc = NULL;
                fu_desc = NULL;

                /* 0x0201 is microphone device */
                if (USB_OK != usb_class_audio_control_get_it_descriptor(
                            dev_handle,
                            intf_handle,
                            0x0201,
                            (void**)&it_desc)
                   )
                {
                    printf("\nFailed to get microphone terminal descriptor. Check your device!\n");
                    break;
                }

                /* The sourceID for FU is the input terminal ID */
                if (USB_OK != usb_class_audio_control_get_fu_descriptor(
                            dev_handle,
                            intf_handle,
                            it_desc->bTerminalID,
                            (void**)&fu_desc)
                   )
                {
                    printf("\nFailed to get FU descriptor. Check your device!\n");
                    break;
                }

                /* initialize new interface members and select this interface */
                if (USB_OK != _usb_hostdev_select_interface(dev_handle,
                            intf_handle, (void *)&audio_control.CLASS_INTF))
                {
                    printf("Failed to open interface. Check your device!\n");
                    break;
                }

                printf("----- Audio control interface: attach event -----\n");
                fflush(stdout);
                printf("State = attached");
                printf("  Class = %d", intf_ptr->bInterfaceClass);
                printf("  SubClass = %d", intf_ptr->bInterfaceSubClass);
                printf("  Protocol = %d\n", intf_ptr->bInterfaceProtocol);
                break;
            }
        case USB_INTF_EVENT:
            {
                USB_STATUS status;
                status = usb_class_audio_init_ipipe(
                        (CLASS_CALL_STRUCT_PTR)\
                        &audio_control.CLASS_INTF,
                        NULL,NULL
                        );
                if ((USB_OK != status) && (USBERR_OPEN_PIPE_FAILED != status))
                    break;

                printf("----- Audio control interface: interface event -----\n");
                audio_control.DEV_STATE = USB_DEVICE_INTERFACED;
                break;
            }

        case USB_DETACH_EVENT:
            {
                AUDIO_CONTROL_INTERFACE_STRUCT_PTR if_ptr;

                if_ptr = (AUDIO_CONTROL_INTERFACE_STRUCT_PTR) \
                         audio_control.CLASS_INTF.class_intf_handle;

                _lwevent_destroy(&if_ptr->control_event);

                printf("----- Audio control interface: detach event -----\n");
                fflush(stdout);
                printf("State = detached");
                printf("  Class = %d", intf_ptr->bInterfaceClass);
                printf("  SubClass = %d", intf_ptr->bInterfaceSubClass);
                printf("  Protocol = %d\n", intf_ptr->bInterfaceProtocol);
                fflush(stdout);
                audio_control.DEV_HANDLE = NULL;
                audio_control.INTF_HANDLE = NULL;
                audio_control.DEV_STATE = USB_DEVICE_DETACHED;
                it_desc=NULL;
                fu_desc=NULL;
                _lwevent_set(block_rec, EV_BLOCK0_W_READY | EV_BLOCK1_W_READY);
                remainder=0;
                requests=0;
                break;
            }
        default:
            printf("Audio AC Device: unknown control event\n");
            fflush(stdout);
            break;
    } /* EndSwitch */
    fflush(stdout);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_host_audio_stream_event
 * Returned Value : None
 * Comments       :
 *     Called when stream interface has been attached, detached, etc.
 *END*--------------------------------------------------------------------*/
void usb_host_audio_stream_event
(
 /* [IN] pointer to device instance */
 _usb_device_instance_handle      dev_handle,

 /* [IN] pointer to interface descriptor */
 _usb_interface_descriptor_handle intf_handle,

 /* [IN] code number for event causing callback */
 uint32_t                          event_code
 )
{ /* Body */
    INTERFACE_DESCRIPTOR_PTR   intf_ptr = (INTERFACE_DESCRIPTOR_PTR)intf_handle;

    /* Check audio stream interface alternating 0 */
    if (0 == intf_ptr->bNumEndpoints)
    {
        printf("----- skip this stream interface event -----\n");
        printf("Alternate setting: %d, event: %d, interface number: %d\n", intf_ptr->bAlternateSetting, event_code, intf_ptr->bInterfaceNumber);
        return;
    }

    switch (event_code) {
        case USB_CONFIG_EVENT:
            /* Drop through into attach, same processing */
        case USB_ATTACH_EVENT:
            {
                /* Try to find out the target AS interface
                 * for microphone case, it is interface with output terminal
                 */
                if(0==target){
                    USB_AUDIO_STREAM_DESC_SPEPIFIC_AS_IF_PTR     as_itf_desc = NULL;
                    USB_AUDIO_STREAM_DESC_SPECIFIC_ISO_ENDP_PTR  iso_endp_spec_desc = NULL;

                    /* finds all the descriptors in the configuration */
                    if (USB_OK != usb_class_audio_stream_get_descriptors(
                                dev_handle,
                                intf_handle,
                                &as_itf_desc,
                                &frm_type_desc,
                                &iso_endp_spec_desc)
                       )
                    {
                        break;
                    };

                    /* Find out the Terminal associated with this AS interface's endpoint */
                    usb_class_audio_control_get_descriptor_id(
                            dev_handle,
                            audio_control.INTF_HANDLE,
                            as_itf_desc->bTerminalLink,
                            (void**)&ot_desc
                            );
                    /* Check the Type of the terminal. For micrphone case, the terminal descriptor type should be
                     * USB_DESC_SUBTYPE_AUDIO_CS_OT and the Terminal type should be usb stream type.
                     */
                    if(1==ot_desc->wTerminalType[1] && USB_DESC_SUBTYPE_AUDIO_CS_OT==ot_desc->bDescriptorSubtype){
                        /* Find out the target AS interface
                         * Save this AS iterface information which is needed in follow record process
                         */
                        target=1;
                        /* set all info got from descriptors to the class interface struct */
                        usb_class_audio_control_set_descriptors((void *)&audio_control.CLASS_INTF,
                                header_desc, it_desc, ot_desc, fu_desc);

                        if((USB_DEVICE_IDLE == audio_stream.DEV_STATE) ||\
                                (USB_DEVICE_DETACHED == audio_stream.DEV_STATE))
                        {
                            audio_stream.DEV_HANDLE  = dev_handle;
                            audio_stream.INTF_HANDLE = intf_handle;
                            audio_stream.DEV_STATE      = USB_DEVICE_ATTACHED;
                        }
                        else
                        {
                            printf("audio_stream.DEV_STATE=%d\n",audio_stream.DEV_STATE);

                            printf("In AS event - Audio device already attached\n");
                            fflush(stdout);
                        }
                        /* initialize new interface members and select this interface */
                        if (USB_OK != _usb_hostdev_select_interface(dev_handle,
                                    intf_handle, (void *)&audio_stream.CLASS_INTF))
                        {
                            break;
                        }

                        /* Andy Tian
                           Now just use this method, we need get the packet size dynamically when doing record
                           Here it is the first sample rate.
                         */
                        // packet_size = USB_Audio_Get_Packet_Size(frm_type_desc);
                        /* Andy Tian
                           comment it now, we need move it to the record function
                         */

                        /* set all info got from descriptors to the class interface struct */
                        usb_class_audio_stream_set_descriptors(
                                (void *)&audio_stream.CLASS_INTF,
                                as_itf_desc,
                                frm_type_desc,
                                iso_endp_spec_desc
                                );
                        /* Save the target interface point which will be used in next interfaced event handle */
                        g_intf_ptr = (INTERFACE_DESCRIPTOR_PTR)intf_handle;
                        printf("----- Audio stream interface: attach event -----\n");
                        fflush(stdout);
                        printf("State = attached");
                        printf("  Class = %d", intf_ptr->bInterfaceClass);
                        printf("  SubClass = %d", intf_ptr->bInterfaceSubClass);
                        printf("  Protocol = %d\n", intf_ptr->bInterfaceProtocol);
                        fflush(stdout);

                        break;
                    }
                    else{
                        printf("----- skip this stream interface event -----\n");
                        printf("Alternate setting: %d, event: %d, interface number: %d\n", intf_ptr->bAlternateSetting, event_code, intf_ptr->bInterfaceNumber);
                    }
                }
                else
                    /* It is not the target audio stream interface, nothing to do */
                {
                    printf("----- skip this stream interface event -----\n");
                    printf("Alternate setting: %d, event: %d, interface number: %d\n", intf_ptr->bAlternateSetting, event_code, intf_ptr->bInterfaceNumber);
                    break;
                }
            }
        case USB_INTF_EVENT:
            {

                /* For now the stack has issue which will generate INTF_EVENT for speaker interface
                   even no select_interface used to that interface. 
                   We need protect our target event from the unnecessary interfaced event
                   */
                if(1==target && g_intf_ptr == intf_handle){
                    audio_stream.DEV_STATE = USB_DEVICE_INTERFACED;
                    if (USB_OK != _usb_hostdev_get_descriptor(
                                dev_handle,
                                intf_handle,
                                /* Functional descriptor
                                 * for interface */
                                USB_DESC_TYPE_EP,
                                /* Index of descriptor */
                                1,
                                /* Index of interface alternate */
                                intf_ptr->bAlternateSetting,
                                (void **)&endp)
                       )
                    {
                        printf("Get end point descriptor error!");
                        break;
                    }
                    target=2;
                    printf("----- Audio stream interface: interface event-----\n");
                }
                else
                {
                    printf("----- Skip this Unnecessary stream interfaced event -----\n");
                    printf("Alternate setting: %d, event: %d, interface number: %d\n", intf_ptr->bAlternateSetting, event_code, intf_ptr->bInterfaceNumber);
                }
                break;
            }
        case USB_DETACH_EVENT:
            {

                /* Free the audio_stream field to support hotplug */
                if(2==target){
                    audio_stream.DEV_HANDLE = NULL;
                    audio_stream.INTF_HANDLE = NULL;
                    audio_stream.DEV_STATE = USB_DEVICE_DETACHED;
                    target=0;
                    printf("----- Audio stream interface: detach event-----\n");
                    fflush(stdout);
                    break;
                }
                else
                    break;
            }
        default:
            printf("Audio AS device: unknown data event\n");
            fflush(stdout);
            break;
    }
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_audio_control_get_descriptor_id
 * Returned Value : USB_OK
 * Comments       :
 *     This function is hunting for target descriptor according to ID in the device configuration
 *     and fills back fields if the descriptor was found
 * It returns USB_OK if success
 *END*--------------------------------------------------------------------*/
USB_STATUS usb_class_audio_control_get_descriptor_id
(
 /* [IN] pointer to device instance */
 _usb_device_instance_handle                dev_handle,

 /* [IN] pointer to interface descriptor */
 _usb_interface_descriptor_handle           intf_handle,

 /* [IN] terminal ID given by TerminalLink field of AS descriptor */
 int8_t         id,

 /* [OUT] pointer to requested descriptor ID */
 void ** ac_desc
 )
{
    INTERFACE_DESCRIPTOR_PTR   intf_ptr = (INTERFACE_DESCRIPTOR_PTR)intf_handle;
    USB_STATUS                 status;
    int32_t i;
    int8_t *     fd;

    status = USB_OK;
    /* collect all interface functional descriptors */
    for (i = 0; ; i++) {
        if (USB_OK != _usb_hostdev_get_descriptor(
                    dev_handle,
                    intf_handle,
                    USB_DESC_TYPE_CS_INTERFACE,  /* Functional descriptor for interface */
                    i,                           /* Index of descriptor */
                    intf_ptr->bAlternateSetting, /* Index of interface alternate */
                    (void **) &fd))
        {
            /* Can not find the target terminal, return failed */
            if (*ac_desc == NULL)
                status = USBERR_INIT_FAILED;
            break;
        }
        /* Check ID */
        if(id==*(fd+3))
        {
            /* Get the target descriptor */
            *ac_desc=(void*)fd;
            break;
        }
    }
    return status;
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_audio_control_get_it_descriptor
 * Returned Value : USB_OK
 * Comments       :
 *     This function is hunting for input microphone descriptor(now it is 0x0201) in the device configuration
 *     and return it if the descriptor was found.
 *END*--------------------------------------------------------------------*/
USB_STATUS usb_class_audio_control_get_it_descriptor
(
 /* [IN] pointer to device instance */
 _usb_device_instance_handle                dev_handle,

 /* [IN] pointer to interface descriptor */
 _usb_interface_descriptor_handle           intf_handle,

 /* [IN] AC descriptor ID */
 int16_t            type,

 /* [OUT] pointer to requested descriptor ID */
 void ** it_desc
 )
{
    INTERFACE_DESCRIPTOR_PTR   intf_ptr = (INTERFACE_DESCRIPTOR_PTR)intf_handle;
    USB_STATUS                 status;
    int32_t i;
    int8_t *     fd;

    status = USB_OK;
    /* collect all interface functional descriptors */
    for (i = 0; ; i++) {
        if (USB_OK != _usb_hostdev_get_descriptor(
                    dev_handle,
                    intf_handle,
                    USB_DESC_TYPE_CS_INTERFACE,  /* Functional descriptor for interface */
                    i,                           /* Index of descriptor */
                    intf_ptr->bAlternateSetting, /* Index of interface alternate */
                    (void **) &fd))
        {
            if (*it_desc == NULL)
                /* Failed to get the target input terminal */
                status = USBERR_INIT_FAILED;
            break;
        }
        /* check whether it is an input terminal */
        if (USB_DESC_SUBTYPE_AUDIO_CS_IT == *(fd+2))
        {
            /* check the type since it is an Input terminal */
            if (type == (*(fd+5) << 8 | *(fd+4)))
            {
                /* Findout the target input terminal */
                *it_desc=(void*)fd;
                break;
            }
        }
    }
    return status;
}


/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_audio_control_get_fu_descriptor
 * Returned Value : USB_OK
 * Comments       :
 *     This function is hunting for FU descriptor according to source ID in the device configuration
 *     and fills back fields if the descriptor was found
 *END*--------------------------------------------------------------------*/
USB_STATUS usb_class_audio_control_get_fu_descriptor
(
 /* [IN] pointer to device instance */
 _usb_device_instance_handle                dev_handle,

 /* [IN] pointer to interface descriptor */
 _usb_interface_descriptor_handle           intf_handle,

 /* [IN] AC descriptor ID */
 int8_t         sid,

 /* [OUT] pointer to requested descriptor ID */
 void ** fu_desc
 )
{
    INTERFACE_DESCRIPTOR_PTR   intf_ptr = (INTERFACE_DESCRIPTOR_PTR)intf_handle;
    USB_STATUS                 status;
    int32_t i;
    int8_t *     fd;

    status = USB_OK;
    /* collect all interface functional descriptors */
    for (i = 0; ; i++) {
        if (USB_OK != _usb_hostdev_get_descriptor(
                    dev_handle,
                    intf_handle,
                    USB_DESC_TYPE_CS_INTERFACE,  /* Functional descriptor for interface */
                    i,                           /* Index of descriptor */
                    intf_ptr->bAlternateSetting, /* Index of interface alternate */
                    (void **) &fd))
        {
            if (*fu_desc == NULL)
            {
                status = USBERR_INIT_FAILED;
            }
            break;
        }
        /* check whether it is a FU */
        if (USB_DESC_SUBTYPE_AUDIO_CS_FU == *(fd+2))
        {
            /* check the sourceID since it is a FU */
            /* For micrphone the original terminal is microphone input terminal
               which is the source of FU. FU control the input terminal's features.
               So comparing the IT's ID with FU's source ID to get the proper FU
               */
            if (sid == *(fd+4))
            {
                /* Findout the target input terminal */
                *fu_desc=(void*)fd;
                break;
            }
        }
    }
    return status;
}
