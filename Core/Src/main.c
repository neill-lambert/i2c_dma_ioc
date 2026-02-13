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
//#include "stm32f4xx_hal_dma.h"
#include <math.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//register addresses
#define STMPE811_DEVICE_ADDRESS 0x82

#define STMPE811_REG_CHP_ID_MSB 0x00
#define STMPE811_REG_CHP_ID_LSB 0x01
#define STMPE811_REG_SYS_CTRL1  0x03
#define STMPE811_REG_SYS_CTRL2  0x04
#define STMPE811_REG_INT_STA	0x0B
#define STMPE811_REG_TO_AF		0x17
#define STMPE811_REG_ADC_CTRL1	0x20
#define STMPE811_REG_ADC_CTRL2	0x21
#define STMPE811_REG_TSC_CTRL	0x40
#define STMPE811_REG_TSC_CFG	0x41
#define STMPE811_REG_FIFO_TH	0x4A
#define STMPE811_REG_FIFO_STA	0x4B
#define STMPE811_REG_FIFO_SIZE	0x4C
#define STMPE811_REG_TSC_FRACT_XYZ 0x56
#define STMPE811_REG_TSC_I_DRIVE   0x58
#define STMPE811_REG_TSC_DATA_NON_INC 0xD7

//register values

#define REG_SYS_CTRL2_ADC_FCT	0x01
#define REG_SYS_CTRL2_TSC_FCT	0x02
#define REG_SYS_CTRL2_IO_FCT	0x04
#define REG_SYS_CTRL2_TS_FCT	0x08
#define REG_SYS_CTRL1_SOFT_RESET_ON	0x02
#define REG_SYS_CTRL1_SOFT_RESET_OFF 0x00
#define REG_ADC_CTRL1_12_BIT_ADC	(0x01 << 3)
#define REG_ADC_CTRL1_SAMPLE_TIME_80CLK	(0x04 << 4)
#define REG_ADC_CTRL2_3_25MHZ	0x01
#define REG_TSC_CFG_4_SAMPLES	(0x02 << 6)
#define REG_TSC_CFG_TOUCH_DELAY_500uS	(0x03 << 3)
#define REG_TSC_CFG_SETTLING_TIME_500Us	0x02
#define REG_TSC__TS_CTRL_STATUS	0x80
#define REG_IO_AF_TOUCH_IO_ALL	0xF0

#define I2CxTIMEOUT			10 //mS  //not using this right now???
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c3;
DMA_HandleTypeDef hdma_i2c3_rx;
DMA_HandleTypeDef hdma_i2c3_tx;

/* USER CODE BEGIN PV */
uint8_t Val2 = 0;
uint8_t value = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
//void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c);
void I2CTransfer ( void ); //unfinished xfer
static void my_I2C3_Init(void);
static void my_DMA_Init(void);
static void my_GPIO_Init(void);
uint8_t I2C_Read_1Byte (uint8_t uc_Dev_Address, uint8_t uc_Reg_Address);
void I2C_Write_1Byte  (uint8_t uc_Dev_Address, uint8_t uc_Reg_Address, uint8_t uc_Data);
void I2C_Read_Via_DMA (uint8_t uc_Dev_Address, uint8_t uc_Reg_Address, uint8_t *uc_pReadBuffer, uint8_t size);
static void DMA_Receive(uint8_t* pBuffer, uint8_t sizeReceive);
static void DMA_Transmit(const uint8_t * pBuffer, uint8_t size);
void ReleaseSerialBus( void );
void stmpe811_TS_Start(uint8_t DeviceAddr);
void stmpe811_IO_EnableAF(uint8_t DeviceAddr, uint8_t IO_Pin);
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
  //look at the order of these...
  my_GPIO_Init();

  if(!(GPIOC->IDR & GPIO_IDR_ID9)) //sda low if i2c3 locked
	  	  	  ReleaseSerialBus();
  my_I2C3_Init();
  //ReleaseSerialBus();

  my_DMA_Init();



  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */

  /* USER CODE BEGIN 2 */
  I2C_Write_1Byte(STMPE811_DEVICE_ADDRESS, STMPE811_REG_SYS_CTRL1, REG_SYS_CTRL1_SOFT_RESET_OFF);
  I2C_Read_1Byte(0x82, STMPE811_REG_SYS_CTRL1);
	HAL_Delay(10);
  I2C_Write_1Byte(STMPE811_DEVICE_ADDRESS, STMPE811_REG_SYS_CTRL1, REG_SYS_CTRL1_SOFT_RESET_ON);
  I2C_Read_1Byte(0x82, STMPE811_REG_SYS_CTRL1);
	HAL_Delay(10);
  stmpe811_TS_Start(STMPE811_DEVICE_ADDRESS);


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


