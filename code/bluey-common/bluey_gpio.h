/*
RGB LED

nRF52 gpio configuration for LED test..

Electronut Labs
electronut.in

*/

#include "nrf_gpio.h"

#define led_R                 19
#define led_G                 18
#define led_B                 17

#define button_sw3            16

/*
 * function to configure RGB LED
*/
void rgb_led_config(void);

/*
 * function to turn all LEDs off
*/
void rgb_led_off(void);

/*
 * function to turn all LEDs on
*/
void rgb_led_set_white(void);

/*
 * function to turn LED on - RED
*/
void rgb_led_set_red(void);

/*
 * function to turn LED on - GREEN
*/
void rgb_led_set_green(void);

/*
 * function to turn LED on - BLUE
*/
void rgb_led_set_blue(void);

/*
 * function to turn LED on - YELLOW
*/
void rgb_led_set_yellow(void);

/*
 * function to turn LED on - CYAN
*/
void rgb_led_set_cyan(void);

/*
 * function to turn LED on - MAGENTA
*/
void rgb_led_set_magenta(void);

/*
 * function to test on-board gpio pins
*/
void cycle_gpio(void);
