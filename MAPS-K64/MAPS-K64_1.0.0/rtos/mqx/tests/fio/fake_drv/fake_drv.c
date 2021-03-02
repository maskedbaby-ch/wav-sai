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
* $FileName: fake_drv.c$
* $Version : 4.0.2.0$
* $Date    : Jul-09-2013$
*
* Comments:
*
*   This file contains the Fake I/O driver functions which are used to test
*   FIO module's functions without using any real peripheral driver.
*
*END************************************************************************/

#include "mqx.h"
#include "fio.h"
#include "fio_prv.h"
#include "io.h"
#include "io_prv.h"
#include "fake_drv.h"
#include "fake_drv_prv.h"

/* Input buffer handler of fake device */
static char *fake_in;
/* Output buffer handler of fake device */
static char *fake_out;
/* Input Buffer memory */
static void *buf_in;
/* Output Buffer memory */
static void *buf_out;

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_fake_install
* Returned Value   : _mqx_uint (Error code or MQX_OK)
* Comments         :
*   Install a Fake I/O device.
*   - [IN] A string that identifies the device for fopen
*
*END*----------------------------------------------------------------------*/
_mqx_uint _io_fake_install(char *identifier)
{
    _mqx_uint result;

    result = _io_dev_install(identifier,
        _io_fake_open,
        _io_fake_close,
        _io_fake_read,
        _io_fake_write,
        _io_fake_ioctl,
        NULL);

    return result;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_fake_open
* Returned Value   : _mqx_int (Error code or MQX_OK)
* Comments         :
*   Opens and initializes Fake I/O device.
*   - [IN] fd_ptr: The file handle for the device being opened
*   - [IN] open_name_ptr: The remaining portion of the name of the device
*   - [IN] flags: The flags to be used during operation
*
*END*----------------------------------------------------------------------*/
_mqx_int _io_fake_open(MQX_FILE_PTR fd_ptr, char *open_name_ptr, char *flags)
{
    /* Allocate memory for buffers */
    buf_in = _mem_alloc(BUF_LEN);
    buf_out = _mem_alloc(BUF_LEN);

    /* Verify allocation */
    if((buf_in != NULL) && (buf_out != NULL))
    {
        /* Initialize buffer handler pointers */
        fake_in = (char *)buf_in;
        fake_out = (char *)buf_out;
        /* Success */
        return (MQX_OK);
    }
    else
    {
        /* Error occurs, return negative value */
        return (IO_ERROR);
    }
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_fake_close
* Returned Value   : _mqx_int (Error code or MQX_OK)
* Comments         :
*   Closes Fake I/O device
*   - [IN] fd_ptr: The file handle for the device being closed
*
*END*----------------------------------------------------------------------*/
_mqx_int _io_fake_close(MQX_FILE_PTR fd_ptr)
{
    _mqx_uint result;

    /* Free the allocated buffer memory */
    result = _mem_free(buf_in);
    result |= _mem_free(buf_out);

    /* Verify deallocation */
    if(result == MQX_OK)
    {
        buf_in = NULL;
        buf_out = NULL;
        /* Success */
        return (MQX_OK);
    }
    else
    {
        /* Error occurs, return negative value */
        return (IO_ERROR);
    }
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_fake_read
* Returned Value   : _mqx_int (Number of characters read)
* Comments         :
*   Reads data from Fake I/O device
*   - [IN] fd_ptr: The file handle for the device
*   - [IN] data_ptr: Where the characters are to be stored
*   - [IN] num: The number of characters to input
*
*END*----------------------------------------------------------------------*/
_mqx_int _io_fake_read(MQX_FILE_PTR fd_ptr, char *data_ptr, _mqx_int num)
{
    _mqx_int i = num;

    /* Check if the number of characters is bigger than buffer size */
    if(i>=BUF_LEN)
    {
        return (IO_ERROR);
    }

    /* Read character until reach the expected number */
    while(i)
    {
        *data_ptr++ = *fake_in++;
        i--;
    }
    /* Return the actual number of characters read */
    return (num);
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_fake_write
* Returned Value   : _mqx_int (Number of characters written)
* Comments         :
*   Writes data to the fdv_ram device
*   - [IN] fd_ptr: The file handle for the device
*   - [IN] data_ptr: Where the characters are
*   - [IN] num: The number of characters to output
*
*END*----------------------------------------------------------------------*/
_mqx_int _io_fake_write( MQX_FILE_PTR fd_ptr, char *data_ptr, _mqx_int num)
{
    _mqx_int i = num;

    /* Check if the number of characters is bigger than buffer size */
    if(i>=BUF_LEN)
    {
        return (IO_ERROR);
    }

    /* Write character until reach the expected number */
    while(i)
    {
        *fake_out++ = *data_ptr++;
        i--;
    }
    /* Return the actual number of characters written */
    return (num);
}


/*FUNCTION*****************************************************************
*
* Function Name    : _io_fake_ioctl
* Returned Value   : _mqx_int
* Comments         :
*   Returns result of ioctl operation.
*   - [IN] fd_ptr: The file handle for the device
*   - [IN] cmd: The ioctl command
*   - [IN] param_ptr: The ioctl parameters
*
*END*********************************************************************/
_mqx_int _io_fake_ioctl(MQX_FILE_PTR fd_ptr, _mqx_uint cmd, void *param_ptr)
{
    switch(cmd)
    {
        /* Clear the I/O buffer */
        case IO_IOCTL_CLEAR_BUFFER:
        {
            _mqx_uint i;
            /* Check if device has already opened */
            if((buf_in == NULL) || (buf_out == NULL))
            {
                return IO_ERROR;
            }
            /* Reset buffer handler */
            fake_in = (char *)buf_in;
            fake_out = (char *)buf_out;
            /* Clear buffer */
            for(i = 0; i < BUF_LEN; i++)
            {
                fake_in[i] = 0;
                fake_out[i] = 0;
            }
            return MQX_OK;
        }

        /* Reset buffer handler for new R/W operation */
        case IO_IOCTL_RESET_HANDLER:
        {
            /* Check if device has already opened */
            if((buf_in == NULL) || (buf_out == NULL))
            {
                return IO_ERROR;
            }
            /* Reset buffer handler */
            fake_in = (char *)buf_in;
            fake_out = (char *)buf_out;
            fseek(fd_ptr, 0, IO_SEEK_SET);
            return MQX_OK;
        }

        /* Get address of the Input buffer */
        case IO_IOCTL_GET_IN_BUFFER:
        {
            char **buf_ptr = (char **)param_ptr;
            /* Check if device has been opened */
            if(buf_in == NULL)
            {
                return IO_ERROR;
            }
            /* Return input pointer */
            *buf_ptr = (char *)buf_in;
            return MQX_OK;
        }

        /* Get address of the Output buffer */
        case IO_IOCTL_GET_OUT_BUFFER:
        {
            char **buf_ptr = (char **)param_ptr;
            /* Check if device has been opened */
            if(buf_out == NULL)
            {
                return IO_ERROR;
            }
            /* Return output pointer */
            *buf_ptr = (char *)buf_out;
            return MQX_OK;
        }

        /* Used to test success case */
        case IO_IOCTL_CHAR_AVAIL:
        {
            bool *res = (bool *)param_ptr;
            *res = TRUE;
            return MQX_OK;
        }

        /* Used to test failure cases */
        case IO_IOCTL_SEEK:
        case IO_IOCTL_FLUSH_OUTPUT:
        {
            return IO_ERROR;
        }

        /* I/O Control Command is not supported */
        default:
        {
            return IO_ERROR_INVALID_IOCTL_CMD;
        }
    }
}
