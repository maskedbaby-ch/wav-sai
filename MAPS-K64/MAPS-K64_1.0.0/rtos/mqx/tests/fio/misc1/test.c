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
* $FileName: test.c$
* $Version : 4.0.2.0$
* $Date    : Jul-22-2013$
*
* Comments:
*
*   This file contains code for the MQX Standard I/O verification program,
*   includes all remaining test cases, which haven't been verified yet,
*   of following functions:
*   _io_atoi, _io_strtod, _io_strtolower, _io_strcasecmp, _io_strcasecmp,
*   _io_modf, _io_feof, _io_fseek, _io_fflush, _io_ftell, _io_ferror,
*   _io_clearerr, _io_fclose.
*
* Requirements:
*
*   FIO001, FIO003, FIO005, FIO018, FIO033, FIO034, FIO035, FIO036, FIO041,
*   FIO042, FIO057, FIO058, FIO072, FIO073, FIO074, FIO075, FIO076, FIO077,
*   FIO078, FIO079, FIO080.
*
*END************************************************************************/

#include <mqx_inc.h>
#include <bsp.h>
#include <fio.h>
#include <io_prv.h>
#include "fake_drv.h"
#include "test.h"
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"
#include <math.h>
#include <ctype.h>

/*------------------------------------------------------------------------
** Test suite function prototype
*/
void tc_1_main_task(void);   /* TEST #1 - Testing function _io_atoi */
void tc_2_main_task(void);   /* TEST #2 - Testing function _io_strtod */
void tc_3_main_task(void);   /* TEST #3 - Testing function _io_strtolower */
void tc_4_main_task(void);   /* TEST #4 - Testing function _io_strcasecmp */
void tc_5_main_task(void);   /* TEST #5 - Testing function _io_strncasecmp */
void tc_6_main_task(void);   /* TEST #6 - Testing function _io_modf */
void tc_7_main_task(void);   /* TEST #7 - Testing function _io_feof */
void tc_8_main_task(void);   /* TEST #8 - Testing function _io_fseek */
void tc_9_main_task(void);   /* TEST #9 - Testing function _io_fflush */
void tc_10_main_task(void);  /* TEST #10 - Testing function _io_ftell */
void tc_11_main_task(void);  /* TEST #11 - Testing function _io_ferror */
void tc_12_main_task(void);  /* TEST #12 - Testing function _io_clearerr */
void tc_13_main_task(void);  /* TEST #13 - Testing function _io_fclose */

/*------------------------------------------------------------------------
** MQX task template
*/
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
    { MAIN_TASK,  main_task, 3000,  9,  "main", MQX_AUTO_START_TASK },
    {         0,          0,    0,  0,       0,                   0 }
};

/******************** Begin Test cases **********************/

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1:
*   This test case is to verify function _io_atoi in the cases of:
*   - Converting different strings to integer value properly.
*   - Converting string to integer value which causes overflow.
*   - Converting invalid string to integer value.
* Requirements:
*   FIO057, FIO058
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    _mqx_int result;
    _mqx_int num;
    uint8_t c;
    char *str = _mem_alloc_zero(MAX_CHAR);

    /* Store the maximum integer value as a string in str */
    sprintf(str, "%d", MAX_MQX_INT);
    /* Convert str to an integer number by calling function _io_atoi */
    result = _io_atoi(str);
    /* Check the returned value must be (MAX_MQX_INT) to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MAX_MQX_INT, "TEST #1: Testing 1.01: Maximum integer");

    /* Test with string presents a numeric number greater than MAX_MQX_INT */
    sprintf(str, "%d%c", MAX_MQX_INT, '0');
    /* Convert str to an integer number by calling function _io_atoi */
    result = _io_atoi(str);
    /* Check the returned value must be (0) to verify integer overflow */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == 0, "TEST #1: Testing 1.02: Integer overflow");

    /* Get the number to test */
    num = (MAX_MQX_INT - 9)/10;

    /* Loop to test converting string with non-numeric character at last */
    for(c = 0; c <= MAX_CHAR; c++)
    {
        /* Leave a non-numeric character instead of NULL character at last of string str */
        sprintf(str, "%d%c", num, c);
        /* Convert str to an integer number by calling function _io_atoi */
        result = _io_atoi(str);

        if ((c >= '0') && (c <= '9'))
        {
            /* If insert digit character at the end */
            EU_ASSERT_REF_TC_MSG(tc_1_main_task, result/10 == num, "TEST #1: Testing 1.03: Insert digit at last");
        }
        else
        {
            /* If insert non-digit char at the end */
            EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == num, "TEST #1: Testing 1.04: Insert non-numeric at last");
        }
    }

    /* Get the number to test */
    num = MAX_MQX_INT/100;

    /* Loop to test converting string with non-numeric character at first */
    for(c = 1; c <= MAX_CHAR; c++)
    {
        sprintf(str, "%c%d", c, num);
        /* Convert str to an integer number by calling function _io_atoi */
        result = _io_atoi(str);

        if ((c == '0') || (c == '+') || (c == ' '))
        {
            EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == num, "TEST #1: Testing 1.05: Insert space and '+' at first");
        }
        else if (c == '-')
        {
            EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == (-1) * num, "TEST #1: Testing 1.06: Insert minus character at first");
        }
        else if ((c > '0') && (c <= '9'))
        {
            EU_ASSERT_REF_TC_MSG(tc_1_main_task, result > num, "TEST #1: Testing 1.07: Insert digit at first ");
        }
        else
        {
            EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == 0, "TEST #1: Testing 1.08: Insert non-digit at first");
        }
    }

    /* Convert null string to integer number by calling function _io_atoi */
    result = _io_atoi(NULL);
    /* Check the returned value must be (0) to verify unsuccessful convert */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == 0, "TEST #1: Testing 1.09: Null string");

    /* Convert whitespace string to integer number by calling function _io_atoi */
    result = _io_atoi("    ");
    /* Check the returned value must be (0) to verify unsuccessful convert */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == 0, "TEST #1: Testing 1.10: Whitespace string");

    _mem_free(str);
}


