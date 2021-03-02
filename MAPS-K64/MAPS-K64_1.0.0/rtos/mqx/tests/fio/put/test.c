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
* $Date    : Jul-18-2013$
*
* Comments:
*
*   This file contains code for the MQX Standard I/O verification program,
*   includes all remaining test cases, which haven't been verified yet,
*   of following functions:
*   _io_fputc, _io_putchar, _io_fputs, _io_puts.
*
* Requirements:
*
*   FIO015, FIO016, FIO038, FIO039, FIO065, FIO066, FIO067, FIO068.
*
*END************************************************************************/

#include <mqx_inc.h>
#include <bsp.h>
#include <fio.h>
#include "fake_drv.h"
#include "test.h"
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"

/*------------------------------------------------------------------------
** Test suite function prototype
*/
void tc_1_main_task(void);  /* TEST #1 - Testing function _io_fputc */
void tc_2_main_task(void);  /* TEST #2 - Testing function _io_putchar */
void tc_3_main_task(void);  /* TEST #3 - Testing function _io_fputs */
void tc_4_main_task(void);  /* TEST #4 - Testing function _io_puts */

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
*   This test case is to verify function _io_fputc in the cases of:
*   - Writing different types of characters to specified device properly.
*   - Writing character to specified device while its handler is invalid.
*   - Writing character to specified device while its I/O function is unavailable.
* Requirements:
*   FIO015, FIO038.
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    MQX_FILE_PTR fake_fp;
    void *bak_ptr;
    char *buf_ptr;
    _mqx_int c, result;

    /* Open fake device, the returned file pointer is stored in fake_fp */
    fake_fp = _io_fopen(FAKE_DEV, NULL);
    /* Check the returned pointer must be NOT NULL to verify successfully opening */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, fake_fp != NULL, "TEST #1: Testing 1.01: Open fake device");

    /* Get the input buffer pointer of fake device and store in buf_ptr */
    _io_ioctl(fake_fp, IO_IOCTL_GET_OUT_BUFFER, &buf_ptr);

    /* Try to put character to invalid file handler (NULL) by calling function _io_fputc */
    result = _io_fputc(TEST_CHAR, NULL);
    /* Check the returned character must be IO_EOF to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == IO_EOF, "TEST #1: Testing 1.02: Failed to put character to invalid file handler");

    /* Backup the function I/O WRITE of device, save address to bak_ptr */
    bak_ptr = (void *)fake_fp->DEV_PTR->IO_WRITE;
    /* Change the function I/O WRITE of device to invalid function */
    fake_fp->DEV_PTR->IO_WRITE = NULL;
    /* Try to put character to device while its I/O WRITE is invalid by calling function _io_fputc */
    result = _io_fputc(TEST_CHAR, fake_fp);
    /* Check the returned character must be IO_EOF to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == IO_EOF, "TEST #1: Testing 1.03: Failed to put character to the device whose I/O WRITE function is invalid");
    /* Verify the error which was set in file handler fake_fp must be correct */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, _io_ferror(fake_fp) == MQX_IO_OPERATION_NOT_AVAILABLE, "TEST #1: Testing 1.04: Verify the error which was set in file handler must be correct");
    /* Restore function I/O WRITE of device to original address which is currently stored in bak_ptr */
    fake_fp->DEV_PTR->IO_WRITE = (IO_WRITE_FPTR)bak_ptr;
    /* Clear error indicator of file handler fake_fp by calling function _io_clearerr */
    _io_clearerr(fake_fp);

    /* Loop from 0 to MAX_CHAR (127) to write all ascii characters to device */
    for(c = 0; c <= MAX_CHAR; c++)
    {
        /* Clear buffer for next writing operation */
        _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
        /* In each iteration, call function _io_fputc to write character one by one to device */
        result = _io_fputc(c, fake_fp);
        /* If any error occurs, break the loop */
        if(result != *buf_ptr)
        {
            break;
        }
    }
    /* Check the iteration value must be (MAX_CHAR + 1) to verify all characters was written correctly */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, c == (MAX_CHAR + 1), "TEST #1: Testing 1.05: Verify all characters written");

    /* Close the fake device whose file handler is stored in fake_fp by calling function _io_fclose */
    result = _io_fclose(fake_fp);
    /* Check the returned result must be MQX_OK to verify successfully closing */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "TEST #1: Testing 1.06: Close fake device");
}


