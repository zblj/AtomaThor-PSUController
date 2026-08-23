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
#include "stm32g0xx_hal.h"
#include "stm32g0xx_ll_crc.h"
#include "stm32g0xx_ll_rcc.h"
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_system.h"
#include "stm32g0xx_ll_exti.h"
#include "stm32g0xx_ll_cortex.h"
#include "stm32g0xx_ll_utils.h"
#include "stm32g0xx_ll_pwr.h"
#include "stm32g0xx_ll_dma.h"
#include "stm32g0xx_ll_gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

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

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define VrefEN_Pin LL_GPIO_PIN_2
#define VrefEN_GPIO_Port GPIOA
#define DACclk_Pin LL_GPIO_PIN_5
#define DACclk_GPIO_Port GPIOA
#define DACsync_Pin LL_GPIO_PIN_6
#define DACsync_GPIO_Port GPIOA
#define DACmosi_Pin LL_GPIO_PIN_7
#define DACmosi_GPIO_Port GPIOA
#define V12sns_Pin LL_GPIO_PIN_2
#define V12sns_GPIO_Port GPIOB
#define ADCcs_Pin LL_GPIO_PIN_12
#define ADCcs_GPIO_Port GPIOB
#define ADCsck_Pin LL_GPIO_PIN_13
#define ADCsck_GPIO_Port GPIOB
#define ADCmiso_Pin LL_GPIO_PIN_14
#define ADCmiso_GPIO_Port GPIOB
#define ADCmosi_Pin LL_GPIO_PIN_15
#define ADCmosi_GPIO_Port GPIOB
#define ADCirq_Pin LL_GPIO_PIN_8
#define ADCirq_GPIO_Port GPIOA
#define USART1_DE_Pin LL_GPIO_PIN_5
#define USART1_DE_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
extern ADC_HandleTypeDef hadc1;
extern I2C_HandleTypeDef hi2c1;
extern LPTIM_HandleTypeDef hlptim1;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern UART_HandleTypeDef huart1;

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