/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2:
*   This test case is to verify function _io_strtod in the cases of:
*   - Converting different strings to double value properly.
*   - Converting string to double value which causes overflow.
*   - Converting string to double value which causes underflow.
*   - Converting invalid string to double value.
* Requirements:
*   FIO077, FIO078, FIO079, FIO080
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    double result;
    uint8_t c;
    char *str = _mem_alloc_zero(MAX_CHAR);

#if 0 /* Disable for following cases to run properly */
    /* TODO: ENGR00273704 */
    /* Store the maximum double value as a string in str */
    sprintf(str, "%e", MAX_DOUBLE);
    /* Convert str to an double value by calling function _io_strtod */
    result = _io_strtod(str, NULL);
    /* Check the returned value must be (MAX_DOUBLE) to verify successful conversion */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MAX_DOUBLE, "TEST #2: Testing 2.01: Maximum double");
#endif

    /* Store the minimum double value as a string in str */
    sprintf(str, "%e", MIN_DOUBLE);
    /* Convert str to an double number by calling function _io_strtod */
    result = _io_strtod(str, NULL);
    /* Check the returned value must be MIN_DOUBLE to verify successful conversion */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, comp_double(result, MIN_DOUBLE, TEST_EPS), "TEST #2: Testing 2.02: Minimum double");

    /* Loop to test converting string with non-numeric character at last */
    for(c = 0; c <= MAX_CHAR; c++)
    {
        /* Leave a non-numeric character instead of NULL character at last of string str */
        sprintf(str, "%e%c", TEST_DOUBLE, c);
        /* Convert str to an double number by calling function _io_strtod */
        result = _io_strtod(str, NULL);
        /* Check if the returned result is incorrect */
        if((c >= '0') && (c <= '9')) 
        {
            /* Do nothing */
        }
        else
        {
            EU_ASSERT_REF_TC_MSG(tc_2_main_task, comp_double(result, TEST_DOUBLE, TEST_EPS), "TEST #2: Testing 2.03: Non-numeric at first");
        }
    }

    /* Loop to test converting string with non-numeric character at first */
    for(c = 1; c <= MAX_CHAR; c++)
    {
        /* Leave a non-numeric character at first of string str */
        sprintf(str, "%c%e", c, TEST_DOUBLE);
        /* Convert str to an double number by calling function _io_strtod */
        result = _io_strtod(str, NULL);

        /* Check if the returned value is incorrect */
        if(((c >= '0') && (c <= '9')) || c == '.' || isspace(c))
        {
            /* Do nothing */
        }
        else if (c == '+' || c == ' ' )
        {
            EU_ASSERT_REF_TC_MSG(tc_2_main_task, comp_double(result, TEST_DOUBLE, TEST_EPS), "TEST #2: Testing 2.04: Positive double");
        }
        else if (c == '-' )
        {
            EU_ASSERT_REF_TC_MSG(tc_2_main_task, comp_double(result, (-1.0)*TEST_DOUBLE, TEST_EPS), "TEST #2: Testing 2.05: Negative double");
        }
        else
        {
            EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == 0.0, "TEST #2: Testing 2.06: Non-numeric at last");
        }
    }

    /* Store the very small double value as a string in str */
    strcpy(str, "1.1E-999\0");
    /* Convert str to an double number by calling function _io_strtod */
    result = _io_strtod(str, NULL);
    /* Check the returned value must be (0) to verify double underflow */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == 0.0, "TEST #2: Testing 2.07: Double underflow");
    /* Check the task error code must be MQX_ERANGE to verify correct error code was set */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, _task_errno == MQX_ERANGE, "TEST #2: Testing 2.08: Verify task error code");
    /* Clear task error code */
    _task_errno = MQX_EOK;

    /* Store the very big double value as a string in str */
    strcpy(str, "1.1E+999\0");
    /* Convert str to an double number by calling function _io_strtod */
    result = _io_strtod(str, NULL);
    /* Check the task error code must be MQX_ERANGE to verify correct error code was set */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, _task_errno == MQX_ERANGE, "TEST #2: Testing 2.09: Verify task error code");
    /* Clear task error code */
    _task_errno = MQX_EOK;

    /* Convert null string to double number by calling function _io_strtod */
    result = _io_strtod(NULL, NULL);
    /* Check the returned value must be (0.0) to verify unsuccessful convert */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == 0.0, "TEST #2: Testing 2.10: Null string");

    /* Convert whitespace string to double number by calling function _io_strtod */
    result = _io_strtod("    ", NULL);
    /* Check the returned value must be (0.0) to verify unsuccessful convert */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == 0.0, "TEST #2: Testing 2.11: Whitespace string");

    _mem_free(str);

}


