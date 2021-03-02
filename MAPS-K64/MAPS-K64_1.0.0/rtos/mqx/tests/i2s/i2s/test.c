/**HEADER********************************************************************
*
* Copyright (c) 2013 Freescale Semiconductor;
* All Rights Reserved
*
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
* $Date    : 08-25-2013$
*
* Comments:
*
*   This file contains the source for i2s module Test.
*
* Requirements: I2S000,I2S001,I2S002,I2S003,I2S004,
*               I2S005,I2S006,I2S007,I2S008,
*               I2S010,I2S011,I2S014,
*               I2S015,I2S016,I2S017,I2S018,I2S019,
*               I2S020,I2S021,I2S022,I2S023,I2S024,
*               I2S025,I2S026,I2S027,I2S028,I2S029,
*               I2S030,I2S031,I2S032,I2S033,I2S034,
*               I2S035,I2S036,I2S037,I2S038,I2S039,
*               I2S040,I2S041,I2S042,I2S043,I2S044,
*               I2S045,I2S046,I2S047,I2S048,I2S049,
*               I2S050,I2S051,I2S052,I2S053,I2S054,
*               I2S055,I2S056,I2S057,I2S058,I2S059,
*               I2S060,I2S064
*END************************************************************************/

#include <mqx_inc.h>
#include <bsp.h>
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"
#include "test.h"

void    main_task(uint32_t);
void    read_task(uint32_t);
//------------------------------------------------------------------------
// Test suite function prototype
void tc_1_main_task(void);// Test #1 - Test initialization
void tc_2_main_task(void);// Test #2 - Test deinitialization
void tc_3_main_task(void);// Test #3 - Test set and get driver mode
void tc_4_main_task(void);// Test #4 - Test set and get clock source
void tc_5_main_task(void);// Test #5 - Test set and get data bits
void tc_6_main_task(void);// Test #6 - Test set and get master clock frequency
void tc_7_main_task(void);// Test #7 - Test set and get frame sync frequency
void tc_8_main_task(void);// Test #8 - Test set and get data format
void tc_9_main_task(void);// Test #9 - Test transmitting operation
void tc_10_main_task(void);// Test #10 - Test while device is in progress
void tc_11_main_task(void);// TEST #11 - Test set and get fifo water mark
void tc_12_main_task(void);// TEST #12 - Test set and get clock mode
//------------------------------------------------------------------------
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
{ MAIN_TASK, main_task,  3000, 9, "Main_Task", MQX_AUTO_START_TASK},
{ TEST_TASK, read_task, 1000, 8, "Read_task",  0                  },
{ 0,         0,          0,      0, 0,           0                }
};


void* get_init_data_ptr(void* dev_ptr)
{
    void **init_ptr;

    init_ptr = (void *)((_mem_size)dev_ptr + DATA_INIT_OFFSET);

    return *init_ptr;
}

void set_init_data_ptr(void* dev_ptr, void * new_init_ptr)
{
    void **init_ptr;

    init_ptr = (void *)((_mem_size)dev_ptr + DATA_INIT_OFFSET);

    *init_ptr = new_init_ptr;
}

/*TASK*-------------------------------------------------------------------
*
* Task Name    : read_task
* Comments     :
*
*END*----------------------------------------------------------------------*/
void read_task
   (
      uint32_t param
   )
{
    _mqx_uint       data;
    MQX_FILE_PTR    i2s_ptr = (MQX_FILE_PTR)param;

    read(i2s_ptr, &data, 1);
}

/******************** Begin Testcases ********************/

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Test initialization
* Requirements : I2S000,I2S001,I2S002,I2S003,I2S004,
*                I2S005,I2S006,I2S007,I2S008,I2S064
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    _mqx_uint                   temp;
    _mqx_uint                   result;
    void                        *temp_ptr;
    void                        *sys_init_ptr;
    void                        *i2s_dev_ptr;
    GENERAL_I2S_INIT_STRUCT     *i2s_init_ptr;
    MQX_FILE_PTR                i2s_ptr;
    MEMORY_ALLOC_INFO_STRUCT    memory_alloc_info;

    /* open i2s drive */
    i2s_ptr = fopen(I2S_DEVICE, NULL);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, i2s_ptr != NULL , "Test #1.0: Test in normal operation");
    if (i2s_ptr == NULL)
    {
        eunit_test_stop();
        _mqx_exit(1);
    }

    i2s_dev_ptr = i2s_ptr->DEV_PTR->DRIVER_INIT_PTR;
    sys_init_ptr = get_init_data_ptr(i2s_dev_ptr);

    i2s_init_ptr = _mem_alloc(sizeof(GENERAL_I2S_INIT_STRUCT));
    _mem_copy(sys_init_ptr, i2s_init_ptr, sizeof(GENERAL_I2S_INIT_STRUCT));

    result = fclose(i2s_ptr);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, result == MQX_OK , "Test #1.1: Test in normal operation");
    if (result != MQX_OK)
    {
        eunit_test_stop();
        _mqx_exit(1);
    }

    set_init_data_ptr(i2s_dev_ptr, NULL);

    fopen(I2S_DEVICE, NULL);
#ifdef BSPCFG_ENABLE_SAI
    result = (_task_errno != MQX_OK);
#else
    result = (_task_errno == I2S_ERROR_INVALID_PARAMETER );
#endif
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, result, "Test #1.2: Test with initial structure pointer is NULL");
    _task_set_error(MQX_OK);

    set_init_data_ptr(i2s_dev_ptr, i2s_init_ptr);

    /* test with unsupported word length */
    temp = i2s_init_ptr->DATA_BITS;
    i2s_init_ptr->DATA_BITS = INVALID_DATA_BITS;

    fopen(I2S_DEVICE, NULL);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, _task_errno == I2S_ERROR_WORD_LENGTH_UNSUPPORTED , "Test #1.3: Test with unsupported word length");
    _task_set_error(MQX_OK);
    i2s_init_ptr->DATA_BITS = temp;

    /* test with invalid channel */
    temp = i2s_init_ptr->HW_CHANNEL;
    i2s_init_ptr->HW_CHANNEL = INVALID_CHANNEL;

    fopen(I2S_DEVICE, NULL);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, _task_errno == I2S_ERROR_CHANNEL_INVALID , "Test #1.4: Test with invalid channel");
    _task_set_error(MQX_OK);
    i2s_init_ptr->HW_CHANNEL = temp;

