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
* $FileName: wav.c$
* $Version :
* $Date    :
*
* Comments:
*
*   This file contains code for operations with WAVE files
*
*END************************************************************************/
#include <mqx.h>
#include <fio.h>
#include "wav.h"
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "audio_microphone.h"
/***************************************
**
** Global variables
****************************************/
extern USB_AUDIO_STREAM_DESC_FORMAT_TYPE_PTR   frm_type_desc;

/*FUNCTION****************************************************************
*
* Function Name    : SetWaveHeader
* Returned Value   : Zero if everything is ok, non zero otherwise
* Comments         :
*    This function creates header according to format and writes it into
*    output file.
*    Returns 0 if everything passed, any other value = IO error
*
*END*********************************************************************/

_mqx_int SetWaveHeader
   (
    /* [OUT] header pointer to save header */
    WAVE_FILE_HEADER_PTR header,

    /* [IN] file for writing */
    char *filename,
    /* length in seconds*/
    char *length,
    uint32_t rate
   )
{
   uint32_t tmp32 = 0;
   uint16_t tmp16 = 0;
   uint32_t vlength;
   FILE_PTR file_ptr;
   /* chunk descriptor */
   WAVE_CHUNK_DESCRIPTOR_PTR ch_des = &(header->CHUNK_DESCRIPTOR);
   /* sub chunk */
   WAVE_FMT_SUBCHUNK_PTR fmt_sch = &(header->FMT_SUBCHUNK);
   /* data chunk */
   WAVE_DATA_SUBCHUNK_PTR data_sch = &(header->DATA_SUBCHUNK);
   /* open file to write */
   file_ptr = fopen(filename, "wb");
   if (NULL == file_ptr)
   {
      return(-1);
   }
   /* Fill header */
   strncpy(ch_des->ChunkID, "RIFF", sizeof(ch_des->ChunkID));
   strncpy(ch_des->Format, "WAVE", sizeof(ch_des->Format));
   strncpy(fmt_sch->Subchunk1ID, "fmt ", sizeof(fmt_sch->Subchunk1ID));
   /* Sub chunk1 Size */
   fmt_sch->Subchunk1Size = 16;
   fmt_sch->AudioFormat = 1; /* PCM */
   /* Sample rate */
   fmt_sch->SampleRate = rate;
   /* Bits per sample */
   fmt_sch->BitsPerSample = frm_type_desc->bBitResolution;
   /* Number of Channels */
   fmt_sch->NumChannels = frm_type_desc->bNrChannels;
   fmt_sch->ByteRate = fmt_sch->SampleRate * fmt_sch->NumChannels *\
                       fmt_sch->BitsPerSample/8;
   /* Block Align*/
   fmt_sch->BlockAlign = fmt_sch->NumChannels * fmt_sch->BitsPerSample/8;
   strncpy(data_sch->Subchunk2ID, "data", sizeof(data_sch->Subchunk2ID));
   /* Sub chunk2 Size*/
   vlength = atoi(length);
   data_sch->Subchunk2Size = vlength * fmt_sch->SampleRate * \
                             fmt_sch->NumChannels * fmt_sch->BitsPerSample/8;
   ch_des->ChunkSize = 4 + (8 + fmt_sch->Subchunk1Size) + (8 + data_sch->Subchunk2Size);

   /* Write header to file */
   /* Chunk descriptor */
   if (sizeof(ch_des->ChunkID) != fwrite(&ch_des->ChunkID, 1, sizeof(ch_des->ChunkID), file_ptr))
      goto write_error;
   tmp32 = HOST_TO_LE_LONG(ch_des->ChunkSize);
   if (sizeof(tmp32) != fwrite(&tmp32, 1, sizeof(tmp32), file_ptr))
      goto write_error;
   if (sizeof(ch_des->Format) != fwrite(&ch_des->Format, 1, sizeof(ch_des->Format), file_ptr))
       goto write_error;
   /* write format sub chunk */
   if (sizeof(fmt_sch->Subchunk1ID) != fwrite(&fmt_sch->Subchunk1ID, 1, sizeof(fmt_sch->Subchunk1ID), file_ptr))
       goto write_error;
   /* write Sub chunk1 Size*/
   tmp32 = HOST_TO_LE_LONG(fmt_sch->Subchunk1Size);
   if (sizeof(tmp32) != fwrite(&tmp32, 1, sizeof(tmp32), file_ptr))
       goto write_error;
   /* write Audio Format*/
   tmp16 = HOST_TO_LE_SHORT(fmt_sch->AudioFormat);
   if (sizeof(tmp16) != fwrite(&tmp16, 1, sizeof(tmp16), file_ptr))
       goto write_error;
   /* write Num Channels*/
   tmp16 = HOST_TO_LE_SHORT(fmt_sch->NumChannels);
   if (sizeof(tmp16) != fwrite(&tmp16, 1, sizeof(tmp16), file_ptr))
       goto write_error;
   /* write Sample Rate*/
   tmp32 = HOST_TO_LE_LONG(fmt_sch->SampleRate);
   if (sizeof(tmp32) != fwrite(&tmp32, 1, sizeof(tmp32), file_ptr))
       goto write_error;
   /* write Byte Rate*/
   tmp32 = HOST_TO_LE_LONG(fmt_sch->ByteRate);
   if (sizeof(tmp32) != fwrite(&tmp32, 1, sizeof(tmp32), file_ptr))
       goto write_error;
   /* write Block Align*/
   tmp16 = HOST_TO_LE_SHORT(fmt_sch->BlockAlign);
   if (sizeof(tmp16) != fwrite(&tmp16, 1, sizeof(tmp16), file_ptr))
       goto write_error;
   /* write Bits Per Sample*/
   tmp16 = HOST_TO_LE_SHORT(fmt_sch->BitsPerSample);
   if (sizeof(tmp16) != fwrite(&tmp16, 1, sizeof(tmp16), file_ptr))
       goto write_error;
   /* data sub chunk */
   if (sizeof(data_sch->Subchunk2ID) != fwrite(&data_sch->Subchunk2ID, 1, sizeof(data_sch->Subchunk2ID), file_ptr))
       goto write_error;
   /* Sub chunk2 Size*/
   tmp32 = HOST_TO_LE_LONG(data_sch->Subchunk2Size);
   if (sizeof(tmp32) != fwrite(&tmp32, 1, sizeof(tmp32), file_ptr))
       goto write_error;

   fclose(file_ptr);
   return(0);

write_error:
   fclose(file_ptr);
   return -1;
}

/* EOF */
