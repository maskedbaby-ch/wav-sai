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
*   an application that writes and reads the  MMA8451Q
*   using I2C driver.Run the case in 2g/4g/8g mode on polled mode, output the
*   data 10 times in each mode.
*   [ENGR00275792][tests][i2c-per-mma]I2c baudrate failure in line 87
*   [ENGR00279778]won't feed back ack after transfer 8bit data finished(i2c driver error in k70/k60)
*END************************************************************************/
#include <mqx.h>
#include <bsp.h>
#include <test.h>
#include <mma8451q.h>
#include "EUnit.h"
#include "eunit_crit_secs.h"
#define BUFFER_SIZE 256
#define MAIN_TASK 1
extern void main_task(uint32_t);

void tc_1_main_task(void);// Test #1 - Testing i2c polled-driver I/O settings and output data in 2g mode
void tc_2_main_task(void);// Test #2 - Testing i2c polled-driver I/O and output data in 4g mode
void tc_3_main_task(void);// Test #3 - Testing i2c polled-driver I/O and output data in 8g mode
TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
  /* Task Index,   Function,  Stack,  Priority, Name,   Attributes,          Param, Time Slice */
   { MAIN_TASK,    main_task, 2000L,    8L,     "Main", MQX_AUTO_START_TASK, 0,     0 },
   { 0 }
};
MQX_FILE_PTR                 fd;
/*TASK*-------------------------------------------------------------------
*
* Task Name : tc_1_main_task
* Comments  :
*   Open i2c driver to support I2C polled-driven I/O and configure mma,
*   run it in 2g mode and output data 10 times.
* Requirement
*   I2C058
*END*----------------------------------------------------------------------*/
void tc_1_main_task()
{
   _mqx_int                     param, result,c;
   I2C_STATISTICS_STRUCT        stats;
   unsigned char *              buffer;
   /* Allocate receive buffer */
   buffer = _mem_alloc_zero (BUFFER_SIZE);
   EU_ASSERT_REF_TC_MSG( tc_1_main_task, buffer != NULL, "Getting receive buffer");
   if (buffer == NULL)
   {
     _time_delay_ticks(4);
     eunit_test_stop();
     _mqx_exit(1);
   };
   /* Open the I2C driver */
   fd = fopen (I2C_DEVICE_POLLED, NULL);
   EU_ASSERT_REF_TC_MSG( tc_1_main_task, fd != NULL, "Test _io_fopen");
   if (fd == NULL)
   {
     _time_delay_ticks(4);
     eunit_test_stop();
     _mqx_exit(1);
   };
   /* Test ioctl commands */
   param = I2C_BUS_BAUDRATE;
   result = ioctl (fd, IO_IOCTL_I2C_ENABLE_DEVICE , NULL);
   EU_ASSERT_REF_TC_MSG( tc_1_main_task, result == I2C_OK , "Test IO_IOCTL_I2C_ENABLE_DEVICE")
   /* Set baud rate */
   result = ioctl(fd, IO_IOCTL_I2C_SET_BAUD, &param);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_OK, "Test IO_IOCTL_I2C_SET_BAUD");
   #if 0
   /* Get current baud rate [ENGR00275792][tests][i2c-per-mma]I2c baudrate failure*/
   result = ioctl(fd, IO_IOCTL_I2C_GET_BAUD, &param);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == I2C_OK) && (param == I2C_BUS_BAUDRATE), "Test IO_IOCTL_I2C_GET_BAUD");
   #endif
   /* Set master mode */
   result = ioctl(fd, IO_IOCTL_I2C_SET_MASTER_MODE, NULL);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_OK, "Test IO_IOCTL_I2C_SET_MASTER_MODE");
   /* Get current mode */
   result = ioctl(fd, IO_IOCTL_I2C_GET_MODE, &param);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == I2C_OK) && (param == I2C_MODE_MASTER), "Test IO_IOCTL_I2C_GET_MODE");
   param = 0x60;
   /*Set station address */
   result = ioctl(fd, IO_IOCTL_I2C_SET_STATION_ADDRESS, &param);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_OK, "Test IO_IOCTL_I2C_SET_STATION_ADDRESS");
   param = 0x00;
   /*Get station address */
   result = ioctl(fd, IO_IOCTL_I2C_GET_STATION_ADDRESS, &param);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == I2C_OK) && (param == 0x60), "Test IO_IOCTL_I2C_GET_STATION_ADDRESS");
   /*Clear statistics */
   result = ioctl(fd, IO_IOCTL_I2C_CLEAR_STATISTICS, NULL);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_OK, "Test IO_IOCTL_I2C_CLEAR_STATISTICS");
   /*Get statistics */
   result = ioctl(fd, IO_IOCTL_I2C_GET_STATISTICS, (void *)&stats);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_OK, "Test IO_IOCTL_I2C_GET_STATISTICS");
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, stats.INTERRUPTS==0 , "Test stats.INTERRUPTS");
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, stats.RX_PACKETS==0 , "Test stats.RX_PACKETS");
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, stats.TX_PACKETS==0 , "Test stats.TX_PACKETS");
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, stats.TX_LOST_ARBITRATIONS==0 , "Test stats.TX_LOST_ARBITRATIONS");
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, stats.TX_ADDRESSED_AS_SLAVE==0 , "Test stats.TX_ADDRESSED_AS_SLAVE");
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, stats.TX_NAKS==0, "Test stats.TX_NAKS");
   /*Get current state*/
   result = ioctl(fd, IO_IOCTL_I2C_GET_STATE, &param);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == I2C_OK) && (param==I2C_STATE_READY), "Test IO_IOCTL_I2C_GET_STATE");
   param = I2C_MMA8451Q_BUS_ADDRESS;
   /*Set destination address */
   result = ioctl(fd, IO_IOCTL_I2C_SET_DESTINATION_ADDRESS, &param);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_OK, "Test IO_IOCTL_I2C_SET_DESTINATION_ADDRESS");
   param = 0x00;
   /* Get destination address */
   result = ioctl(fd, IO_IOCTL_I2C_GET_DESTINATION_ADDRESS, &param);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == I2C_OK) && (param == I2C_MMA8451Q_BUS_ADDRESS), "Test IO_IOCTL_I2C_GET_DESTINATION_ADDRESS");
   if (param != I2C_MMA8451Q_BUS_ADDRESS)
     {
       result = fclose (fd);
       _time_delay_ticks(4);
       eunit_test_stop();
       _mqx_exit(1);
     };
   /*Gets the actual bus state */
   result = ioctl (fd, IO_IOCTL_I2C_GET_BUS_AVAILABILITY , &param);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == I2C_OK) && (param==I2C_BUS_IDLE), "Test IO_IOCTL_I2C_GET_BUS_AVAILABILITY");
   result = ioctl (fd, IO_IOCTL_I2C_GET_STATISTICS, (void *)&stats);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == I2C_OK) , "Test IO_IOCTL_I2C_GET_STATISTICS");
   /* Initialize the MMA8451Q chip and select the mode */
   result=mma8451q_init_client(fd);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == I2C_OK), "Test mma8451q_init_client");
   for(c=0;c<10;c++)
   {
   printf("2G MODE:\n");
   report_abs(fd);
   };
}
/*TASK*-------------------------------------------------------------------
*
* Task Name : tc_2_main_task
* Comments  :
*Change the mma mode to 4g, output the data 10 times.(I2C polled-driven I/O)
* Requirement
*   I2C059
*END*----------------------------------------------------------------------*/
void tc_2_main_task()
{
  _mqx_int  result,c;
  mma_status.mode=MODE_4G;
  result=mma8451q_change_mode(fd,mma_status.mode);
  EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == I2C_OK, "Test: Set mode done ");
  for(c=0;c<10;c++)
  {
    printf("4G MODE:\n");
    report_abs(fd);
  };
}
/*TASK*-------------------------------------------------------------------
*
* Task Name : tc_3_main_task
* Comments  :
*Change the mma mode to 8g, output the data 10 times.(I2C polled-driven I/O)
* Requirement
*   I2C060
*END*----------------------------------------------------------------------*/
void tc_3_main_task()
{
  _mqx_int  result,c;
  mma_status.mode=MODE_8G;
  result=mma8451q_change_mode(fd,mma_status.mode);
  EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == I2C_OK, "Test: Set mode done ");
  for(c=0;c<10;c++)
  {
    printf("8G MODE:\n");
    report_abs(fd);
  };
  /* Close the driver */
  result = fclose (fd);
  EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == I2C_OK, "Test: I2C Close done");
}
//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_1)
EU_TEST_CASE_ADD(tc_1_main_task, " TEST1 - Test I2C Polled driver MMA 2g mode"),
EU_TEST_CASE_ADD(tc_2_main_task, " TEST2 - Test I2C Polled driver MMA 4g mode"),
EU_TEST_CASE_ADD(tc_3_main_task, " TEST3 - Test I2C Polled driver MMA 8g mode"),
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
void main_task( uint32_t dummy)
{
  eunit_mutex_init();
  EU_RUN_ALL_TESTS(eunit_tres_array);
  eunit_test_stop();
  _mqx_exit(0);
}/*EOF*/