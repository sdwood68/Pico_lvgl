/**
 * This Library was originally written by Olivier Van den Eede (4ilo) in 2016.
 * Some refactoring was done and SPI support was added by Aleksander Alekseev (afiskon) in 2018.
 *
 * https://github.com/afiskon/stm32-sh1106
 */

#ifndef __sh1106_H__
#define __sh1106_H__

#include "hardware/i2c.h"
#include "Pico_lvgl.h"

/* vvv I2C config vvv */
#ifndef I2C_SDA
#define I2C_SDA 16
#endif

#ifndef I2C_SCL
#define I2C_SCL 17
#endif

#ifndef I2C_PORT
#define I2C_PORT		i2c0
#endif

#ifndef ADDR
#define ADDR        (0x3C)
#endif
/* ^^^ I2C config ^^^ */

// sh1106 maximum height in pixels
#ifndef SH1106_MAX_HEIGHT
#define SH1106_MAX_HEIGHT          64
#endif

// sh1106 maximum width in pixels
#ifndef SH1106_MAX_WIDTH
#define SH1106_MAX_WIDTH           132
#endif



// Enumeration for screen colors
typedef enum {
    Black = 0x00, // Black color, no pixel
    White = 0x01  // Pixel is set. Color depends on OLED
} COLOR_t;

// Struct to store transformations
typedef struct {
    i2c_inst_t *port;
    uint8_t addr; 
    int8_t col_offset;
    uint8_t width;
    uint8_t height;
    bool inverted;
    bool flipped; 
    bool mirrored; 
    uint8_t page_buffer[SH1106_MAX_WIDTH+1];
    uint8_t cmd_buffer[4];
} sh1106_t;

void sh1106_reset(sh1106_t *disp);

void sh1106_col_start(sh1106_t *disp, uint8_t column);
void ssh1106_chargepump(sh1106_t *disp, uint8_t value);
void sh1106_start_line(sh1106_t *disp, uint8_t row);
void sh1106_contrast(sh1106_t *disp, uint8_t value);
void sh1106_reverse_cols(sh1106_t *disp, bool mirrored);
void sh1106_norm_display(sh1106_t *disp, bool norm);
void sh1106_inverted(sh1106_t *disp, bool invert);
void sh1106_multiplex(sh1106_t *disp, uint8_t ratio);
void sh1106_power_on(sh1106_t *disp, bool on);
void sh1106_display_on(sh1106_t *disp, bool on);
void sh1106_page(sh1106_t *disp, uint8_t page);
void sh1106_flipped(sh1106_t *disp, bool flipped);
void sh1106_vert_offset(sh1106_t *disp, uint8_t offset);
void sh1106_clk_div(sh1106_t *disp, uint8_t divider, uint8_t freq);
void sh1106_dis_pre_charge(sh1106_t *disp, uint8_t discharge, uint8_t precharge);
void sh1106_com_pads(sh1106_t *disp, bool sequential);
void sh1106_vcomp(sh1106_t *disp, uint8_t level);

int sh1106_flush_buffer(sh1106_t *disp, uint8_t *buffer);
int sh1106_flush_area(sh1106_t *disp, uint8_t x, uint8_t y, uint8_t x_size, uint8_t y_size, uint8_t *buffer);
void sh1106_clear_display(sh1106_t *disp);

void sh1106_init(sh1106_t *disp, i2c_inst_t *port, uint8_t addr, uint8_t width, uint8_t height, bool inverted, bool flipped, bool mirrored);

#endif // __sh1106_H__
