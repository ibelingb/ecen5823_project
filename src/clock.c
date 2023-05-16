/*
 * clock.c
 *
 *  Created on: Apr 22, 2020
 *      Author: brian
 */
#include "clock.h"

#include <stdio.h>
#include <inttypes.h>

#include "em_rtcc.h"
#include "em_cmu.h"
#include "log.h"
/*---------------------------------------------------*/
void clockInit(SLEEP_EnergyMode_t mode) {
  RTCC_Init_TypeDef clockInit = RTCC_INIT_DEFAULT;
  clockInit.presc = rtccCntPresc_16;
  /* Set to be Calendar mode to update date/time and not counter */
  clockInit.cntMode = rtccCntModeCalendar;

  /* Provide RTCC clock source and init parameters */
  if(mode == sleepEM3) {
    CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);
  } else {
    CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
  }

  /* Enable Clock switch for RTCC */
  CMU_ClockEnable(CMU_LFECLKSEL_REG, true);

  /* Enable Clock for RTCC */
  CMU_ClockEnable(cmuClock_RTCC, true);

  RTCC_Init(&clockInit);
}

/*---------------------------------------------------*/
uint32_t getCurrentHour(){
  /* Read RTCC Time register and pull out current hours uint32 value */
  uint32_t timeReg = RTCC_TimeGet();
  uint32_t hourTens = ((timeReg & _RTCC_TIME_HOURT_MASK) >> _RTCC_TIME_HOURT_SHIFT) * 10;
  uint32_t hourUnit = ((timeReg & _RTCC_TIME_HOURU_MASK) >> _RTCC_TIME_HOURU_SHIFT);
  return (hourTens + hourUnit);
}
/*---------------------------------------------------*/
void getCurrentHourStr(char* hourString){
  /* Check for valid input */
  if(hourString == NULL){
    LOG_ERROR("NULL pointer passed to getCurrentHourStr()\n");
    return;
  }

  /* Read RTCC Time register and pull out current hours uint32 value  as string */
  uint32_t timeReg = RTCC_TimeGet();
  uint32_t hourTens = ((timeReg & _RTCC_TIME_HOURT_MASK) >> _RTCC_TIME_HOURT_SHIFT)  * 10;
  uint32_t hourUnit = ((timeReg & _RTCC_TIME_HOURU_MASK) >> _RTCC_TIME_HOURU_SHIFT);
  sprintf(hourString, "%"PRIu32, (hourTens + hourUnit));
}
/*---------------------------------------------------*/
void getCurrentTimeStr(char* timeString){
  /* Check for valid input */
  if(timeString == NULL){
    LOG_ERROR("NULL pointer passed to getCurrentTimeStr()\n");
    return;
  }

  /* Read RTCC Time register and pull out current HH:MM:SS uint32 values as string */
  uint32_t timeReg = RTCC_TimeGet();
  uint32_t hourTens = ((timeReg & _RTCC_TIME_HOURT_MASK) >> _RTCC_TIME_HOURT_SHIFT) * 10;
  uint32_t hourUnit = ((timeReg & _RTCC_TIME_HOURU_MASK) >> _RTCC_TIME_HOURU_SHIFT);
  uint32_t minTens  = ((timeReg & _RTCC_TIME_MINT_MASK) >> _RTCC_TIME_MINT_SHIFT) * 10;
  uint32_t minUnit  = ((timeReg & _RTCC_TIME_MINU_MASK) >> _RTCC_TIME_MINU_SHIFT);
  uint32_t secTens  = ((timeReg & _RTCC_TIME_SECT_MASK) >> _RTCC_TIME_SECT_SHIFT) * 10;
  uint32_t secUnit  = ((timeReg & _RTCC_TIME_SECU_MASK) >> _RTCC_TIME_SECU_SHIFT);
  sprintf(timeString, "%" PRIu32 ":%" PRIu32 ":%" PRIu32 , (hourTens + hourUnit), (minTens + minUnit), (secTens + secUnit));
}
/*---------------------------------------------------*/
