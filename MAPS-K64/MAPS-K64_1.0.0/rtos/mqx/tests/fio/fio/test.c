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
* $Version : 3.8.13.0$
* $Date    : Sep-4-2012$
*
* Comments:
*
*   This file contains the source functions for the test of standard
*   I/O library.
*
*END************************************************************************/

#include <string.h>
#include <mqx.h>
#include <bsp.h>
#include <util.h>
#include "test.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"

#if !MQX_USE_IO_OLD
#include <stdio.h>
#define IO_EOF EOF
#endif

#define FILE_BASENAME   test
#define TEST_PRINTING_FLOAT   1   /* Enable floating test */
#define MAIN_TASK      10

void main_task(uint32_t);
//------------------------------------------------------------------------
// Test suite function prototype
void tc_1_main_task(void);
void tc_2_main_task(void);

TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   { MAIN_TASK,      main_task,      8000,  8, "Main",      MQX_AUTO_START_TASK},
   { 0,              0,              0,     0, 0,           0 }
};

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : string_compare
* Returned Value   : TRUE   - If strings are the same
                   : FALSE  - Other
* Comments         :
*       This function compare 2 strings.
*
*END*----------------------------------------------------------------------*/
static bool string_compare(const char *p1, const char *p2)
{
    if((p1 == NULL) || (p2 == NULL))
    {
        return FALSE;
    }
    for (; *p1 == *p2; p1++, p2++)
    {
        if (*p1 == '\0')
        {
            return TRUE;
        }
    }
    return FALSE;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : Testit
* Returned Value   : TRUE   -
                   : FALSE  -
* Comments         :
*
*END*----------------------------------------------------------------------*/
bool Testit
   (
      char     *line,
      char     *fmt
   )
{
   static int index = 16;

   _mqx_uint a1 = 0, a2 = 0, a3 = 0;
   _mqx_int rslt;

   rslt = sscanf(line, fmt, &a1, &a2, &a3 );

   return (rslt >= 0) && (a1 == compare2[index++]) && (a2 == compare2[index++]) && (a3 == compare2[index++]);
}

#define I (_mqx_uint)(555)
#define R (double)(5.5)

/*TASK*-------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : Test printing with varied prefix
*
*END*----------------------------------------------------------------------*/

void tc_1_main_task(void)
{
   int16_t       i, j, k, l, index = 0;
   char         tp[32];
   char         strout[32];
   char         buf[32];
   char        *prefix = buf;

   strcpy(prefix,"%");
   for ( i = 0; i < 2; ++i )
   {
      for ( j = 0; j < 2; ++j )
      {
         for ( k = 0; k < 2; ++k )
         {
            for ( l = 0; l < 2; ++l )
            {
               if ( i == 0 ) {
                  strcat(prefix,"-");
               }
               if ( j == 0 ) {
                  strcat(prefix,"+");
               }
               if ( k == 0 ) {
                  strcat(prefix,"#");
               }
               if ( l == 0 ) {
                  strcat(prefix,"0");
               }
               sprintf(strout, "%5s |",prefix);
               EU_ASSERT_REF_TC_MSG(tc_1_main_task, string_compare(strout, compare1[index++]), "#1.1 Test printing prefix");

               strcpy(tp,prefix);
               strcat(tp,"6d |");
               sprintf(strout, tp,I);
               EU_ASSERT_REF_TC_MSG(tc_1_main_task, string_compare(strout, compare1[index++]), "#1.2 Test printing decimal integer");

               strcpy(tp,prefix);
               strcat(tp,"6o |");
               sprintf(strout,tp,I);
               EU_ASSERT_REF_TC_MSG(tc_1_main_task, string_compare(strout, compare1[index++]), "#1.3 Test printing octal");

               strcpy(tp,prefix);
               strcat(tp,"8x |");
               sprintf(strout,tp,I);
               EU_ASSERT_REF_TC_MSG(tc_1_main_task, string_compare(strout, compare1[index++]), "#1.4 Test printing hexadecimal integer");

#if TEST_PRINTING_FLOAT
               strcpy(tp,prefix);
               strcat(tp,"10.2e |");
               sprintf(strout,tp,R);
               EU_ASSERT_REF_TC_MSG(tc_1_main_task, string_compare(strout, compare1[index++]), "#1.5 Test printing using e character");

               strcpy(tp,prefix);
               strcat(tp,"10.2f |");
               sprintf(strout,tp,R);
               EU_ASSERT_REF_TC_MSG(tc_1_main_task, string_compare(strout, compare1[index++]), "#1.6 Test printing decimal floating point");
#else
               index += 2;
#endif

               strcpy(prefix,"%");
            }
         }
      }
   }
}

/*TASK*-------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : Test scaning with varied format
*
*END*----------------------------------------------------------------------*/

void tc_2_main_task(void)
{
   static char fr_time[20] = "123a45b67c89d54321";
   _mqx_int rslt;
   _mqx_uint days_100, days_10, days_1;
   _mqx_uint hrs_10, hrs_1;
   _mqx_uint min_10, min_1;
   _mqx_uint sec_10, sec_1;
   _mqx_uint msec_100, msec_10, msec_1;
   char  d1, d2, d3 ,d4;
   _mqx_uint i;
   char      *c_ptr;
   bool result;

   /* Date scanning tests */

   rslt = sscanf( fr_time, "%1u%1u%1u%c%1u%1u%c%1u%1u%c%1u%1u%c%1u%1u%3d",
     &days_100, &days_10,  &days_1, &d1,
     &hrs_10,   &hrs_1,    &d2,
     &min_10,   &min_1,    &d3,
     &sec_10,   &sec_1,    &d4,
     &msec_100, &msec_10,  &msec_1 );

   result = (rslt > 0)
           && (days_100 == compare2[0]) && (days_10 == compare2[1]) && (days_1 == compare2[2]) && (d1 == compare2[3])
           && (hrs_10 == compare2[4]) && (hrs_1 == compare2[5]) && (d2 == compare2[6])
           && (min_10 == compare2[7]) && (min_1 == compare2[8]) && (d3 == compare2[9])
           && (sec_10 == compare2[10]) && (sec_1 == compare2[11]) && (d4 == compare2[12])
           && (msec_100 == compare2[13]) && (msec_10 == compare2[14]) && (msec_1 == compare2[15]);

   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result, "Test #2.1 Test date scanning")

   result = Testit(" 123.456.789", "%u.%u.%u");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result, "Test #2.2 Test scaning with assignment suppression")

   result = Testit(" 123.456.789", "%*u.%u.%u");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result, "Test #2.3 Test scaning with assignment suppression")

   result = Testit(" 123.456.789", "%u.%*u.%u");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result, "Test #2.4 Test scaning with assignment suppression")

   result = Testit(" 123.456.789", "%*u.%*u.%u");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result, "Test #2.5 Test scaning with assignment suppression")

   result = Testit(" 123.456.789", "%u.%u.%*u");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result, "Test #2.6 Test scaning with assignment suppression")

   result = Testit(" 123.456.789", "%*u.%u.%*u");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result, "Test #2.7 Test scaning with assignment suppression")

   result = Testit(" 123.456.789", "%u.%*u.%*u");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result, "Test #2.8 Test scaning with assignment suppression")

   result = Testit(" 123.456.789", "%*u.%*u.%*u");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result, "Test #2.9 Test scaning with assignment suppression")

   result = Testit(" 123.456.789", "%u%*c%u%*c%u");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result, "Test #2.10 Test scaning with assignment suppression")

   /* hex tests */
   result = Testit("ff04f86af3ecfbfbffffffffffffffbf", "%02x%02x%02x");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result, "Test #2.11 Test hexadecimal scaning")

   c_ptr = "0102030405060708090a0b0c0d0e0f101112";

   for (i = 0; i < 6; i++)
   {
       result = Testit(c_ptr, "%02x%02x%02x");
       EU_ASSERT_REF_TC_MSG(tc_2_main_task, result, "Test #2.12 Test hexadecimal scaning")
       c_ptr += 6;
   }

   /* leading 0 tests */

   result = Testit("1 2 3", "%x %x %x");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result, "Test #2.13 Test scaning with 0 leading")

   result = Testit("01 02 03", "%x %x %x");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result, "Test #2.14 Test scaning with 0 leading")

   result = Testit("001 002 003", "%x %x %x");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result, "Test #2.15 Test scaning with 0 leading")

   result = Testit("0001 0002 0003", "%x %x %x");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result, "Test #2.16 Test scaning with 0 leading")

   result = Testit("0a 0b 0c", "%x %x %x");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result, "Test #2.17 Test scaning with 0 leading")

   result = Testit("0abc 0def 0abab", "%x %x %x");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result, "Test #2.18 Test scaning with 0 leading")

   /* Width specification tests */

   result = Testit("123.456.789", "%3u.%3u.%3u");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result, "Test #2.19 Test scaning with width specification")

   result = Testit(" 123.456.789", " %3u.%3u.%3u");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result, "Test #2.20 Test scaning with width specification")

   result = Testit("   123    456   789 ", " %3u %3u %3u");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result, "Test #2.21 Test scaning with width specification")

   result = Testit("123456789", "%1u%2u%3u");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result, "Test #2.22 Test scaning with width specification")

   result = Testit("123456789", "%2u%2u%3u");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result, "Test #2.23 Test scaning with width specification")

   result = Testit("123456789", "%3u%2u%3u");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result, "Test #2.24 Test scaning with width specification")

   result = Testit("123456789", "%4u%2u%3u");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result, "Test #2.25 Test scaning with width specification")

   result = Testit("123456789", "%5u%2u%2u");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result, "Test #2.26 Test scaning with width specification")

   result = Testit("123.456.789", "%3u.%3u.%3u");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result, "Test #2.27 Test scaning with width specification")

   /* Ignore whitespace tests */

   result =  Testit(" 123.456.789", "%u. %u.  %u   ");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result, "Test #2.28 Test scaning ignore whitespace")

   result =  Testit(" 123.456.789", "%d.%d.%d");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result, "Test #2.29 Test scaning ignore whitespace")

   result =  Testit(" 123.456.789", "%u.%*u.%u");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result, "Test #2.30 Test scaning ignore whitespace")

   /* Percent n test */
   result =  Testit("123 345", "%d %d%n");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result, "Test #2.31 Test scaning with percent n")

} /* Endbody */

