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
*   _io_register_file_system, _io_unregister_file_system, _io_get_fs_by_name,
*   _io_get_fs_name, _io_get_first_valid_fs, _io_is_fs_valid, _io_find_filename,
*   _io_validate_device, _io_create_prefixed_filename, _io_get_dev_for_path,
*   _io_rel2abs.
*
* Requirements:
*
*   FIO081, FIO082, FIO083, FIO084, FIO085, FIO086, FIO087, FIO088, FIO089,
*   FIO090, FIO091, FIO092, FIO093, FIO094, FIO095, FIO096, FIO097, FIO098,
*   FIO099, FIO100, FIO101, FIO102
*
*END************************************************************************/

#include <mqx_inc.h>
#include <bsp.h>
#include <fio.h>
#include "fpio_prv.h"
#include "fake_drv.h"
#include "test.h"
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"

/*------------------------------------------------------------------------
** Test suite function prototype
*/
void tc_1_main_task(void);  /* TEST #1 - Testing function _io_(un)register_file_system */
void tc_2_main_task(void);  /* TEST #2 - Testing function _io_get_fs_by_name */
void tc_3_main_task(void);  /* TEST #3 - Testing function _io_get_fs_name */
void tc_4_main_task(void);  /* TEST #4 - Testing function _io_get_first_valid_fs */
void tc_5_main_task(void);  /* TEST #5 - Testing function _io_is_fs_valid */
void tc_6_main_task(void);  /* TEST #6 - Testing function _io_find_filename */
void tc_7_main_task(void);  /* TEST #7 - Testing function _io_validate_device */
void tc_8_main_task(void);  /* TEST #8 - Testing function _io_create_prefixed_filename */
void tc_9_main_task(void);  /* TEST #9 - Testing function _io_get_dev_for_path */
void tc_10_main_task(void); /* TEST #10 - Testing function _io_rel2abs */

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
*   This test case is to verify function _io_(un)register_file_system in
*   the cases of:
*   - (Un)registering specified file handler properly.
*   - (Un)registering an invalid file handler.
*   - Registering file handler when files table is full.
*   - Unregistering specified file handler which is not in files table.
* Requirements :
*   FIO097, FIO098, FIO099, FIO100, FIO101, FIO102
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    MQX_FILE_PTR fake_fp;
    _mqx_int i, result;

    /* Try to register an invalid file handler (NULL) by calling function _io_register_file_system */
    result = _io_register_file_system(NULL, FAKE_DEV);
    /* Check the returned result  must be MQX_INVALID_POINTER to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_INVALID_POINTER, "TEST #1: Testing 1.01: Failed to register an invalid file");

    /* Open fake device, the returned file pointer is stored in fake_fp */
    fake_fp = _io_fopen(FAKE_DEV, NULL);
    /* Check the returned pointer must be NOT NULL to verify successfully opening */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, fake_fp != NULL, "TEST #1: Testing 1.02: Open fake device");

    /* Try to register a file handler with invalid name (NULL) by calling function _io_register_file_system */
    result = _io_register_file_system(fake_fp, NULL);
    /* Check the returned result  must be MQX_INVALID_POINTER to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_INVALID_POINTER, "TEST #1: Testing 1.03: Failed to register a file handler with invalid name");

    /* Loop to register MAX_FS_INSTANCES file handlers */
    for(i = 0; i < MAX_FS_INSTANCES; i++)
    {
        /* Calling function _io_register_file_system to register file handlers properly */
        result = _io_register_file_system(fake_fp, FAKE_DEV);
        /* Check the returned value must be MQX_OK to verify the successful operation */
        EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "TEST #1: Testing 1.04: Register file handlers properly");
    }

    /* Calling function _io_register_file_system when files table is full */
    result = _io_register_file_system(fake_fp, FAKE_DEV);
    /* Check the result must be MQX_OUT_OF_MEMORY to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OUT_OF_MEMORY, "TEST #1: Testing 1.05: Failed to register file handler when FS table is full");

    /* Try to unregister an invalid file handler (NULL) by calling function _io_unregister_file_system */
    result = _io_unregister_file_system(NULL);
    /* Check the returned result  must be MQX_INVALID_POINTER to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_INVALID_POINTER, "TEST #1: Testing 1.06: Failed to unregister an invalid file");

    /* Loop to unregister MAX_FS_INSTANCES file handlers */
    for(i = 0; i < MAX_FS_INSTANCES; i++)
    {
        /* Calling function _io_unregister_file_system to register file handlers properly */
        result = _io_unregister_file_system(fake_fp);
        /* Check the returned value must be MQX_OK to verify the successful operation */
        EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "TEST #1: Testing 1.07: Unregister file handlers properly");
    }

    /* Try to unregister a file handler that hasn't been registered by calling function _io_unregister_file_system */
    result = _io_unregister_file_system(fake_fp);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_INVALID_HANDLE, "TEST #1: Testing 1.08: Failed to unregister a file handler that hasn't been registered");

    /* Close the fake device whose file handler is stored in fake_fp by calling function _io_fclose */
    result = _io_fclose(fake_fp);
    /* Check the returned result must be MQX_OK to verify successfully closing */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "TEST #1: Testing 1.09: Close fake device");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2:
