/****************************************************************************
* 
*   This file contains MQX only stationery code.
*
****************************************************************************/
#include <mqx.h>
#include <bsp.h>
#include <mqx_prv.h>
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"

#if BSPCFG_ENABLE_SAI
#include <sai.h>
#else
#include <i2s.h>
#endif

#include "test.h"

#define MAIN_TASK	1
#define WRITE_TASK  2

#define I2S_BAUDRATE_TEST		1000000
#define I2S_DATA_BITS_TEST		16

#define NUMBER_OF_PACKETS_TEST	48
#define NUMBER_OF_BYTES_TEST	(NUMBER_OF_PACKETS_TEST * sizeof(uint32_t))

#if BSPCFG_ENABLE_SAI
#define I2S_PORT				"sai:"
#else
#define I2S_PORT				"ii2s0:"
#endif

#ifndef BSPCFG_ENABLE_SAI
#define I2S_DISABLE	{ \
	I2S0_IER &= ~(I2S_IER_RIE_MASK); \
	I2S0_IER &= ~(I2S_IER_TIE_MASK); \
	I2S0_CR &= ~(I2S_CR_RE_MASK); \
	I2S0_CR &= ~(I2S_CR_TE_MASK); \
} 
#else
#define	I2S_DISABLE	ioctl(i2s_ptr, IO_IOCTL_I2S_DISABLE_DEVICE, NULL)
#endif

void tc_1_main_task(); // " TEST 1 - Test with data bits 8 "
void tc_2_main_task(); //  " TEST 2 - Test with data bits 10 "
void tc_3_main_task(); //  " TEST 3 - Test with data bits 12 "
void tc_4_main_task(); //  " TEST 4 - Test with data bits 18 "
void tc_5_main_task(); //  " TEST 5 - Test with data bits 20 "
void tc_6_main_task(); //  " TEST 6 - Test with data bits 22 "
void tc_7_main_task(); //  " TEST 7 - Test with data bits 24 "
void tc_8_main_task(); //  " TEST 8 - Test with data bits 16 "
void tc_9_main_task(); //  " TEST 9 - Test master read "
void tc_10_main_task(); //  " TEST10 - Test slave write "),
void tc_11_main_task(); //  " TEST 11 - Test slave read "),
void tc_12_main_task(); //  " TEST 12 - Test wave generator "),
    
void main_task(uint32_t);
void write_task(uint32_t);

TASK_TEMPLATE_STRUCT MQX_template_list[] = 
{ 
/*  Task number, Entry point, Stack, Pri, String, Auto? */
   {MAIN_TASK,   main_task,   10000,  7,   "main", MQX_AUTO_START_TASK}, 
   {WRITE_TASK,  write_task,  2000,  8,   "write", 0				},
   {0,           0,           0,     0,   0,      0,                 }
};

/* Global variables */
MQX_FILE_PTR		i2s_ptr;
uint32_t 			fs_freq = I2S_BAUDRATE_TEST >> 6;	
uint32_t 			mclk_freq = I2S_BAUDRATE_TEST << 2;
int16_t			*i2s_wave;
uint32_t				i2s_data[NUMBER_OF_PACKETS_TEST];
uint32_t				i2s_recv[NUMBER_OF_PACKETS_TEST];
uint8_t				test_number = 0;

AUDIO_DATA_FORMAT audio_data_test = {
    AUDIO_LITTLE_ENDIAN,    /* Endian of input data */
    AUDIO_ALIGNMENT_LEFT,   /* Alignment of input data */
    16,                      /* Bit size of input data */
    2,                      /* Sample size in bytes */
    2                       /* Number of channels */
};


/*FUNCTION*--------------------------------------------------------------
* 
* Task Name : sine_wave_generator
* Comments  :
* 	
*END*--------------------------------------------------------------*/
#ifndef I2S_USE_FLOAT // Floating point enabled
int16_t *sine_wave_generator(int16_t amptitute, uint16_t samples)
{
	bool 		over180 = 0;
    uint16_t 		i;    
    ieee_single 	step, angle = 0.0;
    int16_t 		*output;

    if (samples == 0)
    {
      return(0);
    }
    output = (int16_t *) _mem_alloc(sizeof(int16_t) * samples);
    if (output == NULL)
    {
       return(0);
    }
    step = 360.0 / (ieee_single) samples;
    for(i = 0; i < samples; i++)
    {
        if (angle > 180)
        {
            angle = angle - 180;
            over180 = TRUE;
        }
        *(output + i) =  amptitute * (4*(uint32_t)angle*(180 - (uint32_t)angle))/(40500 - (uint32_t)angle * (180 - (uint32_t)angle));
        if (over180)
        {
           *(output + i) = -(*(output + i));
        }
        angle += step;
    }
    return(output);
}
#endif

