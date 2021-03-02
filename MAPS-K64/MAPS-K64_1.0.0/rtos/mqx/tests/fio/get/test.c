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
* $Date    : Jul-09-2013$
*
* Comments:
*
*   This file contains code for the MQX Standard I/O verification program,
*   includes all remaining test cases, which haven't been verified yet,
*   of following functions:
*   _io_fgetc, _io_getchar, _io_fgets, _io_gets, _io_fgetline, _io_getline,
*   _io_fungetc, _io_ungetc, _io_fstatus, _io_status.
*
* Requirements:
*
*   FIO006, FIO007, FIO008, FIO009, FIO010, FIO011, FIO019, FIO028, FIO043,
*   FIO059, FIO060, FIO061, FIO062, FIO063, FIO064, FIO069, FIO070, FIO071.
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
void tc_1_main_task(void);  /* TEST #1 - Testing function _io_fgetc */
void tc_2_main_task(void);  /* TEST #2 - Testing function _io_getchar */
void tc_3_main_task(void);  /* TEST #3 - Testing function _io_fgets */
void tc_4_main_task(void);  /* TEST #4 - Testing function _io_gets */
void tc_5_main_task(void);  /* TEST #5 - Testing function _io_fgetline */
void tc_6_main_task(void);  /* TEST #6 - Testing function _io_getline */
void tc_7_main_task(void);  /* TEST #7 - Testing function _io_fungetc */
void tc_8_main_task(void);  /* TEST #8 - Testing function _io_ungetc */
void tc_9_main_task(void);  /* TEST #9 - Testing function _io_fstatus */
void tc_10_main_task(void); /* TEST #10 - Testing function _io_status */

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
*   This test case is to verify function _io_fgetc in the cases of:
*   - Getting character from device properly.
*   - Getting character from device when it has an ungot character.
*   - Getting character from device while its handler is invalid.
*   - Getting character from device while its I/O function is unavailable.
* Requirements:
*   - Getting character from invalid device.
*   FIO006, FIO007.
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    MQX_FILE_PTR fake_fp;
    void *bak_ptr;
    char *buf_ptr;
    _mqx_int result;

    /* Open fake device, the returned file pointer is stored in fake_fp */
    fake_fp = _io_fopen(FAKE_DEV, NULL);
    /* Check the returned pointer must be (NOT NULL) to verify successfully opening */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, fake_fp != NULL, "TEST #1: Testing 1.01: Open fake device");

    /* Get the input buffer pointer of fake device and store in buf_ptr */
    _io_ioctl(fake_fp, IO_IOCTL_GET_IN_BUFFER, &buf_ptr);

    /* Try to get character from invalid device (NULL) by calling function _io_fgetc */
    result = _io_fgetc(NULL);
    /* Check the returned character must be (IO_EOF) to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == IO_EOF, "TEST #1: Testing 1.02: Failed to get character from invalid device");

    /* Backup the function I/O READ of device, save address to bak_ptr */
    bak_ptr = (void *)fake_fp->DEV_PTR->IO_READ;
    /* Change the function I/O READ of device to invalid function */
    fake_fp->DEV_PTR->IO_READ = NULL;
    /* Try to get character from device while its I/O READ is invalid by calling function _io_fgetc */
    result = _io_fgetc(fake_fp);
    /* Check the returned character must be (IO_EOF) to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == IO_EOF, "TEST #1: Testing 1.03: Failed to get character from the device whose I/O READ is invalid");
    /* Verify the error which was set in file handler fake_fp must be correct */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, _io_ferror(fake_fp) == MQX_IO_OPERATION_NOT_AVAILABLE, "TEST #1: Testing 1.04: Verify the error which was set in file handler must be correct");
    /* Restore function I/O READ of device to original address which is currently stored in bak_ptr */
    fake_fp->DEV_PTR->IO_READ = (IO_READ_FPTR)bak_ptr;
    /* Clear error indicator of file handler fake_fp by calling function _io_clearerr */
    _io_clearerr(fake_fp);

    /* Assume file handler fake_fp has an ungot character */
    fake_fp->HAVE_UNGOT_CHARACTER = TRUE;
    fake_fp->UNGOT_CHARACTER = TEST_CHAR;
    /* Get character from device by calling function _io_fgetc */
    result = _io_fgetc(fake_fp);
    /* Check the returned character must be ungot character to verify successfully getting */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == TEST_CHAR, "TEST #1: Testing 1.05: Get character from device while it has an ungot character");

    /* Suppose there are characters received at device by writing TEST_CHAR to its input */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    buf_ptr[0] = TEST_CHAR;
    /* Get character from device by calling function _io_fgetc */
    result = _io_fgetc(fake_fp);
    /* Check the returned character and assigned character must be the same to verify successfully getting */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == TEST_CHAR, "TEST #1: Testing 1.06: Verify the character read");

    /* Close the fake device whose file handler is stored in fake_fp by calling function _io_fclose */
    result = _io_fclose(fake_fp);
    /* Check the returned result must be (MQX_OK) to verify successfully closing */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "TEST #1: Testing 1.07: Close fake device");
}


