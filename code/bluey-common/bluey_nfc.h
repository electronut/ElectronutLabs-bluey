/**
 * bluey_nfc.h
 *
 * nRF52 NFC interface to obtain temperature data.
 *
 * Electronut Labs
 * electronut.in
*/

#include <stdint.h>
#include "nfc_t2t_lib.h"
#include "nfc_ndef_msg.h"
#include "nfc_text_rec.h"
#include "boards.h"
#include "app_error.h"
#include "hardfault.h"

#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#define MAX_REC_COUNT      2     /**< Maximum records count. */

extern uint8_t m_ndef_msg_buf[256];

/**
 * @brief Callback function for handling NFC events.
 */
void nfc_callback(void * p_context, nfc_t2t_event_t event, const uint8_t * p_data, size_t data_length);


/**
 * @brief Function for encoding the welcome message.
 */
void encode_data(uint8_t * p_buffer, uint32_t * p_len);
