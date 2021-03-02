#include "testing_main_internal.h"
#include "Ddi_media_internal.h"
#include "Ddi_nand_hal_internal.h"
#include "Ddi_nand_ddi.h"
#include "mapper.h"
#include "zone_map_cache.h"
#include "persistent_phy_map.h"
#include "nandhal_spy.h"

uint32_t i_ddi_nand_hal_spy_NandAnalysis[DDI_NAND_HAL_SPY_NAND_ANALYSIS_INDEX];

bool ddi_nand_hal_spy_bIsLinked = TRUE;
ddi_nand_hal_spy_ReadsPerPage_t             n_ddi_nand_hal_spy_ReadsPerPage[DDI_NAND_HAL_SPY_MAX_PAGES];
ddi_nand_hal_spy_PageWriteIndexPerBlock_t   n_ddi_nand_hal_spy_PageWriteIndexPerBlock[DDI_NAND_HAL_SPY_MAX_BLOCKS];
ddi_nand_hal_spy_ErasuresPerBlock_t         n_ddi_nand_hal_spy_ErasuresPerBlock[DDI_NAND_HAL_SPY_MAX_BLOCKS];
ddi_nand_hal_spy_ErasuresPerBlock_t         n_ddi_nand_hal_spy_ErasuresPerBlockMaxQty;
uint32_t                                    i_ddi_nand_hal_spy_ErasuresPerBlockMax;
ddi_nand_hal_spy_TimeAnalysis_t ddi_nand_hal_spy_readTime, ddi_nand_hal_spy_writeTime, ddi_nand_hal_spy_eraseTime;

uint32_t    n_ddi_nand_hal_spy_ReadWarningThreshold = DDI_NAND_HAL_SPY_DEFAULT_READ_WARNING_THRESHOLD;
uint32_t    n_ddi_nand_hal_spy_EraseWarningThreshold = DDI_NAND_HAL_SPY_DEFAULT_ERASE_WARNING_THRESHOLD;

extern NandHalContext_t g_nandHalContext;
extern LdlInfo g_ldlInfo;

NandHalSpy::NandHalSpy() {
   m_originalNandHal = NULL;
}

NandHalSpy::~NandHalSpy() {
   m_originalNandHal = NULL;
}

void NandHalSpy::init(NandPhysicalMedia * original) {
   setOriginalNandPhysicalMedia(original);
} 

void NandHalSpy::setOriginalNandPhysicalMedia( NandPhysicalMedia * original) {
   
   /*
   this->pNANDParams = original->pNANDParams;
   this->wChipNumber= original->wChipNumber; //!< The number of the chip select to which this structure applies.
   this->totalPages= original->totalPages; //!< The number of pages in this chip.
   this->wTotalBlocks= original->wTotalBlocks; //!< The number of blocks in this chip.
   this->wTotalInternalDice= original->wTotalInternalDice; //!< The number of die in this chip.
   this->wBlocksPerDie= original->wBlocksPerDie; //!< The number of blocks in a die.
   this->m_firstAbsoluteBlock= original->m_firstAbsoluteBlock; //!< First absolute block of this chip.
   this->m_firstAbsolutePage= original->m_firstAbsolutePage; //!< First absolute page of this chip.
   */
   
   NandPhysicalMedia * tmp = static_cast<NandPhysicalMedia * >(this);
   *tmp = * original;
   
   //*((NandPhysicalMedia *)this)) = (*original) ;
   m_originalNandHal = original;
   
   g_nandHalContext.nands[wChipNumber] = this;
} 

NandPhysicalMedia *NandHalSpy::getOriginalNandPhysicalMedia() {
   return   m_originalNandHal;
} 


RtStatus_t NandHalSpy::readID(uint8_t * pReadIDCode) {
   return m_originalNandHal->readID(pReadIDCode);
}