*   This test case is to verify function _io_get_fs_by_name in the cases of:
*   - Getting file handler from file name properly.
*   - Getting file handler from invalid file name.
*   - Getting file handler, which is not in files table, from file name.
* Requirements :
*   FIO091, FIO092
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    MQX_FILE_PTR dev_fp;
    MQX_FILE_PTR fake_fp;
    _mqx_int result;

    /* Open fake device, the returned file pointer is stored in fake_fp */
    fake_fp = _io_fopen(FAKE_DEV, NULL);
    /* Check the returned pointer must be NOT NULL to verify successfully opening */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, fake_fp != NULL, "TEST #2: Testing 2.01: Open fake device");

    /* Register file handler fake_fp into files table by calling function _io_register_file_system */
    result = _io_register_file_system(fake_fp, FAKE_DEV);
    /* Check the returned value must be MQX_OK to verify the successful operation */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "TEST #2: Testing 2.02: Register the fake file handler");

    /* Try to get file handler from invalid file name (NULL) by calling function _io_get_fs_by_name */
    dev_fp = _io_get_fs_by_name("");
    /* Check the returned handler must be NULL to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, dev_fp == NULL, "TEST #2: Testing 2.03: Failed to get file handler from invalid file name");

    /* Get file handler from valid file name by calling function _io_get_fs_by_name */
    dev_fp = _io_get_fs_by_name(FAKE_DEV);
    /* Check the returned value must be not NULL to verify the successful operation  */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, dev_fp != NULL, "TEST #2: Testing 2.04: Get file handler from valid file name");

    /* Unregister the fake_fp from files table by calling function _io_unregister_file_system */
    result = _io_unregister_file_system(fake_fp);
    /* Check the returned value must be MQX_OK to verify the successful operation */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "TEST #2: Testing 2.05: Unregister the fake file handler");

    /* Try to get file handler, which is not in files table, by calling function _io_get_fs_by_name */
    dev_fp = _io_get_fs_by_name(FAKE_DEV);
    /* Check the returned handler must be NULL to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, dev_fp == NULL, "TEST #2: Testing 2.06: File system not found in table");

    /* Close the fake device whose file handler is stored in fake_fp by calling function _io_fclose */
    result = _io_fclose(fake_fp);
    /* Check the returned result must be MQX_OK to verify successfully closing */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "TEST #2: Testing 2.07: Close fake device");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : TEST #3:
