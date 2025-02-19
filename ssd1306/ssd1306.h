/**
 * This Library was originally written by Olivier Van den Eede (4ilo) in 2016.
 * Some refactoring was done and SPI support was added by Aleksander Alekseev (afiskon) in 2018.
 *
 * https://github.com/afiskon/stm32-ssd1306
 */

#ifndef __SSD1306_H__
#define __SSD1306_H__

#include "hardware/i2c.h"
#include "Pico_lvgl_ssd1306.h"


/* vvv I2C config vvv */
#define I2C_SDA 8
#define I2C_SCL 9

#ifndef SSD1306_I2C_PORT
#define SSD1306_I2C_PORT		i2c0
#endif

#ifndef SSD1306_I2C_ADDR
#define SSD1306_I2C_ADDR        (0x78)
#endif
/* ^^^ I2C config ^^^ */

// SSD1306 OLED height in pixels
#ifndef SSD1306_HEIGHT
#define SSD1306_HEIGHT          64
#endif

// SSD1306 width in pixels
#ifndef SSD1306_WIDTH
#define SSD1306_WIDTH           128
#endif

// some LEDs don't display anything in first two columns
// #define SSD1306_WIDTH           130

// Enumeration for screen colors
typedef enum {
    Black = 0x00, // Black color, no pixel
    White = 0x01  // Pixel is set. Color depends on OLED
} SSD1306_COLOR;

// Struct to store transformations
typedef struct {
    uint16_t CurrentX;
    uint16_t CurrentY;
    uint8_t Inverted;
    uint8_t Initialized;
} SSD1306_t;

// Procedure definitions
void ssd1306_Init(void);
void ssd1306_Fill(SSD1306_COLOR color);


// Low-level procedures
void ssd1306_Reset(void);
void ssd1306_WriteCommand(uint8_t byte);
void ssd1306_WriteData(uint8_t* buffer, size_t buff_size);
void ssd1306_UpdateScreen(void);

#endif // __SSD1306_H__