RtStatus_t NandHalSpy::readRawData(uint32_t wSectorNum, uint32_t columnOffset,
uint32_t readByteCount, SECTOR_BUFFER * pBuf) {
   /********************************************************************
      Gather statistics.
      We count a read even without checking the status of the
      NAND operation, because even an attempt may imply stress
      on the NAND.
   ********************************************************************/
   ddi_nand_hal_spy_CountPageRead( m_originalNandHal, wSectorNum );

   /********************************************************************
      Do the operaton.
   ********************************************************************/

   return m_originalNandHal->readRawData(wSectorNum, columnOffset, readByteCount, pBuf);
}

RtStatus_t NandHalSpy::readPage(uint32_t uSectorNumber, SECTOR_BUFFER * pBuffer,
SECTOR_BUFFER * pAuxiliary) {
   /********************************************************************
      Gather statistics.
      We count a read even without checking the status of the
      NAND operation, because even an attempt may imply stress
      on the NAND.
   ********************************************************************/
   ddi_nand_hal_spy_CountPageRead( m_originalNandHal, uSectorNumber );

   /********************************************************************
      Do the operaton.
   ********************************************************************/
    
   return m_originalNandHal->readPage(uSectorNumber, pBuffer, pAuxiliary);
}

RtStatus_t NandHalSpy::readMetadata(uint32_t uSectorNumber, SECTOR_BUFFER * pBuffer) {
   /********************************************************************
      Gather statistics.
      We count a read even without checking the status of the
      NAND operation, because even an attempt may imply stress
      on the NAND.
   ********************************************************************/
   ddi_nand_hal_spy_CountPageRead( m_originalNandHal, uSectorNumber );

   /********************************************************************
      Do the operaton.
   ********************************************************************/

   return m_originalNandHal->readMetadata(uSectorNumber, pBuffer);
}

RtStatus_t NandHalSpy::writeRawData(uint32_t pageNumber, uint32_t columnOffset,
uint32_t writeByteCount, const SECTOR_BUFFER * data) {
   /********************************************************************
      Gather statistics.
      We count a write even without checking the status of the
      NAND operation, because even an attempt may imply stress
      on the NAND.
   ********************************************************************/
   ddi_nand_hal_spy_CountPageWrite(m_originalNandHal, pageNumber);

   /********************************************************************
      Do the operaton.
   ********************************************************************/   

   return m_originalNandHal->writeRawData(pageNumber, columnOffset, writeByteCount, data);
}

RtStatus_t NandHalSpy::writePage(uint32_t uSectorNum, const SECTOR_BUFFER * pBuffer, const SECTOR_BUFFER * pAuxiliary) {
   /********************************************************************
      Gather statistics.
      We count a write even without checking the status of the
      NAND operation, because even an attempt may imply stress
      on the NAND.
   ********************************************************************/
   ddi_nand_hal_spy_CountPageWrite(m_originalNandHal, uSectorNum);

   /********************************************************************
      Do the operaton.
   ********************************************************************/   
   
   return m_originalNandHal->writePage(uSectorNum, pBuffer, pAuxiliary);
}

RtStatus_t NandHalSpy::writeFirmwarePage(uint32_t uSectorNum,
const SECTOR_BUFFER * pBuffer, const SECTOR_BUFFER * pAuxiliary) {
   /********************************************************************
      Gather statistics.
      We count a write even without checking the status of the
      NAND operation, because even an attempt may imply stress
      on the NAND.
   ********************************************************************/
   ddi_nand_hal_spy_CountPageWrite(m_originalNandHal, uSectorNum);

   /********************************************************************
      Do the operaton.
   ********************************************************************/   

   return m_originalNandHal->writeFirmwarePage(uSectorNum, pBuffer, pAuxiliary);
}

RtStatus_t NandHalSpy::readFirmwarePage(uint32_t uSectorNumber,
SECTOR_BUFFER * pBuffer, SECTOR_BUFFER * pAuxiliary) {
   /********************************************************************
      Gather statistics.
      We count a read even without checking the status of the
      NAND operation, because even an attempt may imply stress
      on the NAND.
   ********************************************************************/
   ddi_nand_hal_spy_CountPageRead( m_originalNandHal, uSectorNumber );

   /********************************************************************
      Do the operaton.
   ********************************************************************/

   return m_originalNandHal->readFirmwarePage(uSectorNumber, pBuffer, pAuxiliary);
}