/* USER CODE BEGIN 4 */
///**
//  * @brief HAL_I2C_MasterTxCpltCallback
//  * @param I2C_HandleTypeDef
//  * @retval None
//  */
//void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c) {
//    if(hi2c->Instance == I2C3) {
//        // Handle successful reception
//    	asm("nop");
//    }
//}


/************************/
/**
  * @brief my_GPIO_Init, reg init
  * @param None
  * @retval None
  */
static void my_GPIO_Init(void)
{
	//enable i2c3 clock
	 RCC->APB1ENR |= RCC_APB1ENR_I2C3EN;
	 while(!(RCC->APB1ENR & RCC_APB1ENR_I2C3EN)){;};

	//enable clock access to GPIOC and A
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIOAEN;

	//GPIOA8 TO AF
	GPIOA->MODER |= GPIO_MODER_MODER8_1;
	GPIOA->MODER &= ~GPIO_MODER_MODER8_0;

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
	GPIOC->MODER &= ~GPIO_MODER_MODER9_0;
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
	//and interrupts
	DMA1_Stream2->CR |= (DMA_SxCR_TCIE | DMA_SxCR_HTIE);


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
	//and interrupts
	DMA1_Stream4->CR |= (DMA_SxCR_TCIE | DMA_SxCR_HTIE);

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

	I2C3->CR1 |= I2C_CR1_PE;	//i2c start

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

//	I2C3->CR2 |= I2C_CR2_ITERREN;
//	I2C3->CR2 |= I2C_CR2_ITEVTEN;
//	NVIC_EnableIRQ(I2C3_EV_IRQn);

	I2C3->CR2 |= 0x10;
	I2C3->CCR |= 0x50;
	I2C3->TRISE |= 0x11;

//	I2C3->CR1 &= ~I2C_CR1_ENGC; //disable general call
	I2C3->CR2 |= I2C_CR2_LAST;  //set next DMA EOT is last transfer


//	I2C3->CR2 |= I2C_CR2_DMAEN;

}


/************************/
/**
  * @brief I2C_Read_1Byte, not by dma right now
  * @param chip address, register address, read buffer, size of read
  * @retval None
  */
uint8_t I2C_Read_1Byte (uint8_t uc_Dev_Address, uint8_t uc_Reg_Address)
{
	//I2C START
	I2C3->CR1 |= I2C_CR1_ACK;
	I2C3->CR1 |= I2C_CR1_START;
	while (!(I2C3->SR1 & I2C_SR1_SB));
	(void) I2C3->SR1;

	//I2C SEND ADDRESS
	I2C3->DR = (uc_Dev_Address);  //  send the device address
	while (!(I2C3->SR1 & (1<<1)));  // wait for ADDR bit to set
	// ack disable before clearing addr
	I2C3->CR1 &= ~I2C_CR1_ACK;
	// read SR1 and SR2 to clear the ADDR bit
	(void) I2C3->SR1;
	(void) I2C3->SR2;
	//READ ACK HERE TO SEE IF SLAVE HAS SENT IT

	//I2C WRITE
	while (!(I2C3->SR1 & (1<<7)));  // wait for TXE bit to set
	I2C3->DR = uc_Reg_Address;
	while (!(I2C3->SR1 & (1<<2)));  // wait for BTF bit to set

	//I2C START
	I2C3->CR1 |= I2C_CR1_ACK;
	I2C3->CR1 |= I2C_CR1_START;
	while (!(I2C3->SR1 & I2C_SR1_SB));

	//(void) I2C3->DR; //dummy read?
	I2C3->DR = ((uc_Dev_Address)+0x01);  //  send the device address+0x01, during the Read function. Basically we need to set the R/W bit (Bit 0) HIGH during the Read operation. This is common for all the devices that you will use for the I2C.
	while (!(I2C3->SR1 & (1<<1)));  // wait for ADDR bit to set

	I2C3->CR1 &= ~(1<<10);  // clear the ACK bit
	// read SR1 and SR2 to clear the ADDR bit
	(void) I2C3->SR1;
	(void) I2C3->SR2;

	I2C3->CR1 |= (1<<9);  // Stop I2C

	while (!(I2C3->SR1 & (1<<6)));  // wait for RxNE to set
	Val2 = I2C3->DR;
	return I2C3->DR;  // return the data from the DATA REG
}

