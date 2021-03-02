#ifndef _NANDHAL_SPY_H_
#define _NANDHAL_SPY_H_

#include "ddi_nand_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

   // Type must be at least able to represent DDI_NAND_HAL_SPY_DEFAULT_READ_WARNING_THRESHOLD.
   typedef uint16_t    ddi_nand_hal_spy_ReadsPerPage_t;

   // Type must be able to represent DDI_NAND_HAL_SPY_MAX_PAGES_PER_BLOCK.
   typedef uint8_t     ddi_nand_hal_spy_PageWriteIndexPerBlock_t;

   // Type must be at least able to represent DDI_NAND_HAL_SPY_DEFAULT_ERASE_WARNING_THRESHOLD.
   typedef uint16_t    ddi_nand_hal_spy_ErasuresPerBlock_t;

   // Type must be at least able to represent maximum reads and maximum erasures for debugging/logging purposes
   typedef struct {
      uint32_t index;
      uint16_t value;
   } ddi_nand_hal_spy_GetMax_t;

   // This is used to store NAND page read, write and block erase timings and is used for debugging purposes
   typedef struct {
      uint64_t u32_sumofIter;
      uint32_t u32_numofIter;
   } ddi_nand_hal_spy_TimeAnalysis_t;

#define DDI_NAND_HAL_SPY_MAX_NANDS                      4
#define DDI_NAND_HAL_SPY_MAX_PAGES                      (1 << 20)
#define DDI_NAND_HAL_SPY_MAX_PAGES_PER_BLOCK            128
#define DDI_NAND_HAL_SPY_MAX_BLOCKS                     (DDI_NAND_HAL_SPY_MAX_PAGES/DDI_NAND_HAL_SPY_MAX_PAGES_PER_BLOCK)

   // Default quantity of reads per page allowed.
   // Beyond this quantity, spy prints warnings.
#define DDI_NAND_HAL_SPY_DEFAULT_READ_WARNING_THRESHOLD    50000

   // Default quantity of writes per block allowed.
   // Beyond this quantity, spy prints warnings.
#define DDI_NAND_HAL_SPY_DEFAULT_ERASE_WARNING_THRESHOLD   5000

   // Length of array which is used to store NAND Page or Block index to help analyze
   // NAND accesses with the debugger.