RtStatus_t NandHalSpy::eraseBlock(uint32_t uBlockNumber) {
   /********************************************************************
      Gather statistics.
      We count an erase even without checking the status of the
      NAND operation, because even an attempt may imply stress
      on the NAND.
   ********************************************************************/
   ddi_nand_hal_spy_CountBlockErase( m_originalNandHal, uBlockNumber );

   /********************************************************************
      Do the operaton.
   ********************************************************************/

   return m_originalNandHal->eraseBlock(uBlockNumber);
}

RtStatus_t NandHalSpy::eraseMultipleBlocks(uint32_t startBlockNumber,
uint32_t requestedBlockCount, uint32_t * actualBlockCount) {
   uint32_t i;
   uint32_t localActualCount;
   
   /********************************************************************
      Do the operaton.
   ********************************************************************/

   RtStatus_t status = m_originalNandHal->eraseMultipleBlocks(startBlockNumber, requestedBlockCount, &localActualCount);
   
   if (actualBlockCount)
   {
      *actualBlockCount = localActualCount;
   }
   
   /********************************************************************
      Gather statistics.
      We count an erase even without checking the status of the
      NAND operation, because even an attempt may imply stress
      on the NAND.
   ********************************************************************/
    
   for (i = 0; i < localActualCount; ++i) {
      ddi_nand_hal_spy_CountBlockErase( m_originalNandHal, startBlockNumber + i );
   }
   
   return status;
}

RtStatus_t NandHalSpy::copyPages(NandPhysicalMedia * targetNand,
uint32_t wSourceStartSectorNum, uint32_t wTargetStartSectorNum,
uint32_t wNumSectors, SECTOR_BUFFER * sectorBuffer,
SECTOR_BUFFER * auxBuffer, NandCopyPagesFilter * filter,
uint32_t * successfulPages) {
   uint32_t iSector;

   /********************************************************************
      Gather statistics.
      We count a read and write even without checking the status of the
      NAND operation, because even an attempt may imply stress
      on the NAND.
   ********************************************************************/
   for ( iSector=0 ; iSector<wNumSectors ; iSector++)
   {
      ddi_nand_hal_spy_CountPageRead  ( m_originalNandHal, wSourceStartSectorNum + iSector );
      ddi_nand_hal_spy_CountPageWrite ( targetNand, wTargetStartSectorNum + iSector );
   }

   /********************************************************************
      Do the operaton.
   ********************************************************************/ 
   if (targetNand == this) {
      return m_originalNandHal->copyPages(m_originalNandHal, wSourceStartSectorNum, wTargetStartSectorNum, wNumSectors, sectorBuffer, auxBuffer, filter, successfulPages);
   } else {
      return m_originalNandHal->copyPages(targetNand, wSourceStartSectorNum, wTargetStartSectorNum, wNumSectors, sectorBuffer, auxBuffer, filter, successfulPages);
   }
}

bool NandHalSpy::isBlockBad(uint32_t blockAddress, SECTOR_BUFFER * auxBuffer, 
bool checkFactoryMarkings, RtStatus_t * readStatus) {
   return m_originalNandHal->isBlockBad(blockAddress, auxBuffer, checkFactoryMarkings, readStatus);
}

RtStatus_t NandHalSpy::markBlockBad(uint32_t blockAddress, SECTOR_BUFFER * pageBuffer,
SECTOR_BUFFER * auxBuffer) {
   return m_originalNandHal->markBlockBad(blockAddress, pageBuffer, auxBuffer);
}

void NandHalSpy::override()
{
   g_nandHalContext.nands[wChipNumber] = this;
}

void NandHalSpy::restore()
{
   g_nandHalContext.nands[wChipNumber] = m_originalNandHal;
}