/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : TEST #3:
*   This test case is to verify function _io_strtolower in the cases of:
*   - Converting specified string to lower case properly.
*   - Converting invalid string to lower case.
* Requirements:
*   FIO072, FIO073
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
    char *str;
    bool result;
    char c;
    int32_t i;

    /* Allocate memory to store all ascii characters by calling function _mem_alloc */
    str = (char *)_mem_alloc_zero(MAX_CHAR);
    /* Check the returned pointer, which is stored in str, must be NOT NULL to verify successful allocation */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, str != NULL, "TEST #3: Testing 3.01: Allocate memory");

    /* Try to convert NULL string by calling function _io_strtolower */
    result = _io_strtolower(NULL);
    /* Check the returned result must be FALSE to verify unsuccessful convert */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == FALSE, "TEST #3: Testing 3.02: Cannot convert NULL string");

    c = 'A';
    i = 0;
    /* Create a string that contains all UPPER case characters */
    while ((c <= 'Z') && (i < MAX_CHAR))
    {
        str[i++] = c++;
    }

    /* TODO: ENGR00270028 */
    /* Convert string str by calling function _io_strtolower */
    result = _io_strtolower(str);
    /* Check the returned result must be TRUE to verify successful convert */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == TRUE, "TEST #3: Testing 3.03: Convert successfully");

    c = 'a';
    i = 0;
    /* All characters in string str has been converted to lower cases correctly */
    while ((c <= 'z') && (i < MAX_CHAR))
    {
        /* Check the iteration must be (MAX_CHAR + 1) to verify correct convert */
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, str[i] == c, "TEST #3: Testing 3.04: Verify string");
        i++; c++;
    }

    /* Free the allocated memory block str by calling _mem_free. Check the returned value must be MQX_OK */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, _mem_free(str) == MQX_OK, "TEST #3: Testing 3.05: Deallocate memory");
}


