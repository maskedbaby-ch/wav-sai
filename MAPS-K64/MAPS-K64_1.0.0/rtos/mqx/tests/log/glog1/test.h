/**HEADER********************************************************************
*
* Copyright (c) 2008-2013 Freescale Semiconductor;
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
* $Version : 4.0.1.1$
* $Date    : May-29-2013$
*
* Comments:
*
*   This file contains definitions, extern and structs for testing.
*
*END************************************************************************/

#ifndef _log_test_h
#define _log_test_h
/************************************************************************
 * Macro definitions for both log modules
 ************************************************************************/
#define MAIN_TASK           (10)
#define TEST_TASK           (11)
#define MAX_STRING_LENGTH   (20)
#define MAIN_PRIORITY       (9)
#define VALID_LOG_NUMBER    (10)
#define LOG_ENTRY_NUMBER    (10)
#define LOG_PARAM_NUMBER    (2)


#if TEST_LWLOGS
#define LOG_ENTRY_SIZE      (sizeof(LWLOG_ENTRY_STRUCT) / sizeof(_mqx_uint))
#else
#define LOG_ENTRY_SIZE      (sizeof(LOG_ENTRY_STRUCT) / sizeof(_mqx_uint)+LOG_PARAM_NUMBER)
#endif

/************************************************************************
 * Global variables and functions for both log modules
 ************************************************************************/

extern void main_task(uint32_t);
extern void _glog_remove_component(void);
extern _mqx_uint _get_valid_component(void);
extern void _set_valid_component(_mqx_uint);
extern bool _glog_test_entry(_mqx_uint,void*,_mqx_uint,_mqx_uint);
extern void _glog_clear(_mqx_uint);

/************************************************************************
 *  General functions for (lw)log
 *  *********************************************************************/
#if TEST_LWLOGS
#define _glog_create_component()    _lwlog_create_component()
#define _glog_create(A,B,C)         _lwlog_create(A,B,C)
#define _glog_destroy(A)            _lwlog_destroy(A)
#define _glog_disable(A)            _lwlog_disable(A)
#define _glog_write(A,B,C)          _lwlog_write(A,B,C,0,0,0,0,0)
#define _glog_test(A)               _lwlog_test(A)
#define _glog_read(A,B,C)           _lwlog_read(A,B,&C)
#define _glog_read_1(A,B,C)         _lwlog_read(A,B,C)
#define _glog_reset(A)              _lwlog_reset(A)
#define _glog_enable(A)             _lwlog_enable(A)
#else
#define _glog_create_component()    _log_create_component()
#define _glog_create(A,B,C)         _log_create(A,B*LOG_ENTRY_SIZE,C)
#define _glog_destroy(A)            _log_destroy(A)
#define _glog_disable(A)            _log_disable(A)
#define _glog_write(A,B,C)          _log_write(A,2,B,C)
#define _glog_test(A)               _log_test(A)
#define _glog_read(A,B,C)           _log_read(A,B,LOG_PARAM_NUMBER,(LOG_ENTRY_STRUCT_PTR)&C)
#define _glog_read_1(A,B,C)         _log_read(A,B,LOG_PARAM_NUMBER,C)
#define _glog_reset(A)              _log_reset(A)
#define _glog_enable(A)             _log_enable(A)
#endif

#endif