*   This test case is to verify function _io_get_fs_name in the cases of:
*   - Checking file system in files table.
*   - Checking file system not in files table.
*   - Checking file system with invalid file name.
*   - Checking file system with invalid name size.
* Requirements :
*   FIO093, FIO094, FIO095
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
    MQX_FILE_PTR fake_fp;
    _mqx_int result;
    char dev_name[NAME_LEN];

    /* Open fake device, the returned file pointer is stored in fake_fp */
    fake_fp = _io_fopen(FAKE_DEV, NULL);
    /* Check the returned pointer must be NOT NULL to verify successfully opening */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, fake_fp != NULL, "TEST #3: Testing 3.01: Open fake device");

    /* Register file handler fake_fp into files table by calling function _io_register_file_system */
    result = _io_register_file_system(fake_fp, FAKE_DEV);
    /* Check the returned value must be MQX_OK to verify the successful operation */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "TEST #3: Testing 3.02: Register the fake file handler");

    /* Calling function _io_get_fs_name with invalid name size */
    result = _io_get_fs_name(fake_fp, dev_name, 0);
    /* Check the result must be MQX_INVALID_SIZE to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_INVALID_SIZE, "TEST #3: Testing 3.03: Failed to check file system with invalid name size");

    /* Calling function _io_get_fs_name with invalid file handler */
    result = _io_get_fs_name(NULL, dev_name, NAME_LEN);
    /* Check the result must be MQX_INVALID_POINTER unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_INVALID_POINTER, "TEST #3: Testing 3.04: Failed to check file system with invalid handler");

    /* Calling function _io_get_fs_name with invalid file name */
    result = _io_get_fs_name(fake_fp, NULL, NAME_LEN);
    /* Check the result must be MQX_INVALID_POINTER unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_INVALID_POINTER, "TEST #3: Testing 3.05: Failed to check file system with invalid name");

    /* Calling function _io_get_fs_name to check if fake file handler is in file system list */
    result = _io_get_fs_name(fake_fp, dev_name, NAME_LEN);
    /* Check the returned result must be MQX_OK to verify this handler is in list */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "TEST #3: Testing 3.06: Check if fake file handler is in file system list");
    /* Check the file name must be the same */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, !strcmp(dev_name, FAKE_DEV), "TEST #3: Testing 3.07: Check the file name must be the same");

    /* Unregister the fake_fp from files table by calling function _io_unregister_file_system */
    result = _io_unregister_file_system(fake_fp);
    /* Check the returned value must be MQX_OK to verify the successful operation */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "TEST #3: Testing 3.08: Unregister the fake file handler");

    /* Calling function _io_get_fs_name to check if fake file handler is in file system list */
    result = _io_get_fs_name(fake_fp, dev_name, NAME_LEN);
    /* Check the returned result must be MQX_INVALID_POINTER to verify this handler is not in list */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_INVALID_POINTER, "TEST #3: Testing 3.09: Check the pointer to the file name is not NULL");

    /* Close the fake device whose file handler is stored in fake_fp by calling function _io_fclose */
    result = _io_fclose(fake_fp);
    /* Check the returned result must be MQX_OK to verify successfully closing */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "TEST #3: Testing 3.10: Close fake device");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_4_main_task
* Comments     : TEST #4:
*   This test case is to verify function _io_get_first_valid_fs in the cases of:
*   - Getting first file system in files table properly.
*   - Getting file system when files table is currently empty.
* Requirements :
*   FIO089, FIO090
*
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
    MQX_FILE_PTR fake_fp;
    MQX_FILE_PTR dev_fp;
    _mqx_int i, result;

    /* Open fake device, the returned file pointer is stored in fake_fp */
    fake_fp = _io_fopen(FAKE_DEV, NULL);
    /* Check the returned pointer must be NOT NULL to verify successfully opening */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, fake_fp != NULL, "TEST #4: Testing 4.01: Open fake device");

    for(i = 0; i < 2; i++)
    {
        /* Calling function _io_register_file_system to register two file handlers to files table */
        result = _io_register_file_system(fake_fp, FAKE_DEV);
        /* Check the returned value must be MQX_OK to verify the successful operation */
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "TEST #4: Testing 4.02: Register two file handler to files table");
    }

    /* Get pointer of first file system empty table by calling function _io_get_first_valid_fs */
    dev_fp = _io_get_first_valid_fs();
    /* Check the returned handler must be (NOT NULL) to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, dev_fp != NULL, "TEST #4: Testing 4.03: Get pointer of first file system in table");

    /* Unregister a file handler from files table by calling function _io_unregister_file_system */
    result = _io_unregister_file_system(fake_fp);
    /* Check the returned value must be MQX_OK to verify the successful operation */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "TEST #4: Testing 4.04: Unregister file handler");

    /* Get pointer of first file system empty table by calling function _io_get_first_valid_fs */
    dev_fp = _io_get_first_valid_fs();
    /* Check the returned handler must be (NOT NULL) to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, dev_fp != NULL, "TEST #4: Testing 4.05: Get pointer of first file system in table");

    /* Unregister the remaining file handler from files table by calling function _io_unregister_file_system */
    result = _io_unregister_file_system(fake_fp);
    /* Check the returned value must be MQX_OK to verify the successful operation */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "TEST #4: Testing 4.06: Unregister file handler");

    /* Get pointer of file system from empty table by calling function _io_get_first_valid_fs */
    dev_fp = _io_get_first_valid_fs();
    /* Check the returned handler must be (NULL) to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, dev_fp == NULL, "TEST #4: Testing 4.07: Failed to get file system from empty table");

    /* Close the fake device whose file handler is stored in fake_fp by calling function _io_fclose */
    result = _io_fclose(fake_fp);
    /* Check the returned result must be MQX_OK to verify successfully closing */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "TEST #4: Testing 4.08: Close fake device");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_5_main_task