/*FUNCTION*--------------------------------------------------------------
* 
* Task Name : test_i2s_master_init
* Comments  :
* 	
*
*END*--------------------------------------------------------------*/
void test_i2s_master_init(uint8_t data_bits, AUDIO_DATA_FORMAT_PTR audio_ptr)
{
	uint8_t			param;
    _mqx_uint 		result;

	i2s_ptr = fopen(I2S_PORT, NULL);
	switch (test_number)
	{
		case 1:
			EU_ASSERT_REF_TC_MSG(tc_1_main_task, i2s_ptr != NULL, "Test #1.0 Testing I2S Open");
			break;
		
		case 2:
			EU_ASSERT_REF_TC_MSG(tc_2_main_task, i2s_ptr != NULL, "Test #2.0 Testing I2S Open");
			break;
			
		case 3:
			EU_ASSERT_REF_TC_MSG(tc_3_main_task, i2s_ptr != NULL, "Test #3.0 Testing I2S Open");
			break;
		
		case 4:
			EU_ASSERT_REF_TC_MSG(tc_4_main_task, i2s_ptr != NULL, "Test #4.0 Testing I2S Open");
			break;
			
		case 5:
			EU_ASSERT_REF_TC_MSG(tc_5_main_task, i2s_ptr != NULL, "Test #5.0 Testing I2S Open");
			break;
		
		case 6:
			EU_ASSERT_REF_TC_MSG(tc_6_main_task, i2s_ptr != NULL, "Test #6.0 Testing I2S Open");
			break;
		
		case 7:
			EU_ASSERT_REF_TC_MSG(tc_7_main_task, i2s_ptr != NULL, "Test #7.0 Testing I2S Open");
			break;
			
		case 8:
			EU_ASSERT_REF_TC_MSG(tc_8_main_task, i2s_ptr != NULL, "Test #8.0 Testing I2S Open");
			break;
		
		case 9:
			EU_ASSERT_REF_TC_MSG(tc_9_main_task, i2s_ptr != NULL, "Test #9.0 Testing I2S Open");
			break;
			
		case 12:
			EU_ASSERT_REF_TC_MSG(tc_12_main_task, i2s_ptr != NULL, "Test #12.0 Testing I2S Open");
			break;			
	}
	if (i2s_ptr == NULL) 
	{
		IOClient_PTAC_CloseTestingModule();
	    _time_delay_ticks(4);
	    eunit_test_stop();
	    _mqx_exit(1);
	}
	
#ifdef BSPCFG_ENABLE_SAI
	param = (I2S_TX_MASTER | I2S_RX_MASTER);
#else
	param = I2S_MODE_MASTER;
#endif
	
	result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_MODE_MASTER, &param);
	switch (test_number)
	{
		case 1:
			EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1.1 Testing I2S Set master mode");
			break;
		
		case 2:
			EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2.1 Testing I2S Set master mode");
			break;
			
		case 3:
			EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3.1 Testing I2S Set master mode");
			break;
		
		case 4:
			EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4.1 Testing I2S Set master mode");
			break;
			
		case 5:
			EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "Test #5.1 Testing I2S Set master mode");
			break;
		
		case 6:
			EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6.1 Testing I2S Set master mode");
			break;
		
		case 7:
			EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "Test #7.1 Testing I2S Set master mode");
			break;
			
		case 8:
			EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "Test #8.1 Testing I2S Set master mode");
			break;
		
		case 9:
			EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "Test #9.1 Testing I2S Set master mode");
			break;
			
		case 12:
			EU_ASSERT_REF_TC_MSG(tc_12_main_task, result == MQX_OK, "Test #12.1 Testing I2S Set master mode");
			break;			
	}
		
	if (result != MQX_OK) 
	{
		fclose(i2s_ptr);
		IOClient_PTAC_CloseTestingModule();
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
	}

#ifndef BSPCFG_ENABLE_SAI
	result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_CLK_ALWAYS_ENABLED_OFF, NULL);
	switch (test_number)
	{
		case 1:
			EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1.2 Testing I2S Set clock always off");
			break;
		
		case 2:
			EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2.2 Testing I2S Set clock always off");
			break;
			
		case 3:
			EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3.2 Testing I2S Set clock always off");
			break;
		
		case 4:
			EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4.2 Testing I2S Set clock always off");
			break;
			
		case 5:
			EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "Test #5.2 Testing I2S Set clock always off");
			break;
		
		case 6:
			EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6.2 Testing I2S Set clock always off");
			break;
		
		case 7:
			EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "Test #7.2 Testing I2S Set clock always off");
			break;
			
		case 8:
			EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "Test #8.2 Testing I2S Set clock always off");
			break;
		
		case 9:
			EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "Test #9.2 Testing I2S Set clock always off");
			break;
			
		case 12:
			EU_ASSERT_REF_TC_MSG(tc_12_main_task, result == MQX_OK, "Test #12.2 Testing I2S Set clock always off");
			break;			
	}
	if (result != MQX_OK) 
	{
		fclose(i2s_ptr);
		IOClient_PTAC_CloseTestingModule();
		_time_delay_ticks(4);
		eunit_test_stop();
		_mqx_exit(1);
	}
	
