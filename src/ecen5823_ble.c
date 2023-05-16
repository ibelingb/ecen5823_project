/*
 * ecen5823_ble.c
 *
 *  Created on: Feb 23, 2020
 *      Author: brian
 */
/*---------------------------------------------------*/
#include "math.h"

#include "infrastructure.h"
#include "bg_types.h"
#include "gatt_db.h"
#include "em_emu.h"
#include "gecko_ble_errors.h"

#include "ecen5823_ble.h"
#include "ble_device_type.h"
#include "gpio.h"
#include "timer.h"
#include "display.h"
#include "serial_flash.h"
#include "clock.h"
#include "detect_sched.h"
/*---------------------------------------------------*/
// Server Connection Parameters
#define CONN_INTERVAL_MIN   (400) // 250msec
#define CONN_INTERVAL_MAX   (400) // 250msec
#define CONN_DURATION       (0)   // No Advertising duration limit
#define CONN_MAXEVENTS      (0)   // Max Advertising Event Limit

/*---------------------------------------------------*/
uint8_t connectionHandle = 0;

/*---------------------------------------------------*/
/* Function Prototypes */
void lastMotionDetectedNotification();
void motionDetectSchedNotification();
void advertiseServer();
void sendPeriodicNotifications();
/*---------------------------------------------------*/
void gecko_ecen5823_update(struct gecko_cmd_packet* evt) {
  static uint8_t dfuBoot = 0; /* Flag to trigger DFU reset */

  /* Handle event */
  switch BGLIB_MSG_ID(evt->header) {
    case(gecko_evt_system_boot_id):
      setDeviceName(BLE_DEVICE_TYPE_STRING);

      /* Delete all previous bonding states */
      BTSTACK_CHECK_RESPONSE(gecko_cmd_sm_delete_bondings());

      /* Set device into bondable mode */
      BTSTACK_CHECK_RESPONSE(gecko_cmd_sm_set_bondable_mode(true));

      /* Configure new bonding - Require bonding requests be confirm + requires MITM protection (0x5) and to use DisplayYesNo auth confirm method */
      BTSTACK_CHECK_RESPONSE(gecko_cmd_sm_configure(0x5, sm_io_capability_displayyesno));

      advertiseServer();

      /* Update Device connection state */
      setDeviceConnState(CONNECTION_ADVERTISING);

      break;
    /* -------------------------------------------------------- */
    case(gecko_evt_le_connection_opened_id):
      connectionHandle = evt->data.evt_le_connection_opened.connection;
      setDeviceConnState(CONNECTION_ESTABLISHED);

      /* Set BLE connection internal min/max values to be 75msec
       * Additionally, set slave latency to 300 msec (4 intervals of 75msec each)
       * Set timeout based on documented calculation value */
      BTSTACK_CHECK_RESPONSE(gecko_cmd_le_connection_set_parameters(connectionHandle, 60, 60, 3, 1000));
      break;
    /* -------------------------------------------------------- */
    case(gecko_evt_le_connection_phy_status_id):
      /* Capture connection Handle value now that BLE connection complete */
      connectionHandle = evt->data.evt_le_connection_parameters.connection;

      /* Update Device connection state */
      setDeviceConnState(CONNECTION_ESTABLISHED);

      /* Initialize timer to sample temperature now that BLE connection complete */
      timerInit();
      break;
    /* -------------------------------------------------------- */
    case(gecko_evt_sm_confirm_bonding_id):
      connectionHandle = evt->data.evt_sm_confirm_bonding.connection;

      /* Check if bonding handle already exists - do not overwrite if true */
      if(evt->data.evt_sm_confirm_bonding.bonding_handle == -1){
        /* Accept bonding request */
        BTSTACK_CHECK_RESPONSE(gecko_cmd_sm_bonding_confirm(connectionHandle, true));
        setDeviceConnState(CONNECTION_BONDING);
      } else {
        LOG_INFO("Bonding Handle of %d returned - bonding already exists for this connection. Not overwriting. \n", evt->data.evt_sm_confirm_bonding.bonding_handle);
      }

      break;
    /* -------------------------------------------------------- */
    case(gecko_evt_sm_confirm_passkey_id):
      /* Display passkey on LCD */
      setPasskeyValue(evt->data.evt_sm_confirm_passkey.passkey);

      setDeviceConnState(CONNECTION_BONDING);
      break;
    /* -------------------------------------------------------- */
    case(gecko_evt_sm_bonded_id):
      setDeviceConnState(CONNECTION_BONDED);
      break;
    /* -------------------------------------------------------- */
    case(gecko_evt_sm_bonding_failed_id):
      /* Not sure if any handling needs to be done in this event at the moment, will need to revisit */
      /* For Server, noticed this event is triggered while awaiting to confirm passkey via button press; Might want to add a timeout here if no user response received */
      /* For Client, might want to restart discovery process */
      setDeviceConnState(CONNECTION_BONDING_FAILED);

      advertiseServer();
      /* Update Device connection state */
      setDeviceConnState(CONNECTION_ADVERTISING);
      break;
    /* -------------------------------------------------------- */
    case(gecko_evt_le_connection_closed_id):
      /* Determine if OTA is in progress */
      if (dfuBoot) {
        //gecko_cmd_hardware_set_soft_timer(TIMER_STOP, TEMP_TIMER, true); /* Initially stop the timer. */

        /* Reset to DFU OTA mode */
        gecko_cmd_system_reset(2);
      } else {
        /* Update Device connection state */
        setDeviceConnState(CONNECTION_LOST);

        /* Delete all existing bonding states */
        BTSTACK_CHECK_RESPONSE(gecko_cmd_sm_delete_bondings());

        timerDisable(); /* Disable LETIMER0, which will be re-enabled once BLE connection made */

        /* BLE connection lost - Attempt reconnection and re-initiate advertising */
        /* Unable to get client to reconnect with advertising on connection_closed event - needing to reset system to re-init connection sequence. */
        advertiseServer();
        setDeviceConnState(CONNECTION_ADVERTISING);

        /* Reset in order to get Server to reconnect on client disconnect */
        gecko_cmd_system_reset(0);
      }
      break;
    /* -------------------------------------------------------- */
    case(gecko_evt_le_gap_adv_timeout_id):
      /* Update Device connection state */
      setDeviceConnState(CONNECTION_ADV_TIMEOUT);

      /* Delete all existing bonding states */
      BTSTACK_CHECK_RESPONSE(gecko_cmd_sm_delete_bondings());

      /* Advertising timeout reached - Restart advertising */
      advertiseServer();
      break;
    /* -------------------------------------------------------- */
    case(gecko_evt_gatt_server_user_write_request_id):
      connectionHandle = evt->data.evt_gatt_server_user_write_request.connection;

      /* Handle application updates via OTA */
      /* Code in section below leveraged example code provided from Silicon Labs soc-thermometer project */
      if (evt->data.evt_gatt_server_user_write_request.characteristic == gattdb_ota_control) {
        /* Set flag to enter to OTA mode */
        dfuBoot = 1;
        /* Send response to Write Request */
        gecko_cmd_gatt_server_send_user_write_response(
          evt->data.evt_gatt_server_user_write_request.connection,
          gattdb_ota_control,
          bg_err_success);
        /* Close connection to enter to DFU OTA mode */
        gecko_cmd_le_connection_close(evt->data.evt_gatt_server_user_write_request.connection);

      /* Handle Config Schedule update events - Determine which characteristic passed this value */
      /* In either case, read the value passed by the user and save to Detection Sched flash memory, update LCD, and send notification */
      } else if(evt->data.evt_gatt_server_user_write_request.characteristic == gattdb_schedule_start_time) {
        /* Confirm data is provided before reading data array */
        if(evt->data.evt_gatt_server_user_write_request.value.len >= 1){
          if(setDetectionSchedStartTime(evt->data.evt_gatt_server_user_write_request.value.data[0]) == 0) {
            gecko_cmd_gatt_server_send_user_write_response(connectionHandle, evt->data.evt_gatt_server_user_write_request.characteristic, 0); /* Successful Write GATT Response */
          } else {
            /* Invalid data received for setting Schedule Start time - report write failure */
            LOG_ERROR("Invalid value provided when trying to set Detection Schedule Start Time (hr). Must be [0-23].\n");
            gecko_cmd_gatt_server_send_user_write_response(connectionHandle, evt->data.evt_gatt_server_user_write_request.characteristic, (uint8_t)bg_err_att_value_not_allowed);
          }
        } else {
          /* No data provided with write request - Write request failed */
          LOG_ERROR("No value provided when trying to set Detection Schedule Start Time (hr)\n");
          gecko_cmd_gatt_server_send_user_write_response(connectionHandle, evt->data.evt_gatt_server_user_write_request.characteristic, (uint8_t)bg_err_att_request_not_supported);
        }
      } else if(evt->data.evt_gatt_server_user_write_request.characteristic == gattdb_schedule_stop_time) {
        /* Confirm data is provided before reading data array */
        if(evt->data.evt_gatt_server_user_write_request.value.len >= 1){
          if(setDetectionSchedStopTime(evt->data.evt_gatt_server_user_write_request.value.data[0]) == 0) {
            gecko_cmd_gatt_server_send_user_write_response(connectionHandle, evt->data.evt_gatt_server_user_write_request.characteristic, 0); /* Successful Write GATT Response */
          } else {
            /* Invalid data received for setting Schedule Start time - report write failure */
            LOG_ERROR("Invalid value provided when trying to set Detection Schedule Stop Time (hr). Must be [0-23].\n");
            gecko_cmd_gatt_server_send_user_write_response(connectionHandle, evt->data.evt_gatt_server_user_write_request.characteristic, (uint8_t)bg_err_att_value_not_allowed);
          }
        } else {
          /* No data provided with write request */
          LOG_ERROR("No value provided when trying to set Detection Schedule Stop Time (hr)\n");
          gecko_cmd_gatt_server_send_user_write_response(connectionHandle, evt->data.evt_gatt_server_user_write_request.characteristic, (uint8_t)bg_err_att_request_not_supported);
        }
      }

      break;
    /* -------------------------------------------------------- */
    case(gecko_evt_hardware_soft_timer_id):
      /* Update LCD and read current time */
      displayTextToLcd();
      displayUpdate();

      /* Turn off "camera" */
      gpioLed0SetOff();

      /* Send notifications periodically, rotating between them. */
      sendPeriodicNotifications();

      break;
    /* -------------------------------------------------------- */
    case(gecko_evt_system_external_signal_id):
      /* Handle external (user) event signals */
      if((evt->data.evt_system_external_signal.extsignals) & (PB0_BUTTON_PUSH_BITMASK)) {
        /* On Push Button 0 press, send confirmation for bonding process that DisplayYesNo value is valid */
        BTSTACK_CHECK_RESPONSE(gecko_cmd_sm_passkey_confirm(connectionHandle, true));
      } else if((evt->data.evt_system_external_signal.extsignals) & (PIR_SENSOR_BITMASK)) {

        /* If detect schedule in use, determine if event happened to valid window */
        if(isEventInSched()) {
          /* Motion detected, capture current time and turn on camera (LED) */
          getCurrentTimeStr(lastMotionEventTime.timeStr);
          gpioLed0SetOn(); /* Turn on "camera" */

          /* Log event (if enabled) */
          if(gLogMotionEventsEnabled) {
            saveFlashData(LAST_MOTION_EVENT_KEY, sizeof(lastMotionEventTime), (uint8_t*)&lastMotionEventTime);
          }
        }
      }

      break;

    /* -------------------------------------------------------- */
    default:
      break;
  }
}