/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_4_main_task
* Comments     : TEST #4:
*   This test case is to verify function _io_strcasecmp in the cases of:
*   - Comparing two strings which are the same.
*   - Comparing two strings which are the different.
*   - Comparing with NULL strings.
* Requirements:
*   FIO075
*
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
    char *str1, *str2;
    _mqx_int result;
    _mqx_uint len;
    char c;

    /* Allocate memory for string 1 (MAX_CHAR bytes) by calling function _mem_alloc */
    str1 = (char *)_mem_alloc_zero(MAX_CHAR);
    /* Check the returned pointer, which is stored in str1, must be NOT NULL to verify successful allocation */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, str1 != NULL, "TEST #4: Testing 4.01: Allocate memory for string 1");

    /* Allocate memory for string 2 (MAX_CHAR bytes) by calling function _mem_alloc */
    str2 = (char *)_mem_alloc_zero(MAX_CHAR);
    /* Check the returned pointer, which is stored in str2, must be NOT NULL to verify successful allocation */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, str2 != NULL, "TEST #4: Testing 4.02: Allocate memory for string 2");

    len = 0;
    c = 'A';
    /* Fill str1 with all uppercase characters (from A to Z) and a NULL termination */
    /* Fill str2 with all lowercase characters (from a to z) and a NULL termination */
    while ((c <= 'Z') && (len < MAX_CHAR))
    {
        str1[len] = len + 'A';
        str2[len] = len + 'a';
        len++; c++;
    }

    /* Compare str1 with str2 by calling function _io_strcasecmp */
    result = _io_strcasecmp(str1, str2);
    /* Check the returned result must be 0 to verify two strings are the same */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == 0, "TEST #4: Testing 4.03: Same string");

    /* Change one character in str1 from alphabet to number */
    str1[len/2] = '1';
    /* Compare str1 with str2 by calling function _io_strcasecmp */
    result = _io_strcasecmp(str1, str2);
    /* Check the returned result must be smaller than 0 to verify different strings */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result < 0, "TEST #4: Testing 4.04: Different strings");

    /* Compare str2 with str1 by calling function _io_strcasecmp */
    result = _io_strcasecmp(str2, str1);
    /* Check the returned result must be bigger than 0 to verify different strings */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result > 0, "TEST #4: Testing 4.05: Different strings");

    /* Make the str1 to be shorter than str2 */
    str1[len/2] = 0;
    /* Compare str1 with str2 by calling function _io_strcasecmp */
    result = _io_strcasecmp(str1, str2);
    /* Check the returned result must be smaller than 0 to verify different strings */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result < 0, "TEST #4: Testing 4.06: Different strings");

    /* Compare str2 with str1 by calling function _io_strcasecmp */
    result = _io_strcasecmp(str2, str1);
    /* Check the returned result must be bigger than 0 to verify different strings */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result > 0, "TEST #4: Testing 4.07: Different strings");

    /* Free the allocated memory block str1 by calling _mem_free. Check the returned value must be MQX_OK */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, _mem_free(str1) == MQX_OK, "TEST #4: Testing 4.08: Deallocate memory of str1");

    /* Free the allocated memory block str2 by calling _mem_free. Check the returned value must be MQX_OK */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, _mem_free(str2) == MQX_OK, "TEST #4: Testing 4.09: Deallocate memory of str2");
}


