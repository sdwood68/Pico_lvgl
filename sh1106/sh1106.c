#include "sh1106.h"
#include <stdlib.h>
#include <string.h>

void sh1106_Reset(sh1106_t *disp) {
	/* for I2C - do nothing */
}

// Send a byte to the command register
void write_1byte_cmd(sh1106_t *disp, uint8_t reg) {
    // printf("cmd(%02X", reg);   // Continuation bit not set, Data bit not set
    disp->cmd_buffer[0] = 0x00;  
    disp->cmd_buffer[1] = reg;
	i2c_write_blocking(disp->port, disp->addr, disp->cmd_buffer, 2, false);
    // printf(")\n");
}

void write_2byte_cmd(sh1106_t *disp, uint8_t reg, uint8_t data) {
    // printf("cmd_data(%02X, %02X", reg, data);
    // 0x00 is a byte indicating to sh1106 that a command is being sent
    disp->cmd_buffer[0] = 0x80;  // Continuation bit set, Data bit not set
    disp->cmd_buffer[1] = reg;
    disp->cmd_buffer[2] = 0x00;  // Continuation bit not set, Data bit not set
    disp->cmd_buffer[3] = data;
	i2c_write_blocking(disp->port, disp->addr, disp->cmd_buffer, 4, false);
    // printf(")\n");
}

// Send data
int write_data(sh1106_t *disp, uint8_t *buffer, size_t size) {
    // printf("Addr: %x, size: %x\n", buffer, size);
    if (size > sizeof(disp->page_buffer)) return -1;
    disp->page_buffer[0] = 0x40;  // Continuation bit not set, Data bit set
    memcpy(&disp->page_buffer[1], buffer, size);
    i2c_write_blocking(disp->port, disp->addr, disp->page_buffer, size+1, false);
}

/******************************************************************************
 * Register Commands
 *****************************************************************************/

/**
 * \brief   1/2. Set Column Address (0x00-0x0F, 0x10-0x1F):
 *          Specifies column address of display RAM. Divide the column address
 *          into 4 higher bits and 4 lower bits. Set each of them into 
 *          successions. When the microprocessor repeats to access to the 
 *          display RAM, the column address counter is incremented during each
 *          access until address 131 is accessed. The page address is not 
 *          changed during this time. 
 * \param   sh1106_t disp, Display Structure returned by sh1106_init()
 * \param   uint_8t column, starting the column to map to the fisrt  
 */
void sh1106_col_start(sh1106_t *disp, uint8_t column) {
    column += disp->col_offset;
    column %= SSH1106_MAX_WIDTH;
    // if (column > 127) column = 127;
    write_1byte_cmd(disp, 0x00 | (column & 0x0F));
    write_1byte_cmd(disp, 0x10 | (column & 0xF0) >> 4);
}

/**
 * \brief   3. Set Charge Pump voltage value: (30H~33H)
 *          Specifies output voltage (VPP) of the internal charger pump:
* \param    sh1106_t disp, Display Structure returned by sh1106_init()
* \param    uint8_t value, 0 = 6.4V, 1 = 7.4V, 2 = 8.0V (POR), 3 = 9.0V  
*/
void ssh1106_chargepump(sh1106_t *disp, uint8_t value) {
    value = value & 0x03; // Mask the value to the lower two bits
    write_1byte_cmd(disp, 0x30 | value);
}

/**
 * \brief   4. Set Display Start Line: (40H - 7FH)
 *          Specifies line address (refer to Figure. 8) to determine the
 *          initial display line or COM0. The RAM display data becomes the top
 *          line of OLED screen. It is followed by the higher number of lines
 *          in ascending order, corresponding to the duty cycle. When this 
 *          command changes the line address, the smooth scrolling or page 
 *          change takes place.
 * \param   sh1106_t disp, Display Structure returned by sh1106_init()
 * \param   uint8_t row
 */
