/**
 * Bluey Sensor Test Code
 *
 * Electronut Labs
 * electronut.in
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include "ble_hci.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "softdevice_handler.h"
#include "app_timer.h"
#include "app_button.h"
#include "ble_nus.h"
#include "app_uart.h"
#include "app_util_platform.h"
#include "bsp.h"
#include "bsp_btn_ble.h"
#include "boards.h"
#include "app_error.h"
#include "nrf.h"
#include "bsp.h"
#include "nrf_delay.h"
#include "nrf_drv_timer.h"
#include "nrf_drv_gpiote.h"
#include "nrf_drv_twi.h"
#include "LSM6DS3.h"
#include "HDC1010.h"
#include "APDS9301.h"
#include "bluey_gpio.h"
#include "bluey_nfc.h"
#include "bluey_sdcard.h"
#include "bluey_twi.h"


#define RGB_LED                                         // Uncomment to enable RGB LED
#define TEMP_HUMID_DATA                                 // Uncomment to obtain temperature and humidity data
#define AMBIENT_LIGHT_DATA                              // Uncomment to obtain ambient light data
#define IMU_DATA                                        // Uncomment to obtain IMU data
//#define LSM6DS3_TAP_DETECT                              // Uncomment to enable tap detect functionality
#define SDCARD_NFC_TEST                                 // Uncomment to test SD-card and NFC


#define IS_SRVC_CHANGED_CHARACT_PRESENT 0                                           /**< Include the service_changed characteristic. If not enabled, the server's database cannot be changed for the lifetime of the device. */

#if (NRF_SD_BLE_API_VERSION == 3)
#define NRF_BLE_MAX_MTU_SIZE            GATT_MTU_SIZE_DEFAULT                       /**< MTU size used in the softdevice enabling and to reply to a BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST event. */
#endif

#define APP_FEATURE_NOT_SUPPORTED       BLE_GATT_STATUS_ATTERR_APP_BEGIN + 2        /**< Reply when unsupported features are requested. */

#define CENTRAL_LINK_COUNT              0                                           /**< Number of central links used by the application. When changing this number remember to adjust the RAM settings*/
#define PERIPHERAL_LINK_COUNT           1                                           /**< Number of peripheral links used by the application. When changing this number remember to adjust the RAM settings*/

#define DEVICE_NAME                     "Bluey_ST"                               /**< Name of device. Will be included in the advertising data. */
#define NUS_SERVICE_UUID_TYPE           BLE_UUID_TYPE_VENDOR_BEGIN                  /**< UUID type for the Nordic UART Service (vendor specific). */

#define APP_ADV_INTERVAL                64                                          /**< The advertising interval (in units of 0.625 ms. This value corresponds to 40 ms). */
#define APP_ADV_TIMEOUT_IN_SECONDS      180                                         /**< The advertising timeout (in units of seconds). */

#define APP_TIMER_PRESCALER             0                                           /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE         4                                           /**< Size of timer operation queues. */

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(20, UNIT_1_25_MS)             /**< Minimum acceptable connection interval (20 ms), Connection interval uses 1.25 ms units. */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(75, UNIT_1_25_MS)             /**< Maximum acceptable connection interval (75 ms), Connection interval uses 1.25 ms units. */
#define SLAVE_LATENCY                   0                                           /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)             /**< Connection supervisory timeout (4 seconds), Supervision Timeout uses 10 ms units. */
#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER)  /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000, APP_TIMER_PRESCALER) /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                           /**< Number of attempts before giving up the connection parameter negotiation. */

#define DEAD_BEEF                       0xDEADBEEF                                  /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

#define UART_TX_BUF_SIZE                256                                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE                256                                         /**< UART RX buffer size. */

static ble_nus_t                        m_nus;                                      /**< Structure to identify the Nordic UART Service. */
static uint16_t                         m_conn_handle = BLE_CONN_HANDLE_INVALID;    /**< Handle of the current connection. */

static ble_uuid_t                       m_adv_uuids[] = {{BLE_UUID_NUS_SERVICE, NUS_SERVICE_UUID_TYPE}};  /**< Universally unique service identifier. */