#else
	param = I2S_TX_BCLK_NORMAL;
	result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_CLOCK_MODE, &param);
	switch (test_number)
	{
		case 1:
			EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1.2 Testing I2S Set clock mode");
			break;
		
		case 2:
			EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2.2 Testing I2S Set clock mode");
			break;
			
		case 3:
			EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3.2 Testing I2S Set clock mode");
			break;
		
		case 4:
			EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4.2 Testing I2S Set clock mode");
			break;
			
		case 5:
			EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "Test #5.2 Testing I2S Set clock mode");
			break;
		
		case 6:
			EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6.2 Testing I2S Set clock mode");
			break;
		
		case 7:
			EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "Test #7.2 Testing I2S Set clock mode");
			break;
			
		case 8:
			EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "Test #8.2 Testing I2S Set clock mode");
			break;
		
		case 9:
			EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "Test #9.2 Testing I2S Set clock mode");
			break;
			
		case 12:
			EU_ASSERT_REF_TC_MSG(tc_12_main_task, result == MQX_OK, "Test #12.2 Testing I2S Set clock mode");
			break;			
	}	
	if (result != MQX_OK) 
	{
		fclose(i2s_ptr);
		IOClient_PTAC_CloseTestingModule();
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
	}
#endif

	result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_DATA_BITS, &data_bits);
	switch (test_number)
	{
		case 1:
			EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1.3 Testing I2S Set data bits");
			break;
		
		case 2:
			EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2.3 Testing I2S Set data bits");
			break;
			
		case 3:
			EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3.3 Testing I2S Set data bits");
			break;
		
		case 4:
			EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4.3 Testing I2S Set data bits");
			break;
			
		case 5:
			EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "Test #5.3 Testing I2S Set data bits");
			break;
		
		case 6:
			EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6.3 Testing I2S Set data bits");
			break;
		
		case 7:
			EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "Test #7.3 Testing I2S Set data bits");
			break;
			
		case 8:
			EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "Test #8.3 Testing I2S Set data bits");
			break;
		
		case 9:
			EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "Test #9.3 Testing I2S Set data bits");
			break;
			
		case 12:
			EU_ASSERT_REF_TC_MSG(tc_12_main_task, result == MQX_OK, "Test #12.3 Testing I2S Set data bits");
			break;			
	}		
	if (result != MQX_OK) 
	{
		fclose(i2s_ptr);
		IOClient_PTAC_CloseTestingModule();
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
	}	
	
	result = ioctl(i2s_ptr, IO_IOCTL_AUDIO_SET_IO_DATA_FORMAT, audio_ptr);
	switch (test_number)
	{
		case 1:
			EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1.4 Testing I2S Set up data format");
			break;
		
		case 2:
			EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2.4 Testing I2S Set up data format");
			break;
			
		case 3:
			EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3.4 Testing I2S Set up data format");
			break;
		
		case 4:
			EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4.4 Testing I2S Set up data format");
			break;
			
		case 5:
			EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "Test #5.4 Testing I2S Set up data format");
			break;
		
		case 6:
			EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6.4 Testing I2S Set up data format");
			break;
		
		case 7:
			EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "Test #7.4 Testing I2S Set up data format");
			break;
			
		case 8:
			EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "Test #8.4 Testing I2S Set up data format");
			break;
		
		case 9:
			EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "Test #9.4 Testing I2S Set up data format");
			break;
			
		case 12:
			EU_ASSERT_REF_TC_MSG(tc_12_main_task, result == MQX_OK, "Test #12.4 Testing I2S Set up data format");
			break;			
	}				
	if (result != MQX_OK) 
	{
		fclose(i2s_ptr);
		IOClient_PTAC_CloseTestingModule();
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
	}	
	
	result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_MCLK_FREQ, &mclk_freq);
	switch (test_number)
	{
		case 1:
			EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1.5 Testing I2S Set master clock frequency");
			break;
		
		case 2:
			EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2.5 Testing I2S Set master clock frequency");
			break;
			
		case 3:
			EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3.5 Testing I2S Set master clock frequency");
			break;
		
		case 4:
			EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4.5 Testing I2S Set master clock frequency");
			break;
			
		case 5:
			EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "Test #5.5 Testing I2S Set master clock frequency");
			break;
		
		case 6:
			EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6.5 Testing I2S Set master clock frequency");
			break;
		
		case 7:
			EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "Test #7.5 Testing I2S Set master clock frequency");
			break;
			
		case 8:
			EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "Test #8.5 Testing I2S Set master clock frequency");
			break;
		
		case 9:
			EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "Test #9.5 Testing I2S Set master clock frequency");
			break;
			
		case 12:
			EU_ASSERT_REF_TC_MSG(tc_12_main_task, result == MQX_OK, "Test #12.5 Testing I2S Set master clock frequency");
			break;			
	}				
	if (result != MQX_OK) 
	{
		fclose(i2s_ptr);
		IOClient_PTAC_CloseTestingModule();
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
	}
	
	result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_FS_FREQ, &fs_freq);
	switch (test_number)
	{
		case 1:
			EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1.6 Testing I2S Set FS frequency");
			break;
		
		case 2:
			EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2.6 Testing I2S Set FS frequency");
			break;
			
		case 3:
			EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3.6 Testing I2S Set FS frequency");
			break;
		
		case 4:
			EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4.6 Testing I2S Set FS frequency");
			break;
			
		case 5:
			EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "Test #5.6 Testing I2S Set FS frequency");
			break;
		
		case 6:
			EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6.6 Testing I2S Set FS frequency");
			break;
		
		case 7:
			EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "Test #7.6 Testing I2S Set FS frequency");
			break;
			
		case 8:
			EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "Test #8.6 Testing I2S Set FS frequency");
			break;
		
		case 9:
			EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "Test #9.6 Testing I2S Set FS frequency");
			break;
			
		case 12:
			EU_ASSERT_REF_TC_MSG(tc_12_main_task, result == MQX_OK, "Test #12.6 Testing I2S Set FS frequency");
			break;			
	}		
	if (result != MQX_OK) 
	{
		fclose(i2s_ptr);
		IOClient_PTAC_CloseTestingModule();
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
	}
	
	/* Set data format for PTAC */
	result = IOClient_I2S_SetupData(	audio_ptr->ENDIAN,
										audio_ptr->ALIGNMENT,
										audio_ptr->BITS,
										audio_ptr->SIZE,
										audio_ptr->CHANNELS );	
	if (result != MQX_OK) 
	{
		fclose(i2s_ptr);
		IOClient_PTAC_CloseTestingModule();
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
	}
}

/*FUNCTION*--------------------------------------------------------------
* 
* Task Name : test_i2s_slave_init
* Comments  :
* 	
*
*END*--------------------------------------------------------------*/
void test_i2s_slave_init(uint8_t data_bits, AUDIO_DATA_FORMAT_PTR audio_ptr)
{
	uint8_t			param;
    _mqx_uint 		result;

	i2s_ptr = fopen(I2S_PORT, NULL);
	switch (test_number)
	{
		case 10:
			EU_ASSERT_REF_TC_MSG(tc_10_main_task, i2s_ptr != NULL, "Test #10.0 Testing I2S Open");
			break;
			
		case 11:
			EU_ASSERT_REF_TC_MSG(tc_11_main_task, i2s_ptr != NULL, "Test #11.0 Testing I2S Open");
			break;
	}
	if (i2s_ptr == NULL) 
	{
		IOClient_PTAC_CloseTestingModule();
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
	}
    
#ifdef BSPCFG_ENABLE_SAI
    param = (I2S_TX_SLAVE | I2S_RX_SLAVE);
#else
    param = I2S_MODE_SLAVE;
#endif

	result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_MODE_SLAVE, &param);
	switch (test_number)
	{
		case 10:
			EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == MQX_OK, "Test #10.1 Testing I2S Set slave mode");
			break;
			
		case 11:
			EU_ASSERT_REF_TC_MSG(tc_11_main_task, result == MQX_OK, "Test #11.1 Testing I2S Set slave mode");
			break;
	}	
	if (result != MQX_OK) 
	{
		fclose(i2s_ptr);
		IOClient_PTAC_CloseTestingModule();
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
	}		
	
	param = data_bits;
	result = ioctl(i2s_ptr, IO_IOCTL_I2S_SET_DATA_BITS, &param);
	switch (test_number)
	{
		case 10:
			EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == MQX_OK, "Test #10.2 Testing I2S Set data bits");
			break;
			
		case 11:
			EU_ASSERT_REF_TC_MSG(tc_11_main_task, result == MQX_OK, "Test #11.2 Testing I2S Set data bits");
			break;
	}
	if (result != MQX_OK) 
	{
		fclose(i2s_ptr);
		IOClient_PTAC_CloseTestingModule();
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
	}
	
	result = ioctl(i2s_ptr, IO_IOCTL_AUDIO_SET_IO_DATA_FORMAT, audio_ptr);
	switch (test_number)
	{
		case 10:
			EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == MQX_OK, "Test #10.3 Testing I2S Set io data format");
			break;
			
		case 11:
			EU_ASSERT_REF_TC_MSG(tc_11_main_task, result == MQX_OK, "Test #11.3 Testing I2S Set io data format");
			break;
	}	
	if (result != MQX_OK) 
	{
		fclose(i2s_ptr);
		IOClient_PTAC_CloseTestingModule();
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
	}
	
	/* Set receiver ready to receive data */
	result = IOClient_I2S_SetupData(	audio_ptr->ENDIAN,
										audio_ptr->ALIGNMENT,
										audio_ptr->BITS,
										audio_ptr->SIZE,
										audio_ptr->CHANNELS );	
	if (result != MQX_OK) 
	{
		fclose(i2s_ptr);
		IOClient_PTAC_CloseTestingModule();
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
	}
}

/*TASK*--------------------------------------------------------------
* 
* Task Name : write_task
* Comments  :
* 	write repeatedly the number of bytes
*
*END*--------------------------------------------------------------*/

void write_task(uint32_t num)
{
	while (1)
	{
		write(i2s_ptr, i2s_wave, NUMBER_OF_BYTES_TEST);
	}
}

