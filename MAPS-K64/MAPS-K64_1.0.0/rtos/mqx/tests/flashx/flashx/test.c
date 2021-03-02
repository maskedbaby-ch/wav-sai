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
* $Version : 3.8.7.0$
* $Date    : Jun-22-2012$
*
* Comments:
*
*   This file runs the flash memory test.  
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <fio.h>
#include <string.h>
#include <flashx.h>
#include <util.h>

#include "EUnit.h"
#include "eunit_crit_secs.h"
#include "test.h"
#include "flashx_settings.h"



#define FILE_BASENAME test


extern const FLASHX_INIT_STRUCT _test_flashx_init;
extern const uint32_t * cfm;


/*
** constants
*/

#define MAIN_TASK    1
#define ATTR_MASK    (IO_DEV_ATTR_ERASE | IO_DEV_ATTR_POLL | IO_DEV_ATTR_READ | IO_DEV_ATTR_SEEK | IO_DEV_ATTR_WRITE)
#define RESULT_OUT_OF_MEMORY -2


/*
** required settings
*/
#if !BSPCFG_ENABLE_FLASHX
    #error "BSPCFG_ENABLE_FLASHX have to be enabled"
#endif


#if !(PSP_MQX_CPU_IS_KINETIS_K70 || PSP_MQX_CPU_IS_KINETIS_K60 || \
      PSP_MQX_CPU_IS_MCF51CN || PSP_MQX_CPU_IS_MCF5225X || \
      PSP_MQX_CPU_IS_MPX)
    #error "Unsuported platform"
#endif


/* util functions */
void buffer_create_pattern(unsigned char *, _mqx_int, _mqx_int); 
_mqx_int buffer_compare_pattern(unsigned char *, _mqx_int, _mqx_int); 
_mqx_int buffer_optimal_size(_mqx_int, _mqx_int); 
_mqx_int buffer_compare_buffer(unsigned char *first, unsigned char *second, _mqx_int length); 
_mqx_int buffer_compare_sign(unsigned char *first, _mqx_int length, unsigned char sign); 
_mqx_int find_sector_start(MQX_FILE_PTR flash_file, _mqx_int location); 
_mqx_int check_valid_place(MQX_FILE_PTR flash_file, _mqx_int location, _mqx_int size); 
_mqx_int place_is_erased(MQX_FILE_PTR flash_file, _mqx_int location, _mqx_int size); 
_mqx_int place_is_clean(MQX_FILE_PTR flash_file, _mqx_int location, _mqx_int size); 


/* test cases */
void tc_1_main_task(void);
void tc_2_main_task(void);
void tc_3_main_task(void);
_mqx_int tc_4_main_task_cached_buffered(MQX_FILE_PTR flash_file);
_mqx_int tc_4_main_task_cache_unbuffered(MQX_FILE_PTR flash_file);
_mqx_int tc_4_main_task_uncached(MQX_FILE_PTR flash_file);
_mqx_int tc_4_main_task_write_pass(MQX_FILE_PTR, _mqx_int, unsigned char *, _mqx_int, _mqx_int);
_mqx_int tc_4_main_task_write_fail(MQX_FILE_PTR flash_file,_mqx_int location, unsigned char *write_buffer, _mqx_int buffer_size);
_mqx_int tc_4_main_task_erase_sector(MQX_FILE_PTR flash_file, _mqx_int location, _mqx_int size);
void tc_4_main_task(void);
_mqx_int tc_5_main_task_cached_files(MQX_FILE_PTR, MQX_FILE_PTR, MQX_FILE_PTR);
_mqx_int tc_5_main_task_write_and_check_buffer(MQX_FILE_PTR, MQX_FILE_PTR, unsigned char *, _mqx_int, _mqx_int, _mqx_int);
void tc_5_main_task(void);
void tc_6_main_task(void);
void tc_7_main_task_valid_write(void);
void tc_7_main_task_invalid_write(void);
void tc_7_main_task(void);
void main_task(uint32_t dummy);


TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
   { MAIN_TASK,      main_task,      4000L,  8L, "Main",      MQX_AUTO_START_TASK},
   { 0 }
};



/*FUNCTION*****************************************************************
* 
* Function Name    : buffer_create_pattern
* Returned Value   : void
* Comments         :
*   Fill up buffer with pattern. 
*   type - number for different pattern
*   algorithm MUST be the same as use buffer_compare_pattern
*END*********************************************************************/
void buffer_create_pattern(unsigned char *buffer, _mqx_int length, _mqx_int type)
{
    _mqx_int i = 0;
    for (i = 0; i < length; i++) {
      /* create pattern: 0x0e, 0x1e, ... , 0xfe
       * to be human readable and kinetis (backdoor) safe 
       * if pattern is written to incorrect address */
       buffer[i] = ((1 << (type % 5))  | 0x2);
    }
}


/*FUNCTION*****************************************************************
* 
* Function Name    : buffer_compare_pattern
* Returned Value   : _mqx_int
* Comments         :
*   Compare buffer with generated pattern. 
*   type - number for different pattern
*   algorithm MUST be the same as use bufer_create_pattern
*END*********************************************************************/
_mqx_int buffer_compare_pattern(unsigned char *buffer, _mqx_int length, _mqx_int type)
{
    _mqx_int i = 0, status = 1;
    for (i = 0; i < length; i++) {
      /* create pattern: 0x0e, 0x1e, ... , 0xfe
       * to be human readable and kinetis (backdoor) safe 
       * if pattern is written to incorrect address */
       if (buffer[i] != ((1 << (type % 5))  | 0x2))
       {
            status = 0;
            break;
       }
    }
    return status;
}

/*FUNCTION*****************************************************************
* 
* Function Name    : buffer_optimal_size
* Returned Value   : _mqx_int
* Comments         :
*   Calculate optimal buffer size, if there is no enough memory 
*END*********************************************************************/
_mqx_int buffer_optimal_size(_mqx_int required_buffer_size, _mqx_int number_of_buffers)
{
    _mqx_int free_mem = (_mqx_int)get_free_mem();
    if (free_mem < (required_buffer_size + 64) * number_of_buffers) { 
        required_buffer_size = (free_mem - (64 * number_of_buffers)) / number_of_buffers;
    }
    return required_buffer_size;
}

/*FUNCTION*****************************************************************
* 
* Function Name    : buffer_compare_buffer
* Returned Value   : TRUE if buffers contain same values, otherwise FALSE
* Comments         :
*END*********************************************************************/
_mqx_int buffer_compare_buffer(unsigned char *first, unsigned char *second, _mqx_int length)
{
    _mqx_int i = 0;
    for (i = 0; (i < length) && (first[i] == second[i]); i++) {
        ;
    }
    return (i == length ? TRUE : FALSE);
}


/*FUNCTION*****************************************************************
* 
* Function Name    : buffer_compare_sign
* Returned Value   : TRUE if buffers is full filled with sign, otherwise FALSE
* Comments         : 
*END*********************************************************************/
_mqx_int buffer_compare_sign(unsigned char *buffer, _mqx_int length, unsigned char sign)
{
    _mqx_int i = 0;
    for (i = 0; (i < length) && (buffer[i] == sign); i++) {
      ;
    }
    return (i == length ? TRUE : FALSE);
}


