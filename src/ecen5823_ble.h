/*
 * ecen5823_ble.c
 *
 * This file provides the BLE Event State machine for all BLE events received between server and client.
 * Added to this was the event handler for I2C and delay events for the user application.
 * This utilizes the gecko_evt_system_external_signal_id and available external signal bitmask to trigger these.
 *
 *  Created on: Feb 23, 2020
 *      Author: brian
 *
 * Notes and references were used from the following sources:
 *  - Used soc-thermometer SI Labs example project as an example
 *  - https://www.silabs.com/community/wireless/bluetooth/knowledge-base.entry.html/2016/10/04/scheduling_applicati-ERXS
 *  - Used soc-thermometer-client SI Labs example project as a reference
 *  - https://www.silabs.com/community/wireless/bluetooth/knowledge-base.entry.html/2017/08/25/pairing_processes-8XpB
 *  - https://docs.silabs.com/bluetooth/latest/sm
 *  - https://www.silabs.com/documents/public/reference-manuals/wgm110-api-rm.pdf
 *  - https://www.silabs.com/community/wireless/bluetooth/forum.topic.html/assign_a_characteris-agkS
 *  - https://www.silabs.com/community/wireless/bluetooth/knowledge-base.entry.html/2018/05/30/ble_basics_masters-i4n9
 */
/*---------------------------------------------------*/
#ifndef SRC_ECEN5823_BLE_H_
#define SRC_ECEN5823_BLE_H_

/*---------------------------------------------------*/
#include "native_gecko.h"

/*---------------------------------------------------*/
#define SCHEDULER_SUPPORTS_DISPLAY_UPDATE_EVENT (1)
#define TIMER_SUPPORTS_1HZ_TIMER_EVENT          (1)

/*---------------------------------------------------*/
typedef enum connectionState {
  CONNECTION_ADVERTISING = 0,
  CONNECTION_DISCOVERING,
  CONNECTION_ESTABLISHED,
  CONNECTION_LOST,
  CONNECTION_ADV_TIMEOUT,
  HANDLE_INDICATIONS,
  CONNECTION_BONDING,
  CONNECTION_BONDED,
  CONNECTION_BONDING_FAILED
}connectionState_t;

/*---------------------------------------------------*/
/*
 * Primary logic handling for packets/event/data received from BLE device.
 * Also handles external (user) events triggered from I2C and timer interrupts/events.
 *  Details for this are found under gecko_evt_system_external_signal_id().
 *
 * @param evt - data struct returned from Bluetooth event
 */
void gecko_ecen5823_update(struct gecko_cmd_packet* evt);

/*---------------------------------------------------*/
/**
 * Method to convert received FP data from server to client
 *
 * @return a float value based on a UINT32 value written by FLT_TO_UINT32 and
 * UINT32_TO_BITSTREAM
 * @param value_start_little_endian is a pointer to the first byte of the float
 * which is represented in UINT32 format from FLT_TO_UINT32/UINT32_TO_BITSTREAM
 */
float gattUint32ToFloat(const uint8_t *value_start_little_endian);
/*---------------------------------------------------*/

#endif /* SRC_ECEN5823_BLE_H_ */
