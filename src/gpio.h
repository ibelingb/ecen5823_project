/*
 * gpio.c
 *
 *  Methods to enable/disable and configure GPIO pins for BG use
 *  with peripherals and external devices
 *
 *  Created on: Dec 12, 2018
 *      Author: Brian I.
 *
 * See the following for more info:
 *   Radio board user guide:
 *      https://www.silabs.com/documents/login/user-guides/ug279-brd4104a-user-guide.pdf
 *   GPIO documentation:
 *      https://siliconlabs.github.io/Gecko_SDK_Doc/efm32g/html/group__GPIO.html
 *   Configure GPIO Push Button with filtering enabled
 *    https://www.silabs.com/community/mcu/32-bit/forum.topic.html/external_interruptb-hxXa
 */
/*---------------------------------------------------*/
#ifndef SRC_GPIO_H_
#define SRC_GPIO_H_

#include <stdbool.h>

#define GPIO_SET_DISPLAY_EXT_COMIN_IMPLEMENTED  (1)
#define GPIO_DISPLAY_SUPPORT_IMPLEMENTED        (1)

/* PB0_pin, PB1_pin, and PIR_pin defined in header to be shared with timer.c */
#define PB0_pin                                 (6)
#define PB1_pin                                 (7)
#define PIR_pin                                 (2)
/*---------------------------------------------------*/
/*
 * Initialize GPIOs for BG on-board LEDs 0 and 1
 */
void gpioInit();

/*
 * Turn LED0 on
 */
void gpioLed0SetOn();

/*
 * Turn LED0 on
 */
void gpioLed0SetOff();

/*
 * Turn LED1 on
 */
void gpioLed1SetOn();

/*
 * Turn LED1 off
 */
void gpioLed1SetOff();

/*
 * Enable DISPLAY GPIO
 */
void gpioEnableDisplay();

/*
 * Disable DISPLAY GPIO
 */
void gpioDisableDisplay();

/*
 * Set EXTCOMIN GPIO
 *
 * @param high - Set EXTCOMIN GPIO pin high or low based on input value
 */
void gpioSetDisplayExtcomin(bool high);

/*
 * Return current value from PIR Sensor
 * @return - Alarm value from PIR
 */
uint8_t gpioGetPIRState();

#endif /* SRC_GPIO_H_ */