/*FUNCTION*****************************************************************
* 
* Function Name    : find_sector_start
* Returned Value   : start location of sector if succeed, otherwise -1
* Comments         : 
*   Argument location point to some position located inside sector. 
*   Function will calculate the sector start address.
*END*********************************************************************/
_mqx_int find_sector_start(MQX_FILE_PTR flash_file, _mqx_int location)
{
    FLASHX_BLOCK_INFO_STRUCT_PTR    info_ptr = NULL;
    _mqx_int i = 0;
    _mqx_int sector_start = -1;
    _mqx_int block_start = 0;

    /* obtain block map fail */
    if ((MQX_OK != ioctl(flash_file, FLASH_IOCTL_GET_BLOCK_MAP, (uint32_t *)&info_ptr)) 
        && (info_ptr != NULL)
    ) { 
        return -1;
    }

    /* lopp over blocks and find start location of current sector */
    block_start = 0;
    for (i = 0; info_ptr[i].SECTOR_SIZE; i++) {
        /* location is inside block */
        if (location < (info_ptr[i].SECTOR_SIZE * info_ptr[i].NUM_SECTORS)) { 
            /* calculate start of sector */
            sector_start = (_mqx_int)(((location - block_start) / info_ptr[i].SECTOR_SIZE) * info_ptr[i].SECTOR_SIZE);
            break;
        }
        block_start = (_mqx_int)(info_ptr[i].SECTOR_SIZE * info_ptr[i].NUM_SECTORS);
    }
    return sector_start;
}


/*FUNCTION*****************************************************************
* 
* Function Name    : check_valid_place
* Returned Value   : TRUE if place is valid, otherwise FALSE
* Comments         : 
*   Check if data are placed inside valid flash address 
*END*********************************************************************/
_mqx_int check_valid_place(MQX_FILE_PTR flash_file, _mqx_int location, _mqx_int size)
{
    _mqx_int    original_location = 0;
    _mqx_int    last_location = 0;
    
    /* save original location */
    original_location = ftell(flash_file);
    /* get last location */
    fseek(flash_file, 0, IO_SEEK_END);
    last_location = ftell(flash_file);
    /* restore original location */
    fseek(flash_file, original_location, IO_SEEK_SET);
    
    return ((location + size) >= last_location) ? FALSE : TRUE;
}


/*FUNCTION*****************************************************************
* 
* Function Name    : place_is_erased
* Returned Value   : start location of sector if succeed, otherwise -1
* Comments         : 
*   Check if sectors belong to data are erased. 
*   Location point to start address of data, size - data size. 
*END*********************************************************************/
_mqx_int place_is_erased(MQX_FILE_PTR flash_file, _mqx_int location, _mqx_int remain_size) 
{
    unsigned char *buffer = NULL;
    _mqx_int sector_start = -1, sector_size = 0;
    _mqx_int buffer_size = 0, buffer_remain = 0;
    _mqx_int result = 0, status = 1, original_location = 0;
    
    /* save current/original location */
    original_location = ftell(flash_file);
    
    /* find location of first sector */
    sector_start = find_sector_start(flash_file, location);
    status = (sector_start < 0) ? -1 : 1;
    if (status < 0) goto place_is_erased_deinit;
    
    /* Loop over sectors placed under file 
    ** 1) add unused size of first sector to size
    ** 2) loop while status is valid and remain_size > 0
    ** 3) move to next block and decrease remain_size
    */
    for (remain_size = (location - sector_start) + remain_size; 
        ((status > 0) && (remain_size > 0)); 
        sector_start += sector_size, 
        remain_size = remain_size - sector_size
    )
    {
        /* set head to sector start location and get size of sector */
        fseek(flash_file, sector_start, IO_SEEK_SET);
        sector_size = 0;
        result = ioctl(flash_file, FLASH_IOCTL_GET_SECTOR_SIZE, &sector_size);
        status = ((result == MQX_OK) && (sector_size > 0)) ? 1 : -1;
        if (status <= 0) goto place_is_erased_deinit;
        
        /*
        ** calculate buffer size and allocate buffer. Optimal value is sector_size, 
        ** but if there is not enought memory, we have to loop partially 
        */
        buffer_size = (_mqx_int)get_free_mem();
        buffer_size = buffer_size > sector_size ? sector_size : buffer_size;
        buffer_remain = sector_size;
        buffer = _mem_alloc_zero((_mqx_uint)buffer_size);
        status = (buffer == NULL) ? RESULT_OUT_OF_MEMORY : 1;
        if (status <= 0) goto place_is_erased_deinit;
        
        /* partialy loop, at the least one 
        ** read and compare with 0xff
        */
        fseek(flash_file, sector_start, IO_SEEK_SET);
        while(buffer_remain > 0)
        {
            buffer_size = (buffer_remain > buffer_size ? buffer_size : buffer_remain);
            result = read(flash_file, buffer, buffer_size);
            status = (result == buffer_size) ? 1 : -1;
            if (status <= 0) goto place_is_erased_deinit;
            status = buffer_compare_sign(buffer, buffer_size, 0xff);
            if (status <= 0) goto place_is_erased_deinit;
            buffer_remain = buffer_remain - buffer_size;
        }
        
        _mem_free(buffer);
        buffer = NULL;
        if (status < 0) goto place_is_erased_deinit;
    }
    
place_is_erased_deinit:
    if (buffer != NULL) { 
        _mem_free(buffer);
        buffer = NULL;
    } 
    
    /* restore original location */
    fseek(flash_file, original_location, IO_SEEK_SET);
    return status;
}


/*FUNCTION*****************************************************************
* 
* Function Name    : place_is_clean
* Returned Value   : start location of sector if succeed, otherwise -1
* Comments         : 
*   Check whether sector under and above data (XXXX) is erased 
*   ... | XXXXDATA | DATADATA | DATAXXXX |...
*END*********************************************************************/
_mqx_int place_is_clean(MQX_FILE_PTR flash_file, _mqx_int location, _mqx_int size)
{
    _mqx_int    sector_size = 0, sector_position = 0;
    _mqx_int    result = 0, status = 1;
    _mqx_int    buffer_size = 0, buffer_remain = 0;
    unsigned char   *read_buffer = NULL;
    _mqx_int original_location = 0;
    
    /* save current/original location */
    original_location = ftell(flash_file);
    
    /* find and seek to the start position of first sector of data */
    sector_position = find_sector_start(flash_file, location);
    fseek(flash_file, sector_position, IO_SEEK_SET);
    result = ioctl(flash_file, FLASH_IOCTL_GET_SECTOR_SIZE, &sector_size);
    if (result != MQX_OK)
    {
        status = -1;
        goto place_is_clean_deinit;
    }
    
    /* calculate buffer size and allocate buffer */
    buffer_size = (_mqx_int)get_free_mem();
    buffer_size = buffer_size > sector_size ? sector_size : buffer_size;
    buffer_remain = location - sector_position;
    read_buffer = _mem_alloc_zero((_mqx_uint)buffer_size);
    status = (read_buffer == NULL) ? RESULT_OUT_OF_MEMORY : 1;
    if (status <= 0) goto place_is_clean_deinit;
    
    /* read buffer and check if 'empty' */
    fseek(flash_file, sector_position, IO_SEEK_SET);
    while(buffer_remain > 0)
    {
        buffer_size = buffer_remain > buffer_size ? buffer_size : buffer_remain;
        // read buffer 
        result = read(flash_file, read_buffer,buffer_size);
        if (result != buffer_size)
        {
            status = -1;
            goto place_is_clean_deinit;
        }
        // check if buffer is empty 
        status = buffer_compare_sign(read_buffer, buffer_size, 0xff);
        if (status <= 0)
        {
            goto place_is_clean_deinit;
        }
        buffer_remain = buffer_remain - buffer_size;
    }
    // free buffer 
    _mem_free(read_buffer);
    read_buffer = NULL;
    
    
    /* find and seek to the start position of last sector of data */
    sector_position = location + size - 1;
    sector_position = find_sector_start(flash_file, sector_position);
    fseek(flash_file, sector_position, IO_SEEK_SET);
    result = ioctl(flash_file, FLASH_IOCTL_GET_SECTOR_SIZE, &sector_size);
    if (result != MQX_OK)
    {
        status = -1;
        goto place_is_clean_deinit;
    }
    
    /* calculate buffer size and allocate buffer */
    buffer_size = (_mqx_int)get_free_mem();
    buffer_size = buffer_size > sector_size ? sector_size : buffer_size;
    buffer_remain = sector_size - ((location + size) - sector_position);
    read_buffer = _mem_alloc_zero((_mqx_uint)buffer_size);
    status = (read_buffer == NULL) ? RESULT_OUT_OF_MEMORY : 1;
    if (status <= 0) goto place_is_clean_deinit;
    
    /* read buffer and check if 'empty' */
    fseek(flash_file, (location + size), IO_SEEK_SET);
    while(buffer_remain > 0)
    {
        buffer_size = buffer_remain > buffer_size ? buffer_size : buffer_remain;
        // read buffer 
        result = read(flash_file, read_buffer, buffer_size);
        if (result != buffer_size)
        {
            status = -1;
            goto place_is_clean_deinit;
        }
        // check if buffer is empty 
        status = buffer_compare_sign(read_buffer, buffer_size, 0xff);
        if (status <= 0)
        {
            goto place_is_clean_deinit;
        }
        buffer_remain = buffer_remain - buffer_size;
    }
    // free buffer 
    _mem_free(read_buffer);
    read_buffer = NULL;
    
place_is_clean_deinit:
    if (read_buffer != NULL) { 
        _mem_free(read_buffer);
        read_buffer = NULL;
    }
    fseek(flash_file, original_location, IO_SEEK_SET);
    return status;
}


