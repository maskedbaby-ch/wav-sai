/*
 * Copyright (c) 2013 - 2014, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*******************************************************************************
 * Standard C Included Files
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
/*******************************************************************************
 * SDK Included Files
 ******************************************************************************/
#include "fsl_wm8960_driver.h"
#include "fsl_clock_manager.h"
#include "fsl_device_registers.h"
#include "fsl_soundcard.h"
#include "board.h"
#include "fsl_sdhc_card.h"
#include "sdmmc.h"
#include "sdhc_sdcard.h"
#include "ff.h"
#include "diskio.h"
#include "fsl_sd_disk.h"
#include "wav.h"
/*******************************************************************************
 * Application Included Files
 ******************************************************************************/
#include "audio.h"
#include "equalizer.h"
//#include "pcm_data.h"
#include "terminal_menu.h" 
/*******************************************************************************
 * Definitions
 ******************************************************************************/
static FATFS g_fileSystem; /* File system object */
static FIL g_fileObject;   /* File object */

typedef enum
{
    kTestResultPassed = 0U,
    kTestResultFailed,
    kTestResultInitFailed,
    kTestResultAborted,
} test_result_t;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void pin_setup(void);
void user_menu(void);
void playback_set(uint8_t *menuData);
/*******************************************************************************
 * Global Variables
 ******************************************************************************/
extern menu_machine_t g_menuMachine[];
extern menu_state_t g_menuState;
static semaphore_t cd; /*!< Semaphore for card detection indication */
static volatile uint32_t cardInserted = 0; /*!< Flag to indicate a card has been inserted */
static volatile uint32_t cardInited = 0; /*!< Flag to indicate the card has been initialized successfully */
task_handler_t musicControl;

#if !(defined(CPU_MK22FN128VDC10) || defined(CPU_MK22FN256VDC12))
float32_t g_dspStore[2 * AUDIO_BUFFER_BLOCK_SIZE];
float32_t g_dspResult[AUDIO_BUFFER_BLOCK_SIZE];
float32_t g_dspInput[AUDIO_BUFFER_BLOCK_SIZE];
#endif
/*******************************************************************************
 * Code
 ******************************************************************************/
void pin_setup(void)
{
    configure_i2s_pins(0);
}

void sdhc_card_detection(bool inserted)
{
    /* set or clear cardInserted flag */
    if (inserted)
    {
        cardInserted = 1;
    }
    else
    {
        cardInserted = 0;
    }
    
    /* Post semaphore object used for card detection */
    OSA_SemaPost(&cd);
}

/*!
 * @brief Function to be called from Card detection interrupt context etc.
 */
void sdhc_cd_irqhandler(void)
{
    if (GPIO_DRV_ReadPinInput(kGpioSdhc0Cd))
#if defined FRDM_K64F120M
        sdhc_card_detection(true);
    else
        sdhc_card_detection(false);
#elif defined TWR_K64F120M
        sdhc_card_detection(false);
    else
        sdhc_card_detection(true);
#elif defined MAPS_K64
        sdhc_card_detection(true);
    else
        sdhc_card_detection(false);
#else
#error unknown board
#endif
}

/*!
 * @brief This function demonstrates read, write and erase functionalities of the card
 */
static test_result_t card_insert(void)
{
    sdhc_card_t card = {0};
    sdhc_host_t host = {0};
    sdhc_user_config_t config = {0};
    uint32_t i, status;
    uint8_t proceed;
    
    /* initailize user sdhc configuration structure */
    config.transMode = kSdhcTransModePio;
    config.clock = SDMMC_CLK_100KHZ;
    config.cdType = kSdhcCardDetectGpio;
   
    /* initialize the SDHC driver with the user configuration */
    if (SDHC_DRV_Init(BOARD_SDHC_INSTANCE, &host, &config) != kStatus_SDHC_NoError)
    {
        return kTestResultFailed;
    }

    /* wait for a card detection */
    sdhc_cd_irqhandler();

    /* card instertion is detected based on interrupt */
    do
    {
        status = OSA_SemaWait(&cd, OSA_WAIT_FOREVER);
    } while ((status == kStatus_OSA_Idle) || (!cardInserted));

    if (cardInserted)
    {
        printf("A card is detected\n\r");
    }
    else
    {
        printf("A card is removed\n\r");
        return kTestResultFailed;
    }  
    return kTestResultPassed;
}

