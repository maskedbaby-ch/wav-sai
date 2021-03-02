/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
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
* $Date    : Jun-22-2012$
*
* Comments:
*
*   This include file is used to define the demonstration constants
*
*END************************************************************************/

/* General defines */
#include "util.h"

#define MAIN_TASK    10

#define  NONE        LINKER_USED_NONE
#define  INT_RAM     LINKER_USED_INTRAM
#define  INT_FLASH   LINKER_USED_INTFLASH
#define  EXT_RAM     LINKER_USED_EXTRAM
#define  EXT_FLASH   LINKER_USED_EXTFLASH
#define  DDR         LINKER_USED_DDR

typedef struct ram_size_struct{
	_mqx_uint	LINKER_FILE;
	_mqx_uint	SIZE;
} RAM_SIZE, * RAM_SIZE_PTR;


void main_task(uint32_t parameter);
_mqx_uint   get_expected_mem_size();