void I2C_Write_1Byte  (uint8_t uc_Dev_Address, uint8_t uc_Reg_Address, uint8_t uc_Data) //rm0090 p849
{

	//I2C START
	I2C3->CR1 |= I2C_CR1_ACK;
	I2C3->CR1 |= I2C_CR1_START;
	while (!(I2C3->SR1 & I2C_SR1_SB));
	//I2C SEND ADDRESS
	I2C3->DR = (uc_Dev_Address);  //  send the device address
	while (!(I2C3->SR1 & (1<<1)));  // wait for ADDR bit to set
	// ack disable before clearing addr
	//I2C3->CR1 &= ~I2C_CR1_ACK;
	// read SR1 and SR2 to clear the ADDR bit
	(void) I2C3->SR1;
	(void) I2C3->SR2;
	//READ ACK HERE TO SEE IF SLAVE HAS SENT IT

	//I2C WRITE
	while (!(I2C3->SR1 & (1<<7)));  // wait for TXE bit to set
	//wait for dr to be empty?
	I2C3->DR = (uint16_t)uc_Reg_Address; //0xO IS REG VALUE TO WRITE INTO THE I2C DR

	while (!(I2C3->SR1 & (1<<7)));  // wait for TXE bit to set
	//wait for dr to be empty?
	I2C3->DR = ((uc_Data) + 0x00);  //  send the device address+0x01, during the write function. Basically we need to set the R/W bit (Bit 0) low during the write operation. This is common for all the devices that you will use for the I2C.
	Val2 = I2C3->DR;
	while (!(I2C3->SR1 & (1<<7)));  // wait for TXE bit to set
	while (!(I2C3->SR1 & (1<<2)));  // wait for BTF bit to set

	//txe and btf = 1, cleared by stop condition
	I2C3->CR1 |= (1<<9);  // Stop I2C

	(void) I2C3->SR1;
	(void) I2C3->DR;

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

		DMA1_Stream4->PAR=(uint32_t)&I2C3->DR;
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

void I2C_Read_Via_DMA (uint8_t uc_Dev_Address, uint8_t uc_Reg_Address, uint8_t *uc_pReadBuffer, uint8_t size) //used to read streams of data from peripheral. eg, x and y coordinates.
{
	//wait until the bus is free
	while(I2C3->SR2&I2C_SR2_BUSY){;}

	//I2C START
	//I2C3->CR1 |= I2C_CR1_ACK;
	I2C3->CR1 |= I2C_CR1_START;
	while(!(I2C3->SR1&I2C_SR1_SB)){;} //while (!(I2C3->SR1 & I2C_SR1_SB));

	(void) I2C3->SR1;

	I2C3->DR = (uc_Dev_Address | 0);  //send the device address with write

	while(((I2C3->SR1)&I2C_SR1_ADDR)==0){;} //while (!(I2C3->SR1 & (1<<1)));  // wait for ADDR bit to set

	(void) I2C3->SR1;
	(void) I2C3->SR2;  // read SR1 and SR2 to clear the ADDR bit
	//READ ACK HERE TO SEE IF SLAVE HAS SENT IT

	//I2C WRITE
	while(I2C_SR1_TXE != (I2C_SR1_TXE & I2C3->SR1))	//while (!(I2C3->SR1 & (1<<7)));  // wait for TXE bit to set
	{
		//do nothing
	}

	if (2 <= size)
	{
		I2C3->CR1 |= I2C_CR1_ACK; // ack enable
		I2C3->DR = (uc_Reg_Address);
	}
	else
	{
		I2C3->CR1 &= ~I2C_CR1_ACK; // ack disable
		I2C3->DR = uc_Reg_Address;
	}

	// wait for BTF bit to set
	while (!(I2C_SR1_BTF & I2C3->SR1))
	{
		//DO NOTHING
	}
	//I2C START
	//I2C3->CR1 |= I2C_CR1_ACK;
	I2C3->CR1 |= I2C_CR1_START;

	//WAIT FOR SB FLAG TO SET
	while (I2C_SR1_SB != (I2C_SR1_SB & I2C3->SR1))
	{
		//DO NOTHING
	}
	//read sr1
	(void) I2C3->SR1;
	//send slave address with read
	I2C3->DR = ( uc_Dev_Address | (uint8_t) 0x01);
	while(((I2C3->SR1)&I2C_SR1_ADDR)==0){;}
	//start DMA
	DMA_Receive(uc_pReadBuffer, size);

	//read sr1 and sr2
	(void) I2C3->SR1;
	(void) I2C3->SR2;
}

static void DMA_Receive(uint8_t* pBuffer, uint8_t sizeReceive)
{
	if (pBuffer != NULL)
	{
		//i2c configuration..
		// 1. Ensure the stream is disabled before configuring
		DMA1_Stream2->CR &= ~DMA_SxCR_EN;
		while(DMA1_Stream2->CR & DMA_SxCR_EN);

		//set periph address, i2c dr.
		DMA1_Stream2->PAR = (uint32_t)&I2C3->DR;
		//set memory address, pBuffer
		DMA1_Stream2->M0AR = (uint32_t)pBuffer;
		//size of xfer
		DMA1_Stream2->NDTR = sizeReceive;
		//clear all interrupts
		DMA1->LIFCR |= DMA_LIFCR_CTCIF2;

		DMA1->LIFCR |= DMA_LIFCR_CHTIF2;

		DMA1->LIFCR |= DMA_LIFCR_CTEIF2;

		DMA1->LIFCR |= DMA_LIFCR_CDMEIF2;

		DMA1->LIFCR |= DMA_LIFCR_CFEIF2;

		//enable dma
		DMA1_Stream2->CR |= DMA_SxCR_EN;

	}
	else
	{
		//do nothing
	}
}

void ReleaseSerialBus( void )
{
	I2C3->CR1 &= ~I2C_CR1_PE;	//TURN PE OFF

	//sda to op mode oopen drain
	GPIOC->MODER |= GPIO_MODER_MODER9_0;
	GPIOC->MODER &= ~GPIO_MODER_MODER9_1;

	//sda to open drain
	GPIOC->OTYPER |= GPIO_OTYPER_OT9;


	//sclk to op mode oopen drain
	GPIOA->MODER |= GPIO_MODER_MODER8_0;
	GPIOA->MODER &= ~GPIO_MODER_MODER8_1;

	//sclk to open drain
	GPIOA->OTYPER |= GPIO_OTYPER_OT8;


	//write 1 to ODR
	GPIOA->ODR |= (1<<8);
	GPIOC->ODR |= (1<<9);

	//check sclk and sda idr are high..wait while low
	while(!(GPIOA->IDR & (1<<8))){;};
	while(!(GPIOC->IDR & (1<<9))){
		for (int i = 0; i < 15; i++)
		{
		GPIOA->ODR &= ~(1<<8); //toggle clock in bursts of 15 til sda is released. this works!
		GPIOA->ODR |= (1<<8);
		}
	};

	// Configure the SDA I/O as General Purpose Output Open-Drain, Low level (Write 0 to
	//GPIOx_ODR).
	GPIOC->ODR &= ~(1<<9);
	//check its low, ie wait while high
	while(GPIOC->IDR & (1<<9)){;};

	// Configure the SCLK I/O as General Purpose Output Open-Drain, Low level (Write 0 to
	//GPIOx_ODR).
	GPIOA->ODR &= ~(1<<8);
	//check its low, ie wait while high
	while(GPIOA->IDR & (1<<8)){;};

	// Configure the SCLK I/O as General Purpose Output Open-Drain, High level (Write 1 to
	//GPIOx_ODR).
	GPIOA->ODR |= (1<<8);
	//check its high, ie wait while low
	while(!(GPIOA->IDR & (1<<8))){;};

	// Configure the SDA I/O as General Purpose Output Open-Drain, High level (Write 1 to
	//GPIOx_ODR).
	GPIOC->ODR |= (1<<9);
	//check its high, ie wait while low
	while(!(GPIOC->IDR & (1<<9))){;};

	//configure sda and sclk as af od.

	//GPIOA8 TO AF
	GPIOA->MODER |= GPIO_MODER_MODER8_1;
	GPIOA->MODER &= ~GPIO_MODER_MODER8_0;

	//GPIOC9 TO AF
	GPIOC->MODER |= GPIO_MODER_MODER9_1;
	GPIOC->MODER &= ~GPIO_MODER_MODER9_0;

	//AF INDEX FOR A8 TO I2C3SCL
	// AFRH AF4, 0b100, 0x4.
	GPIOA->AFR[1] |= GPIO_AFRH_AFRH0_2;
	//AF INDEX FOR C9 TO I2C3SDA
	// AFRH AF4, 0b100, 0x4.
	GPIOC->AFR[1] |= GPIO_AFRH_AFRH1_2;

	//sw reset
	I2C3->CR1 |= I2C_CR1_SWRST;
	I2C3->CR1 &= ~I2C_CR1_SWRST;

	//enable pe
	I2C3->CR1 |= I2C_CR1_PE;

	//more from stack exchange... https://electronics.stackexchange.com/questions/272427/stm32-busy-flag-is-set-after-i2c-initialization
}

void stmpe811_TS_Start(uint8_t DeviceAddr)
{
	uint8_t uc_Mode;
	//get current reg value for ctrl 2
	uc_Mode = I2C_Read_1Byte(DeviceAddr, STMPE811_REG_SYS_CTRL2);

	//set functionalities to be enabled
	uc_Mode &= ~(REG_SYS_CTRL2_IO_FCT); //turn everything off

	//write new reg val
	I2C_Write_1Byte(DeviceAddr, STMPE811_REG_SYS_CTRL2, uc_Mode);
	I2C_Read_1Byte(DeviceAddr, STMPE811_REG_SYS_CTRL2);
	//sel tsc pins in tsc af mode
	stmpe811_IO_EnableAF(DeviceAddr, REG_IO_AF_TOUCH_IO_ALL);
	//set functionalities to enable
	//tsc adc and ts compensation enable
	uc_Mode &= ~(REG_SYS_CTRL2_TSC_FCT | REG_SYS_CTRL2_ADC_FCT | REG_SYS_CTRL2_TS_FCT);
	//SET NEW REG VAL
	//NB TEMP SENSOR NEEDED TO COMPENSATE TOUCH SCREEN PARAMS
	//ADC IS USED FOR 4 WIRE TOUCH SCREEN OPERATION
	I2C_Write_1Byte(DeviceAddr, STMPE811_REG_SYS_CTRL2, uc_Mode);
	// select sample time bit number adc ref
	I2C_Write_1Byte(DeviceAddr, STMPE811_REG_ADC_CTRL1, REG_ADC_CTRL1_12_BIT_ADC | REG_ADC_CTRL1_SAMPLE_TIME_80CLK);
	//WAIT 2MS
	//HAL_Delay(5);
	//select adc clk speed 3.25mhz
	I2C_Write_1Byte(DeviceAddr, STMPE811_REG_ADC_CTRL2, REG_ADC_CTRL2_3_25MHZ);
	//SET 2nF FILTER CAP
	I2C_Write_1Byte(DeviceAddr, STMPE811_REG_TSC_CFG, REG_TSC_CFG_4_SAMPLES | REG_TSC_CFG_TOUCH_DELAY_500uS | REG_TSC_CFG_SETTLING_TIME_500Us);

	//CFG TOUCH FIFO THRESHOLD
	I2C_Write_1Byte(DeviceAddr, STMPE811_REG_FIFO_TH, 0x01);
	//CLEAR FIFO MEM
	I2C_Write_1Byte(DeviceAddr, STMPE811_REG_FIFO_STA, 0x01);
	//BACK INTO OPERATION MODE
	I2C_Write_1Byte(DeviceAddr, STMPE811_REG_FIFO_STA, 0x00);
	//SET RANGE + ACCURACY OF PRESSURE MEASUREMENT
	I2C_Write_1Byte(DeviceAddr, STMPE811_REG_TSC_FRACT_XYZ, 0x01);
	// SET RECHARGE LIMIT FOR TSC PINS 50mA
	I2C_Write_1Byte(DeviceAddr, STMPE811_REG_TSC_I_DRIVE, 0x01);
	//ENABLE TSC
	I2C_Write_1Byte(DeviceAddr, STMPE811_REG_TSC_CTRL, 0x01);
	//CLEAR ALL STATUS PENDING BITS IF ANY
	I2C_Write_1Byte(DeviceAddr, STMPE811_REG_INT_STA, 0xFF);
	//DELAY
	HAL_Delay(2);
}

void stmpe811_IO_EnableAF(uint8_t DeviceAddr, uint8_t IO_Pin)
{
	uint8_t tmp = 0;
	//get current reg value
	tmp = I2C_Read_1Byte(DeviceAddr, STMPE811_REG_TO_AF);
	//enable selected pin af
	tmp &= ~(uint8_t) IO_Pin;
	//and write it back
	I2C_Write_1Byte(DeviceAddr, STMPE811_REG_TO_AF, tmp);
}

//void DMA1_Stream2_IRQHandler(void) //rx
//			{
//
//			if((DMA1->LISR)&DMA_LISR_TCIF2)
//					{
//					//finished=1;
//					//log_debug("I2C finished receiving using DMA1_Stream2");
//					I2C3->CR1 |= I2C_CR1_STOP;
//					DMA1->LIFCR=DMA_LIFCR_CTCIF2;
//					}
//			if((DMA1->LISR)&DMA_LISR_HTIF2)
//					{
//					//log_debug("DMA1 stream2 half transfer interrupt");
//					DMA1->LIFCR=DMA_LIFCR_CHTIF2;
//					}
//
//			if((DMA1->LISR)&DMA_LISR_TEIF2)
//					{
//					//log_debug("DMA1 stream5 error");
//					DMA1->LIFCR=DMA_LIFCR_CTEIF2;
//					}
//			}
//
//void DMA1_Stream4_IRQHandler(void)	//tx
//			{
//
//			if((DMA1->HISR)&DMA_HISR_TCIF4)
//					{
//					//log_debug("I2C finished transmiting using DMA1_Stream6");
//					//finished=1;
//					I2C3->CR1 |= I2C_CR1_STOP;
//					DMA1->HIFCR=DMA_HIFCR_CTCIF4;
//
//					}
//			if((DMA1->HISR)&DMA_HISR_HTIF4)
//					{
//					//log_debug("DMA1 stream6 half transfer interrupt");
//					DMA1->HIFCR=DMA_HIFCR_CHTIF4;
//					}
//
//			if((DMA1->HISR)&DMA_HISR_TEIF4)
//					{
//					//log_debug("DMA1 stream6 error");
//					DMA1->HIFCR=DMA_HIFCR_CTEIF4;
//					}
//
//			}


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
