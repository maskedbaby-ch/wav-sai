#ifndef __fake_drv_prv_h__
#define __fake_drv_prv_h__
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
* $FileName: fake_drv_prv.h$
* $Version : 4.0.2.0$
* $Date    : Jul-09-2013$
*
* Comments:
*
*   The file contains definitions, global variables and functions prototypes
*   of the Fake I/O drivers.
*
*END************************************************************************/

#define BUF_LEN (300)

_mqx_int _io_fake_open(MQX_FILE_PTR, char *, char *);
_mqx_int _io_fake_close(MQX_FILE_PTR);
_mqx_int _io_fake_read(MQX_FILE_PTR, char *, _mqx_int);
_mqx_int _io_fake_write( MQX_FILE_PTR, char *, _mqx_int);
_mqx_int _io_fake_ioctl(MQX_FILE_PTR, _mqx_uint, void *);

#endif
