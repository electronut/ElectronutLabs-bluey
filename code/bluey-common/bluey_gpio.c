/*
RGB LED

nRF52 gpio configuration for LED test..

Electronut Labs
electronut.in

*/

#include "bluey_gpio.h"
#include "nrf_delay.h"

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

/*
 * function to test on-board gpio pins
*/
void cycle_gpio(void)
{
  uint8_t gpio[] = {26, 2, 27, 3, 28, 4, 29, 22, 30, 23, 31, 24, 25};       // gpio pins
  uint8_t i;
  static bool gpio_config = false;

  // configure gpio pins as output
  if(gpio_config == false) {
    for(i = 0; i < 13; i++) {
      nrf_gpio_pin_dir_set(gpio[i], NRF_GPIO_PIN_DIR_OUTPUT);
    }
    gpio_config = true;
  }

  // toggle gpio pins
  for(i = 0; i < 13; i++) {
    nrf_gpio_pin_set(gpio[i]);
    nrf_delay_ms(200);
    nrf_gpio_pin_clear(gpio[i]);
    nrf_delay_ms(200);
  }
}