/*TASK*-------------------------------------------------------------------
* 
* Task Name    : tc_1_main_task
* Comments     :
*   Tests device and flash information
*END*----------------------------------------------------------------------*/
void tc_1_main_task(void)
{
#if !ENABLE_CASE1
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, TRUE, "Test 1.0 # Disabled");
#else
    MQX_FILE_PTR                    flash_file = NULL;
    FLASHX_BLOCK_INFO_STRUCT_PTR    info_ptr = NULL;
    _mqx_uint    tmp = 0;
    _mqx_int    result = 0;
    _mqx_uint    ident_arr[3] = {0, 0, 0};
    
    /* init required data */
    flash_file = fopen(FLASHX_WORK_RW, NULL);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (flash_file != NULL), " Test 1.1 # Open file failed");
    if (!flash_file) {
        return;
    }
    
    /* device driver informations */
    result = ioctl(flash_file, IO_IOCTL_DEVICE_IDENTIFY, &ident_arr);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), " Test 1.2 # Can't obtain device information");
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (ident_arr[0] == IO_DEV_TYPE_PHYS_FLASHX), " Test 1.2.1 # Physical type should be IO_DEV_TYPE_PHYS_FLASHX");
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (ident_arr[1] == IO_DEV_TYPE_LOGICAL_MFS), " Test 1.2.2 # Logical type should be IO_DEV_TYPE_LOGICAL_MFS");
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (ident_arr[2] == ATTR_MASK), " Test 1.2.3 # Atributes are not correctly set");
    
    /* mfs informations */
    result = ioctl(flash_file, IO_IOCTL_GET_NUM_SECTORS, &tmp);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK && tmp), " Test 1.3 # Ioctl IO_IOCTL_GET_NUM_SECTORS failed");
    result = ioctl(flash_file, IO_IOCTL_GET_BLOCK_SIZE, &tmp);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK && tmp), " Test 1.4 # Ioctl IO_IOCTL_GET_BLOCK_SIZE failed");
    
    /* flash informations */
    result = ioctl(flash_file, FLASH_IOCTL_GET_NUM_SECTORS, &tmp);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK && tmp), " Test 1.5 # Ioctl FLASH_IOCTL_GET_NUM_SECTORS failed");
    result = ioctl(flash_file, FLASH_IOCTL_GET_WIDTH, &tmp);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK && tmp), " Test 1.6 # Ioctl FLASH_IOCTL_GET_WIDTH failed");
    result = ioctl(flash_file, FLASH_IOCTL_GET_BLOCK_MAP, (uint32_t *)&info_ptr);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK && info_ptr != NULL), " Test 1.7 # Ioctl FLASH_IOCTL_GET_BLOCK_MAP failed");
    
    /* deinit - close file_ptr */
    result = fclose(flash_file);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), " Test 1.8 # close file failed");
#endif
}


/*TASK*-------------------------------------------------------------------
* 
* Task Name    : tc_2_main_task
* Comments     :
*   Test block size, seek operation
*END*----------------------------------------------------------------------*/
void tc_2_main_task(void)
{
#if !ENABLE_CASE2
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, TRUE, "Test 2.0 # Disabled");
#else
    MQX_FILE_PTR                    flash_file = NULL;
    FLASHX_BLOCK_INFO_STRUCT_PTR    info_ptr = NULL;
    _mqx_int    i, j = 0;
    _mqx_int    sector_size = 0;
    _mqx_int    block_begin = 0;
    _mqx_int    result = 0;
    _mqx_int    location = 0;
    _mqx_int    end_location = 0;
    
    /* init required data */
    flash_file = fopen(FLASHX_WHOLE_RO, NULL);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (flash_file != NULL), " Test 2.1 # Open file failed");
    if (!flash_file){
        goto tc_2_main_task_deinit;
    }
    result = ioctl(flash_file, FLASH_IOCTL_GET_BLOCK_MAP, (uint32_t *)&info_ptr);
    result = (result == MQX_OK && info_ptr != NULL && info_ptr[0].SECTOR_SIZE);
    if (!result) {
        goto tc_2_main_task_deinit;
    }
    
    /* get end file position */
    fseek(flash_file, 0, IO_SEEK_END);
    end_location = ftell(flash_file);
    
    /* loop over the blocks, place seek head to calculated (begin, middle, end) sector location 
     * and compare values with equal IOCTL functions 
     */
    for (i = 0; info_ptr[i].SECTOR_SIZE; i++) { 
        /* first sector of block has higher address than eof */
        for (j = 0; (j < info_ptr[i].NUM_SECTORS) && (block_begin < end_location); j++) { 
            /* seek to sector begin */
            location = (_mqx_int)(block_begin + (j * info_ptr[i].SECTOR_SIZE));
            fseek(flash_file, location, IO_SEEK_SET);
            result = ioctl(flash_file, FLASH_IOCTL_GET_SECTOR_SIZE, &sector_size);
            result = (result == MQX_OK && sector_size == info_ptr[i].SECTOR_SIZE);
            EU_ASSERT_REF_TC_MSG(tc_2_main_task, result, " Test 2.2 # Sector size should be the same");
            if (!result) {
                goto tc_2_main_task_deinit;
            }
            
            /* half sector */
            location = (_mqx_int)(block_begin + (j * info_ptr[i].SECTOR_SIZE) + (info_ptr[i].SECTOR_SIZE / 2));
            fseek(flash_file, location, IO_SEEK_SET);
            result = ioctl(flash_file, FLASH_IOCTL_GET_SECTOR_SIZE, &sector_size);
            result = (result == MQX_OK && sector_size == info_ptr[i].SECTOR_SIZE);
            EU_ASSERT_REF_TC_MSG(tc_2_main_task, result, " Test 2.3 # Sector size should be the same");
            if (!result) {
                goto tc_2_main_task_deinit;
            }
            
            /* last valid position of sector */
            location = (_mqx_int)(block_begin + (j * info_ptr[i].SECTOR_SIZE) + (info_ptr[i].SECTOR_SIZE - 1));
            fseek(flash_file, location, IO_SEEK_SET);
            result = ioctl(flash_file, FLASH_IOCTL_GET_SECTOR_SIZE, &sector_size);
            result = (result == MQX_OK && sector_size == info_ptr[i].SECTOR_SIZE);
            EU_ASSERT_REF_TC_MSG(tc_2_main_task, result, " Test 2.4 # Sector size should be the same");
            if (!result) {
                goto tc_2_main_task_deinit;
            }
        }
        block_begin += info_ptr[i].SECTOR_SIZE * info_ptr[i].NUM_SECTORS;
    }
    