RtStatus_t ddi_nand_hal_spy_Init( NandPhysicalMedia * pNANDDescriptor,
ddi_nand_hal_spy_ReadsPerPage_t     nReadWarningThreshold,
ddi_nand_hal_spy_ErasuresPerBlock_t nEraseWarningThreshold )
{
   RtStatus_t  RetVal;
   uint32_t    iPage   = 0;
   uint32_t    iBlock  = 0;
   int         iNand   = 0;

   //  This init function gets called once for each chip-enable.
   //  The total quantity of blocks (and by implication, pages) that
   //  nand_hal_spy tracks is the lesser of the size of the
   //  spy memory, or the size of all NANDs present.

   // Remember the highest chip number we've seen.
   stc_MaxQuantityChips = 1 + (uint32_t)MAX(stc_MaxQuantityChips, pNANDDescriptor->wChipNumber);

   // Compute the quantities of pages and blocks of a system with that quantity of chips...
   stc_MaxPages    = pNANDDescriptor->totalPages * stc_MaxQuantityChips;
   stc_MaxBlocks   = pNANDDescriptor->wTotalBlocks  * stc_MaxQuantityChips;

   // ...but nand_hal_spy is limited by the quantities of pages and blocks
   // that it can track in its memory.
   stc_MaxPages    = (uint32_t)MIN( stc_MaxPages, DDI_NAND_HAL_SPY_MAX_PAGES );
   stc_MaxBlocks   = (uint32_t)MIN( stc_MaxBlocks, DDI_NAND_HAL_SPY_MAX_BLOCKS );

   // If already initialized, do nothing else.
   if (stc_bInitialized) return (SUCCESS);

   if ( NULL == pNANDDescriptor )              return (ERROR_DDI_NAND_GROUP_GENERAL);
   if ( NULL == pNANDDescriptor->pNANDParams ) return (ERROR_DDI_NAND_GROUP_GENERAL);

   /* *******************************************************************
      Modify the warning thresholds, if they were given as parameters.
   ******************************************************************* */

   if ( 0 != nReadWarningThreshold )
   n_ddi_nand_hal_spy_ReadWarningThreshold     = nReadWarningThreshold;

   if ( 0 != nEraseWarningThreshold )
   n_ddi_nand_hal_spy_EraseWarningThreshold    = nEraseWarningThreshold;

   
   /* *******************************************************************
      Compute values for stc_PageOriginPerNand for all NANDs.
      stc_PageOriginPerNand[] are offsets into the
      n_ddi_nand_hal_spy_ReadsPerPage[] counter array for each
      NAND, used to quickly access the counter array upon page
      reads.
   ******************************************************************* */
   for ( iNand=0 ; iNand<DDI_NAND_HAL_SPY_MAX_NANDS ; iNand++ )
   {
      if ( iPage < DDI_NAND_HAL_SPY_MAX_PAGES )
      {            
         // iPage fits within the range of SPY memory.
         // Memorize its origin.
         stc_PageOriginPerNand[iNand] = iPage;
         iPage += pNANDDescriptor->totalPages;
      }
      else
      {
         // iPage does not fit within the range of SPY memory.
         // Set the origin beyond the legal range as a flag.
         stc_PageOriginPerNand[iNand] = iPage;
      }
   }

   /* *******************************************************************
      Compute values for stc_BlockOriginPerNand for all NANDs.
      stc_BlockOriginPerNand[] are offsets into the
      n_ddi_nand_hal_spy_PageWriteIndexPerBlock[] and
      n_ddi_nand_hal_spy_ErasuresPerBlock counter arrays for each
      NAND, used to quickly access the counter arrays upon block
      operations.
   ******************************************************************* */
   for ( iNand=0 ; iNand<DDI_NAND_HAL_SPY_MAX_NANDS ; iNand++ )
   {
      if ( iBlock < DDI_NAND_HAL_SPY_MAX_BLOCKS )
      {            
         // iBlock fits within the range of SPY memory.
         // Memorize its origin.
         stc_BlockOriginPerNand[iNand] = iBlock;
         iBlock += pNANDDescriptor->wTotalBlocks;
      }
      else
      {
         // iBlock does not fit within the range of SPY memory.
         // Set the origin beyond the legal range as a flag.
         stc_BlockOriginPerNand[iNand] = iBlock;
      }
   }



   /* *******************************************************************
      Override the HAL API functions for this NAND with our own
      spy functions.
   ******************************************************************* */
   
   NandHalSpy * interposer = new NandHalSpy();
   interposer->init(pNANDDescriptor);
   interposer->override();
   stc_bAPIOverridden = TRUE;
   stc_pNANDDescriptor = interposer;

   /* *******************************************************************
      Clear the counters for all NAND blocks and pages.
   ******************************************************************* */
   RetVal = ddi_nand_hal_spy_Reset();

   if (SUCCESS == RetVal)
   {
      // Init successful.
      stc_bInitialized = TRUE;
   }
   else
   {
      // Init failed.  Clean up.
      ddi_nand_hal_spy_DeInitPrivate();
   }

   return (RetVal);
}

