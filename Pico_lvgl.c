/**
 ******************************************************************************
 * @file        : Pico_lvgl_ssd1309.c
 * @brief       : Main program body
 ******************************************************************************
 * @attention
 * 
 * Copyright (c) 2025 Stuart Wood
 * All rights reserve. 
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 * 
 ******************************************************************************
 */

/* Includes ----------------------------------------------------------------*/
#include <stdio.h>
#include "pico/stdlib.h"
#include "lvgl/lvgl.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "lv_port_disp.h"

/*********************
 *      DEFINES
 *********************/
#define LED_PIN 		25
// I2C defines
#define I2C_PORT 		i2c0
#define I2C_SDA 		16
#define I2C_SCL 		17

static uint32_t my_tick(void) {
	return to_ms_since_boot(get_absolute_time());
}

// // lv_tick_get_cb_t get_millis_cb = get_millis;
// static uint32_t my_tick_get_cb(void) {
// 	return get_millis();
// }

void toggle_led(uint32_t timeout) {
	static uint32_t last_timeout;
	uint32_t millis = my_tick();
	if (millis - last_timeout > timeout) {
		last_timeout += timeout;
		// printf("[%6d]: ", millis);
		// printf("Hello, world!\n");
		gpio_put(LED_PIN, !gpio_get(LED_PIN));
	}
}

int main()
{
    stdio_init_all();

	sleep_ms(5000);

	// Default LED
    gpio_init(LED_PIN);
	gpio_set_dir(LED_PIN, GPIO_OUT);

	// I2C Initialisation. Using it at 100Khz.
    i2c_init(I2C_PORT, 400*1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

	/*
	 * LVGL Initialization
	 */
	// Tick Initialization must come first before lv_init()
	lv_tick_set_cb(my_tick);
	lv_init();
	lv_port_disp_init();

	lv_obj_t *label = lv_label_create(lv_screen_active());
	lv_label_set_text(label, "Hello World");
	lv_obj_set_style_text_color(lv_screen_active(), lv_color_black(), LV_PART_MAIN);
	lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

	printf("Starting Main loop\n");
	while (true) {
		toggle_led(1000);
		lv_timer_handler_run_in_period(10);                     
    } 
}