void sh1106_start_line(sh1106_t *disp, uint8_t line) {
    if (line > 63) line = 63;
    write_1byte_cmd(disp, 0x40 | line);
}

/**
 * \brief   5. Set Contrast Control Register: (Double Bytes Command) 
 *          This command is to set contrast setting of the display. The chip 
 *          has 256 contrast steps from 00 to FF. The segment output current
 *          increases as the contrast step value increases. Segment output 
 *          current setting: ISEG = α/256 X IREF X scale factor Where: α is 
 *          contrast step; IREF is reference current equals 18.75µA; Scale 
 *          factor = 16. 
 * \param   sh1106_t disp, Display Structure returned by sh1106_init()
 * \param   uint8_t value (0-255), POR = 128
 */
void sh1106_contrast(sh1106_t *disp, uint8_t value) {
    write_2byte_cmd(disp, 0x81, value);
}

/**
 * \brief   6. Set Segment Re-map: (A0H - A1H) 
 *          Change the relationship between RAM column address and segment 
 *          driver. The order of segment driver output pads can be reversed by
 *          software. This allows flexible IC layout during OLED module 
 *          assembly. For details, refer to the column address section of 
 *          Figure. 8. When display data is written or read, the column address
 *          is incremented by 1 as shown in Figure. 1. 
 *          When ADC = “L”, the right rotates (normal direction). (POR) 
 *          When ADC = “H”, the left rotates (reverse direction). 
 * \param   sh1106_t disp, Display Structure returned by sh1106_init()
 * \param   bool mirrored, POR = false
 */
void sh1106_reverse_cols(sh1106_t *disp, bool mirrored) {
    write_1byte_cmd(disp, mirrored ? 0xA1 : 0xA0);
    disp->mirrored = mirrored;
}

/**
 * \brief   7. Set Entire Display OFF/ON: (A4H - A5H) 
 *          Forcibly turns the entire display on regardless of the contents of
 *          the display data RAM. At this time, the contents of the display 
 *          data RAM are held. This command has priority over the 
 *          normal/reverse display command. 
 *          normal = true, Display reflects the contents of RAM.
 *          normal = flase, All display segments are ON.
 * \param   sh1106_t disp, Display Structure returned by sh1106_init()
 * \param   bool normal, POR = true 
 */
void sh1106_norm_display(sh1106_t *disp, bool normal) {
    write_1byte_cmd(disp, normal ? 0xA4 : 0xA5);
}

/**
 * \brief   8. Set Normal/Reverse Display: (A6H -A7H)
 *          Reverses the display ON/OFF status without rewriting the contents
 *          of the display data RAM.
 *          When D = “L”, the RAM data is high, being OLED ON potential 
 *          (normal display). (POR)
 *          When D = “H”, the RAM data is low, being OLED ON potential (reverse display) 
 * \param   sh1106_t disp, Display Structure returned by sh1106_init()
 * \param   bool inverted, POR = false
 */
void sh1106_inverted(sh1106_t *disp, bool inverted) {
    write_1byte_cmd(disp, inverted ? 0xA6 : 0xA7); //--set inverse color
    disp->inverted = inverted;
}

/**
 * \brief   9. Set Multiplex Ration: (Double Bytes Command)
 *          This command switches default 64 multiplex modes to any multiplex 
 *          ratio from 1 to 64. The output pads COM0-COM63 will be switched to
 *          corresponding common signal. 
 * \param   sh1106_t disp, Display Structure returned by sh1106_init()
 * \param   uint8_t ratio (1-64), POR = 64
 */
void sh1106_multiplex(sh1106_t *disp, uint8_t ratio) {
    ratio =- 1;
    if (ratio > 63) ratio = 63;
    write_2byte_cmd(disp, 0xA8, ratio); 
}

