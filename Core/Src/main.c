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
#define STMPE811_REG_INT_EN		0x0A
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
#define STMPE811_REG_INT_CTRL	0X09

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
#define REG_TSC_CTRL_OP_MODE_Z_ONLY 	(0X01 << 3)

#define I2CxTIMEOUT			10 //mS
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
//touch info
uint16_t us_TouchPointX, us_TouchPointY; 		//x/y locations
uint8_t uc_TouchDetected;
volatile uint8_t g_I2C_TransferComplete;
// Global Statistics
volatile uint32_t stats_TotalTouches = 0;
volatile uint32_t stats_I2C_Recoveries = 0;
volatile uint32_t stats_DMA_Timeouts = 0;
volatile uint32_t stats_StuckPinKicks = 0; // When IDR was low but no EXTI fired

volatile uint32_t ul_g_ms_ticks = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
static void delay_ms(uint32_t ms);
void stmpe811_TS_Start(uint8_t DeviceAddr);
static void my_I2C3_Init(void);
static void my_DMA_Init(void);
static void my_GPIO_Init(void);
uint8_t I2C_Read_1Byte (uint8_t uc_Dev_Address, uint8_t uc_Reg_Address);
void I2C_Write_1Byte  (uint8_t uc_Dev_Address, uint8_t uc_Reg_Address, uint8_t uc_Data);
i2c_status_t I2C_Read_Via_DMA (uint8_t uc_Dev_Address, uint8_t uc_Reg_Address, const uint8_t *uc_pReadBuffer, uint8_t size);
void ReleaseSerialBus( void );
void stmpe811_IO_EnableAF(uint8_t DeviceAddr, uint8_t IO_Pin);
void Touch_Init(uint8_t DeviceAddr);
uint8_t stmpe811_TS_DetectTouch(uint8_t DeviceAddr);
i2c_status_t stmpe811_TS_GetXY(uint16_t *X, uint16_t *Y);
void Touch_Process (void);
static i2c_status_t  I2C_Wait_Addr_With_Timeout(void);
void Touch_Interrupt_Init(void);
void STMPE811_Init_Interrupts(void);
static void my_SysTick_Init(void);
void STMPE811_Emergency_Clear(void);
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

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  //look at the order of these...
  my_GPIO_Init();
  my_SysTick_Init();  // Add this line

  if(!(GPIOC->IDR & GPIO_IDR_ID9)) //sda low if i2c3 locked
	  	  	  ReleaseSerialBus();
  my_I2C3_Init();
  my_DMA_Init();




  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */

  /* USER CODE BEGIN 2 */
  Touch_Init(STMPE811_DEVICE_ADDRESS);

  Touch_Interrupt_Init();

  STMPE811_Init_Interrupts();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  Touch_Process();
	  __asm volatile("nop");
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
	RCC->CR |= RCC_CR_HSEON;
	while(!(RCC->CR & RCC_CR_HSERDY));

	//set flash prefetch and latency settings.
	FLASH->ACR |= FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN | FLASH_ACR_LATENCY_15WS																																																														;

	//set timpre for a non zero ppre1 or ppre2 val. rm0090,p208
	RCC->DCKCFGR |= RCC_DCKCFGR_TIMPRE;

	//configure prescalars HCLK, PCLK1, PCLK2,
	//AHB prescalar.
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
	//APB1 prescalar.
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;
	//APB2 Prescalar
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;

	//pll settings
	RCC->PLLCFGR = (4 <<0) | (180 << 6) | (0 << 16) | (3 << 24) | (1 << 22);

	//enable pll and wait for ready
	RCC->CR |= RCC_CR_PLLON;
	while (!(RCC->CR & RCC_CR_PLLRDY));

	//select clock source
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

	//set pwr enable clock and voltage regulator
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	PWR->CR |= PWR_CR_VOS;

	//enable clock access
	//RCC_TypeDef *pRCC = (RCC_TypeDef *)RCC_BASE;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;
}


