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
* $Date    : Jul-28-2013$
*
* Comments:
*
*   This file contains code for the MQX Standard I/O verification program,
*   includes all remaining test cases, which haven't been verified yet,
*   of following functions:
*   _io_fprintf, _io_vfprintf, _io_vprintf, _io_vsnprintf, _io_vsprintf,
*   _io_fscanf, _io_scanf.
*
* Requirements:
*
*   FIO014, FIO017, FIO024, FIO029, FIO030, FIO037, FIO040, FIO046, FIO053,
*   FIO054, FIO055, FIO056.
*
*END************************************************************************/

#include <mqx_inc.h>
#include <bsp.h>
#include <fio.h>
#include "test.h"
#include "fake_drv.h"
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"

/*------------------------------------------------------------------------
** Test suite function prototype
*/
void tc_1_main_task(void);  /* TEST #1 - Testing function _io_fprintf */
void tc_2_main_task(void);  /* TEST #2 - Testing function _io_vfprintf */
void tc_3_main_task(void);  /* TEST #3 - Testing function _io_vprintf */
void tc_4_main_task(void);  /* TEST #4 - Testing function _io_vsnprintf */
void tc_5_main_task(void);  /* TEST #5 - Testing function _io_vsprintf */
void tc_6_main_task(void);  /* TEST #6 - Testing function _io_fscanf */
void tc_7_main_task(void);  /* TEST #7 - Testing function _io_scanf */

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
*   This test case is to verify function _io_fprintf in the cases of:
*   - Printing different formats of string to specifed device properly.
*   - Printing to specifed device when its I/O function is unavailable.
* Requirements:
*   FIO014, FIO037
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    MQX_FILE_PTR fake_fp;
    void *bak_ptr;
    char *prefix, *buf_ptr;
    _mqx_int i, j, k, l;
    _mqx_int index, result;
    char tp[LEN], str[LEN];

    /* Open fake device, the returned file pointer is stored in fake_fp */
    fake_fp = _io_fopen(FAKE_DEV, NULL);
    /* Check the returned pointer must be NOT NULL to verify successfully opening */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, fake_fp != NULL, "TEST #1: Testing 1.01: Open fake device");

    /* Get the output buffer pointer of fake device and store in buf_ptr */
    _io_ioctl(fake_fp, IO_IOCTL_GET_OUT_BUFFER, &buf_ptr);

    /* Backup the function I/O WRITE of device, save address to bak_ptr */
    bak_ptr = (void *)fake_fp->DEV_PTR->IO_WRITE;
    /* Change the function I/O WRITE of device to invalid function */
    fake_fp->DEV_PTR->IO_WRITE = NULL;
    /* Try to print to device when its I/O WRITE is invalid by calling function _io_fprintf */
    result = _io_fprintf(fake_fp, "Test string\0");
    /* Check the returned character must be IO_EOF to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == IO_EOF, "TEST #1: Testing 1.02: Failed to put character to the device whose I/O WRITE function is invalid");
    /* Restore function I/O WRITE of device to original address which is currently stored in bak_ptr */
    fake_fp->DEV_PTR->IO_WRITE = (IO_WRITE_FPTR)bak_ptr;

    index = 0;
    prefix = str;

    strcpy(prefix, "%");
    for (i = 0; i < 2; ++i)
    {
        for (j = 0; j < 2; ++j)
        {
            for (k = 0; k < 2; ++k)
            {
                for (l = 0; l < 2; ++l)
                {
                    if(i == 0)
                    {
                       strcat(prefix, "-");
                    }
                    if(j == 0)
                    {
                       strcat(prefix, "+");
                    }
                    if(k == 0)
                    {
                       strcat(prefix, "#");
                    }
                    if (l == 0)
                    {
                       strcat(prefix, "0");
                    }

                    /*
                    ** Calling the function _io_fprintf to print prefix to device.
                    ** Compares output string with expected string to verify the operation of _io_fprintf
                    */
                    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
                    _io_fprintf(fake_fp, "%5s |", prefix);
                    EU_ASSERT_REF_TC_MSG(tc_1_main_task, !strcmp(buf_ptr, print_arr[index++]), "TEST #1: Testing 1.03: Test printing prefix");

                    /*
                    ** Calling the function _io_fprintf to print an decimal integer with varied prefixes to device.
                    ** Compares output string with expected string to verify the operation of _io_fprintf
                    */
                    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
                    strcpy(tp, prefix);
                    strcat(tp, "6d |");
                    _io_fprintf(fake_fp, tp, I);
                    EU_ASSERT_REF_TC_MSG(tc_1_main_task, !strcmp(buf_ptr, print_arr[index++]), "TEST #1: Testing 1.04: Test printing decimal integer");

                    /*
                    ** Calling the function _io_fprintf to print an octal integer with varied prefixes to device.
                    ** Compares output string with expected string to verify the operation of _io_fprintf.
                    */
                    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
                    strcpy(tp, prefix);
                    strcat(tp, "6o |");
                    _io_fprintf(fake_fp, tp, I);
                    EU_ASSERT_REF_TC_MSG(tc_1_main_task, !strcmp(buf_ptr, print_arr[index++]), "TEST #1: Testing 1.05: Test printing octal");

                    /*
                    ** Calling the function _io_fprintf to print an hexadecimal integer with varied prefixes to device.
                    ** Compares output string with expected string to verify the operation of _io_fprintf.
                    */
                    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
                    strcpy(tp, prefix);
                    strcat(tp, "8x |");
                    _io_fprintf(fake_fp, tp, I);
                    EU_ASSERT_REF_TC_MSG(tc_1_main_task, !strcmp(buf_ptr, print_arr[index++]), "TEST #1: Testing 1.06: Test printing hexadecimal integer");

                    /*
                    ** Calling the function _io_fprintf to print an float using e character with varied prefixes to device.
                    ** Compares output string with expected string to verify the operation of _io_fprintf.
                    */
                    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
                    strcpy(tp, prefix);
                    strcat(tp, "10.2e |");
                    _io_fprintf(fake_fp, tp, R);
                    EU_ASSERT_REF_TC_MSG(tc_1_main_task, !strcmp(buf_ptr, print_arr[index++]), "TEST #1: Testing 1.07: Test printing using e character");

                    /*
                    ** Calling the function _io_fprintf to print an decimal floating point with varied prefixes to device.
                    ** Compares output string with expected string to verify the operation of _io_fprintf.
                    */
                    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
                    strcpy(tp, prefix);
                    strcat(tp, "10.2f |");
                    _io_fprintf(fake_fp, tp, R);
                    EU_ASSERT_REF_TC_MSG(tc_1_main_task, !strcmp(buf_ptr, print_arr[index++]), "TEST #1: Testing 1.08: Test printing decimal floating point");

                    strcpy(prefix, "%");
                }
            }
        }
    }

    /* Close the fake device whose file handler is stored in fake_fp by calling function _io_fclose */
    result = _io_fclose(fake_fp);
    /* Check the returned result must be MQX_OK to verify successfully closing */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "TEST #1: Testing 1.09: Close fake device");
}