/**
 * \brief   10. Set DC-DC OFF/ON: (Double Bytes Command)
 *          This command is to control the DC-DC voltage converter. The 
 *          converter will be turned on by issuing this command then display ON
 *          command. The panel display must be off while issuing this command.
 *          When D = “L”, DC-DC is disable. 
 *          When D = “H”, DC-DC will be turned on when display on. (POR) 
 * \param   sh1106_t disp, Display Structure returned by sh1106_init()
 * \param   bool On, POR = true
 */
void sh1106_power_on(sh1106_t *disp, bool on) {
    write_2byte_cmd(disp, 0xAD, on ? 0x8B : 0x8A); 
}

/**
 * \brief   11. Display OFF/ON: (AEH - AFH)
 *          Alternatively turns the display on and off. 
 *          When D = “L”, Display OFF OLED. (POR)
 *          When D = “H”, Display ON OLED. 
 *          When the display OFF command is executed, power saver mode will be
 *          entered. 
 *          Sleep mode: 
 *              This mode stops every operation of the OLED display system, and
 *              can reduce current consumption nearly to a static current value
 *              if no access is made from the microprocessor. The internal 
 *              status in the sleep mode is as follows:
 *              1) Stops the oscillator circuit and DC-DC circuit. 
 *              2) Stops the OLED drive and outputs Hz as the segment/common 
 *                 driver output.
 *              3) Holds the display data and operation mode provided before 
 *                 the start of the sleep mode. 
 *              4) The MPU can access to the built-in display RAM.
 * \param   sh1106_t disp, Display Structure returned by sh1106_init()
 * \param   bool On, POR = true
 */
void sh1106_display_on(sh1106_t *disp, bool on) {
    write_1byte_cmd(disp, on ? 0xAF : 0xAE);
}

/**
 * \brief   12. Set Page Address: (B0H - B7H) 
 *          Specifies page address to load display RAM data to page address 
 *          register. Any RAM data bit can be accessed when its page address
 *          and column address are specified. The display remains unchanged 
 *          even when the page address is changed.
 * \param   sh1106_t disp, Display Structure returned by sh1106_init()
 * \param   uint8_t page (0-7)
 */
void sh1106_page(sh1106_t *disp, uint8_t page) {
    if (page > 7) page = 7;
    write_1byte_cmd(disp, 0xB0 | page);
}

/**
 * \brief   13. Set Common Output Scan Direction: (C0H - C8H)
 *          This command sets the scan direction of the common output allowing
 *          layout flexibility in OLED module design. In addition, the display
 *          will have immediate effect once this command is issued. That is, 
 *          if this command is sent during normal display, the graphic display
 *          will be vertically flipped. 
 *          When D = “L”, Scan from COM0 to COM [N -1]. (POR) 
 *          When D = “H”, Scan from COM [N -1] to COM0.  
 * \param   sh1106_t disp, Display Structure returned by sh1106_init()
 * \param   bool flipped, POR = false
 */
void sh1106_flipped(sh1106_t *disp, bool flipped) {
    write_1byte_cmd(disp, flipped ? 0xC8 : 0xC0);
    disp->flipped = flipped;
}

/**
 * \brief   14. Set Display Offset: (0xD3, Double Bytes Command)
 *          This is a double byte command. The next command specifies the 
 *          mapping of display start line to one of COM0-63 (it is assumed that
 *          COM0 is the display start line, that equals to 0). For example, to 
 *          move the COM16 towards the COM0 direction for 16 lines, the 6-bit 
 *          data in the second byte should be given by 010000. To move in the 
 *          opposite direction by 16 lines, the 6-bit data should be given by 
 *          (64-16), so the second byte should be 100000. 
 * \param   sh1106_t disp, Display Structure returned by sh1106_init()
 * \param   uint8_t offset (0-63), POR = 0
 */
 void sh1106_vert_offset(sh1106_t *disp, uint8_t offset) {
    if (offset > 63) offset = 63;
    write_2byte_cmd(disp, 0xD3, offset);
}