#ifdef BSPCFG_ENABLE_SAI
    temp = i2s_init_ptr->TX_CHANNEL;
    i2s_init_ptr->TX_CHANNEL = INVALID_CHANNEL;
    fopen(I2S_DEVICE, NULL);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, _task_errno == I2S_ERROR_INVALID_TX_CHANNEL , "Test #1.4.1: Test with invalid channel");
    _task_set_error(MQX_OK);
    i2s_init_ptr->TX_CHANNEL = temp;

    temp = i2s_init_ptr->RX_CHANNEL;
    i2s_init_ptr->RX_CHANNEL = INVALID_CHANNEL;
    fopen(I2S_DEVICE, NULL);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, _task_errno == I2S_ERROR_INVALID_RX_CHANNEL , "Test #1.4.2: Test with invalid channel");
    _task_set_error(MQX_OK);
    i2s_init_ptr->RX_CHANNEL = temp;
#endif

    /* test with invalid MODE */
    temp = i2s_init_ptr->MODE;
    i2s_init_ptr->MODE = INVALID_MODE;

    fopen(I2S_DEVICE, NULL);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, _task_errno == I2S_ERROR_MODE_INVALID , "Test #1.5: Test with invalid MODE");
    _task_set_error(MQX_OK);
    i2s_init_ptr->MODE = temp;

    /* test with invalid Clock source */
    temp = i2s_init_ptr->CLOCK_SOURCE;
    i2s_init_ptr->CLOCK_SOURCE = I2S_CLK_EXT + 1;

    fopen(I2S_DEVICE, NULL);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, _task_errno == I2S_ERROR_CLK_INVALID , "Test #1.6: Test with invalid Clock source");
    _task_set_error(MQX_OK);
    i2s_init_ptr->CLOCK_SOURCE = temp;

    /* test with invalid buffer size */
    temp = i2s_init_ptr->BUFFER_SIZE;
    i2s_init_ptr->BUFFER_SIZE = 1;

    fopen(I2S_DEVICE, NULL);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, _task_errno == I2S_ERROR_BUFFER_SMALL , "Test #1.7: Test with invalid buffer size");
    _task_set_error(MQX_OK);
    i2s_init_ptr->BUFFER_SIZE = temp;

    /* test with invalid audio data format */
    temp_ptr = (void *) i2s_init_ptr->IO_FORMAT;
    i2s_init_ptr->IO_FORMAT = &audio_data_invalid;

    fopen(I2S_DEVICE, NULL);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, _task_errno == AUDIO_ERROR_INVALID_IO_FORMAT , "Test #1.8: Test with invalid audio data format");
    _task_set_error(MQX_OK);
    i2s_init_ptr->IO_FORMAT = temp_ptr;

    set_init_data_ptr(i2s_dev_ptr, sys_init_ptr);
    _mem_free(i2s_init_ptr);

    /* the all free memory is allocated */
    memory_alloc_all(&memory_alloc_info);
    _task_set_error(MQX_OK);
    /* test with memory was fully allocated */
    i2s_ptr = fopen(I2S_DEVICE, NULL);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, _task_errno == MQX_OUT_OF_MEMORY , "Test #1.9: Test with memory fully allocated");
    memory_free_all(&memory_alloc_info);
    _task_set_error(MQX_OK);

    /* test in normal operation */
    i2s_ptr = fopen(I2S_DEVICE, NULL);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, _task_errno == MQX_OK , "Test #1.10: Test in normal operation");
    _task_set_error(MQX_OK);

    /* test when device already opened */
    fopen(I2S_DEVICE, NULL);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, _task_errno == MQX_IO_OPERATION_NOT_AVAILABLE , "Test #1.11: Test device already openned");
    _task_set_error(MQX_OK);

    /* test in normal operation */
    result = fclose(i2s_ptr);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, result == MQX_OK , "Test #1.12: Test close device in normal operation");
    _task_set_error(MQX_OK);
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2: Test deinitialization
* Requirements : I2S000,I2S010,I2S011
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    _mqx_uint       result;
    MQX_FILE_PTR    i2s_ptr, i2s_cpy_ptr;

    /* open the device for test cases */
    i2s_ptr = fopen(I2S_DEVICE, NULL);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, i2s_ptr != NULL , "Test #2.0 Test open the i2s device");
    _task_set_error(MQX_OK);
    if (i2s_ptr == NULL)
    {
        eunit_test_stop();
        _mqx_exit(1);
    }
    i2s_cpy_ptr = _mem_alloc(sizeof(MQX_FILE));
    _mem_copy(i2s_ptr, i2s_cpy_ptr, sizeof(MQX_FILE));

    /* test in normal operation */
    result = fclose(i2s_cpy_ptr);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, result == MQX_OK , "Test #2.1: Test close device in normal operation");
    _task_set_error(MQX_OK);

    //TODO: ENGR00277380 [SAI] calling fclose cause exception on cw10gcc on release target
    /* test close the device already closed */
    result = fclose(i2s_ptr);
#ifdef BSPCFG_ENABLE_SAI
    result = (result == I2S_ERROR_INVALID_PARAMETER);
#else
    result = (result == MQX_IO_OPERATION_NOT_AVAILABLE);