/* USER CODE BEGIN 4 */

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

	DMA1_Stream2->CR=0x00;//reset everything

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
    I2C3->CR1 &= ~I2C_CR1_PE;
    while(I2C3->CR1 & I2C_CR1_PE);

    // Use direct assignment (=) instead of (|=)
    I2C3->CR2 = 16;   // 16 MHz PCLK1
    I2C3->CCR = 80;   // 100 kHz Standard Mode

    // Increase TRISE slightly to be more "probe friendly"
    // This gives the signals more time to rise with the extra probe capacitance
    I2C3->TRISE = 20;

    I2C3->CR1 |= I2C_CR1_PE;
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
	//(void) I2C3->DR;
	while ((I2C3->SR2 & (1<<1)));  //wait while bus busy
	return I2C3->DR;  // return the data from the DATA REG

}
/****************************
 * @brief
 *
 * @param
 * @retval
 */
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
	while (!(I2C3->SR1 & (1<<7)));  // wait for TXE bit to set
	while (!(I2C3->SR1 & (1<<2)));  // wait for BTF bit to set

	//txe and btf = 1, cleared by stop condition
	I2C3->CR1 |= (1<<9);  // Stop I2C
	while ((I2C3->SR2 & (1<<1)));  //wait while bus busy

	(void) I2C3->SR1;
	(void) I2C3->DR;

}

/****************************
 * @brief
 *
 * @param
 * @retval
 */
i2c_status_t  I2C_Read_Via_DMA (uint8_t uc_Dev_Address, uint8_t uc_Reg_Address, const uint8_t *uc_pReadBuffer, uint8_t size) //used to read streams of data from peripheral. eg, x and y coordinates.
{
	i2c_status_t status = I2C_OK;

	// Always start with a known completion state (prevents “missed IRQ” hangs/false completes)
	g_I2C_TransferComplete = 0;

	/* Optional but very useful: detect a physically hung BUSY before we start */
	uint32_t busy_timeout = 200000;
	while ((I2C3->SR2 & I2C_SR2_BUSY) && --busy_timeout) { __asm volatile ("nop"); }
	if (busy_timeout == 0U)
	{
		return I2C_ERR_BUS_HUNG;
	}
	// 1. PREPARE DMA1 STREAM 2 (I2C3 RX)
	    DMA1_Stream2->CR &= ~DMA_SxCR_EN;             // Disable to configure
	    DMA1_Stream2->M0AR = (uint32_t)uc_pReadBuffer;   // Destination memory
	    DMA1_Stream2->PAR  = (uint32_t)&I2C3->DR;      // Source peripheral
	    DMA1_Stream2->NDTR = size;                    // Number of bytes
	    DMA1_Stream2->CR &= ~DMA_SxCR_CHSEL; // Clear bits 25, 26, 27
	    DMA1_Stream2->CR |= (3 << 25);       // Set to Channel 3
	    DMA1_Stream2->CR  |= DMA_SxCR_MINC | DMA_SxCR_TCIE; // Mem Inc + Interrupt
	    DMA1_Stream2->CR  &= ~DMA_SxCR_DIR;           // Dir: Peri -> Mem

	    // 2. PHASE 1: Write Register Address to Sensor
	    I2C3->CR1 |= I2C_CR1_START;
	    while (!(I2C3->SR1 & I2C_SR1_SB));            // Wait for Start bit
	    I2C3->DR = (uc_Dev_Address);                 // Send Slave Addr (Write)
	    if ((status = I2C_Wait_Addr_With_Timeout()))
	    {
	    	return status; //either nack or timeout
	    }
	    //addr is set, so clear by reading sr2..
	    (void)I2C3->SR2;                              // Clear ADDR (Read SR1 then SR2)
	    I2C3->DR = uc_Reg_Address;                          // Send Register to read
	    while (!(I2C3->SR1 & I2C_SR1_BTF));           // Wait for Byte Transfer Finished

	    // 3. PHASE 2: Repeated Start for Reading
	    I2C3->CR1 |= I2C_CR1_START;
	    while (!(I2C3->SR1 & I2C_SR1_SB));
	    I2C3->DR = (uc_Dev_Address) | 0x01;          // Send Slave Addr (Read)
	    status = I2C_Wait_Addr_With_Timeout();
		if (status != I2C_OK)
	  	    {
	  	    	return status;
	  	    }

	    // 4. PHASE 3: Handover to DMA
	    I2C3->CR1 |= I2C_CR1_ACK;						//enable ack before clearing addr
	    I2C3->CR2 |= I2C_CR2_LAST;                    // Auto-NACK on last DMA byte
	    DMA1_Stream2->NDTR = size;                    // Number of bytes
	    DMA1_Stream2->CR |= DMA_SxCR_EN;              // Start the DMA Stream
	    I2C3->CR2 |= I2C_CR2_DMAEN;                   // Enable I2C DMA requests
	    (void) I2C3->SR1;
	    (void) I2C3->SR2;  // read SR1 and SR2 to clear the ADDR bit

	    //5. PHASE 4: wait for hardware to finish
	    uint32_t timeout = 100000;
	    while (g_I2C_TransferComplete == 0 && timeout--)
	    	{
	    	__asm volatile("nop");
	        }

	    if (timeout == 0U)
	    {
	    	return I2C_ERR_DMA_TIMEOUT;
	    	/* Handle Timeout */
	    }

	    g_I2C_TransferComplete = 0; // Reset for next time
	    return I2C_OK;
}

