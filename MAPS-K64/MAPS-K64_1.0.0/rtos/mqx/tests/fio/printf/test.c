
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
* $Date    : Jul-12-2012$
*
* Comments:
*
*   This file contains testcase for testing printf function. Testcase redirect stdout to pipe. Then is possible to compare *   result from test_compare.h. test_compare.h is reference created with gcc.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#if MQX_USE_IO_OLD
#include <io_pipe.h>
#else
#include <npipe.h>
#include <stdio.h>
#endif
#include "test.h"
#include "test_compare.h"
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"

#define FILE_BASENAME   test
#define MAIN_TASK  10

#include <stdint.h>

#if TEST_FLOATING_NRS == 1
    #if ! MQX_INCLUDE_FLOATING_POINT_IO
        #error MQX_INCLUDE_FLOATING_POINT_IO
    #endif
#endif
#if PRINT_ASSERT == 1
   /// #error Printing of asserts have to be disabled. Please add option PRINT_ASSERT=0 to your IDE project settings or change it in eunit_crit_secs.h
#endif

#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
    #error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

/* definitions for tc_5_main_task*/
#define TEMP_ARRAY_SIZE (50)
#define MAX_WIDTH       (4)
#define MAX_PRECISION   (5)
typedef enum
{
    LEFT_ADJUSTMENT,
    RIGHT_ADJUSTMENT
}adjustment;

/* pipe descriptor */

#if MQX_USE_IO_OLD
void   *pipe = NULL;
#else
FILE *pipe = NULL;
#endif

void    main_task(uint32_t);
//------------------------------------------------------------------------
// Test suite function prototype
void tc_1_main_task(void);//Test function 1"),
void tc_2_main_task(void);//Test function 2"),
void tc_3_main_task(void);//Test function 3"),
void tc_4_main_task(void);//Test function 4"),
void tc_5_main_task(void);//Test function 5"),
void tc_6_main_task(void);//Test function 6"),
void tc_7_main_task(void);//Test function 7"),
//------------------------------------------------------------------------
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
{ MAIN_TASK, main_task,  4000,   7, "Main_Task", MQX_AUTO_START_TASK},
{ 0,         0,          0,      0, 0,           0                  }
};

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : compare_result
* Returned Value   : TRUE   - If reference is the same as recieved string
                   : FALSE  - Other
* Comments         :
*       This function compare recieved data from pipe with reference string.
*
*END*----------------------------------------------------------------------*/
static bool compare_result(int test_number)
{
    static uint32 comp_count = 0;
    uint32_t aval, rcvsize, i;
    char rcv_str[20];

#if MQX_USE_IO_OLD
    /* checking of pipe  is empty or not */
    ioctl(pipe, PIPE_IOCTL_CHAR_AVAIL, &aval);
    if (aval == 0)
    {
        return FALSE;
    }
    /* read data from pipe */
    ioctl(pipe, PIPE_IOCTL_NUM_CHARS_FULL, &rcvsize);
    read(pipe, &rcv_str, rcvsize);
#else
    rcvsize = fread(rcv_str, 1, 20, pipe);
#endif
    /* compare result */
    switch (test_number)
    {
    case 1: /* printing decimal numbers */
    case 2: /* printing floating numbers */
        for (i = 0; i < rcvsize; i++)
        {
            if (rcv_str[i] != compare_array[comp_count][i])
            {
                comp_count++;
                return FALSE;
            }
        }
        /* last char in reference string should be  '\0' */
        if (compare_array[comp_count++][rcvsize] != '\0')
        {
            return FALSE;
        }
        return TRUE;
     case 7: /* test for zero character printing */
        if ((rcvsize == 1) && (rcv_str[0] == 0x00))
        {
            return TRUE;
        }
        return FALSE;

     default:
        return FALSE;
    }
}
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : string_compare
* Returned Value   : TRUE   - If strings are the same
                   : FALSE  - Other
* Comments         :
*       This function compare 2 strings.
*
*END*----------------------------------------------------------------------*/
static bool string_compare(const char *p1,const char *p2)
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
* Function Name    : str_check
* Returned Value   : TRUE   - If string is correct
                   : FALSE  - Other