/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_5_main_task
* Comments     : TEST #5:
*   This test case is to verify function _io_strncasecmp in the cases of:
*   - Comparing n characters of two strings which are the same.
*   - Comparing n characters of two strings which are the different.
*   - Comparing n characters of NULL strings.
* Requirements:
*   FIO076
*
*END*---------------------------------------------------------------------*/
void tc_5_main_task(void)
{
    char *str1, *str2;
    _mqx_int result;
    _mqx_uint len;
    char c;

    /* Allocate memory for string 1 (MAX_CHAR bytes) by calling function _mem_alloc */
    str1 = (char *)_mem_alloc_zero(MAX_CHAR);
    /* Check the returned pointer, which is stored in str1, must be NOT NULL to verify successful allocation */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, str1 != NULL, "TEST #5: Testing 5.01: Allocate memory for string 1");

    /* Allocate memory for string 2 (MAX_CHAR bytes) by calling function _mem_alloc */
    str2 = (char *)_mem_alloc_zero(MAX_CHAR);
    /* Check the returned pointer, which is stored in str2, must be NOT NULL to verify successful allocation */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, str2 != NULL, "TEST #5: Testing 5.02: Allocate memory for string 2");

    len = 0;
    c = 'A';
    /* Fill str1 with all uppercase characters (from A to Z) and a NULL termination */
    /* Fill str2 with all lowercase characters (from a to z) and a NULL termination */
    while ((c <= 'Z') && (len < MAX_CHAR))
    {
        str1[len] = len + 'A';
        str2[len] = len + 'a';
        len++; c++;
    }

    /* Compare all characters in str1 with str2 by calling function _io_strncasecmp */
    result = _io_strncasecmp(str1, str2, len + 1);
    /* Check the returned result must be 0 to verify two strings are the same */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == 0, "TEST #5: Testing 5.03: Same string");

    /* Compare no character in str1 with str2 by calling function _io_strncasecmp */
    result = _io_strncasecmp(str1, str2, 0);
    /* Check the returned result must be 0 to verify two strings are the same */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == 0, "TEST #5: Testing 5.04: Same string");

    /* Change one character in str1 from alphabet to number */
    str1[len/2] = '1';
    /* Compare str2 with str1 (exclude all from the changed character to end) by calling function _io_strncasecmp */
    result = _io_strncasecmp(str2, str1, len/2);
    /* Check the returned result must be 0 to verify two strings are the same */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == 0, "TEST #5: Testing 5.05: Different strings");

    /* Compare str1 with str2 (include the changed character) by calling function _io_strncasecmp */
    result = _io_strncasecmp(str1, str2, len/2 + 1);
    /* Check the returned result must be smaller than 0 to verify different strings */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result < 0, "TEST #5: Testing 5.06: Different strings");

    /* Compare str2 with str1 (include the changed character) by calling function _io_strncasecmp */
    result = _io_strncasecmp(str2, str1, len/2 + 1);
    /* Check the returned result must be bigger than 0 to verify different strings */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result > 0, "TEST #5: Testing 5.07: Different strings");

    /* Make the str1 to be shorter than str2 by changing the numeric character to NULL */
    str1[len/2] = 0;
    /* Compare str2 with str1 (exclude all from the changed character to end) by calling function _io_strncasecmp */
    result = _io_strncasecmp(str2, str1, len/2);
    /* Check the returned result must be 0 to verify two strings are the same */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == 0, "TEST #5: Testing 5.08: Different strings");

    /* Compare str1 with str2 (include the changed character) by calling function _io_strncasecmp */
    result = _io_strncasecmp(str1, str2, len/2 + 1);
    /* Check the returned result must be smaller than 0 to verify different strings */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result < 0, "TEST #5: Testing 5.09: Different strings");

    /* Compare str2 with str1 (include the changed character) by calling function _io_strncasecmp */
    result = _io_strncasecmp(str2, str1, len/2 + 1);
    /* Check the returned result must be bigger than 0 to verify different strings */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result > 0, "TEST #5: Testing 5.10: Different strings");

    /* Free the allocated memory block str1 by calling _mem_free. Check the returned value must be MQX_OK */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, _mem_free(str1) == MQX_OK, "TEST #5: Testing 5.11: Deallocate memory of str1");

    /* Free the allocated memory block str2 by calling _mem_free. Check the returned value must be MQX_OK */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, _mem_free(str2) == MQX_OK, "TEST #5: Testing 5.12: Deallocate memory of str2");
}


/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_6_main_task
* Comments     : TEST #6:
*   This test case is to verify function _io_modf in the cases of:
*   - Getting the fractional part of positive value.
*   - Getting the fractional part of negative value.
*   - Getting the fractional part of zero value.
* Requirements:
*   FIO074
*
*END*---------------------------------------------------------------------*/
void tc_6_main_task(void)
{
    double int_part;
    double frac_part;

    /* Get the fractional part of positive double value */
    frac_part = _io_modf(1.23456, &int_part);
    /* Verify the fractional part must be correct */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, comp_double(frac_part, 0.23456, TEST_EPS), "TEST #6: Testing 6.01: Verify the fractional part");
    /* Verify the integral part must be correct */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, comp_double(int_part, 1.0, TEST_EPS), "TEST #6: Testing 6.02: Verify the integral part");

    /* Get the fractional part of negative double value */
    frac_part = _io_modf(-1.23456, &int_part);
    /* Verify the fractional part must be correct */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, comp_double(frac_part, -0.23456, TEST_EPS), "TEST #6: Testing 6.03: Verify the fractional part");
    /* Verify the integral part must be correct */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, comp_double(int_part, -1.0, TEST_EPS), "TEST #6: Testing 6.04: Verify the integral part");

    /* Get the fractional part of zero value */
    frac_part = _io_modf(0.0, &int_part);
    /* Verify the fractional part must be correct */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, comp_double(frac_part, 0.0, TEST_EPS), "TEST #6: Testing 6.05: Verify the fractional part");
    /* Verify the integral part must be correct */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, comp_double(int_part, 0.0, TEST_EPS), "TEST #6: Testing 6.06: Verify the integral part");
}