/* clean up resources */
tc_2_main_task_deinit:
    /* deinit - close file_ptr */
    if (flash_file) {
        result = fclose(flash_file);
        flash_file = NULL;
    }
#endif
}


/*TASK*-------------------------------------------------------------------
* 
* Task Name    : tc_3_main_task
* Comments     :
*   Test erase full chip, loop over banks and test if are empty
*   Working area doesn't cover full flash size
*END*----------------------------------------------------------------------*/
void tc_3_main_task()
{
#if !ENABLE_CASE3
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, TRUE, "Test 3.0 # Disabled");
#else
    MQX_FILE_PTR    flash_file = NULL;
    _mqx_int        size = 0;
    _mqx_int        result = 0;
    
    /* open file device */
    flash_file = fopen(FLASHX_ERASED_RO, NULL);
    result = (flash_file != NULL);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result, " Test 3.1 # Open file failed");
    if (!result) {
        goto tc_3_main_task_deinit;
    }
    
    /* erase FULL flash */
    result = ioctl(flash_file, FLASH_IOCTL_ERASE_CHIP, NULL);
    result = (result == MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result, " Test 3.2 # Chip erase failed");
    if (!result) {
       goto tc_3_main_task_deinit;
    }
    
    
    /* check if flash is erased */
    fseek(flash_file, 0, IO_SEEK_END);
    size = ftell(flash_file);
    result = place_is_erased(flash_file, 0, size);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result, " Test 3.3 # Chip erase failed");
    if (!result) {
       goto tc_3_main_task_deinit;  
    }
    
/* clean up resources */
tc_3_main_task_deinit: 
    if (flash_file) { 
        fclose(flash_file);
        flash_file = NULL;
    }
    
#if CASE3_CFM_ENABLE
    /* set disable protected bits and secure bits */
    flash_file = fopen(FLASHX_CFM, NULL);
    if (flash_file != NULL) { 
        ioctl(flash_file, FLASH_IOCTL_ENABLE_SECTOR_CACHE, NULL);
        fseek(flash_file, CASE3_CFM_OFFSET, IO_SEEK_SET);
        write(flash_file, (void *)&cfm, CASE3_CFM_SIZE);
        fclose(flash_file);
        flash_file = NULL;
    }
#endif
#endif
}


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : tc_4_main_task_cached_buffered
* Comments     :
*   Set file to be cached and buffered
*END*----------------------------------------------------------------------*/
_mqx_int tc_4_main_task_cached_buffered(MQX_FILE_PTR flash_file)
{
    _mqx_int  result = 0;

    result = ioctl(flash_file, FLASH_IOCTL_ENABLE_SECTOR_CACHE, NULL);
    result = (result == MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result, " Test 4.1.1 # IOCTL enable cache failed"); 
    if (!result) {
        return result; 
    }

    result = ioctl(flash_file, FLASH_IOCTL_ENABLE_BUFFERING, NULL);
    result = (result == MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result, " Test 4.1.2 # IOCTL enable buffer failed"); 
    if (!result) {
        return result; 
    }
    
    return TRUE;
}

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : tc_4_main_task_cache_unbuffered
* Comments     :
*   Set file to be cached and unbuffered
*END*----------------------------------------------------------------------*/
_mqx_int tc_4_main_task_cache_unbuffered(MQX_FILE_PTR flash_file)
{
    _mqx_int  result = 0;

    result = ioctl(flash_file, FLASH_IOCTL_ENABLE_SECTOR_CACHE, NULL);
    result = (result == MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result, " Test 4.2.1 # IOCTL enable cache failed"); 
    if (!result) {
        return result; 
    }

    result = ioctl(flash_file, FLASH_IOCTL_DISABLE_BUFFERING, NULL);
    result = (result == MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result, " Test 4.2.2 # IOCTL disable buffer failed"); 
    if (!result) {
        return result; 
    }

    return TRUE;
}

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : tc_4_main_task_uncached
* Comments     :
*   Set file to be uncached
*END*----------------------------------------------------------------------*/
_mqx_int tc_4_main_task_uncached(MQX_FILE_PTR flash_file)
{
    _mqx_int  result = 0;
    result = ioctl(flash_file, FLASH_IOCTL_DISABLE_SECTOR_CACHE, NULL);
    result = (result == MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result, " Test 4.3 # IOCTL disable cache failed"); 
    return result;
}

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : tc_4_main_task_write_pass
* Comments     :
*   Write pattern, read data, compare
*END*----------------------------------------------------------------------*/
_mqx_int tc_4_main_task_write_pass(
    MQX_FILE_PTR    flash_file, 
    _mqx_int        location, 
    unsigned char       *write_buffer, 
    _mqx_int        buffer_size, 
    _mqx_int        buffer_type
)
{
    _mqx_int result = TRUE;
    
    // create pattern 
    buffer_create_pattern(write_buffer, buffer_size, buffer_type);
    
    /* write buffer 2 */
    fseek(flash_file, location, IO_SEEK_SET);
    result = write(flash_file, write_buffer, buffer_size);
    result = (result == buffer_size);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result, " Test 4.4.1 # Write new data return unexpected value");
    if (!result) { 
        return FALSE;
    }

    /* read again */
    fseek(flash_file, location, IO_SEEK_SET);
    result = read(flash_file, write_buffer, buffer_size);
    result = (result == buffer_size);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result, " Test 4.4.2 # Read new data return unexpected value");
    if (!result) { 
        return FALSE;
    }

    /* compare buffers*/
    result = buffer_compare_pattern(write_buffer, buffer_size, buffer_type);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result, " Test 4.4.3 # Compare buffers return unexpected value"); 
    if (!result) { 
        return FALSE;
    }
    return TRUE;
}

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : tc_4_main_task_write_fail
* Comments     :
*   Write pattern should fail
*END*----------------------------------------------------------------------*/
_mqx_int tc_4_main_task_write_fail(
    MQX_FILE_PTR    flash_file, 
    _mqx_int        location, 
    unsigned char       *write_buffer, 
    _mqx_int        buffer_size
)
{
    _mqx_int result = TRUE;
    
    /* write pattern */
    fseek(flash_file, location, IO_SEEK_SET);
    result = write(flash_file, write_buffer, buffer_size);
    result = (result != buffer_size);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result, " Test 4.5.1 # Write new data should fail");
    if (!result) { 
        return FALSE;
    }

    return TRUE;
}

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : tc_4_main_task_erase_sector
* Comments     :
*   Function will erase sector placed under data
*   Data -> interval <location, location + size)
*END*----------------------------------------------------------------------*/
_mqx_int tc_4_main_task_erase_sector(
    MQX_FILE_PTR    flash_file, 
    _mqx_int        location, 
    _mqx_int        size 
)
{
    _mqx_int    result = TRUE;
    _mqx_int    sector_start = 0;
    _mqx_int    sector_size = 0;

    /* find location of first sector */
    sector_start = find_sector_start(flash_file, location);
    if (sector_start < 0) { 
        return FALSE; 
    }

    /* size over sectors = sector offset + data size */
    size = (location - sector_start) + size;
    /* loop over sectors size */
    while (size > 0) {
        /* set head to sector start */
        fseek(flash_file, sector_start, IO_SEEK_SET);
        
        /* get current sector size */
        sector_size = 0;
        result = ioctl(flash_file, FLASH_IOCTL_GET_SECTOR_SIZE, &sector_size);
        result = ((result == MQX_OK) && (sector_size > 0));
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, result, " Test 4.6.1 # Ioctl FLASH_IOCTL_GET_SECTOR_SIZE return unexpected value");
        if (!result) {
            return FALSE;
        }
        
        /* erase sector */
        result = ioctl(flash_file, FLASH_IOCTL_ERASE_SECTOR, NULL);
        result = (result == MQX_OK);
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, result, " Test 4.6.2 # Ioctl FLASH_IOCTL_ERASE_SECTOR return unexpected value");
        if (!result) { 
            return FALSE;
        }
        
        /* move to next sector */
        sector_start += sector_size;
        size = size - sector_size;
    }
    return TRUE;
}

