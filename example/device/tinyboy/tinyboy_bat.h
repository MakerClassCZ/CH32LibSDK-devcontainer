// ****************************************************************************
//
//                             TinyBoy - Battery
//
// ****************************************************************************

#if USE_BAT

#ifndef _TINYBOY_BAT_H
#define _TINYBOY_BAT_H

#ifdef __cplusplus
extern "C" {
#endif

// init battery measurement
void BatInit();

// get battery voltage in V
u32 GetBat();

// terminate battery measurement
void BatTerm();

#ifdef __cplusplus
}
#endif

#endif // _TINYBOY_BAT_H

#endif // USE_TINYBOY
