/**
 * @file    flash_test.c
 * @brief   Basic read/program/erase flash exercise for STM32F4xx bare metal.
 *
 * @note    Read/program/erase string "Bdang" to sector 5
 */
#include "stm32f4xx_rcc.h"

/* Macro zone --------------------------------------------------- */
#define SECTOR_5_ADDR       0x08020000

#define KEY1                0x45670123
#define KEY2                0xCDEF89AB

#define FLIFT_BASE_ADDR     0x40023C00
typedef struct{
  uint32_t ACR;
  uint32_t KEYR;
  uint32_t OPTKEYR;
  uint32_t SR;
  uint32_t CR;
  uint32_t OPTCR;
} FLIFT_TypeDef;

#define FLIFT ((volatile FLIFT_TypeDef*)FLIFT_BASE_ADDR)

/* Declaration zone --------------------------------------------- */
void bdang_init_clock(void);
void bdang_init_flash(void);
void bdang_erase_sector_5(void);
void bdang_program_sector_5(char* str);
void bdang_flash_unlock(void);
void bdang_flash_lock(void);
void bdang_write_byte(uint32_t addr, char byte);

/* Main func ---------------------------------------------------- */
void main(void){
  bdang_init_flash();
  bdang_init_clock();
  char* str = "Bdang";
  bdang_flash_unlock();
  bdang_erase_sector_5();
  bdang_flash_lock();

  bdang_flash_unlock();
  FLIFT->CR |= (1 << 0);
  bdang_program_sector_5(str);
  FLIFT->CR &= ~(1 << 0);
  bdang_flash_lock();

  while (1){

  }
}

/* Definetion zone ---------------------------------------------- */
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
  while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET)
  RCC_PLLConfig(
      RCC_PLLSource_HSI,                    // Source: HSI 16MHz
      8,                                    // PLLM: 16/8 = 2MHz (VCO input)
      128,                                  // PLLN: 2×128 = 256MHz (VCO output)
      4,                                    // PLLP: 256/4 = 64MHz (SYSCLK) 
      6                                     // PLLQ: 256/6 = 42.67MHz (USB)
  );
  RCC_PLLCmd(ENABLE);
  while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET){}; 
  RCC_HCLKConfig(RCC_SYSCLK_Div1);
  RCC_PCLK1Config(RCC_HCLK_Div2);
  RCC_PCLK2Config(RCC_HCLK_Div1);
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
}

/**
 * @brief Init Flash, LATENCY 2, clear error flag, enable all cache
 * 
 * @retval None
 */
void bdang_init_flash(void){
  FLIFT->ACR &= ~(0xF << 0);                  // Clear old LATENCY
  FLIFT->ACR |= (2 << 0);                     // LATENCY 2
  FLIFT->ACR &= ~(0x7 << 8);                  // Clear cache config
  FLIFT->ACR |= (1 << 8);                     // Prefetch enable
  FLIFT->ACR |= (1 << 9);                     // Instruction cache enable
  FLIFT->ACR |= (1 << 10);                    // Data cache enable

  FLIFT->CR &= ~(3 << 8);                     // PSize = 00, write each byte

  FLIFT->SR = 0xF3;                           // Clear flags
}

/**
 * @brief Delete sector 5 in flash
 * 
 * @note Use func of STL to coded this func
 * 
 * @retval uint8_t variable,  0 if the status is FALSE, 1 if the status is true
 */
void bdang_erase_sector_5(void){
  while ((FLIFT->SR & (1 << 16)));    // Wait BSY
  FLIFT->CR |= (1 << 1);              // Erase by sector
  FLIFT->CR &= ~(0xF << 3);           // Clear sector 5
  FLIFT->CR |= (5 << 3);              // Set sector 5
  FLIFT->CR |= (1 << 16);             // STRT bit 
  while ((FLIFT->SR & (1 << 16)));    // Wait BSY
  FLIFT->CR &= ~(1 << 1);             // Clear SER
}

/**
 * @brief Write a string to sector 5
 * 
 * @retval None
 */
void bdang_program_sector_5(char* str){
  uint32_t i;
  for (i = 0; str[i] != '\0'; i++){
    bdang_write_byte(SECTOR_5_ADDR + i, str[i]);
  }
  bdang_write_byte(SECTOR_5_ADDR + i, '\0'); 
}

/**
 * @brief Write each byte to addr
 * 
 * @retval None
 */
void bdang_write_byte(uint32_t addr, char byte){
  *(volatile uint8_t*)addr = byte;
  while ((FLIFT->SR & (1 << 16)));              // Wait BSY
}

/**
 * @brief Lock and unlock flash
 * 
 * @retval None
 */
void bdang_flash_unlock(void){
  FLIFT->KEYR = KEY1;
  FLIFT->KEYR = KEY2;
}
void bdang_flash_lock(void){
  FLIFT->CR |= (1 << 31);
}