/*TASK*--------------------------------------------------------------
* 
* Task Name : tc_1_main_task
* Comments  : Test with data bits 8
*
*END*--------------------------------------------------------------*/
void tc_1_main_task()
{
	uint8_t			data_bits;
    _mqx_uint 		result;
    uint32_t	 		i;  

    test_number++;
    
	for (i = 0; i < NUMBER_OF_PACKETS_TEST; i++) 
	{
		i2s_data[i] = 0;
		i2s_data[i] |= ((i * 4 + 4) << 24);
		i2s_data[i] |= ((i * 4 + 3) << 16);
		i2s_data[i] |= ((i * 4 + 2) << 8);
		i2s_data[i] |= (i * 4 + 1);
	}
	
	/*							
	 * 	Test data format: 8 bits, 1 channel
	 */ 
	audio_data_test.BITS = 8;
	audio_data_test.SIZE = 1;
	audio_data_test.CHANNELS = 1;
	data_bits = 8;
	
	test_i2s_master_init(data_bits, &audio_data_test);

	/* Set receiver ready to receive data */
	IOClient_I2S_SetReceiverStart(	IOClient_TestPoint_I2S0,
									I2S_MODE_SLAVE,
									I2S_BAUDRATE_TEST,
									data_bits,
									NUMBER_OF_BYTES_TEST);
	
	/* write data */
	write(i2s_ptr, i2s_data, NUMBER_OF_BYTES_TEST);	
	
	/* Send the compared message to receiver for testing */
	result = IOClient_I2S_TestMessageFrame(NUMBER_OF_BYTES_TEST, (uint8_t *)i2s_data);
		
	EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == 0, "Test #1.7 Testing I2S writes in master mode");
	
	/* Change the value of an element of compared message */
	i2s_data[0]++;	
	
	/* Send the compared message again to receiver for testing */
	result = IOClient_I2S_TestMessageFrame(NUMBER_OF_BYTES_TEST, (uint8_t *)i2s_data);
	
	i2s_data[0]--;	

	/* The result should indicates data is not invalid */
	EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != 0, "Test #1.8 Testing I2S writes in master mode");

	/* Disable device to disable interrupt */ 
	I2S_DISABLE;

	result = fclose(i2s_ptr);
	EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1.9 Testing I2S Close");		
}

/*TASK*--------------------------------------------------------------
* 
* Task Name : tc_2_main_task
* Comments  : Test with data bits 10
*
*END*--------------------------------------------------------------*/
void tc_2_main_task()
{
	uint8_t			data_bits;
    _mqx_uint 		result;
    uint32_t	 		i;  

    test_number++;
	  
	/*							
	 * 	Test data format: 10 bits, 2 channel
	 */ 
	audio_data_test.BITS = 10;
	audio_data_test.SIZE = 2;
	audio_data_test.CHANNELS = 2;
	data_bits = 10;

	test_i2s_master_init(data_bits, &audio_data_test);
	
	/* Set receiver ready to receive data */
	IOClient_I2S_SetReceiverStart(	IOClient_TestPoint_I2S0,
									I2S_MODE_SLAVE,
									I2S_BAUDRATE_TEST,
									data_bits,
									NUMBER_OF_BYTES_TEST);
	
	/* write data */
	write(i2s_ptr, i2s_data, NUMBER_OF_BYTES_TEST);	

	/* Send the compared message to receiver for testing */
	result = IOClient_I2S_TestMessageFrame(NUMBER_OF_BYTES_TEST, (uint8_t *)i2s_data);
		
	EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == 0, "Test #2.7 Testing I2S writes in master mode");

	/* Disable device to disable interrupt */ 
	I2S_DISABLE;

	result = fclose(i2s_ptr);
	EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2.8 Testing I2S Close");	
	
}	

/*TASK*--------------------------------------------------------------
* 
* Task Name : tc_3_main_task
* Comments  : Test with data bits 12
*
*END*--------------------------------------------------------------*/
void tc_3_main_task()
{
	uint8_t			data_bits;
    _mqx_uint 		result;
    uint32_t	 		i;  

    test_number++;
	/*							
	 * 	Test data format: 12 bits, 2 channels
	 */ 	
	audio_data_test.BITS = 12;
	audio_data_test.SIZE = 2;
	audio_data_test.CHANNELS = 2;
	data_bits = 12;

	test_i2s_master_init(data_bits, &audio_data_test);
	
	/* Set receiver ready to receive data */
	IOClient_I2S_SetReceiverStart(	IOClient_TestPoint_I2S0,
									I2S_MODE_SLAVE,
									I2S_BAUDRATE_TEST,
									data_bits,
									NUMBER_OF_BYTES_TEST);
	
	/* write data */
	write(i2s_ptr, i2s_data, NUMBER_OF_BYTES_TEST);	

	/* Send the compared message to receiver for testing */
	result = IOClient_I2S_TestMessageFrame(NUMBER_OF_BYTES_TEST, (uint8_t *)i2s_data);
	
	EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == 0, "Test #3.7 Testing I2S writes in master mode");
	/* Disable device to disable interrupt */ 
	I2S_DISABLE;

	result = fclose(i2s_ptr);
	EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3.8 Testing I2S Close");	
}