/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_7_main_task
* Comments     : TEST #7:
*   This test case is to verify function _io_feof in the cases of:
*   - Checking EOF of invalid device.
*   - Verifying EOF has not been reached on device.
*   - Verifying EOF has been reached on device.
* Requirements:
*   FIO003, FIO034.
*
*END*---------------------------------------------------------------------*/
void tc_7_main_task(void)
{
    MQX_FILE_PTR fake_fp;
    _mqx_int result;

    /* Open fake device by calling function _io_fopen */
    fake_fp = _io_fopen(FAKE_DEV, NULL);
    /* Check the returned pointer, which is stored in fake_fp, must be (NOT NULL) to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, fake_fp != NULL, "TEST #7: Testing 7.01: Open fake device");

    /* Determine if EOF has been reached on invalid device (NULL) by calling function _io_fopen */
    result = _io_feof(NULL);
    /* Check the returned result must be (IO_ERROR) to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == IO_ERROR, "TEST #7: Testing 7.02: Failed to determine EOF of invalid device");

    /* Determine if EOF has been reached on fake device by calling function _io_fopen */
    result = _io_feof(fake_fp);
    /* Check the returned result must be (0) to verify EOF has not been reached */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == 0, "TEST #7: Testing 7.03: Verify EOF has not been reached on fake device");

    /* Suppose the flag EOF of fake device is set */
    fake_fp->FLAGS |= IO_FLAG_AT_EOF;
    /* Determine if EOF has been reached on fake device by calling function _io_fopen */
    result = _io_feof(fake_fp);
    /* Check the returned result must be (1) to verify EOF has already been reached */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == 1, "TEST #7: Testing 7.04: Verify EOF has already been reached on fake device");

    /* Close fake device by calling function _io_close */
    result = _io_fclose(fake_fp);
    /* Check the returned result must be MQX_OK to verify closing successfully */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "TEST #7: Testing 7.05: Close fake device");
}


/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_8_main_task
* Comments     : TEST #8:
*   This test case is to verify function _io_fseek in the cases of:
*   - Seeking on invalid device.
*   - Seeking on specified device with invalid offset.
*   - Seeking on specified device with invalid mode.
* Requirements:
*   FIO018, FIO041
*
*END*---------------------------------------------------------------------*/
void tc_8_main_task(void)
{
    MQX_FILE_PTR fake_fp;
    _mqx_int result;

    /* Open fake device by calling function _io_fopen */
    fake_fp = _io_fopen(FAKE_DEV, NULL);
    /* Check the returned pointer, which is stored in fake_fp, must be (NOT NULL) to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, fake_fp != NULL, "TEST #8: Testing 8.01: Open fake device");

    /* Call function _io_seek to seek position on an invalid device (NULL) */
    result = _io_fseek(NULL, 0, IO_SEEK_SET);
    /* Check the returned result must be (IO_ERROR) to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == IO_ERROR, "TEST #8: Testing 8.02: Failed to seek on invalid device");

    /* Call function _io_fseek to seek position on fake device handler with an invalid offset using mode IO_SEEK_SET */
    result = _io_fseek(fake_fp, INVALID_OFFSET, IO_SEEK_SET);
    /* Check the returned result must be (IO_ERROR) to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == IO_ERROR, "TEST #8: Testing 8.03: Failed to seek with invalid offset");

    /* Call function _io_fseek to seek position on fake device handler with an invalid offset using mode IO_SEEK_CUR */
    result = _io_fseek(fake_fp, INVALID_OFFSET, IO_SEEK_CUR);
    /* Check the returned result must be (IO_ERROR) to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == IO_ERROR, "TEST #8: Testing 8.04: Failed to seek with invalid offset");

    /* Call function _io_fseek to seek position on fake device handler with an invalid offset using mode IO_SEEK_END */
    result = _io_fseek(fake_fp, INVALID_OFFSET, IO_SEEK_END);
    /* Check the returned result must be (IO_ERROR) to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == IO_ERROR, "TEST #8: Testing 8.05: Failed to seek with invalid offset");

    /* Call function _io_fseek to seek position on fake device handler using invalid mode */
    result = _io_fseek(fake_fp, 0, MAX_MQX_INT);
    /* Check the returned result must be (IO_ERROR) to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == IO_ERROR, "TEST #8: Testing 8.06: Failed to seek using invalid mode");

    /* Close fake device by calling function _io_close */
    result = _io_fclose(fake_fp);
    /* Check the returned result must be MQX_OK to verify closing successfully */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "TEST #8: Testing 8.07: Close fake device");
}


