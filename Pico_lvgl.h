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

/* Includes -----------------------------------------------------------------*/
#include <stdio.h>
#include "pico/stdlib.h"

#ifdef __cplusplus
}
#endif

/* Prototypes ---------------------------------------------------------------*/
uint32_t get_millis_cb(void);

#endif /* __PICO_LVGL_sh1106_H */
