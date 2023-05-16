/*
 * timer.c
 *
 *  Methods to enable and utilize the BG Timer to provide precise delays and interrupts
 *  based on the device clocking in use.
 *
 *  Created on: Jan 26, 2020
 *      Author: brian
 *
 *  Resources Used:
 *    - https://www.silabs.com/documents/public/reference-manuals/efr32xg1-rm.pdf
 *    - https://siliconlabs.github.io/Gecko_SDK_Doc/efm32g/html/index.html
 *    - an0026-efm32-letimer.pdf
 */
/*---------------------------------------------------*/
#include <math.h>
#include "em_core.h"
#include "native_gecko.h"

#include "timer.h"
#include "gpio.h"
#include "cmu.h"
/*---------------------------------------------------*/
#define CLKRATE (32768) // Hz

#define TEMP_SAMPLE_PERIOD_MSEC  (3000) /* Temp Sample period in msec */

#define LFXO_CLKRATE_PRESCALER_FACTOR_1SEC (CLKRATE/LFXO_PRESCALER_DIV) /* Conversion for LFXO with 1 Prescaler to 1 sec (32768Hz/Prescaler) */
#define ULFRCO_CLKRATE_PRESCALER_FACTOR_1SEC (1000) /* Conversion for ULFRCO with 1 Prescaler to 1 sec (1000Hz/1) */

#define MSEC_TO_SEC (1000)
#define USEC_TO_SEC (1000000)
#define LFXO_TICK_TO_USEC   (61)   /* Convert LFXO CNT tick to usec (1/(32768/2) = 0.000061 sec) */
#define ULFRCO_TICK_TO_USEC (1000) /* Convert ULFRXO CNT tick to usec (1/1000 = 0.001 sec) */

#define USEC_TO_WAIT (1000000)
/*---------------------------------------------------*/
/* Global and Private Variables */
uint32_t g_msecRolloverCounter; /* Counter used to track timer rollover events for timerGetRunTimeMilliseconds() */

SLEEP_EnergyMode_t currentEnergyMode;
/*---------------------------------------------------*/
void timerInit() {
  /* Define LETIMER_TypDef and disable timer */
  LETIMER_Init_TypeDef timerInit = {0};

  /* Initialize LETIMER0 timer */
  LETIMER_Init(LETIMER0, &timerInit);

  /* Enable Interrupts for even and odd numbered number GPIO pins, clearing interrupt before enabling */
  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);
  NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
  NVIC_EnableIRQ(GPIO_ODD_IRQn);

  /* Enable Timer */
  LETIMER_Enable(LETIMER0, true);
}

/*---------------------------------------------------*/
void timerDisable() {
  NVIC_DisableIRQ(LETIMER0_IRQn);
  LETIMER_IntDisable(LETIMER0, LETIMER_IEN_COMP0);
  LETIMER_Enable(LETIMER0, false);
}

/*---------------------------------------------------*/
void timerSetCurrentEnergyMode(SLEEP_EnergyMode_t mode) {
  currentEnergyMode = mode;
}

/*---------------------------------------------------*/
void timerWaitUs(uint32_t us_wait) {
  bool delay = true;
  uint32_t currentCount = 0;
  uint32_t delayCntTicks = 0;
  uint32_t rolloverCount = 0;
  uint32_t numTicks = 0;
  uint32_t numRollover = 0;
  uint32_t comp0MaxValue = 0;

  /* Get value of LETIMER COMP0 Register */
  comp0MaxValue = LETIMER_CompareGet(LETIMER0, 0);

  /* Calculate number of ticks to delay based on CLK in use and total delay time, rounded up */
  if(currentEnergyMode > sleepEM3){
    numTicks = (uint32_t)ceil((us_wait) / LFXO_TICK_TO_USEC);
  } else {
    numTicks = (uint32_t)ceil((us_wait) / ULFRCO_TICK_TO_USEC);
  }

  /* Capture current LETIMER counter value */
  currentCount = LETIMER_CounterGet(LETIMER0);

  // Determine CNT value to delay, accounting for COUNT rollovers as well
  if(numTicks > currentCount) {
    rolloverCount = ((numTicks - currentCount) / comp0MaxValue);
    rolloverCount += 1;

    delayCntTicks = (comp0MaxValue - ((numTicks % comp0MaxValue ) - currentCount));
  } else {
    delayCntTicks = currentCount - numTicks;
  }

  // Determine counts to wait based on clock selected
  while(delay) {
    currentCount = LETIMER_CounterGet(LETIMER0);

    if ( numRollover != rolloverCount) {
      if( currentCount == comp0MaxValue ) {
        numRollover++;
      }
    } else {
      if( delayCntTicks == currentCount ){
        delay  = false;
      }
    }
  }

  return;
}