#endif
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, result, "Test #2.2 Test close the device already closed");
    _task_set_error(MQX_OK);

    /* test with NULL pointer */
    result = fclose(NULL);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, result != MQX_OK , "Test #2.3: Test close device with NULL pointer");
    _task_set_error(MQX_OK);
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : TEST #3: Test set and get driver mode
* Requirements : I2S000,I2S010,I2S016,I2S018,I2S046
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
    _mqx_uint       result;
    uint8_t         param, i2s_mode;
    MQX_FILE_PTR    i2s_ptr;

    /* open the device for test cases */
    i2s_ptr = fopen(I2S_DEVICE, NULL);
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, i2s_ptr != NULL , "Test #3.0 Test open the i2s device");
    _task_set_error(MQX_OK);
    if (i2s_ptr == NULL)
    {
        eunit_test_stop();
        _mqx_exit(1);
    }

    param = SLAVE_MODE;
    /* set device in to slave mode */
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_MODE_SLAVE, &param);
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, result == MQX_OK , "Test #3.1: Test set device to slave mode");
    _task_set_error(MQX_OK);

    /* test get device mode */
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_GET_MODE, &i2s_mode);
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, i2s_mode == param , "Test #3.2: Test device mode should be correct");
    _task_set_error(MQX_OK);

    param = MASTER_MODE;
    /* set device in to master mode */
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_MODE_MASTER, &param);
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, result == MQX_OK , "Test #3.3: Test set device to master mode");
    _task_set_error(MQX_OK);

    /* test get device mode */
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_GET_MODE, &i2s_mode);
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, i2s_mode == param , "Test #3.4: Test device mode should be correct");
    _task_set_error(MQX_OK);

    /* test in normal operation */
    result = fclose(i2s_ptr);
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, result == MQX_OK , "Test #3.5: Test close device in normal operation");
    _task_set_error(MQX_OK);
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_4_main_task
* Comments     : TEST #4: Test set and get clock source
* Requirements : I2S000,I2S010,I2S020,I2S022,I2S023,I2S025,I2S047
*
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
    _mqx_uint   result;
    uint8_t     param, clock_source;
    MQX_FILE_PTR    i2s_ptr;

    /* open the device for test cases */
    i2s_ptr = fopen(I2S_DEVICE, NULL);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, i2s_ptr != NULL , "Test #4.0 Test open the i2s device");
    _task_set_error(MQX_OK);
    if (i2s_ptr == NULL)
    {
        eunit_test_stop();
        _mqx_exit(1);
    }

    param = SLAVE_MODE;
    /* set device to slave mode */
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_MODE_SLAVE, &param);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, result == MQX_OK , "Test #4.1: Test set device to slave mode");
    _task_set_error(MQX_OK);

    /* test it is unable to change clock source in slave mode */
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_CLOCK_SOURCE_EXT, NULL);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, result == IO_ERROR_INVALID_IOCTL_CMD , "Test #4.2: Test clock source cannot be changed in slave mode");
    _task_set_error(MQX_OK);

    /* test it is unable to change clock source in slave mode */
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_CLOCK_SOURCE_INT, NULL);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, result == IO_ERROR_INVALID_IOCTL_CMD , "Test #4.3: Test clock source cannot be changed in slave mode");
    _task_set_error(MQX_OK);

    param = MASTER_MODE;
    /* set to master mode */
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_MODE_MASTER, &param);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, result == MQX_OK , "Test #4.4: Test set device to master mode");
    _task_set_error(MQX_OK);

    /* test clock source is set successfully*/
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_CLOCK_SOURCE_EXT, NULL);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, result == MQX_OK , "Test #4.5: Test clock source successfully changed in master mode");
    _task_set_error(MQX_OK);

    /* test clock source was set correctly */
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_GET_CLOCK_SOURCE, &clock_source);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, clock_source == I2S_CLK_EXT , "Test #4.6: Test clock source was set correctly");
    _task_set_error(MQX_OK);

    /* test set clock source to internal source*/
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_CLOCK_SOURCE_INT, NULL);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, result == MQX_OK , "Test #4.7: Test set to internal source");
    _task_set_error(MQX_OK);

    /* test clock source was set correctly */
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_GET_CLOCK_SOURCE, &clock_source);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, clock_source == I2S_CLK_INT , "Test #4.8: Test clock source was set correctly");
    _task_set_error(MQX_OK);

    /* test in normal operation */
    result = fclose(i2s_ptr);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, result == MQX_OK , "Test #4.9: Test close device in normal operation");
    _task_set_error(MQX_OK);
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_5_main_task
* Comments     : TEST #5: Test set and get data bits
* Requirements : I2S000,I2S010,I2S026,I2S028,I2S048
*
*END*---------------------------------------------------------------------*/
void tc_5_main_task(void)
{
    _mqx_uint   result;
    uint8_t     data_bits;
    MQX_FILE_PTR    i2s_ptr;

    /* open the device for test cases */
    i2s_ptr = fopen(I2S_DEVICE, NULL);
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, i2s_ptr != NULL , "Test #5.0 Test open the i2s device");
    _task_set_error(MQX_OK);
    if (i2s_ptr == NULL)
    {
        eunit_test_stop();
        _mqx_exit(1);
    }

    /* test with valid word length */
    data_bits = 8;
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_DATA_BITS, &data_bits);
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, result == MQX_OK , "Test #5.1: Test set word length with valid value");
    _task_set_error(MQX_OK);

    /* test set word length to unsupported value */
    data_bits = 1;
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_DATA_BITS, &data_bits);
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, result == I2S_ERROR_WORD_LENGTH_UNSUPPORTED , "Test #5.2: Test set word length with unsupported value");
    _task_set_error(MQX_OK);

    /* test if word length was not changed to invalid value */
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_GET_DATA_BITS, &data_bits);
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, data_bits == 8 , "Test #5.3: Test word length should not be changed to invalid value");
    _task_set_error(MQX_OK);

    /* test with valid word length */
    data_bits = 16;
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_DATA_BITS, &data_bits);
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, result == MQX_OK , "Test #5.4: Test set word length with valid value");
    _task_set_error(MQX_OK);

    /* test read value must be correct */
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_GET_DATA_BITS, &data_bits);
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, data_bits == 16 , "Test #5.5: Test word length was set correctly");
    _task_set_error(MQX_OK);

    /* test in normal operation */
    result = fclose(i2s_ptr);
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, result == MQX_OK , "Test #5.6: Test close device in normal operation");
    _task_set_error(MQX_OK);
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_6_main_task
* Comments     : TEST #6: Test set and get master clock frequency
* Requirements : I2S000,I2S010,I2S031,I2S033,I2S049
*
*END*---------------------------------------------------------------------*/
void tc_6_main_task(void)
{
    _mqx_uint   result;
    uint32_t        frequency, freq_read;
    MQX_FILE_PTR    i2s_ptr;

    /* open the device for test cases */
    i2s_ptr = fopen(I2S_DEVICE, NULL);
    EU_ASSERT_REF_TC_MSG( tc_6_main_task, i2s_ptr != NULL , "Test #6.0 Test open the i2s device");
    _task_set_error(MQX_OK);
    if (i2s_ptr == NULL)
    {
        eunit_test_stop();
        _mqx_exit(1);
    }

    /* set clock source to internal clock */
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_CLOCK_SOURCE_INT, NULL);
    EU_ASSERT_REF_TC_MSG( tc_6_main_task, result == MQX_OK , "Test #6.1: Test set clock source to internal clock");
    _task_set_error(MQX_OK);

    /* set master clock frequency with invalid frequency */
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_MCLK_FREQ, NULL);
    EU_ASSERT_REF_TC_MSG( tc_6_main_task, result == I2S_ERROR_INVALID_PARAMETER , "Test #6.2: Test set master clock frequency with invalid value");
    _task_set_error(MQX_OK);