/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_9_main_task
* Comments     : TEST #9:
*   This test case is to verify function _io_fflush in the cases of:
*   - Flushing invalid device.
*   - Flushing specified device with error occurs.
* Requirements:
*   FIO005, FIO036
*
*END*---------------------------------------------------------------------*/
void tc_9_main_task(void)
{
    MQX_FILE_PTR fake_fp;
    _mqx_int result;

    /* Open fake device by calling function _io_fopen */
    fake_fp = _io_fopen(FAKE_DEV, NULL);
    /* Check the returned pointer, which is stored in fake_fp, must be (NOT NULL) to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, fake_fp != NULL, "TEST #9: Testing 9.01: Open fake device");

    /* Try to flush all buffer data of an invalid device by calling function _io_fflush */
    result = _io_fflush(NULL);
    /* Check the returned result must be (IO_ERROR) to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == IO_ERROR, "TEST #9: Testing 9.02: Failed to flush invalid device");

    /* Try to flush all buffer data of fake device by calling function _io_fflush */
    result = _io_fflush(fake_fp);
    /* Check the returned result must be (IO_ERROR) to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == IO_ERROR, "TEST #9: Testing 9.03: Flush fake device with error");

    /* Close fake device by calling function _io_close */
    result = _io_fclose(fake_fp);
    /* Check the returned result must be MQX_OK to verify closing successfully */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "TEST #9: Testing 9.04: Close fake device");
}


