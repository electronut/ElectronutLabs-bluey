/*
 Bluey-ultrasonic-test

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
#include "app_pwm.h"
#include "nrf_drv_timer.h"
#include "nrf_gpio.h"
#include "app_button.h"
#include "ble_nus.h"
#include "app_uart.h"
#include "app_util_platform.h"
#include "bsp.h"
#include "nrf_delay.h"
#include "bsp_btn_ble.h"

#define IS_SRVC_CHANGED_CHARACT_PRESENT 0                                           /**< Include the service_changed characteristic. If not enabled, the server's database cannot be changed for the lifetime of the device. */

#if (NRF_SD_BLE_API_VERSION == 3)
#define NRF_BLE_MAX_MTU_SIZE            GATT_MTU_SIZE_DEFAULT                       /**< MTU size used in the softdevice enabling and to reply to a BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST event. */
#endif

#define APP_FEATURE_NOT_SUPPORTED       BLE_GATT_STATUS_ATTERR_APP_BEGIN + 2        /**< Reply when unsupported features are requested. */

#define CENTRAL_LINK_COUNT              0                                           /**< Number of central links used by the application. When changing this number remember to adjust the RAM settings*/
#define PERIPHERAL_LINK_COUNT           1                                           /**< Number of peripheral links used by the application. When changing this number remember to adjust the RAM settings*/

#define DEVICE_NAME                     "Bluey-bot"                               /**< Name of device. Will be included in the advertising data. */
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


#define START "on"
#define STOP "off"
#define RIGHT "right"
#define LEFT "left"
#define REVERSE "reverse"

// HC-SR04 Trigger pin
const uint32_t pinTrig = 4;
// HC-SR04 Echo pin
const uint32_t pinEcho = 22;
// input pins for motor 1
const uint32_t enable1 = 2;                                                            // drv8835 PWM input for right motor
const uint32_t phase1   = 24;                                                           // drv8835 Phase input for right motor

// input pins for motor 2
const uint32_t enable2 = 3;                                                            // drv8835 PWM input for left motor
const uint32_t phase2   = 23;                                                           // drv8835 Phase input for left motor

// pin to select PH/EN mode in DRV8835
const uint32_t mode = 26;

// pins to RGB LED
const uint32_t led_R = 19;                                                             // pin for red LED
const uint32_t led_G = 18;                                                             // pin for green LED
const uint32_t led_B = 17;                                                             // pin for blue LED


float value = 0;

// creating TIMER 4 instance for HCSR-04
const nrf_drv_timer_t TIMER_HCSR04 = NRF_DRV_TIMER_INSTANCE(4);

// setting up pwm uLEFTg timer1
APP_PWM_INSTANCE(PWM1,1);

static volatile bool ready_flag;

/*
 *  function to call PWM callback
*/
void pwm_ready_callback(uint32_t pwm_id)
{
  ready_flag = true;
}

// set in start_timer()
static volatile uint32_t tCount = 0;

// count to us (micro seconds) conversion factor
static volatile float countToUs = 1;

/* get distance measurement from HC-SR04:
* Send a 10us HIGH pulse on the Trigger pin.
* The sensor sends out a “sonic burst” of 8 cycles.
* Listen to the Echo pin, and the duration of the next HIGH
* RIGHTal will give you the time taken by the sound to go back
* and forth from sensor to target.
* returns true only if a valid distance is obtained
*/
void getDistance(float* dist)
{
  // send 12us trigger pulse
  //    _
  // __| |__
  nrf_gpio_pin_clear(pinTrig);
  nrf_delay_us(20);
  nrf_gpio_pin_set(pinTrig);
  nrf_delay_us(12);
  nrf_gpio_pin_clear(pinTrig);
  nrf_delay_us(20);

  // listen for echo and time it
  //       ____________
  // _____|            |___

  // reset counter
   tCount = 0;
   // wait till Echo pin goes high or counter is too high
   while(!nrf_gpio_pin_read(pinEcho) && tCount < 1000);
   // reset counter
   tCount = 0;
   // wait till Echo pin goes low or counter is too high
   while(nrf_gpio_pin_read(pinEcho) && tCount < 1000);

   // calculate duration in us
   float duration = countToUs*tCount;

   // dist = duration * speed of sound * 1/2
   // dist in cm = duration in us * 10^-6 * 340.29 * 100 * 1/2
   float distance = duration * 0.017;
   *dist = distance;
}

/*
* function to increment the tcount value whenever interrupt rises
*/
void timer_hcsr04_event_handler(nrf_timer_event_t event_type, void* p_context)
{
  switch(event_type) {
    case NRF_TIMER_EVENT_COMPARE0:
    tCount++;
    break;
  default:
  //Do nothing.
     break;
  }
}

