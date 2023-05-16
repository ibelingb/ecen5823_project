/*
 * ble_device_type.h
 *
 *  Created on: Feb 16, 2019
 *      Author: danwa
 */

#ifndef SRC_BLE_DEVICE_TYPE_H_
#define SRC_BLE_DEVICE_TYPE_H_
#include <stdbool.h>

/**
 * Set to 1 to configure this build as a BLE server.
 * Comment out to configure as a BLE client
 */
#define DEVICE_IS_BLE_SERVER 1

#if DEVICE_IS_BLE_SERVER
#define BUILD_INCLUDES_BLE_SERVER 1
#define BUILD_INCLUDES_BLE_CLIENT 0
#define BLE_DEVICE_TYPE_STRING "Motion Detector"
static inline bool IsServerDevice() { return true; }
static inline bool IsClientDevice() { return false; }
#else
#define BUILD_INCLUDES_BLE_SERVER 0
#define BUILD_INCLUDES_BLE_CLIENT 1
#define BLE_DEVICE_TYPE_STRING "Client"
static inline bool IsClientDevice() { return true;}
static inline bool IsServerDevice() { return false; }
#endif

#endif /* SRC_BLE_DEVICE_TYPE_H_ */
