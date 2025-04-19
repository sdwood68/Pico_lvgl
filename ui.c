/**
 * @file ui.c
 *
 */


/*********************
 *      INCLUDES
 *********************/
#include "ui.h"
#include "Pico_lvgl.h"
#include "lv_port_indev.h"


/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
*  STATIC VARIABLES
**********************/
static lv_style_t style_1;
static lv_style_t style_2;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
static void back_event_handler(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target_obj(e);
    lv_obj_t * menu = (lv_obj_t *)lv_event_get_user_data(e);

    if(lv_menu_back_button_is_root(menu, obj)) {
        lv_obj_t * mbox1 = lv_msgbox_create(NULL);
        lv_msgbox_add_title(mbox1, "Hello");
        lv_msgbox_add_text(mbox1, "Root back btn click.");
        lv_msgbox_add_close_button(mbox1);
    }
}

void my_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
	printf("Code: ");
	switch (code) {
	case LV_EVENT_PRESSED:
		printf("LV_EVENT_PRESSED\n");
		break;
	case LV_EVENT_PRESSING:
		printf("LV_EVENT_PRESSING\n");
		break;
	case LV_EVENT_PRESS_LOST:
		printf("LV_EVENT_PRESS_LOST\n");
		break;
	case LV_EVENT_SHORT_CLICKED:
		printf("LV_EVENT_SHORT_CLICKED\n");
		break;
	case LV_EVENT_LONG_PRESSED:
		printf("LV_EVENT_LONG_PRESSED\n");
		break;
	case LV_EVENT_LONG_PRESSED_REPEAT:
		printf("LV_EVENT_LONG_PRESSED_REPEAT\n");
		break;
	case LV_EVENT_CLICKED:
		printf("LV_EVENT_CLICKED\n");
		break;
	case LV_EVENT_RELEASED:
		printf("LV_EVENT_RELEASED\n");
		break;
	case LV_EVENT_KEY:
		uint32_t key = lv_indev_get_key(keypad);
		printf("LV_EVENT_KEY %d\n", key);
		break;
	case LV_EVENT_STYLE_CHANGED:
		printf("LV_EVENT_STYLE_CHANGED\n");
		break;	default:
		printf("%d\n", code);
		break;
	}
}

void ui_init(void) {
	/* 
	 *LVGL STYLES
	 */
	lv_style_set_border_width(&style_1, 0);

	lv_style_set_border_width(&style_2, 2);

    /*
	 * LVGL HELLO WORLD
	 */
	// lv_obj_set_style_bg_color(lv_screen_active(), lv_color_white(), LV_PART_MAIN);
	// lv_obj_t *hello_world_scr = lv_label_create(NULL);
	// lv_label_set_text(hello_world_scr, "Hello World This is Long");
	// lv_obj_set_size(hello_world_scr, lv_display_get_horizontal_resolution(NULL), lv_display_get_vertical_resolution(NULL));
	// lv_obj_set_style_text_color(lv_screen_active(), lv_color_black(), LV_PART_MAIN);
	// lv_obj_align(hello_world_scr, LV_ALIGN_CENTER, 0, 0);

	/*
	 * Menu example
	 */
	#if 1
	/*Create a menu object*/
	lv_obj_t * menu_scr = lv_menu_create(NULL);
	lv_obj_set_size(menu_scr, lv_display_get_horizontal_resolution(NULL), lv_display_get_vertical_resolution(NULL));
	lv_obj_center(menu_scr);
	lv_obj_t * cont;
	lv_obj_t * label;

	/*Create a sub page*/
	lv_obj_t * sub_page = lv_menu_page_create(menu_scr, NULL);

	cont = lv_menu_cont_create(sub_page);
	label = lv_label_create(cont);
	lv_label_set_text(label, "Hello, I am hiding here");

	/*Create a main page*/
	lv_obj_t * param_menu = lv_menu_page_create(menu_scr, NULL);

	lv_obj_t * param_1 = lv_menu_cont_create(param_menu);
    lv_obj_t * param_l_label = lv_label_create(param_1);
	lv_obj_set_size(param_1, lv_display_get_horizontal_resolution(NULL), lv_display_get_vertical_resolution(NULL)/4);
	lv_label_set_text(param_l_label, "Param 1:");
    lv_obj_add_style(param_l_label, &style_1, LV_STATE_DEFAULT);  // Apply always
    lv_obj_add_style(param_l_label, &style_2, LV_STATE_FOCUSED);  

	lv_obj_t * param_2 = lv_menu_cont_create(param_menu);
	lv_obj_t * param_2_label = lv_label_create(param_2);
	lv_obj_set_size(param_2, lv_display_get_horizontal_resolution(NULL), lv_display_get_vertical_resolution(NULL)/4);
	lv_label_set_text(param_2_label, "Param 2:");
    lv_obj_add_style(param_2_label, &style_1, LV_STATE_DEFAULT);  // Apply always
    lv_obj_add_style(param_2_label, &style_2, LV_STATE_FOCUSED);  

	lv_obj_t * param_3 = lv_menu_cont_create(param_menu);
	lv_obj_t * param_3_label = lv_label_create(param_3);
	lv_obj_set_size(param_3, lv_display_get_horizontal_resolution(NULL), lv_display_get_vertical_resolution(NULL)/4);
	lv_label_set_text(param_3_label, "Param 3:");
    lv_obj_add_style(param_3_label, &style_1, LV_STATE_DEFAULT);  // Apply always
    lv_obj_add_style(param_3_label, &style_2, LV_STATE_FOCUSED);  
	lv_menu_set_load_page_event(menu_scr, cont, sub_page);

	lv_obj_t * param_4 = lv_menu_cont_create(param_menu);
	lv_obj_t * param_4_label = lv_label_create(param_4);
	lv_obj_set_size(param_4, lv_display_get_horizontal_resolution(NULL), lv_display_get_vertical_resolution(NULL)/4);
	lv_label_set_text(param_4_label, "Param 4:");
    lv_obj_add_style(param_4_label, &style_1, LV_STATE_DEFAULT);  // Apply always
    lv_obj_add_style(param_4_label, &style_2, LV_STATE_FOCUSED);  

	lv_menu_set_page(menu_scr, param_menu);

	/* 
	 * Create an input group for the keypad 
	 */
	lv_group_t * kb_group = lv_group_create();
	lv_group_add_obj(kb_group, menu_scr);
	lv_indev_set_group(keypad, kb_group);
	lv_obj_add_event_cb(menu_scr, my_event_cb, LV_EVENT_KEY, NULL);

	// lv_screen_load(hello_world_scr);
	lv_screen_load(menu_scr);

	#endif
}