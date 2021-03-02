/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
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
* $FileName: eeprom_int.c$
* $Version : 3.8.7.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains read/write functions to access I2C EEPROMs
*   using I2C interrupt driver.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <i2c.h>
#include "eeprom.h"


/*FUNCTION****************************************************************
* 
* Function Name    : i2c_write_eeprom_interrupt
* Returned Value   : void
* Comments         : 
*   Writes the provided data buffer at address in I2C EEPROMs
*
*END*********************************************************************/

bool i2c_write_eeprom_interrupt
   (
      /* [IN] The file pointer for the I2C channel */
      MQX_FILE_PTR fd,

      /* [IN] The address in EEPROM to write to */
      uint32_t    addr,

      /* [IN] The array of characters are to be written in EEPROM */
      unsigned char  *buffer,

      /* [IN] Number of bytes in that buffer */
      _mqx_int   n      
   )
{ /* Body */
	
   bool		isOK = TRUE;	
   uint32_t       param;
   _mqx_int    length;
   _mqx_int    result;
   uint8_t        mem[I2C_EEPROM_MEMORY_WIDTH];

   /* Protect I2C transaction in multitask environment */
//   _lwsem_wait (&lock);
   do
   {
      /* I2C bus address also contains memory block index */
      param = I2C_EEPROM_BUS_ADDRESS;
      if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_SET_DESTINATION_ADDRESS, &param))
      {
         return FALSE;
      }

      length = (_mqx_int)(I2C_EEPROM_PAGE_SIZE - (addr & (I2C_EEPROM_PAGE_SIZE - 1)));
      if (length > n) length = n;

      /* Initiate start and send I2C bus address */
      fwrite (mem, 1, 0, fd);

      /* Check ack (device exists) */
      if (I2C_OK == ioctl (fd, IO_IOCTL_FLUSH_OUTPUT, &param))
      {
         if (param) 
         {
            /* Stop I2C transfer */
            if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_STOP, NULL))
            {
            	isOK = FALSE;
            } 
            break;
         }
      } else {
    	  isOK = FALSE;
      }

      /* Write address within memory block */
#if I2C_EEPROM_MEMORY_WIDTH == 2
      mem[0] = (uint8_t)(addr >> 8);
      mem[1] = (uint8_t)addr;
      /* Write to address 0x%02x%02x ... ", mem[0], mem[1] */
      do 
      {
         result = fwrite (mem, 1, 2, fd);
      } while (result < 2);
      printf ("OK\n");
#else
      mem[0] = (uint8_t)addr;
      /*  Write to address 0x%02x ... ", mem[0] */
      do 
      {
         result = fwrite (mem, 1, 1, fd);
      } while (result < 1);
#endif

      /* Page write of data */
      result = 0;
      do
      {
         result += fwrite (buffer + result, 1, length - result, fd);
      } while (result < length);
      
      /* Wait for completion */
      result = fflush (fd);
      if (MQX_OK != result)
      {
         isOK = FALSE;
      } 

      /* Stop I2C transfer - initiate EEPROM write cycle */
      if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_STOP, NULL))
      {
    	  isOK = FALSE;
      } 

      /* Wait for EEPROM write cycle finish - acknowledge */
      result = 0;
      do 
      {  /* Sends just I2C bus address, returns acknowledge bit and stops */
         fwrite (&result, 1, 0, fd);
         
         if (I2C_OK != ioctl (fd, IO_IOCTL_FLUSH_OUTPUT, &param))
         {
        	 isOK = FALSE;
         }
         
         if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_STOP, NULL))
         {
        	 isOK = FALSE;
         }
         result++;
      } while ((param & 1) || (result <= 1));

      /* Next round */
      buffer += length;
      addr += length;
      n -= length;
        
   } while (n);

   /* Release the transaction lock */
//   _lwsem_post (&lock);
   return isOK;
} /* Endbody */

   
/*FUNCTION****************************************************************
* 
* Function Name    : i2c_read_eeprom_interrupt
* Returned Value   : void
* Comments         : 
*   Reads into the provided data buffer from address in I2C EEPROM
*
*END*********************************************************************/

bool i2c_read_eeprom_interrupt
   (
      /* [IN] The file pointer for the I2C channel */
      MQX_FILE_PTR fd,

      /* [IN] The address in EEPROM to read from */
      uint32_t    addr,

      /* [IN] The array of characters to be written into */
      unsigned char  *buffer,

      /* [IN] Number of bytes to read */
      _mqx_int   n      
   )
{ /* Body */
	bool		isOK = TRUE;
   _mqx_int    param;
   _mqx_int    result;
   uint8_t        mem[I2C_EEPROM_MEMORY_WIDTH];

   if (0 == n) 
   {
      return TRUE;
   }

   /* Protect I2C transaction in multitask environment */
//   _lwsem_wait (&lock);
   
   /* I2C bus address also contains memory block index */
   param = I2C_EEPROM_BUS_ADDRESS;
   if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_SET_DESTINATION_ADDRESS, &param))
   {
	   isOK = FALSE;
   } 

   /* Initiate start and send I2C bus address */
   fwrite (mem, 1, 0, fd);

   /* Check ack (device exists) */
   if (I2C_OK == ioctl (fd, IO_IOCTL_FLUSH_OUTPUT, &param))
   {
      if (param) 
      {
         /* Stop I2C transfer */
         if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_STOP, NULL))
         {
        	 isOK = FALSE;
         } 
         
         /* Release the transaction lock */
         //_lwsem_post (&lock);
         
         return TRUE;
      }
   } else {
	   isOK = FALSE;
   }

   /* Write address within memory block */
#if I2C_EEPROM_MEMORY_WIDTH == 2
      mem[0] = (uint8_t)(addr >> 8);
      mem[1] = (uint8_t)addr;
      /*  Write to address 0x%02x%02x ... ", mem[0], mem[1]) */
      do 
      {
         result = fwrite (mem, 1, 2, fd);
      } while (result < 2);

#else
      mem[0] = (uint8_t)addr;
      /* Write to address 0x%02x ... ", mem[0]) */
      do 
      {
         result = fwrite (mem, 1, 1, fd);
      } while (result < 1);
#endif

   /* Wait for completion */
   result = fflush (fd);
   if (MQX_OK != result)
   {
	   isOK = FALSE;
   }

   /* Restart I2C transfer for reading */
   if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_REPEATED_START, NULL))
   {
	   isOK = FALSE;
   } 
   /* Set read request */
   param = n;
   if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_SET_RX_REQUEST, &param))
   {
	   isOK = FALSE;
   } 

   /* Read all data */
   result = 0;
   do
   {
      result += fread (buffer + result, 1, n - result, fd);
   } while (result < n);
      
   /* Stop I2C transfer - initiate EEPROM write cycle */
   if (I2C_OK == ioctl (fd, IO_IOCTL_I2C_STOP, NULL))
   {
	   isOK = FALSE;
   } 
   
   /* Release the transaction lock */
//   _lwsem_post (&lock);
   return isOK;
} /* Endbody */

