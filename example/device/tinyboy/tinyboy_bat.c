// ****************************************************************************
//
//                             TinyBoy - Battery
//
// ****************************************************************************

#include "../../include.h"

// init battery measurement
void BatInit() {
#if USE_KEY
  // Terminate key service to avoid interference with ADC
  KeyTerm();
#else
  // Enable ADC
  ADC1_Enable();
#endif

  // ADC1 clock enable
  RCC_ADC1ClkEnable();

  // initialize ADC single conversion mode
  ADC1_InitSingle();
}

// get battery voltage, integer in mV
u32 GetBat() {
  // switch to battery input
  u32 vref = ADC1_GetSingleMul(8, B16) >> 9; // result value (12+16-9) 19 bits
  vref = (u32)(1200 << 19) / vref;

  return vref;
}

// terminate battery measurement
void BatTerm() {
  RCC_ADC1ClkDisable();
#if USE_KEY
  KeyInit();
#else
  // disable ADC
  ADC1_Disable();
#endif
}
