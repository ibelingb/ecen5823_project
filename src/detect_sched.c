/*
 * detect_sched.c
 *
 *  Created on: Apr 25, 2020
 *      Author: brian
 */

#include "detect_sched.h"
#include "stdio.h"
#include "em_core.h"

#include "clock.h"
#include "serial_flash.h"

/*---------------------------------------------------*/
bool isEventInSched(){
  uint32_t currHour = getCurrentHour();

  /* Each if-statement below handles a difference case for checking the configured schedule */

  /* Case where start/stop times are 0,0 (all day) */
  if((motionDetectConfig.motionDetectStartTime == 0) && (motionDetectConfig.motionDetectStopTime == 0))
  {
    return true;
  }

  /* Case where startTime is less than stopTime */
  if((motionDetectConfig.motionDetectStartTime < motionDetectConfig.motionDetectStopTime) &&
     ((currHour >= motionDetectConfig.motionDetectStartTime) && (currHour < motionDetectConfig.motionDetectStopTime)))
  {
    return true;
  }

  /* Case where startTime is equal to stopTime */
  if((motionDetectConfig.motionDetectStartTime == motionDetectConfig.motionDetectStopTime) &&
     (currHour == motionDetectConfig.motionDetectStartTime))
  {
    return true;
  }

  /* Case where startTime is greater than stopTime - check should wrap around midnight */
  if((motionDetectConfig.motionDetectStartTime > motionDetectConfig.motionDetectStopTime) &&
      ((currHour >= motionDetectConfig.motionDetectStartTime) || (currHour < motionDetectConfig.motionDetectStopTime)))
  {
    return true;
  }

  /* Case not found, return false */
  return false;
}
/*---------------------------------------------------*/
uint8_t setDetectionSchedStartTime(uint8_t schedStartTimeHour){
  CORE_DECLARE_IRQ_STATE; /* Used to enable CRITICAL SECTION methods */

  /* Verify provided Motion Detect hour is valid [0-23] */
  if((schedStartTimeHour < 0) || (schedStartTimeHour > 23)) {
    return -1;
  }

  /* Save received sched hour to Detection Sched flash memory, update application variable, and update LCD */
  CORE_ENTER_CRITICAL();
  motionDetectConfig.motionDetectStartTime = schedStartTimeHour;
  CORE_EXIT_CRITICAL();
  saveFlashData(MOTION_DETECT_SCHED_KEY, sizeof(motionDetectConfig), (uint8_t*)&motionDetectConfig);
  setSchedString();

  return 0;
}

/*---------------------------------------------------*/
uint8_t setDetectionSchedStopTime(uint8_t schedStopTimeHour){
  CORE_DECLARE_IRQ_STATE; /* Used to enable CRITICAL SECTION methods */

  /* Verify provided Motion Detect hour is valid [0-23] */
  if((schedStopTimeHour < 0) || (schedStopTimeHour > 23)) {
    return -1;
  }

  /* Save received sched hour to Detection Sched flash memory, update application variable, and update LCD */
  CORE_ENTER_CRITICAL();
  motionDetectConfig.motionDetectStopTime = schedStopTimeHour;
  CORE_EXIT_CRITICAL();
  saveFlashData(MOTION_DETECT_SCHED_KEY, sizeof(motionDetectConfig), (uint8_t*)&motionDetectConfig);
  setSchedString();

  return 0;
}
/*---------------------------------------------------*/
void setSchedString(){
  CORE_DECLARE_IRQ_STATE; /* Used to enable CRITICAL SECTION methods */

  CORE_ENTER_CRITICAL();
  uint8_t startTime = motionDetectConfig.motionDetectStartTime;
  uint8_t stopTime = motionDetectConfig.motionDetectStopTime;
  CORE_EXIT_CRITICAL();

  if((startTime == 0) && (stopTime == 0)){
    sprintf(configSchedStr, "All Day");
  } else {
    sprintf(configSchedStr, "%d to %d", startTime, stopTime);
  }
}
/*---------------------------------------------------*/