/*---------------------------------------------------*/
void timerSetEventInMs(uint32_t ms_until_wakeup) {
  uint32_t compareValue = 0;
  uint32_t currTimerCount = LETIMER_CounterGet(LETIMER0);

  if(ms_until_wakeup <= currTimerCount) {
    compareValue = (currTimerCount - ms_until_wakeup);
  } else {
    ms_until_wakeup -= currTimerCount;
    compareValue = TEMP_SAMPLE_PERIOD_MSEC - ms_until_wakeup;
  }

  LETIMER_CompareSet(LETIMER0, 1, compareValue);
  LETIMER_IntClear(LETIMER0, LETIMER_IEN_COMP1);
  LETIMER_IntEnable(LETIMER0, LETIMER_IEN_COMP1);
}

/*---------------------------------------------------*/
uint32_t timerGetRunTimeMilliseconds() {
  uint32_t msecTime = 0;

  /* Calculate number seconds based on rollover count and current timer count */
  msecTime = TEMP_SAMPLE_PERIOD_MSEC - LETIMER_CounterGet(LETIMER0);
  msecTime += (g_msecRolloverCounter * TEMP_SAMPLE_PERIOD_MSEC);

  return msecTime;
}
/*---------------------------------------------------*/
void timer0EM3BlockStart() {
  CORE_DECLARE_IRQ_STATE; /* Used to enable CRITICAL SECTION methods */
  CORE_ENTER_CRITICAL();
  SLEEP_SleepBlockBegin(sleepEM3);
  CORE_EXIT_CRITICAL();
}
/*---------------------------------------------------*/
void timer0EM3BlockEnd() {
  CORE_DECLARE_IRQ_STATE; /* Used to enable CRITICAL SECTION methods */
  CORE_ENTER_CRITICAL();
  SLEEP_SleepBlockEnd(sleepEM3);
  CORE_EXIT_CRITICAL();
}
/*---------------------------------------------------*/
/*
 * GPIO_EVEN interrupt handler. Utilized to enable the g_eventProcessingBitmask
 * global variable to be processed from the main() application while-loop.
 */
void GPIO_EVEN_IRQHandler(void) {
  CORE_DECLARE_IRQ_STATE; /* Used to enable CRITICAL SECTION methods */
  uint32_t intFlag = 0;

  CORE_ENTER_CRITICAL();
  intFlag = GPIO_IntGet();

  if((intFlag & (1 << PB0_pin)) > 0){
    /* Clear interrupt triggered */
    GPIO_IntClear(intFlag);

    /* Send signal that push button interrupt occurred */
    gecko_external_signal(PB0_BUTTON_PUSH_BITMASK);
  } else if((intFlag & (1 << PIR_pin)) > 0){
    /* Clear interrupt triggered */
    GPIO_IntClear(intFlag);

    /* Send signal that push button interrupt occurred */
    gecko_external_signal(PIR_SENSOR_BITMASK);
  }

  CORE_EXIT_CRITICAL();
}
/*---------------------------------------------------*/
/*
 * GPIO_ODD interrupt handler. Utilized to enable the g_eventProcessingBitmask
 * global variable to be processed from the main() application while-loop.
 */
void GPIO_ODD_IRQHandler(void) {
  CORE_DECLARE_IRQ_STATE; /* Used to enable CRITICAL SECTION methods */
  uint32_t intFlag = 0;

  CORE_ENTER_CRITICAL();
  intFlag = GPIO_IntGet();

  if((intFlag & (1 << PB1_pin)) > 0){
    /* Clear interrupt triggered */
    GPIO_IntClear(intFlag);

    /* Send signal that push button interrupt occurred */
    gecko_external_signal(PB1_BUTTON_PUSH_BITMASK);
  }

  CORE_EXIT_CRITICAL();
}