/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2:
*   This test case is to verify function _io_getchar in the cases of:
*   - Getting character from stdin properly.
*   - Getting character from stdin when it has an ungot character.
*   - Getting character from stdin while its handler is invalid.
*   - Getting character from stdin while its I/O function is unavailable.
* Requirements:
*   FIO059, FIO060.
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    KERNEL_DATA_STRUCT_PTR kernel_data;
    MQX_FILE_PTR fake_fp, stdin_fp;
    void *bak_ptr;
    char *buf_ptr;
    _mqx_int result;

    /* Open fake device, the returned file pointer is stored in fake_fp */
    fake_fp = _io_fopen(FAKE_DEV, NULL);
    /* Check the returned pointer must be (NOT NULL) to verify successfully opening */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, fake_fp != NULL, "TEST #2: Testing 2.01: Open fake device");

    /* Get the stdin handler, the returned pointer is stored in stdin_fp */
    stdin_fp = (MQX_FILE_PTR)stdin;
    /* Check the returned pointer must be (NOT NULL) to verify valid stdin */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, stdin_fp != NULL, "TEST #2: Testing 2.02: Get the stdin handler");

    /* Get the input buffer pointer of fake device and store in buf_ptr */
    _io_ioctl(fake_fp, IO_IOCTL_GET_IN_BUFFER, &buf_ptr);
    /* Get current kernel data */
    _GET_KERNEL_DATA(kernel_data);

    /* Backup the stdin pointer, save address to bak_ptr */
    bak_ptr = kernel_data->ACTIVE_PTR->STDIN_STREAM;
    /* Change the stdin pointer to invalid pointer */
    kernel_data->ACTIVE_PTR->STDIN_STREAM = NULL;
    /* Try to get character from stdin while its handler is invalid by calling function _io_getchar */
    result = _io_getchar();
    /* Check the returned character must be (IO_EOF) to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == IO_EOF, "TEST #2: Testing 2.03: Getting character from stdin while its handler is invalid should fail");
    /* Restore the stdin pointer to original address which is currently stored in bak_ptr */
    kernel_data->ACTIVE_PTR->STDIN_STREAM = bak_ptr;

    /* Backup the function I/O READ of stdin, save address to bak_ptr */
    bak_ptr = (void *)stdin_fp->DEV_PTR->IO_READ;
    /* Change the function I/O READ of stdin to invalid function */
    stdin_fp->DEV_PTR->IO_READ = NULL;
    /* Try to get character from stdin while its I/O READ is invalid by calling function _io_getchar */
    result = _io_getchar();
    /* Check the returned character must be (IO_EOF) to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == IO_EOF, "TEST #2: Testing 2.04: Getting character from stdin while its I/O READ is invalid should fail");
    /* Verify the error which was set in file handler stdin_fp must be correct */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, _io_ferror(stdin_fp) == MQX_IO_OPERATION_NOT_AVAILABLE, "TEST #2: Testing 2.05: Verify the error which was set in file handler must be correct");
    /* Restore function I/O READ of stdin to original address which is currently stored in bak_ptr */
    stdin_fp->DEV_PTR->IO_READ = (IO_READ_FPTR)bak_ptr;
    /* Clear error indicator of file handler stdin_fp by calling function _io_clearerr */
    _io_clearerr(stdin_fp);

    /* Assume stdin has an ungot character */
    stdin_fp->HAVE_UNGOT_CHARACTER = TRUE;
    stdin_fp->UNGOT_CHARACTER = TEST_CHAR;
    /* Get character from stdin by calling function _io_getchar */
    result = _io_getchar();
    /* Check the returned character must be ungot character to verify successfully getting */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == TEST_CHAR, "TEST #2: Testing 2.06: Getting character from stdin while it has an ungot character");

    /* Back up the default device of stdin (stdin_fp->DEV_PTR), save address to bak_ptr */
    bak_ptr = (void *)stdin_fp->DEV_PTR;
    /* Change the default device of stdin to fake device (fake_fp->DEV_PTR) */
    stdin_fp->DEV_PTR = fake_fp->DEV_PTR;
    /* Clear all flags */
    stdin_fp->FLAGS = 0;

    /* Suppose there are characters received at device by writing character to its input */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    buf_ptr[0] = TEST_CHAR;
    /* Get character from stdin by calling function _io_getchar */
    result = _io_getchar();
    /* Check the returned character and assigned character must be the same to verify successfully getting */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == TEST_CHAR, "TEST #2: Testing 2.07: Verify the character read");
    /* Restore the default device of stdin to original address which is currently stored in bak_ptr */
    stdin_fp->DEV_PTR = (IO_DEVICE_STRUCT_PTR)bak_ptr;

    /* Close the fake device whose file handler is stored in fake_fp by calling function _io_fclose */
    result = _io_fclose(fake_fp);
    /* Check the returned result must be MQX_OK to verify successfully closing */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "TEST #2: Testing 2.08: Close fake device");
}


/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : TEST #3:
*   This test case is to verify function _io_fgets in the cases of:
*   - Getting string from device properly.
*   - Getting string from device while its handler is invalid.
*   - Getting string from device while its I/O function is unavailable.
* Requirements:
*   FIO010, FIO011.
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
    MQX_FILE_PTR fake_fp;
    void *bak_ptr;
    char *str, *tmp, *buf_ptr;
    _mqx_int i, result;

    /* Open fake device, the returned file pointer is stored in fake_fp */
    fake_fp = _io_fopen(FAKE_DEV, NULL);
    /* Check the returned pointer must be (NOT NULL) to verify successfully opening */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, fake_fp != NULL, "TEST #3: Testing 3.01: Open fake device");

    /* Get the input buffer pointer of fake device and store in buf_ptr */
    _io_ioctl(fake_fp, IO_IOCTL_GET_IN_BUFFER, &buf_ptr);

    /* Allocate memory for storing received string by calling function _mem_alloc */
    tmp = (char *)_mem_alloc(MAX_CHAR + 1);
    /* Check the returned pointer, which is stored in tmp, must be (NOT NULL) to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, tmp != NULL, "TEST #3: Testing 3.02: Allocate memory for storing chars");

    /* Try to get string from invalid file device (NULL) by calling function _io_fgets */
    str = _io_fgets(tmp, 0, NULL);
    /* Check the returned string must be (NULL) to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, str == NULL, "TEST #3: Testing 3.03: Failed to get character from invalid file device");

    /* Backup the function I/O READ of device, save address to bak_ptr */
    bak_ptr = (void *)fake_fp->DEV_PTR->IO_READ;
    /* Change the function I/O READ of device to invalid function */
    fake_fp->DEV_PTR->IO_READ = NULL;
    /* Try to get character from device while its I/O READ is invalid by calling function _io_fgets */
    str = _io_fgets(tmp, 0, fake_fp);
    /* Check the returned string must be (NULL) to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, str == NULL, "TEST #3: Testing 3.04: Failed to get character from the device whose I/O READ is invalid");
    /* Verify the error which was set in file handler fake_fp must be correct */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, _io_ferror(fake_fp) == MQX_IO_OPERATION_NOT_AVAILABLE, "TEST #3: Testing 3.05: Verify the error which was set in file handler must be correct");
    /* Restore function I/O READ of device to original address which is currently stored in bak_ptr */
    fake_fp->DEV_PTR->IO_READ = (IO_READ_FPTR)bak_ptr;
    /* Clear error indicator of file handler fake_fp by calling function _io_clearerr */
    _io_clearerr(fake_fp);

    /* Suppose there are strings with Newline character at input of device */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    for(i = 0; i < MAX_CHAR; i++)
    {
        buf_ptr[i] = TEST_CHAR;
    }
    while (--i > 0)
    {
        buf_ptr[i] = '\n';
        _io_ioctl(fake_fp, IO_IOCTL_RESET_HANDLER, NULL);
        /* Try to get string of characters from device by calling function _io_fgets */
        str = _io_fgets(tmp, 0, fake_fp);
        /* Check the string length must be correct */
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, strlen(str) == i, "TEST #3: Testing 3.06: Verify the length read");
        /* Check the string read must be correct */
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, !strncmp(str, buf_ptr, i), "TEST #3: Testing 3.07: Verify the string read");
    }

    /* Suppose there are strings with Carrige Return character at input of device */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    for(i = 0; i < MAX_CHAR; i++)
    {
        buf_ptr[i] = TEST_CHAR;
    }
    while (--i > 0)
    {
        buf_ptr[i] = '\r';
        _io_ioctl(fake_fp, IO_IOCTL_RESET_HANDLER, NULL);
        /* Try to get string of characters from device by calling function _io_fgets */
        str = _io_fgets(tmp, 0, fake_fp);
        /* Check the string length must be correct */
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, strlen(str) == i, "TEST #3: Testing 3.08: Verify the length read");
        /* Check the string read must be correct */
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, !strncmp(str, buf_ptr, i), "TEST #3: Testing 3.09: Verify the string read");
    }

    /* Suppose there is string of characters received at input of device */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    for(i = 0; i < MAX_CHAR; i++)
    {
        buf_ptr[i] = TEST_CHAR;
    }
    _io_ioctl(fake_fp, IO_IOCTL_RESET_HANDLER, NULL);
    /* Try to get string with specified length from device by calling function _io_fgets */
    str = _io_fgets(tmp, MAX_CHAR, fake_fp);
    /* Check the string length must be correct */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, strlen(str) == (MAX_CHAR - 1), "TEST #3: Testing 3.10: Verify the length read");
    /* Check the string read must be correct */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, !strncmp(str, buf_ptr, (MAX_CHAR - 1)), "TEST #3: Testing 3.11: Verify the string read");

    /* Free the allocated memory by calling function _mem_free, check result must be (MQX_OK) to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, _mem_free(tmp) == MQX_OK, "TEST #3: Testing 3.12: Free the allocated memory");

    /* Close the fake device whose file handler is stored in fake_fp by calling function _io_fclose */
    result = _io_fclose(fake_fp);
    /* Check the returned result must be MQX_OK to verify successfully closing */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "TEST #3: Testing 3.13: Close fake device");
}


