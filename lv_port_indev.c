/**
 * @file lv_port_indev.c
 *
 */
/*Copy this file as "lv_port_indev.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_indev.h"
#include "pico_lvgl.h"
#include "hardware/gpio.h"

/*********************
 *      DEFINES
 *********************/
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0') 

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void keypad_init(void);
static void keypad_read(lv_indev_t *indev, lv_indev_data_t *data);
static uint32_t keypad_get_key(void);

/**********************
 *  STATIC VARIABLES
 **********************/
lv_indev_t *keypad;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_indev_init(void)
{
    /**
     * Here you will find example implementation of input devices supported by LittelvGL:
     *  - Keypad (supports GUI usage only with key)
     *
     *  The `..._read()` function are only examples.
     *  You should shape them according to your hardware
     */

    /*Register a keypad input device*/
    keypad = lv_indev_create();
    lv_indev_set_type(keypad, LV_INDEV_TYPE_KEYPAD);
    lv_indev_set_read_cb(keypad, keypad_read);
    lv_indev_set_long_press_time(keypad, 500);

    /*Later you should create group(s) with `lv_group_t * group = lv_group_create()`,
     *add objects to the group with `lv_group_add_obj(group, obj)`
     *and assign this input device to group to navigate in it:
     *`lv_indev_set_group(keypad, group);`*/
    // Create a group and add the label to it
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your keypad*/
static void keypad_init(void)
{
    /*Your code comes here*/
}

/*Will be called by the library to read the mouse*/
static void keypad_read(lv_indev_t *indev_drv, lv_indev_data_t *data)
{
    static uint32_t last_key = 0;
    gpio_put(LED0, 1);

    /*Get whether the a key is pressed and save the pressed key*/
    uint32_t act_key = keypad_get_key();

    if (act_key != 0) {
        data->state = LV_INDEV_STATE_PRESSED;

        /*Translate the keys to LVGL control characters according to your key definitions*/
        switch (act_key) {
        case 1:
            act_key = LV_KEY_ENTER;
            // LV_LOG_USER("ENTER KEY");
            break;
        case 2:
            act_key = LV_KEY_UP;
            // LV_LOG_USER("UP KEY");
            break;
        case 3:
            act_key = LV_KEY_DOWN;
            // LV_LOG_USER("DOWN KEY");
            break;
        case 4:
            act_key = LV_KEY_RIGHT;
            // LV_LOG_USER("RIGHT KEY");
            break;
        case 5:
            act_key = LV_KEY_LEFT;
            // LV_LOG_USER("LEFT KEY");
            break;
        }
        last_key = act_key;
    }
    else {
        data->state = LV_INDEV_STATE_RELEASED;
    }

    data->key = last_key;
    gpio_put(LED0, 0);
}

/*Get the currently being pressed key.  0 if no key is pressed*/
static uint32_t keypad_get_key(void)
{
    static uint32_t last_keys = 0;
    uint32_t keys = ~gpio_get_all() & KEY_MASK;
    uint32_t key = 0;
    // printf("key: "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(keys));
    if (keys & (1 << ENT_PIN)) return 1;
    else if (keys & (1 << UP_PIN)) return 2;
    else if (keys & (1 << DOWN_PIN)) return 3;
    else if (keys & (1 << LEFT_PIN)) return 4;
    else if (keys & (1 << RIGHT_PIN)) return 5;
    else return 0;
}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif