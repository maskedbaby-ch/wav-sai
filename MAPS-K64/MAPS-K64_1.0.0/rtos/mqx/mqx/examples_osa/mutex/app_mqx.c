/*HEADER**********************************************************************
*
* Copyright 2014 Freescale Semiconductor, Inc.
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
*   This file contains the MQX RTOS specific code for the lock example.
*
*END************************************************************************/

#include "fsl_os_abstraction.h"

/* RTOS specific header files */
#include "app_rtos.h"
/* Application specific header file */
#include "main.h"

enum {
    MAIN_TASK = 1,
    PRINT_TASK
} task_index_t;


#define TASK_TEMPLATE_LIST_END         {0, 0, 0, 0, 0, 0, 0}

const TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
    /* Task: Main Task */
    {
    /* Task number                      */  MAIN_TASK,
    /* Task entry point                 */  main_task,
    /* Task stack size                  */  MAIN_TASK_STACK_SIZE,
    /* Task priority                    */  MAIN_TASK_PRIORITY_RTOS,
    /* Task name                        */  "main",
    /* Task attributes                  */  (MQX_AUTO_START_TASK),
    /* Task parameter                   */  (0),
    /* Task time slice                  */  (0)
    },
    /* Task: Print Task */
    {
    /* Task number                      */  PRINT_TASK,
    /* Task entry point                 */  print_task,
    /* Task stack size                  */  PRINT_TASK_STACK_SIZE,
    /* Task priority                    */  PRINT_TASK_PRIORITY_RTOS,
    /* Task name                        */  "print",
    /* Task attributes                  */  (0),
    /* Task parameter                   */  (0),
    /* Task time slice                  */  (3)
    },
    /* End of Task Template List */
    TASK_TEMPLATE_LIST_END
};

/* EOF */