/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2:
*   This test case is to verify function _io_vfprintf in the cases of:
*   - Printing different formats of string to specifed device properly.
*   - Printing to specifed device when its I/O function is unavailable.
* Requirements:
*   FIO055, FIO056
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    MQX_FILE_PTR fake_fp;
    void *bak_ptr;
    char *prefix, *buf_ptr;
    _mqx_int i, j, k, l;
    _mqx_int index, result;
    char tp[LEN], str[LEN];

    /* Open fake device, the returned file pointer is stored in fake_fp */
    fake_fp = _io_fopen(FAKE_DEV, NULL);
    /* Check the returned pointer must be NOT NULL to verify successfully opening */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, fake_fp != NULL, "TEST #2: Testing 2.01: Open fake device");

    /* Get the output buffer pointer of fake device and store in buf_ptr */
    _io_ioctl(fake_fp, IO_IOCTL_GET_OUT_BUFFER, &buf_ptr);

    /* Backup the function I/O WRITE of device, save address to bak_ptr */
    bak_ptr = (void *)fake_fp->DEV_PTR->IO_WRITE;
    /* Change the function I/O WRITE of device to invalid function */
    fake_fp->DEV_PTR->IO_WRITE = NULL;
    /* Try to print to device when its I/O WRITE is invalid by calling function _io_vfprintf */
    result = test_vfprintf(fake_fp, "Test string\0");
    /* Check the returned character must be IO_EOF to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == IO_EOF, "TEST #2: Testing 2.02: Failed to put character to the device whose I/O WRITE function is invalid");
    /* Restore function I/O WRITE of device to original address which is currently stored in bak_ptr */
    fake_fp->DEV_PTR->IO_WRITE = (IO_WRITE_FPTR)bak_ptr;

    index = 0;
    prefix = str;

    strcpy(prefix, "%");
    for (i = 0; i < 2; ++i)
    {
        for (j = 0; j < 2; ++j)
        {
            for (k = 0; k < 2; ++k)
            {
                for (l = 0; l < 2; ++l)
                {
                    if(i == 0)
                    {
                       strcat(prefix, "-");
                    }
                    if(j == 0)
                    {
                       strcat(prefix, "+");
                    }
                    if(k == 0)
                    {
                       strcat(prefix, "#");
                    }
                    if (l == 0)
                    {
                       strcat(prefix, "0");
                    }

                    /*
                    ** Calling the function _io_vfprintf to print prefix to device.
                    ** Compares output string with expected string to verify the operation of _io_vfprintf
                    */
                    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
                    test_vfprintf(fake_fp, "%5s |", prefix);
                    EU_ASSERT_REF_TC_MSG(tc_2_main_task, !strcmp(buf_ptr, print_arr[index++]), "TEST #2: Testing 2.03: Test printing prefix");

                    /*
                    ** Calling the function _io_vfprintf to print an decimal integer with varied prefixes to device.
                    ** Compares output string with expected string to verify the operation of _io_vfprintf
                    */
                    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
                    strcpy(tp, prefix);
                    strcat(tp, "6d |");
                    test_vfprintf(fake_fp, tp, I);
                    EU_ASSERT_REF_TC_MSG(tc_2_main_task, !strcmp(buf_ptr, print_arr[index++]), "TEST #2: Testing 2.04: Test printing decimal integer");

                    /*
                    ** Calling the function _io_vfprintf to print an octal integer with varied prefixes to device.
                    ** Compares output string with expected string to verify the operation of _io_vfprintf.
                    */
                    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
                    strcpy(tp, prefix);
                    strcat(tp, "6o |");
                    test_vfprintf(fake_fp, tp, I);
                    EU_ASSERT_REF_TC_MSG(tc_2_main_task, !strcmp(buf_ptr, print_arr[index++]), "TEST #2: Testing 2.05: Test printing octal");

                    /*
                    ** Calling the function _io_vfprintf to print an hexadecimal integer with varied prefixes to device.
                    ** Compares output string with expected string to verify the operation of _io_vfprintf.
                    */
                    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
                    strcpy(tp, prefix);
                    strcat(tp, "8x |");
                    test_vfprintf(fake_fp, tp, I);
                    EU_ASSERT_REF_TC_MSG(tc_2_main_task, !strcmp(buf_ptr, print_arr[index++]), "TEST #2: Testing 2.06: Test printing hexadecimal integer");

                    /*
                    ** Calling the function _io_vfprintf to print an float using e character with varied prefixes to device.
                    ** Compares output string with expected string to verify the operation of _io_vfprintf.
                    */
                    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
                    strcpy(tp, prefix);
                    strcat(tp, "10.2e |");
                    test_vfprintf(fake_fp, tp, R);
                    EU_ASSERT_REF_TC_MSG(tc_2_main_task, !strcmp(buf_ptr, print_arr[index++]), "TEST #2: Testing 2.07: Test printing using e character");

                    /*
                    ** Calling the function _io_vfprintf to print an decimal floating point with varied prefixes to device.
                    ** Compares output string with expected string to verify the operation of _io_vfprintf.
                    */
                    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
                    strcpy(tp, prefix);
                    strcat(tp, "10.2f |");
                    test_vfprintf(fake_fp, tp, R);
                    EU_ASSERT_REF_TC_MSG(tc_2_main_task, !strcmp(buf_ptr, print_arr[index++]), "TEST #2: Testing 2.08: Test printing decimal floating point");

                    strcpy(prefix, "%");
                }
            }
        }
    }

    /* Close the fake device whose file handler is stored in fake_fp by calling function _io_fclose */
    result = _io_fclose(fake_fp);
    /* Check the returned result must be MQX_OK to verify successfully closing */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "TEST #2: Testing 2.09: Close fake device");
}