/*
* set up and start Timer4
*/
void init_dist_measurement(void)
{
  nrf_gpio_pin_dir_set(pinTrig, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(pinEcho, NRF_GPIO_PIN_DIR_INPUT);
  uint32_t time_ticks;
  uint32_t err_code = NRF_SUCCESS;

  // Configure TIMER_HCSR04
  err_code = nrf_drv_timer_init(&TIMER_HCSR04, NULL,
                                timer_hcsr04_event_handler);
  APP_ERROR_CHECK(err_code);
  time_ticks = 500;

  // set conversion factor
  int prescaler = 0;
  countToUs = 0.0625*time_ticks*(1 << prescaler);

  nrf_drv_timer_extended_compare(&TIMER_HCSR04,
                                 NRF_TIMER_CC_CHANNEL0,
                                 time_ticks,
                                 NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);

  nrf_drv_timer_enable(&TIMER_HCSR04);
}

/*
* function to set LED - red
*/
void red()
{
  nrf_gpio_pin_clear(led_R);
  nrf_gpio_pin_set(led_B);
  nrf_gpio_pin_set(led_G);
}

/*
* function to set LED - green
*/
void green()
{
  nrf_gpio_pin_clear(led_G);
  nrf_gpio_pin_set(led_B);
  nrf_gpio_pin_set(led_R);
}

/*
* function to set LED - blue
*/
void blue()
{
  nrf_gpio_pin_clear(led_B);
  nrf_gpio_pin_set(led_G);
  nrf_gpio_pin_set(led_R);
}

/*
* function to set LED - turquoise
*/
void turquoise()
{
  nrf_gpio_pin_clear(led_G);
  nrf_gpio_pin_set(led_R);
  nrf_gpio_pin_clear(led_B);
}

/*
* function to set LED - yellow
*/
void yellow()
{
  nrf_gpio_pin_clear(led_G);
  nrf_gpio_pin_clear(led_R);
  nrf_gpio_pin_set(led_B);
}

/*
* function to stop the bluey-ble-controlled-bot
*/
void stop()
{
  // LED - red
  red();
  while (app_pwm_channel_duty_set(&PWM1, 0, 0) == NRF_ERROR_BUSY);
  while (app_pwm_channel_duty_set(&PWM1, 1, 0) == NRF_ERROR_BUSY);
}

/*
* function to move the bot forward
*/
void front()
{
  // LED - green
  green();
  nrf_gpio_pin_clear(phase2);
  nrf_gpio_pin_clear(phase1);
  while (app_pwm_channel_duty_set(&PWM1, 0, 60) == NRF_ERROR_BUSY);
  while (app_pwm_channel_duty_set(&PWM1, 1, 60) == NRF_ERROR_BUSY);
}

/*
* function to move the bot backwards
*/
void reverse ()
{
  // LED - blue
  blue();
  nrf_gpio_pin_set(phase2);
  nrf_gpio_pin_set(phase1);
  while (app_pwm_channel_duty_set(&PWM1, 0, 40) == NRF_ERROR_BUSY);
  while (app_pwm_channel_duty_set(&PWM1, 1, 40) == NRF_ERROR_BUSY);
}

/*
* function to move the bot left
*/
void left()
{
  // LED - yellow
  yellow();
  nrf_gpio_pin_clear(23);
  nrf_gpio_pin_clear(24);
  while (app_pwm_channel_duty_set(&PWM1, 0, 40) == NRF_ERROR_BUSY);
  while (app_pwm_channel_duty_set(&PWM1, 1, 0) == NRF_ERROR_BUSY);
}

/*
* function to move the bot right
*/
void right()
{
  // LED - turquoise
  turquoise();
  nrf_gpio_pin_clear(23);
  nrf_gpio_pin_clear(24);
  while (app_pwm_channel_duty_set(&PWM1, 0, 0) == NRF_ERROR_BUSY);
  while (app_pwm_channel_duty_set(&PWM1, 1, 40) == NRF_ERROR_BUSY);
}

/*
* function to make the bot autonomous
*/
void auto_run()
{
   // move forward
  front();
  // compute the distance
  getDistance(&value);
  if(value <= 24) {
    // stop
    stop();
    nrf_delay_ms(500);
    // move backward
    reverse();
    nrf_delay_ms(600);
    // turn left
    left();
    nrf_delay_ms(500);
  }

}

/*
* function to initiate pwm_init
*/
void pwm_init(void)
{
  ret_code_t err_code;
  // setting two channel pwm for motors
  app_pwm_config_t pwm1_cfg = APP_PWM_DEFAULT_CONFIG_2CH(1000L,enable1,enable2);
  /* Switch the polarity of the channels. */
  pwm1_cfg.pin_polarity[0] = APP_PWM_POLARITY_ACTIVE_HIGH;
  pwm1_cfg.pin_polarity[1] = APP_PWM_POLARITY_ACTIVE_HIGH;
  /* Initialize and enable PWM. */
  err_code = app_pwm_init(&PWM1,&pwm1_cfg,pwm_ready_callback);
  APP_ERROR_CHECK(err_code);
  // enabling pulse width modulation
  app_pwm_enable(&PWM1);
}

/*
*initialization of BBevent
*/
typedef enum _BBEventType {
  eBBEvent_on,
  eBBEvent_off,
  eBBEvent_left,
  eBBEvent_right,
  eBBEvent_reverse,
} BBEventType;

typedef struct _BBEvent
{
  bool pending;
  BBEventType event;
  int data;
} BBEvent;

BBEvent bbEvent;

/*
* function to handle BBEvent
*/
void handle_bbevent(BBEvent* bbEvent)
{
  switch(bbEvent->event) {
    case eBBEvent_on:
    {
      front();
    }
    break;

    case eBBEvent_off:
    {
      stop();
    }
    break;

    case eBBEvent_left:
    {
      left();
    }
    break;

    case eBBEvent_right:
    {
      right();
    }
    break;

    case eBBEvent_reverse:
    {
      reverse();
    }

    default:
        break;
}
  // clear
  bbEvent->pending = false;
}


/*
* function for assert handler
*/
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
  app_error_handler(DEAD_BEEF, line_num, p_file_name);
}


