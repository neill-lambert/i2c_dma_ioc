/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
  typedef enum
  {
    I2C_OK = 0,
    I2C_ERR_NACK,
    I2C_ERR_TIMEOUT,
    I2C_ERR_BUS_HUNG,
    I2C_ERR_DMA_TIMEOUT,
  } i2c_status_t;

  static const char* i2c_status_str(i2c_status_t s)
  {
    switch (s)
    {
      case I2C_OK:              return "I2C_OK";
      case I2C_ERR_NACK:        return "I2C_ERR_NACK";
      case I2C_ERR_TIMEOUT:     return "I2C_ERR_TIMEOUT";
      case I2C_ERR_BUS_HUNG:    return "I2C_ERR_BUS_HUNG";
      case I2C_ERR_DMA_TIMEOUT: return "I2C_ERR_DMA_TIMEOUT";
      default:                  return "I2C_ERR_UNKNOWN";
    }
  }
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
i2c_status_t STMPE811_Write_Reg_Safe(uint8_t reg, uint8_t value);
uint8_t STMPE811_Read_Reg_Simple(uint8_t reg);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