/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_10_main_task
* Comments     : TEST #10:
*   This test case is to verify function _io_ftell in the cases of:
*   - Getting current position of an invalid device.
* Requirements:
*   FIO042
*
*END*---------------------------------------------------------------------*/
void tc_10_main_task(void)
{
    _mqx_int result;

    /* Try to get current position of an invalid device handler by calling function _io_ftell */
    result = _io_ftell(NULL);
    /* Check the returned result must be (IO_ERROR) to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == IO_ERROR, "TEST #10: Testing 10.01: Failed to get current position of invalid device");
}


/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_11_main_task
* Comments     : TEST #11:
*   This test case is to verify function _io_ferror in the cases of:
*   - Geting error code of an invalid device
* Requirements:
*   FIO035
*
*END*---------------------------------------------------------------------*/
void tc_11_main_task(void)
{
    _mqx_int result;

    /* Try to get error code of an invalid device handler by calling function _io_ferror */
    result = _io_ferror(NULL);
    /* Check the returned result must be (IO_ERROR) to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, result == IO_ERROR, "TEST #11: Testing 11.01: Failed to get error code of invalid device");
}


/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_12_main_task
* Comments     : TEST #12:
*   This test case is to verify function _io_clearerr in the cases of:
*   - Clear error indicator of specified device handler properly.
* Requirements:
*   FIO001
*
*END*---------------------------------------------------------------------*/
void tc_12_main_task(void)
{
    MQX_FILE_PTR fake_fp;
    _mqx_int result;

    /* Open fake device by calling function _io_fopen */
    fake_fp = _io_fopen(FAKE_DEV, NULL);
    /* Check the returned pointer, which is stored in fake_fp, must be (NOT NULL) to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_12_main_task, fake_fp != NULL, "TEST #12: Testing 12.01: Open fake device");

    /* Suppose the fake device handler had error code and reached EOF */
    fake_fp->ERROR = IO_ERROR;
    fake_fp->FLAGS |= IO_FLAG_AT_EOF;

    /* Clear error code of fake device handler by calling function _io_clearerr */
    _io_clearerr(fake_fp);
    /* Check the error code of fake device must be (MQX_OK) to verify error has been cleared */
    EU_ASSERT_REF_TC_MSG(tc_12_main_task, fake_fp->ERROR == MQX_OK, "TEST #12: Testing 12.03: Clear error code");
    /* Check the EOF flag of fake device must be cleared to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_12_main_task, !(fake_fp->FLAGS & IO_FLAG_AT_EOF), "TEST #12: Testing 12.04: Clear EOF flag");

    /* Close fake device by calling function _io_close */
    result = _io_fclose(fake_fp);
    /* Check the returned result must be (MQX_OK) to verify closing successfully */
    EU_ASSERT_REF_TC_MSG(tc_12_main_task, result == MQX_OK, "TEST #12: Testing 12.05: Close fake device");
}


/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_13_main_task
* Comments     : TEST #13:
*   This test case is to verify function _io_fclose in the cases of:
*   - Closing an invalid device.
*   - Closing specified device when its I/O function is unavailable.
* Requirements:
*   FIO033
*
*END*---------------------------------------------------------------------*/
void tc_13_main_task(void)
{
    MQX_FILE_PTR fake_fp;
    _mqx_int result;

    /* Open fake device by calling function _io_fopen */
    fake_fp = _io_fopen(FAKE_DEV, NULL);
    /* Check the returned pointer, which is stored in fake_fp, must be (NOT NULL) to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_13_main_task, fake_fp != NULL, "TEST #13: Testing 13.01: Open fake device");

    /* Try to close an invalid device by calling function _io_fclose */
    result = _io_fclose(NULL);
    /* Check the returned result must be (IO_ERROR) to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_13_main_task, result == IO_ERROR, "TEST #13: Testing 13.02: Failed to close an invalid device");

    /* Make the function I/O CLOSE of fake device to be invalid */
    fake_fp->DEV_PTR->IO_CLOSE = NULL;
    /* Try to close an fake device when its CLOSE function is invalid by calling function _io_fclose */
    result = _io_fclose(fake_fp);
    /* Check the returned result must be (IO_ERROR) to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_13_main_task, result == IO_ERROR, "TEST #13: Testing 13.03: Failed to close device if it doesn't have I/O CLOSE");
    /* The file handler fake_fp was deallocated although error's returned, then we don't need to close again */
}


/*------------------------------------------------------------------------
** Define Test Suite
*/
EU_TEST_SUITE_BEGIN(suite_misc1)
    EU_TEST_CASE_ADD(tc_1_main_task,  "TEST #1 - Testing function _io_atoi"),
    EU_TEST_CASE_ADD(tc_2_main_task,  "TEST #2 - Testing function _io_strtod"),
    EU_TEST_CASE_ADD(tc_3_main_task,  "TEST #3 - Testing function _io_strtolower"),
    EU_TEST_CASE_ADD(tc_4_main_task,  "TEST #4 - Testing function _io_strcasecmp"),
    EU_TEST_CASE_ADD(tc_5_main_task,  "TEST #5 - Testing function _io_strncasecmp"),
    EU_TEST_CASE_ADD(tc_6_main_task,  "TEST #6 - Testing function _io_modf"),
    EU_TEST_CASE_ADD(tc_7_main_task,  "TEST #7 - Testing function _io_feof"),
    EU_TEST_CASE_ADD(tc_8_main_task,  "TEST #8 - Testing function _io_fseek"),
    EU_TEST_CASE_ADD(tc_9_main_task,  "TEST #9 - Testing function _io_fflush"),
    EU_TEST_CASE_ADD(tc_10_main_task, "TEST #10 - Testing function _io_ftell"),
    EU_TEST_CASE_ADD(tc_11_main_task, "TEST #11 - Testing function _io_ferror"),
    EU_TEST_CASE_ADD(tc_12_main_task, "TEST #12 - Testing function _io_clearerr"),
    EU_TEST_CASE_ADD(tc_13_main_task, "TEST #13 - Testing function _io_fclose"),
EU_TEST_SUITE_END(suite_misc1)
/* Add test suites */
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_misc1, "MQX Standard I/O Component Test Suite, 13 Tests")
EU_TEST_REGISTRY_END()
/*------------------------------------------------------------------------*/

/*TASK*-------------------------------------------------------------------
*
* Task Name    : main_task
* Comments     :
*   This task creates all defined test tasks and lets them run continuously.
*
*END*----------------------------------------------------------------------*/
void main_task(uint32_t param)
{
    _mqx_uint result;

    _int_install_unexpected_isr();

    /* Install fake device's interface */
    result = _io_fake_install(FAKE_DEV);
    /* If anything's not OK, stop the test */
    if(result != MQX_OK)
    {
        _mqx_exit(0);
    }

    /* Create test tasks */
    eunit_mutex_init();
    EU_RUN_ALL_TESTS(eunit_tres_array);
    eunit_test_stop();
}

/*FUNCTION*-------------------------------------------------------------------
*
* Task Name        : comp_double
* Returned Value   : bool (TRUE for two numbers of equal value, FALSE for opposite)
* Comments         :
*   Compare two floating point number with expected error value.
*   - [IN] d1: Double value 1
*   - [IN] d2: Double value 2
*   - [IN] eps: The error allowed.
*
*END*----------------------------------------------------------------------*/
bool comp_double(double d1, double d2, double eps)
{
    double diff, epsilon;

    epsilon = fabs((d2)? d2 : MIN_DOUBLE);
    epsilon *= eps;

    diff = fabs(d1 - d2);

    return diff <= epsilon;

}