/*TASK*-------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : Test return value from sscanf
*
*END*----------------------------------------------------------------------*/

void tc_3_main_task(void)
{
    _mqx_int result, a1, a2;

    result = sscanf("", "%d" , &a1);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == IO_EOF, "Test #3.1 Resul test: Empty input string");

    result = sscanf("z", "%d" , &a1);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == 0, "Test #3.2 Resul test: First wrong conversion");

    result = sscanf("12 z", "%d %d" , &a1, &a2);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == 1, "Test #3.3 Resul test: First correct and second wrong conversion");

    result = sscanf("12 5", "%d %d" , &a1, &a2);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == 2, "Test #3.4 Resul test: Two correct conversion");

    result = sscanf("Z 5", "%d %d" , &a1, &a2);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == 0, "Test #3.5 Resul test: First wrong and second correct conversion");
}

/******************** End Testcases ********************/

/******************** Register tests *******************/
EU_TEST_SUITE_BEGIN(suite_fio)
    EU_TEST_CASE_ADD(tc_1_main_task, "Test #1 - Test printing with varied prefix"),
    EU_TEST_CASE_ADD(tc_2_main_task, "Test #2 - Test scaning with varied format"),
    EU_TEST_CASE_ADD(tc_3_main_task, "Test #3 - Test result of sscanf")
EU_TEST_SUITE_END(suite_fio)

EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_fio, "Test of standard I/O library")
EU_TEST_REGISTRY_END()

/******************** End Register *********************/

/*TASK*-------------------------------------------------------------------
*
* Task Name    : main_task
* Comments     :
*   This task tests the formatted I/O library.
*
*END*----------------------------------------------------------------------*/

void main_task
   (
      uint32_t dummy
   )
{
     eunit_mutex_init();
     EU_RUN_ALL_TESTS(eunit_tres_array);
     eunit_test_stop();
} /* Endbody */

/* EOF */