/*TASK*--------------------------------------------------------------
* 
* Task Name : tc_4_main_task
* Comments  : Test with data bits 18
*
*END*--------------------------------------------------------------*/
void tc_4_main_task()
{
	uint8_t			data_bits;
    _mqx_uint 		result;
    uint32_t	 		i;  

    test_number++;	
	/*							
	 * 	Test data format: 18 bits, 2 channels
	 */ 	
	audio_data_test.BITS = 18;
	data_bits = 18;
	audio_data_test.SIZE = 3;
	audio_data_test.CHANNELS = 2;
	

	test_i2s_master_init(data_bits, &audio_data_test);
	
	/* Set receiver ready to receive data */
	IOClient_I2S_SetReceiverStart(	IOClient_TestPoint_I2S0,
									I2S_MODE_SLAVE,
									I2S_BAUDRATE_TEST,
									data_bits,
									NUMBER_OF_BYTES_TEST);
	
	/* write data */
	write(i2s_ptr, i2s_data, NUMBER_OF_BYTES_TEST);	
	
	/* Send the compared message to receiver for testing */
	result = IOClient_I2S_TestMessageFrame(NUMBER_OF_BYTES_TEST, (uint8_t *)i2s_data);
		
	EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == 0, "Test #4.7 Testing I2S writes in master mode");

	/* Disable device to disable interrupt */ 
	I2S_DISABLE;

	result = fclose(i2s_ptr);
	EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4.8 Testing I2S Close");	
}

/*TASK*--------------------------------------------------------------
* 
* Task Name : tc_5_main_task
* Comments  : Test with data bits 20
*
*END*--------------------------------------------------------------*/
void tc_5_main_task()
{
	uint8_t			data_bits;
    _mqx_uint 		result;
    uint32_t	 		i;  

    test_number++;	
	/*							
	 * 	Test data format: 20 bits, 2 channels
	 */ 
	audio_data_test.BITS = 20;
	audio_data_test.SIZE = 3;
	audio_data_test.CHANNELS = 2;
	data_bits = 20;

	test_i2s_master_init(I2S_DATA_BITS_TEST, &audio_data_test);
	
	/* Set receiver ready to receive data */
	IOClient_I2S_SetReceiverStart(	IOClient_TestPoint_I2S0,
									I2S_MODE_SLAVE,
									I2S_BAUDRATE_TEST,
									data_bits,
									NUMBER_OF_BYTES_TEST);
	
	/* write data */
	write(i2s_ptr, i2s_data, NUMBER_OF_BYTES_TEST);	
	
	/* Send the compared message to receiver for testing */
	result = IOClient_I2S_TestMessageFrame(NUMBER_OF_BYTES_TEST, (uint8_t *)i2s_data);
		
	EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == 0, "Test #5.7 Testing I2S writes in master mode");
	
	/* Disable device to disable interrupt */ 
	I2S_DISABLE;

	result = fclose(i2s_ptr);
	EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "Test #5.8 Testing I2S Close");	
}

/*TASK*--------------------------------------------------------------
* 
* Task Name : tc_6_main_task
* Comments  : Test with data bits 22
*
*END*--------------------------------------------------------------*/
void tc_6_main_task()
{
	uint8_t			data_bits;
    _mqx_uint 		result;
    uint32_t	 		i;  

    test_number++;	
	/*							
	 * 	Test data format: 22 bits, 2 channels
	 */ 
	audio_data_test.BITS = 22;
	audio_data_test.SIZE = 3;
	audio_data_test.CHANNELS = 2;
	data_bits = 22;

	test_i2s_master_init(I2S_DATA_BITS_TEST, &audio_data_test);
	
	/* Set receiver ready to receive data */
	IOClient_I2S_SetReceiverStart(	IOClient_TestPoint_I2S0,
									I2S_MODE_SLAVE,
									I2S_BAUDRATE_TEST,
									data_bits,
									NUMBER_OF_BYTES_TEST);
	
	/* write data */
	write(i2s_ptr, i2s_data, NUMBER_OF_BYTES_TEST);	
		
	/* Send the compared message to receiver for testing */
	result = IOClient_I2S_TestMessageFrame(NUMBER_OF_BYTES_TEST, (uint8_t *)i2s_data);
		
	EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == 0, "Test #6.7 Testing I2S writes in master mode");
	
	/* Disable device to disable interrupt */ 
	I2S_DISABLE;

	result = fclose(i2s_ptr);
	EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6.8 Testing I2S Close");	
}
/*TASK*--------------------------------------------------------------
* 
* Task Name : tc_7_main_task
* Comments  : Test with data bits 24
*
*END*--------------------------------------------------------------*/
void tc_7_main_task()
{
	uint8_t			data_bits;
    _mqx_uint 		result;
    uint32_t	 		i;  

    test_number++;	
	/*							
	 * 	Test data format: 24 bits, 2 channels
	 */ 
	audio_data_test.BITS = 24;
	audio_data_test.SIZE = 3;
	audio_data_test.CHANNELS = 2;
	data_bits = 24;

	test_i2s_master_init(I2S_DATA_BITS_TEST, &audio_data_test);
	
	/* Set receiver ready to receive data */
	IOClient_I2S_SetReceiverStart(	IOClient_TestPoint_I2S0,
									I2S_MODE_SLAVE,
									I2S_BAUDRATE_TEST,
									data_bits,
									NUMBER_OF_BYTES_TEST);
	
	/* write data */
	write(i2s_ptr, i2s_data, NUMBER_OF_BYTES_TEST);		
	
	/* Send the compared message to receiver for testing */
	result = IOClient_I2S_TestMessageFrame(NUMBER_OF_BYTES_TEST, (uint8_t *)i2s_data);
		
	EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == 0, "Test #7.7 Testing I2S writes in master mode");

	/* Disable device to disable interrupt */ 
	I2S_DISABLE;

	result = fclose(i2s_ptr);
	EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "Test #7.8 Testing I2S Close");	

}
/*TASK*--------------------------------------------------------------
* 
* Task Name : tc_8_main_task
* Comments  : Test with data bits 16
*
*END*--------------------------------------------------------------*/
void tc_8_main_task()
{
	uint8_t			data_bits;
    _mqx_uint 		result;
    uint32_t	 		i;  

    test_number++;	
	/*							
	 * 	Test data format: 16 bits, 2 channels
	 */ 
	audio_data_test.BITS = 16;
	audio_data_test.SIZE = 2;
	audio_data_test.CHANNELS = 2;
	data_bits = 16;

	test_i2s_master_init(I2S_DATA_BITS_TEST, &audio_data_test);
	
	/* Set receiver ready to receive data */
	IOClient_I2S_SetReceiverStart(	IOClient_TestPoint_I2S0,
									I2S_MODE_SLAVE,
									I2S_BAUDRATE_TEST,
									data_bits,
									NUMBER_OF_BYTES_TEST);
	
	/* write data */
	write(i2s_ptr, i2s_data, NUMBER_OF_BYTES_TEST);	
	
	/* Send the compared message to receiver for testing */
	result = IOClient_I2S_TestMessageFrame(NUMBER_OF_BYTES_TEST, (uint8_t *)i2s_data);
		
	EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == 0, "Test #8.7 Testing I2S writes in master mode");
	
	/* Disable device to disable interrupt */ 
	I2S_DISABLE;

	result = fclose(i2s_ptr);
	EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "Test #8.8 Testing I2S Close");	
}

