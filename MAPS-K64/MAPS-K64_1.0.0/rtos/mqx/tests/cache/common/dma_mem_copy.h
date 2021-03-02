/**HEADER**********************************************************************
*
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
*******************************************************************************
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
*******************************************************************************
*
* $FileName: dma_mem_copy.h$
* $Version : 3.8.2.0$
* $Date    : Oct-2-2012$
*
* Comments:
*
*
*
*END**************************************************************************/

#ifndef __dma_mem_copy_h__
#define __dma_mem_copy_h__

#define DMA_CHANNEL_NUMBER  15 /* Use number for default dma chanel from 0 to 31 */
#define DMA_SOURCE          58 /* Always enabled  DMA request source valid for K70 and vybrid */
/*Function definitios*/
void dma_mem_copy(void *src_ptr, void *dest_ptr, _mem_size num_units);
int dma_mem_copy_init(void);

#endif