/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2:
*   This test case is to verify function _io_putchar in the cases of:
*   - Writing different types of characters to stdout properly.
*   - Writing character to stdout while its handler is invalid.
*   - Writing character to stdout while its I/O function is unavailable.
* Requirements:
*   FIO065, FIO066.
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    KERNEL_DATA_STRUCT_PTR kernel_data;
    MQX_FILE_PTR fake_fp, stdout_fp;
    void *bak_ptr;
    char *buf_ptr;
    _mqx_int c, result;

    /* Open fake device, the returned file pointer is stored in fake_fp */
    fake_fp = _io_fopen(FAKE_DEV, NULL);
    /* Check the returned pointer must be NOT NULL to verify successfully opening */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, fake_fp != NULL, "TEST #2: Testing 2.01: Open fake device");

    /* Get the stdout file handler, the returned pointer is stored in stdout_fp */
    stdout_fp = (MQX_FILE_PTR)stdout;
    /* Check the returned pointer must be NOT NULL to verify valid stdout */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, stdout_fp != NULL, "TEST #2: Testing 2.02: Get the stdout device");

    /* Get the input buffer pointer of fake device and store in buf_ptr */
    _io_ioctl(fake_fp, IO_IOCTL_GET_OUT_BUFFER, &buf_ptr);
    /* Get current kernel data */
    _GET_KERNEL_DATA(kernel_data);

    /* Backup the stdout pointer, save address to bak_ptr */
    bak_ptr = kernel_data->ACTIVE_PTR->STDOUT_STREAM;
    /* Change the stdout pointer to invalid pointer */
    kernel_data->ACTIVE_PTR->STDOUT_STREAM = NULL;
    /* Try to put character to invalid file handler (NULL) by calling function _io_putchar */
    result = _io_putchar(TEST_CHAR);
    /* Check the returned character must be IO_EOF to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == IO_EOF, "TEST #2: Testing 2.02: Failed to put character to invalid file handler");
    /* Restore the stdout pointer to original address which is currently stored in bak_ptr */
    kernel_data->ACTIVE_PTR->STDOUT_STREAM = bak_ptr;

    /* Backup the function I/O WRITE of STDOUT, save address to bak_ptr */
    bak_ptr = (void *)stdout_fp->DEV_PTR->IO_WRITE;
    /* Change the function I/O WRITE of stdout to invalid function */
    stdout_fp->DEV_PTR->IO_WRITE = NULL;
    /* Try to put character to stdout while its I/O WRITE is invalid by calling function _io_putchar */
    result = _io_putchar(TEST_CHAR);
    /* Check the returned character must be IO_EOF to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == IO_EOF, "TEST #2: Testing 2.03: Failed to put character to the device whose I/O WRITE function is invalid");
    /* Verify the error which was set in file handler stdout_fp must be correct */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, _io_ferror(stdout_fp) == MQX_IO_OPERATION_NOT_AVAILABLE, "TEST #2: Testing 2.04: Verify the error which was set in file handler must be correct");
    /* Restore function I/O WRITE of stdout to original address which is currently stored in bak_ptr */
    stdout_fp->DEV_PTR->IO_WRITE = (IO_WRITE_FPTR)bak_ptr;
    /* Clear error indicator of file handler stdout_fp by calling function _io_clearerr */
    _io_clearerr(stdout_fp);

    /* Back up the default device of stdout (stdout_fp->DEV_PTR), save address to bak_ptr */
    bak_ptr = (void *)stdout_fp->DEV_PTR;
    /* Change the default device of stdout to fake device (fake_fp->DEV_PTR) */
    stdout_fp->DEV_PTR = fake_fp->DEV_PTR;
    /* Clear all flags */
    stdout_fp->FLAGS = 0;

    /* Loop from 0 to MAX_CHAR (127) to write all ascii characters to stdout */
    for(c = 0; c <= MAX_CHAR; c++)
    {
        /* Clear buffer for next writing operation */
        _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
        /* In each iteration, call function _io_putchar to write character one by one to stdout */
        result = _io_putchar(c);
        /* If any error occurs, break the loop */
        if(result != *buf_ptr)
        {
            break;
        }
    }
    /* Check the iteration value must be (MAX_CHAR + 1) to verify all characters was written correctly */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, c == (MAX_CHAR + 1), "TEST #2: Testing 2.05: Verify all characters written");

    /* Restore the default device of stdout to original address which is currently stored in bak_ptr */
    stdout_fp->DEV_PTR = (IO_DEVICE_STRUCT_PTR)bak_ptr;

    /* Close the fake device whose file handler is stored in fake_fp by calling function _io_fclose */
    result = _io_fclose(fake_fp);
    /* Check the returned result must be MQX_OK to verify successfully closing */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "TEST #2: Testing 2.06: Close fake device");
}


