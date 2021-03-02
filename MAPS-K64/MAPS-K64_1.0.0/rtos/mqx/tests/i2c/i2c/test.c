/**HEADER********************************************************************
*
* Copyright (c) 2013 Freescale Semiconductor;
* All Rights Reserved
*
***************************************************************************
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************
*
* $FileName: test.c$
* $Version :
* $Date    : Aug-5-2013$
*
* Comments:
*
*   This file contains the source for a simple example of an
*   an application that open and allocate the i2c.
*   [ENGR00275792][tests][i2c-per-mma]I2c baudrate failure in line 130(REQUIREMENT I2C013)
*   [ENGR00277314][I2C047]IO_IOCTL_I2C_CLEAR_STATISTICS can't fail(REQUIREMENT I2C047)
*   [ENGR00278063][I2C049]IO_IOCTL_I2C_DISABLE_DEVICE requirement issue(REQUIREMENT I2C049)
*   [ENGR00278068][I2C051]IO_IOCTL_I2C_ENABLE_DEVICE requirement issue(REQUIREMENT I2C051)
*END************************************************************************/
#include <mqx.h>
#include <bsp.h>
#include <test.h>
#include "EUnit.h"
#include "eunit_crit_secs.h"
#define I2C_BUS_BAUDRATE 100000
#define BUFFER_SIZE 256
#define MAIN_TASK 10
#define TEST1_TASK 11

extern void main_task(uint32_t);
extern void busy_task(uint32_t);
void tc_1_main_task(void);// Test #1 - Testing Open and allocate the i2c by polled driver. Test the APIs.
void tc_2_main_task(void);// Test #2 - Testing Test while device is in progress
void tc_3_main_task(void);// Test #3 - Testing Open and allocate the i2c by interrupt driver. Test the APIs.

TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
  /* Task Index,   Function,  Stack,  Priority, Name,   Attributes,          Param, Time Slice */
   { MAIN_TASK,    main_task, 2000L,    8L,     "Main", MQX_AUTO_START_TASK, 0,     0 },
   { TEST1_TASK,   busy_task,   2000L,    6L,   "busy_task", 0, 0,0},
   { 0 }
};

MQX_FILE_PTR fd;

/*TASK*-------------------------------------------------------------------
*
* Task Name    : busy_task
* Comments     :
* Set the i2c in progress.
*END*----------------------------------------------------------------------*/
void busy_task
   (
      uint32_t dummy
   )
{
  unsigned char * buffer;
  write(fd,buffer,1);
}