* Comments     : TEST #5:
*   This test case is to verify function _io_is_fs_valid in the cases of:
*   - Checking specified file system when it is in the files table.
*   - Checking specified file system when it is not in the files table.
* Requirements :
*   FIO096
*
*END*---------------------------------------------------------------------*/
void tc_5_main_task(void)
{
    MQX_FILE_PTR fake_fp;
    _mqx_int result;
    bool valid;

    /* Open fake device, the returned file pointer is stored in fake_fp */
    fake_fp = _io_fopen(FAKE_DEV, NULL);
    /* Check the returned pointer must be NOT NULL to verify successfully opening */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, fake_fp != NULL, "TEST #5: Testing 5.01: Open fake device");

    /* Register file handler fake_fp into files table by calling function _io_register_file_system */
    result = _io_register_file_system(fake_fp, FAKE_DEV);
    /* Check the returned value must be MQX_OK to verify the successful operation */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "TEST #5: Testing 5.02: Register the fake file handler");

    /* Check if the fake file handler is in files table by calling function _io_is_fs_valid */
    valid = _io_is_fs_valid(fake_fp);
    /* Check the returned value must be TRUE to verify the successful operation */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, valid, "TEST #5: Testing 5.03: Verify the file handler is in files table");

    /* Unregister the fake file handler from files table by calling function _io_unregister_file_system */
    result = _io_unregister_file_system(fake_fp);
    /* Check the returned value must be MQX_OK to verify the successful operation */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "TEST #5: Testing 5.04: Unregister file handler");

    /* Check if the fake file handler is in files table by calling function _io_is_fs_valid */
    valid = _io_is_fs_valid(fake_fp);
    /* Check the returned value must be FALSE to verify file handler is not in table */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, !valid, "TEST #5: Testing 5.05: Verify the file handler is not in files table");

    /* Close the fake device whose file handler is stored in fake_fp by calling function _io_fclose */
    result = _io_fclose(fake_fp);
    /* Check the returned result must be MQX_OK to verify successfully closing */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "TEST #5: Testing 5.06: Close fake device");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_6_main_task