/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : TEST #3:
*   This test case is to verify function _io_vprintf in the cases of:
*   - Printing different formats of string to stdout properly.
*   - Printing to stdout when its I/O function is unavailable.
* Requirements:
*   FIO053, FIO054
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
    MQX_FILE_PTR fake_fp, stdout_fp;
    void *bak_ptr;
    char *prefix, *buf_ptr;
    _mqx_int i, j, k, l;
    _mqx_int index, result;
    char tp[LEN], str[LEN];

    /* Open fake device, the returned file pointer is stored in fake_fp */
    fake_fp = _io_fopen(FAKE_DEV, NULL);
    /* Check the returned pointer must be NOT NULL to verify successfully opening */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, fake_fp != NULL, "TEST #3: Testing 3.01: Open fake device");

    /* Get the STDOUT file handler, the returned pointer is stored in stdout_fp */
    stdout_fp = (MQX_FILE_PTR)stdout;
    /* Check the returned pointer must be NOT NULL to verify valid stdout */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, stdout_fp != NULL, "TEST #3: Testing 3.02: Get the stdout device");

    /* Get the output buffer pointer of fake device and store in buf_ptr */
    _io_ioctl(fake_fp, IO_IOCTL_GET_OUT_BUFFER, &buf_ptr);

    /* Backup the function I/O WRITE of device, save address to bak_ptr */
    bak_ptr = (void *)stdout_fp->DEV_PTR->IO_WRITE;
    /* Change the function I/O WRITE of device to invalid function */
    stdout_fp->DEV_PTR->IO_WRITE = NULL;
    /* Try to print to device when its I/O WRITE is invalid by calling function _io_vprintf */
    result = test_vfprintf(NULL, "Test string\0");
    /* Check the returned character must be IO_EOF to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == IO_EOF, "TEST #3: Testing 3.03: Failed to put character to the device whose I/O WRITE function is invalid");
    /* Restore function I/O WRITE of device to original address which is currently stored in bak_ptr */
    stdout_fp->DEV_PTR->IO_WRITE = (IO_WRITE_FPTR)bak_ptr;

    /* Back up the default device of STDOUT (stdout_fp->DEV_PTR), save address to bak_ptr */
    bak_ptr = (void *)stdout_fp->DEV_PTR;
    /* Change the default device of STDOUT to fake device (fake_fp->DEV_PTR) */
    stdout_fp->DEV_PTR = fake_fp->DEV_PTR;

    index = 0;
    prefix = str;

    strcpy(prefix, "%");
    for (i = 0; i < 2; ++i)
    {
        for (j = 0; j < 2; ++j)
        {
            for (k = 0; k < 2; ++k)
            {
                for (l = 0; l < 2; ++l)
                {
                    if(i == 0)
                    {
                       strcat(prefix, "-");
                    }
                    if(j == 0)
                    {
                       strcat(prefix, "+");
                    }
                    if(k == 0)
                    {
                       strcat(prefix, "#");
                    }
                    if (l == 0)
                    {
                       strcat(prefix, "0");
                    }

                    /*
                    ** Calling the function _io_vprintf to print prefix to device.
                    ** Compares output string with expected string to verify the operation of _io_vprintf
                    */
                    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
                    test_vfprintf(NULL, "%5s |", prefix);
                    EU_ASSERT_REF_TC_MSG(tc_3_main_task, !strcmp(buf_ptr, print_arr[index++]), "TEST #3: Testing 3.04: Test printing prefix");

                    /*
                    ** Calling the function _io_vprintf to print an decimal integer with varied prefixes to device.
                    ** Compares output string with expected string to verify the operation of _io_vprintf
                    */
                    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
                    strcpy(tp, prefix);
                    strcat(tp, "6d |");
                    test_vfprintf(NULL, tp, I);
                    EU_ASSERT_REF_TC_MSG(tc_3_main_task, !strcmp(buf_ptr, print_arr[index++]), "TEST #3: Testing 3.05: Test printing decimal integer");

                    /*
                    ** Calling the function _io_vprintf to print an octal integer with varied prefixes to device.
                    ** Compares output string with expected string to verify the operation of _io_vprintf.
                    */
                    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
                    strcpy(tp, prefix);
                    strcat(tp, "6o |");
                    test_vfprintf(NULL, tp, I);
                    EU_ASSERT_REF_TC_MSG(tc_3_main_task, !strcmp(buf_ptr, print_arr[index++]), "TEST #3: Testing 3.06: Test printing octal");

                    /*
                    ** Calling the function _io_vprintf to print an hexadecimal integer with varied prefixes to device.
                    ** Compares output string with expected string to verify the operation of _io_vprintf.
                    */
                    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
                    strcpy(tp, prefix);
                    strcat(tp, "8x |");
                    test_vfprintf(NULL, tp, I);
                    EU_ASSERT_REF_TC_MSG(tc_3_main_task, !strcmp(buf_ptr, print_arr[index++]), "TEST #3: Testing 3.07: Test printing hexadecimal integer");

                    /*
                    ** Calling the function _io_vprintf to print an float using e character with varied prefixes to device.
                    ** Compares output string with expected string to verify the operation of _io_vprintf.
                    */
                    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
                    strcpy(tp, prefix);
                    strcat(tp, "10.2e |");
                    test_vfprintf(NULL, tp, R);
                    EU_ASSERT_REF_TC_MSG(tc_3_main_task, !strcmp(buf_ptr, print_arr[index++]), "TEST #3: Testing 3.08: Test printing using e character");

                    /*
                    ** Calling the function _io_vprintf to print an decimal floating point with varied prefixes to device.
                    ** Compares output string with expected string to verify the operation of _io_vprintf.
                    */
                    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
                    strcpy(tp, prefix);
                    strcat(tp, "10.2f |");
                    test_vfprintf(NULL, tp, R);
                    EU_ASSERT_REF_TC_MSG(tc_3_main_task, !strcmp(buf_ptr, print_arr[index++]), "TEST #3: Testing 3.09: Test printing decimal floating point");

                    strcpy(prefix, "%");
                }
            }
        }
    }

    /* Restore the default device of STDOUT to original address which is currently stored in bak_ptr */
    stdout_fp->DEV_PTR = (IO_DEVICE_STRUCT_PTR)bak_ptr;

    /* Close the fake device whose file handler is stored in fake_fp by calling function _io_fclose */
    result = _io_fclose(fake_fp);
    /* Check the returned result must be MQX_OK to verify successfully closing */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "TEST #3: Testing 3.10: Close fake device");
}


