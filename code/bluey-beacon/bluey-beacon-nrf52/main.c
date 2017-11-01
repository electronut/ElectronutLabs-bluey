/*

  bluey beacon

  This puts bluey in non-connectable advertising-only mode with no scan response.
  The ad packets send out temperature, humidity and ambient light levels.

  Electronut Labs
  electronut.in

*/

#include <stdbool.h>
#include <stdint.h>
#include "ble_advdata.h"
#include "nordic_common.h"
#include "softdevice_handler.h"
#include "bsp.h"
#include "app_timer.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "app_uart.h"

#include <stdlib.h>

#include "nrf_delay.h"
#include "nrf_drv_twi.h"
#include "HDC1010.h"
#include "APDS9301.h"
#include "bluey_twi.h"

#define CENTRAL_LINK_COUNT              0                                 /**< Number of central links used by the application. When changing this number remember to adjust the RAM settings*/
#define PERIPHERAL_LINK_COUNT           0                                 /**< Number of peripheral links used by the application. When changing this number remember to adjust the RAM settings*/

#define IS_SRVC_CHANGED_CHARACT_PRESENT 0                                 /**< Include or not the service_changed characteristic. if not enabled, the server's database cannot be changed for the lifetime of the device*/

#define APP_CFG_NON_CONN_ADV_TIMEOUT    0                                 /**< Time for which the device must be advertising in non-connectable mode (in seconds). 0 disables timeout. */
#define NON_CONNECTABLE_ADV_INTERVAL    MSEC_TO_UNITS(2000, UNIT_0_625_MS) /**< The advertising interval for non-connectable advertisement (100 ms). This value can vary between 100ms to 10.24s). */
#define BEACON_INTERVAL      APP_TIMER_TICKS(2000, APP_TIMER_PRESCALER)

#define DEAD_BEEF                       0xDEADBEEF                        /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

#define UART_TX_BUF_SIZE                256                                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE                256

#define APP_TIMER_PRESCALER             0                                 /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE         4                                 /**< Size of timer operation queues. */

#define DEVICE_NAME "bluey"

static ble_gap_adv_params_t m_adv_params;

// temperature & humidty sensor initialization
float temperature = 0;
float humidity = 0;
float lux = 0;

void set_adv_data(bool init);

/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in]   line_num   Line number of the failing ASSERT call.
 * @param[in]   file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}


APP_TIMER_DEF(m_beacon_timer_id);

// one-shot flag for setting adv data
static volatile bool g_setAdvData = false;

/**@brief Function for handling the Battery measurement timer timeout.
 *
 * @details This function will be called each time the battery level measurement timer expires.
 *
 * @param[in] p_context  Pointer used for passing some arbitrary information (context) from the
 *                       app_start_timer() call to the timeout handler.
 */
static void beacon_timeout_handler(void * p_context)
{
    UNUSED_PARAMETER(p_context);
    // set update data flag
    g_setAdvData = true;
}

/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module. This creates and starts application timers.
 */
static void timers_init(void)
{
    uint32_t err_code;

    // Initialize timer module.
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);

    // Create timers.
    err_code = app_timer_create(&m_beacon_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                beacon_timeout_handler);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for starting application timers.
 */
static void application_timers_start(void)
{
    uint32_t err_code;

    // Start application timers.
    err_code = app_timer_start(m_beacon_timer_id, BEACON_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);
}