/*TASK*--------------------------------------------------------------
* 
* Task Name : tc_9_main_task
* Comments  : Test master read
*
*END*--------------------------------------------------------------*/
void tc_9_main_task()
{
    uint32_t	 		i;
    _mqx_uint 		result;  
    
    test_number++;
	
	test_i2s_master_init(I2S_DATA_BITS_TEST, &audio_data_test);
	
	/* Request PTAC write requested data in slave mode */
	IOClient_I2S_SendGroupTransmitter(	IOClient_TestPoint_I2S0,
										I2S_MODE_SLAVE,
										I2S_BAUDRATE_TEST,
										I2S_DATA_BITS_TEST,
										1,
										NUMBER_OF_BYTES_TEST,
										(uint8_t *)i2s_data);

	/* Read data on the bus */
	read(i2s_ptr, i2s_recv, NUMBER_OF_BYTES_TEST);

	/* Check data received */
	for (i = 0; i < NUMBER_OF_PACKETS_TEST; i++)
	{
		if (i2s_data[i] != i2s_recv[i]) break;
	}
	
	EU_ASSERT_REF_TC_MSG(tc_9_main_task, i == NUMBER_OF_PACKETS_TEST, "Test #9.7 Testing I2S read in master mode");
	
	/* Disable device to disable interrupt */ 
	I2S_DISABLE;
	
	result = fclose(i2s_ptr);	
	EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "Test #9.8 Testing I2S Close");

}

/*TASK*--------------------------------------------------------------
* 
* Task Name : tc_10_main_task
* Comments  : Test slave write
*
*END*--------------------------------------------------------------*/
void tc_10_main_task()
{
    _mqx_uint 		result;
    uint32_t	 		i;
     	
	test_number++;
	
	test_i2s_slave_init(I2S_DATA_BITS_TEST, &audio_data_test);
	
	IOClient_I2S_SetReceiverStart(	IOClient_TestPoint_I2S0,
									I2S_MODE_MASTER,
									I2S_BAUDRATE_TEST,
									I2S_DATA_BITS_TEST,
									NUMBER_OF_BYTES_TEST);
	
	write(i2s_ptr, i2s_data, NUMBER_OF_BYTES_TEST);		
	
	/* Send the compared message to receiver for testing */
	result = IOClient_I2S_TestMessageFrame(NUMBER_OF_BYTES_TEST, (uint8_t *)i2s_data);
	
	EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == 0, "Test #10.4 Testing I2S writes in slave mode");
	
	/* Change the value of an element of compared message */
	i2s_data[0]++;	
	
	/* Send the compared message again to receiver for testing */
	result = IOClient_I2S_TestMessageFrame(NUMBER_OF_BYTES_TEST, (uint8_t *)i2s_data);
	
	i2s_data[0]--;	
	
	/* The result should indicates data is not invalid */
	EU_ASSERT_REF_TC_MSG(tc_10_main_task, result != 0, "Test #10.5 Testing I2S writes in slave mode");
	
	/* Disable device to disable interrupt */ 
	I2S_DISABLE;
	
	result = fclose(i2s_ptr);
	EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == MQX_OK, "Test #10.6 Testing I2S Close");
}

