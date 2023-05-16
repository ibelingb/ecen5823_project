/*
 * serial_flash.c
 *
 *  Created on: Apr 20, 2020
 *      Author: brian
 */

#include "serial_flash.h"
#include "native_gecko.h"
#include "gecko_ble_errors.h"
#include "em_core.h"

#define KEY_MIN_VALUE (0x4000)
#define KEY_MAX_VALUE (0x4700)

/*---------------------------------------------------*/
uint8_t saveFlashData(uint16_t key, uint8_t len, uint8_t* data) {
  CORE_DECLARE_IRQ_STATE; /* Used to enable CRITICAL SECTION methods */

  /* Confirm input variables are valid */
  if(data == NULL){
    LOG_ERROR("Pointer to application variable to save data for saveFlashData() is NULL\n");
    return (-1);
  }
  if((key < KEY_MIN_VALUE) || (key > KEY_MAX_VALUE)){
    LOG_ERROR("User specified key to save data to serial flash memory is out of range, should be [0x4000 - 0x4700]\n");
    return (-2);
  }

  /* Save data to flash memory */
  CORE_ENTER_CRITICAL();
  BTSTACK_CHECK_RESPONSE(gecko_cmd_flash_ps_save(key, len, data));
  CORE_EXIT_CRITICAL();

  return 0;
}

/*---------------------------------------------------*/
uint8_t loadFlashData(uint16_t key, uint8_t* data){
  CORE_DECLARE_IRQ_STATE; /* Used to enable CRITICAL SECTION methods */
  struct gecko_msg_flash_ps_load_rsp_t *loadResult;

  /* Confirm input variables are valid */
  if(data == NULL){
    LOG_ERROR("Pointer to application variable to load data from loadFlashData() is NULL");
    return (-1);
  }
  if((key < KEY_MIN_VALUE) || (key > KEY_MAX_VALUE)){
    LOG_ERROR("User specified key to load data from serial flash memory is out of range, should be [0x4000 - 0x4700]\n");
    return (-2);
  }

  /* Load flash data into application */
  CORE_ENTER_CRITICAL();
  loadResult = gecko_cmd_flash_ps_load(key);
  CORE_EXIT_CRITICAL();

  if(loadResult->result == 0) {
    memcpy(data, loadResult->value.data, loadResult->value.len);
  } else {
    LOG_ERROR("Failed to load data from serial flash based on key value {%d}\n", key);
    return -1;
  }

  return 0;
}

/*---------------------------------------------------*/