/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_4_main_task
* Comments     : TEST #4:
*   This test case is to verify function _io_vsnprintf in the cases of:
*   - Printing different formats of string to specifed string properly.
* Requirements:
*   FIO030
*
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
    char *prefix;
    _mqx_int i, j, k, l;
    _mqx_int index;
    char tp[LEN], str[LEN];
    char buff[LEN] = {0};

    index = 0;
    prefix = str;

    strcpy(prefix, "%");
    for (i = 0; i < 2; ++i)
    {
        for (j = 0; j < 2; ++j)
        {
            for (k = 0; k < 2; ++k)
            {
                for (l = 0; l < 2; ++l)
                {
                    if(i == 0)
                    {
                       strcat(prefix, "-");
                    }
                    if(j == 0)
                    {
                       strcat(prefix, "+");
                    }
                    if(k == 0)
                    {
                       strcat(prefix, "#");
                    }
                    if (l == 0)
                    {
                       strcat(prefix, "0");
                    }

                    /*
                    ** Calling the function _io_vsnprintf to print prefix to buff.
                    ** Compares buff with expected string to verify the operation of _io_vsnprintf
                    */
                    test_vsnprintf(buff, LEN, "%5s |", prefix);
                    EU_ASSERT_REF_TC_MSG(tc_4_main_task, !strcmp(buff, print_arr[index++]), "TEST #4: Testing 4.01: Test printing prefix");

                    /*
                    ** Calling the function _io_vsnprintf to print an decimal integer with varied prefixes to buff.
                    ** Compares buff with expected string to verify the operation of _io_vsnprintf
                    */
                    strcpy(tp, prefix);
                    strcat(tp, "6d |");
                    test_vsnprintf(buff, LEN, tp, I);
                    EU_ASSERT_REF_TC_MSG(tc_4_main_task, !strcmp(buff, print_arr[index++]), "TEST #4: Testing 4.02: Test printing decimal integer");

                    /*
                    ** Calling the function _io_vsnprintf to print an octal integer with varied prefixes to buff.
                    ** Compares buff with expected string to verify the operation of _io_vsnprintf.
                    */
                    strcpy(tp, prefix);
                    strcat(tp, "6o |");
                    test_vsnprintf(buff, LEN, tp, I);
                    EU_ASSERT_REF_TC_MSG(tc_4_main_task, !strcmp(buff, print_arr[index++]), "TEST #4: Testing 4.03: Test printing octal");

                    /*
                    ** Calling the function _io_vsnprintf to print an hexadecimal integer with varied prefixes to buff.
                    ** Compares buff with expected string to verify the operation of _io_vsnprintf.
                    */
                    strcpy(tp, prefix);
                    strcat(tp, "8x |");
                    test_vsnprintf(buff, LEN, tp, I);
                    EU_ASSERT_REF_TC_MSG(tc_4_main_task, !strcmp(buff, print_arr[index++]), "TEST #4: Testing 4.04: Test printing hexadecimal integer");

                    /*
                    ** Calling the function _io_vsnprintf to print an float using e character with varied prefixes to buff.
                    ** Compares buff with expected string to verify the operation of _io_vsnprintf.
                    */
                    strcpy(tp, prefix);
                    strcat(tp, "10.2e |");
                    test_vsnprintf(buff, LEN, tp, R);
                    EU_ASSERT_REF_TC_MSG(tc_4_main_task, !strcmp(buff, print_arr[index++]), "TEST #4: Testing 4.05: Test printing using e character");

                    /*
                    ** Calling the function _io_vsnprintf to print an decimal floating point with varied prefixes to buff.
                    ** Compares buff with expected string to verify the operation of _io_vsnprintf.
                    */
                    strcpy(tp, prefix);
                    strcat(tp, "10.2f |");
                    test_vsnprintf(buff, LEN, tp, R);
                    EU_ASSERT_REF_TC_MSG(tc_4_main_task, !strcmp(buff, print_arr[index++]), "TEST #4: Testing 4.06: Test printing decimal floating point");

                    strcpy(prefix, "%");
                }
            }
        }
    }
}


