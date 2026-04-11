// CrossZGB fuctions which need to be overridden by the game

#include <gbdk/platform.h>

// Master System PAUSE button support
#if defined(MASTERSYSTEM)
extern volatile UINT8 pause;

// the function name MUST be "NMI_ISR", that replace the empty stub handler
void NMI_ISR (void) CRITICAL INTERRUPT {
    pause = (pause) ? FALSE : TRUE;
}
#endif
