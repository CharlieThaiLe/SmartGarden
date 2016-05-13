#ifndef _TIMER_DELAY_H_
#define _TIMER_DELAY_H_

#include <stdbool.h>
#include <stdint.h>


//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif


#define START_TIMER_DELAY 0x00
#define GOALS_TIMER_DELAY 0x01

extern void TimerDelayConfigure(void);
extern void TimerDelayus( unsigned long us);
extern void TimerDelayms( unsigned int ms);
extern void TimerDelays( unsigned char s);
extern void TimerDelayInterrupt(void);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif
