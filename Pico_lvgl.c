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
#include "sh1106.h"
#include "lvgl/lvgl.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

#define LED_PIN 				25

// #define sh1106_HEIGHT			64
// #define sh1106_WIDTH			128
// #define sh1106_BUFFER_SIZE		((sh1106_WIDTH * sh1106_HEIGHT) / 4)

#define BIT_SET(a, b)   ((a) |= (1U << (b)))
#define BIT_CLEAR(a, b) ((a) &= ~(1U << (b)))

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 16
#define I2C_SCL 17

// void sh1106_flush_cb(lv_display_t *disp_drv, const lv_area_t *area, uint8_t *px_map) {

// 	uint8_t row1 = area->y1 >> 3;
// 	uint8_t row2 = area->y2 >> 3;
// 	px_map += 8; // To skip the palette, since we are monochrom display
// 	uint8_t *buf = (uint8_t*) px_map;

// 	for (uint8_t row = row1; row <= row2; row++) {

// 		sh1106_wr_cmd(display, 0xB0 | row);          // Set the page start address
// 		sh1106_wr_cmd(display, 0x00 | (area->x1 & 0xF)); // Set the lower start column address
// 		sh1106_wr_cmd(display, 0x10 | ((area->x1 >> 4) & 0xF)); // Set the upper start column address

// 		for (uint16_t x = area->x1; x <= area->x2; x++) {
// 			write_data(display, buf, 1);
// 			buf++;
// 		}
// 	}
// 	/* IMPORTANT!!!
//      * Inform LVGL that you are ready with the flushing and buf is not used anymore*/ 
// 	lv_disp_flush_ready(disp_drv);
// }

uint32_t get_millis(void) {
	return to_ms_since_boot(get_absolute_time());
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
bool reserved_addr(uint8_t addr) {
    return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}

void scanI2C() {
    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));

    printf("\nI2C Bus Scan\n");
    printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");

    for (int addr = 0; addr < (1 << 7); ++addr) {
        if (addr % 16 == 0) {
            printf("%02x ", addr);
        }

        // Perform a 1-byte dummy read from the probe address. If a slave
        // acknowledges this address, the function returns the number of bytes
        // transferred. If the address byte is ignored, the function returns
        // -1.

        // Skip over any reserved addresses.
        int ret;
        uint8_t rxdata;
        if (reserved_addr(addr))
            ret = PICO_ERROR_GENERIC;
        else
            ret = i2c_read_blocking(i2c_default, addr, &rxdata, 1, false);

        printf(ret < 0 ? "." : "@");
        printf(addr % 16 == 15 ? "\n" : "  ");
    }
    printf("Done.\n");
// #endif
}


int main()
{
    stdio_init_all();

	sleep_ms(5000);

	// uint8_t buffer[sh1106_BUF_SIZE] = { 0xFF };  // all elements 0xFF
	
	// Default LED

    gpio_init(LED_PIN);
	gpio_set_dir(LED_PIN, GPIO_OUT);

	// I2C Initialisation. Using it at 100Khz.
    i2c_init(I2C_PORT, 100*1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

	scanI2C();

	printf("Starting sh1106 Display Init\n");
	sh1106_t display = sh1106_init(I2C_PORT, 0x3C, false, false, false);

	/*
	 * LVGL Initialization
	 */
	printf("Starting LVGL Init\n");
	lv_init();
	printf("LVGL Init\n");

	// Tick Initialization
	lv_tick_get_cb_t get_millis_cb = get_millis;
	lv_tick_set_cb(get_millis_cb);

	// lv_display_t * display = lv_display_create(sh1106_WIDTH, sh1106_HEIGHT);
	// if (!display) {
	//   printf("LVGL display creation failed!\n");
	//   while (1);
	// }
	// printf("LVGL display created\n");
	
	// // Allocate the display buffer
	// static uint8_t display_buf[(sh1106_HEIGHT * sh1106_WIDTH)/8 + 8];
	// if (!display_buf) {
	// 	printf("Display buffer allocation failed!\n");
	// 	while (1);
	// }
	// printf("Buffer Allocated\n");

	// Configure the display
	// lv_display_set_buffers(display, display_buf, NULL, sizeof(display_buf), LV_DISPLAY_RENDER_MODE_PARTIAL);
	// printf("Display Buffer set\n");

	// lv_display_set_flush_cb(display, sh1106_flush_cb);
	// printf("Flush callback set\n");

	// lv_display_set_render_mode(display, LV_DISPLAY_RENDER_MODE_PARTIAL);
	
	printf("Starting Mail loop\n");
	static bool invert = false;
	uint32_t millis = 0;
	uint32_t old_millis = 0;
	while (true) {
		toggle_led(1000);
		millis = get_millis();
		if (millis - old_millis > 2000) {
			old_millis += 2000;
			sh1106_inverted(display, invert);
			invert = !invert;
		}
		// lv_timer_handler();                     
    }
}