/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_4_main_task
* Comments     : TEST #4:
*   This test case is to verify function _io_gets in the cases of:
*   - Getting string from stdin properly.
*   - Getting string from stdin while its handler is invalid.
*   - Getting string from stdin while its I/O function is unavailable.
* Requirements:
*   FIO063, FIO064.
*
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
    KERNEL_DATA_STRUCT_PTR kernel_data;
    MQX_FILE_PTR fake_fp, stdin_fp;
    void *bak_ptr;
    char *str, *tmp, *buf_ptr;
    _mqx_int i, result;

    /* Open fake device, the returned file pointer is stored in fake_fp */
    fake_fp = _io_fopen(FAKE_DEV, NULL);
    /* Check the returned pointer must be (NOT NULL) to verify successfully opening */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, fake_fp != NULL, "TEST #4: Testing 4.01: Open fake device");

    /* Get the stdin handler, the returned pointer is stored in stdin_fp */
    stdin_fp = (MQX_FILE_PTR)stdin;
    /* Check the returned pointer must be (NOT NULL) to verify valid stdin */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, stdin_fp != NULL, "TEST #4: Testing 4.02: Get the stdin handler");

    /* Allocate memory for storing received string by calling function _mem_alloc */
    tmp = (char *)_mem_alloc(MAX_CHAR + 1);
    /* Check the returned pointer, which is stored in tmp, must be (NOT NULL) to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, tmp != NULL, "TEST #4: Testing 4.03: Allocate memory for storing chars");

    /* Get the input buffer pointer of fake device and store in buf_ptr */
    _io_ioctl(fake_fp, IO_IOCTL_GET_IN_BUFFER, &buf_ptr);
    /* Get current kernel data */
    _GET_KERNEL_DATA(kernel_data);

    /* Backup the stdin pointer, save address to bak_ptr */
    bak_ptr = kernel_data->ACTIVE_PTR->STDIN_STREAM;
    /* Change the stdin pointer to invalid pointer */
    kernel_data->ACTIVE_PTR->STDIN_STREAM = NULL;
    /* Try to get string from invalid file handler (NULL) by calling function _io_gets */
    str = _io_gets(tmp);
    /* Check the returned string must be NULL to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, str == NULL, "TEST #4: Testing 4.04: Failed to get character from invalid file handler");
    /* Restore the stdin pointer to original address which is currently stored in bak_ptr */
    kernel_data->ACTIVE_PTR->STDIN_STREAM = bak_ptr;

    /* Backup the function I/O READ of device, save address to bak_ptr */
    bak_ptr = (void *)stdin_fp->DEV_PTR->IO_READ;
    /* Change the function I/O READ of device to invalid function */
    stdin_fp->DEV_PTR->IO_READ = NULL;
    /* Try to get character from device while its I/O READ is invalid by calling function _io_gets */
    str = _io_gets(tmp);
    /* Check the returned string must be NULL to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, str == NULL, "TEST #4: Testing 4.05: Failed to get character from the device whose I/O READ is invalid");
    /* Verify the error which was set in file handler stdin_fp must be correct */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, _io_ferror(stdin_fp) == MQX_IO_OPERATION_NOT_AVAILABLE, "TEST #4: Testing 4.06: Verify the error which was set in file handler must be correct");
    /* Restore function I/O READ of device to original address which is currently stored in bak_ptr */
    stdin_fp->DEV_PTR->IO_READ = (IO_READ_FPTR)bak_ptr;
    /* Clear error indicator of file handler stdin_fp by calling function _io_clearerr */
    _io_clearerr(stdin_fp);

    /* Back up the device pointer of stdin (stdin_fp->DEV_PTR), save address to bak_ptr */
    bak_ptr = (void *)stdin_fp->DEV_PTR;
    /* Change the device pointer of stdin to fake device (fake_fp->DEV_PTR) */
    stdin_fp->DEV_PTR = fake_fp->DEV_PTR;
    /* Clear all flags */
    stdin_fp->FLAGS = 0;

    _io_set_handle(IO_STDIN, fake_fp);
    /* Suppose there are strings with Newline character at input of device */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    for(i = 0; i < MAX_CHAR; i++)
    {
        buf_ptr[i] = TEST_CHAR;
    }
    while (--i > 0)
    {
        buf_ptr[i] = '\n';
        _io_ioctl(fake_fp, IO_IOCTL_RESET_HANDLER, NULL);
        /* Try to get string of characters from device by calling function _io_fgets */
        str = _io_gets(tmp);
        /* Check the string length must be correct */
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, strlen(str) == i, "TEST #4: Testing 4.07: Verify the length read");
        /* Check the string read must be correct */
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, !strncmp(str, buf_ptr, i), "TEST #4: Testing 4.08: Verify the string read");
    }

    /* Suppose there are strings with Carrige Return character at input of device */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    for(i = 0; i < MAX_CHAR; i++)
    {
        buf_ptr[i] = TEST_CHAR;
    }
    while (--i > 0)
    {
        buf_ptr[i] = '\r';
        _io_ioctl(fake_fp, IO_IOCTL_RESET_HANDLER, NULL);
        /* Try to get string of characters from device by calling function _io_fgets */
        str = _io_gets(tmp);
        /* Check the string length must be correct */
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, strlen(str) == i, "TEST #4: Testing 4.09: Verify the length read");
        /* Check the string read must be correct */
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, !strncmp(str, buf_ptr, i), "TEST #4: Testing 4.10: Verify the string read");
    }

    /* Restore the device pointer of stdin to original address which is currently stored in bak_ptr */
    stdin_fp->DEV_PTR = (IO_DEVICE_STRUCT_PTR)bak_ptr;

    /* Free the allocated memory by calling function _mem_free, check result must be (MQX_OK) to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, _mem_free(tmp) == MQX_OK, "TEST #4: Testing 4.11: Free the allocated memory");

    /* Close the fake device whose file handler is stored in fake_fp by calling function _io_fclose */
    result = _io_fclose(fake_fp);
    /* Check the returned result must be MQX_OK to verify successfully closing */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "TEST #4: Testing 4.12: Close fake device");
}


/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_5_main_task
* Comments     : TEST #5:
*   This test case is to verify function _io_fgetline in the cases of:
*   - Getting line from specified device properly.
*   - Getting line from specified device while its handler is invalid.
*   - Getting line from specified device while its I/O function is unavailable.
* Requirements:
*   FIO008, FIO009.
*
*END*---------------------------------------------------------------------*/
void tc_5_main_task(void)
{
    MQX_FILE_PTR fake_fp;
    void *bak_ptr;
    char *tmp, *buf_ptr;
    _mqx_int i, result;

    /* Open fake device, the returned file pointer is stored in fake_fp */
    fake_fp = _io_fopen(FAKE_DEV, NULL);
    /* Check the returned pointer must be (NOT NULL) to verify successfully opening */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, fake_fp != NULL, "TEST #5: Testing 5.01: Open fake device");

    /* Allocate memory for storing received string by calling function _mem_alloc */
    tmp = (char *)_mem_alloc(MAX_CHAR + 1);
    /* Check the returned pointer, which is stored in tmp, must be (NOT NULL) to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, tmp != NULL, "TEST #5: Testing 5.02: Allocate memory for storing chars");

    /* Get the input buffer pointer of fake device and store in buf_ptr */
    _io_ioctl(fake_fp, IO_IOCTL_GET_IN_BUFFER, &buf_ptr);

    /* Try to get string from invalid file handler (NULL) by calling function _io_fgetline */
    result = _io_fgetline(NULL, tmp, 0);
    /* Check the returned character result must be (IO_EOF) to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == IO_EOF, "TEST #5: Testing 5.03: Failed to get character from invalid file handler");

    /* Backup the function I/O READ of device, save address to bak_ptr */
    bak_ptr = (void *)fake_fp->DEV_PTR->IO_READ;
    /* Change the function I/O READ of device to invalid function */
    fake_fp->DEV_PTR->IO_READ = NULL;
    /* Try to get character from device while its I/O READ is invalid by calling function _io_fgetline */
    result = _io_fgetline(fake_fp, tmp, 0);
    /* Check the returned character result must be (IO_EOF) to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == IO_EOF, "TEST #5: Testing 5.04: Failed to get character from invalid file handler");
    /* Verify the error which was set in file handler fake_fp must be correct */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, _io_ferror(fake_fp) == MQX_IO_OPERATION_NOT_AVAILABLE, "TEST #5: Testing 5.05: Verify the error which was set in file handler must be correct");
    /* Restore function I/O READ of device to original address which is currently stored in bak_ptr */
    fake_fp->DEV_PTR->IO_READ = (IO_READ_FPTR)bak_ptr;
    /* Clear error indicator of file handler fake_fp by calling function _io_clearerr */
    _io_clearerr(fake_fp);

        /* Suppose there are strings with Newline character at input of device */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    for(i = 0; i < MAX_CHAR; i++)
    {
        buf_ptr[i] = TEST_CHAR;
    }
    while (--i > 0)
    {
        buf_ptr[i] = '\n';
        _io_ioctl(fake_fp, IO_IOCTL_RESET_HANDLER, NULL);
        /* Try to get string of characters from device by calling function _io_fgets */
        result = _io_fgetline(fake_fp, tmp, 0);
        /* Check the string length must be correct */
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == i, "TEST #5: Testing 5.06: Verify the length read");
        /* Check the string read must be correct */
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, !strncmp(tmp, buf_ptr, result), "TEST #5: Testing 5.07: Verify the string read");
    }

    /* Suppose there are strings with Carrige Return character at input of device */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    for(i = 0; i < MAX_CHAR; i++)
    {
        buf_ptr[i] = TEST_CHAR;
    }
    while (--i > 0)
    {
        buf_ptr[i] = '\r';
        _io_ioctl(fake_fp, IO_IOCTL_RESET_HANDLER, NULL);
        /* Try to get string of characters from device by calling function _io_fgets */
        result = _io_fgetline(fake_fp, tmp, 0);
        /* Check the string length must be correct */
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == i, "TEST #5: Testing 5.08: Verify the length read");
        /* Check the string read must be correct */
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, !strncmp(tmp, buf_ptr, result), "TEST #5: Testing 5.09: Verify the string read");
    }

    /* Suppose there is string of characters received at input of device */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    for(i = 0; i < MAX_CHAR; i++)
    {
        buf_ptr[i] = TEST_CHAR;
    }
    _io_ioctl(fake_fp, IO_IOCTL_RESET_HANDLER, NULL);
    /* Try to get string with specified length from device by calling function _io_fgets */
    result = _io_fgetline(fake_fp, tmp, MAX_CHAR);
    /* Check the string length must be correct */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == (MAX_CHAR - 1), "TEST #5: Testing 5.10: Verify the length read");
    /* Check the string read must be correct */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, !strncmp(tmp, buf_ptr, result), "TEST #5: Testing 5.11: Verify the string read");

    /* Free the allocated memory by calling function _mem_free, check result must be (MQX_OK) to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, _mem_free(tmp) == MQX_OK, "TEST #5: Testing 5.12: Free the allocated memory");

    /* Close the fake device whose file handler is stored in fake_fp by calling function _io_fclose */
    result = _io_fclose(fake_fp);
    /* Check the returned result must be MQX_OK to verify successfully closing */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "TEST #5: Testing 5.13: Close fake device");
}


