/*
  Bluey pedometer


  Electronut Labs
  electronut.in
 */


#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
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
#include "nrf_drv_twi.h"
#include "nrf_delay.h"
#include "bluey_service.h"
#include "HDC1010.h"
#include "LSM6DS3.h"
#include "APDS9301.h"

#define IS_SRVC_CHANGED_CHARACT_PRESENT 0                                           /**< Include the service_changed characteristic. If not enabled, the server's database cannot be changed for the lifetime of the device. */

#if (NRF_SD_BLE_API_VERSION == 3)
#define NRF_BLE_MAX_MTU_SIZE            GATT_MTU_SIZE_DEFAULT                       /**< MTU size used in the softdevice enabling and to reply to a BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST event. */
#endif

#define APP_FEATURE_NOT_SUPPORTED       BLE_GATT_STATUS_ATTERR_APP_BEGIN + 2        /**< Reply when unsupported features are requested. */

#define CENTRAL_LINK_COUNT              0                                           /**< Number of central links used by the application. When changing this number remember to adjust the RAM settings*/
#define PERIPHERAL_LINK_COUNT           1                                           /**< Number of peripheral links used by the application. When changing this number remember to adjust the RAM settings*/

#define DEVICE_NAME                     "Bluey_walkie"                               /**< Name of device. Will be included in the advertising data. */
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

//static ble_nus_t                        m_nus;                                      /**< Structure to identify the Nordic UART Service. */
static ble_os_t                         m_our_service;                              /**< Structure to identify the Nordic UART Service. */

static uint16_t                         m_conn_handle = BLE_CONN_HANDLE_INVALID;    /**< Handle of the current connection. */

//static ble_uuid_t                       m_adv_uuids[] = {{BLE_UUID_NUS_SERVICE, NUS_SERVICE_UUID_TYPE}};  /**< Universally unique service identifier. */

static ble_uuid_t                       m_adv_uuids[] = {{BLE_UUID_OUR_SERVICE_UUID, BLE_UUID_TYPE_VENDOR_BEGIN}};  /**< Universally unique service identifier. */


#define OUR_ACCEL_TIMER_INTERVAL     APP_TIMER_TICKS(1000, APP_TIMER_PRESCALER) // 1000 ms intervals

APP_TIMER_DEF(m_our_sensor_timer_id);

int flag = 0;
int flag1 = 0;
int16_t x_val, y_val, z_val;
float faccel_Y;
int i,a,count = 0;
float paccel_Y = 0.0f;
float sampled_Y;
float sample[50];
int acc_val = 0;

/*
* Function to calculate the step count from accelerometer data
*/

void calculate()
{
  LSM6DS3_read_accl_data(&x_val,&y_val,&z_val);
  for(i = 0; i < 50; i++)
    {
     faccel_Y = (paccel_Y+y_val)/2;
     paccel_Y = y_val;
     sample[i] = faccel_Y;
     sampled_Y += sample[i];
    }
    sampled_Y = sampled_Y/50;
    if ((sampled_Y <= -4000 && flag == 0) || (sampled_Y >= 4000 && flag1 == 0))
    {
      count += 1;
      flag = 1;
      flag1 = 1;
    }
    else if ((sampled_Y <= -4000 && flag == 0) || (sampled_Y >= 4000 && flag1 == 0))
    {
      a = a+1;
    }
    if (sampled_Y >= -4000)
    {
      flag  = 0;
    }
    else if (sampled_Y <= 4000)
    {
      flag1 = 0;
    }
}

/*
* In the timer handler, we read pedometer data
* And then pushed via the custom service.
*/

static void timer_timeout_sensor_handler(void * p_context)
{
  calculate();
  acc_val = count;
  characteristic_imu_update(&m_our_service, &acc_val);
}

/*
* Function to initialize the timer
*/

static void timers_init(void)
{
  // Initialize timer module.
  APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);
  app_timer_create(&m_our_sensor_timer_id, APP_TIMER_MODE_REPEATED, timer_timeout_sensor_handler);
}

/*
* Function for starting timers.
*/

static void application_timers_start(void)
{
  app_timer_start(m_our_sensor_timer_id, OUR_ACCEL_TIMER_INTERVAL, NULL);
}

/*
* Function for assert macro callback.
*/

void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
  app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

/*
* Function for the GAP initialization.
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

/*
* Function for initializing services that will be used by the application.
*/

static void services_init(void)
{
  our_service_init(&m_our_service);
}

/*
* Function for handling an event from the Connection Parameters Module.
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

/*
* Function for handling errors from the Connection Parameters module.
*/

static void conn_params_error_handler(uint32_t nrf_error)
{
  APP_ERROR_HANDLER(nrf_error);
}

/*
* Function for initializing the Connection Parameters module.
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

/*
* Function for putting the chip into sleep mode.
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

/*
* Function for handling advertising events.
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

/*
* Function for the application's SoftDevice event handler.
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

/*
* Function for dispatching a SoftDevice event to all modules with a SoftDevice event handler
*/

static void ble_evt_dispatch(ble_evt_t * p_ble_evt)
{
  ble_conn_params_on_ble_evt(p_ble_evt);
  // custom service event
  ble_our_service_on_ble_evt(&m_our_service, p_ble_evt);
  on_ble_evt(p_ble_evt);
  ble_advertising_on_ble_evt(p_ble_evt);
  bsp_btn_ble_on_ble_evt(p_ble_evt);
}

/*
* Function for the SoftDevice initialization.
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

/*
* Function for handling events from the BSP module.
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

#if 0
/*
* Function for handling app_uart events.
*/

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
            APP_ERROR_HANDLER(p_event->data.error_communication);
            break;

        case APP_UART_FIFO_ERROR:
            APP_ERROR_HANDLER(p_event->data.error_code);
            break;

        default:
            break;
    }
}

/*
* Function for initializing the UART module.
*/

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
}
#endif

/*
* Function for initializing the Advertising functionality.
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
   advdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
   advdata.uuids_complete.p_uuids  = &m_adv_uuids[0];
   memset(&scanrsp, 0, sizeof(scanrsp));
   //scanrsp.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
   //scanrsp.uuids_complete.p_uuids  = &m_adv_uuids[1];
   memset(&options, 0, sizeof(options));
   options.ble_adv_fast_enabled  = true;
   options.ble_adv_fast_interval = APP_ADV_INTERVAL;
   options.ble_adv_fast_timeout  = APP_ADV_TIMEOUT_IN_SECONDS;
   err_code = ble_advertising_init(&advdata, &scanrsp, &options, on_adv_evt, NULL);
   APP_ERROR_CHECK(err_code);
 }

/*
* Function for placing the application in low power state while waiting for events.
*/

static void power_manage(void)
{
  uint32_t err_code = sd_app_evt_wait();
  APP_ERROR_CHECK(err_code);
}

const nrf_drv_twi_t p_twi_sensors = NRF_DRV_TWI_INSTANCE(0);

/*
 * TWI initialization.
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
 * Function to read sensor registers.
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

/*
* Application main function.
 */

int main(void)
{
  uint32_t err_code;
  // Initialize.
  APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);
  timers_init();
  //uart_init();
  ble_stack_init();
  gap_params_init();
  services_init();
  advertising_init();
  conn_params_init();
  application_timers_start();
  err_code = ble_advertising_start(BLE_ADV_MODE_FAST);
  APP_ERROR_CHECK(err_code);
  twi_init();
  LSM6DS3_init();
  LSM6DS3_config();
  // Enter main loop.
  for (;;)
  {
   power_manage();
  }
}
