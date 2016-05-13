
#include "driverlib/rom.h"
#include "timerdelay.h"
#include "inc/hw_ints.h"
#include "inc/hw_timer.h"
#include "inc/hw_types.h"
#include "inc/hw_nvic.h"
#include "inc/hw_memmap.h"

#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"

static uint8_t g_TimerDelayFlag;


void TimerDelayConfigure(void){
  SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER0);
  TimerConfigure( WTIMER0_BASE,TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PERIODIC_UP);
  TimerLoadSet64(WTIMER0_BASE,0xFFFFFFFFF);
  TimerIntEnable(WTIMER0_BASE,TIMER_TIMA_MATCH);
  TimerIntRegister(WTIMER0_BASE,TIMER_A,TimerDelayInterrupt);
  TimerEnable(WTIMER0_BASE,TIMER_A);
  IntMasterEnable();
  IntEnable(INT_WTIMER1A);
  g_TimerDelayFlag = START_TIMER_DELAY;
}
/*

*/
void TimerDelayInterrupt(void){
  TimerIntClear(WTIMER0_BASE,TIMER_TIMA_MATCH);
  g_TimerDelayFlag = GOALS_TIMER_DELAY;
}
/*

*/
void TimerDelayus(unsigned long us){
  TimerMatchSet64(WTIMER0_BASE,us*(SysCtlClockGet()/1000000));
  g_TimerDelayFlag = START_TIMER_DELAY;
  HWREG(WTIMER0_BASE+TIMER_O_TAV) = 0;
  while(g_TimerDelayFlag != GOALS_TIMER_DELAY);
}
/*

*/
void TimerDelayms(unsigned int ms){
  TimerDelayus(ms*1000);
}
/*

*/
void TimerDelays(unsigned char s){
  TimerDelayus(s*1000000);
}

