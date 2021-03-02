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
*   This file contains the source for the lock osa example program.
*
*END************************************************************************/

#include <stdio.h>
#include <assert.h>

#include "fsl_os_abstraction.h"
#include "app_rtos.h"
#include "main.h"

mutex_t           print_mutex;

task_handler_t    print_task_handler;
OSA_TASK_DEFINE(print_task, PRINT_TASK_STACK_SIZE);

char* strings1[] = { "1: ", "Hello from Print task 1" };
char* strings2[] = { "2: ", "Print task 2 is alive" };

/*TASK************************************************************************
*
* Task Name : main_task
* Comments  : This task creates a mutex and then two
*             instances of the print task.
*END*************************************************************************/

void main_task( task_param_t init_data)
{
    osa_status_t     result;

    puts("\r\n****************** Lock OSA Example ******************\r\n");

    /* Create the mutex */
    result = OSA_MutexCreate(&print_mutex);
    if (result != kStatus_OSA_Success) {
        printf("Initialize print mutex failed.\r\n");
    }
    /* Create print tasks */
    result = OSA_TaskCreate( print_task,
                          "print1",
                          PRINT_TASK_STACK_SIZE,
                          NULL,
                          PRINT_TASK_PRIORITY_OSA,
                          (task_param_t)strings1,
                          false,
                          &print_task_handler
                        );
    if (result != kStatus_OSA_Success) {
        printf("Task creation failed.\r\n");
    }

    result = OSA_TaskCreate( print_task,
                          "print2",
                          PRINT_TASK_STACK_SIZE,
                          NULL,
                          PRINT_TASK_PRIORITY_OSA,
                          (task_param_t)strings2,
                          false,
                          &print_task_handler
                        );

    if (result != kStatus_OSA_Success) {
        printf("Task creation failed.\r\n");
    }
}

/*TASK************************************************************************
*
* Task Name : print_task
* Comments  : This task prints a message. It uses a mutex to
*             ensure I/O is not interleaved.
*END*************************************************************************/

void print_task( task_param_t init_data )
{
    osa_status_t     result;
    char **strings = (char **)init_data;

    while (1)
    {
        result = OSA_MutexLock(&print_mutex, 0);
        if (result != kStatus_OSA_Success) {
            printf("Mutex lock failed.\r\n");
        }

        /*
         * The yield in between the puts calls just returnes back,
         * as the other task is blocked waiting for the mutex.
         * Both strings are always printed together on a single line.
         */
        printf(strings[0]);
        OSA_TimeDelay(500);
        printf("%s\r\n",strings[1]);
        OSA_MutexUnlock(&print_mutex);

        /* Passes control to the other task (which then obtains the mutex) */
        OSA_TimeDelay(500);
    }
}

/* EOF */