/****************************
 * @brief
 *
 * @param
 * @retval
 */
i2c_status_t STMPE811_Write_Reg_Safe(const uint8_t reg, const uint8_t value)
{
    // 1. Start Condition
    I2C3->CR1 |= I2C_CR1_START;
    uint32_t timeout = 10000;
    while (!(I2C3->SR1 & I2C_SR1_SB) && --timeout);
    if (timeout == 0U) return I2C_ERR_TIMEOUT;

	// 2. Device Address (Write)
	I2C3->DR = (0x41 << 1); // 7-bit 0x41 shifted
	const i2c_status_t st = I2C_Wait_Addr_With_Timeout();
	if (st != I2C_OK) return st;
	(void)I2C3->SR2; // Clear ADDR flag

    // 3. Send Register Address
    I2C3->DR = reg;
    timeout = 10000;
    while (!(I2C3->SR1 & I2C_SR1_TXE) && --timeout);
    if (I2C3->SR1 & I2C_SR1_AF) goto nack_recovery;

    // 4. Send Data Value
    I2C3->DR = value;
    timeout = 10000;
    while (!(I2C3->SR1 & I2C_SR1_BTF) && --timeout);
	if (timeout == 0U) goto timeout_recovery;
    if (I2C3->SR1 & I2C_SR1_AF) goto nack_recovery;

    // 5. Stop Condition
    I2C3->CR1 |= I2C_CR1_STOP;
    return I2C_OK;

nack_recovery:
    I2C3->SR1 &= ~I2C_SR1_AF;
    I2C3->CR1 |= I2C_CR1_STOP;
    return I2C_ERR_NACK;
timeout_recovery:
    I2C3->CR1 |= I2C_CR1_STOP;
    return I2C_ERR_TIMEOUT;
}

uint8_t STMPE811_Read_Reg_Simple(const uint8_t reg)
{
	// 1. Start & Address (Write mode to send reg address)
	I2C3->CR1 |= I2C_CR1_START;
	while (!(I2C3->SR1 & I2C_SR1_SB));
	I2C3->DR = (0x41 << 1);
	while (!(I2C3->SR1 & I2C_SR1_ADDR));
	(void)I2C3->SR2; // Clear ADDR

    I2C3->DR = reg;
    while (!(I2C3->SR1 & I2C_SR1_BTF));

    // 2. Repeated Start & Address (Read mode)
    I2C3->CR1 |= I2C_CR1_START;
    while (!(I2C3->SR1 & I2C_SR1_SB));
    I2C3->DR = (0x41 << 1) | 0x01;
    while (!(I2C3->SR1 & I2C_SR1_ADDR));

    // 3. Prepare NACK and STOP before reading the byte
    I2C3->CR1 &= ~I2C_CR1_ACK;
    (void)I2C3->SR2; // Clear ADDR starts the clock
    I2C3->CR1 |= I2C_CR1_STOP;

    // 4. Wait for and return the data
    while (!(I2C3->SR1 & I2C_SR1_RXNE));
    return I2C3->DR;
}


/****************************
 * @brief
 *
 * @param
 * @retval
 */
