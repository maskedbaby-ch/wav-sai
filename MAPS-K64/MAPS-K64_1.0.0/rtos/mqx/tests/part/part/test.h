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
* $FileName: test.h$
* $Version : 3.8.2.1$
* $Date    : Jul-23-2012$
*
* Comments:
*
*   This include file is used to define the demonstration constants
*
*END************************************************************************/


#define MAIN_TASK  10
#define TEST_TASK  11
#define TEST2_TASK 12

#if PSP_MQX_CPU_IS_KINETIS_L

#define MEM_EXT_SIZE          (512)
#define MEM_PART_SIZE         (512)

#define BLOCK_SIZE            (50)
#define NUMBER_OF_BLOCKS      (5)

#else
#define MEM_EXT_SIZE          (2048)
#define MEM_PART_SIZE         (2048)

#define BLOCK_SIZE            (100)
#define NUMBER_OF_BLOCKS      (10)

#endif


extern void      main_task(uint32_t);
extern void      test_task(uint32_t);
extern void      test2_task(uint32_t);
extern _mqx_uint test_alloc_all_blocks(_partition_id,bool);
extern _mqx_uint test_free_all_blocks(_mqx_uint_ptr);
extern void      test_fill_block(unsigned char *,_mqx_uint);
extern _mqx_uint test_calculate_min_part_size(_mqx_uint,_mqx_uint,_mqx_uint);
extern _mqx_uint test_calculate_max_part_size(_mqx_uint,_mqx_uint,_mqx_uint);
extern _mqx_uint test_calculate_actual_part_size(unsigned char *,_mqx_uint,_mqx_uint,
   _mqx_uint);
extern _mqx_uint test_calculate_max_block_size(_mqx_uint);
extern _mqx_uint test_calculate_number_of_blocks(unsigned char *,_mqx_uint,_mqx_uint,bool);

