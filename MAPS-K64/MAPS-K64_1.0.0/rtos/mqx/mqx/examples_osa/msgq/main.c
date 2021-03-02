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
*   This file contains the source for the msgq osa example program.
*
*END************************************************************************/

#include <stdio.h>
#include <assert.h>

#include "fsl_os_abstraction.h"
#include "app_rtos.h"
#include "main.h"

/* Handlers definitions */
msg_queue_handler_t server_queue_handler;
msg_queue_handler_t client_queue_handler;

task_handler_t client_task_handler;


/* Use light weight message queues */
MSG_QUEUE_DECLARE(server_queue, NUM_MESSAGES, MSG_SIZE);
MSG_QUEUE_DECLARE(client_queue, NUM_MESSAGES, MSG_SIZE);

/*TASK************************************************************************
*
* Task Name : server_task
* Comments  : This task initializes the message queues,
*  creates three client tasks, and then waits for a message.
*  After recieving a message, the task returns the message to
*  the sender.

*END*************************************************************************/

void server_task ( task_param_t init_data )
{
    msg_queue_t         msg;
    msg_queue_t         *pmsg;
    osa_status_t        result;
    uint32_t            i;

    OSA_TASK_DEFINE(client_task, CLIENT_TASK_STACK_SIZE);

    puts("\r\n****************** MSGQ OSA Example ******************\r\n");

    server_queue_handler = OSA_MsgQCreate((msg_queue_t *)&server_queue, NUM_MESSAGES, (sizeof(msg) + sizeof(unsigned int) - 1) / sizeof(unsigned int));
    client_queue_handler = OSA_MsgQCreate((msg_queue_t *)&client_queue, NUM_MESSAGES, (sizeof(msg) + sizeof(unsigned int) - 1) / sizeof(unsigned int));

    /* create the client tasks */
    for (i = 0; i < NUM_CLIENTS; i++) {
        printf("Creating client task %c\r\n", 'A' + i);
        result = OSA_TaskCreate( client_task,
                              "client",
                              CLIENT_TASK_STACK_SIZE,
                              NULL,
                              CLIENT_TASK_PRIORITY_OSA,
                              (task_param_t)i,
                              false,
                              &client_task_handler
                            );
        if (result != kStatus_OSA_Success) {
            /* task creation failed */
        } /* Endif */
        OSA_TimeDelay(100);
    }

    printf("*****\r\n");

    while (TRUE) {
        pmsg = &msg;
        OSA_MsgQGet(server_queue_handler, &pmsg, OSA_WAIT_FOREVER);
        printf("Server task received %c \r\n", msg);
        OSA_MsgQPut(client_queue_handler, &pmsg);
    }
}


/*TASK************************************************************************
*
* Task Name : client_task
* Comments  : This task sends a message to the server_task and
*   then waits for a reply.
*
*END*************************************************************************/

void client_task ( task_param_t init_data )
{
    msg_queue_t msg;
    msg_queue_t *pmsg;
    char* msg_c;
    char taskid = 'A'+ init_data;

    printf("Client task %c created\r\n", taskid);
    /* Wait for other client task to be created */
    OSA_TimeDelay(500);


    while (TRUE) {
        msg_c = (char *)&msg;
        *msg_c = '0'+ init_data;
        printf("Client task %c sending %c\r\n", taskid, *msg_c);
        (void)OSA_TaskYield();
        OSA_MsgQPut(server_queue_handler, &msg);
        /* wait for a return message */
        pmsg = &msg;
        OSA_MsgQGet(client_queue_handler, &pmsg, OSA_WAIT_FOREVER);
    }
}
