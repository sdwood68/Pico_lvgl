#include "sh1106.h"
#include <string.h>
#define sh1106_INVERSE_COLOR

void sh1106_Reset(sh1106_t disp) {
	/* for I2C - do nothing */
}

// Send a byte to the command register
void write_1byte_cmd(sh1106_t disp, uint8_t reg) {
    printf("cmd(%02X", reg);   // Continuation bit not set, Data bit not set
    disp.buffer[0] = 0x00;  
    disp.buffer[1] = reg;
	i2c_write_blocking(disp.port, disp.addr, disp.buffer, 2, false);
    printf(")\n");
}

void write_2byte_cmd(sh1106_t display, uint8_t reg, uint8_t data) {
    printf("cmd_data(%02X, %02X", reg, data);
    // 0x00 is a byte indicating to sh1106 that a command is being sent
    display.buffer[0] = 0x80;  // Continuation bit set, Data bit not set
    display.buffer[1] = reg;
    display.buffer[2] = 0x00;  // Continuation bit not set, Data bit not set
    display.buffer[3] = data;
	i2c_write_blocking(display.port, display.addr, display.buffer, 4, false);
    printf(")\n");
}

// Send data
void write_data(sh1106_t disp, uint8_t* buffer, size_t size) {
    printf("data bytes %d", size);
    disp.buffer[0] = 0x40;  // Continuation bit not set, Data bit set
    memcpy(&disp.buffer[1], buffer, size);
    i2c_write_blocking(disp.port, disp.addr, disp.buffer, size+1, false);
}

/******************************************************************************
 * Register Commands
 *****************************************************************************/

// Column Address & Mirrored
void sh1106_column_reverse(sh1106_t disp, bool mirrored) {
    write_1byte_cmd(disp, mirrored ? COLUMN_REMAP_OFF : COLUMN_REMAP_ON); // Mirror horizontally
    disp.mirrored = mirrored;
}

void sh1106_col_start(sh1106_t disp, uint8_t column) {
    if (column > 127) column = 127;
    write_1byte_cmd(disp, LOWCOLUMN | (column & 0x0F));
    write_1byte_cmd(disp, HIGHCOLUMN | (column & 0xF0) >> 4);
}

// Line Address & Flipped
void sh1106_flipped(sh1106_t disp, bool flipped) {
    write_1byte_cmd(disp, flipped ? COM_INC : COM_DEC); // Mirror vertically
    disp.flipped = flipped;
}

// Fill the whole screen with the given color
void sh1106_fill_buffer(sh1106_t disp, COLOR_t color) {
    for(uint32_t i = 0; i < BUF_SIZE; i++) {
        disp.buffer[i] = (color == Black) ? 0x00 : 0xFF;
    }
}

void sh1106_row_start(sh1106_t disp, uint8_t row) {
    if (row > 63) row = 63;
    write_1byte_cmd(disp, STARTLINE | (row & 0x3F));
}

// Oscillator Commands

// Write the nbuffer to the display RAM
void sh1106_UpdateScreen(sh1106_t disp) {
    for(uint8_t page = 0; page < 8; page++) {
        write_1byte_cmd(disp, PAGE_START | page);
        write_1byte_cmd(disp, LOWCOLUMN);
        write_1byte_cmd(disp, HIGHCOLUMN);
        write_data(disp, &disp.buffer[sh1106_WIDTH * page], PAGE_SIZE);
    }
}

void sh1106_contrast(sh1106_t disp, uint8_t value) {
    write_2byte_cmd(disp, CONTRAST, value); //--set contrast control register - CHECK
}

void sh1106_display(sh1106_t disp, bool on) {
    write_1byte_cmd(disp, on ? DISPLAY_ON : DISPLAY_OFF);
}

void sh1106_inverted(sh1106_t disp, bool inverted) {
    write_1byte_cmd(disp, inverted ? INVERTED_ON : INVERTED_OFF); //--set inverse color
    disp.inverted = inverted;
}

void sh1106_vert_offset(sh1106_t disp, uint8_t offset) {
    write_2byte_cmd(disp, VERT_OFFSET, offset); //-set display offset - CHECK
}

void sh1106_start_page(sh1106_t disp, uint8_t page) {
    if (page > 7) page = 7;
    write_1byte_cmd(disp, PAGE_START | page); //Set Page Start Address for Page Addressing Mode,0-7
}

void sh1106_multiplex(sh1106_t disp, uint8_t ratio) {
    write_2byte_cmd(disp, MULTIPLEX, ratio); //--set multiplex ratio(1 to 64) - CHECK
}

void sh1106_norm_display(sh1106_t disp, bool norm) {
    write_1byte_cmd(disp, norm ? DISPLAY_ALL_ON : DISPLAY_NORM); // Display: Normal (POR, 0xA4), All On (0xA5)
}

void sh1106_clk_div(sh1106_t disp, uint8_t divider, uint8_t freq) {
    divider =- 1;
    if (divider > 15) divider = 0;
    if (freq > 15) freq = 5;
    freq = freq << 4;
    write_2byte_cmd(disp, DISPLAYCLOCKDIV, freq | divider); //--set display clock divide ratio/oscillator frequency
}

void sh1106_dis_pre_charge(sh1106_t disp, uint8_t discharge, uint8_t precharge) {
    if (discharge = 0) discharge = 2;
    if (discharge > 15) discharge = 2;
    if (precharge = 0) precharge = 2;
    if (precharge > 15) precharge = 2;
    write_2byte_cmd(disp, PRECHARGE, discharge << 4 | precharge); //--set pre-charge period
}

void sh1106_common_pads(sh1106_t disp, bool sequential) {
    write_2byte_cmd(disp, COMMON_PADS, sequential ? 0x02 : 0x12); //--set com pins hardware configuration - CHECK
}

void sh1106_vcomp(sh1106_t disp, uint8_t level) {
    write_2byte_cmd(disp, VCOMDETECT, level);
}

// Initialize the oled screen
sh1106_t sh1106_init(i2c_inst_t *port, uint8_t addr, bool inverted, bool flipped, bool mirrored) {

    static sh1106_t disp;

    disp.port = port; 
    disp.addr = addr; 
    disp.inverted = inverted;
    disp.mirrored = mirrored;  

	sh1106_Reset(disp);  	// Reset OLED
    sleep_ms(100);      // Wait for the screen to boot
        
    // Init OLED
    sh1106_display(disp, false); //display off
    sh1106_start_page(disp, 0); //Set Page Start Address for Page Addressing Mode,0-7
    sh1106_col_start(disp, 0); //---set low column address
    sh1106_row_start(disp, 0);
    sh1106_contrast(disp, 0x7F);
    sh1106_flipped(disp, flipped);
    sh1106_inverted(disp, inverted);
    sh1106_multiplex(disp, 0x3F);
    sh1106_norm_display(disp, true);
    sh1106_vert_offset(disp, 0x00);
    sh1106_clk_div(disp, 16, 0);
    sh1106_dis_pre_charge(disp, 2, 2);
    sh1106_common_pads(disp, false);
    sh1106_vcomp(disp, 0x20);  // por = 0x35
    sh1106_display(disp, true); //--turn on sh1106 panel

    sh1106_fill_buffer(disp, Black);        // Clear screen
    sh1106_UpdateScreen(disp);       // Flush buffer to screen
    
    // Set default values for screen object
    disp.CurrentX = 0;
    disp.CurrentY = 0;
    
    return disp;
}