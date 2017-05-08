/*
RGB LED

nRF52 gpio configuration for LED test..

Electronut Labs
electronut.in

*/

#include "led-and-button.h"

/*
 * function to configure RGB LED
*/
void rgb_led_config(void)
{
  nrf_gpio_pin_dir_set(led_R, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(led_G, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(led_B, NRF_GPIO_PIN_DIR_OUTPUT);

  rgb_led_off();
}

/*
 * function to turn all LEDs off
*/
void rgb_led_off(void)
{
  nrf_gpio_pin_set(led_R);
  nrf_gpio_pin_set(led_G);
  nrf_gpio_pin_set(led_B);
}

/*
 * function to turn all LEDs on
*/
void rgb_led_set_white(void)
{
  nrf_gpio_pin_clear(led_R);
  nrf_gpio_pin_clear(led_G);
  nrf_gpio_pin_clear(led_B);
}

/*
 * function to turn LED on - RED
*/
void rgb_led_set_red(void)
{
  nrf_gpio_pin_clear(led_R);
  nrf_gpio_pin_set(led_G);
  nrf_gpio_pin_set(led_B);
}

/*
 * function to turn LED on - GREEN
*/
void rgb_led_set_green(void)
{
  nrf_gpio_pin_set(led_R);
  nrf_gpio_pin_clear(led_G);
  nrf_gpio_pin_set(led_B);
}

/*
 * function to turn LED on - BLUE
*/
void rgb_led_set_blue(void)
{
  nrf_gpio_pin_set(led_R);
  nrf_gpio_pin_set(led_G);
  nrf_gpio_pin_clear(led_B);
}

/*
 * function to turn LED on - YELLOW
*/
void rgb_led_set_yellow(void)
{
  nrf_gpio_pin_clear(led_R);
  nrf_gpio_pin_clear(led_G);
  nrf_gpio_pin_set(led_B);
}

/*
 * function to turn LED on - CYAN
*/
void rgb_led_set_cyan(void)
{
  nrf_gpio_pin_set(led_R);
  nrf_gpio_pin_clear(led_G);
  nrf_gpio_pin_clear(led_B);
}

/*
 * function to turn LED on - MAGENTA
*/
void rgb_led_set_magenta(void)
{
  nrf_gpio_pin_clear(led_R);
  nrf_gpio_pin_set(led_G);
  nrf_gpio_pin_clear(led_B);
}