/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : TEST #3:
*   This test case is to verify function _io_fputs in the cases of:
*   - Writing string to specified device properly.
*   - Writing string to specified device while its handler is invalid.
*   - Writing string to specified device while its I/O function is unavailable.
* Requirements:
*   FIO016, FIO039.
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
    MQX_FILE_PTR fake_fp;
    void *bak_ptr;
    char *str, *buf_ptr;
    _mqx_int result;
    int32_t i;

    /* Open fake device, the returned file pointer is stored in fake_fp */
    fake_fp = _io_fopen(FAKE_DEV, NULL);
    /* Check the returned pointer must be NOT NULL to verify successfully opening */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, fake_fp != NULL, "TEST #3: Testing 3.01: Open fake device");

    /* Allocate memory for storing writing string by calling function _mem_alloc */
    str = (char *)_mem_alloc(MAX_CHAR);
    /* Check the returned pointer, which is stored in str, must be NOT NULL to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, str != NULL, "TEST #3: Testing 3.02: Allocate memory for storing chars");

    /* Get the input buffer pointer of fake device and store in buf_ptr */
    _io_ioctl(fake_fp, IO_IOCTL_GET_OUT_BUFFER, &buf_ptr);

    /* Try to put character to invalid file handler (NULL) by calling function _io_fputs */
    result = _io_fputs(TEST_STR, NULL);
    /* Check the returned character must be IO_EOF to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == IO_EOF, "TEST #3: Testing 3.03: Failed to write string to invalid file handler");

    /* Backup the function I/O WRITE of device, save address to bak_ptr */
    bak_ptr = (void *)fake_fp->DEV_PTR->IO_WRITE;
    /* Change the function I/O WRITE of device to invalid function */
    fake_fp->DEV_PTR->IO_WRITE = NULL;
    /* Try to put character to device while its I/O WRITE is invalid by calling function _io_fputs */
    result = _io_fputs(TEST_STR, fake_fp);
    /* Check the returned character must be IO_EOF to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == IO_EOF, "TEST #3: Testing 3.04: Failed to write string to the device whose I/O WRITE function is invalid");
    /* Verify the error which was set in file handler fake_fp must be correct */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, _io_ferror(fake_fp) == MQX_IO_OPERATION_NOT_AVAILABLE, "TEST #3: Testing 3.05: Verify the error which was set in file handler must be correct");
    /* Restore function I/O WRITE of device to original address which is currently stored in bak_ptr */
    fake_fp->DEV_PTR->IO_WRITE = (IO_WRITE_FPTR)bak_ptr;
    /* Clear error indicator of file handler fake_fp by calling function _io_clearerr */
    _io_clearerr(fake_fp);

    /* Create strings with different length. The pointers are stored in str */
    for(i = 0; i < MAX_CHAR; i++)
    {
        str[i] = TEST_CHAR;
    }
    while (--i > 0)
    {
        str[i] = '\0';
        /* Clear buffer for next writing operation */
        _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
        /* Write the string str to device by calling function _io_fputs */
        result = _io_fputs(str, fake_fp);
        /* Check the returned character count must be 0 to verify successful operation */
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == i, "TEST #3: Testing 3.06: Verify string length");
        /* Check the chacters written must be correct */
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, !strncmp(buf_ptr, str, result), "TEST #3: Testing 3.07: Verify chacracters written");
    }

    /* Free the allocated memory by calling function _mem_free, check result must be (MQX_OK) to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, _mem_free(str) == MQX_OK, "TEST #3: Testing 3.08: Free the allocated memory");

    /* Close the fake device whose file handler is stored in fake_fp by calling function _io_fclose */
    result = _io_fclose(fake_fp);
    /* Check the returned result must be MQX_OK to verify successfully closing */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "TEST #3: Testing 3.09: Close fake device");
}