/**************************************************************************
* Function:    ddi_nand_hal_spy_DeInit
*
* Description: Reverses the actions of ddi_nand_hal_spy_Init().
*
* Input:       
*
* Output:      
*
* Return:      
*
* See Also:    
*************************************************************************/
RtStatus_t ddi_nand_hal_spy_DeInit(void)
{
   // If we're not initialized, then do nothing.
   if ( !stc_bInitialized ) return (SUCCESS);

   return ( ddi_nand_hal_spy_DeInitPrivate( ) );
}

/**************************************************************************
* Function:    ddi_nand_hal_spy_Reset
*
* Description: Erases the SPY counters.
*
* Input:       
*
* Output:      
*
* Return:      
*
* See Also:    
*************************************************************************/
RtStatus_t ddi_nand_hal_spy_Reset(void)
{
   memset(n_ddi_nand_hal_spy_ReadsPerPage, 0, sizeof(n_ddi_nand_hal_spy_ReadsPerPage) );

   memset(n_ddi_nand_hal_spy_PageWriteIndexPerBlock, 0, sizeof(n_ddi_nand_hal_spy_PageWriteIndexPerBlock) );

   memset(n_ddi_nand_hal_spy_ErasuresPerBlock, 0, sizeof(n_ddi_nand_hal_spy_ErasuresPerBlock) );
   n_ddi_nand_hal_spy_ErasuresPerBlockMaxQty   = 0;
   i_ddi_nand_hal_spy_ErasuresPerBlockMax      = DDI_NAND_HAL_SPY_MAX_BLOCKS;

   return (SUCCESS);
}

/**************************************************************************
 * Function:    ddi_nand_hal_spy_isIndexinArr
 *
 * Description: static function used to find if a particular page or block index
 *                  is in the array. This array will be filled using debugger and
 *                  is used to analyse NAND accesses
 *
 * Input:       Page or Block index
 *
 * Output:     bool - TRUE - if index value is in the array, FALSE - otherwise
 *
 * Return:      TRUE - if index value is in the array, FALSE - otherwise
 *
 * See Also:    
 *************************************************************************/
static bool ddi_nand_hal_spy_isIndexinArr(uint32_t iPage_or_iBlock)
{
    uint32_t arrayIndex;
    bool RetVal = FALSE;

    for ( arrayIndex = 0 ; arrayIndex < DDI_NAND_HAL_SPY_NAND_ANALYSIS_INDEX; arrayIndex++ )
    {
        if ( iPage_or_iBlock == i_ddi_nand_hal_spy_NandAnalysis[arrayIndex] )
        {
            RetVal = TRUE;
        }
    }
    return RetVal;
}

