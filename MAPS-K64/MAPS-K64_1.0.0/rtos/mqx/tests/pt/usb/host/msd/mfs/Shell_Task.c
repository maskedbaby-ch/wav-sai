/**HEADER********************************************************************
*
* Copyright (c) 2008 - 2013 Freescale Semiconductor;
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
*FileName: Shell_Task.c$
*Version : 4.0.2$
*Date    : Jul-29-2013$
*
* Comments:
*
*
*
*END************************************************************************/



#include "MFS_USB.h"

const SHELL_COMMAND_STRUCT Shell_commands[] = {
   { "cd",        Shell_cd },
   { "copy",      Shell_copy },
   { "del",       Shell_del },
   { "disect",    Shell_disect},
   { "dir",       Shell_dir },
   { "exit",      Shell_exit },
   { "format",    Shell_format },
   { "help",      Shell_help },
   { "mkdir",     Shell_mkdir },
   { "pwd",       Shell_pwd },
   { "read",      Shell_read },
   { "ren",       Shell_rename },
   { "rmdir",     Shell_rmdir },
   { "sh",        Shell_sh },
   { "type",      Shell_type },
   { "write",     Shell_write },
   { "compare",   Shell_compare },
   { "readtest",  Shell_read_test },
   { "writetest", Shell_write_test },
   { "df",        Shell_df },
   {"create",     Shell_create},
   {"rmdir",      Shell_rmdir},
   { "?",         Shell_command_list },

   { NULL,        NULL }
};


/*TASK*-----------------------------------------------------------------
*
* Function Name  : Shell_Task
* Returned Value : void
* Comments       :
*
*END------------------------------------------------------------------*/

void Shell_Task(uint32_t temp)
{

   /* Run the shell on the serial port */
   for(;;)  {
      Shell(Shell_commands, NULL);
      printf("Shell exited, restarting...\n");
   }
}

/* EOF */
