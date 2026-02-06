/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32f429xx.h"
#include "stm32f4xx_hal_dma.h"
#include <math.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c3;
DMA_HandleTypeDef hdma_i2c3_rx;
DMA_HandleTypeDef hdma_i2c3_tx;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C3_Init(void);
/* USER CODE BEGIN PFP */
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c);
void I2CTransfer ( void ); //unfinished xfer
static void my_I2C3_Init(void);
static void my_DMA_Init(void);
static void my_GPIO_Init(void);
void I2C_Read_1Byte (uint8_t uc_Dev_Address, uint8_t uc_Reg_Address, uint8_t *uc_pReadBuffer);
void I2C_Write_Via_DMA(uint8_t uc_Dev_Address, uint8_t uc_Reg_Address, uint8_t *uc_pWriteBuffer, uint8_t size);
void I2C_Read_Via_DMA (uint8_t uc_Dev_Address, uint8_t uc_Reg_Address, uint8_t *uc_pReadBuffer, uint8_t size);
static void DMA_Receive(uint8_t* pBuffer, uint8_t sizeTransfer);
static void DMA_Transmit(const uint8_t * pBuffer, uint8_t size);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  my_GPIO_Init();
  //my_DMA_Init();
  my_I2C3_Init();
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */

  /* USER CODE BEGIN 2 */
  uint8_t value[2];

  I2C_Read_1Byte(0x82, 0x01, &value[0]);
  HAL_Delay(2);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 64;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV4;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C3_Init(void)
{

  /* USER CODE BEGIN I2C3_Init 0 */

  /* USER CODE END I2C3_Init 0 */

  /* USER CODE BEGIN I2C3_Init 1 */

  /* USER CODE END I2C3_Init 1 */
  hi2c3.Instance = I2C3;
  hi2c3.Init.ClockSpeed = 100000;
  hi2c3.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c3.Init.OwnAddress1 = 0;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c3, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c3, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C3_Init 2 */

  /* USER CODE END I2C3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);
  /* DMA1_Stream4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/**
  * @brief HAL_I2C_MasterTxCpltCallback
  * @param I2C_HandleTypeDef
  * @retval None
  */
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c) {
    if(hi2c->Instance == I2C3) {
        // Handle successful reception
    	asm("nop");
    }
}


/************************/
/**
  * @brief my_GPIO_Init, reg init
  * @param None
  * @retval None
  */
static void my_GPIO_Init(void)
{
	//enable clock access to GPIOC and A
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIOAEN;
	// 1. Enable i2c3 Clock
	RCC->APB1ENR |= RCC_APB1ENR_I2C3EN;

//	// 2. Force I2C Reset
//	I2C3->CR1 |= I2C_CR1_SWRST;
//	for(int i = 0; i < 1000; i++); // Small delay
//	I2C3->CR1 &= ~I2C_CR1_SWRST;
//	for(int i = 0; i < 1000; i++); // Small delay

	//reset i2c peripheral
	//RCC->APB1RSTR |= RCC_APB1RSTR_I2C3RST;
	//RCC->APB1RSTR &= ~RCC_APB1RSTR_I2C3RST;


	//GPIOA8 TO AF
	GPIOA->MODER |= GPIO_MODER_MODER8_1;
	//AF INDEX FOR A8 TO I2C3SCL
	// AFRH AF4, 0b100, 0x4.
	GPIOA->AFR[1] |= GPIO_AFRH_AFRH0_2;
	//ospeed8 0x3, V HIGH
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR8;
	//ot8 0x1, O DRAIN
	GPIOA->OTYPER |= GPIO_OTYPER_OT8;
	//PIN A8 PULL UP? 01 = PULL UP
	//GPIOA->PUPDR |= GPIO_PUPDR_PUPDR8_0;
	//GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR8_1;
	//GPIOC9 TO AF
	GPIOC->MODER |= GPIO_MODER_MODER9_1;
	//AF INDEX FOR C9 TO I2C3SDA
	// AFRH AF4, 0b100, 0x4.
	GPIOC->AFR[1] |= GPIO_AFRH_AFRH1_2;
	//ospeed9 0x3, V HIGH
	GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR9;
	//otype9 0x1, O DRAIN
	GPIOC->OTYPER |= GPIO_OTYPER_OT9;
	//PIN C9 PULL UP?
	//PIN A8 PULL UP? 01 = PULL UP
	//GPIOC->PUPDR |= GPIO_PUPDR_PUPDR9_0;
	//GPIOC->PUPDR &= ~GPIO_PUPDR_PUPDR9_1;
}


/************************/
/**
  * @brief my_DMA_Init, reg init
  * @param None
  * @retval None
  */
