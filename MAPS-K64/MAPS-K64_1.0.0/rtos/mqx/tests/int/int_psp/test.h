#ifndef __test_h__
#define __test_h__ 1
/**HEADER*******************************************************************
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
* $Version : 3.7.2.0$
* $Date    : Feb-2-2011$
*
* Comments:
*
*   This file defines the constants for the test
*
*END************************************************************************/

#define MAIN_TASK     10
#define MSGHI_TASK    11
#define MSGLO_TASK    12
#define EVENTHI_TASK  13
#define EVENTLO_TASK  14
#define SEMHI_TASK    15
#define SEMLO_TASK    16
#define LWSEMHI_TASK  17
#define LWSEMLO_TASK  18
#define TASKQHI_TASK  19
#define TASKQLO_TASK  20
#define TIMERHI_TASK  21
#define TIMERLO_TASK  22
#define KISRHI_TASK   23
#define KISRLO_TASK   24

#define MAIN_PRIO      9
#define MSGHI_PRIO     8
#define MSGLO_PRIO     9
#define EVENTHI_PRIO   8
#define EVENTLO_PRIO   9
#define SEMHI_PRIO     8
#define SEMLO_PRIO     9
#define LWSEMHI_PRIO   8
#define LWSEMLO_PRIO   9
#define TASKQHI_PRIO   8
#define TASKQLO_PRIO   9
#define TIMERHI_PRIO   8
#define TIMERLO_PRIO   9
#define KISRHI_PRIO    0
#define KISRLO_PRIO    9

extern void main_task(uint32_t parameter);
extern void msg_higher_task(uint32_t parameter);
extern void msg_lower_task(uint32_t parameter);
extern void event_higher_task(uint32_t parameter);
extern void event_lower_task(uint32_t parameter);
extern void sem_higher_task(uint32_t parameter);
extern void sem_lower_task(uint32_t parameter);
extern void lwsem_higher_task(uint32_t parameter);
extern void lwsem_lower_task(uint32_t parameter);
extern void taskq_higher_task(uint32_t parameter);
extern void taskq_lower_task(uint32_t parameter);
extern void timer_higher_task(uint32_t parameter);
extern void timer_lower_task(uint32_t parameter);

void kisr_higher_task(uint32_t parameter);
void kisr_lower_task(uint32_t parameter);

#endif
/* EOF */
