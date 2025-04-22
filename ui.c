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
lv_group_t * kb_group;
lv_style_t style_1;
lv_style_t style_2;
	
const char *event_name[] = {
	"LV_EVENT_ALL"
	"LV_EVENT_PRESSED",             /**< The object has been pressed*/
    "LV_EVENT_PRESSING",            /**< The object is being pressed (called continuously while pressing)*/
    "LV_EVENT_PRESS_LOST",          /**< The object is still being pressed but slid cursor/finger off of the object */
    "LV_EVENT_SHORT_CLICKED",       /**< The object was pressed for a short period of time, then released it. Not called if scrolled.*/
    "LV_EVENT_LONG_PRESSED",        /**< Object has been pressed for at least `long_press_time`.  Not called if scrolled.*/
    "LV_EVENT_LONG_PRESSED_REPEAT", /**< Called after `long_press_time` in every `long_press_repeat_time` ms.  Not called if scrolled.*/
    "LV_EVENT_CLICKED",             /**< Called on release if not scrolled (regardless to long press)*/
    "LV_EVENT_RELEASED",            /**< Called in every cases when the object has been released*/
    "LV_EVENT_SCROLL_BEGIN",        /**< Scrolling begins. The event parameter is a pointer to the animation of the scroll. Can be modified*/
    "LV_EVENT_SCROLL_THROW_BEGIN",
    "LV_EVENT_SCROLL_END",          /**< Scrolling ends*/
    "LV_EVENT_SCROLL",              /**< Scrolling*/
    "LV_EVENT_GESTURE",             /**< A gesture is detected. Get the gesture with `lv_indev_get_gesture_dir(lv_indev_active());` */
    "LV_EVENT_KEY",                 /**< A key is sent to the object. Get the key with `lv_indev_get_key(lv_indev_active());`*/
    "LV_EVENT_ROTARY",              /**< An encoder or wheel was rotated. Get the rotation count with `lv_event_get_rotary_diff(e);`*/
    "LV_EVENT_FOCUSED",             /**< The object is focused*/
    "LV_EVENT_DEFOCUSED",           /**< The object is defocused*/
    "LV_EVENT_LEAVE",               /**< The object is defocused but still selected*/
    "LV_EVENT_HIT_TEST",            /**< Perform advanced hit-testing*/
    "LV_EVENT_INDEV_RESET",         /**< Indev has been reset*/
    "LV_EVENT_HOVER_OVER",          /**< Indev hover over object*/
    "LV_EVENT_HOVER_LEAVE",         /**< Indev hover leave object*/
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
// static void back_event_handler(lv_event_t * e)
// {
//     lv_obj_t * obj = lv_event_get_target_obj(e);
//     lv_obj_t * menu = (lv_obj_t *)lv_event_get_user_data(e);

//     if(lv_menu_back_button_is_root(menu, obj)) {
//         lv_obj_t * mbox1 = lv_msgbox_create(NULL);
//         lv_msgbox_add_title(mbox1, "Hello");
//         lv_msgbox_add_text(mbox1, "Root back btn click.");
//         lv_msgbox_add_close_button(mbox1);
//     }
// }

void my_event_cb(lv_event_t * e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code < 23) {
		printf("Code: %s\n", event_name[code]);
	} else {
		printf("Code: %d\n", code);
	}

	switch (code) {
	case LV_EVENT_PRESSED:
		// LV_LOG_INFO("%d: LV_EVENT_PRESSED\n", code);
		break;
	case LV_EVENT_PRESSING:
		// LV_LOG_INFO("%d: LV_EVENT_PRESSING\n", code);
		break;
	case LV_EVENT_PRESS_LOST:
		// LV_LOG_INFO("%d: LV_EVENT_PRESS_LOST\n", code);
		break;
	case LV_EVENT_SHORT_CLICKED:
		// LV_LOG_INFO("%d: LV_EVENT_SHORT_CLICKED\n", code);
		break;
	case LV_EVENT_LONG_PRESSED:
		// LV_LOG_INFO("%d: LV_EVENT_LONG_PRESSED\n", code);
		break;
	case LV_EVENT_LONG_PRESSED_REPEAT:
		// LV_LOG_INFO("%d: LV_EVENT_LONG_PRESSED_REPEAT\n", code);
		break;
	case LV_EVENT_CLICKED:
		// LV_LOG_INFO("%d: LV_EVENT_CLICKED\n", code);
		lv_obj_set_state(NULL, LV_STATE_FOCUSED, true);
		break;
	case LV_EVENT_RELEASED:
		// LV_LOG_INFO("%d: LV_EVENT_RELEASED\n", code);
		break;
	case LV_EVENT_KEY:
		uint32_t key = lv_indev_get_key(keypad);
		LV_LOG_INFO("%d: LV_EVENT_KEY %d\n", code, key);
		break;
	case LV_EVENT_STYLE_CHANGED:
		// printf("%d: LV_EVENT_STYLE_CHANGED\n", code);
		break;
	case LV_EVENT_FOCUSED:
		// printf("%d: LV_EVENT_FOCUSED\n", code);
		break;
	case LV_EVENT_DEFOCUSED:
		// printf("%d: LV_EVENT_DEFOCUSED\n", code);
		break;
	case LV_EVENT_COVER_CHECK:
		// printf("%d: LV_EVENT_COVER_CHECK\n", code);
		break;
	case LV_EVENT_DRAW_MAIN_BEGIN:
		// printf("%d: LV_EVENT_DRAW_MAIN_BEGIN\n", code);
		break;
	case LV_EVENT_DRAW_MAIN:
		// printf("%d: LV_EVENT_DRAW_MAIN\n", code);
		break;
	case LV_EVENT_DRAW_MAIN_END:
		// printf("%d: LV_EVENT_DRAW_MAIN_END\n", code);
		break;
	}
}

