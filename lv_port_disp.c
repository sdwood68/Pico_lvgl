/**
 * @file lv_port_disp_templ.c
 *
 */

/*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp.h"
#include "sh1106.h"
#include <stdbool.h>
#include "images/Box.h"
#include "images/circle_16x16x3.h"


/*********************
 *      DEFINES
 *********************/
#ifndef MY_DISP_HOR_RES
    #define MY_DISP_HOR_RES    128
#endif

#ifndef MY_DISP_VER_RES
    #define MY_DISP_VER_RES    64
#endif

#define BYTE_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_I1)) /*will be 2 for RGB565 */

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);
static void disp_flush(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);

/**********************
 *  STATIC VARIABLES
 **********************/
static sh1106_t sh1106;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_disp_init(void)
{
    /*-------------------------
     * Initialize your display
     * -----------------------*/
    disp_init();

	printf("Starting LVGL Init\n");
    printf("LVGL Init\n");

    /*------------------------------------
     * Create a display and set a flush_cb
     * -----------------------------------*/
    lv_display_t * disp = lv_display_create(MY_DISP_HOR_RES, MY_DISP_VER_RES);
    lv_display_set_flush_cb(disp, disp_flush);

    // Allocate the display buffer
	LV_ATTRIBUTE_MEM_ALIGN
	static uint8_t disp_buf_1[MY_DISP_HOR_RES * MY_DISP_VER_RES * BYTE_PER_PIXEL];
	LV_ATTRIBUTE_MEM_ALIGN
	static uint8_t disp_buf_2[MY_DISP_HOR_RES * MY_DISP_VER_RES * BYTE_PER_PIXEL];
	lv_display_set_buffers(disp, disp_buf_1, disp_buf_2, sizeof(disp_buf_1), LV_DISPLAY_RENDER_MODE_FULL);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
	printf("Starting sh1106 Display Init\n");
	sh1106_init(&sh1106, I2C_PORT, 0x3C, 128, 64, false, false, false);
	sh1106_flush_buffer(&sh1106, BOX);
 	sh1106_inverted(&sh1106, true);
	// sleep_ms(2000);
	// sh1106_flush_area(&sh1106, 8, 8, 16, 16, CIRCLE_16X16X3);
	// sleep_ms(2000);
	// sh1106_flush_area(&sh1106, 24, 12, 16, 16, CIRCLE_16X16X3);
}

volatile bool disp_flush_enabled = true;

/* Enable updating the screen (the flushing process) when disp_flush() is called by LVGL */
void disp_enable_update(void)
{
    disp_flush_enabled = true;
}

/* Disable updating the screen (the flushing process) when disp_flush() is called by LVGL */
void disp_disable_update(void)
{
    disp_flush_enabled = false;
}

/*Flush the content of the internal buffer the specific area on the display.
 *`px_map` contains the rendered image as raw pixel map and it should be copied to `area` on the display.
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_display_flush_ready()' has to be called when it's finished.*/
static void disp_flush(lv_display_t * disp_drv, const lv_area_t * area, uint8_t * px_map)
{
    if(disp_flush_enabled) {
        printf("disp_flush()\n");
        /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/

        // int32_t x;
        // int32_t y;
        // for(y = area->y1; y <= area->y2; y++) {
        //     for(x = area->x1; x <= area->x2; x++) {
        //         /*Put a pixel to the display. For example:*/
        //         /*put_px(x, y, *px_map)*/
        //         px_map++;
        //     }
        // }
        
        sh1106_flush_buffer(&sh1106, px_map);
    }

    /*IMPORTANT!!!
     *Inform the graphics library that you are ready with the flushing*/
    lv_display_flush_ready(disp_drv);
}

#else /*Enable this file at the top*/
/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
