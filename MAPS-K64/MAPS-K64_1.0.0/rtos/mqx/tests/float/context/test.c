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
* $Version : 3.8.1.0$
* $Date    : Aug-10-2012$
*
* Comments:
*
*   This test creates four tasks
*     float1    - priority 5
*     float2    - priority 6
*     no_float3 - priority 7
*     no_float4 - priority 8
*   where 'float' means that task has floating point support turned on
*   since its creation by flag in task template structure.
*   'no float' means that floating point support is turned on during the
*   task execution.
*   General idea of the test is to set fpu registers to some known value in
*   one task, then switch to another task, damage registers there, switch
*   back and check that we have the values which we set before task switch.
*   This we do across various combination of float and nonfloat tasks.
*
*   Follow the task switches numbering to see the testflow.
*
*END************************************************************************/

#include <math.h>
#include <mqx.h>
#include <bsp.h>
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"

#define MAIN_TASK      10
#define FLOAT1_TASK    11
#define FLOAT2_TASK    12
#define NO_FLOAT1_TASK 13
#define NO_FLOAT2_TASK 14

void tc_1_main_task( void );
void tc_1_float1_task( void );
void tc_1_float2_task( void );
void tc_1_no_float1_task( void );
void tc_1_no_float2_task( void );

void main_task( uint32_t );
void float1_task( uint32_t index );
void float2_task( uint32_t index );
void no_float1_task( uint32_t index );
void no_float2_task( uint32_t index );

void fpu_fill( void );
void fpu_damage( void );
unsigned int fpu_check( void );

_task_id float1_id;
_task_id float2_id;
_task_id no_float1_id;
_task_id no_float2_id;

TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
    { MAIN_TASK,      main_task,      1800,  9, "main",      MQX_AUTO_START_TASK },    
    { FLOAT1_TASK,    float1_task,    2000,  5, "float1",    MQX_AUTO_START_TASK | MQX_FLOATING_POINT_TASK },
    { FLOAT2_TASK,    float2_task,    2000,  6, "float2",    MQX_AUTO_START_TASK | MQX_FLOATING_POINT_TASK },
    { NO_FLOAT1_TASK, no_float1_task, 2000,  7, "no_float1", MQX_AUTO_START_TASK },
    { NO_FLOAT2_TASK, no_float2_task, 2000,  8, "no_float2", MQX_AUTO_START_TASK },
    { 0,              0,              0,     0, 0,           0 }
};
/*
    Define Test Suite
*/
EU_TEST_SUITE_BEGIN(suite_float0)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST #1: Test of saving and loading of floating point context")
EU_TEST_SUITE_END(suite_float0)

/*
    Add test suites
*/
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_float0, " Test of Floating Point")
EU_TEST_REGISTRY_END()

/* 
    Task Name    : float 1 task
    Comments     :
      01 - we starts here, get task id and block, go to float2
      10 - we came from float2 and damage registers, go to float2
*/
void tc_1_float1_task( void )
{
    /* 01 */
    float1_id = _task_get_id();
    fpu_fill();
    _task_block();    
    
    /* 10 */    
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, fpu_check(), "Test #1 Testing: 1.3.1 float to float and back.");    

    fpu_damage();
    _task_block();
}
void float1_task( uint32_t index ) 
{
    eunit_mutex_init();
    EU_RUN_ALL_TESTS(eunit_tres_array);

    tc_1_float1_task();
}

/* 
    Task Name    : float 2 task
    Comments     :
      02 - came from float1, get id, go to no_float1
      07 - came from no_float2, damage registers and go back to no_float2
      09 - came from no_float2, we fill registers and go to float1
      11 - came from float1, we check registers and fill them again, then go to no_float2
      13 - came ftom no_float2, check registers and go to no_float2
      15 - came from no_float2, fill regs and go back to no_float2
      17 - came from no_float2, check and block to no_float2
*/
void tc_1_float2_task( void )
{
    /* 02 */
    float2_id = _task_get_id();
    _task_block();

    /* 07 */    
    fpu_damage();
    _task_block();

    /* 09 */
    fpu_fill();
    _task_ready( _task_get_td( float1_id ) );

    /* 11 */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, fpu_check(), "Test #1 Testing: 1.3.2 float to float and back.");    
    fpu_fill();
    _task_block();

    /* 13 */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, fpu_check(), "Test #1 Testing: 1.4 float to nonfloat (with modify) and back.");    
    _task_block();

    /* 15 */
    fpu_fill();
    _task_block();

    /* 17 */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, fpu_check(), "Test #1 Testing: 1.5 float to nonfloat (without modify) and back.");
    _task_block();
}
void float2_task( uint32_t index )
{
    tc_1_float2_task();
}

/*
    Task Name    : no float 1 task
    Comments     :
      03 - came from float2, get id and to no_float2
      05 - came from no_float2, enable fpu, damage the registers and go back to no_float2
*/
void tc_1_no_float1_task( void )
{
    /* 03 */
    no_float1_id = _task_get_id();
    _task_block();

    /* 05 */    
    _task_enable_fp();
    fpu_damage();    
    _task_block();    
}
void no_float1_task( uint32_t index )
{
    tc_1_no_float1_task();
}

/* 
    Task Name    : no float 2 task
    Comments     :
      04 - came from no_float1, get id, enable fpu and fill it, then go to no_float1
      06 - check fpu, disable fpu, then enable it again fill it and go to float2 task
      08 - check fpu, disable it
      12 - enable fpu, damage it, go to float2 task
      14 - disable fpu, go to float2 task
      16 - consume some cpu cycles and go to float2 task
      18 - block, END of this cool test
*/
void tc_1_no_float2_task( void )
{  
    /* 04 */
    no_float2_id = _task_get_id();
    _task_enable_fp();
    fpu_fill();
    _task_ready( _task_get_td( no_float1_id ) );

    /* 06 */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, fpu_check(), "Test #1 Testing: 1.1 nonfloat to nonfloat and back.");
    _task_disable_fp();
    _task_enable_fp();
    fpu_fill();    
    _task_ready( _task_get_td( float2_id ) );

    /* 08 */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, fpu_check(), "Test #1 Testing: 1.2 nonfloat to float and back.");    
    _task_disable_fp();
    _task_ready( _task_get_td( float2_id ) );

    /* 12 */
    _task_enable_fp();
    fpu_damage();
    _task_ready( _task_get_td( float2_id ) );    

    /* 14 */
    _task_disable_fp();
    _task_ready( _task_get_td( float2_id ) );

    /* 16 */
    for(int i=0; i<100000; i++)
    {;;;;}
    _task_ready( _task_get_td( float2_id ) );

    /* 18 */
    _task_block();
}
void no_float2_task( uint32_t index )
{
    tc_1_no_float2_task();
}


/*
    Task Name    : tc_1_main_task
    Comments     : TEST #1: 
*/
void tc_1_main_task( void )
{
    //_task_set_priority(0, 7, &old_prio);
    //_task_stop_preemption();
    //_task_start_preemption();
}
void main_task( uint32_t parameter )
{   
   eunit_test_stop();
}
