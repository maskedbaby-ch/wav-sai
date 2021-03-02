/*HEADER**********************************************************************
*
* Copyright 2008, 2014 Freescale Semiconductor, Inc.
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
*END************************************************************************/

#include <stdio.h>

#include "fsl_os_abstraction.h"
#include "fsl_gpio_driver.h"
#include "board.h"

#include "app_rtos.h"
#include "main.h"

/*TASK************************************************************************
*
* Task Name    : hello_task
* Comments     : This task prints welcome message to terminal
*                and continues periodically incrementing counter.
*
*END*************************************************************************/

void hello_task ( task_param_t init_data )
{
    int counter = (int)init_data;

    puts("\r\n****************** Hello OSA Example ******************\r\n");
    printf("\r\nFreescale %s board \n", BOARD_NAME);

    while(1)
    {
        OSA_TimeDelay(500);
        printf("\rCounter: %d", counter++);
    }
}

/*TASK************************************************************************
*
* Task Name    : led_task
* Comments     : This periodically task toggles by LED1 using GPIO driver.
*
*END*************************************************************************/
void led_task ( task_param_t init_data )
{
    int counter = (int)init_data;

    while(1)
    {
        OSA_TimeDelay(500);
        GPIO_DRV_TogglePinOutput(kGpioLED1);
        counter++;
    }
}