/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_5_main_task
* Comments     : TEST #5:
*   This test case is to verify function _io_vsprintf in the cases of:
*   - Printing different formats of string to specifed string properly.
* Requirements:
*   FIO029
*
*END*---------------------------------------------------------------------*/
void tc_5_main_task(void)
{
    char *prefix;
    _mqx_int i, j, k, l;
    _mqx_int index;
    char tp[LEN], str[LEN];
    char buff[LEN] = {0};

    index = 0;
    prefix = str;

    strcpy(prefix, "%");
    for (i = 0; i < 2; ++i)
    {
        for (j = 0; j < 2; ++j)
        {
            for (k = 0; k < 2; ++k)
            {
                for (l = 0; l < 2; ++l)
                {
                    if(i == 0)
                    {
                       strcat(prefix, "-");
                    }
                    if(j == 0)
                    {
                       strcat(prefix, "+");
                    }
                    if(k == 0)
                    {
                       strcat(prefix, "#");
                    }
                    if (l == 0)
                    {
                       strcat(prefix, "0");
                    }

                    /*
                    ** Calling the function _io_vsprintf to print prefix to buff.
                    ** Compares buff with expected string to verify the operation of _io_vsprintf
                    */
                    test_vsnprintf(buff, 0, "%5s |", prefix);
                    EU_ASSERT_REF_TC_MSG(tc_5_main_task, !strcmp(buff, print_arr[index++]), "TEST #5: Testing 5.01: Test printing prefix");

                    /*
                    ** Calling the function _io_vsprintf to print an decimal integer with varied prefixes to buff.
                    ** Compares buff with expected string to verify the operation of _io_vsprintf
                    */
                    strcpy(tp, prefix);
                    strcat(tp, "6d |");
                    test_vsnprintf(buff, 0, tp, I);
                    EU_ASSERT_REF_TC_MSG(tc_5_main_task, !strcmp(buff, print_arr[index++]), "TEST #5: Testing 5.02: Test printing decimal integer");

                    /*
                    ** Calling the function _io_vsprintf to print an octal integer with varied prefixes to buff.
                    ** Compares buff with expected string to verify the operation of _io_vsprintf.
                    */
                    strcpy(tp, prefix);
                    strcat(tp, "6o |");
                    test_vsnprintf(buff, 0, tp, I);
                    EU_ASSERT_REF_TC_MSG(tc_5_main_task, !strcmp(buff, print_arr[index++]), "TEST #5: Testing 5.03: Test printing octal");

                    /*
                    ** Calling the function _io_vsprintf to print an hexadecimal integer with varied prefixes to buff.
                    ** Compares buff with expected string to verify the operation of _io_vsprintf.
                    */
                    strcpy(tp, prefix);
                    strcat(tp, "8x |");
                    test_vsnprintf(buff, 0, tp, I);
                    EU_ASSERT_REF_TC_MSG(tc_5_main_task, !strcmp(buff, print_arr[index++]), "TEST #5: Testing 5.04: Test printing hexadecimal integer");

                    /*
                    ** Calling the function _io_vsprintf to print an float using e character with varied prefixes to buff.
                    ** Compares buff with expected string to verify the operation of _io_vsprintf.
                    */
                    strcpy(tp, prefix);
                    strcat(tp, "10.2e |");
                    test_vsnprintf(buff, 0, tp, R);
                    EU_ASSERT_REF_TC_MSG(tc_5_main_task, !strcmp(buff, print_arr[index++]), "TEST #5: Testing 5.05: Test printing using e character");

                    /*
                    ** Calling the function _io_vsprintf to print an decimal floating point with varied prefixes to buff.
                    ** Compares buff with expected string to verify the operation of _io_vsprintf.
                    */
                    strcpy(tp, prefix);
                    strcat(tp, "10.2f |");
                    test_vsnprintf(buff, 0, tp, R);
                    EU_ASSERT_REF_TC_MSG(tc_5_main_task, !strcmp(buff, print_arr[index++]), "TEST #5: Testing 5.06: Test printing decimal floating point");

                    strcpy(prefix, "%");
                }
            }
        }
    }
}