void mkdir_my(const TCHAR *path)
{
    FRESULT error;
    error = f_mkdir(_T(path));
    if (error)
    {
        if (error == FR_EXIST)
        {
            printf("Directory exists.\r\n");
        }
        else
        {
            printf("Make directory failed.\r\n");
        }
    }
}
static void musicControl(void *arg)
{
}

/******************************************************************************/
int main (void)
{ 
    FRESULT error;
    DIR directory;
    TCHAR* fileame[13];
    uint32_t index = 0;
    uint8_t filePrefix[20] = "1:/music/";
    __wavctrl wav_temp;
    test_result_t testResult;
    FILINFO fileInformation;
    const TCHAR driverNumberBuffer[3U] = {SD + '0', ':', '/'};
    hardware_init();
    
    OSA_Init();

    /* Initialize UART terminal. */
    dbg_uart_init();
	//DbgConsole_Init(BOARD_DEBUG_UART_INSTANCE, BOARD_DEBUG_UART_BAUD, kDebugConsoleUART);
    
    /* Configure SAI pins. */
    pin_setup();
    
    CLOCK_SYS_EnableSdhcClock(0);
      
    /* Set pin muxing for card detection (CD) pin */
    GPIO_DRV_Init(sdhcCdPin, NULL);
    


    if (card_insert() != kTestResultPassed)
    {
      printf("No card\r\n");
    }
    if (f_mount(&g_fileSystem,driverNumberBuffer,1))
    {
        printf("Mount volume failed.\r\n");
        return -1;
    }

//    error = f_chdrive(SD);
//    if (error)
//    {
//        printf("Change drive failed.\r\n");
//        return -1;
//    }
    
    printf("\r\nList the file in that directory......\r\n");

    error = f_opendir(&directory, "1:/music");
    if (error)
    {
        printf("Open directory failed.\r\n");
        return -1;
    }

    for (;;)
    {
        error = f_readdir(&directory, &fileInformation);

        /* To the end. */
        if ((error != FR_OK) || (fileInformation.fname[0U] == 0U))
        {
            break;
        }
        if (fileInformation.fname[0] == '.')
        {
            continue;
        }
        if (fileInformation.fattrib & AM_DIR)
        {
        }
        else
        {
            printf("General file : %s.\r\n", fileInformation.fname);
            fileame[index] = (TCHAR*)malloc(strlen(fileInformation.fname) + 1);
            memcpy(fileame[index],fileInformation.fname,strlen(fileInformation.fname) + 1);
        }
        index++;
    }
    
    for(uint32_t i=0; i<index;i++)
    {
        strcat((char*)filePrefix,fileame[i]);
        error = f_open(&g_fileObject, filePrefix, FA_READ);
        if (error)
        {
            printf("Open file failed.\r\n");
            return -1;
        }
        play_file(&g_fileObject,filePrefix,4);
        if (f_close(&g_fileObject))
        {
            printf("\r\nClose file failed.\r\n");
            return -1;
        }
        memset(filePrefix+9,0,13);
    }
    while(1)
    {
      //  user_menu(); /* User menu, viewable through serial terminal. */
    }
}
/******************************************************************************/
void user_menu(void)
{
    uint8_t menuMsg[3];
    uint8_t count;

    count = 0;
    while(g_menuState < kMenuEndState)
    {
        menuMsg[count] = (*g_menuMachine[g_menuState].menu_func)();
        count++;
    }

    playback_set(menuMsg);

    g_menuState = kMenuSelectPlay;

}
/******************************************************************************/
void playback_set(uint8_t *menuData)
{

    switch(menuData[0])
    {
#if 0
        case '1':
            switch(menuData[1])
            {
                case '1':
                    stream_audio(kFFT, menuData[2]);
                    break;

                case '2':
                    stream_audio(kNoDSP, menuData[2]);
                    break;

                default:
                    __asm("NOP");
                    break;
            }
            break;
#endif
        case '1':
            switch(menuData[1])
            {
                case '1':
                  //  play_wav((uint32_t *)music, menuData[2]);
                    break;

                default:
                    __asm("NOP");
                    break;
            }
            break;

        default:
            __asm("NOP");
            break;
    }

}
/******************************************************************************
 * EOF
 ******************************************************************************/