* Comments     : TEST #6:
*   This test case is to verify function _io_find_filename in the cases of:
*   - Getting file name from string which contains valid file name.
*   - Getting file name from string which does not contain valid file name.
*   - Getting file name from invalid string.
* Requirements :
*   FIO081, FIO082
*
*END*---------------------------------------------------------------------*/
void tc_6_main_task(void)
{
    char *name_ptr;
    _mqx_uint i = 0;
    bool valid;
    char dev_name1[NAME_LEN] = "file:name";
    char dev_name2[NAME_LEN] = "filename";

    /* Find file name from invalid string (NULL) by calling function _io_find_filename */
    name_ptr = _io_find_filename(NULL);
    /* Check the returned name must be NULL to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, name_ptr == NULL, "TEST #6: Testing 6.01: Failed to find name from invalid string");

    /* Find file name from invalid string (contains invalid character) by calling function _io_find_filename */
    name_ptr = _io_find_filename("file\name");
    /* Check the returned name must be NULL to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, name_ptr == NULL, "TEST #6: Testing 6.02: The file name is not found in specified string");

    /* Find file name from string dev_name1 (contains valid name) by calling function _io_find_filename */
    name_ptr = _io_find_filename(dev_name1);
    /* Check the returned name must be NOT NULL to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, name_ptr != NULL, "TEST #6: Testing 6.03: Tests function _io_find_filename");

    /* Verify the file name was found by checking the pointer position */
    while(dev_name1[i]!= ':')
    {
        i++;
    }

    if(dev_name1[i + 1] == *name_ptr)
    {
        valid = TRUE;
    }
    else
    {
        valid = FALSE;
    }
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, valid, "TEST #6: Testing 6.04: Verify the file name was found");

    /* Find file name from string dev_name2 (doesn't contain valid name) by calling function _io_find_filename */
    name_ptr = _io_find_filename(dev_name2);
    /* Verifies the returned name must be the same with input string to verify no file name was found */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, name_ptr == dev_name2, "TEST #6: Testing 6.05:  Verify the file name was not found");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_7_main_task
* Comments     : TEST #7:
*   This test case is to verify function _io_validate_device in the cases of:
*   - There's valid device name in input string.
*   - There's no valid device name in input string.
* Requirements :
*   FIO083
*
*END*---------------------------------------------------------------------*/
void tc_7_main_task(void)
{
    bool valid;
    char dev_name1[NAME_LEN] = "valid:\0";
    char dev_name2[NAME_LEN] = "invalid:n\0";

    /* Calling function _io_validate_device with input string is valid device */
    valid = _io_validate_device(dev_name1);
    /* Checks returned value must be TRUE to verify the input string is device name*/
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, valid, "TEST #7: Testing 7.01: Verify the input string is valid device name");

    /* Calling function _io_validate_device with input string is invalid device */
    valid = _io_validate_device(dev_name2);
    /* Verifies the function must return FALSE */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, !valid, "TEST #7: Testing 7.02: Verify the input string is not valid device name");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_8_main_task
* Comments     : TEST #8:
*   This test case is to verify function _io_create_prefixed_filename in
*   the cases of:
*   - Valid prefixed file name is added to device name
*   - Invalid prefixed file name is not added to device name
* Requirements :
*   FIO084
*
*END*---------------------------------------------------------------------*/
void tc_8_main_task(void)
{
    char name[NAME_LEN] = {0};
    char dev_name1[NAME_LEN] = "val:\0";
    char dev_name2[NAME_LEN] = "inval\0";

    /* Create new file name from valid prefixed name by calling function _io_create_prefixed_filename */
    _io_create_prefixed_filename(name, dev_name1, FAKE_DEV);
    /* Verify prefixed file name was added to device name */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, !strncmp(name, dev_name1, strlen(dev_name1)), "TEST #8: Testing 8.01: Verify prefixed file name was added to device name");

    /* Try to create new file name from invalid prefixed name by calling function _io_create_prefixed_filename */
    _io_create_prefixed_filename(name, dev_name2, FAKE_DEV);
    /* Verify prefixed file name was not added to device name */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, !strncmp(name, FAKE_DEV, strlen(FAKE_DEV)), "TEST #8: Testing 8.01: Verify prefixed file name was added to device name");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_9_main_task