/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_6_main_task
* Comments     : TEST #6:
*   This test case is to verify function _io_fscanf in the cases of:
*   - Scanning different formats of string from specified device properly.
*   - Scanning empty string from specified device.
* Requirements:
*   FIO017, FIO040
*
*END*---------------------------------------------------------------------*/
void tc_6_main_task(void)
{
    MQX_FILE_PTR fake_fp;
    char *c_ptr, *buf_ptr;
    _mqx_uint i;
    _mqx_int result;
    bool bool_res;

    /* Open fake device, the returned file pointer is stored in fake_fp */
    fake_fp = _io_fopen(FAKE_DEV, NULL);
    /* Check the returned pointer must be NOT NULL to verify successfully opening */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, fake_fp != NULL, "TEST #6: Testing 6.01: Open fake device");

    /* Get the input buffer pointer of fake device and store in buf_ptr */
    _io_ioctl(fake_fp, IO_IOCTL_GET_IN_BUFFER, &buf_ptr);

    /* Test scanning with leading whitespace */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, " 123.456.789\n");
    bool_res = test_fscanf(fake_fp, "%u.%u.%u");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, bool_res, "TEST #6: Testing 6.02: Test scanning with leading whitespace");

    /* Test scanning with assignment suppression */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, " 123.456.789\n");
    bool_res = test_fscanf(fake_fp, "%*u.%u.%u");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, bool_res, "TEST #6: Testing 6.03: Test scanning with assignment suppression");

    /* Test scanning with assignment suppression */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, " 123.456.789\n");
    bool_res = test_fscanf(fake_fp, "%u.%*u.%u");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, bool_res, "TEST #6: Testing 6.04: Test scanning with assignment suppression");

    /* Test scanning with assignment suppression */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, " 123.456.789\n");
    bool_res = test_fscanf(fake_fp, "%*u.%*u.%u");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, bool_res, "TEST #6: Testing 6.05: Test scanning with assignment suppression");

    /* Test scanning with assignment suppression */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, " 123.456.789\n");
    bool_res = test_fscanf(fake_fp, "%u.%u.%*u");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, bool_res, "TEST #6: Testing 6.06: Test scanning with assignment suppression");

    /* Test scanning with assignment suppression */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, " 123.456.789\n");
    bool_res = test_fscanf(fake_fp, "%*u.%u.%*u");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, bool_res, "TEST #6: Testing 6.07: Test scanning with assignment suppression");

    /* Test scanning with assignment suppression */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, " 123.456.789\n");
    bool_res = test_fscanf(fake_fp, "%u.%*u.%*u");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, bool_res, "TEST #6: Testing 6.08: Test scanning with assignment suppression");

    /* Test scanning with assignment suppression */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, " 123.456.789\n");
    bool_res = test_fscanf(fake_fp, "%*u.%*u.%*u");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, bool_res, "TEST #6: Testing 6.09: Test scanning with assignment suppression");

    /* Test scanning with assignment suppression */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, " 123.456.789\n");
    bool_res = test_fscanf(fake_fp, "%u%*c%u%*c%u");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, bool_res, "TEST #6: Testing 6.10: Test scanning with assignment suppression");

    /* Test hexadecimal scanning */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, "ff04f86af3ecfbfbffffffffffffffbf\n");
    bool_res = test_fscanf(fake_fp, "%02x%02x%02x");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, bool_res, "TEST #6: Testing 6.11: Test hexadecimal scanning");

    /* Test hexadecimal scanning */
    c_ptr = "0102030405060708090a0b0c0d0e0f101112\n";
    for (i = 0; i < 6; i++)
    {
        _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
        strcpy(buf_ptr, c_ptr);
        bool_res = test_fscanf(fake_fp, "%02x%02x%02x");
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, bool_res, "TEST #6: Testing 6.12: Test hexadecimal scanning");
        c_ptr += 6;
    }

    /* Test hexadecimal scanning */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, "1 2 3\n");
    bool_res = test_fscanf(fake_fp, "%x %x %x");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, bool_res, "TEST #6: Testing 6.13: Test hexadecimal scanning");

    /* Test scanning with 0 leading */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, "01 02 03\n");
    bool_res = test_fscanf(fake_fp, "%x %x %x");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, bool_res, "TEST #6: Testing 6.14: Test scanning with 0 leading");

    /* Test scanning with 0 leading */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, "001 002 003\n");
    bool_res = test_fscanf(fake_fp, "%x %x %x");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, bool_res, "TEST #6: Testing 6.15: Test scanning with 0 leading");

    /* Test scanning with 0 leading */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, "0001 0002 0003\n");
    bool_res = test_fscanf(fake_fp, "%x %x %x");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, bool_res, "TEST #6: Testing 6.16: Test scanning with 0 leading");

    /* Test scanning with 0 leading */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, "0a 0b 0c\n");
    bool_res = test_fscanf(fake_fp, "%x %x %x");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, bool_res, "TEST #6: Testing 6.17: Test scanning with 0 leading");

    /* Test scanning with 0 leading */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, "0abc 0def 0abab\n");
    bool_res = test_fscanf(fake_fp, "%x %x %x");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, bool_res, "TEST #6: Testing 6.18: Test scanning with 0 leading");

    /* Test scanning with width specification */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, "123.456.789\n");
    bool_res = test_fscanf(fake_fp, "%3u.%3u.%3u");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, bool_res, "TEST #6: Testing 6.19: Test scanning with width specification");

    /* Test scanning with width specification */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, " 123.456.789\n");
    bool_res = test_fscanf(fake_fp, " %3u.%3u.%3u");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, bool_res, "TEST #6: Testing 6.20: Test scanning with width specification");

    /* Test scanning with width specification */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, "   123    456   789 \n");
    bool_res = test_fscanf(fake_fp, " %3u %3u %3u");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, bool_res, "TEST #6: Testing 6.21: Test scanning with width specification");

    /* Test scanning with width specification */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, "123456789\n");
    bool_res = test_fscanf(fake_fp, "%1u%2u%3u");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, bool_res, "TEST #6: Testing 6.22: Test scanning with width specification");

    /* Test scanning with width specification */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, "123456789\n");
    bool_res = test_fscanf(fake_fp, "%2u%2u%3u");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, bool_res, "TEST #6: Testing 6.23: Test scanning with width specification");

    /* Test scanning with width specification */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, "123456789\n");
    bool_res = test_fscanf(fake_fp, "%3u%2u%3u");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, bool_res, "TEST #6: Testing 6.24: Test scanning with width specification");

    /* Test scanning with width specification */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, "123456789\n");
    bool_res = test_fscanf(fake_fp, "%4u%2u%3u");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, bool_res, "TEST #6: Testing 6.25: Test scanning with width specification");

    /* Test scanning with width specification */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, "123456789\n");
    bool_res = test_fscanf(fake_fp, "%5u%2u%2u");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, bool_res, "TEST #6: Testing 6.26: Test scanning with width specification");

    /* Test scanning with width specification */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, "123.456.789\n");
    bool_res = test_fscanf(fake_fp, "%3u.%3u.%3u");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, bool_res, "TEST #6: Testing 6.27: Test scanning with width specification");

    /* Test scanning ignore whitespace */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, " 123.456.789\n");
    bool_res = test_fscanf(fake_fp, "%u. %u.  %u   ");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, bool_res, "TEST #6: Testing 6.28: Test scanning ignore whitespace");

    /* Test scanning ignore whitespace */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, " 123.456.789\n");
    bool_res = test_fscanf(fake_fp, "%d.%d.%d");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, bool_res, "TEST #6: Testing 6.29: Test scanning ignore whitespace");

    /* Test scanning ignore whitespace */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, " 123.456.789\n");
    bool_res = test_fscanf(fake_fp, "%u.%*u.%u");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, bool_res, "TEST #6: Testing 6.30: Test scanning ignore whitespace");

    /* Test scanning with percent n */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, "123 345\n");
    bool_res = test_fscanf(fake_fp, "%d %d%n");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, bool_res, "TEST #6: Testing 6.31: Test scanning with percent n");

    /* Test scanning with empty input string */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    result = fscanf(fake_fp, "%d", &i);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == IO_EOF, "TEST #6: Testing 6.32: Failed to scan an empty input string");

    /* Close the fake device whose file handler is stored in fake_fp by calling function _io_fclose */
    result = _io_fclose(fake_fp);
    /* Check the returned result must be MQX_OK to verify successfully closing */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "TEST #6: Testing 6.33: Close fake device");
}