/*TASK*-------------------------------------------------------------------
* 
* Task Name    : tc_4_main_task
* Comments     :
*   Test case tc_4_main_task will try to open single file and 
*   write pattern to different sector, different size, different mode, ... 
*END*----------------------------------------------------------------------*/
void tc_4_main_task(void)
{
#if !ENABLE_CASE4
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, TRUE, "Test 4.7.0 # Disabled");
#else
    MQX_FILE_PTR    flash_file = NULL;
    _mqx_int        i = 0, j = 0;
    _mqx_int        sector_size = 0;
    _mqx_int        result = 0;
    _mqx_int        location = 0;
    /* start from second sector 0 */
    _mqx_int        start_location = 0;
    unsigned char       *write_buffer = NULL;
    _mqx_int        buffer_size = 0;
    _mqx_int        sub_mode = 0, protect = 0;
    
    /* init required data */
    flash_file = fopen(FLASHX_WORK_RW, NULL);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (flash_file != NULL), " Test 4.7.1 # Open file failed");
    if (!flash_file){
        goto tc_4_main_task_deinit;
    }
    
    /* remove protection if protection is supported and set */
    ioctl(flash_file, FLASH_IOCTL_WRITE_PROTECT, (void *)&protect);
    
    /* get sector size */
    fseek(flash_file, 0, IO_SEEK_SET);
    result = ioctl(flash_file, FLASH_IOCTL_GET_SECTOR_SIZE, &sector_size);
    result = (result == MQX_OK && sector_size > 0);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result, " Test 4.7.2 # Sector size failed");
    if (!result) { 
        goto tc_4_main_task_deinit;
    }
    
    
    /* set position and buffer size */
    start_location = 0;
    for(i = 0; 1; i++) {
#if CASE4_HALF_PARTIAL
        if (i == 0) { 
            sub_mode = CASE4_HALF_PARTIAL;
            
            location = start_location + (sector_size / 4);
            buffer_size = sector_size / 2;
            
            /* check if buffer will reside inside flash_file, if not 
             * move to first sector of flash_file and set appropiate offset */
            result = check_valid_place(flash_file, location, buffer_size);
            if (!result) {
                location = location % sector_size;
            }
        }
        else 
#endif
#if CASE4_ONE_PARTIAL
        if (i == 1) { 
            sub_mode = CASE4_ONE_PARTIAL;
            
            location = start_location + ((sector_size / 4) * 3);
            buffer_size = sector_size;
            
            /* check if buffer will reside inside flash_file, if not 
             * move to first sector of flash_file and set appropiate offset */
            result = check_valid_place(flash_file, location, buffer_size);
            if (!result) {
                location = location % sector_size;
            }
        }
        else 
#endif
#if CASE4_THREE_FULL
        if (i == 2) { 
            sub_mode = CASE4_THREE_FULL;
            
            location = start_location + (sector_size * 2);
            buffer_size = sector_size * 3;
            
            /* check if buffer will reside inside flash_file, if not 
             * move to first sector of flash_file and set appropiate offset */
            result = check_valid_place(flash_file, location, buffer_size);
            if (!result) {
                location = location % sector_size;
            }
        }
        else 
#endif
#if CASE4_THREE_PARTIAL
        if (i == 3) { 
            sub_mode = CASE4_THREE_PARTIAL;
            
            location = start_location + (sector_size * 5) + (sector_size / 2);
            buffer_size = sector_size * 3;
            
            /* check if buffer will reside inside flash_file, if not 
             * move to first sector of flash_file and set appropiate offset */
            result = check_valid_place(flash_file, location, buffer_size);
            if (!result) {
                location = location % sector_size;
            }
        } else 
#endif
        if (i == 4) { 
            break;
        }
        else { 
            continue;
        }
        
        /* allocate buffers */
        buffer_size = buffer_size;
        write_buffer = _mem_alloc_zero((_mqx_uint)buffer_size);
        result = (write_buffer != NULL);
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, result, " Test 4.7.3 # Failed to allocate buffers");
        if (!result) { 
            goto tc_4_main_task_deinit;
        }
        
        /* decide which action should be performed 
         * if assert fail in sub_test, just jump to deinit */
        for (j = 0; 1; j++) { 
            result = ((sub_mode >> CASE4_SHIFT_VALUE(j)) & CASE4_SHIFT_MASK);
            if (result == CASE4_USE_END) break;
            
            switch(result) { 
                case CASE4_USE_CACHED_BUFFERED:
                    /* write with enabled cache and enabled buffer, write HAVE TO pass ! */
                    result = tc_4_main_task_cached_buffered(flash_file);
                    if (!result) {
                        goto tc_4_main_task_deinit;
                    }
                    result = tc_4_main_task_write_pass(flash_file, location, write_buffer, buffer_size, (i + j));
                    if (!result) { 
                        goto tc_4_main_task_deinit;
                    }
                break;
                case CASE4_USE_CACHED_UNBUFFERED:
                    /* write with enabled cache and disabled buffer, write HAVE TO pass ! */
                    result = tc_4_main_task_cache_unbuffered(flash_file);
                    if (!result) {
                        goto tc_4_main_task_deinit;
                    }
                    result = tc_4_main_task_write_pass(flash_file, location, write_buffer, buffer_size, (i + j));
                    if (!result) { 
                        goto tc_4_main_task_deinit;
                    }
                break;
                case CASE4_USE_ERASE_SECTOR:
                    /* erase sectors where buffer data should be placed */
                    result = tc_4_main_task_erase_sector(flash_file, location, buffer_size);
                    if (!result) { 
                        goto tc_4_main_task_deinit;
                    }
                    /* check if it erased */
                    result = place_is_erased(flash_file, location, buffer_size);
                    result = (result > 0);
                    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result, " Test 4.7.4 # Function place_is_erased failed");
                    if (!result) { 
                        goto tc_4_main_task_deinit;
                    }
                break;
                case CASE4_USE_UNCACHED_PASS:
                    /* write with disabled cache, write HAVE TO pass ! */
                    result = tc_4_main_task_uncached(flash_file);
                    if (!result) { 
                        goto tc_4_main_task_deinit;
                    }
                    result = tc_4_main_task_write_pass(flash_file, location, write_buffer, buffer_size, (i + j));
                    if (!result) { 
                        goto tc_4_main_task_deinit;
                    }
                break;
                case CASE4_USE_UNCACHED:
                    /* write with disabled cache, write status depends on value of sectors ! */
                    result = tc_4_main_task_uncached(flash_file);
                    if (!result) {
                        goto tc_4_main_task_deinit;
                    }
                    result = place_is_clean(flash_file, location, buffer_size);
                    if (result < 0) { 
                        EU_ASSERT_REF_TC_MSG(tc_4_main_task, FALSE, " Test 4.7.5 # Function place_is_clean failed");
                        goto tc_4_main_task_deinit;
                    }
                    else if (result > 0) { 
                        /* place is clean write should pass */
                        result = tc_4_main_task_write_pass(flash_file, location, write_buffer, buffer_size, (i + j));
                    }
                    else { 
                        /* place is messed write should fail */
                        result = tc_4_main_task_write_fail(flash_file, location, write_buffer, buffer_size);
                    }
                    if (!result) { 
                        goto tc_4_main_task_deinit;
                    }
                break;
            }
        }
        
        /* free buffers */
        _mem_free(write_buffer);
        write_buffer = NULL;
    }