/**
 * \brief   15. Set Display Clock Divide Ratio/Oscillator Frequency: (0xD5, 
 *              Double Bytes Command)
 *          This command is used to set the frequency of the internal display
 *          clocks (DCLKs). It is defined as the divide ratio (Value from 1 to
 *          16) used to divide the oscillator frequency. POR is 1. Frame 
 *          frequency is determined by divide ratio, number of display clocks 
 *          per row, MUX ratio and oscillator frequency. 
 * \param   sh1106_t disp, Display Structure returned by sh1106_init()
 * \param   uint8_t divider (1-16), POR = 1
 * \param   uint8_t freq (0-15), 0 = -25%, 5 = 0% (POR), 15 = +50%
 */
void sh1106_clk_div(sh1106_t *disp, uint8_t divider, uint8_t freq) {
    divider =- 1;
    if (divider > 15) divider = 0;
    if (freq > 15) freq = 5;
    freq = freq << 4;
    write_2byte_cmd(disp, 0xD5, freq | divider);
}

/**
 * \brief   16. Set Dis-charge/Pre-charge Period: (0xD9, Double Bytes Command)
 *          This command is used to set the duration of the pre-charge period.
 *          The interval is counted in number of DCLK. POR is 2 DCLKs. 
 * \param   sh1106_t disp, Display Structure returned by sh1106_init()
 * \param   uint8_t discharge (1-16), POR = 2
 * \param   uint8_t precharge (1-15), POR = 2
 */
void sh1106_dis_pre_charge(sh1106_t *disp, uint8_t discharge, uint8_t precharge) {
    if (discharge = 0) discharge = 2;
    if (discharge > 15) discharge = 2;
    if (precharge = 0) precharge = 2;
    if (precharge > 15) precharge = 2;
    write_2byte_cmd(disp, 0xD9, discharge << 4 | precharge);
}

/**
 * \brief   17. Set Common pads hardware configuration: (0xDA, Double Bytes
 *              Command)
 *          This command is to set the common signals pad configuration 
 *          (sequential or alternative) to match the OLED panel hardware 
 *          layout. 
 * \param   sh1106_t disp, Display Structure returned by sh1106_init()
 * \param   bool sequential, POR = false
 */
void sh1106_com_pads(sh1106_t *disp, bool sequential) {
    write_2byte_cmd(disp, 0xDA, sequential ? 0x02 : 0x12);
}

/**
 * \brief   18. Set VCOM Deselect Level: (0xDB, Double Bytes Command)
 *          This command is to set the common pad output voltage level at 
 *          deselect stage. 
 *          VCOM = β X VREF = (0.430 + A[7:0] X 0.006415) X VREF 
 *          0x00 = 0.430
 *          0x35 = 0.770 (POR)
 *          0x3F = 0.834
 *          0x40-0xFF = 1
 * \param   sh1106_t disp, Display Structure returned by sh1106_init()
 * \param   uint8_t value, POR = 0x35
 */
void sh1106_vcomp(sh1106_t *disp, uint8_t level) {
    write_2byte_cmd(disp, 0xDB, level);
}

/**
 * \brief   19. Read-Modify-Write: (E0H)
 *          A pair of Read-Modify-Write and End commands must always be used.
 *          Once read-modify-write is issued, column address is not incremental
 *          by read display data command but incremental by write display data
 *          command only. It continues until End command is issued. When the 
 *          End is issued, column address returns to the address when 
 *          read-modify-write is issued. This can reduce the microprocessor 
 *          load when data of a specific display area is repeatedly changed 
 *          during cursor blinking or others.
 */

 /**
  * \brief  20. End: (EEH)
  *         Cancels Read-Modify-Write mode and returns column address to the
  *         original address (when Read-Modify-Write is issued.) 
  */

/**
 * \brief   21. NOP: (E3H)
 *          Non-Operation Command. 
 */

/**
 * \brief   Write the buffer to the display RAM
 * \param   sh1106_t disp, Display Structure returned by sh1106_init()
 * \param   uint8_t *buffer, pointer to the buffer, buffer must have a size
 *                           equal to (132 * 8) bytes.
 */