/**************************************************************************
* Function:    ddi_nand_hal_spy_CountBlockErase
*
* Description: Increments the erase counter for wBlockNum in the NAND
*              identified by pNANDDescriptor.
*              Prints a message if the quantity of erasures reaches
*              n_ddi_nand_hal_spy_EraseWarningThreshold.
*
* Input:       wSectorNum is the block relative to the origin of the NAND
*              designated by pNANDDescriptor.
*
* Output:      
*
* Return:      
*
* See Also:    
*************************************************************************/
RtStatus_t ddi_nand_hal_spy_CountBlockErase( NandPhysicalMedia * pNANDDescriptor, uint32_t wBlockNum )
{
   uint32_t    iPage;
   uint32_t    nPages;
   uint32_t    iBlock;

   if ( wBlockNum >= pNANDDescriptor->wTotalBlocks )
   {
      WL_TEST_LOG("HAL SPY block %d beyond max blocks %d.  Chip = %d\r\n",
      wBlockNum,
      pNANDDescriptor->wTotalBlocks,
      pNANDDescriptor->wChipNumber );
      return ( ERROR_DDI_NAND_GROUP_GENERAL );
   }

   // Offset the NAND-based block number into the dimensions of n_ddi_nand_hal_spy_ErasuresPerBlock[].
   iBlock = wBlockNum + stc_BlockOriginPerNand[pNANDDescriptor->wChipNumber];

   if ( iBlock >= DDI_NAND_HAL_SPY_MAX_BLOCKS )
   {
      WL_TEST_LOG("HAL SPY block %d out of log range.  chip=%d \r\n",
      iBlock,
      pNANDDescriptor->wChipNumber );
      return ( ERROR_DDI_NAND_GROUP_GENERAL );
   }

   /* *******************************************************************
      Record the statistic for erasures

      We only record the statistic up to the threshold.
      
      Do not change the "<=" logic.  It is needed in the subsequent
      logging code. The counter will reach the threshold, causing one
      message to be logged, and next time will pass to "+1" above
      threshold and cease incrementing.  No more messages will be
      logged, nor will the counter increase.
   ******************************************************************* */
   if ( n_ddi_nand_hal_spy_ErasuresPerBlock[iBlock] <= n_ddi_nand_hal_spy_EraseWarningThreshold )
   {
      n_ddi_nand_hal_spy_ErasuresPerBlock[iBlock]++;
   }

   if ( stc_bSpyNandAnalysis )
   {
      if ( ddi_nand_hal_spy_isIndexinArr(iBlock) )
      {
// #pragma asm
         // nop
// #pragma endasm
      }
   }

   /* *******************************************************************
      Record the maximum.
   ******************************************************************* */
   if ( n_ddi_nand_hal_spy_ErasuresPerBlock[iBlock] > n_ddi_nand_hal_spy_ErasuresPerBlockMaxQty )
   {
      n_ddi_nand_hal_spy_ErasuresPerBlockMaxQty   = n_ddi_nand_hal_spy_ErasuresPerBlock[iBlock];
      i_ddi_nand_hal_spy_ErasuresPerBlockMax      = iBlock;
   }
   
   if ( n_ddi_nand_hal_spy_ErasuresPerBlock[iBlock] == n_ddi_nand_hal_spy_EraseWarningThreshold )
   {
      WL_TEST_LOG("HAL SPY: write/erase limit reached for chip=%d block=%d\r\n",
      pNANDDescriptor->wChipNumber,
      wBlockNum );
   }

   /* *******************************************************************
      Clean the statistics for reads from the affected pages.
   ******************************************************************* */
   iPage       = pNANDDescriptor->blockToPage(iBlock);
   nPages      = pNANDDescriptor->blockToPage(iBlock+1) - iPage;

   memset(&(n_ddi_nand_hal_spy_ReadsPerPage[iPage]), 0, (nPages * sizeof(ddi_nand_hal_spy_ReadsPerPage_t)) );

   /* *******************************************************************
      Zero the index of the last page written to this block.
   ******************************************************************* */
   n_ddi_nand_hal_spy_PageWriteIndexPerBlock[iBlock] = 0;

   return ( SUCCESS );
}