* Comments         :
*       check if string is correct. Used in string precision test 6
*
*END*----------------------------------------------------------------------*/
static bool str_check(const char* const original_s, const char* const compare_s, int width, int prec, int adjustment)
{
    int i, j, last;

    if (LEFT_ADJUSTMENT == adjustment)
    {
        for (i = 0; i < prec; i++)
        {
            /* Check if we are at the end of a original string */
            if ('\0' == original_s[i])
            {
                break;
            }
            /* Compare character which should be the same */
            if (original_s[i] != compare_s[i])
            {
                return FALSE;
            }
        }
        for (; i < width; i++)
        {
            if (compare_s[i] != ' ')
            {
                return FALSE;
            }
        }
        /* Check if we are at the end of compared string */
        return ('\0' == compare_s[i])? TRUE : FALSE;
    }
    else    /* RIGHT_ADJUSTMENT */
    {
        /* Find last character */
        last = 0;
        while ('\0' != compare_s[last])
        {
            last++;
            if(TEMP_ARRAY_SIZE <= last)
            {
                return FALSE;
            }
        }
        /* Count of character cannot be less than width*/
        if (last < width)
        {
            return FALSE;
        }
        /* adjust precision if original text is smaller than precision */
        i = 0;
        while ('\0' != original_s[i])
        {
            i++;
            if(TEMP_ARRAY_SIZE <= i)
            {
                return FALSE;
            }
        }
        prec = i > prec ? prec : i;
        /* Check space leading if there any*/
        for (i = 0; i < (last - prec); i++)
        {
            if (compare_s[i] != ' ')
            {
                return FALSE;
            }
        }
        for (j = 0; i < last; i++, j++)
        {
            /* Compare character which should be the same */
            if (original_s[j] != compare_s[i])
            {
                return FALSE;
            }
        }
        return TRUE;
    }
}

