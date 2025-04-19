/**
  *****************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  *****************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  *****************************************************************************
  */

/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef __PICO_LVGL_sh1106_H
#define __PICO_LVGL_sh1106_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>
#include "pico/stdlib.h"

/*********************
 *      DEFINES
 *********************/
#define LED_PIN 		25
#define LED0 		  	18
#define LED1 		  	19
#define LED2 			  20
#define LED3 			  21
#define LED_MASK		(1 << LED_PIN | 1 << LED0 | 1 << LED1 | 1 << LED2 | 1 << LED3)
#define UP_PIN			1
#define DOWN_PIN		0
#define LEFT_PIN		2
#define RIGHT_PIN		3
#define ENT_PIN			4
#define KEY_MASK    (1 << UP_PIN | 1 << DOWN_PIN | 1 << LEFT_PIN | 1 << RIGHT_PIN | 1 << ENT_PIN)

// I2C defines
#define I2C_PORT 		i2c0
#define I2C_SDA 		16
#define I2C_SCL 		17

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

uint32_t get_millis_cb(void);

#ifdef __cplusplus
}
#endif

#endif /* __PICO_LVGL_sh1106_H */