/**************************************************************************
* Function:    ddi_nand_hal_spy_CountPageRead
*
* Description: Increments the read counter for wSectorNum (page number) in the NAND
*              identified by pNANDDescriptor.
*              Prints a message if the quantity of reads reaches
*              n_ddi_nand_hal_spy_ReadWarningThreshold.
*
* Input:       wSectorNum is the page relative to the origin of the NAND
*              designated by pNANDDescriptor.
*
* Output:      
*
* Return:      
*
* See Also:    
*************************************************************************/
RtStatus_t ddi_nand_hal_spy_CountPageRead( NandPhysicalMedia * pNANDDescriptor, 
uint32_t wSectorNum )
{
   uint32_t    iPage;

   if ( NULL == pNANDDescriptor ) return (ERROR_DDI_NAND_GROUP_GENERAL);

   if ( wSectorNum >= pNANDDescriptor->totalPages )
   {
      WL_TEST_LOG("HAL SPY sector %d beyond max sectors %d.  Chip = %d\r\n", 
         wSectorNum,
         pNANDDescriptor->totalPages,
         pNANDDescriptor->wChipNumber);
      return ( ERROR_DDI_NAND_GROUP_GENERAL );
   }

   iPage = stc_PageOriginPerNand[pNANDDescriptor->wChipNumber] + wSectorNum;

   if ( iPage >= DDI_NAND_HAL_SPY_MAX_PAGES )
   {
      WL_TEST_LOG("HAL SPY page %d out of log range.  chip=%d sector=%d\r\n",
         iPage,
         pNANDDescriptor->wChipNumber,
         wSectorNum );
      return ( ERROR_DDI_NAND_GROUP_GENERAL );
   }

   /* *******************************************************************
      Record the statistic for reads
      
      We only record the statistic up to the threshold.
      
      Do not change the "<=" logic.  It is needed in the subsequent
      logging code. The counter will reach the threshold, causing one
      message to be logged, and next time will pass to "+1" above
      threshold and cease incrementing.  No more messages will be
      logged, nor will the counter increase.
   ******************************************************************* */
   if ( n_ddi_nand_hal_spy_ReadsPerPage[iPage] <= n_ddi_nand_hal_spy_ReadWarningThreshold )
   {
      n_ddi_nand_hal_spy_ReadsPerPage[iPage]++;
   }

   if ( stc_bSpyNandAnalysis )
   {
      if ( ddi_nand_hal_spy_isIndexinArr(iPage) )
      {
// #pragma asm
         // nop
// #pragma endasm
      }
   }

   if ( n_ddi_nand_hal_spy_ReadsPerPage[iPage] == n_ddi_nand_hal_spy_ReadWarningThreshold )
   {
      WL_TEST_LOG("HAL SPY: read limit reached for chip=%d page=%d\r\n",
         pNANDDescriptor->wChipNumber,
         wSectorNum );
   }

   return ( SUCCESS );
}