#ifndef BSPCFG_ENABLE_SAI
    // TODO: [ENGR00276560] Driver does not return correctly when set unsupportable frequency
    /* set master clock frequency with invalid frequency */
    frequency = BSP_SYSTEM_CLOCK;
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_MCLK_FREQ, &frequency);
    EU_ASSERT_REF_TC_MSG( tc_6_main_task, result == I2S_ERROR_DIVIDER_VALUE_INVALID, "Test #6.3: Test set master clock frequency with invalid value");
    _task_set_error(MQX_OK);
#endif

    /* set master clock frequency with valid frequency */
    frequency = I2S_MCLK_FREQ_TEST;
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_MCLK_FREQ, &frequency);
    EU_ASSERT_REF_TC_MSG( tc_6_main_task, result == MQX_OK , "Test #6.4: Test set master clock frequency with valid value");
    _task_set_error(MQX_OK);

    /* test frequency was set correctly */
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_GET_MCLK_FREQ, &freq_read);
    EU_ASSERT_REF_TC_MSG( tc_6_main_task, ABS_DIFF(freq_read, frequency) <= FREQ_TOLERANCE , "Test #6.5: Test frequency was set correctly");
    _task_set_error(MQX_OK);

    /* test in normal operation */
    result = fclose(i2s_ptr);
    EU_ASSERT_REF_TC_MSG( tc_6_main_task, result == MQX_OK , "Test #6.6: Test close device in normal operation");
    _task_set_error(MQX_OK);

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_7_main_task
* Comments     : TEST #7: Test set and get frame sync frequency
* Requirements : I2S000,I2S010,I2S016,I2S018,I2S034,I2S036,I2S048,I2S050,I2S052
*
*END*---------------------------------------------------------------------*/
void tc_7_main_task(void)
{
    _mqx_uint           result;
    uint32_t            frequency, freq_read;
    uint8_t             param, data_bits;
    MQX_FILE_PTR        i2s_ptr;

    /* open the device for test cases */
    i2s_ptr = fopen(I2S_DEVICE, NULL);
    EU_ASSERT_REF_TC_MSG( tc_7_main_task, i2s_ptr != NULL , "Test #7.0 Test open the i2s device");
    _task_set_error(MQX_OK);
    if (i2s_ptr == NULL)
    {
        eunit_test_stop();
        _mqx_exit(1);
    }

    param = SLAVE_MODE;
    /* set to slave mode */
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_MODE_SLAVE, &param);
    EU_ASSERT_REF_TC_MSG( tc_7_main_task, result == MQX_OK , "Test #7.1: Test set device to slave mode");
    _task_set_error(MQX_OK);

    /* set master clock frequency with valid frequency */
    frequency = I2S_MCLK_FREQ_TEST;
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_MCLK_FREQ, &frequency);
    EU_ASSERT_REF_TC_MSG( tc_7_main_task, result == MQX_OK , "Test #7.2: Test set master clock frequency with valid value");
    _task_set_error(MQX_OK);

    /* set frame sync frequency to invalid value */
    frequency = 0;
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_FS_FREQ, &frequency);
    EU_ASSERT_REF_TC_MSG( tc_7_main_task, result == I2S_ERROR_INVALID_PARAMETER, "Test #7.3: Test set frame sync frequency with invalid value");
    _task_set_error(MQX_OK);

    frequency = BSP_BUS_CLOCK;
    /* set frame sync frequency to invalid value */
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_FS_FREQ, &frequency);
#ifdef BSPCFG_ENABLE_SAI
    result = (result == I2S_ERROR_INVALID_PARAMETER);
#else
    //TODO: [ENGR00276560] Driver does not return correctly when set unsupportable frequency
    result = (result == I2S_ERROR_FREQUENCY_INVALID);
#endif
    EU_ASSERT_REF_TC_MSG( tc_7_main_task, result, "Test #7.4: Test set frame sync frequency with invalid value");
    _task_set_error(MQX_OK);

    /* get word length */
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_GET_DATA_BITS, &data_bits);
    EU_ASSERT_REF_TC_MSG( tc_7_main_task, result == MQX_OK , "Test #7.5: Test read word length operation");
    _task_set_error(MQX_OK);

    /* valid frequency */
    frequency = I2S_FS_FREQ_TEST;

    /* set frame sync with valid frequency */
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_FS_FREQ, &frequency);
    EU_ASSERT_REF_TC_MSG( tc_7_main_task, result == MQX_OK , "Test #7.6: Test set frame sync frequency with valid value");
    _task_set_error(MQX_OK);

    /* frame sync frequency set should be correct */
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_GET_FS_FREQ, &freq_read);
    EU_ASSERT_REF_TC_MSG( tc_7_main_task, ABS_DIFF(freq_read, frequency) <= FREQ_TOLERANCE , "Test #7.7: Test read frame sync frequency operation");
    _task_set_error(MQX_OK);

    frequency = freq_read;
    /* read bit clock frequency */
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_GET_BCLK_FREQ, &freq_read);
    result = ABS_DIFF(freq_read, frequency * data_bits * 2) <= FREQ_TOLERANCE;

    EU_ASSERT_REF_TC_MSG( tc_7_main_task, result , "Test #7.8: Test BCLK frequency should be correct");
    _task_set_error(MQX_OK);

    param = MASTER_MODE;
    /* set device to master mode */
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_MODE_MASTER, &param);
    EU_ASSERT_REF_TC_MSG( tc_7_main_task, result == MQX_OK , "Test #7.9: Test set device to master mode");
    _task_set_error(MQX_OK);

    /* the if frequency is not valid in master mode */
    frequency = BSP_BUS_CLOCK;
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_FS_FREQ, &frequency);
#ifdef BSPCFG_ENABLE_SAI
    result = (result == I2S_ERROR_INVALID_PARAMETER);
#else
    //TODO: [ENGR00276560] Driver does not return correctly when set unsupportable frequency
    result = (result == I2S_ERROR_DIVIDER_VALUE_INVALID);
