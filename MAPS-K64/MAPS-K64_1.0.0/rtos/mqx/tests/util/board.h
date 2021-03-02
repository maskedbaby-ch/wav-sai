/*!
	@file board.h
	@version 0.0.1.0
	
	\brief board depended definition
	\project MQX Test Application Software
	\author Vadym Volokitin, 
	\author Freescale Semiconductor, Roznov pod Radhostem, Czech Republic
	Freescale Semiconductor Inc.
	(c) Copyright 2008 Freescale Semiconductor Inc.
	ALL RIGHTS RESERVED.
*/
//#ifndef IOCLIENT_H
//#define IOCLIENT_H

/******************* inclusions **********************************************/

#include <bsp.h>

//#include "IOClient_Types.h"
//#include "IOClient_Api.h"
//#endif /* IOCLIENT_H */
 
 /* TWRMCF51CN - Lasko board definition */
#ifdef BSP_TWRMCF51CN
#define TEST_B12    (GPIO_PORT_TC | GPIO_PIN1)
#define TEST_B13    (GPIO_PORT_TB | GPIO_PIN1)
#define TEST_B14    (GPIO_PORT_TB | GPIO_PIN3)
#define TEST_B15    (GPIO_PORT_TB | GPIO_PIN4)
#define TEST_B16    (GPIO_PORT_TB | GPIO_PIN2)
#define TEST_B17    (GPIO_PORT_TC | GPIO_PIN0)
#define TEST_B18    (GPIO_PORT_TB | GPIO_PIN7)
#define TEST_B19    (GPIO_PORT_TB | GPIO_PIN6)
#define TEST_B20    (GPIO_PORT_TB | GPIO_PIN5)
#define TEST_B21    (GPIO_PORT_TF | GPIO_PIN5) //GPIO1
#define TEST_B22    (GPIO_PORT_TF | GPIO_PIN4) //GPIO2
#define TEST_B23    (GPIO_PORT_TD | GPIO_PIN2) //GPIO3
#define TEST_B35    (GPIO_PORT_TE | GPIO_PIN1) //GPIO4
#define TEST_B52    (GPIO_PORT_TH | GPIO_PIN2) //GPIO5
#define TEST_A35    (GPIO_PORT_TE | GPIO_PIN0) //GPIO6
#define TEST_A11    (GPIO_PORT_TG | GPIO_PIN7) //GPIO7
#define TEST_A10    (GPIO_PORT_TG | GPIO_PIN5) //GPIO8
#define TEST_A9     (GPIO_PORT_TG | GPIO_PIN6) //GPIO9

/* GPIO */

/* TWRMCF51CN PINS */ 
#define GPIO1 TEST_B21
#define GPIO2 TEST_B22 //TEST_B17
#define GPIO3 TEST_B20 //TEST_B23 - overload, any idea
#define GPIO4 TEST_B19 //TEST_B35
#define GPIO5 TEST_B52
#define GPIO6 TEST_B18 //TEST_A35
#define GPIO7 TEST_A11
#define GPIO8 TEST_B19 //TEST_A10 - overload
#define GPIO9 TEST_A9 
/* MEZZANINE BOARD PINS */
#define IOClient_TestPoint_GPIO1 IOClient_TestPoint_Port_B21
#define IOClient_TestPoint_GPIO2 IOClient_TestPoint_Port_B22
#define IOClient_TestPoint_GPIO3 IOClient_TestPoint_Port_B20
#define IOClient_TestPoint_GPIO4 IOClient_TestPoint_Port_B19
#define IOClient_TestPoint_GPIO5 IOClient_TestPoint_Port_B52
#define IOClient_TestPoint_GPIO6 IOClient_TestPoint_Port_B18
#define IOClient_TestPoint_GPIO7 IOClient_TestPoint_Port_A11
#define IOClient_TestPoint_GPIO8 IOClient_TestPoint_Port_B19
#define IOClient_TestPoint_GPIO9 IOClient_TestPoint_Port_A9

#endif