/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_7_main_task
* Comments     : TEST #7:
*   This test case is to verify function _io_scanf in the cases of:
*   - Scanning different formats of string from stdin properly.
*   - Scanning empty string from stdin.
* Requirements:
*   FIO024, FIO046
*
*END*---------------------------------------------------------------------*/
void tc_7_main_task(void)
{
    MQX_FILE_PTR fake_fp, stdin_fp;
    void *bak_ptr;
    char *c_ptr, *buf_ptr;
    _mqx_uint i;
    _mqx_int result;
    bool bool_res;

    /* Open fake device, the returned file pointer is stored in fake_fp */
    fake_fp = _io_fopen(FAKE_DEV, NULL);
    /* Check the returned pointer must be NOT NULL to verify successfully opening */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, fake_fp != NULL, "TEST #7: Testing 7.01: Open fake device");

    /* Get the STDOUT file handler, the returned pointer is stored in stdin_fp */
    stdin_fp = (MQX_FILE_PTR)stdin;
    /* Check the returned pointer must be NOT NULL to verify valid stdout */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, stdin_fp != NULL, "TEST #7: Testing 7.02: Get the stdout device");

    /* Get the input buffer pointer of fake device and store in buf_ptr */
    _io_ioctl(fake_fp, IO_IOCTL_GET_IN_BUFFER, &buf_ptr);

    /* Back up the default device of STDOUT (stdin_fp->DEV_PTR), save address to bak_ptr */
    bak_ptr = (void *)stdin_fp->DEV_PTR;
    /* Change the default device of STDOUT to fake device (fake_fp->DEV_PTR) */
    stdin_fp->DEV_PTR = fake_fp->DEV_PTR;

    /* Test scanning with leading whitespace */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, " 123.456.789\n");
    bool_res = test_fscanf(NULL, "%u.%u.%u");
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, bool_res, "TEST #7: Testing 7.03: Test scanning with leading whitespace");

    /* Test scanning with assignment suppression */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, " 123.456.789\n");
    bool_res = test_fscanf(NULL, "%*u.%u.%u");
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, bool_res, "TEST #7: Testing 7.04: Test scanning with assignment suppression");

    /* Test scanning with assignment suppression */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, " 123.456.789\n");
    bool_res = test_fscanf(NULL, "%u.%*u.%u");
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, bool_res, "TEST #7: Testing 7.05: Test scanning with assignment suppression");

    /* Test scanning with assignment suppression */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, " 123.456.789\n");
    bool_res = test_fscanf(NULL, "%*u.%*u.%u");
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, bool_res, "TEST #7: Testing 7.06: Test scanning with assignment suppression");

    /* Test scanning with assignment suppression */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, " 123.456.789\n");
    bool_res = test_fscanf(NULL, "%u.%u.%*u");
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, bool_res, "TEST #7: Testing 7.07: Test scanning with assignment suppression");

    /* Test scanning with assignment suppression */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, " 123.456.789\n");
    bool_res = test_fscanf(NULL, "%*u.%u.%*u");
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, bool_res, "TEST #7: Testing 7.08: Test scanning with assignment suppression");

    /* Test scanning with assignment suppression */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, " 123.456.789\n");
    bool_res = test_fscanf(NULL, "%u.%*u.%*u");
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, bool_res, "TEST #7: Testing 7.09: Test scanning with assignment suppression");

    /* Test scanning with assignment suppression */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, " 123.456.789\n");
    bool_res = test_fscanf(NULL, "%*u.%*u.%*u");
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, bool_res, "TEST #7: Testing 7.10: Test scanning with assignment suppression");

    /* Test scanning with assignment suppression */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, " 123.456.789\n");
    bool_res = test_fscanf(NULL, "%u%*c%u%*c%u");
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, bool_res, "TEST #7: Testing 7.11: Test scanning with assignment suppression");

    /* Test hexadecimal scanning */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, "ff04f86af3ecfbfbffffffffffffffbf\n");
    bool_res = test_fscanf(NULL, "%02x%02x%02x");
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, bool_res, "TEST #7: Testing 7.12: Test hexadecimal scanning");

    /* Test hexadecimal scanning */
    c_ptr = "0102030405060708090a0b0c0d0e0f101112\n";
    for (i = 0; i < 6; i++)
    {
        _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
        strcpy(buf_ptr, c_ptr);
        bool_res = test_fscanf(NULL, "%02x%02x%02x");
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, bool_res, "TEST #7: Testing 7.13: Test hexadecimal scanning");
        c_ptr += 6;
    }

    /* Test hexadecimal scanning */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, "1 2 3\n");
    bool_res = test_fscanf(NULL, "%x %x %x");
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, bool_res, "TEST #7: Testing 7.14: Test hexadecimal scanning");

    /* Test scanning with 0 leading */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, "01 02 03\n");
    bool_res = test_fscanf(NULL, "%x %x %x");
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, bool_res, "TEST #7: Testing 7.15: Test scanning with 0 leading");

    /* Test scanning with 0 leading */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, "001 002 003\n");
    bool_res = test_fscanf(NULL, "%x %x %x");
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, bool_res, "TEST #7: Testing 7.16: Test scanning with 0 leading");

    /* Test scanning with 0 leading */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, "0001 0002 0003\n");
    bool_res = test_fscanf(NULL, "%x %x %x");
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, bool_res, "TEST #7: Testing 7.17: Test scanning with 0 leading");

    /* Test scanning with 0 leading */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, "0a 0b 0c\n");
    bool_res = test_fscanf(NULL, "%x %x %x");
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, bool_res, "TEST #7: Testing 7.18: Test scanning with 0 leading");

    /* Test scanning with 0 leading */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, "0abc 0def 0abab\n");
    bool_res = test_fscanf(NULL, "%x %x %x");
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, bool_res, "TEST #7: Testing 7.19: Test scanning with 0 leading");

    /* Test scanning with width specification */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, "123.456.789\n");
    bool_res = test_fscanf(NULL, "%3u.%3u.%3u");
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, bool_res, "TEST #7: Testing 7.20: Test scanning with width specification");

    /* Test scanning with width specification */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, " 123.456.789\n");
    bool_res = test_fscanf(NULL, " %3u.%3u.%3u");
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, bool_res, "TEST #7: Testing 7.21: Test scanning with width specification");

    /* Test scanning with width specification */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, "   123    456   789 \n");
    bool_res = test_fscanf(NULL, " %3u %3u %3u");
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, bool_res, "TEST #7: Testing 7.22: Test scanning with width specification");

    /* Test scanning with width specification */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, "123456789\n");
    bool_res = test_fscanf(NULL, "%1u%2u%3u");
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, bool_res, "TEST #7: Testing 7.23: Test scanning with width specification");

    /* Test scanning with width specification */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, "123456789\n");
    bool_res = test_fscanf(NULL, "%2u%2u%3u");
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, bool_res, "TEST #7: Testing 7.24: Test scanning with width specification");

    /* Test scanning with width specification */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, "123456789\n");
    bool_res = test_fscanf(NULL, "%3u%2u%3u");
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, bool_res, "TEST #7: Testing 7.25: Test scanning with width specification");

    /* Test scanning with width specification */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, "123456789\n");
    bool_res = test_fscanf(NULL, "%4u%2u%3u");
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, bool_res, "TEST #7: Testing 7.26: Test scanning with width specification");

    /* Test scanning with width specification */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, "123456789\n");
    bool_res = test_fscanf(NULL, "%5u%2u%2u");
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, bool_res, "TEST #7: Testing 7.27: Test scanning with width specification");

    /* Test scanning with width specification */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, "123.456.789\n");
    bool_res = test_fscanf(NULL, "%3u.%3u.%3u");
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, bool_res, "TEST #7: Testing 7.28: Test scanning with width specification");

    /* Test scanning ignore whitespace */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, " 123.456.789\n");
    bool_res = test_fscanf(NULL, "%u. %u.  %u   ");
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, bool_res, "TEST #7: Testing 7.29: Test scanning ignore whitespace");

    /* Test scanning ignore whitespace */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, " 123.456.789\n");
    bool_res = test_fscanf(NULL, "%d.%d.%d");
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, bool_res, "TEST #7: Testing 7.30: Test scanning ignore whitespace");

    /* Test scanning ignore whitespace */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, " 123.456.789\n");
    bool_res = test_fscanf(NULL, "%u.%*u.%u");
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, bool_res, "TEST #7: Testing 7.31: Test scanning ignore whitespace");

    /* Test scanning with percent n */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    strcpy(buf_ptr, "123 345\n");
    bool_res = test_fscanf(NULL, "%d %d%n");
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, bool_res, "TEST #7: Testing 7.32: Test scanning with percent n");

    /* Test scanning with empty input string */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    result = scanf("%d", &i);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == IO_EOF, "TEST #7: Testing 7.33: Failed to scan an empty input string");

    /* Restore the default device of STDOUT to original address which is currently stored in bak_ptr */
    stdin_fp->DEV_PTR = (IO_DEVICE_STRUCT_PTR)bak_ptr;

    /* Close the fake device whose file handler is stored in fake_fp by calling function _io_fclose */
    result = _io_fclose(fake_fp);
    /* Check the returned result must be MQX_OK to verify successfully closing */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "TEST #7: Testing 7.34: Close fake device");
}


