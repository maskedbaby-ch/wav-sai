/*HEADER**********************************************************************
*
* Copyright 2014 Freescale Semiconductor, Inc.
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
*   This file contains the MQX RTOS specific code for msgq osa example.
*
*END************************************************************************/

#include "fsl_os_abstraction.h"

/* RTOS specific header files */
#include "app_rtos.h"
/* Application specific header file */
#include "main.h"

enum {
    SERVER_TASK = 1,
    CLIENT_TASK
} task_index_t;


#define TASK_TEMPLATE_LIST_END         {0, 0, 0, 0, 0, 0, 0}

const TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
    /* Task: Server Task */
    {
    /* Task number                      */  SERVER_TASK,
    /* Task entry point                 */  server_task,
    /* Task stack size                  */  SERVER_TASK_STACK_SIZE,
    /* Task priority                    */  SERVER_TASK_PRIORITY_RTOS,
    /* Task name                        */  "server",
    /* Task attributes                  */  (MQX_AUTO_START_TASK),
    /* Task parameter                   */  (0),
    /* Task time slice                  */  (0)
    },
    /* Task: Client Task */
    {
    /* Task number                      */  CLIENT_TASK,
    /* Task entry point                 */  client_task,
    /* Task stack size                  */  CLIENT_TASK_STACK_SIZE,
    /* Task priority                    */  CLIENT_TASK_PRIORITY_RTOS,
    /* Task name                        */  "client",
    /* Task attributes                  */  (0),
    /* Task parameter                   */  (0),
    /* Task time slice                  */  (0)
    },
    /* End of Task Template List */
    TASK_TEMPLATE_LIST_END
};

/* EOF */
