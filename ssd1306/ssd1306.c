#include "ssd1306.h"

// void SSD1306::cmd(unsigned char command) {
//     // 0x00 is a byte indicating to ssd1306 that a command is being sent
//     uint8_t buff[2] = {0x00, command};
//     i2c_write_blocking(this->i2CInst, this->address, buff, sizeof(buff), false);
// }

void ssd1306_Reset(void) {
	/* for I2C - do nothing */
}

// Send a byte to the command register
void ssd1306_WriteCommand(uint8_t byte) {
    uint8_t cmd = 0x00;
	i2c_write_blocking(SSD1306_I2C_PORT, SSD1306_I2C_ADDR, &cmd, 1, true);
    i2c_write_blocking(SSD1306_I2C_PORT, SSD1306_I2C_ADDR, &byte, 1, false);
}

// Send data
void ssd1306_WriteData(uint8_t* buffer, size_t buff_size) {
    uint8_t cmd = 0x40;
    i2c_write_blocking(SSD1306_I2C_PORT, SSD1306_I2C_ADDR, &cmd, 1, true);
    i2c_write_blocking(SSD1306_I2C_PORT, SSD1306_I2C_ADDR, buffer, buff_size, false);
    // HAL_I2C_Mem_Write(SSD1306_I2C_PORT, SSD1306_I2C_ADDR, 0x40, 1, buffer, buff_size, false);
}

// Screenbuffer
static uint8_t SSD1306_Buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];

// Screen object
static SSD1306_t SSD1306;

// Initialize the oled screen
void ssd1306_Init(void) {
	// Reset OLED
	ssd1306_Reset();

    // Wait for the screen to boot
    sleep_ms(100);
    
    // Init OLED
    ssd1306_WriteCommand(0xAE); //display off

    ssd1306_WriteCommand(0x20); //Set Memory Addressing Mode   
    ssd1306_WriteCommand(0x10); // 00,Horizontal Addressing Mode; 01,Vertical Addressing Mode;
                                // 10,Page Addressing Mode (RESET); 11,Invalid

    ssd1306_WriteCommand(0xB0); //Set Page Start Address for Page Addressing Mode,0-7

#ifdef SSD1306_MIRROR_VERT
    ssd1306_WriteCommand(0xC0); // Mirror vertically
#else
    ssd1306_WriteCommand(0xC8); //Set COM Output Scan Direction
#endif

    ssd1306_WriteCommand(0x00); //---set low column address
    ssd1306_WriteCommand(0x10); //---set high column address

    ssd1306_WriteCommand(0x40); //--set start line address - CHECK

    ssd1306_WriteCommand(0x81); //--set contrast control register - CHECK
    ssd1306_WriteCommand(0xFF);

#ifdef SSD1306_MIRROR_HORIZ
    ssd1306_WriteCommand(0xA0); // Mirror horizontally
#else
    ssd1306_WriteCommand(0xA1); //--set segment re-map 0 to 127 - CHECK
#endif

#ifdef SSD1306_INVERSE_COLOR
    ssd1306_WriteCommand(0xA7); //--set inverse color
#else
    ssd1306_WriteCommand(0xA6); //--set normal color
#endif

    ssd1306_WriteCommand(0xA8); //--set multiplex ratio(1 to 64) - CHECK
    ssd1306_WriteCommand(0x3F); //

    ssd1306_WriteCommand(0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content

    ssd1306_WriteCommand(0xD3); //-set display offset - CHECK
    ssd1306_WriteCommand(0x00); //-not offset

    ssd1306_WriteCommand(0xD5); //--set display clock divide ratio/oscillator frequency
    ssd1306_WriteCommand(0xF0); //--set divide ratio

    ssd1306_WriteCommand(0xD9); //--set pre-charge period
    ssd1306_WriteCommand(0x22); //

    ssd1306_WriteCommand(0xDA); //--set com pins hardware configuration - CHECK
    ssd1306_WriteCommand(0x12);

    ssd1306_WriteCommand(0xDB); //--set vcomh
    ssd1306_WriteCommand(0x20); //0x20,0.77xVcc

    ssd1306_WriteCommand(0x8D); //--set DC-DC enable
    ssd1306_WriteCommand(0x14); //
    ssd1306_WriteCommand(0xAF); //--turn on SSD1306 panel

    // Clear screen
    ssd1306_Fill(Black);
    
    // Flush buffer to screen
    ssd1306_UpdateScreen();
    
    // Set default values for screen object
    SSD1306.CurrentX = 0;
    SSD1306.CurrentY = 0;
    
    SSD1306.Initialized = 1;
}

// Fill the whole screen with the given color
void ssd1306_Fill(SSD1306_COLOR color) {
    /* Set memory */
    uint32_t i;

    for(i = 0; i < sizeof(SSD1306_Buffer); i++) {
        SSD1306_Buffer[i] = (color == Black) ? 0x00 : 0xFF;
    }
}

// Write the screenbuffer with changed to the screen
void ssd1306_UpdateScreen(void) {
    uint8_t i;
    for(i = 0; i < 8; i++) {
        ssd1306_WriteCommand(0xB0 + i);
        ssd1306_WriteCommand(0x00);
        ssd1306_WriteCommand(0x10);
        ssd1306_WriteData(&SSD1306_Buffer[SSD1306_WIDTH*i],SSD1306_WIDTH);
    }
}