/******************** Begin Testcases ********************/
/*TASK*---------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1: printing decimal numbers
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    int nr, f, s, psuff, ppref;
    char tmpstr[BUF_LENGTH];
#if TEST_INTEGER_NRS
    for (nr = 0; nr < NR_CNT; nr++)
    {
        for (f = 0; f < NR_FLAGS; f++)
        {
            for (s = 0; s < NR_SPECS; s++)
            {
                for (ppref = 0; ppref < NR_PREC_PREF; ppref++)
                {
                    for (psuff = 0; psuff < NR_PREC_SUFF; psuff++)
                    {
                        sprintf(tmpstr, "%%%s%s%s%c", flags[f], prec_prefix[ppref], prec_suffix[psuff], specifiers[s]);
                       fprintf(pipe, tmpstr, numbers[nr]);
                        EU_ASSERT_REF_TC_MSG( tc_1_main_task, compare_result(1) , "1.1 Comparing failed");
                    }
                }
            }
        }
    }
#endif
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2: printing floating point numbers
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
#if TEST_FLOATING_NRS
    int nr, f, s, psuff, ppref;
    char tmpstr[BUF_LENGTH];
    float *b;
    for (nr = 0; nr < NR_FCNT; nr++)
    {
        for (f = 0; f < NR_FLAGS; f++)
        {
            for (s = 0; s < NR_FSPECS; s++)
            {
                for (ppref = 0; ppref < NR_PREC_PREF; ppref++)
                {
                    for (psuff = 0; psuff < NR_PREC_SUFF; psuff++)
                    {
                        sprintf(tmpstr, "%%%s%s%s%c", flags[f], prec_prefix[ppref], prec_suffix[psuff], fspecifiers[s]);
                        fprintf(pipe, tmpstr, fnumbers[nr]);
                        EU_ASSERT_REF_TC_MSG( tc_2_main_task, compare_result(2) , "2.1 Comparing failed");
                    }
                }
            }
        }
    }
    for (nr = 0; nr < NR_FSCNT; nr++)
    {
        for (f = 0; f < NR_FLAGS; f++)
        {
            for (s = 0; s < NR_FSPECS; s++)
            {
                for (ppref = 0; ppref < NR_PREC_PREF; ppref++)
                {
                    for (psuff = 0; psuff < NR_PREC_SUFF; psuff++)
                    {
                        sprintf(tmpstr, "%%%s%s%s%c", flags[f], prec_prefix[ppref], prec_suffix[psuff], fspecifiers[s]);
                        b = (float*)&fspec[nr];
                        fprintf(pipe, tmpstr, *b);
                        EU_ASSERT_REF_TC_MSG( tc_2_main_task, compare_result(2) , "2.2 Comparing failed");
                    }
                }
            }
        }
    }
#endif

}
/*TASK*---------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : TEST #1: snprintf test
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
    int c, i, j;
    char tmpstr[BUF_LENGTH];
    for (i = 0; i < BUF_LENGTH; i++)
    {
        /*25 ASCII characters are used and contoll */
        c = snprintf(tmpstr, i , "012%d567%d:;<=>?@ABCDEFG%c", 34, 89, 'H');
        EU_ASSERT_REF_TC_MSG( tc_3_main_task, c == 25, "3.1 Length of string is incorrect");
        /* compare string with ASCCI */
        for (j = 0; (j < (i - 1)) && (j < c); j++)
        {
            EU_ASSERT_REF_TC_MSG( tc_3_main_task, tmpstr[j] == (char)('0' + j), "3.2 String incorrect");
        }
        /* check last ending character */
        if ((i != 0) && (i <= c))
        {
            EU_ASSERT_REF_TC_MSG( tc_3_main_task, tmpstr[i - 1] == '\0' , "3.3 Length of string is incorrect");
        }
        /* it is possible to know size of string without buffer */
        c = snprintf(NULL, 0 , "012%d567%d:;<=>?@ABCDEFG%c", 34, 89, 'H');
        EU_ASSERT_REF_TC_MSG( tc_3_main_task, c == 25, "3.4 Length of string is incorrect");
    }


}
/*TASK*---------------------------------------------------------------------
*
* Task Name    : tc_4_main_task
* Comments     : TEST #1: testcase for length modifiers characters h, hh, l, ll, L
*
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
    int i = 0;
    char tmpstr[BUF_LENGTH];
    const char * const compare[] =
    {
#if TEST_INTEGER_NRS
        "255",
        "65535",
        "4294967295",
        "4294967295",
        "18446744073709551615",
        "-1",
        "-1",
        "-1",
        "-1",
        "-1",
        "-1-1-1-1-1",
        "-1-1-1-1-1",
        "255655354294967295429496729518446744073709551615",
        "184467440737095516154294967295429496729565535255",
        "377",
        "177777",
        "37777777777",
        "37777777777",
        "1777777777777777777777",
        "FF",
        "FFFF",
        "FFFFFFFF",
        "FFFFFFFF",
        "FFFFFFFFFFFFFFFF",
#endif
#if TEST_FLOATING_NRS
        "-65.535",
        "-65.535"
#endif
    };
#if TEST_INTEGER_NRS
   /* unsigned specifiers */
    snprintf( tmpstr, BUF_LENGTH, "%hhu" ,(uint8_t) -1);  /* 255 */
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, string_compare(tmpstr,compare[i++]), "4.1 String incorrect");
    snprintf( tmpstr, BUF_LENGTH, "%hu"  ,(uint16_t) -1); /* 65535 */
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, string_compare(tmpstr,compare[i++]), "4.2 String incorrect");
    snprintf( tmpstr, BUF_LENGTH, "%u"   ,(uint32_t) -1); /* 4294967295 */
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, string_compare(tmpstr,compare[i++]), "4.3 String incorrect");
    snprintf( tmpstr, BUF_LENGTH, "%lu"  ,(uint32_t) -1); /* 4294967295 */
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, string_compare(tmpstr,compare[i++]), "4.4 String incorrect");
    snprintf( tmpstr, BUF_LENGTH, "%llu" ,(uint64_t) -1); /* 18446744073709551615 */
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, string_compare(tmpstr,compare[i++]), "4.5 String incorrect");

   /* signed specifiers */
    snprintf( tmpstr, BUF_LENGTH, "%hhd" , (int8_t) -1 );    /* -1 */
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, string_compare(tmpstr,compare[i++]), "4.6 String incorrect");
    snprintf( tmpstr, BUF_LENGTH, "%hd"  , (int16_t) -1 );   /* -1 */
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, string_compare(tmpstr,compare[i++]), "4.7 String incorrect");
    snprintf( tmpstr, BUF_LENGTH, "%d"   , (int32_t) -1 );   /* -1 */
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, string_compare(tmpstr,compare[i++]), "4.8 String incorrect");
    snprintf( tmpstr, BUF_LENGTH, "%ld"  , (int32_t) -1 );   /* -1 */
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, string_compare(tmpstr,compare[i++]), "4.9 String incorrect");
    snprintf( tmpstr, BUF_LENGTH, "%lld" , (int64_t) -1 );   /* -1 */
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, string_compare(tmpstr,compare[i++]), "4.10 String incorrect");

    snprintf( tmpstr, BUF_LENGTH, "%hhd%hd%d%ld%lld" , (int8_t) -1, (int16_t) -1 , (int32_t) -1, (int32_t) -1, (int64_t) -1 );     /* -1-1-1-1-1 */
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, string_compare(tmpstr,compare[i++]), "4.11 String incorrect");
    snprintf( tmpstr, BUF_LENGTH, "%lld%ld%d%hd%hhd" , (int64_t) -1, (int32_t) -1 , (int32_t) -1, (int16_t) -1, (int8_t) -1 );     /* -1-1-1-1-1 */
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, string_compare(tmpstr,compare[i++]), "4.12 String incorrect");
    snprintf( tmpstr, BUF_LENGTH, "%hhu%hu%u%lu%llu" , (uint8_t) -1, (uint16_t) -1 , (uint32_t) -1, (uint32_t) -1, (uint64_t) -1 );     /* 255655354294967295429496729518446744073709551615 */
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, string_compare(tmpstr,compare[i++]), "4.13 String incorrect");
    snprintf( tmpstr, BUF_LENGTH, "%llu%lu%u%hu%hhu" , (uint64_t) -1, (uint32_t) -1 , (uint32_t) -1, (uint16_t) -1, (uint8_t) -1 );     /* 184467440737095516154294967295429496729565535255 */
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, string_compare(tmpstr,compare[i++]), "4.14 String incorrect");

    snprintf( tmpstr, BUF_LENGTH, "%hho" , (int8_t)  -1);    /* 377 */
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, string_compare(tmpstr,compare[i++]), "4.15 String incorrect");
    snprintf( tmpstr, BUF_LENGTH, "%ho"  , (int16_t) -1);    /* 177777 */
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, string_compare(tmpstr,compare[i++]), "4.16 String incorrect");
    snprintf( tmpstr, BUF_LENGTH, "%o"   , (int32_t) -1);    /* 37777777777 */
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, string_compare(tmpstr,compare[i++]), "4.17 String incorrect");
    snprintf( tmpstr, BUF_LENGTH, "%lo"  , (int32_t) -1);    /* 37777777777 */
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, string_compare(tmpstr,compare[i++]), "4.18 String incorrect");
    snprintf( tmpstr, BUF_LENGTH, "%llo" , (int64_t) -1);    /* 1777777777777777777777 */
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, string_compare(tmpstr,compare[i++]), "4.19 String incorrect");

    snprintf( tmpstr, BUF_LENGTH, "%hhX" , (int8_t) -1);     /* FF */
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, string_compare(tmpstr,compare[i++]), "4.20 String incorrect");
    snprintf( tmpstr, BUF_LENGTH, "%hX"  , (int16_t) -1);    /* FFFF */
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, string_compare(tmpstr,compare[i++]), "4.21 String incorrect");
    snprintf( tmpstr, BUF_LENGTH, "%X"   , (int32_t) -1);    /* FFFFFFFF */
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, string_compare(tmpstr,compare[i++]), "4.22 String incorrect");
    snprintf( tmpstr, BUF_LENGTH, "%lX"  , (int32_t) -1);    /* FFFFFFFF */
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, string_compare(tmpstr,compare[i++]), "4.23 String incorrect");
    snprintf( tmpstr, BUF_LENGTH, "%llX" , (int64_t) -1);    /* FFFFFFFFFFFFFFFF */
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, string_compare(tmpstr,compare[i++]), "4.24 String incorrect");
#endif
#if TEST_FLOATING_NRS
    snprintf( tmpstr, BUF_LENGTH, "%Lg" , (double) -65.535);    /* -65.535 */
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, string_compare(tmpstr,compare[i++]), "4.25 String incorrect");
    snprintf( tmpstr, BUF_LENGTH, "%g" , (float) -65.535);    /* -65.535 */
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, string_compare(tmpstr,compare[i++]), "4.26 String incorrect");
#endif

}
/*TASK*---------------------------------------------------------------------
*
* Task Name    : tc_5_main_task
* Comments     : TEST #5: string precision
*
*END*---------------------------------------------------------------------*/
void tc_5_main_task(void)
{
    int i = 0;
    char tmpstr[BUF_LENGTH];
    char string[] = "abcde";

    const char * const compare[] =
    {
        "",
        "abcde",
        "abcde",

        "",
        "",
        "a",
        "ab",
        "ab",
        "abc",

        "  ab",
        "  a"
    };
    snprintf( tmpstr, 1, "%s" ,string);  /* "" */
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, string_compare(tmpstr,compare[i++]), "5.1 String incorrect");
    snprintf( tmpstr, BUF_LENGTH, "%s" ,string);  /* "abcde" */
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, string_compare(tmpstr,compare[i++]), "5.2 String incorrect");
    snprintf( tmpstr, BUF_LENGTH, "%S" ,string);  /* "abcde" */
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, string_compare(tmpstr,compare[i++]), "5.3 String incorrect");

    snprintf( tmpstr, BUF_LENGTH, "%.s" ,string);  /* "" */
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, string_compare(tmpstr,compare[i++]), "5.4 String incorrect");
    snprintf( tmpstr, BUF_LENGTH, "%.0s" ,string);  /* "" */
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, string_compare(tmpstr,compare[i++]), "5.5 String incorrect");
    snprintf( tmpstr, BUF_LENGTH, "%.1s" ,string);  /* "a" */
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, string_compare(tmpstr,compare[i++]), "5.6 String incorrect");
    snprintf( tmpstr, BUF_LENGTH, "%.2s" ,string);  /* "ab" */
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, string_compare(tmpstr,compare[i++]), "5.7 String incorrect");
    snprintf( tmpstr, 3, "%.3s" ,string);  /* "ab" */
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, string_compare(tmpstr,compare[i++]), "5.8 String incorrect");
    snprintf( tmpstr, BUF_LENGTH, "%.*s" ,3, string);  /* "abc" */
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, string_compare(tmpstr,compare[i++]), "5.9 String incorrect");

    snprintf( tmpstr, BUF_LENGTH, "%4.2s" ,string);  /* "  ab" */
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, string_compare(tmpstr,compare[i++]), "5.10 String incorrect");
    snprintf( tmpstr, 4, "%4.2s" ,string);  /* "  a" */
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, string_compare(tmpstr,compare[i++]), "5.11 String incorrect");

}