// set adv data
void set_adv_data(bool init)
{
  uint32_t      err_code;

  ble_advdata_t advdata;
  uint8_t       flags = BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED;

  ble_advdata_manuf_data_t        manuf_data; // Variable to hold manufacturer specific data
  // Initialize with easily identifiable data
  uint8_t data[]                      = {
    0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9,
    0xb1, 0xb2, 0xb3, 0xb4
  };

  // get sensor data
  if (!init) {
    // get T/H
    uint16_t temp_val, humid_val;
    HDC1010_get_temp_raw(&temp_val);
    data[0] = temp_val >> 8;
    data[1] = temp_val;
    HDC1010_get_humid_raw(&humid_val);
    data[2] = humid_val >> 8;
    data[3] = humid_val;
    // get ambient light value
    uint16_t adc_ch0, adc_ch1;
    APDS9301_read_adc_data(&adc_ch0, &adc_ch1);
    data[4] = adc_ch0 >> 8;
    data[5] = adc_ch0;
    data[6] = adc_ch1 >> 8;
    data[7] = adc_ch1;
  }

  manuf_data.company_identifier       = 0xFFFF;
  manuf_data.data.p_data              = data;
  manuf_data.data.size                = sizeof(data);

  // Build advertising data struct to pass into @ref ble_advertising_init.
  memset(&advdata, 0, sizeof(advdata));

  advdata.name_type               = BLE_ADVDATA_SHORT_NAME;
  advdata.short_name_len          = 5;
  advdata.flags                   = flags; //BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
  advdata.p_manuf_specific_data   = &manuf_data;
  // if you don't set tx power, you get 3 extra bytes for manuf data
  //int8_t tx_power                 = -4;
  //advdata.p_tx_power_level        = &tx_power;
  advdata.include_appearance      = true;

  err_code = ble_advdata_set(&advdata, 0);
  APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing the Advertising functionality.
 *
 * @details Encodes the required advertising data and passes it to the stack.
 *          Also builds a structure to be passed to the stack when starting advertising.
 */
static void advertising_init(void)
{
    uint32_t      err_code;


    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    set_adv_data(true);

    // Initialize advertising parameters (used when starting advertising).
    memset(&m_adv_params, 0, sizeof(m_adv_params));

    m_adv_params.type        = BLE_GAP_ADV_TYPE_ADV_NONCONN_IND;
    m_adv_params.p_peer_addr = NULL;                             // Undirected advertisement.
    m_adv_params.fp          = BLE_GAP_ADV_FP_ANY;
    m_adv_params.interval    = NON_CONNECTABLE_ADV_INTERVAL;
    m_adv_params.timeout     = APP_CFG_NON_CONN_ADV_TIMEOUT;
}

/**@brief Function for starting advertising.
 */
static void advertising_start(void)
{
    uint32_t err_code;

    err_code = sd_ble_gap_adv_start(&m_adv_params);
    APP_ERROR_CHECK(err_code);

    err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
    uint32_t err_code;

    nrf_clock_lf_cfg_t clock_lf_cfg = NRF_CLOCK_LFCLKSRC;

    // Initialize the SoftDevice handler module.
    SOFTDEVICE_HANDLER_INIT(&clock_lf_cfg, NULL);

    ble_enable_params_t ble_enable_params;
    err_code = softdevice_enable_get_default_config(CENTRAL_LINK_COUNT,
                                                    PERIPHERAL_LINK_COUNT,
                                                    &ble_enable_params);
    APP_ERROR_CHECK(err_code);

    //Check the ram settings against the used number of links
    CHECK_RAM_START_ADDR(CENTRAL_LINK_COUNT,PERIPHERAL_LINK_COUNT);

    // Enable BLE stack.
    err_code = softdevice_enable(&ble_enable_params);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for doing power management.
 */
static void power_manage(void)
{
    uint32_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
}

const nrf_drv_twi_t p_twi_sensors = NRF_DRV_TWI_INSTANCE(0);

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

    //err_code = nrf_drv_twi_init(&m_twi_lis2dh12, &twi_lis2dh12_config, twi_handler, NULL);
    err_code = nrf_drv_twi_init(&p_twi_sensors, &twi_sensors_config, NULL, NULL);        // twi in blocking mode.
    APP_ERROR_CHECK(err_code);

    nrf_drv_twi_enable(&p_twi_sensors);
}
/*
 * function to read sensor registers.
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

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    uint32_t err_code;
    // Initialize.
    err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);
    err_code = bsp_init(BSP_INIT_LED, APP_TIMER_TICKS(100, APP_TIMER_PRESCALER), NULL);
    APP_ERROR_CHECK(err_code);
    ble_stack_init();
    advertising_init();
    timers_init();

    // Start execution.
    NRF_LOG_INFO("BLE Beacon started\r\n");
    advertising_start();

    application_timers_start();

    twi_init();

    HDC1010_init(TEMP_OR_HUMID);

    APDS9301_init();

    // Enter main loop.
    for (;; )
    {
      // one-shot flag to set adv data
      if(g_setAdvData) {
        set_adv_data(false);
        g_setAdvData = false;
      }

      power_manage();
    }
}


/**
 * @}
 */