/**@brief Function for assert macro callback.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyse
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num    Line number of the failing ASSERT call.
 * @param[in] p_file_name File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}


/**@brief Function for the GAP initialization.
 *
 * @details This function will set up all the necessary GAP (Generic Access Profile) parameters of
 *          the device. It also sets the permissions and appearance.
 */
static void gap_params_init(void)
{
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *) DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling the data from the Nordic UART Service.
 *
 * @details This function will process the data received from the Nordic UART BLE Service and send
 *          it to the UART module.
 *
 * @param[in] p_nus    Nordic UART Service structure.
 * @param[in] p_data   Data to be send to UART module.
 * @param[in] length   Length of the data.
 */
/**@snippet [Handling the data received over BLE] */
static void nus_data_handler(ble_nus_t * p_nus, uint8_t * p_data, uint16_t length)
{
    for (uint32_t i = 0; i < length; i++)
    {
        while (app_uart_put(p_data[i]) != NRF_SUCCESS);
    }
    while (app_uart_put('\r') != NRF_SUCCESS);
    while (app_uart_put('\n') != NRF_SUCCESS);
}
/**@snippet [Handling the data received over BLE] */


/**@brief Function for initializing services that will be used by the application.
 */
static void services_init(void)
{
    uint32_t       err_code;
    ble_nus_init_t nus_init;

    memset(&nus_init, 0, sizeof(nus_init));

    nus_init.data_handler = nus_data_handler;

    err_code = ble_nus_init(&m_nus, &nus_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling an event from the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module
 *          which are passed to the application.
 *
 * @note All this function does is to disconnect. This could have been done by simply setting
 *       the disconnect_on_fail config parameter, but instead we use the event handler
 *       mechanism to demonstrate its use.
 *
 * @param[in] p_evt  Event received from the Connection Parameters Module.
 */
static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    uint32_t err_code;

    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}


/**@brief Function for handling errors from the Connection Parameters module.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for initializing the Connection Parameters module.
 */
static void conn_params_init(void)
{
    uint32_t               err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 */
static void sleep_mode_enter(void)
{
    uint32_t err_code = bsp_indication_set(BSP_INDICATE_IDLE);
    APP_ERROR_CHECK(err_code);

    // Prepare wakeup buttons.
    err_code = bsp_btn_ble_sleep_mode_prepare();
    APP_ERROR_CHECK(err_code);

    // Go to system-off mode (this function will not return; wakeup will cause a reset).
    err_code = sd_power_system_off();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    uint32_t err_code;

    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_FAST:
            err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING);
            APP_ERROR_CHECK(err_code);
            break;
        case BLE_ADV_EVT_IDLE:
            sleep_mode_enter();
            break;
        default:
            break;
    }
}


/**@brief Function for the application's SoftDevice event handler.
 *
 * @param[in] p_ble_evt SoftDevice event.
 */
static void on_ble_evt(ble_evt_t * p_ble_evt)
{
    uint32_t err_code;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            err_code = bsp_indication_set(BSP_INDICATE_CONNECTED);
            APP_ERROR_CHECK(err_code);
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            break; // BLE_GAP_EVT_CONNECTED

        case BLE_GAP_EVT_DISCONNECTED:
            err_code = bsp_indication_set(BSP_INDICATE_IDLE);
            APP_ERROR_CHECK(err_code);
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
            break; // BLE_GAP_EVT_DISCONNECTED

        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            // Pairing not supported
            err_code = sd_ble_gap_sec_params_reply(m_conn_handle, BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL, NULL);
            APP_ERROR_CHECK(err_code);
            break; // BLE_GAP_EVT_SEC_PARAMS_REQUEST

        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            // No system attributes have been stored.
            err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0, 0);
            APP_ERROR_CHECK(err_code);
            break; // BLE_GATTS_EVT_SYS_ATTR_MISSING

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break; // BLE_GATTC_EVT_TIMEOUT

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break; // BLE_GATTS_EVT_TIMEOUT

        case BLE_EVT_USER_MEM_REQUEST:
            err_code = sd_ble_user_mem_reply(p_ble_evt->evt.gattc_evt.conn_handle, NULL);
            APP_ERROR_CHECK(err_code);
            break; // BLE_EVT_USER_MEM_REQUEST

        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
        {
            ble_gatts_evt_rw_authorize_request_t  req;
            ble_gatts_rw_authorize_reply_params_t auth_reply;

            req = p_ble_evt->evt.gatts_evt.params.authorize_request;

            if (req.type != BLE_GATTS_AUTHORIZE_TYPE_INVALID)
            {
                if ((req.request.write.op == BLE_GATTS_OP_PREP_WRITE_REQ)     ||
                    (req.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_NOW) ||
                    (req.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_CANCEL))
                {
                    if (req.type == BLE_GATTS_AUTHORIZE_TYPE_WRITE)
                    {
                        auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_WRITE;
                    }
                    else
                    {
                        auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_READ;
                    }
                    auth_reply.params.write.gatt_status = APP_FEATURE_NOT_SUPPORTED;
                    err_code = sd_ble_gatts_rw_authorize_reply(p_ble_evt->evt.gatts_evt.conn_handle,
                                                               &auth_reply);
                    APP_ERROR_CHECK(err_code);
                }
            }
        } break; // BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST

#if (NRF_SD_BLE_API_VERSION == 3)
        case BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST:
            err_code = sd_ble_gatts_exchange_mtu_reply(p_ble_evt->evt.gatts_evt.conn_handle,
                                                       NRF_BLE_MAX_MTU_SIZE);
            APP_ERROR_CHECK(err_code);
            break; // BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST
#endif

        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for dispatching a SoftDevice event to all modules with a SoftDevice
 *        event handler.
 *
 * @details This function is called from the SoftDevice event interrupt handler after a
 *          SoftDevice event has been received.
 *
 * @param[in] p_ble_evt  SoftDevice event.
 */
static void ble_evt_dispatch(ble_evt_t * p_ble_evt)
{
    ble_conn_params_on_ble_evt(p_ble_evt);
    ble_nus_on_ble_evt(&m_nus, p_ble_evt);
    on_ble_evt(p_ble_evt);
    ble_advertising_on_ble_evt(p_ble_evt);
    bsp_btn_ble_on_ble_evt(p_ble_evt);
}


/**@brief Function for the SoftDevice initialization.
 *
 * @details This function initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
    uint32_t err_code;

    nrf_clock_lf_cfg_t clock_lf_cfg = NRF_CLOCK_LFCLKSRC;

    // Initialize SoftDevice.
    SOFTDEVICE_HANDLER_INIT(&clock_lf_cfg, NULL);

    ble_enable_params_t ble_enable_params;
    err_code = softdevice_enable_get_default_config(CENTRAL_LINK_COUNT,
                                                    PERIPHERAL_LINK_COUNT,
                                                    &ble_enable_params);
    APP_ERROR_CHECK(err_code);

    //Check the ram settings against the used number of links
    CHECK_RAM_START_ADDR(CENTRAL_LINK_COUNT,PERIPHERAL_LINK_COUNT);

    // Enable BLE stack.
#if (NRF_SD_BLE_API_VERSION == 3)
    ble_enable_params.gatt_enable_params.att_mtu = NRF_BLE_MAX_MTU_SIZE;
#endif
    err_code = softdevice_enable(&ble_enable_params);
    APP_ERROR_CHECK(err_code);

    // Subscribe for BLE events.
    err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling events from the BSP module.
 *
 * @param[in]   event   Event generated by button press.
 */
void bsp_event_handler(bsp_event_t event)
{
    uint32_t err_code;
    switch (event)
    {
        case BSP_EVENT_SLEEP:
            sleep_mode_enter();
            break;

        case BSP_EVENT_DISCONNECT:
            err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            if (err_code != NRF_ERROR_INVALID_STATE)
            {
                APP_ERROR_CHECK(err_code);
            }
            break;

        case BSP_EVENT_WHITELIST_OFF:
            if (m_conn_handle == BLE_CONN_HANDLE_INVALID)
            {
                err_code = ble_advertising_restart_without_whitelist();
                if (err_code != NRF_ERROR_INVALID_STATE)
                {
                    APP_ERROR_CHECK(err_code);
                }
            }
            break;

        default:
            break;
    }
}

// flag to check if UART is enabled or disabled.
static volatile bool uart_enabled = false;