/*
* function for the GAP initialization.
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
* function for handling the data from the Nordic UART Service.
*/

static void nus_data_handler(ble_nus_t * p_nus, uint8_t * p_data,
                             uint16_t length)
{
  // clear events
  bbEvent.pending = false;
  if (strstr((char*)(p_data), START)) {
    bbEvent.pending = true;
    bbEvent.event = eBBEvent_on;
  }
  else if (strstr((char*)(p_data), STOP)) {
    bbEvent.pending = true;
    bbEvent.event = eBBEvent_off;
  }
  else if (strstr((char*)(p_data), RIGHT)) {
    bbEvent.pending = true;
    bbEvent.event = eBBEvent_right;
  }
    else if (strstr((char*)(p_data), LEFT)) {
      bbEvent.pending = true;
      bbEvent.event = eBBEvent_left;
  }
  else if (strstr((char*)(p_data), REVERSE)) {
    bbEvent.pending = true;
    bbEvent.event = eBBEvent_reverse;
  }
}

/*
* function for initializing services that will be used by the application.
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


/*
* function for handling an event from the Connection Parameters Module.
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
*function for handling errors from the Connection Parameters module.
*/
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/*
* function for initializing the Connection Parameters module.
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
* function for putting the chip into sleep mode.
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
* function for handling advertising events.
*/
static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
  switch (ble_adv_evt)
    {
      case BLE_ADV_EVT_FAST:
      break;
      case BLE_ADV_EVT_IDLE:
      break;
      default:
      break;
    }
}


/*
*function for the application's SoftDevice event handler.
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
          break; // BLE_GATTS_EVT_SYS_ATTR_MISLEFTG

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
      } break;                                                                 // BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST

  #if (NRF_SD_BLE_API_VERSION == 3)
        case BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST:
            err_code = sd_ble_gatts_exchange_mtu_reply(p_ble_evt->evt.gatts_evt.conn_handle,
                                                       NRF_BLE_MAX_MTU_SIZE);
            APP_ERROR_CHECK(err_code);
            break;                                                               // BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST
#endif

        default:
            // No implementation needed.
            break;
    }
}


/*
*function for dispatching a SoftDevice event
*/
static void ble_evt_dispatch(ble_evt_t * p_ble_evt)
{
  ble_conn_params_on_ble_evt(p_ble_evt);
  ble_nus_on_ble_evt(&m_nus, p_ble_evt);
  on_ble_evt(p_ble_evt);
  ble_advertising_on_ble_evt(p_ble_evt);
  bsp_btn_ble_on_ble_evt(p_ble_evt);
}


/*
*function for the SoftDevice initialization.
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
* function for handling events from the BSP module.
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


/*
* function for handling app_uart events.
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
*function for initializing the UART module.
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


/*
*function for initializing the advertising functionality.
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

bool is_connected()
{
  return (m_conn_handle != BLE_CONN_HANDLE_INVALID);
}



/*
* this is the main function
*/

int main(void)
{
  uint32_t err_code;

  // configure LED pins
  nrf_gpio_pin_dir_set(led_R, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(led_G, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(led_B, NRF_GPIO_PIN_DIR_OUTPUT);

  //setting up the pin for Phase/Enable mode in DRV8835
  nrf_gpio_pin_dir_set(phase1, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(phase2, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(mode, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_set(mode);

  // Initialize.
  APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);
  uart_init();
  pwm_init();
  ble_stack_init();
  gap_params_init();
  services_init();
  advertising_init();
  conn_params_init();

  // start advertising
  err_code = ble_advertising_start(BLE_ADV_MODE_FAST);
  APP_ERROR_CHECK(err_code);

  // initialize HCSR-04
  init_dist_measurement();

  bool prevStateConnected = false;

  while(1) {
    if(is_connected()) {
      // stop if coming from an unconnected state
      if(!prevStateConnected) {
        stop();
      }

      if(bbEvent.pending) {
        handle_bbevent(&bbEvent);
      }
      prevStateConnected = true;
    }

    else {
      // start moving if previous state was connected
      if(prevStateConnected) {
        front();
      }
      auto_run();
      prevStateConnected = false;
    }
  }
}
