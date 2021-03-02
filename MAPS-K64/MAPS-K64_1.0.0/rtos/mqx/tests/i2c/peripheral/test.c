/****************************************************************************
* 
*   This file contains MQX only stationery code.
*
****************************************************************************/
#include <mqx.h>
#include <bsp.h>
#include <mqx_prv.h>
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"

#include <i2c.h>
#include "test.h"
#include "eeprom.h"

#define MAIN_TASK	1

void main_task(uint32_t);

TASK_TEMPLATE_STRUCT MQX_template_list[] = 
{ 
/*  Task number, Entry point, Stack, Pri, String, Auto? */
   {MAIN_TASK,   main_task,   2000,  7,   "main", MQX_AUTO_START_TASK},  
   {0,           0,           0,     0,   0,      0,                 }
};

/*TASK*--------------------------------------------------------------
* 
* Task Name : tc_1_main_task
* Comments  :
*
*END*--------------------------------------------------------------*/
void tc_1_main_task()
{
	MQX_FILE_PTR         		fd;
	uint32_t                     i, j;
	_mqx_int                    param, result;
	I2C_STATISTICS_STRUCT       stats;
	unsigned char                   *buffer;
	uint8_t						temp;
	bool						bResult;
			 
 
	/* Allocate receive buffer */
	buffer = _mem_alloc_zero(BUFFER_SIZE);
	if (buffer == NULL) 
	{
		IOClient_PTAC_CloseTestingModule();
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
	}	

	/* Open the I2C driver */         
	fd = fopen(I2C_DEVICE_POLLED, NULL);
	EU_ASSERT_REF_TC_MSG(tc_1_main_task, fd != NULL, "Test #1.0 Testing I2C Open");
	if (fd == NULL) 
	{	
		IOClient_PTAC_CloseTestingModule();
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
	}

	/* Test ioctl commands */
	param = I2C_BUS_BAUDRATE;
	result = ioctl(fd, IO_IOCTL_I2C_SET_BAUD, &param);
	EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_OK, "Test #1.1 Testing I2C Set baud");

	/* Get current baud rate */
	result = ioctl(fd, IO_IOCTL_I2C_GET_BAUD, &param);
	EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == I2C_OK) && (param == I2C_BUS_BAUDRATE), "Test #1.2 Testing I2C Get baud");
	if (param != I2C_BUS_BAUDRATE)
	{
		result = fclose (fd);
		IOClient_PTAC_CloseTestingModule();
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
	}
	
	/* Set master mode */
	result = ioctl(fd, IO_IOCTL_I2C_SET_MASTER_MODE, NULL);
	EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_OK, "Test #1.3 Testing I2C Set mode");

	/* Get current mode */
	result = ioctl(fd, IO_IOCTL_I2C_GET_MODE, &param);
	EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_OK, "Test #1.4 Testing I2C Get mode");
	   
	param = 0x60;
	/*Set station address */
	result = ioctl(fd, IO_IOCTL_I2C_SET_STATION_ADDRESS, &param);
	EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_OK, "Test #1.5 Testing I2C Set station address");
	   
	param = 0x00;
	/*Get station address */
	result = ioctl(fd, IO_IOCTL_I2C_GET_STATION_ADDRESS, &param);
	EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == I2C_OK) && (param == 0x60), "Test #1.6 Testing I2C Set station address");
	
	/*Clear statistics */
	result = ioctl(fd, IO_IOCTL_I2C_CLEAR_STATISTICS, NULL);
	EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_OK, "Test #1.7 Testing I2C Clear statistics");
	   
	/*Get statistics */
	result = ioctl(fd, IO_IOCTL_I2C_GET_STATISTICS, (void *)&stats);
	EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_OK, "Test #1.8 Testing I2C Get statistics");
																				
	/*Get current state*/
	result = ioctl(fd, IO_IOCTL_I2C_GET_STATE, &param);
	EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_OK, "Test #1.9 Testing I2C Get current state");
	   
	param = I2C_EEPROM_BUS_ADDRESS;
	/*Set destination address */
	result = ioctl(fd, IO_IOCTL_I2C_SET_DESTINATION_ADDRESS, &param);
	EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_OK, "Test #1.10 Testing I2C Set destination address");
	
	param = 0x00;
	/* Get destination address */
	result = ioctl(fd, IO_IOCTL_I2C_GET_DESTINATION_ADDRESS, &param);
	EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == I2C_OK) && (param == I2C_EEPROM_BUS_ADDRESS), "Test #1.11 Testing I2C Get destination address");
	if (param != I2C_EEPROM_BUS_ADDRESS)
	{
		result = fclose (fd);
		IOClient_PTAC_CloseTestingModule();
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
	}
		   
	/* Test EEPROM communication */
	i2c_write_eeprom_polled (fd, I2C_EEPROM_MEMORY_ADDRESS1, (unsigned char *)TEST_STRING, 0);
	
	i2c_read_eeprom_polled (fd, I2C_EEPROM_MEMORY_ADDRESS1, buffer, 0);

	i2c_write_eeprom_polled (fd, I2C_EEPROM_MEMORY_ADDRESS1, (unsigned char *)TEST_STRING, 1);
	
	i2c_read_eeprom_polled (fd, I2C_EEPROM_MEMORY_ADDRESS1, buffer, 1);
	
	EU_ASSERT_REF_TC_MSG(tc_1_main_task, buffer[0] == TEST_STRING[0], "Test #1.16 I2C communication");
	if (buffer[0] != TEST_STRING[0])
	{
		result = fclose (fd);
		IOClient_PTAC_CloseTestingModule();
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
	}

	_mem_zero (buffer, BUFFER_SIZE);
	i2c_write_eeprom_polled (fd, I2C_EEPROM_MEMORY_ADDRESS1, buffer, sizeof(TEST_STRING));
	
	i2c_read_eeprom_polled (fd, I2C_EEPROM_MEMORY_ADDRESS1, buffer, sizeof(TEST_STRING));
	
	for (i = 0; i < BUFFER_SIZE; i++)
	{
		if (buffer[i] != 0)
	    {
			break;
	    }
	}
	
	EU_ASSERT_REF_TC_MSG(tc_1_main_task, i == BUFFER_SIZE, "Test #1.19 I2C communication");
	if (i != BUFFER_SIZE)
	{
		result = fclose (fd);
		IOClient_PTAC_CloseTestingModule();
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
	}
	   
	i2c_write_eeprom_polled (fd, I2C_EEPROM_MEMORY_ADDRESS1, (unsigned char *)TEST_STRING, sizeof(TEST_STRING));
	
	i2c_read_eeprom_polled (fd, I2C_EEPROM_MEMORY_ADDRESS1, buffer, sizeof(TEST_STRING));
		
	for (i = 0; i < sizeof(TEST_STRING); i++)
	{
		if (buffer[i] != TEST_STRING[i])
		{
			break;
		}
	}
	
	EU_ASSERT_REF_TC_MSG(tc_1_main_task, i == sizeof(TEST_STRING), "Test #1.22 I2C communication");
	if (i != sizeof(TEST_STRING))
	{
		result = fclose (fd);
		IOClient_PTAC_CloseTestingModule();
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
	}
	
	/* Close the driver */
	result = fclose (fd);
	EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_OK, "Test #1.23 Testing I2C Close");	
}

