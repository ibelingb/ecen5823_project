/*
 * serial_flash.h
 *
 *  Created on: Apr 20, 2020
 *      Author: brian
 *
 * Notes and references were used from the following sources:
 *  - https://docs.silabs.com/bluetooth/latest/flash#cmd_flash_ps_save
 *  - https://www.silabs.com/documents/public/reference-manuals/bluetooth-api-reference.pdf
 *  - https://www.silabs.com/community/wireless/bluetooth/knowledge-base.entry.html/2017/05/02/how_to_save_floatva-Udm8
 */

#ifndef SERIAL_FLASH_H_
#define SERIAL_FLASH_H_
/*---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

#define MOTION_DETECT_SCHED_KEY (0x4000)
#define LAST_MOTION_EVENT_KEY (0x4001)

/*---------------------------------------------------*/
/*
 * Save data of the specified length to serial flash non-volatile memory (NVM) on the Blue Gecko device.
 *
 * @param key - User specified key within range 0x4000 to 0x4700
 * @param len - Length in bytes of data to be saved to flash memory
 * @param data - Pointer to data to be saved to flash memory
 * @return - success/failure
 */
uint8_t saveFlashData(uint16_t key, uint8_t len, uint8_t* data);

/*
 * Load data from serial flash non-volatile memory (NVM) on the Blue Gecko device to an application variable.
 *
 * @param key - User specified key within range 0x4000 to 0x4700
 * @param data - Pointer to application variable to be loaded with data stored within flash memory
 * @return - success/failure
 */
uint8_t loadFlashData(uint16_t key, uint8_t* data);
/*---------------------------------------------------*/

#endif /* SERIAL_FLASH_H_ */
