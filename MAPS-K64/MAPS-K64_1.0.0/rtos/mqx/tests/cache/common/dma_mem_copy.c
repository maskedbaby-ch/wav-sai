/**HEADER********************************************************************
*
* Copyright (c) 2013 Freescale Semiconductor;
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
* $FileName: dma_mem_copy.c$
* $Version : 3.8.3.0$
* $Date    : Oct-2-2012$
*
* Comments:
*
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <lwsem.h>
#include <dma.h>
#include "dma_mem_copy.h"

LWSEM_STRUCT job_done;
DMA_CHANNEL_HANDLE dch;    // handle is just a pointer

/*FUNCTION****************************************************************
*
* Function Name    : dma_callback
* Returned Value   : callback routine
* Comments         :
*   Notifies task about transfer completion.
*
*END*********************************************************************/
static void dma_callback(void *callback_data, int tcds_done, uint32_t tcd_seq)
{
   _lwsem_post(&job_done);
   return;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : dma_mem_copy_init
* Returned Value   : 0 if succeed.
*                    error otherwise.
* Comments         :
*   This function initializes DMA.
*
*END*----------------------------------------------------------------------*/
int dma_mem_copy_init(void)
{
    int result;
    _lwsem_create(&job_done, 0);

    /* Claim channel no DMA_CHANNEL_NUMBER */
    result = dma_channel_claim(&dch, DMA_CHANNEL_NUMBER);
    if (result)
    {
        return result;
    }
    /* Allocate slots for 1 tcd (tcd fifo with 1 item) and no special flags */
    result = dma_channel_setup(dch, 1, 0);
    if (result)
    {
        dma_channel_release(dch);
        return result;
    }
    /*Use data request source DMA_SOURCE.*/
    result = dma_request_source(dch, DMA_SOURCE);
    if (result)
    {
        dma_channel_release(dch);
        return result;
    }
    /* Register callback for dma transfer */
    dma_callback_reg(dch, (DMA_EOT_CALLBACK)dma_callback, NULL);
    /* Data request is enabled in hardware after actual transfer is submitted */
    dma_request_enable(dch);

    return MQX_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : dma_mem_copy
* Returned Value   : none
* Comments         :
*   This function start DMA transfer
*
*END*----------------------------------------------------------------------*/
void dma_mem_copy(void *src_ptr, void *dest_ptr, _mem_size num_units)
{
    DMA_TCD     tcd;
    uint32_t    tcd_seq;

    /* Prepare memory to memory transfer */
    dma_tcd_memcpy(&tcd, src_ptr, dest_ptr, num_units);
    /* Submit it, the transfer is waiting as data request is not enabled */
    dma_transfer_submit(dch, &tcd, &tcd_seq);
    /* block the task until completion of the background operation */
    _lwsem_wait(&job_done);

    // check dma_channel_status if you want to know success
}