/*TASK*--------------------------------------------------------------
* 
* Task Name : tc_2_main_task
* Comments  :
*
*END*--------------------------------------------------------------*/
void tc_2_main_task()
{
	MQX_FILE_PTR         		fd;
	uint32_t                     i, j;
	_mqx_int                    param, result;
	I2C_STATISTICS_STRUCT       stats;
	unsigned char                   *buffer;
	uint8_t						temp;
	bool						bResult;
	
	/* Allocate receive buffer */
	buffer = _mem_alloc_zero(BUFFER_SIZE);
	if (buffer == NULL) 
	{
		IOClient_PTAC_CloseTestingModule();
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
	}	

	/* Open the I2C driver */         
	fd = fopen(I2C_DEVICE_INTERRUPT, NULL);
	EU_ASSERT_REF_TC_MSG(tc_2_main_task, fd != NULL, "Test #2.0 Testing I2C interrupt Open");
	if (fd == NULL) 
	{	
		IOClient_PTAC_CloseTestingModule();
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
	}

	/* Test ioctl commands */
	param = I2C_BUS_BAUDRATE;
	result = ioctl(fd, IO_IOCTL_I2C_SET_BAUD, &param);
	EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == I2C_OK, "Test #2.1 Testing I2C Set baud");

	/* Get current baud rate */
	result = ioctl(fd, IO_IOCTL_I2C_GET_BAUD, &param);
	EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == I2C_OK) && (param == I2C_BUS_BAUDRATE), "Test #2.2 Testing I2C Get baud");
	if (param != I2C_BUS_BAUDRATE)
	{
		result = fclose (fd);
		IOClient_PTAC_CloseTestingModule();
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
	}
	
	/* Set master mode */
	result = ioctl(fd, IO_IOCTL_I2C_SET_MASTER_MODE, NULL);
	EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == I2C_OK, "Test #2.3 Testing I2C Set mode");

	/* Get current mode */
	result = ioctl(fd, IO_IOCTL_I2C_GET_MODE, &param);
	EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == I2C_OK, "Test #2.4 Testing I2C Get mode");
	   
	param = 0x60;
	/*Set station address */
	result = ioctl(fd, IO_IOCTL_I2C_SET_STATION_ADDRESS, &param);
	EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == I2C_OK, "Test #2.5 Testing I2C Set station address");
	   
	param = 0x00;
	/*Get station address */
	result = ioctl(fd, IO_IOCTL_I2C_GET_STATION_ADDRESS, &param);
	EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == I2C_OK) && (param == 0x60), "Test #2.6 Testing I2C Set station address");
	
	/*Clear statistics */
	result = ioctl(fd, IO_IOCTL_I2C_CLEAR_STATISTICS, NULL);
	EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == I2C_OK, "Test #2.7 Testing I2C Clear statistics");
	   
	/*Get statistics */
	result = ioctl(fd, IO_IOCTL_I2C_GET_STATISTICS, (void *)&stats);
	EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == I2C_OK, "Test #2.8 Testing I2C Get statistics");
																				
	/*Get current state*/
	result = ioctl(fd, IO_IOCTL_I2C_GET_STATE, &param);
	EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == I2C_OK, "Test #2.9 Testing I2C Get current state");
	   
	param = I2C_EEPROM_BUS_ADDRESS;
	/*Set destination address */
	result = ioctl(fd, IO_IOCTL_I2C_SET_DESTINATION_ADDRESS, &param);
	EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == I2C_OK, "Test #2.10 Testing I2C Set destination address");
	
	param = 0x00;
	/* Get destination address */
	result = ioctl(fd, IO_IOCTL_I2C_GET_DESTINATION_ADDRESS, &param);
	EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == I2C_OK) && (param == I2C_EEPROM_BUS_ADDRESS), "Test #2.11 Testing I2C Get destination address");
	if (param != I2C_EEPROM_BUS_ADDRESS)
	{
		result = fclose (fd);
		IOClient_PTAC_CloseTestingModule();
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
	}
		   
	/* Test EEPROM communication */
	i2c_write_eeprom_interrupt (fd, I2C_EEPROM_MEMORY_ADDRESS2, (unsigned char *)TEST_STRING, 0);
	
	i2c_read_eeprom_interrupt (fd, I2C_EEPROM_MEMORY_ADDRESS2, buffer, 0);

	i2c_write_eeprom_interrupt (fd, I2C_EEPROM_MEMORY_ADDRESS2, (unsigned char *)TEST_STRING, 1);
	
	i2c_read_eeprom_interrupt (fd, I2C_EEPROM_MEMORY_ADDRESS2, buffer, 1);
	
	EU_ASSERT_REF_TC_MSG(tc_2_main_task, buffer[0] == TEST_STRING[0], "Test #2.16 I2C communication");
	if (buffer[0] != TEST_STRING[0])
	{
		result = fclose (fd);
		IOClient_PTAC_CloseTestingModule();
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
	}

	_mem_zero (buffer, BUFFER_SIZE);
	i2c_write_eeprom_interrupt (fd, I2C_EEPROM_MEMORY_ADDRESS2, buffer, sizeof(TEST_STRING));
	
	i2c_read_eeprom_interrupt (fd, I2C_EEPROM_MEMORY_ADDRESS2, buffer, sizeof(TEST_STRING));
	
	for (i = 0; i < BUFFER_SIZE; i++)
	{
		if (buffer[i] != 0)
	    {
			break;
	    }
	}
	
	EU_ASSERT_REF_TC_MSG(tc_2_main_task, i == BUFFER_SIZE, "Test #2.19 I2C communication");
	if (i != BUFFER_SIZE)
	{
		result = fclose (fd);
		IOClient_PTAC_CloseTestingModule();
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
	}
	   
	i2c_write_eeprom_interrupt (fd, I2C_EEPROM_MEMORY_ADDRESS2, (unsigned char *)TEST_STRING, sizeof(TEST_STRING));
	
	i2c_read_eeprom_interrupt (fd, I2C_EEPROM_MEMORY_ADDRESS2, buffer, sizeof(TEST_STRING));

	for (i = 0; i < sizeof(TEST_STRING); i++)
	{
		if (buffer[i] != TEST_STRING[i])
		{
			break;
		}
	}

	EU_ASSERT_REF_TC_MSG(tc_2_main_task, i == sizeof(TEST_STRING), "Test #2.22 I2C communication");
	if (i != sizeof(TEST_STRING))
	{
		result = fclose (fd);
		IOClient_PTAC_CloseTestingModule();
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
	}	
	
	/* Close the driver */
	result = fclose (fd);
	EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == I2C_OK, "Test #2.23 Testing I2C Close");	


}
/******************** End Testcases ********************/

//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_1)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST1 - Test I2C Polled driver"),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST2 - Test I2C Interrupt driver"),
EU_TEST_SUITE_END(suite_1)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_1, " - I2C peripheral test")
EU_TEST_REGISTRY_END()
//------------------------------------------------------------------------

/*TASK*--------------------------------------------------------------
* 
* Task Name : main_task
* Comments  :
*
*END*--------------------------------------------------------------*/
void main_task(uint32_t dummy)
{
	IOClient_Init(UART3, 9600, BSP_SYSTEM_CLOCK);
	
	IOClient_PTAC_OpenTestingModule(IOClient_TestPoint_I2C0);
	
	IOClient_I2C_SetSlaveStart(	IOClient_TestPoint_I2C0,
								I2C_EEPROM_BUS_ADDRESS, 
								I2C_BUS_BAUDRATE,
								I2C_EEPROM_SIZE,
								I2C_EEPROM_PAGE_SIZE);
	
	eunit_mutex_init();
	EU_RUN_ALL_TESTS(eunit_tres_array);
	
	IOClient_PTAC_CloseTestingModule();
	eunit_test_stop();
    _mqx_exit(0);
}

/* EOF */