#endif
    EU_ASSERT_REF_TC_MSG( tc_7_main_task, result , "Test #7.10: Test set FS frequency in master mode");
    _task_set_error(MQX_OK);

    /* set frequency should be MQX_OK */
    frequency = I2S_FS_FREQ_TEST;
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_FS_FREQ, &frequency);
    EU_ASSERT_REF_TC_MSG( tc_7_main_task, result == MQX_OK , "Test #7.11: Test set FS frequency in master mode");
    _task_set_error(MQX_OK);

    /* FS frequency should be set correctly */
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_GET_FS_FREQ, &freq_read);
    EU_ASSERT_REF_TC_MSG( tc_7_main_task, ABS_DIFF(freq_read, frequency) <= FREQ_TOLERANCE , "Test #7.12: Test FS frequency was set correctly");
    _task_set_error(MQX_OK);

    /* BLCK frequency should be correct */
    frequency = freq_read;
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_GET_BCLK_FREQ, &freq_read);
#ifdef BSPCFG_ENABLE_SAI
    result = ABS_DIFF(freq_read, frequency * data_bits * 2) <= FREQ_TOLERANCE;
#else
    result = ABS_DIFF(freq_read, frequency * 64) <= FREQ_TOLERANCE;
#endif
    EU_ASSERT_REF_TC_MSG( tc_7_main_task,  result, "Test #7.13: Test BCLK frequency should be correct");
    _task_set_error(MQX_OK);

    /* test in normal operation */
    result = fclose(i2s_ptr);
    EU_ASSERT_REF_TC_MSG( tc_7_main_task, result == MQX_OK , "Test #7.14: Test close device in normal operation");
    _task_set_error(MQX_OK);
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_8_main_task
* Comments     : TEST #8: Test set and get data format
* Requirements : I2S000,I2S010,I2S037,I2S038,I2S054
*
*END*---------------------------------------------------------------------*/
void tc_8_main_task(void)
{
    _mqx_uint           result;
    bool                compare_rslt;
    MQX_FILE_PTR        i2s_ptr;
    AUDIO_DATA_FORMAT   audio_read, audio_data = {
            AUDIO_LITTLE_ENDIAN,    /* Endian of input data */
            AUDIO_ALIGNMENT_LEFT,   /* Alignment of input data */
            8,                      /* Bit size of input data */
            1,                      /* Sample size in bytes */
            1                       /* Number of channels */
    };


    /* open the device for test cases */
    i2s_ptr = fopen(I2S_DEVICE, NULL);
    EU_ASSERT_REF_TC_MSG( tc_8_main_task, i2s_ptr != NULL , "Test #8.0 Test open the i2s device");
    _task_set_error(MQX_OK);
    if (i2s_ptr == NULL)
    {
        eunit_test_stop();
        _mqx_exit(1);
    }

    /* test with invalid audio data format */
    audio_data.BITS = 1;
    result = ioctl(i2s_ptr, IO_IOCTL_AUDIO_SET_IO_DATA_FORMAT, &audio_data);
    EU_ASSERT_REF_TC_MSG( tc_8_main_task, result == AUDIO_ERROR_INVALID_IO_FORMAT , "Test #8.1: Test set invalid audio data format");
    _task_set_error(MQX_OK);

    /* test with valid data format should be MQX_OK */
    audio_data.BITS = 8;
    result = ioctl(i2s_ptr, IO_IOCTL_AUDIO_SET_IO_DATA_FORMAT, &audio_data);
    EU_ASSERT_REF_TC_MSG( tc_8_main_task, result == MQX_OK , "Test #8.2: Test set audio data format operation");
    _task_set_error(MQX_OK);

    /* get audio data format */
    result = ioctl(i2s_ptr, IO_IOCTL_AUDIO_GET_IO_DATA_FORMAT, &audio_read);
    EU_ASSERT_REF_TC_MSG( tc_8_main_task, result == MQX_OK, "Test #8.3: Test get audio data format operation");
    _task_set_error(MQX_OK);

    //TODO: [ENGR00276562] Set and get audio data format are not identical
    /* test set and get data should be the same */
     compare_rslt = (audio_read.ALIGNMENT == audio_data.ALIGNMENT)
                && (audio_read.ENDIAN == audio_data.ENDIAN)
                && (audio_read.BITS == audio_data.BITS)
                && (audio_read.SIZE == audio_data.SIZE)
                && (audio_read.CHANNELS == audio_data.CHANNELS);

    EU_ASSERT_REF_TC_MSG( tc_8_main_task, compare_rslt , "Test #8.4: Test data read should be correct");

    /* test in normal operation */
    result = fclose(i2s_ptr);
    EU_ASSERT_REF_TC_MSG( tc_8_main_task, result == MQX_OK , "Test #8.5: Test close device in normal operation");
    _task_set_error(MQX_OK);
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_9_main_task
* Comments     : TEST #9: Test data transmit
* Requirements : I2S000,I2S010,I2S014,I2S015,I2S029,I2S030,I2S044
*                I2S045,I2S051,I2S053,I2S060
*
*END*---------------------------------------------------------------------*/
void tc_9_main_task(void)
{
    bool                    dummy = FALSE;
    uint8_t                 param;
    uint8_t                 *data_ptr, *temp_ptr;
    _mqx_uint               result, i;
    I2S_STATISTICS_STRUCT   stats;
    MQX_FILE_PTR            i2s_ptr;
    AUDIO_DATA_FORMAT       audio_format;

    /* open the device for test cases */
    i2s_ptr = fopen(I2S_DEVICE, NULL);
    EU_ASSERT_REF_TC_MSG( tc_9_main_task, i2s_ptr != NULL , "Test #9.0 Test open the i2s device");
    _task_set_error(MQX_OK);
    if (i2s_ptr == NULL)
    {
        eunit_test_stop();
        _mqx_exit(1);
    }

    param = MASTER_MODE;
    /* set device to master mode */
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_MODE_MASTER, &param);
    EU_ASSERT_REF_TC_MSG( tc_9_main_task, result == MQX_OK , "Test #9.1: Test set device to master mode");
    _task_set_error(MQX_OK);

    data_ptr = _mem_alloc(BYTE_SENT);
    for (i = 0; i < BYTE_SENT; i++)
    {
        data_ptr[i] = i;
    }

    /* get audio data format */
    result = ioctl(i2s_ptr, IO_IOCTL_AUDIO_GET_IO_DATA_FORMAT, &audio_format);
    EU_ASSERT_REF_TC_MSG( tc_9_main_task, result == MQX_OK, "Test #9.2: Test get audio data format operation");
    _task_set_error(MQX_OK);

    /* transmit data */
    result = write(i2s_ptr, data_ptr, BYTE_SENT);
    EU_ASSERT_REF_TC_MSG( tc_9_main_task, _task_errno == MQX_OK, "Test #9.3: Test transmit operation");
    _task_set_error(MQX_OK);

    /* flush output */
    result = ioctl(i2s_ptr, IO_IOCTL_FLUSH_OUTPUT, NULL);
    EU_ASSERT_REF_TC_MSG( tc_9_main_task, result == MQX_OK, "Test #9.4: Test transmit operation");

    /* get statistics and ensure number of bytes sent is correct */
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_GET_STATISTICS, &stats);
    EU_ASSERT_REF_TC_MSG( tc_9_main_task, BYTE_SENT == stats.TX_PACKETS * audio_format.CHANNELS, "Test #9.5: Test transmit operation");
    _task_set_error(MQX_OK);

    /* clear statistics */
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_CLEAR_STATISTICS, NULL);
    EU_ASSERT_REF_TC_MSG( tc_9_main_task, result == MQX_OK, "Test #9.6: Test clear statistics operation");
    _task_set_error(MQX_OK);

    /* get statistics and ensure it is cleared */
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_GET_STATISTICS, &stats);

    temp_ptr = (uint8_t *)&stats;
    for (i = 0; i < sizeof(stats); i++)
    {
        EU_ASSERT_REF_TC_MSG(tc_9_main_task, *(temp_ptr + i) == 0, "Test #9.7: Test transmit operation");
    }

    //TODO: [ENGR00276553] I2S driver does not transmit data after disable then enable by ioctl
    /* disable device */
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_DISABLE_DEVICE, NULL);
    EU_ASSERT_REF_TC_MSG( tc_9_main_task, result == MQX_OK , "Test #9.8: Test disable device operation");
    _task_set_error(MQX_OK);

    /* enable device */
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_ENABLE_DEVICE, NULL);
    EU_ASSERT_REF_TC_MSG( tc_9_main_task, result == MQX_OK , "Test #9.9: Test enable device operation");
    _task_set_error(MQX_OK);

    result = write(i2s_ptr, data_ptr, BYTE_SENT);
    EU_ASSERT_REF_TC_MSG( tc_9_main_task, _task_errno == MQX_OK, "Test #9.10: Test transmit operation");
    _task_set_error(MQX_OK);

    /* flush output data */
    result = ioctl(i2s_ptr, IO_IOCTL_FLUSH_OUTPUT, NULL);
    EU_ASSERT_REF_TC_MSG( tc_9_main_task, result == MQX_OK, "Test #9.11: Test transmit operation");

    /* get statistics and ensure number of bytes sent is correct */
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_GET_STATISTICS, &stats);
    EU_ASSERT_REF_TC_MSG( tc_9_main_task, BYTE_SENT == stats.TX_PACKETS * audio_format.CHANNELS, "Test #9.12: Test transmit operation");
    _task_set_error(MQX_OK);

    /* clear statistics */
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_CLEAR_STATISTICS, NULL);
    EU_ASSERT_REF_TC_MSG( tc_9_main_task, result == MQX_OK, "Test #9.13: Test clear statistics operation");
    _task_set_error(MQX_OK);

    _mem_free(data_ptr);

