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
*END************************************************************************/

#ifndef __test_h__
#define __test_h__

#include <bsp.h>


#define MAIN_TASK       10
#define TEST_TASK       11

#define BYTE_SENT       128

#ifdef BSPCFG_ENABLE_SAI
#define I2S_DEVICE     "sai:"
#else
#define I2S_DEVICE     "ii2s0:"
#endif

#define FREQ_TOLERANCE      1000
#define DATA_INIT_OFFSET    20

#define AUDIO_SAMPLE_RATE       (44100)

#define I2S_FS_FREQ_TEST        AUDIO_SAMPLE_RATE

#define I2S_BCLK_FREQ_TEST      (AUDIO_SAMPLE_RATE * 64)

#ifdef BSPCFG_ENABLE_SAI
#define I2S_MCLK_FREQ_TEST      (AUDIO_SAMPLE_RATE * 384)
#else
#define I2S_MCLK_FREQ_TEST      (AUDIO_SAMPLE_RATE * 256)
#endif

#define FIFO_WATERMARK_TEST      1

#ifdef BSPCFG_ENABLE_SAI
#define TEST_DUMMY_MODE         I2S_USE_FLOAT
#else
#define TEST_DUMMY_MODE         1
#endif

#ifdef BSPCFG_ENABLE_SAI
#define MASTER_MODE     (I2S_TX_MASTER | I2S_RX_MASTER)
#define SLAVE_MODE      (I2S_TX_SLAVE | I2S_RX_SLAVE)
#else
#define MASTER_MODE     I2S_MODE_MASTER
#define SLAVE_MODE      I2S_MODE_SLAVE
#endif

#define INVALID_MODE        (SLAVE_MODE + 1)
#define INVALID_CHANNEL     0xff
#define INVALID_DATA_BITS   (AUDIO_BIT_SIZE_MAX + 1)

#define MIN(a, b)       ((a) < (b) ? (a) : (b))
#define ABS_DIFF(a, b)  ((a) < (b) ? ((b) - (a)) : ((a) - (b)))

typedef struct general_i2s_init_struct
{
    /* Selected SAI HW module */
    uint8_t HW_CHANNEL;

#ifdef BSPCFG_ENABLE_SAI
    /* The SAI TX channel to initialize */
    uint8_t TX_CHANNEL;

    /* The SAI RX channel to initialize */
    uint8_t RX_CHANNEL;

    /* Clock setup: sync-async; bitclock: normal-swapped */
    uint8_t CLOCK_MODE;
#endif
    /* Default operating mode */
    uint8_t MODE;

    /* Number of valid data bits*/
    uint8_t DATA_BITS;

    /* I2S master clock source*/
    uint8_t CLOCK_SOURCE;

#ifndef BSPCFG_ENABLE_SAI
    /* Data are stereo */
    bool STEREO;
#endif

    /* Transmit dummy data */
    bool TX_DUMMY;

    /* Interrupt level to use */
#ifdef BSPCFG_ENABLE_SAI
    _mqx_uint LEVEL;
#else
    uint8_t   LEVEL;
#endif

    /* Tx buffer size */
    uint32_t BUFFER_SIZE;

#ifdef BSPCFG_ENABLE_SAI
    /* Internal master clock source */
    CM_CLOCK_SOURCE MCLK_SRC;
#endif

    /* I/O data format */
    AUDIO_DATA_FORMAT const * IO_FORMAT;

} GENERAL_I2S_INIT_STRUCT, * GENERAL_I2S_INIT_STRUCT_PTR;

AUDIO_DATA_FORMAT const audio_data_invalid = {
                AUDIO_LITTLE_ENDIAN,    /* Endian of input data */
                AUDIO_ALIGNMENT_LEFT,   /* Alignment of input data */
                1,                      /* Bit size of input data */
                1,                      /* Sample size in bytes */
                1                       /* Number of channels */
    };

#endif /*__test_h_ */
