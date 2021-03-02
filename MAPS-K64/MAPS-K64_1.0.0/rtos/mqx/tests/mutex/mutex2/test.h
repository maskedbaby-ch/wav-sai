#ifndef __test_h__
#define __test_h__
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
* $Version : 3.8.2.0$
* $Date    : Sep-6-2011$
*
* Comments:
*
*   This include file for the mutex2 test program
*
*END************************************************************************/

#define MAIN_TASK          (10)
#define MUTEX_TASK         (11)

#define MAIN_PRIO   (7)

#define NUMBER_OF_MUTEX_TASKS   (4)
#define MUTEX_PRIO              (MAIN_PRIO+NUMBER_OF_MUTEX_TASKS+1)

#define TEST_SPIN_LIMIT        (100)

extern MUTEX_STRUCT          mutex1;
extern _task_id              global_mutex_ids[NUMBER_OF_MUTEX_TASKS];
extern volatile unsigned char        global_mutex_seq_array[NUMBER_OF_MUTEX_TASKS];
extern volatile _mqx_uint    global_mutex_seq_index;
extern MUTEX_ATTR_STRUCT     mutex_attr;

extern void    main_task(uint32_t);
extern void    mutex_task(uint32_t);
extern _mqx_uint test_get_task_current_priority(_task_id,_mqx_uint_ptr);
extern void    test_check_mutex_task_priorities(void);
extern void    test_mutex_count( _mqx_uint );
extern void    test_mutex_sched_seq(MUTEX_ATTR_STRUCT_PTR,_mqx_uint,_mqx_uint,
   _mqx_uint);
extern void    test_print_array(unsigned char *,_mqx_uint);
extern void    test_print_mutex_prios(void);

#endif
/* EOF */