/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_6_main_task
* Comments     : TEST #6:
*   This test case is to verify function _io_getline in the cases of:
*   - Getting line from stdin properly.
*   - Getting line from stdin while its handler is invalid.
*   - Getting line from stdin while its I/O function is unavailable.
* Requirements:
*   FIO061, FIO062.
*
*END*---------------------------------------------------------------------*/
void tc_6_main_task(void)
{
    KERNEL_DATA_STRUCT_PTR kernel_data;
    MQX_FILE_PTR fake_fp, stdin_fp;
    void *bak_ptr;
    char *tmp, *buf_ptr;
    _mqx_int i, result;

    /* Open fake device, the returned file pointer is stored in fake_fp */
    fake_fp = _io_fopen(FAKE_DEV, NULL);
    /* Check the returned pointer must be (NOT NULL) to verify successfully opening */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, fake_fp != NULL, "TEST #6: Testing 6.01: Open fake device");

    /* Get the stdin handler, the returned pointer is stored in stdin_fp */
    stdin_fp = (MQX_FILE_PTR)stdin;
    /* Check the returned pointer must be (NOT NULL) to verify valid stdin */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, stdin_fp != NULL, "TEST #6: Testing 6.02: Get the stdin handler");

    /* Allocate memory for storing received string by calling function _mem_alloc */
    tmp = (char *)_mem_alloc(MAX_CHAR + 1);
    /* Check the returned pointer, which is stored in tmp, must be (NOT NULL) to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, tmp != NULL, "TEST #6: Testing 6.03: Allocate memory for storing chars");

    /* Get the input buffer pointer of fake device and store in buf_ptr */
    _io_ioctl(fake_fp, IO_IOCTL_GET_IN_BUFFER, &buf_ptr);
    /* Get current kernel data */
    _GET_KERNEL_DATA(kernel_data);

    /* Backup the stdin pointer, save address to bak_ptr */
    bak_ptr = kernel_data->ACTIVE_PTR->STDIN_STREAM;
    /* Change the stdin pointer to invalid pointer */
    kernel_data->ACTIVE_PTR->STDIN_STREAM = NULL;
    /* Try to get string from invalid file handler (NULL) by calling function _io_getline */
    result = _io_getline(tmp, 0);
    /* Check the returned character result must be (IO_EOF) to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == IO_EOF, "TEST #6: Testing 6.04: Failed to get character from invalid file handler");
    /* Restore the stdin pointer to original address which is currently stored in bak_ptr */
    kernel_data->ACTIVE_PTR->STDIN_STREAM = bak_ptr;

    /* Backup the function I/O READ of device, save address to bak_ptr */
    bak_ptr = (void *)stdin_fp->DEV_PTR->IO_READ;
    /* Change the function I/O READ of device to invalid function */
    stdin_fp->DEV_PTR->IO_READ = NULL;
    /* Try to get character from device while its I/O READ is invalid by calling function _io_getline */
    result = _io_getline(tmp, 0);
    /* Check the returned character result must be (IO_EOF) to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == IO_EOF, "TEST #6: Testing 6.05: Failed to get character from invalid file handler");
    /* Verify the error which was set in file handler stdin_fp must be correct */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, _io_ferror(stdin_fp) == MQX_IO_OPERATION_NOT_AVAILABLE, "TEST #6: Testing 6.06: Verify the error which was set in file handler must be correct");
    /* Restore function I/O READ of device to original address which is currently stored in bak_ptr */
    stdin_fp->DEV_PTR->IO_READ = (IO_READ_FPTR)bak_ptr;
    /* Clear error indicator of file handler stdin_fp by calling function _io_clearerr */
    _io_clearerr(stdin_fp);

    /* Back up the device pointer of stdin (stdin_fp->DEV_PTR), save address to bak_ptr */
    bak_ptr = (void *)stdin_fp->DEV_PTR;
    /* Change the device pointer of stdin to fake device (fake_fp->DEV_PTR) */
    stdin_fp->DEV_PTR = fake_fp->DEV_PTR;
    /* Clear all flags */
    stdin_fp->FLAGS = 0;

        /* Suppose there are strings with Newline character at input of device */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    for(i = 0; i < MAX_CHAR; i++)
    {
        buf_ptr[i] = TEST_CHAR;
    }
    while (--i > 0)
    {
        buf_ptr[i] = '\n';
        _io_ioctl(fake_fp, IO_IOCTL_RESET_HANDLER, NULL);
        /* Try to get string of characters from device by calling function _io_fgets */
        result = _io_getline(tmp, 0);
        /* Check the string length must be correct */
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == i, "TEST #6: Testing 6.07: Verify the length read");
        /* Check the string read must be correct */
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, !strncmp(tmp, buf_ptr, result), "TEST #6: Testing 6.08: Verify the string read");
    }

    /* Suppose there are strings with Carrige Return character at input of device */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    for(i = 0; i < MAX_CHAR; i++)
    {
        buf_ptr[i] = TEST_CHAR;
    }
    while (--i > 0)
    {
        buf_ptr[i] = '\r';
        _io_ioctl(fake_fp, IO_IOCTL_RESET_HANDLER, NULL);
        /* Try to get string of characters from device by calling function _io_fgets */
        result = _io_getline(tmp, 0);
        /* Check the string length must be correct */
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == i, "TEST #6: Testing 6.09: Verify the length read");
        /* Check the string read must be correct */
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, !strncmp(tmp, buf_ptr, result), "TEST #6: Testing 6.10: Verify the string read");
    }

    /* Suppose there is string of characters received at input of device */
    _io_ioctl(fake_fp, IO_IOCTL_CLEAR_BUFFER, NULL);
    for(i = 0; i < MAX_CHAR; i++)
    {
        buf_ptr[i] = TEST_CHAR;
    }
    _io_ioctl(fake_fp, IO_IOCTL_RESET_HANDLER, NULL);
    /* Try to get string with specified length from device by calling function _io_fgets */
    result = _io_getline(tmp, MAX_CHAR);
    /* Check the string length must be correct */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == (MAX_CHAR - 1), "TEST #6: Testing 6.11: Verify the length read");
    /* Check the string read must be correct */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, !strncmp(tmp, buf_ptr, result), "TEST #6: Testing 6.12: Verify the string read");

    /* Restore the device pointer of stdin to original address which is currently stored in bak_ptr */
    stdin_fp->DEV_PTR = (IO_DEVICE_STRUCT_PTR)bak_ptr;

    /* Free the allocated memory by calling function _mem_free, check result must be (MQX_OK) to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, _mem_free(tmp) == MQX_OK, "TEST #6: Testing 6.13: Free the allocated memory");

    /* Close the fake device whose file handler is stored in fake_fp by calling function _io_fclose */
    result = _io_fclose(fake_fp);
    /* Check the returned result must be MQX_OK to verify successfully closing */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "TEST #6: Testing 6.14: Close fake device");
}


