
// ****************************************************************************
//
//                        TinyBoy - Device init
//
// ****************************************************************************

#include "../../include.h"

// Initialize device
void DevInit()
{
#if USE_KEY		// 1=use keyboard support
	// Initialize keyboard service (should be called before display init)
	KeyInit();
#endif

#if USE_SOUND		// use sound support 1=tone, 2=melody
	// Sound initialize
	SoundInit();
#endif

#if USE_DISP		// 1=use display support
	// Display initialize
	DispInit();
#endif

#if USE_DRAW || USE_PRINT
	// clear screen
	DrawClear();
#endif
}

// Terminate device
void DevTerm()
{
#if USE_KEY		// 1=use keyboard support
	// wait for no key pressed
	KeyWaitNoPressed();
#endif

#if USE_DISP		// 1=use display support
	// Display terminate
	DispTerm();
#endif

#if USE_SOUND		// use sound support 1=tone, 2=melody
	// Sound terminate
	SoundTerm();
#endif

#if USE_KEY		// 1=use keyboard support
	// terminate keyboard
	KeyTerm();
#endif
}
