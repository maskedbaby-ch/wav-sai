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
* $FileName: test.c$
* $Version : 3.7.3.0$
* $Date    : Mar-24-2011$
*
* Comments:
*
*   This file contains code for the MQX message verification
*   program.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <fio.h>
#include <message.h>
#include <util.h>
//#include <test_module.h>
#include "EUnit.h"
#include "eunit_crit_secs.h"

#define FILE_BASENAME   test
/* Task IDs */
#define MESSAGE_TASK 5

extern void message_task(uint32_t);
//------------------------------------------------------------------------
// Test suite function prototype
extern void tc_1_main_task(void);//Allocate a message from the private message pool
//------------------------------------------------------------------------
TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{ 
    {MESSAGE_TASK, message_task, 1500, 8, "message", MQX_AUTO_START_TASK},
    {0, 0, 0, 0, 0, 0 }
};


typedef struct server_message
{
   MESSAGE_HEADER_STRUCT   HEADER;
   unsigned char                   DATA[5];
} SERVER_MESSAGE, * SERVER_MESSAGE_PTR;


//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_1_main_task
// Comments     : TEST #1: Allocate a message from the private message pool
//
//END---------------------------------------------------------------------

void tc_1_main_task(void)
{
    _pool_id pool1;
   int i;
   pool1 = _msgpool_create(sizeof(SERVER_MESSAGE),5,5,0);//Create a private message pool
   for (i = 0; i < 20; i++) {
      void   *msg = _msg_alloc( pool1 );
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, msg != NULL, "Allocate a message from the private message pool")
      if (msg == NULL) {
        break;
      } /* Endif */
   }        
}
/*END----------------------------------------------------*/

//------------------------------------------------------------------------
// Define Test Suite
 EU_TEST_SUITE_BEGIN(suite_3)
    EU_TEST_CASE_ADD(tc_1_main_task, " Test #1 - Testing message")
 EU_TEST_SUITE_END(suite_3)
// Add test suites
 EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_3, " - Testing unlimited growth message pool")
 EU_TEST_REGISTRY_END()
//------------------------------------------------------------------------
 
/*TASK*-----------------------------------------------------
* 
* Task Name    : message_task
* Comments     :
*
*END*-----------------------------------------------------*/
void message_task
   (
      uint32_t initial_data
   )
{
   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();
}
/* EOF */
