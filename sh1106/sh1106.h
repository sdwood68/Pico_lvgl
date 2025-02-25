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

// sh1106 OLED height in pixels
#ifndef sh1106_HEIGHT
#define sh1106_HEIGHT          64
#endif

// sh1106 width in pixels
#ifndef sh1106_WIDTH
#define sh1106_WIDTH           132
#endif

#define PAGE_SIZE               sh1106_WIDTH

#ifndef BUF_SIZE
#define BUF_SIZE ((sh1106_HEIGHT * sh1106_WIDTH / 8) + 8) // Pad the buffer size by 8 bytes for cammands to be added or LVGLs pix_map
#endif

enum uint8_t {
    // Set the display start column lower and upper nibble where A[7:0] is the column (0-131)
    LOWCOLUMN = 0x00,   // 0x00 ored with the lower nibble of the column, binary: 0 0 0 0 A3 A2 A1 A0
    HIGHCOLUMN = 0x10,  // 0x10 ored with the higher nibble of the column, binary: 0 0 0 1 A7 A6 A5 A4

    // Set the charge pump voltage Address 0x30 - 0x33
    // 0 = 6.4V
    // 1 = 7.4V
    // 2 = 8.0V
    // 3 = 9.0V
    CHARGE_PUMP = 0x30, 

    // Set the display start line, Address 0x40 - 0x7F
    // Binary: 0 1 A5 A4 A3 A2 A1 A0
    // Where A[5:0] is the start line 
    STARTLINE = 0x40,

    // Set the Contrast Control register
    // First part is just the Contrast Register (0x81) second byte is the level (0-255)
    CONTRAST = 0x81,    // Two Byte Command

    // Set Segment Re-Map (0xA0 - 0xA1)
    // Change the relationship between RAM column address and segment driver
    COLUMN_REMAP_OFF = 0xA0,    // normal direction
    COLUMN_REMAP_ON = 0xA1,     // Reverse direction

    // Set Entire Display OFF / ON (0xA4-0xA5)
    DISPLAY_NORM = 0xA4,     // The normal display status is provided (POR)
    DISPLAY_ALL_ON = 0xA5,   // The entire display ON status is provided

    // Set Normal/Reverse Display (0xA6-0xA7)
    INVERTED_OFF = 0xA6,    // 1 in RAM, pixel is on (POR)
    INVERTED_ON = 0xA7,     // 0 in Ram, pixel is on

    // Set Multiplex Ratio (0xA8)
    // (Double Byte Command)
    MULTIPLEX = 0xA8,

    // Set teh DC-DC Converter OFF/ON (0xAD)
    // (Double byte Command)
    DC_DC_CTRL = 0xAD,  // REGISTER ENABLE COMMAND
    DC_DC_ON = 0x8A,    // 2nd Byte, Disable Converter
    DC_DC_OFF = 0x8B,   // 2nd Byte, Enable Conveter (POR)

    // Display Off / On (0xAE-0xAF)
    DISPLAY_ON = 0xAE,   // (POR)
    DISPLAY_OFF = 0xAF,

    // Set the Page Address (0xB0-0xB7)
    PAGE_START = 0xB0,  // Page Mode page 0 is POR

    // Set Output Scan Direction (0xC0-0xC8)
    COM_INC = 0xC0, // Scan from Column 0 to 131 (POR)
    COM_DEC = 0xC8, // Scan from Column 131 to 0

    // Set Display Offset (0xD3)
    // (double byte command)
    // second byte is the Line to map to the top of the display
    VERT_OFFSET = 0xD3, // Enable Offset, data = 00 (POR)
    
    // Display Clock Divider Ratio/Oscillator Frequency
    // (double byte command)
    // second byte lower nible (A3 A2 A1 A0) is the divider ratio 1-16, 1 on POR
    // second byte higher nibble (A7 A6 A5 A4) is the % deviation of the frequency
    // 0x0 = -25%
    // 0x5 = 0% (POR)
    // 0xF = +50%
    DISPLAYCLOCKDIV = 0xD5,

    // Set Discharge / Pre-charge Period (0xD9)
    // (double byte command)
    // Precharge Period Adjust A[3:0] in clock periods
    //      0 is invalid, 2 is POR, max is 15 clock periods
    // Dis-charge Period Adjust, A[7:4]
    //      0 is invalid, 2 is PORm max is 15 clock periods
    PRECHARGE = 0xD9,

    // Set Common pads hardware configuration (0xDA)
    // (double byte command)
    // 2nd Byte is either Sequential (0x02) or Alternate (POR 0x12)
    COMMON_PADS = 0xDA,

    // Set VCOM Deselect level (0xDB)
    // (double byte command)
    // 2nd Byte (0x00-0x3F)
    // Vcom = (0.430 + value x 6.415m) * Vref
    // POR is 35 -> 0.77xVref
    VCOMDETECT = 0xDB,

    // Read Modify Write Register (0xE0)
    // pair of Read-Modify-Write and End commands must always be used. Once read-modify-write is issued, column address is 
    // not incremental by read display data command but incremental by write display data command only. It continues until End 
    // command is issued. When the End is issued, column address returns to the address when read-modify-write is issued. This 
    // can reduce the microprocessor load when data of a specific display area is repeatedly changed during cursor blinking or 
    // others. 
    RD_MOD_WR = 0xE0,
    RMW_END = 0xEE,

    // NOP Command
    NOP = 0xE3,
};

typedef enum {
    HORZ = 0x00,
    VERT = 0x01,
    PAGE = 0x02,  
} MEM_MODE_t;

// Enumeration for screen colors
typedef enum {
    Black = 0x00, // Black color, no pixel
    White = 0x01  // Pixel is set. Color depends on OLED
} COLOR_t;

// Struct to store transformations
typedef struct {
    i2c_inst_t *port;
    uint8_t addr; 
    uint16_t CurrentX;
    uint16_t CurrentY;
    bool inverted;
    bool flipped; 
    bool mirrored; 
    uint8_t buffer[BUF_SIZE];
} sh1106_t;

// Procedure definitions
sh1106_t sh1106_init(i2c_inst_t *port, uint8_t addr, bool inverted, bool flipped, bool mirrored);
void sh1106_fill_buffer(sh1106_t disp, COLOR_t color);

// Low-level procedures
void sh1106_reset(sh1106_t disp);
void sh1106_UpdateScreen(sh1106_t disp);
void sh1106_inverted(sh1106_t disp, bool invert);
void sh1106_mirrored(sh1106_t disp, bool mirrored);
void sh1106_flipped(sh1106_t disp, bool flipped);
void sh1106_contrast(sh1106_t disp, uint8_t value);
void sh1106_display(sh1106_t disp, bool on);
void sh1106_vert_offset(sh1106_t disp, uint8_t offset);
void sh1106_col_start(sh1106_t disp, uint8_t column);
void sh1106_row_start(sh1106_t disp, uint8_t row);
void sh1106_start_page(sh1106_t disp, uint8_t page);
void sh1106_dis_pre_charge(sh1106_t disp, uint8_t discharge, uint8_t precharge);
void sh1106_common_pads(sh1106_t disp, bool sequential);
void sh1106_vcomp(sh1106_t disp, uint8_t level);

#endif // __sh1106_H__
