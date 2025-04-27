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
lv_obj_t * param_menu_init(void);
void my_event_cb(lv_event_t * e);

/**********************
*  STATIC VARIABLES
**********************/
lv_obj_t * home_screen;
lv_obj_t * param_menu;

param_t param1 = {"Param 1:", 16, 16, 8};
param_t param2 = {"Param 2:", 64, 128, 0};
param_t param3 = {"Param 3:", 124, 128, 0};
param_t param4 = {"Param 4:", 16, 32, 0};
param_t param5 = {"Param 5:", 32, 32, 16};

LV_FONT_DECLARE(arial_b_14);

lv_group_t * kb_group;
lv_style_t style_1;
lv_style_t style_2;
	
const char *event_name[] = {
	"ALL"
	"PRESSED",             /**< The object has been pressed*/
    "PRESSING",            /**< The object is being pressed (called continuously while pressing)*/
    "PRESS_LOST",          /**< The object is still being pressed but slid cursor/finger off of the object */
    "SHORT_CLICKED",       /**< The object was pressed for a short period of time, then released it. Not called if scrolled.*/
    "LONG_PRESSED",        /**< Object has been pressed for at least `long_press_time`.  Not called if scrolled.*/
    "LONG_PRESSED_REPEAT", /**< Called after `long_press_time` in every `long_press_repeat_time` ms.  Not called if scrolled.*/
    "CLICKED",             /**< Called on release if not scrolled (regardless to long press)*/
    "RELEASED",            /**< Called in every cases when the object has been released*/
    "SCROLL_BEGIN",        /**< Scrolling begins. The event parameter is a pointer to the animation of the scroll. Can be modified*/
    "SCROLL_THROW_BEGIN",
    "SCROLL_END",          /**< Scrolling ends*/
    "SCROLL",              /**< Scrolling*/
    "GESTURE",             /**< A gesture is detected. Get the gesture with `lv_indev_get_gesture_dir(lv_indev_active());` */
    "KEY",                 /**< A key is sent to the object. Get the key with `lv_indev_get_key(lv_indev_active());`*/
    "ROTARY",              /**< An encoder or wheel was rotated. Get the rotation count with `lv_event_get_rotary_diff(e);`*/
    "FOCUSED",             /**< The object is focused*/
    "DEFOCUSED",           /**< The object is defocused*/
    "LEAVE",               /**< The object is defocused but still selected*/
    "HIT_TEST",            /**< Perform advanced hit-testing*/
    "INDEV_RESET",         /**< Indev has been reset*/
    "HOVER_OVER",          /**< Indev hover over object*/
    "HOVER_LEAVE",         /**< Indev hover leave object*/
	
	/** Drawing events*/
	"COVER_CHECK",        /**< Check if the object fully covers an area. The event parameter is `lv_cover_check_info_t *`.*/
	"REFR_EXT_DRAW_SIZE", /**< Get the required extra draw area around the object (e.g. for shadow). The event parameter is `int32_t *` to store the size.*/
	"DRAW_MAIN_BEGIN",    /**< Starting the main drawing phase*/
	"DRAW_MAIN",          /**< Perform the main drawing*/
	"DRAW_MAIN_END",      /**< Finishing the main drawing phase*/
	"DRAW_POST_BEGIN",    /**< Starting the post draw phase (when all children are drawn)*/
	"DRAW_POST",          /**< Perform the post draw phase (when all children are drawn)*/
	"DRAW_POST_END",      /**< Finishing the post draw phase (when all children are drawn)*/
	"DRAW_TASK_ADDED",      /**< Adding a draw task */

	/** Special events*/
	"VALUE_CHANGED",       /**< The object's value has changed (i.e. slider moved)*/
	"INSERT",              /**< A text is inserted to the object. The event data is `char *` being inserted.*/
	"REFRESH",             /**< Notify the object to refresh something on it (for the user)*/
	"READY",               /**< A process has finished*/
	"CANCEL",              /**< A process has been cancelled */
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
	/* Get the LVGL menu item associated with the event. */
	lv_obj_t * cont = lv_event_get_target(e);
	/* Get the Label associated with the menu item. */
	lv_obj_t * label = lv_obj_get_child(cont, 0);
	/* Get the user data assocoated with the menu item. */
	param_t *param = lv_event_get_user_data(e);

	/* Debug Code to print out events details and user data */
	lv_event_code_t code = lv_event_get_code(e);
	if (code < 36) {
		printf("LV_EVENT_%s", event_name[code]);
	} else {
		printf(", Code: %d\n", code);
	}
	printf(", Param. Name: ");
	(param != NULL) ? printf("%s", param->name) : printf("NULL");
	printf(", Value: ");
	(param != NULL) ? printf("%d", param->value) : printf("NULL");

	/* Keypad Processing for changing parameter values */
	if (code == LV_EVENT_KEY && param != NULL) {
		uint32_t key = lv_indev_get_key(keypad);
		printf(", KEY: %d", key);
		if (param != NULL) {
			switch (key) {
				case LV_KEY_RIGHT:
					printf(", Right");
					param->value -= param->min;
					param->value += 1;
					param->value %= param->span;
					param->value += param->min;
					break;
				case LV_KEY_LEFT:
					printf(", Left");
					param->value -= param->min;
					param->value -= 1;
					param->value %= param->span;
					param->value += param->min;
			}
			printf(", New Value: %d", param->value);
		}
		lv_label_set_text_fmt(label, "%s %d", param->name, param->value);
	}
	printf("\n");

}