int sh1106_write_screen(sh1106_t *disp, uint8_t *buffer) {
    for(uint8_t page = 0; page < 8; page++) {
        sh1106_page(disp, page);
        sh1106_col_start(disp, disp->col_offset);
        write_data(disp, buffer + SH1106_WIDTH * page, SH1106_WIDTH);
    }
    return 0;
}

int sh1106_write_area(sh1106_t *disp, uint8_t x1, uint8_t x2, uint8_t y1, uint8_t y2, uint8_t *buffer) {
    // printf("Flush Area, %d, %d, %d, %d\n", x, y, x_size, y_size);
	uint8_t x_1 = x1 & ~(0x07);             // round down by 8 
    uint8_t x_2 = x2 | (0x07);              // round up by 8
    uint8_t width = x_2 - x_1 + 1;
    uint8_t y_1 = y1 & ~(0x7);              // round down by 8 
    uint8_t y_2 = y2 | (0x07);              // round up by 8
    uint8_t start_page = y_1 >> 3;
    uint8_t end_page = y_2 >> 3;
    printf("X range = %d - %d\n", x_1, x_2);
    printf("width = %d\n", width);
    printf("start page = %d\n", start_page);
    printf("end page = %d\n", end_page);
	for (uint8_t page = start_page; page <= end_page; page++) {
		sh1106_page(disp, page);          		// Set the page start address
		sh1106_col_start(disp, x_1 + disp->col_offset);     // Set the lower start column address
		write_data(disp, buffer + x_1, width);
		buffer += SH1106_WIDTH;
    }
}

// Fill the whole screen with the given color
void sh1106_clear_display(sh1106_t *disp) {
    memset(&disp->page_buffer, 0, sizeof(disp->page_buffer));
    disp->page_buffer[0] = 0x40;  // Continuation bit not set, Data bit set
    for(uint8_t page = 0; page < 8; page++) {
        sh1106_page(disp, page);
        sh1106_col_start(disp, 0);
        i2c_write_blocking(disp->port, disp->addr, disp->page_buffer, sizeof(disp->page_buffer), false);  
    }
}

// Initialize the oled screen
bool sh1106_init(sh1106_t *disp, i2c_inst_t *port, uint8_t addr, uint8_t col_offset) {

    disp->port = port; 
    disp->addr = addr;
    disp->col_offset = col_offset;
    disp->inverted = false;
    disp->mirrored = false;
    disp->flipped = false;

    sh1106_Reset(disp);  	            // Reset OLED
    sleep_ms(10);                       // Wait for the screen to boot
        
    // Init OLED
    sh1106_display_on(disp, false);     //display off
    ssh1106_chargepump(disp, 2);        // POR = 2, 8.0V
    sh1106_power_on(disp, true);        // Turn on the DC-DC converter
    sh1106_com_pads(disp, false);       // POR = false
    sh1106_multiplex(disp, 64);         // POR = 64
    sh1106_norm_display(disp, true);    // POR = true
    sh1106_clk_div(disp, 1, 5);         // POR = 1, 5
    sh1106_dis_pre_charge(disp, 2, 2);  // POR is 2, 2
    sh1106_vcomp(disp, 0x20);           // POR = 0x35
    sh1106_page(disp, 0);               // POR = 0, page 0
    sh1106_col_start(disp, 0);          // POR = 0, column 0
    sh1106_start_line(disp, 0);         // POR = 0, RAM Display line 0
    sh1106_vert_offset(disp, 0x00);     // POR = 0, offset lines = 0
    sh1106_contrast(disp, 0x80);        // POR = 128, Half Brightness
    sh1106_flipped(disp, disp->flipped);
    sh1106_reverse_cols(disp, disp->mirrored);
    sh1106_inverted(disp, disp->inverted);
    sh1106_clear_display(disp);
    sh1106_display_on(disp, true);      // turn on display

    return true;
}