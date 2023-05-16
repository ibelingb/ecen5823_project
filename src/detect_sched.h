/*
 * detect_sched.h
 *
 * Methods to configure and check if Motion Detected events are within the schedule set by the user. Methods include
 * ways to set the start and stop hour detection and what string to display on the LCD screen.
 *
 *  Created on: Apr 25, 2020
 *      Author: brian
 */

#ifndef SRC_DETECT_SCHED_H_
#define SRC_DETECT_SCHED_H_

/*---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

#define TIMESTAMP_LENGTH (10)
#define SCHED_STR_LENGTH (8)
/*---------------------------------------------------*/
/* Struct to store Motion Detect Schedule Config data to Flash Memory */
struct MotionDetectConfig {
  uint8_t motionDetectStartTime; /* Hour of day to begin detecting motion [0-23] */
  uint8_t motionDetectStopTime; /* Hour of day to stop detecting motion [0-23] */
} motionDetectConfig;
/* Struct to store Time of last motion detect event to Flash Memory */
struct LastMotionEventTime {
  char timeStr[TIMESTAMP_LENGTH];
} lastMotionEventTime;

bool gLogMotionEventsEnabled;
char configSchedStr[SCHED_STR_LENGTH];

/*---------------------------------------------------*/
/*
 * Method to determine if motion detect event occurred within user set hour range
 * @return - bool of true/false if event in schedule range (hour increments)
 */
bool isEventInSched();

/*
 * Set Detection Schedule Config start time in hour segments
 *
 * @param schedStartTimeHour - Hour of day to detect motion events [0-23]
 * @return - Status, success (0) or failure (-)
 */
uint8_t setDetectionSchedStartTime(uint8_t schedStartTimeHour);

/*
 * Set Detection Schedule Config stop time in hour segments
 *
 * @param schedStartTimeHour - Hour of day to detect motion events [0-23]
 * @return - Status, success (0) or failure (-)
 */
uint8_t setDetectionSchedStopTime(uint8_t schedStopTimeHour);

/*
 * Set Schedule Config string for LCD screen and client service
 */
void setSchedString();
/*---------------------------------------------------*/
#endif /* SRC_DETECT_SCHED_H_ */
