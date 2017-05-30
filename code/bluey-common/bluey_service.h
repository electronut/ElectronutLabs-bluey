/**
 * Bluey custom service and characteristic
 *
 * Electronut Labs
 * electronut.in
 *
 */
 
#ifndef OUR_SERVICE_H__
#define OUR_SERVICE_H__

#include <stdint.h>
#include "ble.h"
#include "sdk_common.h"
#include "ble_srv_common.h"


#define BLE_NUS_MAX_DATA_LEN (GATT_MTU_SIZE_DEFAULT - 3) /**< Maximum length of data (in bytes) that can be transmitted to the peer by the Nordic UART service module. */

// FROM_SERVICE_TUTORIAL: Defining 16-bit service and 128-bit base UUIDs
#define BLE_UUID_OUR_BASE_UUID                 {{0x71, 0x26, 0x00, 0x1f, 0x36, 0x92, 0x64, 0xbc, 0xe6, 0x11, 0xde, 0xed, 0x00, 0x35, 0x90, 0x8a}} // 128-bit base UUID

#define BLE_UUID_OUR_SERVICE_UUID                0xE102 // Unique Service UUID

// ALREADY_DONE_FOR_YOU: Defining 16-bit characteristic UUID
#define BLE_UUID_ACCEL_CHARACTERISTIC_UUID       0xACC1 // Unique Charecteristic UUID

#define BLE_UUID_TEMP_HUMID_CHARACTERISTIC_UUID  0x1339 // Unique Charecteristic UUID

#define BLE_UUID_LUX_CHARACTERISTIC_UUID         0x7C10 // Unique Charecteristic UUID



// This structure contains various status information for our service.
// The name is based on the naming convention used in Nordics SDKs.
// 'ble� indicates that it is a Bluetooth Low Energy relevant structure and
// �os� is short for Our Service).
typedef struct
{
    uint16_t                    conn_handle;    /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection).*/
    uint16_t                    service_handle; /**< Handle of Our Service (as provided by the BLE stack). */
    // OUR_JOB: Step 2.D, Add handles for the characteristic attributes to our struct
    ble_gatts_char_handles_t    char_imu_handles;
    ble_gatts_char_handles_t    char_temp_humid_handles;
    ble_gatts_char_handles_t    char_lux_handles;
}ble_os_t;

/**@brief Function for handling BLE Stack events related to our service and characteristic.
 *
 * @details Handles all events from the BLE stack of interest to Our Service.
 *
 * @param[in]   p_our_service       Our Service structure.
 * @param[in]   p_ble_evt  Event received from the BLE stack.
 */
void ble_our_service_on_ble_evt(ble_os_t * p_our_service, ble_evt_t * p_ble_evt);

/**@brief Function for initializing our new service.
 *
 * @param[in]   p_our_service       Pointer to Our Service structure.
 */
void our_service_init(ble_os_t * p_our_service);

/**
 * @brief Function to update IMU characterisstic.
 */
void characteristic_imu_update(ble_os_t *p_our_service, int16_t *value);

/**
 * @brief Function to update temperature and humidity characterisstic.
 */
void characteristic_temp_humid_update(ble_os_t *p_our_service, uint16_t *value);

/**
 * @brief Function to update lux characterisstic.
 */
void characteristic_lux_update(ble_os_t *p_our_service, uint16_t *value);

#endif  /* _ OUR_SERVICE_H__ */