/*------------------------------------------------------------------------
** Define Test Suite
*/
EU_TEST_SUITE_BEGIN(suite_fio2)
    EU_TEST_CASE_ADD(tc_1_main_task, "TEST #1 - Testing function _io_fprintf"),
    EU_TEST_CASE_ADD(tc_2_main_task, "TEST #2 - Testing function _io_vfprintf"),
    EU_TEST_CASE_ADD(tc_3_main_task, "TEST #3 - Testing function _io_vprintf"),
    EU_TEST_CASE_ADD(tc_4_main_task, "TEST #4 - Testing function _io_vsnprintf"),
    EU_TEST_CASE_ADD(tc_5_main_task, "TEST #5 - Testing function _io_vsprintf"),
    EU_TEST_CASE_ADD(tc_6_main_task, "TEST #6 - Testing function _io_fscanf"),
    EU_TEST_CASE_ADD(tc_7_main_task, "TEST #7 - Testing function _io_scanf"),
EU_TEST_SUITE_END(suite_fio2)
/* Add test suites */
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_fio2, "MQX Standard I/O Component Test Suite, 7 Tests")
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
* Function Name    : test_vfprintf
* Returned Value   : Returned value from function _io_vfprintf
* Comments         :
*   Help testing functions _io_vfprintf and _io_vprintf
*   - [IN] file_ptr: The stream to print upon.
*   - [IN] fmt_ptr : The format string.
*
*END*----------------------------------------------------------------------*/
_mqx_int test_vfprintf(MQX_FILE_PTR file_ptr, const char *fmt_ptr, ...)
{
    va_list ap;
    _mqx_int result;

    va_start(ap, fmt_ptr);
    /* Choose the appropriate function to test */
    if(file_ptr == NULL)
    {
        /* Test function _io_vprintf */
        result = _io_vprintf(fmt_ptr, ap);
    }
    else
    {
        /* Test function _io_vfprintf */
        result = _io_vfprintf(file_ptr, fmt_ptr, ap);
    }
    va_end(ap);

    return result;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : test_vsnprintf
* Returned Value   : Returned value from function test_vsnprintf
* Comments         :
*   Help testing functions _io_vsprintf and _io_vsnprintf
*   - [IN] str_ptr  : The string to print into.
*   - [IN] max_count: The maximal size of string.
*   - [IN] fmt_ptr  : The format string.
*
*END*----------------------------------------------------------------------*/
_mqx_int test_vsnprintf(char *str_ptr, _mqx_int max_count, const char *fmt_ptr, ...)
{
    va_list ap;
    _mqx_int result;

    va_start(ap, fmt_ptr);
    /* Choose the appropriate function to test */
    if(max_count == 0)
    {
        /* Test function _io_vsprintf */
        result = _io_vsprintf(str_ptr, fmt_ptr, ap);
    }
    else
    {
        /* Test function _io_vsnprintf */
        result = _io_vsnprintf(str_ptr, max_count, fmt_ptr, ap);
    }
    va_end(ap);

    return result;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : Testit
* Returned Value   : bool (TRUE)
* Comments         :
*   Help testing functions _io_fscanf and _io_scanf
*   - [IN] file_ptr: The stream to scan from.
*   - [IN] fmt_ptr : The format string to use when scanning.
*END*----------------------------------------------------------------------*/
bool test_fscanf(MQX_FILE_PTR file_ptr, const char *fmt_ptr)
{
    _mqx_uint a1, a2, a3;
    _mqx_int res;

    a1 = a2 = a3 = 0;
    /* Choose the appropriate function to test */
    if(file_ptr == NULL)
    {
        static _mqx_int index = 0;
        /* Test function _io_scanf */
        res = _io_scanf(fmt_ptr, &a1, &a2, &a3);
        /* Verify the scanned values */
        return (res >= 0) && (a1 == scan_arr[index++]) && (a2 == scan_arr[index++]) && (a3 == scan_arr[index++]);
    }
    else
    {
        static _mqx_int index = 0;
        /* Test function _io_fscanf */
        res = _io_fscanf(file_ptr, fmt_ptr, &a1, &a2, &a3);
        /* Verify the scanned values */
        return (res >= 0) && (a1 == scan_arr[index++]) && (a2 == scan_arr[index++]) && (a3 == scan_arr[index++]);
    }
}
