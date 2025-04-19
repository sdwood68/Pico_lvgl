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
#include "lv_port_indev.h"
#include "Pico_lvgl.h"
#include "ui.h"

static uint32_t my_tick(void) {

	return to_ms_since_boot(get_absolute_time());
}

void toggle_led_period(uint32_t timeout) {
	static uint32_t last_timeout;
	uint32_t millis = my_tick();
	if (millis - last_timeout > timeout) {
		last_timeout += timeout;
		// printf("toggle_led [%6d]\n", millis);
		gpio_put(LED_PIN, !gpio_get(LED_PIN));
	}
}


int main()
{
    stdio_init_all();

	sleep_ms(5000);

	// Default LED
    gpio_init_mask(LED_MASK + KEY_MASK);
	gpio_set_dir_out_masked(LED_MASK);
	gpio_pull_up(UP_PIN);
	gpio_pull_up(DOWN_PIN);
	gpio_pull_up(LEFT_PIN);
	gpio_pull_up(RIGHT_PIN);
	gpio_pull_up(ENT_PIN);

	// I2C Initialisation. Using it at 100Khz.
    i2c_init(I2C_PORT, 400*1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

	/*
	 * LVGL Initialization
	 */
	lv_init();
	lv_port_disp_init();
	lv_port_indev_init();
	lv_tick_set_cb(my_tick);

	/*
	 * LVGL
	 */
	ui_init();
	// lv_obj_set_style_bg_color(lv_screen_active(), lv_color_white(),LV_PART_MAIN);
	// lv_obj_t *label = lv_label_create(lv_screen_active());
	// lv_label_set_text(label, "Hello World");
	// lv_obj_set_style_text_color(lv_screen_active(), lv_color_black(), LV_PART_MAIN);
	// lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
	// sleep_ms(5000);

	// // Create a label to display key presses
	// lv_obj_t *label_key = lv_label_create(lv_scr_act());
	// lv_label_set_text(label_key, "Press a key");
	// lv_obj_align(label_key, LV_ALIGN_CENTER, 0, 0);

	// // Create a group and add the label to it
	// lv_group_t * group = lv_group_create();
	// lv_group_add_obj(group, label_key);
	// lv_indev_set_group(keypad, group);


	printf("Starting Main loop\n");
	while (true) {
		toggle_led_period(500);
		lv_timer_handler_run_in_period(5);             
    } 
}
