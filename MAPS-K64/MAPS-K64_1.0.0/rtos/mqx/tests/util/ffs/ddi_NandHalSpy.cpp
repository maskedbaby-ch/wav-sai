#include "Ddi_nand_hal_internal.h"
#include "ddi_nand_hal.h"
#include "nandhal_spy.h"
#include "ddi_NandHalSpy.h"

extern NandHalContext_t g_nandHalContext;
/*NandHalSpy *p_NandHalSpy;*/

void NandHalSpyInit() {

   /* p_NandHalSpy = new NandHalSpy();
   NandPhysicalMedia * original = NULL;   
   
   original = g_nandHalContext.nands[0];
   p_NandHalSpy->init(original);
   g_nandHalContext.nands[0] = p_NandHalSpy;
   */
   //TODO:manhnb
   ddi_nand_hal_spy_Init(g_nandHalContext.nands[0], 
      50000 /* nReadWarningThreshold */,
      5000  /* nEraseWarningThreshold */);
}

void NandHalSpyDeinit() {
   NandPhysicalMedia * original = NULL;
   
   /*g_nandHalContext.nands[0] = p_NandHalSpy->getOriginalNandPhysicalMedia();*/
   
   ddi_nand_hal_spy_DeInit();
   /*
   delete p_NandHalSpy;
   p_NandHalSpy = NULL;*/

   
}

/* EOF */
