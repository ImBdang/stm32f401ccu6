/**
 * @file    usart2.c
 * @brief   Communication with stm32f401ccu6 by usart2
 *
 * @note    None
 */
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_flash.h"
#include "core_cm4.h"

/* Global variable ---------------------------------------------- */
uint32_t msTicks = 0;

/* Macro zone --------------------------------------------------- */


/* Declaration zone --------------------------------------------- */
void bdang_init_clock(void);
void bdang_init_gpio(void);
void bdang_init_usart2(void);
void delay_ms(uint32_t ms);
void bdang_init_systick(uint32_t ticks);
void bdang_usart_sendstring(USART_TypeDef *USARTx, const char *str);
uint8_t bdang_usart_receive(USART_TypeDef* USARTx);

/* Main func ---------------------------------------------------- */
void main(void){
  bdang_init_clock();      
  bdang_init_gpio();       
  bdang_init_usart2(); 
  SystemCoreClockUpdate();
  bdang_init_systick(1000);

  while (1){
    uint8_t tmp;
    bdang_usart_sendstring(USART2, "bdang");
    tmp = bdang_usart_receive(USART2);
    delay_ms(1000);
  }
}

/* Definetion zone ---------------------------------------------- */
/**
 * @brief Send each char in a string
 * 
 * @retval None
 */
void bdang_usart_sendstring(USART_TypeDef *USARTx, const char *str) {
    while (*str) {
        USART_SendData(USARTx, *str++);
        while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
    }
}

/**
 * @brief Receive usart 
 * 
 * @retval None 
 */
uint8_t bdang_usart_receive(USART_TypeDef* USARTx) {
    while (USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == RESET);
    return (uint8_t)USART_ReceiveData(USARTx);
}

/**
 * @brief Use HSI as the PLLSRC, PLLSRC is used for SYSCLK with 64Mhz
 * 
 * @note AHB, SYSCLK, PCLK2 = 64Mhz
 *       PCLK1 = 32Mhz
 * 
 * @retval None 
 */
void bdang_init_clock(void){
  RCC_HSICmd(ENABLE);
  while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);
  RCC_PLLConfig(
    RCC_PLLSource_HSI,
    8,                            // M: 16/8 = 2MHz
    192,                          // N: 2×192 = 384MHz 
    6,                            // P: 384/6 = 64MHz 
    8                             // Q: 384/8 = 48MHz 
  );
  RCC_PLLCmd(ENABLE);
  while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET){}; 
  FLASH_SetLatency(FLASH_ACR_LATENCY_2WS);
  RCC_HCLKConfig(RCC_SYSCLK_Div1);
  RCC_PCLK1Config(RCC_HCLK_Div2);
  RCC_PCLK2Config(RCC_HCLK_Div1);
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
}


/**
 * @brief Init GPIOA
 * 
 * @retval None
 */
void bdang_init_gpio(void){
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  GPIO_InitTypeDef gpioa_config = {
    .GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3,
    .GPIO_Mode = GPIO_Mode_AF,
    .GPIO_Speed = GPIO_Speed_25MHz, 
    .GPIO_OType = GPIO_OType_PP,
    .GPIO_PuPd = GPIO_PuPd_NOPULL
  };
  GPIO_Init(GPIOA, &gpioa_config);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);             // TX
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);             // RX
}

/**
 * @brief Init USART2
 *
 * @note USART2 use GPIOA, PIN 2, PIN3
 *  
 * @retval None
 */

void bdang_init_usart2(void){
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  USART_InitTypeDef usart2_config = {
    .USART_BaudRate = 115200,
    .USART_WordLength = USART_WordLength_8b, 
    .USART_StopBits = USART_StopBits_1,
    .USART_Parity = USART_Parity_No,
    .USART_Mode = USART_Mode_Rx | USART_Mode_Tx,
    .USART_HardwareFlowControl = USART_HardwareFlowControl_None
  };
  USART_Init(USART2, &usart2_config);
  USART_Cmd(USART2, ENABLE);
}

/**
 * @brief Handler systick 
 * 
 * @retval None
 */
void SysTick_Handler(void){
    msTicks++;
}

/**
 * @brief Basic delay by systick
 * 
 * @retval None
 */
void delay_ms(uint32_t ms) {
    uint32_t start = msTicks;
    while ((msTicks - start) < ms) {
    }
}

/**
 * @brief Config systick
 * 
 * @retval None
 */
void bdang_init_systick(uint32_t ticks){
  if (SysTick_Config(SystemCoreClock / ticks) != 0) {
    while(1); 
  }
}