static void my_DMA_Init(void)
{
	//enable clock access dma1
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

	//clear all interrupts to ensure no misfiring when enabling
	DMA1->LIFCR |= DMA_LIFCR_CTCIF2;

	DMA1->LIFCR |= DMA_LIFCR_CHTIF2;

	DMA1->LIFCR |= DMA_LIFCR_CTEIF2;

	DMA1->LIFCR |= DMA_LIFCR_CDMEIF2;

	DMA1->LIFCR |= DMA_LIFCR_CFEIF2;

	DMA1_Stream2->CR &= ~(DMA_SxCR_CHSEL |
							DMA_SxCR_PL |
							DMA_SxCR_MSIZE |
							DMA_SxCR_PSIZE |
							DMA_SxCR_PINC |
							DMA_SxCR_MINC |
							DMA_SxCR_DIR |
							DMA_SxCR_EN);
	//wait til disabled
	while(DMA1_Stream2->CR & DMA_SxCR_EN); //rx

	//SELECT CH3 0x3, 0b011.
	DMA1_Stream2->CR |= DMA_SxCR_CHSEL_0 | DMA_SxCR_CHSEL_1;
	//MEM INCR
	DMA1_Stream2->CR |= DMA_SxCR_MINC;
	//enable cr_dir. periph to mem. 0b00, 0x00
	DMA1_Stream2->CR &= ~(DMA_SxCR_DIR_1 | DMA_SxCR_DIR_0);


	//and for dma1stream4
	DMA1_Stream4->CR &= ~(DMA_SxCR_CHSEL |
							DMA_SxCR_PL |
							DMA_SxCR_MSIZE |
							DMA_SxCR_PSIZE |
							DMA_SxCR_PINC |
							DMA_SxCR_MINC |
							DMA_SxCR_DIR |
							DMA_SxCR_EN);
	//wait til disabled
	while(DMA1_Stream4->CR & DMA_SxCR_EN);	//tx
	//SELECT CH3 0x3, 0b011.
	DMA1_Stream4->CR |= DMA_SxCR_CHSEL_0 | DMA_SxCR_CHSEL_1;
	//MEM INCR
	DMA1_Stream4->CR |= DMA_SxCR_MINC;
	//enable cr_dir MEM TO PERIPH 0x1, 0b01.
	DMA1_Stream4->CR |= DMA_SxCR_DIR_0;
	DMA1_Stream4->CR &= ~DMA_SxCR_DIR_1;

	/* DMA interrupt init */
	/* DMA1_Stream2_IRQn interrupt configuration */
	NVIC_SetPriority(DMA1_Stream2_IRQn, 0);
	NVIC_EnableIRQ(DMA1_Stream2_IRQn);
	/* DMA1_Stream4_IRQn interrupt configuration */
	NVIC_SetPriority(DMA1_Stream4_IRQn, 0);
	NVIC_EnableIRQ(DMA1_Stream4_IRQn);

	//ENABLE
	DMA1_Stream2->CR |= DMA_SxCR_EN; //rx
	DMA1_Stream4->CR |= DMA_SxCR_EN; //tx
}



/***************************/
/**
  * @brief my_I2C3_Init, reg init
  * @param None
  * @retval None
  */
static void my_I2C3_Init(void)
{

	I2C3->CR1	&= ~I2C_CR1_PE;
	//wait til disabled
	while(I2C3->CR1 & I2C_CR1_PE);

	//sw reset after all i2c gpio config. cr1bit15, to 1 and then back to 0.
	//I2C3->CR1 |= I2C_CR1_SWRST;
	//I2C3->CR1 &= ~I2C_CR1_SWRST;

	//I2C3->CR2 |= I2C_CR2_ITERREN;
	//I2C3->CR2 |= I2C_CR2_ITEVTEN;

	I2C3->CR2 |= 0x10;
	I2C3->CCR |= 0x50;
	I2C3->TRISE |= 0x11;

	//I2C3->CR2 |= I2C_CR2_DMAEN;

	I2C3->CR1 |= I2C_CR1_PE;	//i2c start

}


/************************/
/**
  * @brief I2C_Read_1Byte, not by dma right now
  * @param chip address, register address, read buffer, size of read
  * @retval None
  */
