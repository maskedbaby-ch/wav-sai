/**HEADER********************************************************************
*
* Copyright (c) 2011-2013 Freescale Semiconductor;
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
* $FileName: wav.h$
* $Version :
* $Date    :
*
* Comments:
*
*   This file contains the definitions of constants and structures
*   required for the WAVE file format that is a subset of Microsoft's
*   RIFF specification for the storage of multimedia files. .
*
*END************************************************************************/
#ifndef __wav_h_
#define __wav_h_
#include <mqx.h>

#define WAVE_HEADER_SIZE 44
#if PSP_ENDIAN
#define BYTESWAP16(x)      (uint16_t)((((x) & 0xFF00) >> 0x8) | (((x) & 0xFF) << 0x8))
#define BYTESWAP32(val)    (uint32_t)((BYTESWAP16((uint32_t)(val) & \
                           (uint32_t)0xFFFF) << 0x10) |  \
                           (BYTESWAP16((uint32_t)((val) >> 0x10))))
#else
#define BYTESWAP16(x)       (uint16_t)(x)
#define BYTESWAP32(x)       (uint32_t)(x)
#endif

typedef struct wave_chunk_descriptor
{
   /* ChunkID. Contains the letters "RIFF" in ASCII form
     (0x52494646 big-endian form).
   */
    char ChunkID[4];

   /* ChunkSize. 4 + (8 + SubChunk1Size) + (8 + SubChunk2Size)
      This is the size of the rest of the chunk
      following this number.  This is the size of the
      entire file in bytes minus 8 bytes for the
      two fields not included in this count:
      ChunkID and ChunkSize.
   */
   uint32_t ChunkSize;

   /* Format. Contains the letters "WAVE"
   (0x57415645 big-endian form).
   */
    char Format[4];

} WAVE_CHUNK_DESCRIPTOR, * WAVE_CHUNK_DESCRIPTOR_PTR;

typedef struct wave_fmt_subchunk
{
   /*
   Subchunk1ID. Contains the letters "fmt "
   (0x666d7420 big-endian form).
   */
    char Subchunk1ID[4];

   /*
   Subchunk1Size. 16 for PCM. This is the size of the
   rest of the Subchunk which follows this number.
   */
   uint32_t Subchunk1Size;

   /*
   AudioFormat. PCM = 1 (i.e. Linear quantization)
   Values other than 1 indicate some
   form of compression.
   */
   uint16_t AudioFormat;

   /*
   NumChannels. Mono = 1, Stereo = 2, etc.
   */
   uint16_t NumChannels;

   /*
   SampleRate. 8000, 44100, etc.
   */
   uint32_t SampleRate;

   /*
   ByteRate. SampleRate * NumChannels * BitsPerSample/8
   */
   uint32_t ByteRate;

   /*
   BlockAlign. NumChannels * BitsPerSample/8
   The number of bytes for one sample including
   all channels.
   */
   uint16_t BlockAlign;

   /*
   BitsPerSample. 8 bits = 8, 16 bits = 16, etc.
   */
   uint16_t BitsPerSample;

} WAVE_FMT_SUBCHUNK, * WAVE_FMT_SUBCHUNK_PTR;

typedef struct wave_data_subchunk
{
   /*
   Subchunk2ID. Contains the letters "data"
   (0x64617461 big-endian form).
   */
    char Subchunk2ID[4];

   /*
   Subchunk2Size. NumSamples * NumChannels * BitsPerSample/8
   This is the number of bytes in the data.
   You can also think of this as the size
   of the read of the subchunk following this number.
   */
   uint32_t Subchunk2Size;

} WAVE_DATA_SUBCHUNK, * WAVE_DATA_SUBCHUNK_PTR;

typedef struct wave_file_header
{
   /* The RIFF chunk descriptor*/
   WAVE_CHUNK_DESCRIPTOR CHUNK_DESCRIPTOR;

   /* Format subchunk */
   WAVE_FMT_SUBCHUNK FMT_SUBCHUNK;

   /* Audio data */
   WAVE_DATA_SUBCHUNK DATA_SUBCHUNK;

} WAVE_FILE_HEADER, * WAVE_FILE_HEADER_PTR;

#ifdef __cplusplus
extern "C" {
#endif

extern _mqx_int SetWaveHeader(
                                WAVE_FILE_HEADER_PTR header,
                                char *filename,
                                char *length,
                                uint32_t rate
                              );

#ifdef __cplusplus
}
#endif

#endif