void ReleaseSerialBus(void) {
    stats_I2C_Recoveries++; // Increment our "Glitch" counter

    //uint32_t timeout = 10000;
    I2C3->CR1 &= ~I2C_CR1_PE; // Disable peripheral

    // 1. Set SCL (A8) and SDA (C9) to Open Drain Output
    GPIOA->MODER = (GPIOA->MODER & ~GPIO_MODER_MODER8) | GPIO_MODER_MODER8_0;
    GPIOA->OTYPER |= GPIO_OTYPER_OT8;

    GPIOC->MODER = (GPIOC->MODER & ~GPIO_MODER_MODER9) | GPIO_MODER_MODER9_0;
    GPIOC->OTYPER |= GPIO_OTYPER_OT9;

    // 2. The "Clock Out" sequence
    // If SDA is low, toggle SCL until the slave releases it
    int pulses = 0;
    while (!(GPIOC->IDR & (1 << 9)) && pulses < 20) {
        GPIOA->ODR &= ~(1 << 8); // SCL Low
        for(volatile int i=0; i<500; i++); // Wait for rise/fall
        GPIOA->ODR |= (1 << 8);  // SCL High
        for(volatile int i=0; i<500; i++);
        pulses++;
    }

    // 3. Generate a manual STOP condition (SDA goes high while SCL is high)
    GPIOA->ODR &= ~(1 << 8); // SCL Low
    GPIOC->ODR &= ~(1 << 9); // SDA Low
    for(volatile int i=0; i<500; i++);
    GPIOA->ODR |= (1 << 8);  // SCL High
    for(volatile int i=0; i<500; i++);
    GPIOC->ODR |= (1 << 9);  // SDA High (STOP!)

    // 4. Re-configure AF correctly
    // Clear the 4-bit fields first!
    GPIOA->AFR[1] &= ~(0xF << (0 * 4)); // Clear AFH bit field for Pin 8
    GPIOA->AFR[1] |=  (4 << (0 * 4));   // Set to AF4 (I2C3)

    GPIOC->AFR[1] &= ~(0xF << (1 * 4)); // Clear AFH bit field for Pin 9
    GPIOC->AFR[1] |=  (4 << (1 * 4));   // Set to AF4 (I2C3)

    // Set pins back to Alternate Function mode
    GPIOA->MODER = (GPIOA->MODER & ~GPIO_MODER_MODER8) | GPIO_MODER_MODER8_1;
    GPIOC->MODER = (GPIOC->MODER & ~GPIO_MODER_MODER9) | GPIO_MODER_MODER9_1;

    // 5. Hardware Reset I2C
    I2C3->CR1 |= I2C_CR1_SWRST;
    for(volatile int i=0; i<100; i++);
    I2C3->CR1 &= ~I2C_CR1_SWRST;

    I2C3->CR1 |= I2C_CR1_PE; // Enable I2C3
}
/****************************
 * @brief
 *
 * @param
 * @retval
 */
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
	// select the sample time bit number adc ref
	I2C_Write_1Byte(DeviceAddr, STMPE811_REG_ADC_CTRL1, REG_ADC_CTRL1_12_BIT_ADC | REG_ADC_CTRL1_SAMPLE_TIME_80CLK);
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

	I2C_Write_1Byte(DeviceAddr, STMPE811_REG_INT_EN, 1<<0); //touch detected interrupt
	I2C_Write_1Byte(DeviceAddr, STMPE811_REG_INT_EN, 1<<1); //fifo threshold detetcted
	I2C_Write_1Byte(DeviceAddr, STMPE811_REG_INT_CTRL, 1<<0); //global interrupt active
	I2C_Write_1Byte(DeviceAddr, STMPE811_REG_INT_CTRL, 1<<1); //interrupt type edge


	//tsc op mode z only. bits 3:1 to 0b100
	//I2C_Write_1Byte(DeviceAddr, STMPE811_REG_TSC_CTRL, REG_TSC_CTRL_OP_MODE_Z_ONLY);
	//ENABLE TSC
	I2C_Write_1Byte(DeviceAddr, STMPE811_REG_TSC_CTRL, 0x01);
	//CLEAR ALL STATUS PENDING BITS IF ANY
	I2C_Write_1Byte(DeviceAddr, STMPE811_REG_INT_STA, 0xFF);
	//DELAY
	delay_ms(2);
}

/****************************
 * @brief
 *
 * @param
 * @retval
 */
void STMPE811_Init_Interrupts(void)
{
    // 1. Reset the sensor
    STMPE811_Write_Reg_Safe(0x03, 0x01); // SYS_CTRL1: Soft Reset
    for(volatile int i=0; i<100000; i++); // Wait for reset
    STMPE811_Write_Reg_Safe(0x03, 0x00); // Release Reset

    // 2. Enable TSC and ADC
    STMPE811_Write_Reg_Safe(0x04, 0x00); // SYS_CTRL2: All blocks ON

    // 3. Configure Interrupts
    // Register 0x09: INT_CTRL
    // Bit 2: Polarity (0=Active Low), Bit 1: Type (0=Level), Bit 0: Global Enable (1)
    STMPE811_Write_Reg_Safe(0x09, 0x01);

    // Register 0x08: INT_EN
    // Bit 0: Touch Detect Enable
    STMPE811_Write_Reg_Safe(0x08, 0x01);

    // 4. Configure Touchscreen Controller
    STMPE811_Write_Reg_Safe(0x41, 0x01); // TSC_CTRL: Enable TSC

    // 5. Clear any pending interrupts on the sensor
    STMPE811_Write_Reg_Safe(0x0B, 0xFF); // INT_STA: Write 1 to clear all
}