/*TASK*--------------------------------------------------------------
* 
* Task Name : tc_11_main_task
* Comments  : Test slave read
*
*END*--------------------------------------------------------------*/
void tc_11_main_task()
{
    uint32_t	 		i;
    _mqx_uint 		result;
   
    test_number++;
			    
	test_i2s_slave_init(I2S_DATA_BITS_TEST, &audio_data_test);
	
	/* Request PTAC write requested data in master mode */
	IOClient_I2S_SendGroupTransmitter(	IOClient_TestPoint_I2S0,
										I2S_MODE_MASTER,
										I2S_BAUDRATE_TEST,
										I2S_DATA_BITS_TEST,
										1,
										NUMBER_OF_BYTES_TEST,
										(uint8_t *)i2s_data);

	/* Read data on the bus */
	read(i2s_ptr, i2s_recv, NUMBER_OF_BYTES_TEST);
	
	/* Compare received data */
	for (i = 0; i < NUMBER_OF_PACKETS_TEST; i++)
	{
		if (i2s_data[i] != i2s_recv[i]) break;
	}
	EU_ASSERT_REF_TC_MSG(tc_11_main_task, i == NUMBER_OF_PACKETS_TEST, "Test #11.4 Testing I2S read in slave mode");

	/* Disable device to disable interrupt */ 
	I2S_DISABLE;
	
	result = fclose(i2s_ptr);	
	EU_ASSERT_REF_TC_MSG(tc_11_main_task, result == MQX_OK, "Test #11.5 Testing I2S Close");
}

/*TASK*--------------------------------------------------------------
* 
* Task Name : tc_12_main_task
* Comments  : Test wave generator
*
*END*--------------------------------------------------------------*/
void tc_12_main_task()
{
    _mqx_uint 		result;
    uint32_t	 		i;  
    _task_id		id;
	
	test_number++;

#define DURATION_TIME	20 	/* generating sine wave for 20 seconds */
   	
#ifndef I2S_USE_FLOAT // Floating point enabled
	i2s_wave = sine_wave_generator(BIT16_MAX, NUMBER_OF_PACKETS_TEST);
#else
	i2s_wave = (int16_t *)i2s_data;
#endif
	
	/*							
	 * 	Test data format: 16 bits, 2 channels
	 */ 
	audio_data_test.BITS = 16;
	audio_data_test.SIZE = 2;
	audio_data_test.CHANNELS = 2;
	
	test_i2s_master_init(16, &audio_data_test);
		
	/* Set receiver ready to receive data */
	IOClient_I2S_TestPeriodicMessageSetup(	IOClient_TestPoint_I2S0,
											I2S_MODE_SLAVE,
											I2S_BAUDRATE_TEST,
											I2S_DATA_BITS_TEST,
											DURATION_TIME,						
											NUMBER_OF_BYTES_TEST, 
											(uint8_t *)i2s_wave);
	
	id = _task_create(0, WRITE_TASK, 0);
	EU_ASSERT_REF_TC_MSG(tc_12_main_task, id != MQX_NULL_TASK_ID, "Test #12.7 Create the write task should be success ");
	if (id == MQX_NULL_TASK_ID) 
	{
		fclose(i2s_ptr);
		IOClient_PTAC_CloseTestingModule();
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
	}
	
	/* generating sine wave for seconds */
	_time_delay(DURATION_TIME * 1000);
	
	/* Disable device to disable interrupt */ 
	I2S_DISABLE;
	
	result = fclose(i2s_ptr);
	EU_ASSERT_REF_TC_MSG(tc_12_main_task, result == MQX_OK, "Test #12.8 Testing I2S Close");
	
	_task_destroy(id);
	
	result = IOClient_I2S_TestPeriodicMessageResult();	
	EU_ASSERT_REF_TC_MSG(tc_12_main_task, result == 0, "Test #12.9 Testing I2S writes in master mode");
getchar();
}

/******************** End Testcases ********************/

//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_2)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST 1 - Test with data bits 8 "),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST 2 - Test with data bits 10 "),
	EU_TEST_CASE_ADD(tc_3_main_task, " TEST 3 - Test with data bits 12 "),
    EU_TEST_CASE_ADD(tc_4_main_task, " TEST 4 - Test with data bits 18 "),
	EU_TEST_CASE_ADD(tc_5_main_task, " TEST 5 - Test with data bits 20 "),
    EU_TEST_CASE_ADD(tc_6_main_task, " TEST 6 - Test with data bits 22 "),
    EU_TEST_CASE_ADD(tc_7_main_task, " TEST 7 - Test with data bits 24 "),
    EU_TEST_CASE_ADD(tc_8_main_task, " TEST 8 - Test with data bits 16 "),
	EU_TEST_CASE_ADD(tc_9_main_task, " TEST 9 - Test master read "),
    EU_TEST_CASE_ADD(tc_10_main_task, " TEST10 - Test slave write "),
    EU_TEST_CASE_ADD(tc_11_main_task, " TEST 11 - Test slave read "),
    EU_TEST_CASE_ADD(tc_12_main_task, " TEST 12 - Test wave generator "),
EU_TEST_SUITE_END(suite_2)

// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_2, " - I2S peripheral test")
EU_TEST_REGISTRY_END()
//------------------------------------------------------------------------

/*TASK*--------------------------------------------------------------
* 
* Task Name : main_task
* Comments  :
*
*END*--------------------------------------------------------------*/
void main_task(uint32_t dummy)
{	
	IOClient_PTAC_OpenTestingModule(IOClient_TestPoint_I2S0);
	
	eunit_mutex_init();
	EU_RUN_ALL_TESTS(eunit_tres_array);
	
	IOClient_PTAC_CloseTestingModule();
	
	eunit_test_stop();
}

/* EOF */