void I2C_Read_1Byte (uint8_t uc_Dev_Address, uint8_t uc_Reg_Address, uint8_t *uc_pReadBuffer)
{
	//I2C START
	I2C3->CR1 |= I2C_CR1_ACK;
	I2C3->CR1 |= I2C_CR1_START;
	while (!(I2C3->SR1 & I2C_SR1_SB));
	//I2C SEND ADDRESS
	I2C3->DR = (uc_Dev_Address);  //  send the device address
	while (!(I2C3->SR1 & (1<<1)));  // wait for ADDR bit to set
	uint8_t temp = I2C3->SR1 | I2C3->SR2;  // read SR1 and SR2 to clear the ADDR bit
	//READ ACK HERE TO SEE IF SLAVE HAS SENT IT

	//I2C WRITE
	while (!(I2C3->SR1 & (1<<7)));  // wait for TXE bit to set
	I2C3->DR = uc_Reg_Address; //0xO IS REG VALUE TO WRITE INTO THE I2C DR
	while (!(I2C3->SR1 & (1<<2)));  // wait for BTF bit to set

	//I2C START
	I2C3->CR1 |= I2C_CR1_ACK;
	I2C3->CR1 |= I2C_CR1_START;
	while (!(I2C3->SR1 & I2C_SR1_SB));

	/**** STEP 1-a ****/
	temp = I2C3->DR; //dummy read?
	I2C3->DR = (uc_Dev_Address+0x01);  //  send the device address+0x01, during the Read function. Basically we need to set the R/W bit (Bit 0) HIGH during the Read operation. This is common for all the devices that you will use for the I2C.
	while (!(I2C3->SR1 & (1<<1)));  // wait for ADDR bit to set

	/**** STEP 1-b ****/
	I2C3->CR1 &= ~(1<<10);  // clear the ACK bit
	temp = I2C3->SR1 | I2C3->SR2;  // read SR1 and SR2 to clear the ADDR bit
	I2C3->CR1 |= (1<<9);  // Stop I2C

	/**** STEP 1-c ****/
	while (!(I2C3->SR1 & (1<<6)));  // wait for RxNE to set

	/**** STEP 1-d ****/
	uc_pReadBuffer = I2C3->DR;  // Read the data from the DATA REG
}



void I2C_Write_Via_DMA(uint8_t uc_Dev_Address, uint8_t uc_Reg_Address, uint8_t *uc_pWriteBuffer, uint8_t size)
{
	/* how embeddedexpert does it.. https://blog.embeddedexpert.io/?p=624
	Wait until the bus is free
	while(I2C1->SR2&I2C_SR2_BUSY){;}
	Generate START
	I2C1->CR1 |= I2C_CR1_START;
	Wait SB flag is set
	while(!(I2C1->SR1&I2C_SR1_SB)){;}
	Read SR1
	(void)I2C1->SR1;
	Send slave address with write
	I2C1->DR = (SensorAddr<<1);
	Wait ADDR flag is set
	while(((I2C1->SR1)&I2C_SR1_ADDR)==0){;}
	Start DMA
	DMA_Transmit(pWriteBuffer, NumByteToWrite);
	Read SR1
	(void)I2C1->SR1;
	Read SR2
	(void)I2C1->SR2;*/

	//AND ME...
	//I2C START
	I2C3->CR1 |= I2C_CR1_ACK;
	I2C3->CR1 |= I2C_CR1_START;
	while (!(I2C3->SR1 & I2C_SR1_SB));
	//I2C SEND ADDRESS
	I2C3->DR = (uc_Dev_Address);  //  send the device address
	while (!(I2C3->SR1 & (1<<1)));  // wait for ADDR bit to set
	uint8_t temp = I2C3->SR1 | I2C3->SR2;  // read SR1 and SR2 to clear the ADDR bit
	//READ ACK HERE TO SEE IF SLAVE HAS SENT IT

	//I2C WRITE
	while (!(I2C3->SR1 & (1<<7)));  // wait for TXE bit to set
	I2C3->DR = uc_Reg_Address; //0xO IS REG VALUE TO WRITE INTO THE I2C DR
	while (!(I2C3->SR1 & (1<<2)));  // wait for BTF bit to set

	//I2C START
	I2C3->CR1 |= I2C_CR1_ACK;
	I2C3->CR1 |= I2C_CR1_START;
	while (!(I2C3->SR1 & I2C_SR1_SB));

	/**** STEP 1-a ****/
	temp = I2C3->DR; //dummy read?
	I2C3->DR = (uc_Dev_Address+0x01);  //  send the device address+0x01, during the Read function. Basically we need to set the R/W bit (Bit 0) HIGH during the Read operation. This is common for all the devices that you will use for the I2C.
	while (!(I2C3->SR1 & (1<<1)));  // wait for ADDR bit to set

	//dma write function in here...
	DMA_Transmit(uc_pWriteBuffer, size);
	/**** STEP 1-b ****/
	I2C3->CR1 &= ~(1<<10);  // clear the ACK bit
	temp = I2C3->SR1 | I2C3->SR2;  // read SR1 and SR2 to clear the ADDR bit
	I2C3->CR1 |= (1<<9);  // Stop I2C

	/**** STEP 1-c ****/
	while (!(I2C3->SR1 & (1<<6)));  // wait for RxNE to set

	/**** STEP 1-d ****/
	uint8_t uc_readBuffer = I2C3->DR;  // Read the data from the DATA REG
}