void ui_init(void) {
	/* ****************************************** 
	 * LVGL STYLES
	 * *****************************************/
	lv_style_init(&style_1);
	lv_style_set_text_font(&style_1, &arial_b_14);
	
	lv_style_init(&style_2);
	lv_style_set_text_font(&style_2, &arial_b_14);

		
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

	/* Initialize the parameter menue */
	param_menu = param_menu_init();

	// lv_screen_load(hello_world_scr);
	lv_screen_load(param_menu);
}

lv_obj_t * param_menu_init(void) {
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
	lv_obj_t * menu = lv_menu_create(NULL);
	lv_obj_set_size(menu, 128, 64);
	// lv_menu_set_mode_root_back_button(menu, LV_MENU_ROOT_BACK_BUTTON_ENABLED);
	lv_group_add_obj(kb_group, menu);	// Must be Added to the kb_group
	lv_obj_add_event_cb(menu, my_event_cb, LV_EVENT_KEY, NULL); 	// Add a event call back to main_menu_page
	lv_obj_add_style(menu, &style_1, LV_STATE_DEFAULT);
	lv_obj_center(menu);

	/* Create the main menu page */
	lv_obj_t * menu_page = lv_menu_page_create(menu, "Parameters:");
	lv_menu_set_page(menu, menu_page);
	lv_group_add_obj(kb_group, menu_page);	// Must be added to the kb_group

	/* Add content to main_menu_page */
	
	// 1st menu item
	cont = lv_menu_cont_create(menu_page);	
	lv_obj_set_size(cont, 128, 16);
	lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_add_flag(cont, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
	lv_group_add_obj(kb_group, cont);		// Must be added to the kb_group
	lv_obj_add_event_cb(cont, my_event_cb, LV_EVENT_KEY, &param1); 	// Add a event call back to main_menu_page
	lv_obj_set_user_data(cont, &param1);
	label = lv_label_create(cont);			// Add a Label 
	lv_label_set_text_fmt(label, "%s %3d", param1.name, param1.value);	// Set the label text

	// 2nd menu item
	cont = lv_menu_cont_create(menu_page);	
	lv_obj_set_size(cont, 128, 16);
	lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_add_flag(cont, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
	lv_group_add_obj(kb_group, cont);		// Must be added to the kb_group
	lv_obj_add_event_cb(cont, my_event_cb, LV_EVENT_KEY, &param2); 	// Add a event call back to main_menu_page
	lv_obj_set_user_data(cont, &param2);
	label = lv_label_create(cont);			// Add a Label 
	lv_label_set_text_fmt(label, "%s %3d", param2.name, param2.value);	// Set the label text

	// 3rd menu item
	cont = lv_menu_cont_create(menu_page);	
	lv_obj_set_size(cont, 128, 16);
	lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_add_flag(cont, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
	lv_group_add_obj(kb_group, cont);		// Must be added to the kb_group
	lv_obj_add_event_cb(cont, my_event_cb, LV_EVENT_KEY, NULL); 	// Add a event call back to main_menu_page
	// lv_obj_set_user_data(cont, &param3);
	label = lv_label_create(cont);			// Add a Label 
	lv_label_set_text(label, "Param 3:");
	// lv_label_set_text_fmt(label, "%s %3d", param3.name, param3.value);	// Set the label text
	
	// 4th menu item
	cont = lv_menu_cont_create(menu_page);	
	lv_obj_set_size(cont, 128, 16);
	lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_add_flag(cont, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
	lv_group_add_obj(kb_group, cont);		// Must be added to the kb_group
	lv_obj_add_event_cb(cont, my_event_cb, LV_EVENT_KEY, &param4); 	// Add a event call back to main_menu_page
	lv_obj_set_user_data(cont, &param4);
 
	label = lv_label_create(cont);			// Add a Label 
	lv_label_set_text_fmt(label, "%s %3d", param4.name, param4.value);	// Set the label text
	
	// 5th menu item
	cont = lv_menu_cont_create(menu_page);	
	lv_obj_set_size(cont, 128, 16);
	lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_add_flag(cont, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
	lv_group_add_obj(kb_group, cont);		// Must be added to the kb_group
	lv_obj_add_event_cb(cont, my_event_cb, LV_EVENT_KEY, &param5); 	// Add a event call back to main_menu_page
	lv_obj_set_user_data(cont, &param5);
	label = lv_label_create(cont);			// Add a Label 
	lv_label_set_text_fmt(label, "%s %3d", param5.name, param5.value);	// Set the label text

	return menu;
}