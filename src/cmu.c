/*
 * cmu.c
 *
 *  Created on: Jan 26, 2020
 *      Author: Brian I.
 */
/*-----------------------------------*/
#include "cmu.h"
#include <stdbool.h>

/*-----------------------------------*/
void cmuEnable(SLEEP_EnergyMode_t mode) {
  /* Variables set based on EM selected */
  CMU_Osc_TypeDef oscillator;
  CMU_Select_TypeDef clockSelect;
  CMU_ClkDiv_TypeDef clkDiv;

  if((mode == sleepEM0) || (mode == sleepEM1) || (mode == sleepEM2)){
    oscillator = cmuOsc_LFXO; /* 32768 Hz */
    clockSelect = cmuSelect_LFXO;
    clkDiv = LFXO_PRESCALER_DIV;
  } else if(mode == sleepEM3){
    oscillator = cmuOsc_ULFRCO; /* 1000 Hz */
    clockSelect = cmuSelect_ULFRCO;
    clkDiv = cmuClkDiv_1;
  }

  /* Enable Oscillator */
  CMU_OscillatorEnable(oscillator, true, false);

  /* Select source of LFACLK */
  CMU_ClockSelectSet(cmuClock_LFA, clockSelect);

  /* Enable Clock switch */
  CMU_ClockEnable(CMU_LFACLKSEL_REG, true);

  /* Set prescalar value */
  CMU_ClockDivSet(cmuClock_LETIMER0, clkDiv);

  /* Enable Clock for LETIMER0 */
  CMU_ClockEnable(cmuClock_LETIMER0, true);

  return;
}
