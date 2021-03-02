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
* $Version : 3.8.4.0$
* $Date    : Sep-30-2011$
*
* Comments:
*
*   This file contains the source functions for testing the accuracy
*   ipsum function.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <pcb.h>
#include "util.h"
//#include <test_module.h>
#include "EUnit.h"
#include "eunit_crit_secs.h"

#define FILE_BASENAME test
#define MAIN_TASK      10

extern void main_task(uint32_t parameter);
extern int16_t my_rand(void);
extern void my_srand(uint16_t);
uint16_t IP_Sum(uint32_t sum, register _mem_size length, register unsigned char  *buf);
//------------------------------------------------------------------------
// Test suite function
void tc_1_main_task(void);//TEST # 1: Test _mem_sum_ip function 
void tc_2_main_task(void);//TEST # 2: Test zeros
void tc_3_main_task(void);//TEST # 3: Test ff
void tc_4_main_task(void);//TEST # 4: Test Special Pattern
//------------------------------------------------------------------------                          
TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
   { MAIN_TASK, main_task, 2000, 8, "Main", MQX_AUTO_START_TASK},
   { 0,         0,         0,    0, 0,      0                  }
};

static unsigned char t1[41] = {0x0,0x0,0x0,0x0,0x2,0x0,0x96,0x2,0x61,0x62,0x63,0x64,
0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,0x70,0x71,0x72,0x73,
0x74,0x75,0x76,0x77,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x68};
static uint16_t s1 = 0xaaa6;
static uint16_t i1 = 0x5559;

static unsigned char t2[41] = {0x0,0x0,0x0,0x0,0x2,0x0,0x97,0x2,0x61,0x62,0x63,0x64,
0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,0x70,0x71,0x72,0x73,
0x74,0x75,0x76,0x77,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x68};
static uint16_t s2 = 0xaba6;
static uint16_t i2 = 0x5459;

static unsigned char t3[41] = {0x0,0x0,0x0,0x0,0x2,0x0,0x98,0x2,0x61,0x62,0x63,0x64,
0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,0x70,0x71,0x72,0x73,
0x74,0x75,0x76,0x77,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x68};
static uint16_t s3 = 0xaca6;
static uint16_t i3 = 0x5359;

#define IP_Sum_invert(s)   (((s) == 0xFFFF) ? (s) : ~(s) & 0xFFFF)
#define TEST_SIZE  100 //1516
unsigned char tcpip_data[TEST_SIZE+4];
volatile uint16_t result1, result2;
uint16_t          chksum,n;
unsigned char            v[4];


//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_1_main_task
// Comments     : TEST # 1: Test _mem_sum_ip function 
//
//END---------------------------------------------------------------------
void tc_1_main_task(void)
{
    
   chksum = _mem_sum_ip(0, sizeof(t1), t1);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, chksum == s1,"Test #1 Testing: 1.1: Check chksum with s1")
 
   n = IP_Sum_invert(chksum);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, n == i1,"Test #1 Testing: 1.2: IP_Sum_invert() should return equal i1 value")
   
   mqx_htons(v, n);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (v[1] == (n&255) && v[0] == (n>>8)),"Test #1 Testing: 1.3:")
   
   chksum = _mem_sum_ip(0, sizeof(t2), t2);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, chksum == s2,"Test #1 Testing: 1.4: Check chksum with s2")
   
   n = IP_Sum_invert(chksum);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task,n == i2,"Test #1 Testing: 1.5: IP_Sum_invert() should return equal i2 value")
   
   mqx_htons(v, n);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (v[1] == (n&255) && v[0] == (n>>8)),"Test #1 Testing: 1.6: mqx_htons operation")
   
   chksum = _mem_sum_ip(0, sizeof(t3), t3);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task,chksum == s3,"Test #1 Testing: 1.7: Check chksum with s3")
   
   n = IP_Sum_invert(chksum);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, n == i3,"Test #1 Testing: 1.8: IP_Sum_invert() should return equal i3 value")
   
   mqx_htons(v, n);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (v[1] == (n&255) && v[0] == (n>>8)),"Test #1 Testing: 1.9: mqx_htons operation")
   
}
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_2_main_task
// Comments     : TEST # 2: Test zeros
//
//END---------------------------------------------------------------------
void tc_2_main_task(void)
{
   register uint32_t i;
   for (i = 0; i < sizeof(tcpip_data); ++i) {
      tcpip_data[i] = 0x00;
   } /* Endfor */

   result1 = IP_Sum(0, TEST_SIZE, tcpip_data);
   result2 = _mem_sum_ip(0, TEST_SIZE, tcpip_data);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, ((result1 == 0) && (result1 == result2)),"Test #2 Testing: 2.1: result should equal 0 and result1 should equal result2")
   
}
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_3_main_task
// Comments     : TEST # 3: Test ff
//
//END---------------------------------------------------------------------
void tc_3_main_task(void)
{
   register uint32_t i;  
   for (i = 0; i < sizeof(tcpip_data); ++i) {
      tcpip_data[i] = 0xff;
   } /* Endfor */

   result1 = IP_Sum(0, TEST_SIZE, tcpip_data);
   result2 = _mem_sum_ip(0, TEST_SIZE, tcpip_data);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, ((result1 == 0xFFFF) && (result1 == result2)),"Test #3 Testing: 3.1:result should equal 0xFFFF and result1 should equal result2")
   
}
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_4_main_task
// Comments     : TEST # 4: Test Special Pattern
//
//END---------------------------------------------------------------------
void tc_4_main_task(void)
{
   register uint32_t i, j, k;
   tcpip_data[0] = (char)(0x02);
   tcpip_data[1] = (char)(0x01);
   tcpip_data[2] = (char)(0x0E);
   tcpip_data[3] = (char)(0x01);
   tcpip_data[4] = (char)(0x01);
   tcpip_data[5] = (char)(0xC0);
   tcpip_data[6] = (char)(0x22);
   tcpip_data[7] = (char)(0x96);
   tcpip_data[8] = (char)(0xCA);
   tcpip_data[9] = (char)(0x00);
   tcpip_data[10] = (char)(0x6F);
   tcpip_data[11] = (char)(0x00);
   tcpip_data[12] = (char)(0x6F);
   tcpip_data[13] = (char)(0x04);
   tcpip_data[14] = (char)(0x02);
   tcpip_data[15] = (char)(0xE7);
   tcpip_data[16] = (char)(0x40);

   result1 = IP_Sum(0, 17, tcpip_data);
   result2 = _mem_sum_ip(0, 17, tcpip_data);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, result1 == result2,"Test #4 Testing: 4.1:result1 should equal result2")
   
   for (j = 0; j < 10; j++ ) {
      for (k = 0; k < TEST_SIZE; k++) {
         tcpip_data[k] = my_rand() & 0xFF;
      } /* Endfor */
      for (k = 0; k < TEST_SIZE; k++) {
         for (i = 0; i < 4; i++ ) {
            result1 = IP_Sum(0, (_mem_size)k, &tcpip_data[i]);
            result2 = _mem_sum_ip(0, (_mem_size)k, &tcpip_data[i]);
            EU_ASSERT_REF_TC_MSG(tc_4_main_task, result1 == result2,"Test #4 Testing: 4.2:result1 should equal result2")
            if (result1 != result2) {
               _task_block();
            } /* Endif*/
         } /* Endfor */
      } /* Endfor */
   } /* Endfor */
}