/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_4_main_task
* Comments     : TEST #4:
*   This test case is to verify function _io_puts in the cases of:
*   - Writing string to stdout properly.
*   - Writing string to stdout while its handler is invalid.
*   - Writing string to stdout while its I/O function is unavailable.
* Requirements:
*   FIO067, FIO068.
*
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
    KERNEL_DATA_STRUCT_PTR kernel_data;
    MQX_FILE_PTR fake_fp, stdout_fp;
    void *bak_ptr;
    char *str, *buf_ptr;
    _mqx_int result;
    int32_t i;

    /* Open fake device, the returned file pointer is stored in fake_fp */
    fake_fp = _io_fopen(FAKE_DEV, NULL);
    /* Check the returned pointer must be NOT NULL to verify successfully opening */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, fake_fp != NULL, "TEST #4: Testing 4.01: Open fake device");

    /* Get the stdout file handler, the returned pointer is stored in stdout_fp */
    stdout_fp = (MQX_FILE_PTR)stdout;
    /* Check the returned pointer must be NOT NULL to verify valid stdout */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, stdout_fp != NULL, "TEST #4: Testing 4.02: Get the stdout device");

    /* Allocate memory for storing writing string by calling function _mem_alloc */
    str = (char *)_mem_alloc(MAX_CHAR);
    /* Check the returned pointer, which is stored in str, must be NOT NULL to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, str != NULL, "TEST #4: Testing 4.03: Allocate memory for storing chars");

    /* Get the input buffer pointer of fake device and store in buf_ptr */
    _io_ioctl(fake_fp, IO_IOCTL_GET_OUT_BUFFER, &buf_ptr);
    /* Get current kernel data */
    _GET_KERNEL_DATA(kernel_data);

    /* Backup the stdout pointer, save address to bak_ptr */
    bak_ptr = kernel_data->ACTIVE_PTR->STDOUT_STREAM;
    /* Change the stdout pointer to invalid pointer */
    kernel_data->ACTIVE_PTR->STDOUT_STREAM = NULL;
    /* Try to write string to stdout by calling function _io_puts */
    result = _io_puts(TEST_STR);
    /* Check the returned character must be IO_EOF to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == IO_EOF, "TEST #4: Testing 4.04: Failed to write string to invalid file handler");
    /* Restore the stdout pointer to original address which is currently stored in bak_ptr */
    kernel_data->ACTIVE_PTR->STDOUT_STREAM = bak_ptr;

    /* Backup the function I/O WRITE of STDOUT, save address to bak_ptr */
    bak_ptr = (void *)stdout_fp->DEV_PTR->IO_WRITE;
    /* Change the function I/O WRITE of stdout to invalid function */
    stdout_fp->DEV_PTR->IO_WRITE = NULL;
    /* Try to write string to stdout while its I/O WRITE is invalid by calling function _io_puts */
    result = _io_puts(TEST_STR);
    /* Check the returned character must be IO_EOF to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == IO_EOF, "TEST #4: Testing 4.05: Failed to write string to the device whose I/O WRITE function is invalid");
    /* Verify the error which was set in file handler stdout_fp must be correct */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, _io_ferror(stdout_fp) == MQX_IO_OPERATION_NOT_AVAILABLE, "TEST #4: Testing 4.06: Verify the error which was set in file handler must be correct");
    /* Restore function I/O WRITE of device to original address which is currently stored in bak_ptr */
    stdout_fp->DEV_PTR->IO_WRITE = (IO_WRITE_FPTR)bak_ptr;
    /* Clear error indicator of file handler stdout_fp by calling function _io_clearerr */
    _io_clearerr(stdout_fp);

    /* Back up the default device of stdout (stdout_fp->DEV_PTR), save address to bak_ptr */
    bak_ptr = (void *)stdout_fp->DEV_PTR;
    /* Change the default device of stdout to fake device (fake_fp->DEV_PTR) */
    stdout_fp->DEV_PTR = fake_fp->DEV_PTR;
    /* Clear all flags */
    stdout_fp->FLAGS = 0;

    /* Create strings with different length. The pointers are stored in str */
    for(i = 0; i < MAX_CHAR; i++)
    {
        str[i] = TEST_CHAR;
    }
    while (--i > 0)
    {
        str[i] = '\0';
        /* Clear buffer for next writing operation */
        _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
        /* Write the string str to stdout by calling function _io_puts */
        result = _io_puts(str);
        /* Check the returned character count must be i to verify successful operation */
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == i, "TEST #4: Testing 4.07: Verify string length");
        /* Check the chacters written must be correct */
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, !strncmp(buf_ptr, str, result), "TEST #4: Testing 4.08: Verify chacracters written");
    }

    /* Restore the default device of stdout to original address which is currently stored in bak_ptr */
    stdout_fp->DEV_PTR = (IO_DEVICE_STRUCT_PTR)bak_ptr;

    /* Free the allocated memory by calling function _mem_free, check result must be (MQX_OK) to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, _mem_free(str) == MQX_OK, "TEST #4: Testing 4.09: Free the allocated memory");

    /* Close the fake device whose file handler is stored in fake_fp by calling function _io_fclose */
    result = _io_fclose(fake_fp);
    /* Check the returned result must be MQX_OK to verify successfully closing */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "TEST #4: Testing 4.10: Close fake device");
}


/*------------------------------------------------------------------------
** Define Test Suite
*/
EU_TEST_SUITE_BEGIN(suite_put)
    EU_TEST_CASE_ADD(tc_1_main_task, "TEST #1 - Testing function _io_fputc"),
    EU_TEST_CASE_ADD(tc_2_main_task, "TEST #2 - Testing function _io_putchar"),
    EU_TEST_CASE_ADD(tc_3_main_task, "TEST #3 - Testing function _io_fputs"),
    EU_TEST_CASE_ADD(tc_4_main_task, "TEST #4 - Testing function _io_puts"),
EU_TEST_SUITE_END(suite_put)
/* Add test suites */
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_put, "MQX Standard I/O Component Test Suite, 4 Tests")
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
