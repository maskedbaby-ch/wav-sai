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
*   This file contains the source for the event osa example program.
*
*END************************************************************************/

#include <stdio.h>
#include <assert.h>

#include "fsl_os_abstraction.h"
#include "app_rtos.h"
#include "main.h"


task_handler_t    simulated_isr_task_handler;

/* Event Definitions */
event_t                 event;

/* Event flag masks */
#define EVENT_FLAGS     1


/*TASK************************************************************************
*
* Task Name    : service_task
* Comments     :
*    This task creates an event group and the simulated_isr_task
*    task. It opens a connection to the event group and waits.
*    After the appropriate event bit has been set, it clears
*    the event bit and prints "Tick."
*END*************************************************************************/

void service_task ( task_param_t init_data )
{
    osa_status_t        result;
    event_flags_t       flags;

    puts("\r\n****************** Event OSA Example ******************\r\n");
    /* create event group */
    if ( OSA_EventCreate( &event, kEventManualClear ) != kStatus_OSA_Success )   {
        printf("Make event failed\r\n");
    }

    /* create the isr task */
    OSA_TASK_DEFINE(simulated_isr_task, SIMULATED_ISR_TASK_STACK_SIZE);
    result = OSA_TaskCreate( simulated_isr_task,
                            "simulated_isr",
                            SIMULATED_ISR_TASK_STACK_SIZE,
                            NULL,
                            SIMULATED_ISR_TASK_PRIORITY_OSA,
                            (task_param_t)(0),
                            false,
                            &simulated_isr_task_handler
                        );

    if (result != kStatus_OSA_Success) {
        printf("Creating simulated_isr_task failed\r\n");
    }
    else    {
        printf("Creating simulated_isr_task\r\n");
    }

    while (1)
    {
        if ( OSA_EventWait( &event, EVENT_FLAGS, false, OSA_WAIT_FOREVER, &flags ) != kStatus_OSA_Success )     {
            printf( "Event Wait failed\r\n" );
        }

        if ( OSA_EventClear( &event, EVENT_FLAGS ) != kStatus_OSA_Success ) {
            printf( "Event Clear failed\r\n" );
        }

        printf( "Tick\r\n");
    }
}

/*TASK************************************************************************
*
* Task Name    : simulated_isr_task
* Comments     :
*    This task opens a connection to the event group. After
*    delaying, it sets the event bits.
*END*************************************************************************/

void simulated_isr_task( task_param_t init_data )
{
    while (1) {
        OSA_TimeDelay(500);
        assert( OSA_EventSet( &event, EVENT_FLAGS ) == kStatus_OSA_Success );
    }
}

/* EOF */