#if TEST_DUMMY_MODE
    /* turn on dummy mode */
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_TX_DUMMY_ON, NULL);
    EU_ASSERT_REF_TC_MSG( tc_9_main_task, result == MQX_OK , "Test #9.14: Test turn on dummy mode operation");
    _task_set_error(MQX_OK);

    /* ensure dummy mode is on */
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_GET_TX_DUMMY, &dummy);
    EU_ASSERT_REF_TC_MSG( tc_9_main_task, dummy == TRUE, "Test #9.15: Test get dummy mode operation");
    _task_set_error(MQX_OK);

    /* write NULL in dummy mode */
    result = write(i2s_ptr, NULL, 0);
    EU_ASSERT_REF_TC_MSG( tc_9_main_task, _task_errno == MQX_OK, "Test #9.16: Test transmits dummy data");
    _task_set_error(MQX_OK);

    _time_delay(1000);

    /* get statistics and ensure number of bytes sent is correct */
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_GET_STATISTICS, &stats);
    EU_ASSERT_REF_TC_MSG( tc_9_main_task, stats.TX_PACKETS > 0, "Test #9.17: Test transmit dummy mode");
    _task_set_error(MQX_OK);

    /* turn off dummy mode */
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_TX_DUMMY_OFF, NULL);
    EU_ASSERT_REF_TC_MSG( tc_9_main_task, result == MQX_OK , "Test #9.18: Test turn off dummy mode operation");
    _task_set_error(MQX_OK);

    /* ensure dummy mode is off */
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_GET_TX_DUMMY, &dummy);
    EU_ASSERT_REF_TC_MSG( tc_9_main_task, dummy == FALSE, "Test #9.19: Test get dummy mode operation");
    _task_set_error(MQX_OK);

    /* write NULL data in dummy mode is off */
    result = write(i2s_ptr, NULL, 0);
    EU_ASSERT_REF_TC_MSG( tc_9_main_task, _task_errno == I2S_ERROR_INVALID_PARAMETER, "Test #9.20: Test transmit NULL data should be failed");
    _task_set_error(MQX_OK);