/*---------------------------------------------------*/
/*
 * Method to convert timestamp of last motion detected char array into bitstream for characteristic notification
 */
void lastMotionDetectedNotification(){
  uint8_t htmTimestampBuffer[TIMESTAMP_LENGTH];
  uint8_t *p = htmTimestampBuffer;

  for(int i=0; i<TIMESTAMP_LENGTH; i++){
    UINT8_TO_BITSTREAM(p, (uint8_t)lastMotionEventTime.timeStr[i]);
  }

  BTSTACK_CHECK_RESPONSE(gecko_cmd_gatt_server_send_characteristic_notification(0xFF, gattdb_motion_evt_time_str, TIMESTAMP_LENGTH, htmTimestampBuffer));
}
/*---------------------------------------------------*/
/*
 * Method to convert motion detection schedule char array into bitstream for characteristic notification
 */
void motionDetectSchedNotification(){
  uint8_t htmSchedBuffer[SCHED_STR_LENGTH];
  uint8_t *p = htmSchedBuffer;

  for(int i=0; i<SCHED_STR_LENGTH; i++){
    UINT8_TO_BITSTREAM(p, (uint8_t)configSchedStr[i]);
  }

  BTSTACK_CHECK_RESPONSE(gecko_cmd_gatt_server_send_characteristic_notification(0xFF, gattdb_detection_sched, SCHED_STR_LENGTH, htmSchedBuffer));
}