/****************************
 * @brief
 *
 * @param
 * @retval
 */
void Touch_Interrupt_Init(void) {
    // 1. Enable Clock for GPIOA and SYSCFG
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    // 2. PA15 is JTAG JTDI by default. We must set it to Input mode.
    // This effectively "reclaims" the pin from the debugger.
    GPIOA->MODER &= ~(3 << (15 * 2)); // 00 = Input Mode

    // 2.5. PA15: bits 31:30 should be 01 (Pull-up)
    GPIOA->PUPDR &= ~(3 << 30);
    GPIOA->PUPDR |=  (1 << 30);
    // 3. Configure the EXTI line for PA15
    // EXTI15 is in External Interrupt Configuration Register 4 (EXTICR[3])
    SYSCFG->EXTICR[3] &= ~SYSCFG_EXTICR4_EXTI15;     // Clear bits
    SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI15_PA;  // Map PA15 to EXTI15

    // Force Falling Edge only
    EXTI->FTSR |= EXTI_FTSR_TR15;
    EXTI->RTSR &= ~EXTI_RTSR_TR15;

    // 4. Configure EXTI Line 15
    EXTI->IMR  |= EXTI_IMR_MR15;   // Unmask Line 15
    EXTI->FTSR |= EXTI_FTSR_TR15;  // Trigger on Falling Edge

    // 5. Enable the shared EXTI15_10 Interrupt in NVIC
    NVIC_SetPriority(EXTI15_10_IRQn, 2);
    NVIC_EnableIRQ(EXTI15_10_IRQn);
}


/****************************
 * @brief
 *
 * @param
 * @retval
 */
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
/****************************
 * @brief
 *
 * @param
 * @retval
 */
void Touch_Init(uint8_t DeviceAddr)
{
	I2C_Write_1Byte(DeviceAddr, STMPE811_REG_SYS_CTRL1, REG_SYS_CTRL1_SOFT_RESET_OFF);
	delay_ms(2);
	I2C_Write_1Byte(DeviceAddr, STMPE811_REG_SYS_CTRL1, REG_SYS_CTRL1_SOFT_RESET_ON);
	delay_ms(2);
	stmpe811_TS_Start(DeviceAddr);
}

/****************************
 * @brief
 *
 * @param
 * @retval
 */
uint8_t stmpe811_TS_DetectTouch(uint8_t DeviceAddr)
{
	uint8_t uc_Touched = 0;
	//read touch status, check status of z bit
	const uint8_t uc_State = ((I2C_Read_1Byte(DeviceAddr, STMPE811_REG_TSC_CTRL) & REG_TSC__TS_CTRL_STATUS) ==
	                    REG_TSC__TS_CTRL_STATUS);
	if (uc_State > 0)
	{
		if(I2C_Read_1Byte(DeviceAddr, STMPE811_REG_FIFO_SIZE) > 0)
		{
			//TOUCH DETECTED
			uc_Touched = 1;
		}
	}
	else
	{
		//no touch detected
	}
	return uc_Touched;

}

/****************************
 * @brief
 *
 * @param
 * @retval
 */
void Touch_Process (void)
{
	if (uc_TouchDetected || (!(GPIOA->IDR & (1 << 15))))
	{
		if (!(GPIOA->IDR & (1 << 15)) && !uc_TouchDetected)
		{
			stats_StuckPinKicks++; // We caught a "missed" interrupt
		}

		uc_TouchDetected = 0;
		stats_TotalTouches++;


		i2c_status_t st = stmpe811_TS_GetXY(&us_TouchPointX, &us_TouchPointY);
		if (st != I2C_OK)
			{
			stats_DMA_Timeouts++;
			ReleaseSerialBus();
			my_I2C3_Init();
			// 3. Clear the sensor's internal "sticky" interrupt
			STMPE811_Write_Reg_Safe(0x0B, 0xFF);
		}
		else
			{
			// Do the “sensor cleanup” here (main context), not in the DMA ISR
			STMPE811_Write_Reg_Safe(0x0B, 0xFF); // clear interrupt status
			STMPE811_Write_Reg_Safe(0x4B, 0x01); // FIFO reset
			STMPE811_Write_Reg_Safe(0x4B, 0x00); // FIFO normal mode

			// Clear EXTI pending just in case an edge happened during I2C activity
			EXTI->PR = EXTI_PR_PR15;
		}
	}
}


