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
 #include "ssd1306.h"
 #include "lvgl/lvgl.h"
 
#define SSD1306_HEIGHT			64
#define SSD1306_WIDTH			128
#define SSD1306_BUFFER_SIZE		((SSD1306_WIDTH * SSD1306_HEIGHT) / 4)

#define BIT_SET(a, b)   ((a) |= (1U << (b)))
#define BIT_CLEAR(a, b) ((a) &= ~(1U << (b)))

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9

void ssd1306_flush_cb(lv_display_t *disp_drv, const lv_area_t *area, uint8_t *px_map) {

	uint8_t row1 = area->y1 >> 3;
	uint8_t row2 = area->y2 >> 3;
	uint8_t *buf = (uint8_t*) px_map;

	for (uint8_t row = row1; row <= row2; row++) {

		ssd1306_WriteCommand(0xB0 | row);          // Set the page start address
		ssd1306_WriteCommand(0x00 | (area->x1 & 0xF)); // Set the lower start column address
		ssd1306_WriteCommand(0x10 | ((area->x1 >> 4) & 0xF)); // Set the upper start column address

		for (uint16_t x = area->x1; x <= area->x2; x++) {
			ssd1306_WriteData(buf, 1);
			buf++;
		}
	}
	/* IMPORTANT!!!
     * Inform LVGL that you are ready with the flushing and buf is not used anymore*/ 
	lv_disp_flush_ready(disp_drv);
}

// static void set_px_cb(struct _lv_disp_drv_t *disp_drv, uint8_t *buf,
//     lv_coord_t buf_w, lv_coord_t x, lv_coord_t y, lv_color_t color,
//     lv_opa_t opa) {
// 	(void) disp_drv;
// 	(void) opa;

// 	uint16_t byte_index = x + ((y >> 3) * buf_w);
// 	uint8_t bit_index = y & 0x7;
// 	// == 0 inverts, so we get blue on black
// 	if (color.full == 0) {
// 		BIT_SET(buf[byte_index], bit_index);
// 	} else {
// 		BIT_CLEAR(buf[byte_index], bit_index);
// 	}
// }

// static void rounder_cb(struct _lv_disp_drv_t *disp_drv, lv_area_t *area) {
// 	(void) disp_drv;
// 	area->y1 = (area->y1 & (~0x7));
// 	area->y2 = (area->y2 & (~0x7)) + 7;
// }

// static void lv_port_disp_init(void) {
// 	static lv_disp_draw_buf_t draw_buf; /* Descriptor of a display buffer */
// 	static lv_color_t screenBuffer1[SSD1306_BUFFER_SIZE]; /* Memory area used as display buffer */
// 	static lv_color_t screenBuffer2[SSD1306_BUFFER_SIZE]; /* Memory area used as display buffer */
// 	lv_disp_draw_buf_init(&draw_buf, screenBuffer1, screenBuffer2,
// 	SSD1306_BUFFER_SIZE); /* Initialize the display buffer */

// 	/*-----------------------------------
// 	 * Register the display in LVGL
// 	 *----------------------------------*/
// 	static lv_disp_drv_t disp_drv_ssd1306; /* Descriptor of a display driver */
// 	lv_disp_drv_init(&disp_drv_ssd1306); /* Basic initialization */

// 	/*Set up the functions to access to your display*/
// 	/*Set the resolution of the display*/
// 	disp_drv_ssd1306.hor_res = SSD1306_WIDTH; /** < Horizontal resolution */
// 	disp_drv_ssd1306.ver_res = SSD1306_HEIGHT; /** < Vertical resolution */
// 	disp_drv_ssd1306.full_refresh = 1; /** < 1: Always make the whole screen redrawn */
// 	disp_drv_ssd1306.rotated = LV_DISP_ROT_NONE; /** < 1: turn the display by 90 degree. Does not update coordinates for you! */

// 	/* Used to copy the buffer's content to the display */
// 	disp_drv_ssd1306.flush_cb = display_flush; /** < Write the internal buffer (draw_buf) to the display */
// 	disp_drv_ssd1306.rounder_cb = rounder_cb;
// 	disp_drv_ssd1306.set_px_cb = set_px_cb;

// 	/* Set a display buffer */
// 	disp_drv_ssd1306.draw_buf = &draw_buf;

// 	/* Finally register the driver */
// 	lv_disp_drv_register(&disp_drv_ssd1306);
// }


uint32_t get_millis_cb(void) {
	return to_ms_since_boot(get_absolute_time());
}

// bool lv_inc_timer_cb(repeating_timer_t *rt) {
//     lv_tick_inc(*((uint8_t*)rt->user_data));
//     return true;
// }

// void pico_lvgl_tick_inc_timer_init(uint8_t tick_inc) {
//     static repeating_timer_t rt;
//     static uint8_t _tick_inc;
//     _tick_inc = tick_inc;
//     add_repeating_timer_ms(tick_inc, lv_inc_timer_cb, (void*) (&_tick_inc), &rt);
// }

int main()
{
    stdio_init_all();

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

	ssd1306_Init();

	lv_init();
	lv_display_t * display = lv_display_create(SSD1306_WIDTH, SSD1306_HEIGHT);
	lv_tick_set_cb(get_millis_cb);
	static uint8_t buf[SSD1306_HEIGHT * SSD1306_WIDTH];
	lv_display_set_buffers(display, buf, NULL, sizeof(buf), LV_DISPLAY_RENDER_MODE_PARTIAL);
	// lv_port_disp_init();
	
    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
		lv_timer_handler();
    }
}