/* clean up test resources */
tc_4_main_task_deinit:
    /* deinit - close file_ptr */
    if (write_buffer != NULL) { 
        _mem_free(write_buffer);
        write_buffer = NULL;
    }
    if (flash_file != NULL) { 
        result = fclose(flash_file);
        flash_file = NULL;
    }
#endif
}

/*TASK*-------------------------------------------------------------------
* 
* Task Name    : tc_5_main_task
* Comments     :
*   Test: disable cache for all files
*END*----------------------------------------------------------------------*/
_mqx_int tc_5_main_task_cached_files(
    MQX_FILE_PTR write_file, 
    MQX_FILE_PTR read_file, 
    MQX_FILE_PTR mirror_file
)
{
    _mqx_int result = 0;
    
    result = ioctl(write_file, FLASH_IOCTL_DISABLE_BUFFERING, NULL);
    result = (result == MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result, " Test 5.1.1 # IOCTL FLASH_IOCTL_DISABLE_BUFFERING fail for read_file");
    if (!result) return FALSE;
    result = ioctl(write_file, FLASH_IOCTL_ENABLE_SECTOR_CACHE, NULL);
    result = (result == MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result, " Test 5.1.2 # IOCTL FLASH_IOCTL_ENABLE_SECTOR_CACHE fail for read_file");
    if (!result) return FALSE;
    
    result = ioctl(read_file, FLASH_IOCTL_DISABLE_BUFFERING, NULL);
    result = (result == MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result, " Test 5.1.3 # IOCTL FLASH_IOCTL_DISABLE_BUFFERING fail for write_file");
    if (!result) return FALSE;
    result = ioctl(read_file, FLASH_IOCTL_ENABLE_SECTOR_CACHE, NULL);
    result = (result == MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result, " Test 5.1.4 # IOCTL FLASH_IOCTL_ENABLE_SECTOR_CACHE fail for write_file");
    if (!result) return FALSE;
    
    result = ioctl(mirror_file, FLASH_IOCTL_DISABLE_BUFFERING, NULL);
    result = (result == MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result, " Test 5.1.5 # IOCTL FLASH_IOCTL_DISABLE_BUFFERING fail for mirror_file");
    if (!result) return FALSE;
    result = ioctl(mirror_file, FLASH_IOCTL_ENABLE_SECTOR_CACHE, NULL);
    result = (result == MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result, " Test 5.1.6 # IOCTL FLASH_IOCTL_ENABLE_SECTOR_CACHE fail for mirror_file");
    if (!result) return FALSE;
    
    return TRUE;
}


/*TASK*-------------------------------------------------------------------
* 
* Task Name    : tc_5_main_task_write_and_check_buffer
* Comments     :
*   Test: write to same area using multiple files
*END*----------------------------------------------------------------------*/
_mqx_int tc_5_main_task_write_and_check_buffer(
    MQX_FILE_PTR write_file, 
    MQX_FILE_PTR read_file, 
    unsigned char *buffer, 
    _mqx_int buffer_size, 
    _mqx_int location, 
    _mqx_int pattern_index 
)
{
    _mqx_int result = 0; 
    
    /* seek to position */
    fseek(write_file, location, IO_SEEK_SET);
    fseek(read_file, location, IO_SEEK_SET);
    
    /* create pattern, upper half is different than previous */
    buffer_create_pattern(buffer, buffer_size, pattern_index);
    buffer_create_pattern(&buffer[buffer_size/2], (buffer_size - (buffer_size/2)), (pattern_index + 1));
    
    /* write whole buffer */
    result = write(write_file, buffer, buffer_size);
    result = (result == buffer_size);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result, " Test 5.2.1 # Write function failed");
    if (!result) return FALSE;
    
    /* read whole buffer */
    result = read(read_file, buffer, buffer_size);
    result = (result == buffer_size);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result, " Test 5.2.2 # Read function failed");
    if (!result) return FALSE;
    
    /* whole buffer has non homogeneous pattern, compare should fail */
    result = buffer_compare_pattern(buffer, buffer_size, pattern_index);
    result = (result == FALSE);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result, " Test 5.2.3 # Compare buffers failed");
    if (!result) return FALSE;
    
    /* upper half of buffer should has homogenous pattern */
    result = buffer_compare_pattern(&buffer[buffer_size/2], (buffer_size - (buffer_size/2)), (pattern_index + 1));
    result = (result == TRUE);//TODO
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result, " Test 5.2.4 # Compare buffers failed");
    if (!result) return FALSE;
    
    return TRUE;
}



/*TASK*-------------------------------------------------------------------
* 
* Task Name    : tc_5_main_task
* Comments     :
*   Test: write to same area using multiple files
*END*----------------------------------------------------------------------*/
void tc_5_main_task(void)
{
#if !ENABLE_CASE5
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, TRUE, "Test 5.2.0 # Disabled");
#else
    MQX_FILE_PTR    write_file = NULL, read_file = NULL;
    MQX_FILE_PTR    mirror_file = NULL, tmp_file = NULL;
    unsigned char   *read_buffer = NULL, *buffer = NULL;
    _mqx_int        buffer_size = 0, sector_size = 0;
    _mqx_int        result = 0, location = 0, i = 0;
    _mqx_int        part_size = 0, part_offset = 0;
    _mqx_int        start_location = 0, protect = 0;
    
    
    /* open files */
    write_file = fopen(FLASHX_WORK_RW, NULL);
    result = (write_file != NULL);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result, " Test 5.3.1 # Open read_file failed");
    if (!result) {
        goto tc_5_main_task_deinit;
    }
    
    /* remove protection if protection is supported and set */
    ioctl(write_file, FLASH_IOCTL_WRITE_PROTECT, (void *)&protect);
    
    read_file = fopen(FLASHX_WORK_RW, NULL);
    result = (read_file != NULL);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result, " Test 5.3.2 # Open write_file failed");
    if (!result) {
        goto tc_5_main_task_deinit;
    }
    mirror_file = fopen(FLASHX_MIRROR_RW, NULL);
    result = (mirror_file != NULL);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result, " Test 5.3.3 # Open mirror_file failed");
    if (!result) {
        goto tc_5_main_task_deinit;
    }
    
    result = tc_5_main_task_cached_files(write_file, read_file, mirror_file);
    if (!result) {
        goto tc_5_main_task_deinit;
    }
    
    
    start_location = 0;
    /* get sector size */
    fseek(read_file, 0, IO_SEEK_SET);
    result = ioctl(read_file, FLASH_IOCTL_GET_SECTOR_SIZE, &sector_size);
    result = (result == MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result, " Test 5.3.4 # IOCTl FLASH_IOCTL_GET_SECTOR_SIZE fail for read_file");
    if (!result) { 
        goto tc_5_main_task_deinit;
    }
    
    
    /* loop over tests settings ... */
    for (i = 0; 1; i++) { 
#if CASE5_HALF_PARTIAL
        if (i == 0) { 
            location = start_location + (sector_size / 4);
            buffer_size = sector_size / 2;
            
            /* check if buffer will reside inside flash_file, if not 
             * move to first sector of flash_file and set appropiate offset */
            result = check_valid_place(read_file, location, buffer_size);
            if (!result) {
                location = location % sector_size;
            }
            
            /* swap file pointer */
            tmp_file = write_file;
            write_file = read_file;
            read_file = tmp_file;
        }
        else 
#endif
#if CASE5_ONE_PARTIAL
        if (i == 1) { 
            location = start_location + (3 * (sector_size / 2));
            buffer_size = sector_size;
            
            /* check if buffer will reside inside flash_file, if not 
             * move to first sector of flash_file and set appropiate offset */
            result = check_valid_place(read_file, location, buffer_size);
            if (!result) {
                location = location % sector_size;
            }
            
            /* swap file pointer */
            tmp_file = write_file;
            write_file = read_file;
            read_file = tmp_file;
        }
        else 
#endif
#if CASE5_ONE_FULL
        if (i == 2) {
            location = start_location + (sector_size * 2);
            buffer_size = sector_size;
            
            /* check if buffer will reside inside flash_file, if not 
             * move to first sector of flash_file and set appropiate offset */
            result = check_valid_place(read_file, location, buffer_size);
            if (!result) {
                location = location % sector_size;
            }
            
            /* swap file pointer */
            tmp_file = write_file;
            write_file = read_file;
            read_file = tmp_file;
        }
        else 
#endif
#if CASE5_TWO_PARTIAL
        if (i == 3) {
            location = start_location + (sector_size * 3) + (sector_size / 3);
            buffer_size = sector_size * 2;
            
            /* check if buffer will reside inside flash_file, if not 
             * move to first sector of flash_file and set appropiate offset */
            result = check_valid_place(read_file, location, buffer_size);
            if (!result) {
                location = location % sector_size;
            }
            
            /* swap file pointer */
            tmp_file = write_file;
            write_file = read_file;
            read_file = tmp_file;
        }
        else 
#endif 
#if CASE5_THREE_FULL
        if (i == 4) { 
            location = start_location + (sector_size * 6);
            buffer_size = sector_size * 3;
            
            /* check if buffer will reside inside flash_file, if not 
             * move to first sector of flash_file and set appropiate offset */
            result = check_valid_place(read_file, location, buffer_size);
            if (!result) {
                location = location % sector_size;
            }
            
            /* swap file pointer */
            tmp_file = write_file;
            write_file = read_file;
            read_file = tmp_file;
        }
        else
#endif
        if (i == 5) { 
            break;
        }
        else { 
            continue;
        }
        
        
        /* buffer allocation */
        buffer = _mem_alloc_zero((_mqx_uint)buffer_size);
        result = (buffer != NULL);
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, result, " Test 5.3.5 # Buffer allocation failed");
        if (!result) { 
            goto tc_5_main_task_deinit;
        }
        
        /* check write/read to same flash file */
        result = tc_5_main_task_write_and_check_buffer(
            write_file, 
            read_file, 
            buffer, 
            buffer_size, 
            location, 
            i
        );
        if (!result) { 
            goto tc_5_main_task_deinit;
        }
        /* check write to mirror file, read from read file */
        result = tc_5_main_task_write_and_check_buffer(
            mirror_file, 
            read_file, 
            buffer, 
            buffer_size, 
            location, 
            (i + 2)
        );
        if (!result) { 
            goto tc_5_main_task_deinit;
        }

        /* free buffers */
        _mem_free(buffer);
        buffer = NULL;
    }