#endif

    /* test in normal operation */
    result = fclose(i2s_ptr);
    EU_ASSERT_REF_TC_MSG( tc_9_main_task, result == MQX_OK , "Test #9.21: Test close device in normal operation");
    _task_set_error(MQX_OK);

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_10_main_task
* Comments     : TEST #10: Test while device is in progress
* Requirements : I2S000,I2S010,I2S017,I2S019,I2S021,I2S024,I2S027
*                I2S032,I2S035,I2S039
*
*END*---------------------------------------------------------------------*/
void tc_10_main_task(void)
{
    uint8_t         param;
    _mqx_uint       result, tid;
    volatile MQX_FILE_PTR    i2s_ptr;

    /* open the device for test cases */
    i2s_ptr = fopen(I2S_DEVICE, NULL);
    EU_ASSERT_REF_TC_MSG( tc_10_main_task, i2s_ptr != NULL , "Test #10.0 Test open the i2s device");
    _task_set_error(MQX_OK);
    if (i2s_ptr == NULL)
    {
        eunit_test_stop();
        _mqx_exit(1);
    }

    param = SLAVE_MODE;
    /* set to slave mode */
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_MODE_SLAVE, &param);
    EU_ASSERT_REF_TC_MSG( tc_10_main_task, result == MQX_OK , "Test #10.1: Test set device to slave mode");
    _task_set_error(MQX_OK);

    /* create i2s_task and make it run */
    tid = _task_create(0, TEST_TASK, (uint32_t)i2s_ptr);

    /* device is busy now, stop preemption */
    _task_stop_preemption();

    param = MASTER_MODE;
    /* cannot set to master mode */
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_MODE_MASTER, &param);
    EU_ASSERT_REF_TC_MSG( tc_10_main_task, result == I2S_ERROR_DEVICE_BUSY , "Test #10.2: Test while device is busy");
    _task_set_error(MQX_OK);

    param = SLAVE_MODE;
    /* cannot set to slave mode */
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_MODE_SLAVE, &param);
    EU_ASSERT_REF_TC_MSG( tc_10_main_task, result == I2S_ERROR_DEVICE_BUSY , "Test #10.3: Test while device is busy");
    _task_set_error(MQX_OK);

    /* cannot set clock source */
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_CLOCK_SOURCE_INT, NULL);
    EU_ASSERT_REF_TC_MSG( tc_10_main_task, result == I2S_ERROR_DEVICE_BUSY , "Test #10.4: Test while device is busy");
    _task_set_error(MQX_OK);

    /* cannot set clock source */
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_CLOCK_SOURCE_EXT, NULL);
    EU_ASSERT_REF_TC_MSG( tc_10_main_task, result == I2S_ERROR_DEVICE_BUSY , "Test #10.5: Test while device is busy");
    _task_set_error(MQX_OK);

    /* cannot set word length */
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_DATA_BITS, NULL);
    EU_ASSERT_REF_TC_MSG( tc_10_main_task, result == I2S_ERROR_DEVICE_BUSY , "Test #10.6: Test while device is busy");
    _task_set_error(MQX_OK);

    /* cannot set MCLK frequency */
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_MCLK_FREQ, NULL);
    EU_ASSERT_REF_TC_MSG( tc_10_main_task, result == I2S_ERROR_DEVICE_BUSY , "Test #10.7: Test while device is busy");
    _task_set_error(MQX_OK);

    /* cannot set frame sync frequency */
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_FS_FREQ, NULL);
    EU_ASSERT_REF_TC_MSG( tc_10_main_task, result == I2S_ERROR_DEVICE_BUSY , "Test #10.8: Test while device is busy");
    _task_set_error(MQX_OK);

    /* cannot set audio data format */
    result = ioctl(i2s_ptr, IO_IOCTL_AUDIO_SET_IO_DATA_FORMAT, NULL);
    EU_ASSERT_REF_TC_MSG( tc_10_main_task, result == I2S_ERROR_DEVICE_BUSY , "Test #10.9: Test while device is busy");
    _task_set_error(MQX_OK);

    _task_abort(tid);

    result = fclose(i2s_ptr);
    EU_ASSERT_REF_TC_MSG( tc_10_main_task, result == MQX_OK , "Test #10.10: Test close device");

    _task_start_preemption();
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_11_main_task
* Comments     : TEST #11: Test set and get water mark
* Requirements : I2S000,I2S010,I2S040,I2S041,I2S042,I2S043,
*                I2S055,I2S056
*
*END*---------------------------------------------------------------------*/
void tc_11_main_task()
{
    uint8_t         water_mark;
    _mqx_uint       result;
    MQX_FILE_PTR    i2s_ptr;

    /* open the device for test cases */
    i2s_ptr = fopen(I2S_DEVICE, NULL);
    EU_ASSERT_REF_TC_MSG( tc_11_main_task, i2s_ptr != NULL , "Test #11.0 Test open the i2s device");
    _task_set_error(MQX_OK);
    if (i2s_ptr == NULL)
    {
        eunit_test_stop();
        _mqx_exit(1);
    }

    result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_TXFIFO_WATERMARK, NULL);
    EU_ASSERT_REF_TC_MSG( tc_11_main_task, result == I2S_ERROR_INVALID_PARAMETER , "Test #11.1: Test set TXFIFO water mark with NULL parameter");
    _task_set_error(MQX_OK);

    water_mark = 0xff;
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_TXFIFO_WATERMARK, &water_mark);
    EU_ASSERT_REF_TC_MSG( tc_11_main_task, result == I2S_ERROR_PARAM_OUT_OF_RANGE , "Test #11.2: Test set TXFIFO water mark with parameter out of range");
    _task_set_error(MQX_OK);

    water_mark = FIFO_WATERMARK_TEST;
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_TXFIFO_WATERMARK, &water_mark);
    EU_ASSERT_REF_TC_MSG( tc_11_main_task, result == MQX_OK , "Test #11.3: Test set TXFIFO water mark with valid parameter");
    _task_set_error(MQX_OK);

    result = ioctl(i2s_ptr, IO_IOCTL_I2S_GET_TXFIFO_WATERMARK, &water_mark);
    EU_ASSERT_REF_TC_MSG( tc_11_main_task, water_mark == FIFO_WATERMARK_TEST , "Test #11.4: Get TXFIFO water mark should be correct");
    _task_set_error(MQX_OK);

    result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_RXFIFO_WATERMARK, NULL);
    EU_ASSERT_REF_TC_MSG( tc_11_main_task, result == I2S_ERROR_INVALID_PARAMETER , "Test #11.5: Test set RXFIFO water mark with NULL parameter");
    _task_set_error(MQX_OK);

    water_mark = 0xff;
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_RXFIFO_WATERMARK, &water_mark);
    EU_ASSERT_REF_TC_MSG( tc_11_main_task, result == I2S_ERROR_PARAM_OUT_OF_RANGE , "Test #11.6: Test set RXFIFO water mark with parameter out of range");
    _task_set_error(MQX_OK);

    water_mark = FIFO_WATERMARK_TEST;
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_RXFIFO_WATERMARK, &water_mark);
    EU_ASSERT_REF_TC_MSG( tc_11_main_task, result == MQX_OK , "Test #11.7: Test set RXFIFO water mark with valid parameter");
    _task_set_error(MQX_OK);

    result = ioctl(i2s_ptr, IO_IOCTL_I2S_GET_RXFIFO_WATERMARK, &water_mark);
    EU_ASSERT_REF_TC_MSG( tc_11_main_task, water_mark == FIFO_WATERMARK_TEST , "Test #11.8: Get RXFIFO water mark should be correct");
    _task_set_error(MQX_OK);

    result = fclose(i2s_ptr);
    EU_ASSERT_REF_TC_MSG( tc_11_main_task, result == MQX_OK , "Test #11.9: Test close device");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_12_main_task