/*TASK*---------------------------------------------------------------------
*
* Task Name    : tc_6_main_task
* Comments     : TEST #6: string precision more detailed fo %width.precision s
*
*END*---------------------------------------------------------------------*/
void tc_6_main_task(void)
{
    #define S_SIZE (7)
    char s[S_SIZE]  = {'A','B','C','D','E','F','\0'};
    char  temp_s[TEMP_ARRAY_SIZE];
    char  result_s[TEMP_ARRAY_SIZE];
    unsigned int width, prec;
    int n;

    /* Test all sizes of string from 0 to S_SIZE */
    for (n = (S_SIZE - 1); n >= 0; n--)
    {

        s[n] = '\0';
    /* Right adjustment long string */
        for (width = 0; width <= MAX_WIDTH; width++)
        {
            for (prec = 0; prec <= MAX_PRECISION; prec++)
            {
                snprintf(temp_s, TEMP_ARRAY_SIZE, "%%%d.%ds", width, prec);
                snprintf(result_s, TEMP_ARRAY_SIZE, temp_s, s);
                EU_ASSERT_REF_TC_MSG( tc_6_main_task, str_check(s, result_s, width, prec, RIGHT_ADJUSTMENT), "6.1 String incorrect for right adjustment and long string");
            }
        }
    /* Left adjustment long string */
        for (width = 0; width <= MAX_WIDTH; width++)
        {
            for (prec = 0; prec <= MAX_PRECISION; prec++)
            {
                snprintf(temp_s, TEMP_ARRAY_SIZE, "%%-%d.%ds", width, prec);
                snprintf(result_s, TEMP_ARRAY_SIZE, temp_s, s);
                EU_ASSERT_REF_TC_MSG( tc_6_main_task, str_check(s, result_s, width, prec, LEFT_ADJUSTMENT), "6.2 String incorrect for left adjustment and long string");
            }
        }
    }
}
/*TASK*---------------------------------------------------------------------
*
* Task Name    : tc_7_main_task
* Comments     : TEST #6: printing 0x00 char
*
*END*---------------------------------------------------------------------*/
void tc_7_main_task(void)
{
    int count = 0;
    char c = 0x00;

    count = fprintf(pipe, "%c" ,c); /* 0x00 */
    EU_ASSERT_REF_TC_MSG( tc_7_main_task,  compare_result(7), "7.1 zero char incorrect");
    count = fprintf(pipe, "%c" ,c);  /* 0x00 */
    EU_ASSERT_REF_TC_MSG( tc_7_main_task,  compare_result(7), "7.2 zero char incorrect");
    count = fprintf(pipe, "%c" ,c);  /* 0x00 */
    EU_ASSERT_REF_TC_MSG( tc_7_main_task,  compare_result(7), "7.3 zero char incorrect");

    count = fprintf(pipe, "%.c" ,c);  /* 0x00 */
    EU_ASSERT_REF_TC_MSG( tc_7_main_task,  compare_result(7), "7.4 zero char incorrect");
    count = fprintf(pipe, "%.0c" ,c);  /* 0x00 */
    EU_ASSERT_REF_TC_MSG( tc_7_main_task,  compare_result(7), "7.5 zero char incorrect");
    count = fprintf(pipe, "%.3c" ,c);  /* 0x00 */
    EU_ASSERT_REF_TC_MSG( tc_7_main_task,  compare_result(7), "7.6 zero char incorrect");
    count = fprintf(pipe, "%.*c" ,5 ,c);  /* 0x00 */
    EU_ASSERT_REF_TC_MSG( tc_7_main_task,  compare_result(7), "7.7 zero char incorrect");

    count = fprintf(pipe, "%4.2c" ,c);  /* 0x20,0x20,0x20,0x00 */
    EU_ASSERT_REF_TC_MSG( tc_7_main_task,  count == 4, "7.8 zero char incorrect");
    count = fprintf(pipe, "%4.2c" ,c);  /* 0x20,0x20,0x20,0x00 */
    EU_ASSERT_REF_TC_MSG( tc_7_main_task,  count == 4, "7.9  zero char incorrect");
}