static void DMA_Transmit(const uint8_t * pBuffer, uint8_t size)
{
	  /* Check null pointers */
	  if(NULL != pBuffer)
	  {
	    DMA1_Stream4->CR&=~DMA_SxCR_EN;
		while((DMA1_Stream4->CR)&DMA_SxCR_EN){;}

	    /* Set memory address */
	    DMA1_Stream4->M0AR = (uint32_t)pBuffer;
			DMA1_Stream4->PAR=(uint32_t)&I2C1->DR;
	    /* Set number of data items */
	    DMA1_Stream4->NDTR = size;

	    /* Clear all interrupt flags */
		DMA1->LIFCR |= DMA_LIFCR_CTCIF2;

		DMA1->LIFCR |= DMA_LIFCR_CHTIF2;

		DMA1->LIFCR |= DMA_LIFCR_CTEIF2;

		DMA1->LIFCR |= DMA_LIFCR_CDMEIF2;

		DMA1->LIFCR |= DMA_LIFCR_CFEIF2;

	    /* Enable DMA1_Stream4 */
	    DMA1_Stream4->CR |= DMA_SxCR_EN;
	  }
	  else
	  {
	    /* Null pointers, do nothing */
	  }
}

void I2C_Read_Via_DMA (uint8_t uc_Dev_Address, uint8_t uc_Reg_Address, uint8_t *uc_pReadBuffer, uint8_t size)
{
	//embedded expert...
	/*wait until the bus is free
	while(I2C1->SR2&I2C_SR2_BUSY){;}
	Generate START
	I2C1->CR1 |= I2C_CR1_START;
	Wait SB flag is set
	while(!(I2C1->SR1&I2C_SR1_SB)){;}
	Read SR1
	(void)I2C1->SR1;
	Send slave address with write
	I2C1->DR=(SensorAddr<<1|0);
	Wait ADDR flag is set
	while(((I2C1->SR1)&I2C_SR1_ADDR)==0){;}
	Read SR1
	(void)I2C1->SR1;
	Read SR2
	(void)I2C1->SR2;
	Wait TXE flag is set
	while(I2C_SR1_TXE != (I2C_SR1_TXE & I2C1->SR1))
	{
		Do nothing
	}
	if(2 <= NumByteToRead)
	{
		Acknowledge enable
		I2C1->CR1 |= I2C_CR1_ACK;
		Send register address to read with increment
		I2C1->DR =  (ReadAddr);
	}
	else
	{
		Acknowledge disable
		I2C1->CR1 &= ~I2C_CR1_ACK;
		Send register address to read (single)
		I2C1->DR =  ReadAddr;
	}
	Wait BTF flag is set
	while(!(I2C_SR1_BTF & I2C1->SR1))
	{
		Do nothing
	}
	Generate ReSTART
	I2C1->CR1 |= I2C_CR1_START;

	Wait SB flag is set
	while(I2C_SR1_SB != (I2C_SR1_SB & I2C1->SR1))
	{
		Do nothing
	}
	Read SR1
	(void)I2C1->SR1;
	Send slave address with read
	I2C1->DR =  (SensorAddr<<1 | (uint8_t)0x01);
	Wait ADDR flag is set
	while(((I2C1->SR1)&I2C_SR1_ADDR)==0){;}
	Start DMA
	DMA_Receive(pReadBuffer, NumByteToRead);
	Read SR1
	(void)I2C1->SR1;
	Read SR2
	(void)I2C1->SR2;
	*/

}
static void DMA_Receive(uint8_t* pBuffer, uint8_t sizeTransfer)
{
	//i2c configuration..
	// 1. Ensure the stream is disabled before configuring
	DMA1_Stream0->CR &= ~DMA_SxCR_EN;
	while(DMA1_Stream0->CR & DMA_SxCR_EN);

	//set periph address, i2c dr.
	DMA1_Stream2->PAR = (uint32_t)&(I2C3->DR);
	//set memory address, pBuffer
	DMA1_Stream2->M0AR = *pBuffer;
	//size of xfer
	DMA1_Stream2->NDTR = sizeTransfer;

	DMA1_Stream0->CR |= DMA_SxCR_EN;

}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