#define DDI_NAND_HAL_SPY_NAND_ANALYSIS_INDEX    10

   /* *******************************************************************
   Physical dimensions
******************************************************************* */
   
   static uint32_t     stc_MaxPages        = 0;

   static uint32_t     stc_MaxBlocks       = 0;

   static uint32_t     stc_MaxQuantityChips   = 0;

   static uint32_t     stc_PageOriginPerNand[ DDI_NAND_HAL_SPY_MAX_NANDS ];
   static uint32_t     stc_BlockOriginPerNand[ DDI_NAND_HAL_SPY_MAX_NANDS ];

   /********************************************************************
   Thresholds
******************************************************************* */
   
   // Quantity of reads per page allowed.
   extern uint32_t    n_ddi_nand_hal_spy_ReadWarningThreshold;

   // Quantity of writes per block allowed.
   extern uint32_t    n_ddi_nand_hal_spy_EraseWarningThreshold;

   /********************************************************************
   Overridden descriptors and functions
******************************************************************* */
   
   // Used to restore the NAND HAL API when spy is finished.
   static NandPhysicalMedia *     stc_pNANDDescriptor;

   /********************************************************************
   Initialization flags
******************************************************************* */
   
   // TRUE indicates that spy has overridden the HAL API.
   static bool                                 stc_bAPIOverridden = FALSE;

   // TRUE indicates that spy is initialized.
   static bool                                 stc_bInitialized = FALSE;

   // TRUE indicates that spy debug is needed to analyse NAND accesses (reads/erasures)
   static bool                                 stc_bSpyNandAnalysis = FALSE;

   // NAND Page or Block index to be analysed. This array is filled using debugger to analyse NAND accesses
   extern uint32_t i_ddi_nand_hal_spy_NandAnalysis[DDI_NAND_HAL_SPY_NAND_ANALYSIS_INDEX];

   extern bool ddi_nand_hal_spy_bIsLinked;

   /********************************************************************
   The following are big chunks of memory, destined for SDRAM.
******************************************************************* */
   // Count of the times each page has been read.
   extern ddi_nand_hal_spy_ReadsPerPage_t             n_ddi_nand_hal_spy_ReadsPerPage[DDI_NAND_HAL_SPY_MAX_PAGES];

   // Index of the last page written in each block.
   extern ddi_nand_hal_spy_PageWriteIndexPerBlock_t   n_ddi_nand_hal_spy_PageWriteIndexPerBlock[DDI_NAND_HAL_SPY_MAX_BLOCKS];

   // Count of the times each block has been erased.
   extern ddi_nand_hal_spy_ErasuresPerBlock_t         n_ddi_nand_hal_spy_ErasuresPerBlock[DDI_NAND_HAL_SPY_MAX_BLOCKS];
   extern ddi_nand_hal_spy_ErasuresPerBlock_t         n_ddi_nand_hal_spy_ErasuresPerBlockMaxQty;
   extern uint32_t                                    i_ddi_nand_hal_spy_ErasuresPerBlockMax;

   extern ddi_nand_hal_spy_TimeAnalysis_t ddi_nand_hal_spy_readTime, ddi_nand_hal_spy_writeTime, ddi_nand_hal_spy_eraseTime;

   class NandHalSpy : public NandPhysicalMedia {
   public:
      NandHalSpy();
      
      virtual ~NandHalSpy();
      
      void init(NandPhysicalMedia * original);
      
      virtual RtStatus_t reset() { return SUCCESS;}
      
      virtual RtStatus_t readID(uint8_t * pReadIDCode);
      
      virtual RtStatus_t readRawData(uint32_t wSectorNum, uint32_t columnOffset,
      uint32_t readByteCount, SECTOR_BUFFER * pBuf);
      
      virtual RtStatus_t readPage(uint32_t uSectorNumber, SECTOR_BUFFER * pBuffer,
      SECTOR_BUFFER * pAuxiliary);
      
      virtual RtStatus_t readMetadata(uint32_t uSectorNumber, SECTOR_BUFFER * pBuffer) ;
      
      virtual RtStatus_t writeRawData(uint32_t pageNumber, uint32_t columnOffset,
      uint32_t writeByteCount, const SECTOR_BUFFER * data);
      
      virtual RtStatus_t writePage(uint32_t uSectorNum, const SECTOR_BUFFER * pBuffer,
      const SECTOR_BUFFER * pAuxiliary) ;
      
      virtual RtStatus_t writeFirmwarePage(uint32_t uSectorNum,
      const SECTOR_BUFFER * pBuffer, const SECTOR_BUFFER * pAuxiliary) ;
      
      virtual RtStatus_t readFirmwarePage(uint32_t uSectorNumber,
      SECTOR_BUFFER * pBuffer, SECTOR_BUFFER * pAuxiliary) ;

      virtual RtStatus_t eraseBlock(uint32_t uBlockNumber) ;
      
      virtual RtStatus_t eraseMultipleBlocks(uint32_t startBlockNumber,
      uint32_t requestedBlockCount, uint32_t * actualBlockCount) ;
      
      virtual RtStatus_t copyPages(NandPhysicalMedia * targetNand,
      uint32_t wSourceStartSectorNum, uint32_t wTargetStartSectorNum,
      uint32_t wNumSectors, SECTOR_BUFFER * sectorBuffer,
      SECTOR_BUFFER * auxBuffer, NandCopyPagesFilter * filter,
      uint32_t * successfulPages);
      
      virtual bool isBlockBad(uint32_t blockAddress, SECTOR_BUFFER * auxBuffer,
      
      bool checkFactoryMarkings = false, RtStatus_t * readStatus = NULL);
      
      virtual RtStatus_t markBlockBad(uint32_t blockAddress, SECTOR_BUFFER * pageBuffer,
      SECTOR_BUFFER * auxBuffer);

      virtual RtStatus_t enableSleep(bool isEnabled)	 { return SUCCESS; }
      
      virtual bool isSleepEnabled()  { return false; }
      
      virtual char * getDeviceName(){ return NULL; }
      
      bool isOrderSequential(uint32_t sectorNumber);
      
      void setOriginalNandPhysicalMedia( NandPhysicalMedia * original);

      NandPhysicalMedia * getOriginalNandPhysicalMedia() ;
      
      void override();
      
      void restore();
      
   protected:
      NandPhysicalMedia * m_originalNandHal;
   };

   ////////////////////////////////////////////////////////////////////////////////
   //! \brief      Initialization function for NAND HAL SPY.
   //!
   //! \fntype     Function
   //!
   //! Call this function to start NAND telemetry.  Actions:
   //!     - Uses an initialized NandPhysicalMedia * to learn
   //!       the sizes of the NANDs.
   //!     - Inserts SPY telemetry functions in place of the HAL API
   //!       for these NANDs.  (All NANDs in the system are assumed to be of the
   //!       same size and type, so the same HAL API applies to all NANDs.)
   //!       For each API call, SPY counts the NAND accesses and then invokes
   //!       the original NAND HAL API function.
   //!     - Clears all SPY buffers used to count reads, writes, and erasues.
   //! 
   //! SPY has been activated when this function returns.  Spy prints
   //! warnings to tss_logtext_printf() when any of the following occurs:
   //!     - Any NAND page is read more times than nReadWarningThreshold.
   //!     - Any NAND block is erased more than nEraseWarningThreshold.
   //!     - Any NAND block is written using an out-of-order page sequence.
   //! 
   //! Memory usage:
   //! Memory to hold counters is statically-allocated in ddi_nand_hal_spy,
   //! and is located in SDRAM.  The size of this memory is set by macros
   //! in ddi_nand_hal_spy.c, and is nominally a little over 2Mbytes.
   //!
   //! \param[in]  pNANDDescriptor             An initialized NAND descriptor.
   //! \param[in]  nReadWarningThreshold       Allowable quantity of reads per NAND page.
   //!                                         Use 0 to cause SPY to use its internal default
   //!                                         DDI_NAND_HAL_SPY_DEFAULT_READ_WARNING_THRESHOLD.
   //! \param[out] nEraseWarningThreshold      Allowable quantity of erasures per NAND page.
   //!                                         Use 0 to cause SPY to use its internal default
   //!                                         DDI_NAND_HAL_SPY_DEFAULT_ERASE_WARNING_THRESHOLD.
   //!
   //! \retval SUCCESS                         If SPY has been activated.
   //! \retval ERROR_DDI_NAND_GROUP_GENERAL    If the media descriptor or some referenced descriptor was NULL.
   //!
   //! \pre    pNANDDescriptor must have been initialized.
   ////////////////////////////////////////////////////////////////////////////////
   extern RtStatus_t ddi_nand_hal_spy_Init( NandPhysicalMedia * pNANDDescriptor,
   ddi_nand_hal_spy_ReadsPerPage_t     nReadWarningThreshold,
   ddi_nand_hal_spy_ErasuresPerBlock_t nEraseWarningThreshold );

   ////////////////////////////////////////////////////////////////////////////////
   //! \brief      Stop using NAND HAL SPY.
   //!
   //! \fntype     Function
   //!
   //! Call this function to quit using NAND HAL SPY.  This function
   //! stops SPY from counting NAND uses.  It restores the original
   //! NAND HAL API defined for the NANDs before ddi_nand_hal_spy_Init()
   //! was called.
   //!
   //! No memory is freed, because it is statically allocated in SPY.
   //!
   //! \retval SUCCESS                         If no error has occurred
   //! \retval ERROR_DDI_NAND_GROUP_GENERAL    If SPY was not initialized in the first place.
   //!
   ////////////////////////////////////////////////////////////////////////////////
   extern RtStatus_t ddi_nand_hal_spy_DeInit(void);

   ////////////////////////////////////////////////////////////////////////////////
   //! \brief      Clear all counters in ddi_nand_hal_spy
   //!
   //! \fntype     Function
   //!
   //! Clear all counters in ddi_nand_hal_spy.
   //!
   //! \retval SUCCESS             All the time.
   //!
   ////////////////////////////////////////////////////////////////////////////////
   extern RtStatus_t ddi_nand_hal_spy_Reset(void);

   static bool ddi_nand_hal_spy_isIndexinArr(uint32_t iPage_or_iBlock);
   
   RtStatus_t ddi_nand_hal_spy_CountBlockErase( NandPhysicalMedia * pNANDDescriptor, uint32_t wBlockNum );
   
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
   uint32_t wSectorNum );

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
   RtStatus_t ddi_nand_hal_spy_CountPageWrite( NandPhysicalMedia * pNANDDescriptor, uint32_t wSectorNum );

   static RtStatus_t ddi_nand_hal_spy_DeInitPrivate(void);

   RtStatus_t ddi_nand_hal_spy_RestoreAPI( NandPhysicalMedia * pNANDDescriptor );

#ifdef __cplusplus
}
#endif

#endif

/* EOF */