/******************** End Testcases ********************/

//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_1)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST CASE for printf decimal numbers"),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST CASE for printf float numbers"),
    EU_TEST_CASE_ADD(tc_3_main_task, " TEST CASE for snprintf function"),
    EU_TEST_CASE_ADD(tc_4_main_task, " TEST CASE for length modifiers characters"),
    EU_TEST_CASE_ADD(tc_5_main_task, " TEST CASE for string precision 1"),
    EU_TEST_CASE_ADD(tc_6_main_task, " TEST CASE for string precision 2"),
    EU_TEST_CASE_ADD(tc_7_main_task, " TEST CASE for zero character printing"),
EU_TEST_SUITE_END(suite_1)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_1, " - Test for printf")
EU_TEST_REGISTRY_END()
//------------------------------------------------------------------------

/*TASK*--------------------------------------------------------------
*
* Task Name : main_task
* Comments  :
*
*END*--------------------------------------------------------------*/

void main_task
    (
        uint32_t dummy
    )
{
    /* default stdio driver*/
    void   *old_stdout;
#if MQX_USE_IO_OLD
    /* install pipe with 32byte buffer */
    if (MQX_OK != _io_pipe_install("pipe_std:",(uint32_t) 32, (uint32_t)0))
    {
        printf("Failed to install pipe\n");
        _task_block();
    }

    /* open pipe and change stdout and stdin to pipe */
    pipe = (void *)fopen("pipe_std:", BSP_DEFAULT_IO_OPEN_MODE);
#else
    int pipe_fd;
    extern const NIO_DEV_FN_STRUCT npipe_dev_fn;
    const NPIPE_INIT_DATA_STRUCT npipe_init_data = {32};      // osjtag USB->SERIAL
     /* install pipe with 32byte buffer */
    if (NULL == _nio_dev_install("pipe_std:", &npipe_dev_fn, (void*)&npipe_init_data))
    {
        printf("Failed to install pipe\n");
        _task_block();
    }

    /* open pipe in nonblocking mode*/
    pipe_fd = _nio_open("pipe_std:", 0x0004, 0);/// O_NONBLOCK);
    if (0 > pipe_fd)
    {
        printf("Failed to open pipe fd\n");
        _task_block();
    }
    pipe = fdopen(pipe_fd, "r");
#endif
    /* checking of pipe */
    if (pipe == NULL)
    {
        printf("Failed to open pipe\n");
        _task_block();

    }
    ////* redirect STDOUT to pipe and remember old STDOUT */
   /// old_stdout = _io_set_handle(IO_STDOUT, pipe);

    eunit_mutex_init();
    EU_RUN_ALL_TESTS(eunit_tres_array);

   /// /* change STDOUT to old driver to printf result off test */
  ///  _io_set_handle(IO_STDOUT, old_stdout);
    eunit_test_stop();
    _task_block();
}/* Endbody */
/* EOF */
