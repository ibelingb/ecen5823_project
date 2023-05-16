/*
 * gpio.c
 *
 *  Created on: Feb 1, 2020
 *      Author: Brian I.
 */
/*---------------------------------------------------*/
#include <string.h>

#include "em_core.h"
#include "em_gpio.h"
#include "gpio.h"

/*---------------------------------------------------*/
#define	LED0_port     (gpioPortF)
#define LED0_pin      (4)
#define LED1_port     (gpioPortF)
#define LED1_pin      (5)
#define DISPLAY_port  (gpioPortD)
#define DISPLAY_pin   (15)
#define EXTCOMIN_port (gpioPortD)
#define EXTCOMIN_pin  (13)
#define PB0_port      (gpioPortF)
/* PB0_pin defined in header to be shared with timer.c */
#define PB1_port     (gpioPortF)
/* PB1_pin defined in header to be shared with timer.c */
#define PIR_port      (gpioPortA)
/* PIR_pin defined in header to be shared with timer.c */

uint8_t pLed0On;

/*---------------------------------------------------*/
void gpioInit() {
  /* Set LEDs */
	GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthStrongAlternateStrong);
	GPIO_PinModeSet(LED0_port, LED0_pin, gpioModePushPull, false);
	GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthStrongAlternateStrong);
	GPIO_PinModeSet(LED1_port, LED1_pin, gpioModePushPull, false);
	/* Set PB0 and PB1 Push Button with filtering and interrupt enabled */
  GPIO_PinModeSet(PB0_port, PB0_pin, gpioModeInputPullFilter, true);
  GPIO_IntConfig(PB0_port, PB0_pin, false, true, true);
  GPIO_PinModeSet(PB1_port, PB1_pin, gpioModeInputPullFilter, true);
  GPIO_IntConfig(PB1_port, PB1_pin, false, true, true);
  /* Set PIR Sensor with filtering enabled */
  GPIO_PinModeSet(PIR_port, PIR_pin, gpioModeInputPull, true);
  GPIO_IntConfig(PIR_port, PIR_pin, false, true, true);
}
/*---------------------------------------------------*/
void gpioLed0SetOn() {
	GPIO_PinOutSet(LED0_port,LED0_pin);
	pLed0On = 1;
}

/*---------------------------------------------------*/
void gpioLed0SetOff() {
  if(pLed0On) {
    GPIO_PinOutClear(LED0_port,LED0_pin);
  }
}

/*---------------------------------------------------*/
void gpioLed1SetOn() {
	GPIO_PinOutSet(LED1_port,LED1_pin);
}

/*---------------------------------------------------*/
void gpioLed1SetOff() {
	GPIO_PinOutClear(LED1_port,LED1_pin);
}

/*---------------------------------------------------*/
void gpioEnableDisplay() {
  GPIO_PinOutSet(DISPLAY_port,DISPLAY_pin);
}

/*---------------------------------------------------*/
void gpioDisableDisplay() {
  GPIO_PinOutClear(DISPLAY_port,DISPLAY_pin);
}

/*---------------------------------------------------*/
void gpioSetDisplayExtcomin(bool high) {
  if(high)
    GPIO_PinOutSet(EXTCOMIN_port,EXTCOMIN_pin);
  else
    GPIO_PinOutClear(EXTCOMIN_port,EXTCOMIN_pin);
}
/*---------------------------------------------------*/
uint8_t gpioGetPIRState() {
  return ((uint8_t)(GPIO_PinInGet(PIR_port, PIR_pin)));
}