/*TASK*-------------------------------------------------------------------
*
* Task Name : tc_1_main_task
* Comments  : Open and allocate the i2c by polled driver. Test the APIs.
* Requirements:
*   I2C002,I2C003,I2C004,I2C005,I2C006,I2C011,I2C012,I2C013,I2C014,I2C015,
*   I2C017,I2C019,I2C020,I2C021,I2C022,I2C023,I2C024,I2C025,I2C026,I2C027,
*   I2C028,I2C035,I2C043,I2C044,I2C045,I2C046,I2C050,I2C052,I2C054,I2C057
*END*----------------------------------------------------------------------*/
void tc_1_main_task()
{
   MQX_FILE_PTR fd_invalid,param_invalid;
   _mqx_int param, result;
   I2C_STATISTICS_STRUCT stats;
   unsigned char * buffer;
   /* Allocate receive buffer */
   buffer = _mem_alloc_zero (BUFFER_SIZE);
   EU_ASSERT_REF_TC_MSG( tc_1_main_task, buffer != NULL, "Test #1 Testing 1.1 Getting receive buffer");
   if (buffer == NULL)
   {
     _time_delay_ticks(4);
     eunit_test_stop();
     _mqx_exit(1);
   };
   /* Open the I2C driver polled . Requirements I2C002 I2C003*/
   fd = fopen (I2C_DEVICE_POLLED, NULL);
   EU_ASSERT_REF_TC_MSG( tc_1_main_task, fd != NULL, "Test #1 Testing 1.2 _io_fopen");
   if (fd == NULL)
   {
     _time_delay_ticks(4);
     eunit_test_stop();
     _mqx_exit(1);
   };
   /* Open the I2C driver failed . Requirements I2C004*/
   fd_invalid=fopen (0, NULL);
   EU_ASSERT_REF_TC_MSG( tc_1_main_task, fd_invalid==NULL, "Test #1 Testing 1.3 _io_fopen failed");

   /* Test ioctl commands.*/
   param = I2C_BUS_BAUDRATE;

   /* Enable the i2c device . Requirements I2C050*/
   result = ioctl (fd, IO_IOCTL_I2C_ENABLE_DEVICE , NULL);
   EU_ASSERT_REF_TC_MSG( tc_1_main_task, result == MQX_OK , "Test #1 Testing 1.4 IO_IOCTL_I2C_ENABLE_DEVICE")

   /* Set baud rate . Requirement I2C011*/
   result = ioctl(fd, IO_IOCTL_I2C_SET_BAUD, &param);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1 Testing 1.5 IO_IOCTL_I2C_SET_BAUD");

   /* Set baud rate fail. Requirement I2C012 I2C057*/
   param_invalid=NULL;
   result = ioctl(fd, IO_IOCTL_I2C_SET_BAUD, param_invalid);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_ERROR_INVALID_PARAMETER, "Test #1 Testing 1.6 IO_IOCTL_I2C_SET_BAUD failed output I2C_ERROR_INVALID_PARAMETER");
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_ERROR_INVALID_PARAMETER, "Test #1 Testing 1.7 failure I2C_ERROR_INVALID_PARAMETER");
   #if 0
   /* Get current baud rate . Requirement I2C013 [ENGR00275792][tests][i2c-per-mma]I2c baudrate failure */
   result = ioctl(fd, IO_IOCTL_I2C_GET_BAUD, &param);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK) && (param == I2C_BUS_BAUDRATE), "Test #1 Testing 1.8 IO_IOCTL_I2C_GET_BAUD");
   #endif
   /* Get current baud rate fail. Requirement I2C014 [ENGR00275792][tests][i2c-per-mma]I2c baudrate failure */
   result = ioctl(fd, IO_IOCTL_I2C_GET_BAUD, param_invalid);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_ERROR_INVALID_PARAMETER, "Test #1 Testing 1.9 IO_IOCTL_I2C_GET_BAUD failed output I2C_ERROR_INVALID_PARAMETER");

   /* Set slave mode . Requirement I2C017*/
   result = ioctl(fd, IO_IOCTL_I2C_SET_SLAVE_MODE, NULL);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1 Testing 1.10 IO_IOCTL_I2C_SET_SLAVE_MODE");

   /* Set master mode Requirement I2C015*/
   result = ioctl(fd, IO_IOCTL_I2C_SET_MASTER_MODE, NULL);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1 Testing 1.11 IO_IOCTL_I2C_SET_MASTER_MODE");

   /* Get current mode Requirement I2C019*/
   result = ioctl(fd, IO_IOCTL_I2C_GET_MODE, &param);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK) && (param == I2C_MODE_MASTER), "Test #1 Testing 1.12 IO_IOCTL_I2C_GET_MODE");

   /* Get current mode fail . Requirement I2C020*/
   result = ioctl(fd, IO_IOCTL_I2C_GET_MODE, param_invalid);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_ERROR_INVALID_PARAMETER, "Test #1 Testing 1.13 IO_IOCTL_I2C_GET_MODE failed output I2C_ERROR_INVALID_PARAMETER");

   param = 0x60;
   /*Set station address . Requirement I2C021*/
   result = ioctl(fd, IO_IOCTL_I2C_SET_STATION_ADDRESS, &param);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1 Testing 1.14 IO_IOCTL_I2C_SET_STATION_ADDRESS");

   /*Set station address fail . Requirement I2C022*/
   result = ioctl(fd, IO_IOCTL_I2C_SET_STATION_ADDRESS, param_invalid);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_ERROR_INVALID_PARAMETER, "Test #1 Testing 1.15 IO_IOCTL_I2C_SET_STATION_ADDRESS failed output I2C_ERROR_INVALID_PARAMETER");

   param = 0x00;
   /*Get station address . Requirement I2C023*/
   result = ioctl(fd, IO_IOCTL_I2C_GET_STATION_ADDRESS, &param);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK) && (param == 0x60), "Test #1 Testing 1.16 IO_IOCTL_I2C_GET_STATION_ADDRESS");

   /*Get station address fail . Requirement I2C024*/
   result = ioctl(fd, IO_IOCTL_I2C_GET_STATION_ADDRESS, param_invalid);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_ERROR_INVALID_PARAMETER, "Test #1 Testing 1.17 IO_IOCTL_I2C_GET_STATION_ADDRESS failed output I2C_ERROR_INVALID_PARAMETER");

   /*Clear statistics . Requirement I2C046*/
   result = ioctl(fd, IO_IOCTL_I2C_CLEAR_STATISTICS, NULL);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1 Testing 1.18 IO_IOCTL_I2C_CLEAR_STATISTICS");

   /*Get statistics . Requirement I2C044*/
   result = ioctl(fd, IO_IOCTL_I2C_GET_STATISTICS, (void *)&stats);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1 Testing 1.19 IO_IOCTL_I2C_GET_STATISTICS");
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, stats.INTERRUPTS==0 , "Test #1 Testing 1.19.1 stats.INTERRUPTS");
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, stats.RX_PACKETS==0 , "Test #1 Testing 1.19.2 stats.RX_PACKETS");
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, stats.TX_PACKETS==0 , "Test #1 Testing 1.19.3 stats.TX_PACKETS");
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, stats.TX_LOST_ARBITRATIONS==0 , "Test #1 Testing 1.19.4 stats.TX_LOST_ARBITRATIONS");
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, stats.TX_ADDRESSED_AS_SLAVE==0 , "Test #1 Testing 1.19.5 stats.TX_ADDRESSED_AS_SLAVE");
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, stats.TX_NAKS==0, "Test #1 Testing 1.19.6 stats.TX_NAKS");

   /*Get statistics fail . Requirement I2C045*/
   result = ioctl(fd, IO_IOCTL_I2C_GET_STATISTICS, param_invalid);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_ERROR_INVALID_PARAMETER, "Test #1 Testing 1.20 IO_IOCTL_I2C_GET_STATISTICS failed output I2C_ERROR_INVALID_PARAMETER");

   /*Get current state . Requirement I2C035*/
   result = ioctl(fd, IO_IOCTL_I2C_GET_STATE, &param);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK) && (param==I2C_STATE_READY), "Test #1 Testing 1.21 I2C_STATE_READY");

   /*Get current state fail . Requirement I2C043*/
   result = ioctl(fd, IO_IOCTL_I2C_GET_STATE, param_invalid);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_ERROR_INVALID_PARAMETER, "Test #1 Testing 1.22 IO_IOCTL_I2C_GET_STATE failed");

   param =0x00;
   /*Set destination address . Requirement I2C025*/
   result = ioctl(fd, IO_IOCTL_I2C_SET_DESTINATION_ADDRESS, &param);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1 Testing 1.23 IO_IOCTL_I2C_SET_DESTINATION_ADDRESS");

   /*Set destination address fail .Requirement I2C026 */
   result = ioctl(fd, IO_IOCTL_I2C_SET_DESTINATION_ADDRESS, param_invalid);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_ERROR_INVALID_PARAMETER, "Test #1 Testing 1.24 IO_IOCTL_I2C_SET_DESTINATION_ADDRESS failed output I2C_ERROR_INVALID_PARAMETER");

   param = 0x00;
   /* Get destination address . Requirement I2C027*/
   result = ioctl(fd, IO_IOCTL_I2C_GET_DESTINATION_ADDRESS, &param);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK) && (param == 0x00), "Test #1 Testing 1.25 IO_IOCTL_I2C_GET_DESTINATION_ADDRESS");

   /* Get destination address fail . Requirement I2C028*/
   result = ioctl(fd, IO_IOCTL_I2C_GET_DESTINATION_ADDRESS, param_invalid);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_ERROR_INVALID_PARAMETER, "Test #1 Testing 1.26 IO_IOCTL_I2C_GET_DESTINATION_ADDRESS failed output I2C_ERROR_INVALID_PARAMETER");

   /*Gets the actual bus state . Requirement I2C052*/
   result = ioctl (fd, IO_IOCTL_I2C_GET_BUS_AVAILABILITY , &param);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK) && (param==I2C_BUS_IDLE), "Test #1 Testing 1.27 IO_IOCTL_I2C_GET_BUS_AVAILABILITY on I2C_BUS_IDLE");

   /*Gets the actual bus state fail . Requirement I2C054*/
   result = ioctl (fd, IO_IOCTL_I2C_GET_BUS_AVAILABILITY , param_invalid);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_ERROR_INVALID_PARAMETER, "Test #1 Testing 1.28 IO_IOCTL_I2C_GET_BUS_AVAILABILITY failed and output I2C_ERROR_INVALID_PARAMETER");

   /* Close the driver. Requirement I2C005 */
   result = fclose (fd);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK , "Test #1 Testing 1.29 _io_fclose()");

   /* Close the driver fail . Requirement I2C006 */
   result = fclose (NULL);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == IO_EOF , "Test #1 Testing 1.30 _io_fclose() failed and return IO_EOF");
}