/*---------------------------------------------------*/
/*
 * Method to cycle sending of notifications to ensure client device(s) are regularly updated
 */
void sendPeriodicNotifications(){
  static uint8_t cycleCounter = 0;

  if(cycleCounter % 2 == 0) {
    motionDetectSchedNotification();
  } else {
    lastMotionDetectedNotification();
  }

  cycleCounter++;
}
/*---------------------------------------------------*/
/*
 * Method to configure server advertising parameters and begin advertising for client connections
 */
void advertiseServer() {
  /* Set advertising min/max value to be 250msec -> ((input value 400) * 0.625msec = 250msec)
   * Set all other values to default value of 0 */
  BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_set_advertise_timing(connectionHandle, CONN_INTERVAL_MIN, CONN_INTERVAL_MAX, CONN_DURATION, CONN_MAXEVENTS));

  /* Start general advertising and enable connections. */
  BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_start_advertising(connectionHandle, le_gap_general_discoverable, le_gap_connectable_scannable));
}
/*---------------------------------------------------*/
float gattUint32ToFloat(const uint8_t *value_start_little_endian)
{
  int8_t exponent = (int8_t)value_start_little_endian[3];
  uint32_t mantissa = value_start_little_endian[0] +
            (((uint32_t)value_start_little_endian[1]) << 8) +
            (((uint32_t)value_start_little_endian[2]) << 16);
  return (float)mantissa*pow(10,exponent);
}
/*---------------------------------------------------*/