/**************************************************************************
* Function:    ddi_nand_hal_spy_CountPageWrite
*
* Description: Checks for an out-of-order write in the NAND designated by
*              pNANDDescriptor.  Prints a message if such a write occurs.
*
* Input:       wSectorNum is the page relative to the origin of the NAND
*              designated by pNANDDescriptor.
*
* Output:      
*
* Return:      
*
* See Also:    
*************************************************************************/
RtStatus_t ddi_nand_hal_spy_CountPageWrite( NandPhysicalMedia * pNANDDescriptor, uint32_t wSectorNum )
{
   uint32_t    iBlockInChip;
   uint32_t    iBlock;
   uint32_t    iSectorOffsetWithinBlock;

   if ( NULL == pNANDDescriptor ) return (ERROR_DDI_NAND_GROUP_GENERAL);

   pNANDDescriptor->pageToBlockAndOffset(wSectorNum, &iBlockInChip, &iSectorOffsetWithinBlock);
   
   if ( iBlockInChip >= pNANDDescriptor->wTotalBlocks )
   {
      WL_TEST_LOG("\nHAL SPY block %d beyond max blocks %d of chip = %d\n",
      iBlockInChip,
      pNANDDescriptor->wTotalBlocks,
      pNANDDescriptor->wChipNumber );
      return ( ERROR_DDI_NAND_GROUP_GENERAL );
   }

   iBlock = iBlockInChip + stc_BlockOriginPerNand[pNANDDescriptor->wChipNumber];

   if ( iBlock >= DDI_NAND_HAL_SPY_MAX_BLOCKS )
   {
      WL_TEST_LOG("\nHAL SPY block %d out of log range.  chip=%d sector=%d\n",
      iBlock,
      pNANDDescriptor->wChipNumber,
      wSectorNum );
      return ( ERROR_DDI_NAND_GROUP_GENERAL );
   }

   /* *******************************************************************
      Check if the pages in this block are being written in order.
      MLC NANDs do not tolerate out-of-order writes.
      Note that Type8 NANDs (and possibly other NANDs) allow multiple
      writes to a single physical page as long as they are offset
      to different ranges of columns (bytes).  Therefore, a
      rewrite to the same page is legal for such NANDs, and we
      could have
      (iSectorOffsetWithinBlock == n_ddi_nand_hal_spy_PageWriteIndexPerBlock[ iBlock ]).
   ******************************************************************* */
   if ( iSectorOffsetWithinBlock < n_ddi_nand_hal_spy_PageWriteIndexPerBlock[ iBlock ] )
   {
      // We don't care about 0, because that's the initial value of the statistic.
      if ( 0 != n_ddi_nand_hal_spy_PageWriteIndexPerBlock[ iBlock ] )
      {
         // Out of order
         WL_TEST_LOG("\nHAL SPY: Out-of-order write for chip=%d block=%d sector=%d (in chip), sector-in-block=%d\n",
         pNANDDescriptor->wChipNumber,
         iBlockInChip,
         wSectorNum,
         iSectorOffsetWithinBlock );
         WL_TEST_LOG("\nHAL SPY: Previous write sector=%d, sector-in-block=%d\n",
         pNANDDescriptor->blockToPage(iBlock) + n_ddi_nand_hal_spy_PageWriteIndexPerBlock[ iBlock ],
         n_ddi_nand_hal_spy_PageWriteIndexPerBlock[ iBlock ] );
         
         // Do not record the current page number.  Keep the bigger, previous one.
         // Critical error occurs, we need shutdown the board         
         assert(0);
      }
   }
   else
   {
      /*WL_TEST_LOG("\nHAL SPY: In-of-order write for chip=%d block=%d sector=%d (in chip), sector-in-block=%d\n",
      pNANDDescriptor->wChipNumber,
      iBlockInChip,
      wSectorNum,
      iSectorOffsetWithinBlock );
      WL_TEST_LOG("\nHAL SPY: Previous write sector=%d, sector-in-block=%d\n",
      pNANDDescriptor->blockToPage(iBlock) + n_ddi_nand_hal_spy_PageWriteIndexPerBlock[ iBlock ],
      n_ddi_nand_hal_spy_PageWriteIndexPerBlock[ iBlock ] );*/
      // Record the current page number.
      n_ddi_nand_hal_spy_PageWriteIndexPerBlock[ iBlock ] = iSectorOffsetWithinBlock;
   }
   
   return ( SUCCESS );
}

static RtStatus_t ddi_nand_hal_spy_DeInitPrivate(void)
{
   if ( NULL == stc_pNANDDescriptor )
   {
      return ( ERROR_DDI_NAND_GROUP_GENERAL );
   }

   stc_MaxPages        =
   stc_MaxBlocks       =
   stc_MaxQuantityChips   = 0;

   /* *******************************************************************
      Restore the HAL API functions for this NAND, if necessary.
   ******************************************************************* */
   return ( ddi_nand_hal_spy_RestoreAPI( stc_pNANDDescriptor ) );
}

/**************************************************************************
* Function:    ddi_nand_hal_spy_RestoreAPI
*
* Description: Reverses the actions of ddi_nand_hal_spy_OverrideAPI().
*
* Input:       
*
* Output:      
*
* Return:      
*
* See Also:    
*************************************************************************/
RtStatus_t ddi_nand_hal_spy_RestoreAPI( NandPhysicalMedia * pNANDDescriptor )
{
   if ( !stc_bAPIOverridden ) return ( SUCCESS );

   if ( NULL == pNANDDescriptor ) return ( ERROR_DDI_NAND_GROUP_GENERAL );

   /* *******************************************************************
      Restore the void * used for the NAND HAL API.
   ******************************************************************* */
   ((NandHalSpy *)pNANDDescriptor)->restore();
   delete pNANDDescriptor;

   stc_bAPIOverridden = FALSE;

   return ( SUCCESS );
}

/* EOF */