* Comments     : TEST #9:
*   This test case is to verify function _io_get_dev_for_path in the cases of:
*   - Valid file was parsed from input path.
*   - Invalid file was parsed from input path.
*   - Input path is invalid.
* Requirements :
*   FIO087, FIO088
*
*END*---------------------------------------------------------------------*/
void tc_9_main_task(void)
{
    int32_t result;
    bool valid;
    char name[NAME_LEN] = {0};
    char dev_name1[NAME_LEN] = "val:\0";
    char dev_name2[NAME_LEN] = "inval\0";

    /* Calling function _io_get_dev_for_path to parse a valid path and return device name */
    result = _io_get_dev_for_path(name, &valid, NAME_LEN, dev_name1, FAKE_DEV);
    /* Check returned value must be length of device name */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == strlen(dev_name1), "TEST #9: Testing 9.01: Parse a valid path and return device name");
    /* Verify the correct device name was returned */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, !strncmp(name, dev_name1, result), "TEST #9: Testing 9.02: Verify the correct device name was returned");

    /* Calling function _io_get_dev_for_path when there is no valid device name in parsed path*/
    result = _io_get_dev_for_path(name, &valid, NAME_LEN, dev_name2, FAKE_DEV);
    /* Checks returned value must be length of current device */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == strlen(FAKE_DEV), "TEST #9: Testing 9.03: There was no device name in parsed path");
    /* Verify the device name is current device name */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, !strncmp(name, FAKE_DEV, result), "TEST #9: Testing 9.04: Verify the device name is current device name");

    /* Calling function _io_get_dev_for_path when input string is invalid (NULL) */
    result = _io_get_dev_for_path(name, &valid, NAME_LEN, NULL, FAKE_DEV);
    /* Checks the returned value must be 0 to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == 0, "TEST #9: Testing 9.05: Failed to get device name from invalid path");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_10_main_task
* Comments     : TEST #10:
*   This test case is to verify function _io_rel2abs in the cases of:
*   - Getting absolute path from relative path properly.
*   - Getting absolute path from invalid relative path.
* Requirements :
*   FIO085, FIO086
*
*END*---------------------------------------------------------------------*/
void tc_10_main_task(void)
{
    int32_t result;
    char abs_path[NAME_LEN*2] = {0};
    char cur_dir[NAME_LEN] = "./mqx\0";
    char rel_path[NAME_LEN] = "fio\0";

    /* Get absolute path from relative path by calling function _io_rel2abs */
    result = _io_rel2abs(abs_path, cur_dir, rel_path, NAME_LEN*2, FAKE_DEV);
    /* Check the returned value must be (0) to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == MQX_OK, "TEST #10: Testing 10.01: Valid input path");
    /* Check the returned absolute path must be correct */
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, !strcmp(abs_path, "fake:\\mqx\\fio\0"), "TEST #10: Testing 10.02: Verify the absolute path");

    /* TODO: ENGR00273509 */
    /* Calling function _io_rel2abs with invalid parameter */
    result = _io_rel2abs(abs_path, NULL, NULL, NAME_LEN*2, NULL);
    /* Check the returned value must be (MQX_INVALID_PARAMETER) to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == MQX_INVALID_PARAMETER, "TEST #10: Testing 10.03: Invalid parameter");
}

/*------------------------------------------------------------------------
** Define Test Suite
*/
EU_TEST_SUITE_BEGIN(suite_misc2)
    EU_TEST_CASE_ADD(tc_1_main_task,  "TEST #1 - Testing functions _io_(un)register_file_system"),
    EU_TEST_CASE_ADD(tc_2_main_task,  "TEST #2 - Testing function _io_get_fs_by_name"),
    EU_TEST_CASE_ADD(tc_3_main_task,  "TEST #3 - Testing function _io_get_fs_name"),
    EU_TEST_CASE_ADD(tc_4_main_task,  "TEST #4 - Testing function _io_get_first_valid_fs"),
    EU_TEST_CASE_ADD(tc_5_main_task,  "TEST #5 - Testing function _io_is_fs_valid"),
    EU_TEST_CASE_ADD(tc_6_main_task,  "TEST #6 - Testing function _io_find_filename"),
    EU_TEST_CASE_ADD(tc_7_main_task,  "TEST #7 - Testing function _io_validate_device"),
    EU_TEST_CASE_ADD(tc_8_main_task,  "TEST #8 - Testing function _io_create_prefixed_filename"),
    EU_TEST_CASE_ADD(tc_9_main_task,  "TEST #9 - Testing function _io_get_dev_for_path"),
    EU_TEST_CASE_ADD(tc_10_main_task, "TEST #10 - Testing function _io_rel2abs"),
EU_TEST_SUITE_END(suite_misc2)
/* Add test suites */
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_misc2, "MQX Standard I/O Component Test Suite, 10 Tests")
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