/****************************
 * @brief  |
 *
 * @param
 * @retval
 */
i2c_status_t stmpe811_TS_GetXY(uint16_t *X, uint16_t *Y)
{
	const uint8_t dataXYZ[4] = {0};
	i2c_status_t status = I2C_OK;

	status = I2C_Read_Via_DMA(STMPE811_DEVICE_ADDRESS, STMPE811_REG_TSC_DATA_NON_INC, dataXYZ, sizeof(dataXYZ));
	if (status != I2C_OK)
		{
		// Ensure the bus is released on error paths too
		I2C3->CR1 |= I2C_CR1_STOP;
		return status;
	}
	//calc pos and values
	uint32_t ul_dataXYZ = (dataXYZ[0] << 24 | dataXYZ[1] << 16 | dataXYZ[2] << 8 | dataXYZ[3] << 0);
	*X = (ul_dataXYZ >> 20) & 0x00000FFF;
	*Y = (ul_dataXYZ >> 8) & 0x00000FFF;
	return status;
}


/****************************
 * @brief
 *
 * @param
 * @retval
 */
// Helper to check for errors during address phases
static i2c_status_t I2C_Wait_Addr_With_Timeout(void) {
    uint32_t timeout = 100000;
    if (I2C3->SR1 & I2C_SR1_AF) I2C3->SR1 &= ~I2C_SR1_AF; //if af was set in previous failed transfer, then clear it here
    while (!(I2C3->SR1 & I2C_SR1_ADDR)) {
        // If the Slave NACKs, the AF bit sets
        if (I2C3->SR1 & I2C_SR1_AF) {
            I2C3->SR1 &= ~I2C_SR1_AF; // Clear the error flag
            I2C3->CR1 |= I2C_CR1_STOP; // Release the bus
            return I2C_ERR_NACK; // Error: No Acknowledge
        }
        if (--timeout == 0) return I2C_ERR_TIMEOUT; // Error: Timeout
    }
    return I2C_OK; // Success
}



/****************************
 * @brief
 *
 * @param
 * @retval
 */
void EXTI15_10_IRQHandler(void) {
    // Check if the interrupt came from Line 15
    if (EXTI->PR & EXTI_PR_PR15) {

        // 1. Clear the EXTI pending bit (Write 1 to clear)
        EXTI->PR = EXTI_PR_PR15;
        uc_TouchDetected = 1;
    }
    else
    {
    	//uc_TouchDetected = 0;
    }
}

void STMPE811_Emergency_Clear(void) {
    // Manually write to the interrupt status register to force release of PA15
    // Using your "Safe Write" function from earlier
    STMPE811_Write_Reg_Safe(0x0B, 0xFF);

    // Also, clear the STM32's EXTI pending bit one more time
    // to catch any "ghost" edges caused by the reset
    EXTI->PR = EXTI_PR_PR15;
}

/**
  * @brief Initialize SysTick Timer for 1ms interrupts
  * @param None
  * @retval None
  */
static void my_SysTick_Init(void)
{
    // Calculate the reload value for 1ms tick at 180 MHz
    // SysTick frequency = HCLK / 8 (or HCLK if configured otherwise)
    // For 1ms: Reload = (180,000,000 / 8) / 1000 = 22,500
    uint32_t reload_value = (180000000UL / 8UL) / 1000UL;  // 1ms tick
    
    // 1. Disable SysTick first
    SysTick->CTRL = 0;
    
    // 2. Set reload register
    SysTick->LOAD = reload_value - 1;  // LOAD is 0-indexed
    
    // 3. Clear current value
    SysTick->VAL = 0;
    
    // 4. Configure and enable SysTick
    // bit 2 = Clock Source (1 = AHB/8, 0 = AHB) 
    // bit 1 = Interrupt enable
    // bit 0 = Enable counter
    SysTick->CTRL = (1 << 2) | (1 << 1) | (1 << 0);  // Use AHB/8 divider, enable interrupt, enable counter
}

static void delay_ms(uint32_t ms)
{
	uint32_t start = ul_g_ms_ticks;
	while ((uint32_t)(ul_g_ms_ticks - start) < ms)
	{
		__asm volatile ("nop");
	}
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
  //__disable_irq();
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
