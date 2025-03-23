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

#define BYTE_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_I1))

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);
static void disp_flush(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);
/**
 * Convert a draw buffer in I1 color format from htiled (row-wise)
 * to vtiled (column-wise) buffer layout. The conversion assumes that the buffer width
 * and height is rounded to a multiple of 8.
 * @param buf           pointer to the buffer to be converted
 * @param buf_size      size of the buffer in bytes
 * @param width         width of the buffer
 * @param height        height of the buffer
 * @param out_buf       pointer to the output buffer
 * @param out_buf_size  size of the output buffer in bytes
 * @param bit_order_lsb bit order of the resulting vtiled buffer
 */
void lv_draw_sw_i1_convert_to_vtiled(const void * buf, uint32_t buf_size, uint32_t width, uint32_t height,
    void * out_buf,
    uint32_t out_buf_size, bool bit_order_lsb);

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
    printf("SH1106 Display Init\n");
    disp_init();

	printf("LVGL Init\n");
    
    /*------------------------------------
     * Create a display and set a flush_cb
     * -----------------------------------*/
    lv_display_t * disp = lv_display_create(SH1106_WIDTH, SH1106_HEIGHT);
    lv_display_set_flush_cb(disp, disp_flush);

    // Allocate the display buffer
	LV_ATTRIBUTE_MEM_ALIGN
	static uint8_t disp_buf_1[SH1106_BUF_SIZE + 8];
	LV_ATTRIBUTE_MEM_ALIGN
	static uint8_t disp_buf_2[SH1106_BUF_SIZE + 8];
	lv_display_set_buffers(disp, disp_buf_1, disp_buf_2, sizeof(disp_buf_1), LV_DISPLAY_RENDER_MODE_FULL);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
	printf("Starting sh1106 Display Init\n");
	sh1106_init(&sh1106, I2C_PORT, 0x3C);
    sh1106_flush_buffer(&sh1106, BOX);
 	sh1106_inverted(&sh1106, false);
    sleep_ms(3000);
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

void lv_draw_sw_i1_convert_to_vtiled(const void * buf, uint32_t buf_size, uint32_t width, uint32_t height, 
    void * out_buf, uint32_t out_buf_size, bool bit_order_lsb) {

    LV_ASSERT(buf && out_buf);
    LV_ASSERT(width % 8 == 0 && height % 8 == 0);
    LV_ASSERT(buf_size >= (width / 8) * height);
    LV_ASSERT(out_buf_size >= buf_size);

    lv_memset(out_buf, 0, out_buf_size);

    const uint8_t * src_buf = (uint8_t *)buf;
    uint8_t * dst_buf = (uint8_t *)out_buf;

    for(uint32_t y = 0; y < height; y++) {
        for(uint32_t x = 0; x < width; x++) {
            uint32_t src_index = (x + (y * width)) >> 3;
            uint32_t dst_index = x + (y >> 3) * width;
            uint8_t bit = (src_buf[src_index] >> (7 - (x % 8))) & 0x01;
            if(bit_order_lsb) {
                dst_buf[dst_index] |= (bit << (y % 8));
            }
            else {
                dst_buf[dst_index] |= (bit << (7 - (y % 8)));
            }
        }
    }
}

/*Flush the content of the internal buffer the specific area on the display.
 *`px_map` contains the rendered image as raw pixel map and it should be copied to `area` on the display.
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_display_flush_ready()' has to be called when it's finished.*/
static void disp_flush(lv_display_t *disp_drv, const lv_area_t * area, uint8_t *px_map)
{
    if(disp_flush_enabled) {
        printf("display flush:\n");
        printf("X Range: %d-%d\n", area->x1, area->x2);
        printf("Y Range: %d-%d\n", area->y1, area->y2);

        // Transpose row based data to column 
        px_map += 8; // Skip over the color map.
        uint8_t buffer[SH1106_BUF_SIZE];

        // Now we need to transpose the horizontal data that LVGL uses in to 
        // vertical aligned data in the 8 vertical pages.
        lv_draw_sw_i1_convert_to_vtiled(px_map, SH1106_BUF_SIZE, SH1106_WIDTH,
            SH1106_HEIGHT, buffer, SH1106_BUF_SIZE, true);

        sh1106_write_area(&sh1106, (uint8_t)area->x1, (uint8_t)area->x2, 
            (uint8_t)area->y1, (uint8_t)area->y2, buffer);
        // sh1106_write_screen(&sh1106, px_map);
    }
 
    /*IMPORTANT!!!
     *Inform the graphics library that you are ready with the flushing*/
    lv_display_flush_ready(disp_drv);
}

#else /*Enable this file at the top*/
/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
