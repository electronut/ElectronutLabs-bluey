/**
 * bluey_nfc.c
 *
 * nRF52 NFC interface to obtain temperature data.
 *
 * Electronut Labs
 * electronut.in
 */

#include "bluey_nfc.h"
#include "HDC1010.h"

uint8_t m_ndef_msg_buf[256];

/**
 * @brief Callback function for handling NFC events.
 */
void nfc_callback(void * p_context, nfc_t2t_event_t event, const uint8_t * p_data, size_t data_length)
{
    (void)p_context;

    switch (event)
    {
        case NFC_T2T_EVENT_FIELD_ON:
            bsp_board_led_on(BSP_BOARD_LED_0);
            break;
        case NFC_T2T_EVENT_FIELD_OFF:
            bsp_board_led_off(BSP_BOARD_LED_0);
            break;
        default:
            break;
    }
}


/**
 * @brief function to add temperature record.
 */
void add_temperature_record(nfc_ndef_msg_desc_t * p_ndef_msg_desc)
{
    ret_code_t err_code;
    float temperature;
    static uint8_t str[20];

    temperature = HDC1010_get_temp();

    sprintf((char *)str, "Temperature: %.2f C", temperature);

    static const uint8_t en_code[] = {'t'};

    NFC_NDEF_TEXT_RECORD_DESC_DEF(temp_text_rec,
                                  UTF_8,
                                  en_code,
                                  sizeof(en_code),
                                  str,
                                  sizeof(str));

    err_code = nfc_ndef_msg_record_add(p_ndef_msg_desc,
                                       &NFC_NDEF_TEXT_RECORD_DESC(temp_text_rec));
    APP_ERROR_CHECK(err_code);

}

/**
 * @brief function to add humidity record.
 */
void add_humidity_record(nfc_ndef_msg_desc_t * p_ndef_msg_desc)
{
    uint32_t             err_code;
    float humidity;
    static uint8_t str[20];

    humidity = HDC1010_get_humid();

    sprintf((char *)str, "Humidity: %.2f %%", humidity);

    static const uint8_t en_code[] = {'h'};

    NFC_NDEF_TEXT_RECORD_DESC_DEF(humid_text_rec,
                                  UTF_8,
                                  en_code,
                                  sizeof(en_code),
                                  str,
                                  sizeof(str));

    err_code = nfc_ndef_msg_record_add(p_ndef_msg_desc,
                                       &NFC_NDEF_TEXT_RECORD_DESC(humid_text_rec));
    APP_ERROR_CHECK(err_code);

}

/**
 * @brief Function for encoding the welcome message.
 */
void encode_data(uint8_t * p_buffer, uint32_t * p_len)
{
    NFC_NDEF_MSG_DEF(data, MAX_REC_COUNT);

    nfc_ndef_msg_clear(&NFC_NDEF_MSG(data));

    add_temperature_record(&NFC_NDEF_MSG(data));
    add_humidity_record(&NFC_NDEF_MSG(data));

    uint32_t err_code = nfc_ndef_msg_encode(&NFC_NDEF_MSG(data),
                                            p_buffer,
                                            p_len);
    APP_ERROR_CHECK(err_code);
}