/**@brief   Function for handling app_uart events.
 *
 * @details This function will receive a single character from the app_uart module and append it to
 *          a string. The string will be be sent over BLE when the last character received was a
 *          'new line' i.e '\r\n' (hex 0x0D) or if the string has reached a length of
 *          @ref NUS_MAX_DATA_LENGTH.
 */
/**@snippet [Handling the data received over UART] */
void uart_event_handle(app_uart_evt_t * p_event)
{
    static uint8_t data_array[BLE_NUS_MAX_DATA_LEN];
    static uint8_t index = 0;
    uint32_t       err_code;

    switch (p_event->evt_type)
    {
        case APP_UART_DATA_READY:
            UNUSED_VARIABLE(app_uart_get(&data_array[index]));
            index++;

            if ((data_array[index - 1] == '\n') || (index >= (BLE_NUS_MAX_DATA_LEN)))
            {
                err_code = ble_nus_string_send(&m_nus, data_array, index);
                if (err_code != NRF_ERROR_INVALID_STATE)
                {
                    APP_ERROR_CHECK(err_code);
                }

                index = 0;
            }
            break;

        case APP_UART_COMMUNICATION_ERROR:
          // Close UART module in case of communication error
        	app_uart_close();
          // set UART flag as false to indicate disabled UART
          uart_enabled = false;
            //APP_ERROR_HANDLER(p_event->data.error_communication);
            break;

        case APP_UART_FIFO_ERROR:
            APP_ERROR_HANDLER(p_event->data.error_code);
            break;

        default:
            break;
    }
}
/**@snippet [Handling the data received over UART] */


/**@brief  Function for initializing the UART module.
 */
/**@snippet [UART Initialization] */
static void uart_init(void)
{
    uint32_t                     err_code;
    const app_uart_comm_params_t comm_params =
    {
        RX_PIN_NUMBER,
        TX_PIN_NUMBER,
        RTS_PIN_NUMBER,
        CTS_PIN_NUMBER,
        APP_UART_FLOW_CONTROL_DISABLED,
        false,
        UART_BAUDRATE_BAUDRATE_Baud115200
    };

    APP_UART_FIFO_INIT( &comm_params,
                       UART_RX_BUF_SIZE,
                       UART_TX_BUF_SIZE,
                       uart_event_handle,
                       APP_IRQ_PRIORITY_LOWEST,
                       err_code);
    APP_ERROR_CHECK(err_code);

    // UART enabled
    uart_enabled = true;
}
/**@snippet [UART Initialization] */


/**@brief Function for initializing the Advertising functionality.
 */
static void advertising_init(void)
{
    uint32_t               err_code;
    ble_advdata_t          advdata;
    ble_advdata_t          scanrsp;
    ble_adv_modes_config_t options;

    // Build advertising data struct to pass into @ref ble_advertising_init.
    memset(&advdata, 0, sizeof(advdata));
    advdata.name_type          = BLE_ADVDATA_FULL_NAME;
    advdata.include_appearance = false;
    advdata.flags              = BLE_GAP_ADV_FLAGS_LE_ONLY_LIMITED_DISC_MODE;

    memset(&scanrsp, 0, sizeof(scanrsp));
    scanrsp.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    scanrsp.uuids_complete.p_uuids  = m_adv_uuids;

    memset(&options, 0, sizeof(options));
    options.ble_adv_fast_enabled  = true;
    options.ble_adv_fast_interval = APP_ADV_INTERVAL;
    options.ble_adv_fast_timeout  = APP_ADV_TIMEOUT_IN_SECONDS;

    err_code = ble_advertising_init(&advdata, &scanrsp, &options, on_adv_evt, NULL);
    APP_ERROR_CHECK(err_code);
}


const nrf_drv_twi_t p_twi_sensors = NRF_DRV_TWI_INSTANCE(1);

/**
 * @brief TWI initialization.
 */
void twi_init (void)
{
    ret_code_t err_code;

    const nrf_drv_twi_config_t twi_sensors_config = {
       .scl                = TWI_SCL_PIN,
       .sda                = TWI_SDA_PIN,
       .frequency          = NRF_TWI_FREQ_100K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH
    };

    err_code = nrf_drv_twi_init(&p_twi_sensors, &twi_sensors_config, NULL, NULL);        // twi in blocking mode.
    APP_ERROR_CHECK(err_code);

    nrf_drv_twi_enable(&p_twi_sensors);
}

/**
 * @brief function to read sensor registers.
 */
ret_code_t read_register(nrf_drv_twi_t twi_instance, uint8_t device_addr, uint8_t register_addr, uint8_t *p_data, uint8_t bytes, bool no_stop)
{
  ret_code_t err_code;

  err_code = nrf_drv_twi_tx(&twi_instance, device_addr, &register_addr, 1, no_stop);
  APP_ERROR_CHECK(err_code);

  if(err_code != NRF_SUCCESS) {
    return err_code;
  }

  err_code = nrf_drv_twi_rx(&twi_instance, device_addr, p_data, bytes);
  return err_code;
}


// routine to test interrupts in LSM6DS3 using tap detect functionality.
#ifdef LSM6DS3_TAP_DETECT

uint8_t tap_count = 0;
/**
 * @brief interrupt handler for tap detect
 */
void get_tap_count(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
  tap_count++;
}

/**
 * @brief function to enable interrupts on pins connected to LSM6DS3 interrupt lines.
 */
static void LSM6DS3_gpiote_init(void)
{
  ret_code_t err_code;

  if(!nrf_drv_gpiote_is_init())
   {
     err_code = nrf_drv_gpiote_init();
     APP_ERROR_CHECK(err_code);
   }

  nrf_drv_gpiote_in_config_t int1 = GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);
  int1.pull = NRF_GPIO_PIN_NOPULL;

  err_code = nrf_drv_gpiote_in_init(INT2, &int1, get_tap_count);
  APP_ERROR_CHECK(err_code);

  nrf_drv_gpiote_in_event_enable(INT2, true);
}
#endif

// routine to test SD-card (enabled using button press) and NFC
#ifdef SDCARD_NFC_TEST

float temperature = 0;
float humidity = 0;

uint16_t adc_ch0, adc_ch1;
float lux;

int16_t accX, accY, accZ;
int16_t gyroX, gyroY, gyroZ;
float accel_X, accel_Y, accel_Z;
float gyro_X, gyro_Y, gyro_Z;

/**
 * @brief function to configure sensors for data logging.
 */
void sensors_config(void)
{
  // initialize HDC1010 for temperature and humidity data.
  HDC1010_init(TEMP_OR_HUMID);
  // initialize APDS9301 for ambient light data.
  APDS9301_init();
  // initialize LSM6DS3 for IMU data.
  LSM6DS3_init();
}

/**
 * @brief function to obtain data for logging.
 */
void get_sensor_data(void)
{
  // get temperature and humidity data
  temperature = HDC1010_get_temp();
  humidity = HDC1010_get_humid();
  // get ambient light data
  APDS9301_read_adc_data(&adc_ch0, &adc_ch1);
  lux = getlux(adc_ch0, adc_ch1);

  // get IMU data
  LSM6DS3_read_accl_data(&accX, &accY, &accZ);
  LSM6DS3_read_gyro_data(&gyroX, &gyroY, &gyroZ);

  accel_X = LSM6DS3_accelData_in_g(accX);
  accel_Y = LSM6DS3_accelData_in_g(accY);
  accel_Z = LSM6DS3_accelData_in_g(accZ);

  gyro_X = LSM6DS3_gyroData_in_dps(gyroX);
  gyro_Y = LSM6DS3_gyroData_in_dps(gyroY);
  gyro_Z = LSM6DS3_gyroData_in_dps(gyroZ);
}

/**
  * @brief function to toggle led on button_press
 */
void led_toggle(void)
{
nrf_gpio_pin_clear(18);
nrf_delay_ms(500);
nrf_gpio_pin_set(18);
nrf_delay_ms(500);
}

static volatile bool button_state = false;

/**
 * @brief interrupt handler for button press
 */
void button_press_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
 rgb_led_off();
 led_toggle();
  button_state = true;
}

/**
 * @brief function to enable button test.
 */
static void button_config(void)
{
  ret_code_t err_code;

  if(!nrf_drv_gpiote_is_init())
   {
     err_code = nrf_drv_gpiote_init();
     APP_ERROR_CHECK(err_code);
   }

  nrf_drv_gpiote_in_config_t button_config = GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);
  button_config.pull = NRF_GPIO_PIN_PULLUP;

  err_code = nrf_drv_gpiote_in_init(button_sw3, &button_config, button_press_handler);
  APP_ERROR_CHECK(err_code);

  nrf_drv_gpiote_in_event_enable(button_sw3, true);
}

#endif

// routine to test RGB LED using timer.
#ifdef RGB_LED

const nrf_drv_timer_t TIMER_LED = NRF_DRV_TIMER_INSTANCE(1);

static volatile bool led_state = true;

uint8_t led_index = 0;

/**
 * @brief function to cycle through LED colour combinations
 */
void led_func()
{
  switch(led_index) {
    case 0:
      rgb_led_set_yellow();
      led_state = false;
      break;

    case 1:
      rgb_led_set_cyan();
      led_state = false;
      break;

    case 2:
      rgb_led_set_magenta();
      led_state = false;
      break;
  }
}

/**
 * @brief timer event handler
 */
void timer_led_event_handler(nrf_timer_event_t event_type, void* p_context)
{
    switch (event_type)
    {
      case NRF_TIMER_EVENT_COMPARE0:
        if(led_state) {
         led_index = (led_index) % 3;
         led_func();
         led_index++;
        }
        else {
         rgb_led_off();
         led_state = true;
        }

        default:
        //Do nothing.
        break;
    }
}

/**
 * @brief timer initialization
 */
void led_timer_init(void)
{
  uint32_t time_ms = 200; //Time(in miliseconds) between consecutive compare events.
  uint32_t time_ticks;
  uint32_t err_code = NRF_SUCCESS;

  //Configure TIMER_LED for generating simple light effect - leds on board will invert his state one after the other.
  nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
  err_code = nrf_drv_timer_init(&TIMER_LED, &timer_cfg, timer_led_event_handler);
  APP_ERROR_CHECK(err_code);

  time_ticks = nrf_drv_timer_ms_to_ticks(&TIMER_LED, time_ms);

  nrf_drv_timer_extended_compare(
   &TIMER_LED, NRF_TIMER_CC_CHANNEL0, time_ticks, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);

  nrf_drv_timer_enable(&TIMER_LED);
}
#endif

/**
 * @brief Function for main application entry.
 */
int main(void)
{
  uint32_t err_code;

  APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);

  uart_init();

  ble_stack_init();
  gap_params_init();
  services_init();
  advertising_init();
  conn_params_init();

  err_code = ble_advertising_start(BLE_ADV_MODE_FAST);
  APP_ERROR_CHECK(err_code);

  twi_init();

// RGB LED initialization
#ifdef RGB_LED
  rgb_led_config();
  led_timer_init();
#endif

// SD-card and NFC initialization
#ifdef SDCARD_NFC_TEST
  // configuration to test SD-card
  rgb_led_config();
  sensors_config();
  button_config();

  // configuration to test NFC
  HDC1010_init(TEMP_OR_HUMID);

  uint8_t m_ndef_msg_buf[256];

  uint32_t  len = sizeof(m_ndef_msg_buf);

  err_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(err_code);

  /* Configure LED-pins as outputs */
  bsp_board_leds_init();

  /* Set up NFC */
  err_code = nfc_t2t_setup(nfc_callback, NULL);
  APP_ERROR_CHECK(err_code);

  /* Encode welcome message */
  encode_data(m_ndef_msg_buf, &len);

  /* Set created message as the NFC payload */
  err_code = nfc_t2t_payload_set(m_ndef_msg_buf, len);
  APP_ERROR_CHECK(err_code);

  /* Start sensing NFC field */
  err_code = nfc_t2t_emulation_start();
  APP_ERROR_CHECK(err_code);
#endif

// temperature & humidty sensor initialization
#ifdef TEMP_HUMID_DATA
  float temperature = 0;
  float humidity = 0;
  unsigned char str_th[20];
  HDC1010_init(TEMP_OR_HUMID);
#endif

// ambient light sensor initialization
#ifdef AMBIENT_LIGHT_DATA
  uint16_t adc_ch0, adc_ch1;
  float lux;
  unsigned char str_al[10];
  APDS9301_init();
#endif