tc_5_main_task_deinit:
    if (buffer != NULL) { 
        _mem_free(buffer);
        buffer = NULL;
    }
    if (write_file != NULL) { 
        fclose(write_file);
    }
    if (read_file != NULL) { 
        fclose(read_file);
    }
    if (mirror_file != NULL) { 
        fclose(mirror_file);
    }
#endif
}


/*TASK*-------------------------------------------------------------------
* 
* Task Name    : tc_6_main_task
* Comments     :
*   Test: read, fseek 
*END*----------------------------------------------------------------------*/
void tc_6_main_task(void)
{
#if !ENABLE_CASE6
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, TRUE, "Test 6.0 # Disabled");
#else
    MQX_FILE_PTR                    flash_file = NULL;
    _mqx_int        last = 0;
    _mqx_int        offset = 0;
    _mqx_int        result = 0;
    _mqx_int        location = 0, buffer_size = 0;
    unsigned char   *first_buffer = NULL, *second_buffer = NULL, *third_buffer = NULL;
    
    /* init required data */
    flash_file = fopen(FLASHX_WORK_RO, NULL);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (flash_file != NULL), " Test 6.1 # Open file failed");
    if (!flash_file){
        goto tc_6_main_task_deinit;
    }
    
    /* get first sector size and calculate buffer_size */
    fseek(flash_file, 0, IO_SEEK_SET);
    result = ioctl(flash_file, FLASH_IOCTL_GET_SECTOR_SIZE, &buffer_size);
    result = (result == MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, result, " Test 6.2 # Ioctl FLASH_IOCTL_GET_BLOCK_MAP failed");
    if (!result){
        goto tc_6_main_task_deinit;
    }
    buffer_size = buffer_optimal_size(buffer_size, 3);
    
    /* obtain eof position and calculate offset */
    fseek(flash_file, 0, IO_SEEK_END);
    last = ftell(flash_file);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, last, " Test  6.3 # Last position cannot be zero");
    if (!last) { 
        goto tc_6_main_task_deinit;
    }
    offset = last / 5;


    /* allocate buffers */
    first_buffer = _mem_alloc_zero((_mqx_uint)buffer_size);
    second_buffer = _mem_alloc_zero((_mqx_uint)buffer_size);
    third_buffer = _mem_alloc_zero((_mqx_uint)buffer_size);
    /* read assert fail */
    result = (first_buffer != NULL && second_buffer != NULL && third_buffer != NULL);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, result, " Test 6.4 # Buffers allocation failed");
    if (!result) { 
        goto tc_6_main_task_deinit;
    }


    /* loop over flash size, set seek to same position from start, end, current 
     * and check if readed data are the same 
     * */
    for (location = 0; location <= (last - buffer_size); location += offset) {
        /* calculate position from begining */
        fseek(flash_file, location, IO_SEEK_SET);
        result = read(flash_file, first_buffer, buffer_size);
        /* read assert fail */
        result = (result == buffer_size);
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, result, " Test 6.5 # Reading first_buffe failed");
        if (!result) {
            goto tc_6_main_task_deinit;
        }
        
        /* calculate position from end */
        fseek(flash_file, (-1)*(last - location), IO_SEEK_END);
        result = read(flash_file, second_buffer, buffer_size);
        /* read assert fail */
        result = (result == buffer_size);
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, result, " Test 6.6 # Reading second_buffer failed");
        if (!result) {
            goto tc_6_main_task_deinit;
        }
        result = buffer_compare_buffer(first_buffer, second_buffer, buffer_size);
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, result, " Test 6.7 # First and second buffer should be the same");
        if (!result) {
            goto tc_6_main_task_deinit;
        }
        
        /* calculate position - move back  buffer length */
        fseek(flash_file, (buffer_size * (-1)), IO_SEEK_CUR);
        result = read(flash_file, third_buffer, buffer_size);
        /* read assert fail */
        result = (result == buffer_size);
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, result, " Test  6.8 # Reading third_buffer failed");
        if (!result) {
            goto tc_6_main_task_deinit;
        }
        
        buffer_compare_buffer(first_buffer, second_buffer, buffer_size);
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, result, " Test 6.9 # Second and third buffer should be the same");
        if (!result) {
            goto tc_6_main_task_deinit;
        }
    }

    /* set position before end and read over the end */
    fseek(flash_file, (-1)*(buffer_size / 2), IO_SEEK_END);
    result = read(flash_file, third_buffer, buffer_size);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == (buffer_size / 2)), " Test 6.10 # Read should fail and return len/2 character");

    
/* clean up test resources */
tc_6_main_task_deinit:
    if (first_buffer != NULL) {
        _mem_free(first_buffer);
        first_buffer = NULL;
    }
    if (second_buffer != NULL) {
        _mem_free(second_buffer);
        second_buffer = NULL;
    }
    if (third_buffer != NULL) {
        _mem_free(third_buffer);
        third_buffer = NULL;
    }
    if (flash_file != NULL) {
        result = fclose(flash_file);
        flash_file = NULL;
    }
