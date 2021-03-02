#ifndef __fake_drv_h__
#define __fake_drv_h__
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
* $FileName: fake_drv.h$
* $Version : 4.0.2.0$
* $Date    : Jul-09-2013$
*
* Comments:
*
*   The file contains definitions, global variables and functions prototypes
*   of the Fake I/O drivers.
*
*END************************************************************************/

#define IO_IOCTL_CLEAR_BUFFER   (0xFFFF)
#define IO_IOCTL_RESET_HANDLER  (0xFFFE)
#define IO_IOCTL_GET_IN_BUFFER  (0xFFFD)
#define IO_IOCTL_GET_OUT_BUFFER (0xFFFC)

/* The initialization function of fake device */
_mqx_uint _io_fake_install(char *);

#endif
