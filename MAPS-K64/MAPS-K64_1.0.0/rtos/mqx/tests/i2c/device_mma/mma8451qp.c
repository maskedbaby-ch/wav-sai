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
* $FileName: mma8451qp.c$
* $Version :
* $Date    : Aug-5-2013$
*
* Comments:
*
*   This file contains read/write functions to access I2C MMA8451Q
*   using I2C polled driver.
*
*END************************************************************************/
#include <mqx.h>
#include <bsp.h>
#include <i2c.h>
#include "mma8451q.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"
extern void tc_1_main_task(uint32_t);
 /*FUNCTION****************************************************************
*
* Function Name    : mma8451q_init_client
* Returned Value   : result
* Comments         :
*   Set the 2g/4g/8g mode, fast read bit clear, active mode on.
*
*END*********************************************************************/
int mma8451q_init_client
(
 /* [IN] The file pointer for the I2C channel */
    MQX_FILE_PTR fd
)
{
  int result;
  unsigned char x=0;
  unsigned char *buffer;
  buffer = &x;
  mma_status.mode = MODE_2G;
  result = i2c_write_MMA8451Q_polled(fd, MMA8451Q_XYZ_DATA_CFG, &mma_status.mode,1);
  EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_OK, "Set the mode :2G");
  EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_OK, "Test i2c_write_MMA8451Q_polled");
  mma_status.ctl_reg2=0x01;
  result =i2c_write_MMA8451Q_polled(fd, MMA8451Q_CTRL_REG1,&mma_status.ctl_reg2,1);
  EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_OK, "Fast read clear and active mode on");
  result =i2c_read_MMA8451Q_polled(fd, MMA8451Q_WHO_AM_I, buffer,1);
  EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == I2C_OK) && ((*buffer)==MMA8451Q_ID), "Test WHO_AM_I check");
  EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == I2C_OK) && ((*buffer)==MMA8451Q_ID), "Test i2c_read_MMA8451Q_byte");
  return result;
}

 /*FUNCTION****************************************************************
*
* Function Name    : mma8451q_read_data
* Returned Value   : result
* Comments         :
*   Read sensor data from mma8451q
*
*END*********************************************************************/
int mma8451q_read_data(MQX_FILE_PTR fd,short *x, short *y, short *z)
{
  uint8_t tmp_data[7];
  _mqx_int    result;
  result=i2c_read_MMA8451Q_polled(fd,MMA8451Q_OUT_X_MSB,tmp_data,7);
  EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_OK, "Test: Read 14-bit data x,y,z");
  if (result)
    {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
    };
  *x = ((tmp_data[0] << 8) & 0xff00) | tmp_data[1];
  *y = ((tmp_data[2] << 8) & 0xff00) | tmp_data[3];
  *z = ((tmp_data[4] << 8) & 0xff00) | tmp_data[5];
  *x = (short)(*x) >> 2;
  *y = (short)(*y) >> 2;
  *z = (short)(*z) >> 2;
  if (mma_status.mode == MODE_4G)
  {
    (*x) = (*x) << 1;
    (*y) = (*y) << 1;
    (*z) = (*z) << 1;
  }
  else if (mma_status.mode == MODE_8G)
  {
    (*x) = (*x) << 2;
    (*y) = (*y) << 2;
    (*z) = (*z) << 2;
  }
  return 0;
}
 /*FUNCTION****************************************************************
*
* Function Name    : report_abs
* Returned Value   :
* Comments         :
*   Change the current mode.
*
*END*********************************************************************/
int mma8451q_change_mode(MQX_FILE_PTR fd, int mode)
{
  int result;
  mma_status.mode = mode;
  result = i2c_write_MMA8451Q_polled(fd,MMA8451Q_XYZ_DATA_CFG,&mma_status.mode,1);
  return result;
}
 /*FUNCTION****************************************************************
*
* Function Name    : report_abs
* Returned Value   :
* Comments         :
*   Check the new data is ready and outout the result.
*
*END*********************************************************************/
void report_abs(MQX_FILE_PTR fd)
{
  short x,y,z;
  unsigned char *buffer;
  unsigned char a=0;
  buffer =&a;
  _mqx_int    result;
  result=mma8451q_read_data(fd,&x, &y, &z);
  EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_OK, "Test: Read data done ");
  _time_delay_ticks(4);
  result=i2c_read_MMA8451Q_polled(fd,MMA8451Q_STATUS,buffer,1);
  EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == I2C_OK) && (*buffer & 0x08), "Test: Check if the new data ready");
  if (!(*buffer & 0x08))
  {
    _time_delay_ticks(4);
    eunit_test_stop();
    _mqx_exit(1);
  };
  printf("\n output x:%d   y:%d   z:%d\n",x,y,z);
}
/*FUNCTION****************************************************************
*
* Function Name    : i2c_write_MMA8451Q_polled
* Returned Value   : void
* Comments         :
*   Writes the data buffer at address in  MMA8451Q
*
*END*********************************************************************/
int i2c_write_MMA8451Q_polled
   (
      /* [IN] The file pointer for the I2C channel */
      MQX_FILE_PTR fd,

      /* [IN] The address in MMA8451Q to write to */
      uint8_t    addr,

      /* [IN] The array of characters are to be written in MMA8451Q */
      unsigned char *  buffer,

      /* [IN] Number of bytes in that buffer */
      _mqx_int   n
   )
{ /* Body */
  uint32_t       param;
  _mqx_int    length;
  _mqx_int    result;
  uint8_t        mem[2];
  /* I2C bus address also contains memory block index */
  param = I2C_MMA8451Q_BUS_ADDRESS;
  /*Set destination address */
  result = ioctl(fd, IO_IOCTL_I2C_SET_DESTINATION_ADDRESS, &param);
  EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_OK, "I2C Set destination address");
  fwrite (mem, 1, 0, fd);
  /* Write register address*/
  mem[0] = (uint8_t)addr;
  result = fwrite (mem, 1, 1, fd);
  EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == 1 , "Test _io_write()");
  length = n;
  /* Write of data */
  result = fwrite (buffer, 1, length, fd);
  EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == length, "Test write data...");
  /* Wait for completion */
  result = fflush (fd);
  EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK , "Test fflush ...");
  /* Stop I2C transfer - initiate MMA8451Q write cycle */
  result = ioctl (fd, IO_IOCTL_I2C_STOP, NULL);
  EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_OK, "Test IO_IOCTL_I2C_STOP");
  return 0;
} /* Endbody */
/*FUNCTION****************************************************************
*
* Function Name    : i2c_read_MMA8451Q_polled
* Returned Value   : void
* Comments         :
*   Reads the data buffer from address in  MMA8451Q
*Requirement
* I2C007,I2C008,I2C009,I2C010,I2C029,I2C030,I2C031,I2C033,I2C037
*END*********************************************************************/
int i2c_read_MMA8451Q_polled
   (
      /* [IN] The file pointer for the I2C channel */
      MQX_FILE_PTR fd,

      /* [IN] The address in MMA8451Q to read from */
      uint8_t    addr,

      /* [IN] The array of characters to be written into */
      unsigned char *  buffer,

      /* [IN] Number of bytes to read */
      _mqx_int   n
   )
{ /* Body */
   _mqx_int    param;
   _mqx_int    result;
   uint8_t     mem[2];
   MQX_FILE_PTR fd_invalid,param_invalid;
   fd_invalid=fopen (0, NULL);
   if (0 == n)
     {
       return 0;
     }
   /* I2C bus address also contains memory block index */
   param = I2C_MMA8451Q_BUS_ADDRESS;
   result = ioctl(fd, IO_IOCTL_I2C_SET_DESTINATION_ADDRESS, &param);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_OK, "I2C Set destination address");
   fwrite (mem, 1, 0, fd);
   /* Write register address . Requirement I2C009*/
   mem[0] = (uint8_t)addr;
   result = fwrite (mem, 1, 1, fd);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == 1 , "Test _io_write()");
   /* Write register address fail. Requirement I2C010*/
   mem[0] = (uint8_t)addr;
   result = fwrite (mem, 1, 1, fd_invalid);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == IO_ERROR , "Test _io_write() failed");
   /* Wait for completion */
   result = fflush (fd);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK , "Test fflush ...");
   /*Get current state . Requirement I2C037*/
   result = ioctl(fd, IO_IOCTL_I2C_GET_STATE, &param);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK) && (param==I2C_STATE_TRANSMIT ), "Test I2C_STATE_TRANSMIT state ");
   /* Restart I2C transfer for reading . Requirement I2C031*/
   result = ioctl (fd, IO_IOCTL_I2C_REPEATED_START, NULL);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK , "Test IO_IOCTL_I2C_REPEATED_START");
   /* Set read request . Requirement I2C029*/
   param = n;
   result = ioctl (fd, IO_IOCTL_I2C_SET_RX_REQUEST, &param);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK , "Test IO_IOCTL_I2C_SET_RX_REQUEST");
   /* Set read request fail . Requirement I2C030*/
   param_invalid=NULL;
   result = ioctl (fd, IO_IOCTL_I2C_SET_RX_REQUEST, param_invalid);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_ERROR_INVALID_PARAMETER , "Test IO_IOCTL_I2C_SET_RX_REQUEST failed ");
   /* Read all data . Requirement I2C007*/
   result =  fread (buffer, 1, n, fd);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == n , "Test _io_read()");
   /* Read all data fail . Requirement I2C008*/
   result =  fread (buffer, 1, n, fd_invalid);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == IO_ERROR , "Test _io_read() failed");
   /* Stop I2C transfer - initiate MMA8451Q write cycle . Requirement I2C033*/
   result = ioctl (fd, IO_IOCTL_I2C_STOP, NULL);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_OK, "Test IO_IOCTL_I2C_STOP");
   return 0;
} /* Endbody */
/*FUNCTION****************************************************************
*
* Function Name    : i2c_read_MMA8451Q_byte
* Returned Value   : void
* Comments         :
*   Reads the data buffer from address by byte, not supported in MMA8451Q
*
*END*********************************************************************/
int i2c_read_MMA8451Q_byte
   (
      /* [IN] The file pointer for the I2C channel */
      MQX_FILE_PTR fd,

      /* [IN] The address in MMA8451Q to read from */
      uint8_t    addr,

      /* [IN] The array of characters to be written into */
      unsigned char *  buffer
   )
{ /* Body */
   _mqx_int    param;
   _mqx_int    result;
   uint8_t     mem[2];
   /* I2C bus address also contains memory block index */
   param = I2C_MMA8451Q_BUS_ADDRESS;
   result = ioctl(fd, IO_IOCTL_I2C_SET_DESTINATION_ADDRESS, &param);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_OK, "I2C Set destination address");
   fwrite(mem, 1, 0, fd);
   /* Write register address */
   mem[0] = (uint8_t)addr;
   result = fwrite (mem, 1, 1, fd);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == 1 , "Test _io_write()");
   /* Wait for completion */
   result = fflush (fd);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK , "Test fflush ...");
   /* Stop I2C transfer - initiate MMA8451Q write cycle */
   result = ioctl (fd, IO_IOCTL_I2C_STOP, NULL);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_OK, "Test IO_IOCTL_I2C_STOP");
   fread(mem, 1, 0, fd);
   /* Read all data */
   result =  fread (buffer, 1,1, fd);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result ==1 , "Test _io_read()");
   /* Stop I2C transfer - initiate MMA8451Q write cycle */
   result = ioctl (fd, IO_IOCTL_I2C_STOP, NULL);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == I2C_OK, "Test IO_IOCTL_I2C_STOP");
   return 0;
} /* Endbody */
