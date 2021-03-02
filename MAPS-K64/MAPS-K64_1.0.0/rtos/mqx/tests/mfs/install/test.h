/**HEADER********************************************************************
* 
* Copyright (c) 2008-2013 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International;
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
* $FileName: test.h$
* $Version : 3.8.3.0$
* $Date    : Sep-22-2011$
*
* Comments:
*
*   This include file is used to define the demonstation constants
*
*END************************************************************************/

#define MAIN_TASK       10

/* 
** If this define is uncommented the mem driver
** will use block mode - 2.50 only
*/
/* #define USE_BLOCK_MODE */

#define EXTRA_PARTS      3

#define MFS_CYL          738
#define PHYSICAL_DRI     0x80
#define MEDIA_DESC       0xF8
#define BYTES_PER_SECT   512
#define SECTS_PER_TRACK  32
#define NUM_OF_HEADS     4
#define NUM_SECTORS      25
#define HIDDEN_SECTS     0
#define RESERVED_SECTS   1

#ifdef USE_BLOCK_MODE
#define MEM_OPEN_MODE    "b"
#else
#define MEM_OPEN_MODE    0
#endif

extern void *usb_dev_handle;

#ifdef __cplusplus
extern "C" {
#endif
 
extern void    main_task(uint32_t param);
extern uint32_t _ftest_count_parts(void);

#ifdef __cplusplus
}
#endif
 
/* EOF */