/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_7_main_task
* Comments     : TEST #7:
*   This test case is to verify function _io_fungetc in the cases of:
*   - Pushing back character on device properly.
*   - Pushing back character on device while it has ungot character.
*   - Pushing back character on device while its I/O function is unavailable.
* Requirements:
*   FIO028, FIO043.
*
*END*---------------------------------------------------------------------*/
void tc_7_main_task(void)
{
    MQX_FILE_PTR fake_fp;
    _mqx_int result;

    /* Open fake device, the returned file pointer is stored in fake_fp */
    fake_fp = _io_fopen(FAKE_DEV, NULL);
    /* Check the returned pointer must be (NOT NULL) to verify successfully opening */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, fake_fp != NULL, "TEST #7: Testing 7.01: Open fake device");

    /* Try to push back a character (TEST_CHAR) on invalid file handler by calling function _io_fungetc */
    result = _io_fungetc(TEST_CHAR, NULL);
    /* Check the returned character must be (IO_EOF) to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == IO_EOF, "TEST #7: Testing 7.02: Failed to push back a character on invalid device");

    /* Assume file handler fake_fp has already had an ungot character */
    fake_fp->HAVE_UNGOT_CHARACTER = TRUE;
    /* Try to push back a character (TEST_CHAR) on file handler fake_fp by calling function _io_fungetc */
    result = _io_fungetc(TEST_CHAR, fake_fp);
    /* Check the returned character must be (IO_EOF) to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == IO_EOF, "TEST #7: Testing 7.03: Failed to push back a character while device has already had ungot character");

    /* Clear the ungot character of file handler fake_fp */
    fake_fp->HAVE_UNGOT_CHARACTER = FALSE;
    /* Push back a character (TEST_CHAR) on file handler fake_fp by calling function _io_fungetc */
    result = _io_fungetc(TEST_CHAR, fake_fp);
    /* Check the returned character must be TEST_CHAR to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == TEST_CHAR, "TEST #7: Testing 7.04: Push back a character properly");

    /* Close the fake device whose file handler is stored in fake_fp by calling function _io_fclose */
    result = _io_fclose(fake_fp);
    /* Check the returned result must be MQX_OK to verify successfully closing */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "TEST #7: Testing 7.05: Close fake device");
}


/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_8_main_task
* Comments     : TEST #8:
*   This test case is to verify function _io_ungetc in the cases of:
*   - Pushing back character on stdin properly.
*   - Pushing back character on stdin while it has ungot character.
*   - Pushing back character on stdin while its I/O function is unavailable.
* Requirements:
*   FIO070, FIO071.
*
*END*---------------------------------------------------------------------*/
void tc_8_main_task(void)
{
    KERNEL_DATA_STRUCT_PTR kernel_data;
    MQX_FILE_PTR stdin_fp;
    void *bak_ptr;
    _mqx_int result;

    /* Get current kernel data */
    _GET_KERNEL_DATA(kernel_data);

    /* Backup the stdin pointer, save address to bak_ptr */
    bak_ptr = kernel_data->ACTIVE_PTR->STDIN_STREAM;
    /* Change the stdin pointer to invalid pointer */
    kernel_data->ACTIVE_PTR->STDIN_STREAM = NULL;
    /* Try to push back a character (TEST_CHAR) on invalid file handler by calling function _io_ungetc */
    result = _io_ungetc(TEST_CHAR);
    /* Check the returned character must be (IO_EOF) to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == IO_EOF, "TEST #8: Testing 8.01: Failed to push back a character on invalid file handler");
    /* Restore the stdin pointer to original address which is currently stored in bak_ptr */
    kernel_data->ACTIVE_PTR->STDIN_STREAM = bak_ptr;

    /* Get the stdin file handler, the returned pointer is stored in stdin_fp */
    stdin_fp = (MQX_FILE_PTR)stdin;
    /* Assume file handler stdin_fp has already had an ungot character */
    stdin_fp->HAVE_UNGOT_CHARACTER = TRUE;
    /* Try to push back a character (TEST_CHAR) on file handler stdin_fp by calling function _io_ungetc */
    result = _io_ungetc(TEST_CHAR);
    /* Check the returned character must be (IO_EOF) to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == IO_EOF, "TEST #8: Testing 8.02: Failed to push back a character while file handler has already had ungot character");

    /* Clear the ungot character of file handler stdin_fp */
    stdin_fp->HAVE_UNGOT_CHARACTER = FALSE;
    /* Push back a character (TEST_CHAR) on file handler stdin_fp by calling function _io_ungetc */
    result = _io_ungetc(TEST_CHAR);
    /* Check the returned character must be TEST_CHAR to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == TEST_CHAR, "TEST #8: Testing 8.03: Push back a character properly");
}


/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_9_main_task
* Comments     : TEST #9:
*   This test case is to verify function _io_fstatus in the cases of:
*   - Check available character of invalid device
*   - Check available character of device when it has ungot character
*   - Check available character of device when it has character at input
*   - Check available character of device when its handler is unavailable.
* Requirements:
*   FIO019
*
*END*---------------------------------------------------------------------*/
void tc_9_main_task(void)
{
    MQX_FILE_PTR fake_fp;
    _mqx_int result;
    bool bool_res;

    /* Open fake device by calling function _io_fopen */
    fake_fp = _io_fopen(FAKE_DEV, NULL);
    /* Check the returned pointer, which is stored in fake_fp, must be (NOT NULL) to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, fake_fp != NULL, "TEST #9: Testing 9.01: Open fake device");

    /* Try to check available character of invalid device by calling function _io_fstatus */
    bool_res = _io_fstatus(NULL);
    /* Check the returned result must be (FALSE) to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, bool_res == FALSE, "TEST #9: Testing 9.02: Failed to check available character of invalid device");

    /* Suppose there's an ungot character in device */
    fake_fp->HAVE_UNGOT_CHARACTER = TRUE;
    /* Check available character of device by calling function _io_fstatus */
    bool_res = _io_fstatus(fake_fp);
    /* Check the returned result must be (TRUE) to verify there's available character */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, bool_res == TRUE, "TEST #9: Testing 9.03: Check available character of device when it has ungot character");

    /* Clear the ungot character */
    fake_fp->HAVE_UNGOT_CHARACTER = FALSE;
    /* Suppose there's character at input of device, call function _io_fstatus to check it */
    bool_res = _io_fstatus(fake_fp);
    /* Check the returned result must be (TRUE) to verify there's available character */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, bool_res == TRUE, "TEST #9: Testing 9.04: Check available character of device when it has character at input");

    /* Close fake device by calling function _io_close */
    result = _io_fclose(fake_fp);
    /* Check the returned result must be (MQX_OK) to verify closing successfully */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "TEST #9: Testing 9.05: Close fake device");
}