/*TASK*-------------------------------------------------------------------
*
* Task Name : tc_2_main_task
* Comments  : Test while device is in progress
* Requirements:
*   I2C016,I2C018,I2C036,I2C042,I2C048,I2C053,I2C055
*END*----------------------------------------------------------------------*/
void tc_2_main_task()
{
  _mqx_int param,result,tid;
  fd = fopen (I2C_DEVICE_POLLED, NULL);

  /* create busy_task and make it run */
  tid = _task_create(0, TEST1_TASK, 0 );

  /* device is busy now, stop preemption */
  _task_stop_preemption();

  /*Get current state . Requirement I2C042*/
  result = ioctl(fd, IO_IOCTL_I2C_GET_STATE, &param);
  EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK) && (param==I2C_STATE_FINISHED ), "Test #2 Testing 2.1 I2C_STATE_FINISHED state ");

  /* Set slave mode fail. Requirement I2C018 I2C055*/
  result = ioctl(fd, IO_IOCTL_I2C_SET_SLAVE_MODE, NULL);
  EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == I2C_ERROR_DEVICE_BUSY, "Test #2 Testing 2.2 IO_IOCTL_I2C_SET_SLAVE_MODE fail and output I2C_ERROR_DEVICE_BUSY");

  /* Set master mode fail . Requirement I2C016*/
  result = ioctl(fd, IO_IOCTL_I2C_SET_MASTER_MODE, NULL);
  EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == I2C_ERROR_DEVICE_BUSY, "Test #2 Testing 2.3 IO_IOCTL_I2C_SET_MASTER_MODE fail");

  /*Gets the actual bus state . Requirement I2C053*/
   result = ioctl (fd, IO_IOCTL_I2C_GET_BUS_AVAILABILITY , &param);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK) && (param==I2C_BUS_BUSY), "Test #2 Testing 2.4 IO_IOCTL_I2C_GET_BUS_AVAILABILITY on I2C_BUS_BUSY");

  /*Get current state . Requirement I2C036*/
  ioctl(fd, IO_IOCTL_I2C_REPEATED_START, NULL);
  result = ioctl(fd, IO_IOCTL_I2C_GET_STATE, &param);
  EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK) && (param==I2C_STATE_REPEATED_START), "Test #2 Testing 2.5 I2C_STATE_REPEATED_START state ");

  _task_abort(tid);
  _task_start_preemption();

   /* Disable the i2c device . Requirements I2C048*/
   result = ioctl (fd, IO_IOCTL_I2C_DISABLE_DEVICE , NULL);
   EU_ASSERT_REF_TC_MSG( tc_2_main_task, result == MQX_OK , "Test #2 Testing 2.6 IO_IOCTL_I2C_DISABLE_DEVICE")
}
/*TASK*-------------------------------------------------------------------
*
* Task Name : tc_3_main_task
* Comments  : Open and allocate the i2c by interrupt driver. Test the APIs.
* Requirements:
*   I2C001
*END*----------------------------------------------------------------------*/
void tc_3_main_task()
{
   MQX_FILE_PTR fd_invalid,param_invalid;
   _mqx_int param, result;
   I2C_STATISTICS_STRUCT stats;
   unsigned char * buffer;
   /* Allocate receive buffer */
   buffer = _mem_alloc_zero (BUFFER_SIZE);
   EU_ASSERT_REF_TC_MSG( tc_1_main_task, buffer != NULL, "Test #3: Testing 3.1 Getting receive buffer");
   if (buffer == NULL)
   {
     _time_delay_ticks(4);
     eunit_test_stop();
     _mqx_exit(1);
   };
   /* Open the I2C driver interrupt . Requirements I2C002 I2C003*/
   fd = fopen (I2C_DEVICE_INTERRUPT, NULL);
   EU_ASSERT_REF_TC_MSG( tc_1_main_task, fd != NULL, "Test #3: Testing 3.2 _io_fopen");
   if (fd == NULL)
   {
     _time_delay_ticks(4);
     eunit_test_stop();
     _mqx_exit(1);
   };
   /* Open the I2C driver failed . Requirements I2C004*/
   fd_invalid=fopen (0, NULL);
   EU_ASSERT_REF_TC_MSG( tc_1_main_task, fd_invalid==NULL, "Test #3: Testing 3.3 _io_fopen failed");

   /* Test ioctl commands.*/
   param = I2C_BUS_BAUDRATE;

   /* Enable the i2c device . Requirements I2C050*/
   result = ioctl (fd, IO_IOCTL_I2C_ENABLE_DEVICE , NULL);
   EU_ASSERT_REF_TC_MSG( tc_1_main_task, result == MQX_OK , "Test #3: Testing 3.4 IO_IOCTL_I2C_ENABLE_DEVICE")

   /* Set baud rate . Requirement I2C011*/
   result = ioctl(fd, IO_IOCTL_I2C_SET_BAUD, &param);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #3 Testing 3.5 IO_IOCTL_I2C_SET_BAUD");

   /* Set baud rate fail. Requirement I2C012 I2C057*/
   param_invalid=NULL;
   result = ioctl(fd, IO_IOCTL_I2C_SET_BAUD, param_invalid);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_ERROR_INVALID_PARAMETER, "Test #3 Testing 3.6 IO_IOCTL_I2C_SET_BAUD failed output I2C_ERROR_INVALID_PARAMETER");
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_ERROR_INVALID_PARAMETER, "Test #3 Testing 3.7 failure I2C_ERROR_INVALID_PARAMETER");
   #if 0
   /* Get current baud rate . Requirement I2C013 [ENGR00275792][tests][i2c-per-mma]I2c baudrate failure */
   result = ioctl(fd, IO_IOCTL_I2C_GET_BAUD, &param);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK) && (param == I2C_BUS_BAUDRATE), "Test #3 Testing 3.8 IO_IOCTL_I2C_GET_BAUD");
   #endif
   /* Get current baud rate fail. Requirement I2C014 [ENGR00275792][tests][i2c-per-mma]I2c baudrate failure */
   result = ioctl(fd, IO_IOCTL_I2C_GET_BAUD, param_invalid);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_ERROR_INVALID_PARAMETER, "Test #3 Testing 3.9 IO_IOCTL_I2C_GET_BAUD failed output I2C_ERROR_INVALID_PARAMETER");

   /* Set slave mode . Requirement I2C017*/
   result = ioctl(fd, IO_IOCTL_I2C_SET_SLAVE_MODE, NULL);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #3 Testing 3.10 IO_IOCTL_I2C_SET_SLAVE_MODE");

   /* Set master mode Requirement I2C015*/
   result = ioctl(fd, IO_IOCTL_I2C_SET_MASTER_MODE, NULL);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #3 Testing 3.11 IO_IOCTL_I2C_SET_MASTER_MODE");

   /* Get current mode Requirement I2C019*/
   result = ioctl(fd, IO_IOCTL_I2C_GET_MODE, &param);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK) && (param == I2C_MODE_MASTER), "Test #3 Testing 3.12 IO_IOCTL_I2C_GET_MODE");

   /* Get current mode fail . Requirement I2C020*/
   result = ioctl(fd, IO_IOCTL_I2C_GET_MODE, param_invalid);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_ERROR_INVALID_PARAMETER, "Test #3 Testing 3.13 IO_IOCTL_I2C_GET_MODE failed output I2C_ERROR_INVALID_PARAMETER");

   param = 0x60;
   /*Set station address . Requirement I2C021*/
   result = ioctl(fd, IO_IOCTL_I2C_SET_STATION_ADDRESS, &param);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #3 Testing 3.14 IO_IOCTL_I2C_SET_STATION_ADDRESS");

   /*Set station address fail . Requirement I2C022*/
   result = ioctl(fd, IO_IOCTL_I2C_SET_STATION_ADDRESS, param_invalid);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_ERROR_INVALID_PARAMETER, "Test #3 Testing 3.15 IO_IOCTL_I2C_SET_STATION_ADDRESS failed output I2C_ERROR_INVALID_PARAMETER");

   param = 0x00;
   /*Get station address . Requirement I2C023*/
   result = ioctl(fd, IO_IOCTL_I2C_GET_STATION_ADDRESS, &param);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK) && (param == 0x60), "Test #3 Testing 3.16 IO_IOCTL_I2C_GET_STATION_ADDRESS");

   /*Get station address fail . Requirement I2C024*/
   result = ioctl(fd, IO_IOCTL_I2C_GET_STATION_ADDRESS, param_invalid);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_ERROR_INVALID_PARAMETER, "Test #3 Testing 3.17 IO_IOCTL_I2C_GET_STATION_ADDRESS failed output I2C_ERROR_INVALID_PARAMETER");

   /*Clear statistics */
   result = ioctl(fd, IO_IOCTL_I2C_CLEAR_STATISTICS, NULL);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #3 Testing 3.18 IO_IOCTL_I2C_CLEAR_STATISTICS");

   /*Get statistics . Requirement I2C044*/
   result = ioctl(fd, IO_IOCTL_I2C_GET_STATISTICS, (void *)&stats);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #3 Testing 3.19 IO_IOCTL_I2C_GET_STATISTICS");
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, stats.INTERRUPTS==0 , "Test #3 Testing 3.19.1 stats.INTERRUPTS");
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, stats.RX_PACKETS==0 , "Test #3 Testing 3.19.2 stats.RX_PACKETS");
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, stats.TX_PACKETS==0 , "Test #3 Testing 3.19.3 stats.TX_PACKETS");
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, stats.TX_LOST_ARBITRATIONS==0 , "Test #3 Testing 3.19.4 stats.TX_LOST_ARBITRATIONS");
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, stats.TX_ADDRESSED_AS_SLAVE==0 , "Test #3 Testing 3.19.5 stats.TX_ADDRESSED_AS_SLAVE");
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, stats.TX_NAKS==0, "Test #3 Testing 3.19.6 stats.TX_NAKS");

   /*Get statistics fail . Requirement I2C045*/
   result = ioctl(fd, IO_IOCTL_I2C_GET_STATISTICS, param_invalid);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_ERROR_INVALID_PARAMETER, "Test #3 Testing 3.20 IO_IOCTL_I2C_GET_STATISTICS failed output I2C_ERROR_INVALID_PARAMETER");

   /*Get current state . Requirement I2C035*/
   result = ioctl(fd, IO_IOCTL_I2C_GET_STATE, &param);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK) && (param==I2C_STATE_READY), "Test #3 Testing 3.21 I2C_STATE_READY");

   /*Get current state fail . Requirement I2C043*/
   result = ioctl(fd, IO_IOCTL_I2C_GET_STATE, param_invalid);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_ERROR_INVALID_PARAMETER, "Test #3 Testing 3.22 IO_IOCTL_I2C_GET_STATE failed");

   param =0x00;
   /*Set destination address . Requirement I2C025*/
   result = ioctl(fd, IO_IOCTL_I2C_SET_DESTINATION_ADDRESS, &param);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #3 Testing 3.23 IO_IOCTL_I2C_SET_DESTINATION_ADDRESS");

   /*Set destination address fail .Requirement I2C026 */
   result = ioctl(fd, IO_IOCTL_I2C_SET_DESTINATION_ADDRESS, param_invalid);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_ERROR_INVALID_PARAMETER, "Test #3 Testing 3.24 IO_IOCTL_I2C_SET_DESTINATION_ADDRESS failed output I2C_ERROR_INVALID_PARAMETER");

   param = 0x00;
   /* Get destination address . Requirement I2C027*/
   result = ioctl(fd, IO_IOCTL_I2C_GET_DESTINATION_ADDRESS, &param);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK) && (param == 0x00), "Test #3 Testing 3.25 IO_IOCTL_I2C_GET_DESTINATION_ADDRESS");

   /* Get destination address fail . Requirement I2C028*/
   result = ioctl(fd, IO_IOCTL_I2C_GET_DESTINATION_ADDRESS, param_invalid);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_ERROR_INVALID_PARAMETER, "Test #3 Testing 3.26 IO_IOCTL_I2C_GET_DESTINATION_ADDRESS failed output I2C_ERROR_INVALID_PARAMETER");

   /*Gets the actual bus state . Requirement I2C052*/
   result = ioctl (fd, IO_IOCTL_I2C_GET_BUS_AVAILABILITY , &param);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK) && (param==I2C_BUS_IDLE), "Test #3 Testing 3.27 IO_IOCTL_I2C_GET_BUS_AVAILABILITY on I2C_BUS_IDLE");

   /*Gets the actual bus state fail . Requirement I2C054*/
   result = ioctl (fd, IO_IOCTL_I2C_GET_BUS_AVAILABILITY , param_invalid);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_ERROR_INVALID_PARAMETER, "Test #3 Testing 3.28 IO_IOCTL_I2C_GET_BUS_AVAILABILITY failed and output I2C_ERROR_INVALID_PARAMETER");

   /* Close the driver. Requirement I2C005 */
   result = fclose (fd);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK , "Test #3 Testing 3.29 _io_fclose()");

   /* Close the driver fail . Requirement I2C006 */
   result = fclose (NULL);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == IO_EOF , "Test #3 Testing 3.30 _io_fclose() failed and return IO_EOF");
}
//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_1)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST1 - Test I2C Polled driver"),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST2 - Test I2C while device is in progress"),
	EU_TEST_CASE_ADD(tc_3_main_task, " TEST3 - Test I2C Interrupt driver")
EU_TEST_SUITE_END(suite_1)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_1, " - I2C common test")
EU_TEST_REGISTRY_END()
//------------------------------------------------------------------------
/*TASK*--------------------------------------------------------------
*
* Task Name : main_task
* Comments  :
*
*END*--------------------------------------------------------------*/
void main_task( uint32_t dummy)
{
  eunit_mutex_init();
  EU_RUN_ALL_TESTS(eunit_tres_array);
  eunit_test_stop();
  _mqx_exit(0);
}/*EOF*/