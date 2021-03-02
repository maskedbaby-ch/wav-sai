#ifndef __MEDIA_UNIT_TEST_HELPERS_H__
#define __MEDIA_UNIT_TEST_HELPERS_H__
///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Freescale Semiconductor, Inc. All rights reserved.
// 
// Freescale Semiconductor, Inc.
// Proprietary & Confidential
// 
// This source code and the algorithms implemented therein constitute
// confidential information and may comprise trade secrets of Freescale Semiconductor, Inc.
// or its associates, and any use thereof is subject to the terms and
// conditions of the Confidential Disclosure Agreement pursual to which this
// source code was originally received.
///////////////////////////////////////////////////////////////////////////////

#include "wl_common.h"
#include <string.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <arm_ghs.h>
//#include "components/telemetry/tss_logtext.h"
//#include "os/pmi/os_pmi_api.h"
//#include "os/dmi/os_dmi_api.h"
//#include "hw/core/vmemory.h"
//#include "os/threadx/tx_api.h"
#include "ddi_media_errordefs.h"
#include "sectordef.h"
#include "drive_tags.h"
#include "ddi_media.h"
#include "ddi_media_internal.h"
#include "media_buffer.h"
//#include "drivers/media/include/ddi_media_timers.h"
//#include "auto_free.h"
//#include "rtos/mqx/simple_timer.h"

//extern "C" {
////#include "hw/lradc/hw_lradc.h"
////#include "drivers/rtc/ddi_rtc.h"
////#include "drivers/clocks/ddi_clocks.h"
//}


////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

#define EXTRAS_STATIC_SECTOR_BUFFERS 2
#define EXTRAS_STATIC_AUX_BUFFERS 2

#define EXAMPLE_TEST_TASK_PRIORITY          9
#define EXAMPLE_TEST_TASK_STACK_SIZE        4000

// TODO: extern TX_THREAD g_example_test_thread
//extern TX_THREAD g_example_test_thread;
extern uint32_t g_u32TestStack[EXAMPLE_TEST_TASK_STACK_SIZE/4];

const unsigned kMaxBufferBytes = 9*1024;
const unsigned kMaxBufferWords = kMaxBufferBytes / sizeof(uint32_t);

extern uint32_t g_actualBufferBytes;
extern uint32_t g_actualFirmwarePageSize;

//! \name Buffers
//@{
#if defined(__ghs__)
#pragma alignvar(32)
#endif 
#if defined (__CODEWARRIOR__)
// TODO: #pragma alignvar(32)
#endif 
extern SECTOR_BUFFER s_dataBuffer[CACHED_BUFFER_SIZE_IN_WORDS(kMaxBufferBytes)];

#if defined(__ghs__)
#pragma alignvar(32)
#endif 
#if defined (__CODEWARRIOR__)
// TODO: #pragma alignvar(32)
#endif 

extern SECTOR_BUFFER s_readBuffer[CACHED_BUFFER_SIZE_IN_WORDS(kMaxBufferBytes)];

#if defined(__ghs__)
#pragma alignvar(32)
#endif 
#if defined (__CODEWARRIOR__)
// TODO: #pragma alignvar(32)
#endif 

extern SECTOR_BUFFER g_aux_buffer[SECTOR_BUFFER_ALLOC_SIZE(REDUNDANT_AREA_BUFFER_ALLOCATION)];

#if defined(__ghs__)
#pragma alignvar(32)
#endif 
#if defined (__CODEWARRIOR__)
// TODO: #pragma alignvar(32)
#endif 

extern SECTOR_BUFFER g_read_aux_buffer[SECTOR_BUFFER_ALLOC_SIZE(REDUNDANT_AREA_BUFFER_ALLOCATION)];
//@}

extern bool g_enableFastPrint;
extern char s_printfBuffer[1024];

//#define FASTPRINT(...)  if (g_enableFastPrint) \
//                        { \
//                            snprintf(s_printfBuffer, sizeof(s_printfBuffer), __VA_ARGS__); \
//                            printf(s_printfBuffer); \
//                            tss_logtext_Flush(TX_WAIT_FOREVER); \
//                        }
#define FASTPRINT(...)  if (g_enableFastPrint) \
						{ \
							printf(s_printfBuffer); \
						}

extern const MediaDefinition_t g_mediaDefinition[];

extern RtStatus_t g_nand_hal_insertReadError;


////////////////////////////////////////////////////////////////////////////////
// Prototypes
////////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif
	
RtStatus_t SDKInitialization();
void basic_os_entry(void *threadx_avail_mem);
RtStatus_t test_main(ULONG param);

uint64_t get_kb_s(uint64_t bytes, uint64_t elapsedTime);
uint64_t get_mb_s(uint64_t bytes, uint64_t elapsedTime);
char * bytes_to_pretty_string(uint64_t bytes);
char * microseconds_to_pretty_string(uint64_t elapsedTime);

void fill_data_buffer(SECTOR_BUFFER * buffer, uint32_t sectorNumber, void * p);
void clear_buffer(SECTOR_BUFFER * buffer);
void clear_aux();
void fill_aux(SECTOR_BUFFER * buffer, uint32_t sectorNumber);

bool compare_buffers(const void * a, const void * b, uint32_t count);
unsigned count_buffer_mismatches(const void * a, const void * b, uint32_t count);

//! \brief Returns a bool with a given chance of being true.
//! \param chance Value from 0-10000, where 1000 means a 10.00% chance.
bool random_percent(int chance);

//! \brief Returns a random integer from 0 to the specified maximum.
int random_range(int max);

RtStatus_t tss_Flush();

#ifdef __cplusplus
}
#endif

#endif

/* EOF */
