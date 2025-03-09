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

static uint32_t get_millis(void) {
	return to_ms_since_boot(get_absolute_time());
}

// lv_tick_get_cb_t get_millis_cb = get_millis;
static uint32_t my_tick_get_cb(void) {
	return get_millis();
}

void toggle_led(uint32_t timeout) {
	static uint32_t last_timeout;
	uint32_t millis = get_millis();
	if (millis - last_timeout > timeout) {
		last_timeout += timeout;
		// printf("[%6d]: ", millis);
		// printf("Hello, world!\n");
		gpio_put(LED_PIN, !gpio_get(LED_PIN));
	}
}

// I2C reserves some addresses for special purposes. We exclude these from the scan.
// These are any addresses of the form 000 0xxx or 111 1xxx
// bool reserved_addr(uint8_t addr) {
//     return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
// }

// void scanI2C() {
//     // Make the I2C pins available to picotool
//     bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));

//     printf("\nI2C Bus Scan\n");
//     printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");

//     for (int addr = 0; addr < (1 << 7); ++addr) {
//         if (addr % 16 == 0) {
//             printf("%02x ", addr);
//         }

//         // Perform a 1-byte dummy read from the probe address. If a slave
//         // acknowledges this address, the function returns the number of bytes
//         // transferred. If the address byte is ignored, the function returns
//         // -1.

//         // Skip over any reserved addresses.
//         int ret;
//         uint8_t rxdata;
//         if (reserved_addr(addr))
//             ret = PICO_ERROR_GENERIC;
//         else
//             ret = i2c_read_blocking(i2c_default, addr, &rxdata, 1, false);

//         printf(ret < 0 ? "." : "@");
//         printf(addr % 16 == 15 ? "\n" : "  ");
//     }
//     printf("Done.\n");
// }


int main()
{
    stdio_init_all();

	sleep_ms(5000);

	// uint8_t buffer[sh1106_BUF_SIZE] = { 0xFF };  // all elements 0xFF
	
	// Default LED

    gpio_init(LED_PIN);
	gpio_set_dir(LED_PIN, GPIO_OUT);

	// I2C Initialisation. Using it at 100Khz.
    i2c_init(I2C_PORT, 400*1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

	// scanI2C();

	/*
	 * LVGL Initialization
	 */

	// Tick Initialization must come first before lv_init()
	lv_tick_set_cb(my_tick_get_cb);
	lv_init();
	lv_port_disp_init();

	printf("Starting Main loop\n");
	static bool invert = false;
	uint32_t millis = 0;
	uint32_t old_millis = 0;
	while (true) {
		toggle_led(1000);
		lv_timer_handler_run_in_period(10);                     
    }
}