* Comments     : TEST #12: Test set and get clock mode
* Requirements : I2S000,I2S010,I2S057,I2S058,I2S059
*
*END*---------------------------------------------------------------------*/
void tc_12_main_task()
{
    uint8_t         param;
    _mqx_uint       result;
    MQX_FILE_PTR    i2s_ptr;

    /* open the device for test cases */
    i2s_ptr = fopen(I2S_DEVICE, NULL);
    EU_ASSERT_REF_TC_MSG( tc_12_main_task, i2s_ptr != NULL , "Test #12.0 Test open the i2s device");
    _task_set_error(MQX_OK);
    if (i2s_ptr == NULL)
    {
        eunit_test_stop();
        _mqx_exit(1);
    }

#ifdef BSPCFG_ENABLE_SAI
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_CLOCK_MODE, NULL);
    EU_ASSERT_REF_TC_MSG( tc_12_main_task, result == I2S_ERROR_INVALID_PARAMETER, "Test #12.1: Set clock mode with NULL pointer");
    _task_set_error(MQX_OK);

    param = I2S_TX_SYNCHRONOUS | I2S_RX_SYNCHRONOUS;
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_CLOCK_MODE, &param);
    EU_ASSERT_REF_TC_MSG( tc_12_main_task, result == I2S_ERROR_INVALID_PARAMETER, "Test #12.2: Set invalid clock mode");
    _task_set_error(MQX_OK);

    param = I2S_TX_ASYNCHRONOUS | I2S_RX_ASYNCHRONOUS;
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_CLOCK_MODE, &param);
    EU_ASSERT_REF_TC_MSG( tc_12_main_task, result == MQX_OK, "Test #12.3: Set valid clock mode should be success");
    _task_set_error(MQX_OK);

    // TODO: ENGR00279174: Clock mode set and get values are not identical in SAI driver
    ioctl(i2s_ptr, IO_IOCTL_I2S_GET_CLOCK_MODE, &result);
    EU_ASSERT_REF_TC_MSG( tc_12_main_task, (result & param) == param, "Test #12.4: Get clock mode should be correct");
    _task_set_error(MQX_OK);

    param = I2S_TX_BCLK_NORMAL | I2S_RX_BCLK_NORMAL;
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_CLOCK_MODE, &param);
    EU_ASSERT_REF_TC_MSG( tc_12_main_task, result == MQX_OK, "Test #12.5: Set valid clock mode should be success");
    _task_set_error(MQX_OK);

    ioctl(i2s_ptr, IO_IOCTL_I2S_GET_CLOCK_MODE, &result);
    EU_ASSERT_REF_TC_MSG( tc_12_main_task, (result & param) == param, "Test #12.6: Get clock mode should be correct");
    _task_set_error(MQX_OK);

    result = ioctl(i2s_ptr, IO_IOCTL_I2S_GET_CLOCK_MODE, NULL);
    EU_ASSERT_REF_TC_MSG( tc_12_main_task, result == I2S_ERROR_INVALID_PARAMETER, "Test #12.7: Get clock mode with NULL pointer");
    _task_set_error(MQX_OK);

#else
    result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_CLK_ALWAYS_ENABLED_OFF, NULL);
    EU_ASSERT_REF_TC_MSG( tc_12_main_task, result == MQX_OK, "Test #12.1: Test set clock always enabled off");
    _task_set_error(MQX_OK);

    result = ioctl(i2s_ptr, IO_IOCTL_I2S_GET_CLK_ALWAYS_ENABLED, &param);
    EU_ASSERT_REF_TC_MSG( tc_12_main_task, param == FALSE, "Test #12.2: get clock always enabled status");
    _task_set_error(MQX_OK);

    result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_CLK_ALWAYS_ENABLED_ON, NULL);
    EU_ASSERT_REF_TC_MSG( tc_12_main_task, result == MQX_OK, "Test #12.3: Test set clock always enabled on");
    _task_set_error(MQX_OK);

    result = ioctl(i2s_ptr, IO_IOCTL_I2S_GET_CLK_ALWAYS_ENABLED, &param);
    EU_ASSERT_REF_TC_MSG( tc_12_main_task, param == TRUE, "Test #12.4: get clock always enabled status");
    _task_set_error(MQX_OK);

    result = ioctl(i2s_ptr, IO_IOCTL_I2S_GET_CLK_ALWAYS_ENABLED, NULL);
    EU_ASSERT_REF_TC_MSG( tc_12_main_task, result == I2S_ERROR_INVALID_PARAMETER, "Test #12.5: get clock always enabled status");
    _task_set_error(MQX_OK);

#endif

    result = fclose(i2s_ptr);
    EU_ASSERT_REF_TC_MSG( tc_12_main_task, result == MQX_OK , "Test #12.8: Test close device");

}
/******************** End Testcases ********************/

//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_1)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST #1 - Test initialization"),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST #2 - Test deinitialization"),
    EU_TEST_CASE_ADD(tc_3_main_task, " TEST #3 - Test set and get driver mode"),
    EU_TEST_CASE_ADD(tc_4_main_task, " TEST #4 - Test set and get clock source"),
    EU_TEST_CASE_ADD(tc_5_main_task, " TEST #5 - Test set and get data bits"),
    EU_TEST_CASE_ADD(tc_6_main_task, " TEST #6 - Test set and get master clock frequency"),
    EU_TEST_CASE_ADD(tc_7_main_task, " TEST #7 - Test set and get frame sync frequency"),
    EU_TEST_CASE_ADD(tc_8_main_task, " TEST #8 - Test set and get data format"),
    EU_TEST_CASE_ADD(tc_9_main_task, " TEST #9 - Test transmitting operation"),
    EU_TEST_CASE_ADD(tc_10_main_task, " TEST #10 - Test while device is in progress"),
    EU_TEST_CASE_ADD(tc_11_main_task, " TEST #11 - Test set and get fifo water mark"),
    EU_TEST_CASE_ADD(tc_12_main_task, " TEST #12 - Test set and get clock mode"),
EU_TEST_SUITE_END(suite_1)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_1, " - Internal test for i2s module")
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
   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();
}/* Endbody */
/* EOF */