void ui_init(void) {
	/* ****************************************** 
	 * LVGL STYLES
	 * *****************************************/
	lv_style_init(&style_1);
	lv_style_set_border_width(&style_1, 0);
	// lv_style_set_border_color(&style_1, lv_color_black());
	// lv_style_set_bg_color(&style_1, lv_color_black());
	// lv_style_set_text_color(&style_1, lv_color_white());
	
	lv_style_init(&style_2);
	lv_style_set_border_width(&style_2, 2);
	// lv_style_set_border_color(&style_2, lv_color_white());
	// lv_style_set_bg_color(&style_2, lv_color_black());
	// lv_style_set_text_color(&style_2, lv_color_white());
		
    /*
	 * LVGL HELLO WORLD
	 */
	// lv_obj_set_style_bg_color(lv_screen_active(), lv_color_white(), LV_PART_MAIN);
	// lv_obj_t *hello_world_scr = lv_label_create(NULL);
	// lv_label_set_text(hello_world_scr, "Hello World This is Long");
	// lv_obj_set_size(hello_world_scr, lv_display_get_horizontal_resolution(NULL), lv_display_get_vertical_resolution(NULL));
	// lv_obj_set_style_text_color(lv_screen_active(), lv_color_black(), LV_PART_MAIN);
	// lv_obj_align(hello_world_scr, LV_ALIGN_CENTER, 0, 0);

	/* ******************************************
	 * Create an input group for the keypad 
	 * *****************************************/

	kb_group = lv_group_create();
	lv_indev_set_group(keypad, kb_group);


	/* ******************************************
	 * Create a parameter menu object
	 * - Each object must be added to the input
	 *   group except maybe labels. 
	 * - The top level menu oject must have an
	 *   event callback assigned.
	 * *****************************************/
	lv_obj_t * cont;
	lv_obj_t * label;

	/*Create the menu object*/
	lv_obj_t * param_menu = lv_menu_create(NULL);
	lv_obj_set_size(param_menu, 128, 64);
	// lv_menu_set_mode_root_back_button(param_menu, LV_MENU_ROOT_BACK_BUTTON_ENABLED);
	lv_group_add_obj(kb_group, param_menu);	// Must be Added to the kb_group
	lv_obj_add_event_cb(param_menu, my_event_cb, LV_EVENT_ALL, NULL); 	// Add a event call back to main_menu_page

	lv_obj_center(param_menu);

	/* Create a main menu page */
	lv_obj_t * main_page = lv_menu_page_create(param_menu, "Parameters:");
	lv_group_add_obj(kb_group, main_page);	// Must be added to the kb_group
	
	/* Add content to main_menu_page */
	
	// 1st menu item
	cont = lv_menu_cont_create(main_page);	
	lv_obj_set_size(cont, 128, 16);
	lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_add_flag(cont, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
	lv_group_add_obj(kb_group, cont);		// Must be added to the kb_group
	lv_obj_add_style(cont, &style_1, LV_STATE_DEFAULT);  // Apply always
    lv_obj_add_style(cont, &style_2, LV_STATE_FOCUS_KEY | LV_STATE_FOCUSED);  
	label = lv_label_create(cont);			// Add a Label 
	lv_label_set_text(label, "Param 1:");	// Set the label text

	// 2nd menu item
	cont = lv_menu_cont_create(main_page);	
	lv_obj_set_size(cont, 128, 16);
	lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_add_flag(cont, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
	lv_group_add_obj(kb_group, cont);		// Must be added to the kb_group
	lv_obj_add_style(cont, &style_1, LV_STATE_DEFAULT);  // Apply always
    lv_obj_add_style(cont, &style_2, LV_STATE_FOCUS_KEY | LV_STATE_FOCUSED);  
	label = lv_label_create(cont);			// Add a Label 
	lv_label_set_text(label, "Param 2:");	// Set the label text

	// 3rd menu item
	cont = lv_menu_cont_create(main_page);	
	lv_obj_set_size(cont, 128, 16);
	lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_add_flag(cont, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
	lv_group_add_obj(kb_group, cont);		// Must be added to the kb_group
	lv_obj_add_style(cont, &style_1, LV_STATE_DEFAULT);  // Apply always
    lv_obj_add_style(cont, &style_2, LV_STATE_FOCUS_KEY | LV_STATE_FOCUSED);  
	label = lv_label_create(cont);			// Add a Label 
	lv_label_set_text(label, "Param 3:");	// Set the label text
	
	// 4th menu item
	cont = lv_menu_cont_create(main_page);	
	lv_obj_set_size(cont, 128, 16);
	lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_add_flag(cont, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
	lv_group_add_obj(kb_group, cont);		// Must be added to the kb_group
	lv_obj_add_style(cont, &style_1, LV_STATE_DEFAULT);  // Apply always
    lv_obj_add_style(cont, &style_2, LV_STATE_FOCUS_KEY | LV_STATE_FOCUSED);  
	label = lv_label_create(cont);			// Add a Label 
	lv_label_set_text(label, "Param 4:");	// Set the label text

	
	// 5th menu item
	cont = lv_menu_cont_create(main_page);	
	lv_obj_set_size(cont, 128, 16);
	lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_add_flag(cont, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
	lv_group_add_obj(kb_group, cont);		// Must be added to the kb_group
	lv_obj_add_style(cont, &style_1, LV_STATE_DEFAULT);  // Apply always
    lv_obj_add_style(cont, &style_2, LV_STATE_FOCUS_KEY | LV_STATE_FOCUSED); 
	label = lv_label_create(cont);			// Add a Label 
	lv_label_set_text(label, "Param 5:");	// Set the label text

	
	lv_menu_set_page(param_menu, main_page);

	// lv_screen_load(hello_world_scr);
	lv_screen_load(param_menu);

}