//------------------------------------------------------------------------
// Define Test Suite
 EU_TEST_SUITE_BEGIN(suite_1)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST # 1: Test _mem_sum_ip function"),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST # 2: Test zeros"),
    EU_TEST_CASE_ADD(tc_3_main_task, " TEST # 3: Test ff"),
    EU_TEST_CASE_ADD(tc_4_main_task, " TEST # 3: Test Special Pattern")
 EU_TEST_SUITE_END(suite_1)
// Add test suites
 EU_TEST_REGISTRY_BEGIN()
        EU_TEST_SUITE_ADD(suite_1, "Test of _mem_sum_ip")
 EU_TEST_REGISTRY_END()
//------------------------------------------------------------------------

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : IP_Sum
* Returned Value  : one's complement checksum
* Comments        :
*     Returns a one's complement checksum over a block of memory, as
*     used for packets in Internet protocols (see RFC791 for definition).
*
*     Note:  This function returns 0 iff all summands are 0.
*
*     Note:  It is impossible to overflow the 32-bit accumulator,
*            because we're summing length+1 words.  length is a 16-bit
*            value, and each word is also 16 bits.  Hence, maximum sum
*            is (2^16)(2^16-1) < 2^32.
*
*END*-----------------------------------------------------------------*/
uint16_t IP_Sum
   (
               uint32_t     sum,
      register _mem_size   length,
      register unsigned char  *buf  
   )
{ /* Body */
   register uint32_t total = sum & 0xffff;
   register uint32_t temp;

   if (length & 1) {
      length--;
      total += mqx_ntohc(buf+length) << 8;
   } /* Endif */

   length >>= 1;
   while (length--) {
      temp   = (uint16_t)mqx_ntohc(buf);
      total += temp<<8 | mqx_ntohc(buf+1);
      buf   += 2;
   } /* Endwhile */

   sum = ((total >> 16) & 0xFFFF);
   if (sum) {
      total = (total & 0xFFFF) + sum;
      sum = ((total >> 16) & 0xFFFF);
      if (sum) {
         total = (total & 0xFFFF) + sum;
      } /* Endif */
   } /* Endif */
   return (uint16_t)total;

} /* Endbody */

//TASK--------------------------------------------------------------------
// 
// Task Name    : main_task
// Comments     : 
//
//END---------------------------------------------------------------------
void main_task
   (
      uint32_t parameter
   )
{/* Body */
   register uint32_t i;
   TIME_STRUCT      start_time;
   TIME_STRUCT      end_time;
   
   _int_install_unexpected_isr();
   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   
   my_srand(_time_get_microseconds());

   /* Time assembler version of algorithm */
   _time_delay(10);
   _time_get(&start_time);
   for ( i = 0; i < 10; i ++ ) {//100000
      result1 = _mem_sum_ip(i, TEST_SIZE, tcpip_data);
   } /* Endfor */
   _time_get(&end_time);
   if ( end_time.MILLISECONDS < start_time.MILLISECONDS ) {
      end_time.MILLISECONDS += 1000;
      end_time.SECONDS--;
   } /* Endif */
   end_time.SECONDS      -= start_time.SECONDS;
   end_time.MILLISECONDS -= start_time.MILLISECONDS;
   
   /* Time C version of algorithm */
   _time_delay(10);
   _time_get(&start_time);
   for ( i = 0; i < 10000; i ++ ) {//100000
      result2 = IP_Sum(i,  TEST_SIZE, tcpip_data);
   } /* Endfor */
   _time_get(&end_time);
   if ( end_time.MILLISECONDS < start_time.MILLISECONDS ) {
      end_time.MILLISECONDS += 1000;
      end_time.SECONDS--;
   } /* Endif */
   end_time.SECONDS      -= start_time.SECONDS;
   end_time.MILLISECONDS -= start_time.MILLISECONDS;
   
   //test_stop();
   eunit_test_stop();

}/* Endbody */

/* EOF */
