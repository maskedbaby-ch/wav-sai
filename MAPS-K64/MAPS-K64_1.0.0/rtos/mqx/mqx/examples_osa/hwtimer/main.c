/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
* Copyright 1989-2008 ARC International
*
* This software is owned or controlled by Freescale Semiconductor.
* Use of this software is governed by the Freescale MQX RTOS License
* distributed with this Material.
* See the MQX_RTOS_LICENSE file distributed for more details.
*
* Brief License Summary:
* This software is provided in source form for you to use free of charge,
* but it is not open source software. You are allowed to use this software
* but you cannot redistribute it or derivative works of it in source form.
* The software may be used only in connection with a product containing
* a Freescale microprocessor, microcontroller, or digital signal processor.
* See license agreement file for full license terms including other
* restrictions.
*****************************************************************************
*
* Comments:
*
*   This file contains the source for the hello example program.
*
*
*END************************************************************************/
#include <mqx.h>
#include <bsp.h>
#include <stdio.h>
#include <fsl_hwtimer.h>
#include "app_rtos.h"
#include "main.h"

/*hwtimer example defines*/
#define HWTIMER1_DEV    kPitDevif
#define HWTIMER1_ID     0
#define HWTIMER1_SOURCE_CLK kBusClock
#define HWTIMER1_FREQUENCY  10          //frequency set to hwtimer 1
#define HWTIMER2_DEV    kPitDevif
#define HWTIMER2_ID     1
#define HWTIMER2_SOURCE_CLK kBusClock
#define HWTIMER2_PERIOD     1000000     //period set to 1s to hwtimer 2
/* Task IDs */

/* local function prototypes */
void hwtimer_task(uint32_t );
static void hwtimer1_callback(void *p);
static void hwtimer2_callback(void *p);

hwtimer_t hwtimer1;                               //hwtimer handle
hwtimer_t hwtimer2;                               //hwtimer handle
semaphore_t job_done;                          //semaphore for end of job

/*FUNCTION*----------------------------------------------------------------*
* Function Name  : hwtimer1_callback
* Returned Value : void
* Comments :
*       Callback for hwtimer1
*END*--------------------------------------------------------------------*/
static void hwtimer1_callback(void *p)
{
    GPIO_DRV_TogglePinOutput(kGpioLED1);
}

/*FUNCTION*----------------------------------------------------------------*
* Function Name  : hwtimer2_callback
* Returned Value : void
* Comments :
*   Callback for hwtimer2
*
*END*--------------------------------------------------------------------*/
static void hwtimer2_callback(void *p)
{
    int ticks = HWTIMER_SYS_GetTicks(&hwtimer2);

    if (1 == ticks)
    {
        /* Start hwtimer1*/
        GPIO_DRV_TogglePinOutput(kGpioLED2);
        HWTIMER_SYS_Start(&hwtimer1);
    }
    if (2 == ticks)
    {
        /* block hwtimer1_callback */
        HWTIMER_SYS_BlockCallback(&hwtimer1);
        GPIO_DRV_TogglePinOutput(kGpioLED1);
    }
    if (4 == ticks)
    {
        GPIO_DRV_TogglePinOutput(kGpioLED2);
        HWTIMER_SYS_CancelCallback(&hwtimer1); // Clear pending callback for hwtimer1
        HWTIMER_SYS_UnblockCallback(&hwtimer1);
    }if (6 == ticks)
    {
        GPIO_DRV_TogglePinOutput(kGpioLED2);
        HWTIMER_SYS_Stop(&hwtimer1);
        /* Stop hwtimer1 */
        HWTIMER_SYS_Stop(&hwtimer2);
        /* Stop hwtimer2 */
        OSA_SemaPost(&job_done);
    }
}

/*TASK*-----------------------------------------------------
*
* Task Name    : hwtimer_task
* Comments     :
*    This task prints " HWTIMER"
*
*END*-----------------------------------------------------*/
void hwtimer_task
    (
        uint32_t initial_data
    )
{
    printf("\n--------------------------START OF HWTIMER EXAMPLE--------------------------\n");


    if (kStatus_OSA_Success != OSA_SemaCreate(&job_done, 0))
    {
        printf("Failed to create semaphore");
        return;
    }
    /* Initialization of hwtimer1*/
    printf("Initialization of hwtimer1   :");
    if (kHwtimerSuccess != HWTIMER_SYS_Init(&hwtimer1, &HWTIMER1_DEV, HWTIMER1_ID, (BSP_DEFAULT_MQX_HARDWARE_INTERRUPT_LEVEL_MAX + 1), NULL))
    {
        printf(" FAILED!\n");
    }
    else
    {
        printf(" OK\n");
    }
    printf("Try to set frequency %d Hz to hwtimer1\n",HWTIMER1_FREQUENCY);
    HWTIMER_SYS_SetFreq(&hwtimer1, HWTIMER1_SOURCE_CLK, HWTIMER1_FREQUENCY);
    printf("Read frequency from hwtimer1 : %d Hz\n", HWTIMER_SYS_GetFreq(&hwtimer1));
    printf("Read period from hwtimer1    : %d us\n", HWTIMER_SYS_GetPeriod(&hwtimer1));
    printf("Read modulo from hwtimer1    : %d\n", HWTIMER_SYS_GetModulo(&hwtimer1));
    /* Register isr for hwtimer1 */
    printf("Register callback for hwtimer1\n");
    HWTIMER_SYS_RegisterCallback(&hwtimer1, hwtimer1_callback, NULL);

    /* Initialization of hwtimer2*/
    printf("\nInitialization of hwtimer2   :");
    if (kHwtimerSuccess != HWTIMER_SYS_Init(&hwtimer2, &HWTIMER2_DEV, HWTIMER2_ID, (BSP_DEFAULT_MQX_HARDWARE_INTERRUPT_LEVEL_MAX + 1), NULL))
    {
        printf(" FAILED!\n");
    }
    else
    {
        printf(" OK\n");
    }
    printf("Try to set period %d us to hwtimer2\n", HWTIMER2_PERIOD);
    HWTIMER_SYS_SetPeriod(&hwtimer2, HWTIMER2_SOURCE_CLK, HWTIMER2_PERIOD);
    printf("Read frequency from hwtimer2 : %d Hz\n", HWTIMER_SYS_GetFreq(&hwtimer2));
    printf("Read period from hwtimer2    : %d us\n", HWTIMER_SYS_GetPeriod(&hwtimer2));
    printf("Read modulo from hwtimer2    : %d\n", HWTIMER_SYS_GetModulo(&hwtimer2));
    /* Register isr for hwtimer2 */
    printf("Register callback for hwtimer2\n");
    HWTIMER_SYS_RegisterCallback(&hwtimer2,hwtimer2_callback, NULL);
    /* Start hwtimer1*/
    printf("Start hwtimer2\n");
    HWTIMER_SYS_Start(&hwtimer2);

    /* Waiting for semaphore from callback routine */
    OSA_SemaWait(&job_done, OSA_WAIT_FOREVER);
    printf("hwtimer2 = %d ticks, hwtimer1 = %d ticks\n",HWTIMER_SYS_GetTicks(&hwtimer2), HWTIMER_SYS_GetTicks(&hwtimer1));
    /* Deinitialization of hwtimers */
    printf("Deinit hwtimer1\n");
    HWTIMER_SYS_Deinit(&hwtimer1);
    printf("Deinit hwtimer2\n");
    HWTIMER_SYS_Deinit(&hwtimer2);

    printf("---------------------------END OF HWTIMER EXAMPLE---------------------------\n");
    OSA_SemaDestroy(&job_done);
}

/* EOF */