#endif
}


/*TASK*-------------------------------------------------------------------
* 
* Task Name    : tc_7_main_task
* Comments     :
*   Test: read, write after valid location
*END*----------------------------------------------------------------------*/
void tc_7_main_task_valid_write(void)
{
    _mqx_int        location = 0, valid_part = 0;
    _mqx_int        buffer_size = 0, result = 0, protect = 0;
    MQX_FILE_PTR    flash_file = NULL;
    unsigned char       *write_buffer = NULL;
    
    /* fopen */
    flash_file = fopen(FLASHX_WORK_RW, NULL);
    result = (flash_file != NULL);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result, " Test 7.1.1 # fopen fail");
    if (!result) {
        goto tc_7_main_task_valid_write_deinit;
    }
    
    /* disable write protect */
    ioctl(flash_file, FLASH_IOCTL_WRITE_PROTECT, (void *)&protect);
    
    /* set buffer size */
    fseek(flash_file, -1, IO_SEEK_END);
    result = ioctl(flash_file, FLASH_IOCTL_GET_SECTOR_SIZE, &buffer_size);
    result = (result == MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result, " Test 7.1.2 # Ioctl FLASH_IOCTL_GET_SECTOR_SIZE fail");
    if (!result) {
        goto tc_7_main_task_valid_write_deinit;
    }
    buffer_size = buffer_optimal_size(buffer_size, 1);
    
    /* allocate buffers */
    write_buffer = _mem_alloc_zero((_mqx_uint)buffer_size);
    result = (write_buffer != NULL);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result, " Test 7.1.3 # Buffer allocation failed");
    if (!result) {
        goto tc_7_main_task_valid_write_deinit;
    }
    
    /* set location */
    fseek(flash_file, 0, IO_SEEK_END);
    location = ftell(flash_file);
    valid_part = (buffer_size / 2);
    location = location - valid_part;
    
    /* and try to write over end of flash file */
    buffer_create_pattern(write_buffer, buffer_size, 1);
    fseek(flash_file, location, IO_SEEK_SET);
    result = write(flash_file, write_buffer, buffer_size);
    result = (result == valid_part);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result, " Test 7.1.4 # Write fail ");
    if (!result) { 
        goto tc_7_main_task_valid_write_deinit;
    }

    /* try to read written data */
    fseek(flash_file, location, IO_SEEK_SET);
    result = read(flash_file, write_buffer, buffer_size);
    result = (result == valid_part);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result, " Test 7.1.5 # Read fail ");
    if (!result) { 
        goto tc_7_main_task_valid_write_deinit;
    }

    /* compare valid buffer part */
    result = buffer_compare_pattern(write_buffer, buffer_size, 1);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result, " Test 7.1.6 # Compare fail");
    if (!result) { 
        goto tc_7_main_task_valid_write_deinit;
    }
    
tc_7_main_task_valid_write_deinit:
    if (write_buffer != NULL) {
        _mem_free(write_buffer);
        write_buffer = NULL;
    }
    if (flash_file != NULL) {
        fclose(flash_file);
        flash_file = NULL;
    }
}


/*TASK*-------------------------------------------------------------------
* 
* Task Name    : tc_7_main_task
* Comments     :
*   Test: read, write after valid location
*END*----------------------------------------------------------------------*/
void tc_7_main_task_invalid_write(void)
{
    _mqx_int        location = 0, valid_part = 0;
    _mqx_int        buffer_size = 0, result = 0, protect = 0;
    MQX_FILE_PTR    flash_file = NULL;
    unsigned char       *write_buffer = NULL;
    
    /* open flashx file */
    flash_file = fopen(FLASHX_WORK_RW, NULL);
    result = (flash_file != NULL);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result, " Test 7.2.1 # fopen fail");
    if (!result) {
        goto tc_7_main_task_invalid_write_deinit;
    }
    
    /* disable write protect */
    ioctl(flash_file, FLASH_IOCTL_WRITE_PROTECT, (void *)&protect);
    
    /* set buffer size */
    fseek(flash_file, -1, IO_SEEK_END);
    result = ioctl(flash_file, FLASH_IOCTL_GET_SECTOR_SIZE, &buffer_size);
    result = (result == MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result, " Test 7.2.2 # Ioctl FLASH_IOCTL_GET_SECTOR_SIZE fail");
    if (!result) {
        goto tc_7_main_task_invalid_write_deinit;
    }
    buffer_size = buffer_optimal_size(buffer_size, 1);
    
    /* allocate buffers */
    write_buffer = _mem_alloc_zero((_mqx_uint)buffer_size);
    result = (write_buffer != NULL);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result, " Test 7.2.3 # Buffer allocation failed");
    if (!result) {
        goto tc_7_main_task_invalid_write_deinit;
    }
    
    /* set location */
    fseek(flash_file, 0, IO_SEEK_END);
    location = ftell(flash_file);
    valid_part = (buffer_size / 2);
    location = location + valid_part;
    
    /* and try to write over end of flash file */
    buffer_create_pattern(write_buffer, buffer_size, 2);
    fseek(flash_file, location, IO_SEEK_SET);
    result = write(flash_file, write_buffer, buffer_size);
    result = (result == 0);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result, " Test 7.2.4 # Write fail ");
    if (!result) { 
        goto tc_7_main_task_invalid_write_deinit;
    }

    /* try to read written data */
    fseek(flash_file, location, IO_SEEK_SET);
    result = read(flash_file, write_buffer, buffer_size);
    result = (result == 0);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result, " Test 7.2.5 # Read fail ");
    if (!result) { 
        goto tc_7_main_task_invalid_write_deinit;
    }
    
tc_7_main_task_invalid_write_deinit:
    if (write_buffer != NULL) {
        _mem_free(write_buffer);
        write_buffer = NULL;
    }
    if (flash_file != NULL) {
        fclose(flash_file);
        flash_file = NULL;
    }
}


/*TASK*-------------------------------------------------------------------
* 
* Task Name    : tc_7_main_task
* Comments     :
*   Test: read, write after valid location
*END*----------------------------------------------------------------------*/
void tc_7_main_task(void) 
{ 
#if !ENABLE_CASE7
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, TRUE, "Test 7.0 # Disabled");
#else
    tc_7_main_task_valid_write();
    tc_7_main_task_invalid_write();
#endif
}


EU_TEST_SUITE_BEGIN(suite_int)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST # 1 - Test device and flash information"),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST # 2 - Test blocks size and seek"),
    EU_TEST_CASE_ADD(tc_3_main_task, " TEST # 3 - Test test chip erase"),
    EU_TEST_CASE_ADD(tc_4_main_task, " TEST # 4 - Test single write operation"),
    EU_TEST_CASE_ADD(tc_5_main_task, " TEST # 5 - Test multiple write operation"),
    EU_TEST_CASE_ADD(tc_6_main_task, " TEST # 6 - Test read and seek operation"),
    EU_TEST_CASE_ADD(tc_7_main_task, " TEST # 7 - Test read/write at invalid address"),
EU_TEST_SUITE_END(suite_int)
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_int, " - Test flashx suite")
EU_TEST_REGISTRY_END()


/*TASK*-------------------------------------------------------------------
* 
* Task Name    : main_task
* Comments     :
*   Tests the flash device
*
*END*----------------------------------------------------------------------*/
void main_task(uint32_t dummy)
{
    _int_install_unexpected_isr();
    _io_flashx_install(FLASHX_DRIVER_NAME, &_test_flashx_init);
    eunit_mutex_init();
    EU_RUN_ALL_TESTS(eunit_tres_array);
    eunit_test_stop();
}