// IMU initialization
#ifdef IMU_DATA
#ifndef LSM6DS3_TAP_DETECT
  int16_t accX, accY, accZ;
  int16_t gyroX, gyroY, gyroZ;
  float accel_X, accel_Y, accel_Z;
  float gyro_X, gyro_Y, gyro_Z;
  unsigned char str_imu[30];
#endif
  LSM6DS3_init();
#endif

#ifdef LSM6DS3_TAP_DETECT
  LSM6DS3_gpiote_init();
  LSM6DS3_tap_detect_config();
  printf("Tap Detect test\n");
#endif


  for(;;) {

#ifdef SDCARD_NFC_TEST
    if(button_state) {
      button_state = false;
      get_sensor_data();
      sdcard_sensor_update_data(temperature, humidity, lux, accel_X, accel_Y, accel_Z, gyro_X, gyro_Y, gyro_Z);
      cycle_gpio();
      // nrf_delay_ms(200);
    }
#endif

#ifdef TEMP_HUMID_DATA
    temperature = HDC1010_get_temp();
    humidity = HDC1010_get_humid();
    if(uart_enabled) {
      printf("Temperature: %f C, Humidity: %f %%\n\n", temperature, humidity);
    }
    nrf_delay_ms(1000);
    sprintf((char *)str_th, "%.2f C, %.2f %%\n", (float)temperature, (float)humidity);
    ble_nus_string_send(&m_nus, str_th, strlen((char *)str_th));
#endif

#ifdef AMBIENT_LIGHT_DATA
    APDS9301_read_adc_data(&adc_ch0, &adc_ch1);
    lux = getlux(adc_ch0, adc_ch1);
    if(uart_enabled) {
      printf("LUX: %f lx\n\n", lux);
    }
    nrf_delay_ms(1000);
   sprintf((char *)str_al, "%.2f lux\n", (float)lux);
   ble_nus_string_send(&m_nus, str_al, strlen((char *)str_al));
#endif

#ifdef IMU_DATA
#ifndef LSM6DS3_TAP_DETECT
    LSM6DS3_read_accl_data(&accX, &accY, &accZ);
    LSM6DS3_read_gyro_data(&gyroX, &gyroY, &gyroZ);

    accel_X = LSM6DS3_accelData_in_g(accX);
    accel_Y = LSM6DS3_accelData_in_g(accY);
    accel_Z = LSM6DS3_accelData_in_g(accZ);

    gyro_X = LSM6DS3_gyroData_in_dps(gyroX);
    gyro_Y = LSM6DS3_gyroData_in_dps(gyroY);
    gyro_Z = LSM6DS3_gyroData_in_dps(gyroZ);

    if(uart_enabled) {
      printf("Acceleration:\n");
      printf("X = %.2f g, Y = %.2f g, Z = %.2f g\n\n", accel_X, accel_Y, accel_Z);
    }
    nrf_delay_ms(1000);
    sprintf((char *)str_imu, "%.1fg, %.1fg, %.fg\n", (float)accel_X, (float)accel_Y, (float)accel_Z);
    ble_nus_string_send(&m_nus, str_imu, strlen((char *)str_imu));

    if(uart_enabled) {
      printf("Gyroscope:\n");
      printf("X = %.1f dps, Y = %.1f dps, Z = %.1f dps\n\n\n", gyro_X, gyro_Y, gyro_Z);
    }
    nrf_delay_ms(1000);
    // cannot accomodate "dps" due to NUS limitation of sending float data.
      sprintf((char *)str_imu, "%.2f, %.2f, %.2f\n", (float)gyro_X, (float)gyro_Y, (float)gyro_Z);
      ble_nus_string_send(&m_nus, str_imu, strlen((char *)str_imu));
#endif
#endif

#ifdef LSM6DS3_TAP_DETECT
    if(tap_count > 0) {
      nrf_delay_ms(300);
    if(tap_count == 1) {
      if(uart_enabled) {
        printf("Single tap\n");
      }
    }
    else {
      if(uart_enabled) {
        printf("Double tap\n");
      }
    }
    tap_count = 0;
  }
#endif

#ifndef LSM6DS3_TAP_DETECT
    //nrf_delay_ms(500);
#endif
  }
}

/** @} */