/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_10_main_task
* Comments     : TEST #10:
*   This test case is to verify function _io_status in the cases of:
*   - Check available character of invalid device
*   - Check available character of device when it has ungot character
*   - Check available character of device when it has character at input
*   - Check available character of device when its handler is unavailable.
* Requirements:
*   FIO069
*
*END*---------------------------------------------------------------------*/
void tc_10_main_task(void)
{
    KERNEL_DATA_STRUCT_PTR kernel_data;
    MQX_FILE_PTR fake_fp, stdin_fp;
    void *bak_ptr;
    _mqx_int result;
    bool bool_res;

    /* Open fake device by calling function _io_fopen */
    fake_fp = _io_fopen(FAKE_DEV, NULL);
    /* Check the returned pointer, which is stored in fake_fp, must be (NOT NULL) to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, fake_fp != NULL, "TEST #10: Testing 10.01: Open fake device");

    /* Get the stdin handler, the returned pointer is stored in stdin_fp */
    stdin_fp = (MQX_FILE_PTR)stdin;
    /* Check the returned pointer must be (NOT NULL) to verify valid stdin */
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, stdin_fp != NULL, "TEST #10: Testing 10.02: Get the stdin handler");

    /* Get current kernel data */
    _GET_KERNEL_DATA(kernel_data);

    /* Backup the stdin pointer, save address to bak_ptr */
    bak_ptr = kernel_data->ACTIVE_PTR->STDIN_STREAM;
    /* Change the stdin pointer to invalid pointer */
    kernel_data->ACTIVE_PTR->STDIN_STREAM = NULL;
    /* Try to check available character of invalid device by calling function _io_status */
    bool_res = _io_status();
    /* Check the returned result must be (FALSE) to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, bool_res == FALSE, "TEST #10: Testing 10.03: Failed to check available character of invalid device");
    /* Restore the stdin pointer to original address which is currently stored in bak_ptr */
    kernel_data->ACTIVE_PTR->STDIN_STREAM = bak_ptr;

    /* Suppose there's an ungot character in device */
    stdin_fp->HAVE_UNGOT_CHARACTER = TRUE;
    /* Check available character of device by calling function _io_status */
    bool_res = _io_status();
    /* Check the returned result must be (TRUE) to verify there's available character */
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, bool_res == TRUE, "TEST #10: Testing 10.04: Check available character of device when it has ungot character");

    /* Back up the device pointer of stdin (stdin_fp->DEV_PTR), save address to bak_ptr */
    bak_ptr = (void *)stdin_fp->DEV_PTR;
    /* Change the device pointer of stdin to device (fake_fp->DEV_PTR) */
    stdin_fp->DEV_PTR = fake_fp->DEV_PTR;
    /* Clear all flags */
    stdin_fp->FLAGS = 0;

    /* Clear the ungot character */
    stdin_fp->HAVE_UNGOT_CHARACTER = FALSE;
    /* Suppose there's character at input of device, call function _io_status to check it */
    bool_res = _io_status();
    /* Check the returned result must be (TRUE) to verify there's available character */
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, bool_res == TRUE, "TEST #10: Testing 10.05: Check available character of device when it has character at input");

    /* Restore the device pointer of stdin to original address which is currently stored in bak_ptr */
    stdin_fp->DEV_PTR = (IO_DEVICE_STRUCT_PTR)bak_ptr;

    /* Close fake device by calling function _io_close */
    result = _io_fclose(fake_fp);
    /* Check the returned result must be (MQX_OK) to verify closing successfully */
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == MQX_OK, "TEST #10: Testing 10.06: Close fake device");
}


/*------------------------------------------------------------------------
** Define Test Suite
*/
EU_TEST_SUITE_BEGIN(suite_get)
    EU_TEST_CASE_ADD(tc_1_main_task,  "TEST #1 - Testing function _io_fgetc"),
    EU_TEST_CASE_ADD(tc_2_main_task,  "TEST #2 - Testing function _io_getchar"),
    EU_TEST_CASE_ADD(tc_3_main_task,  "TEST #3 - Testing function _io_fgets"),
    EU_TEST_CASE_ADD(tc_4_main_task,  "TEST #4 - Testing function _io_gets"),
    EU_TEST_CASE_ADD(tc_5_main_task,  "TEST #5 - Testing function _io_fgetline"),
    EU_TEST_CASE_ADD(tc_6_main_task,  "TEST #6 - Testing function _io_getline"),
    EU_TEST_CASE_ADD(tc_7_main_task,  "TEST #7 - Testing function _io_fungetc"),
    EU_TEST_CASE_ADD(tc_8_main_task,  "TEST #8 - Testing function _io_ungetc"),
    EU_TEST_CASE_ADD(tc_9_main_task,  "TEST #9 - Testing function _io_fstatus"),
    EU_TEST_CASE_ADD(tc_10_main_task, "TEST #10 - Testing function _io_status"),
EU_TEST_SUITE_END(suite_get)
/* Add test suites */
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_get, "MQX Standard I/O Component Test Suite, 10 Tests")
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
