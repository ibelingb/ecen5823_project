/*
 * main.c
 *
 *  Main application for ECEN 5823 HW assignments. Completes SI Labs Blue Gecko
 *  HW/peripheral setup and initialization before enter while-loop to handle interrupt
 *  and main app events.
 *
 *  Created on: Jan 26, 2020
 *      Author: brian
 *
 *  Resources Used:
 *    - https://www.silabs.com/documents/public/reference-manuals/efr32xg1-rm.pdf
 *    - https://www.silabs.com/documents/public/data-sheets/efr32bg13-datasheet.pdf
 *    - https://siliconlabs.github.io/Gecko_SDK_Doc/efm32g/html/index.html
 *    - an0007.1-efr32-efm32-series-1-energymodes.pdf
 */

/*---------------------------------------------------*/
#include "gecko_configuration.h"
#include "gpio.h"
#include "native_gecko.h"
#include "sleep.h"
#include "em_emu.h"
#include "em_core.h"
#include "em_i2c.h"
#include "log.h"

#include "cmu.h"
#include "timer.h"
#include "ecen5823_ble.h"
#include "display.h"
#include "ble_device_type.h"
#include "serial_flash.h"
#include "clock.h"
#include "detect_sched.h"

/*---------------------------------------------------*/
/* Set this variable to enable/disable logging of motion events to serial flash NVM */
#define ENABLE_MOTION_EVENT_LOGGING (1) /* 0=Disable | 1=Enable */

/* Variable used to set the energy mode for application, with peripherals enabled accordingly */
const SLEEP_EnergyMode_t energyMode = sleepEM3;
/* Variable used to determine if motion events should be logged */
extern bool gLogMotionEventsEnabled;

/*---------------------------------------------------*/
/*
 * Completes SI Labs Blue Gecko HW/peripheral setup and initialization
 * before enter while-loop for main application processing loop. If logging
 * is enabled, logs will be output via serial port.
 *
 * @param config - HW configuration received from gecko_main.c
 */
int appMain(gecko_configuration_t *config) {
  struct gecko_cmd_packet* evt;
  gLogMotionEventsEnabled = ENABLE_MOTION_EVENT_LOGGING;

  /* Initialize Logging */
  logInit();

  /* Initialize stack */
  gecko_init(config);

  /* Enable system and peripheral clocks */
  cmuEnable(energyMode);

  /* Initialize timer and interrupt handlers */
  timerInit();

  /* Initialize peripherals */
  gpioInit();
  timerSetCurrentEnergyMode(energyMode);

  /* Initialize LCD Display */
  displayInit();

  /* Load serial flash variables from memory */
  loadFlashData(MOTION_DETECT_SCHED_KEY, (uint8_t*)&motionDetectConfig);
  if(gLogMotionEventsEnabled){
    loadFlashData(LAST_MOTION_EVENT_KEY, (uint8_t*)&lastMotionEventTime);
  }

  /* Set Schedule Config string */
  setSchedString();

  /* Initialize Clock for reading date/time */
  clockInit(energyMode);

  /* While loop waiting for Bluetooth events to trigger */
  while(1) {
    /* Bluetooth to enter sleep mode and wait for user and Bluetooth events to be received and handled */
    evt = gecko_wait_event();
    gecko_ecen5823_update(evt);
  }

  return (0);
}
