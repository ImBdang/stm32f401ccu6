/**
 * @file    flash_test.c
 * @brief   Basic program/erase flash exercise for STM32F4xx.
 *
 * @note    Read/program/erase string "Bdang" to sector 5
 */
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_flash.h"

/* Macro zone --------------------------------------------------- */
#define SECTOR_5_ADDR 0x08020000

/* Declaration zone --------------------------------------------- */
void bdang_init_clock(void);
void bdang_init_flash(void);
uint8_t bdang_erase_sector_5(void);
void bdang_program_sector_5(char* str);

/* Main func ---------------------------------------------------- */
void main(void){
  bdang_init_flash();
  bdang_init_clock();
  char* str = "Bdang";

  uint8_t tmp = bdang_erase_sector_5();
  if (tmp){
    bdang_program_sector_5(str);
  }
  
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
  FLASH_SetLatency(FLASH_ACR_LATENCY_2WS);
  FLASH_PrefetchBufferCmd(ENABLE);
  FLASH_DataCacheCmd(ENABLE);
  FLASH_InstructionCacheCmd(ENABLE);
  FLASH_ClearFlag(
    FLASH_FLAG_EOP    |                      // FLASH End of Operation flag      
    FLASH_FLAG_OPERR  |                      // FLASH operation Error flag
    FLASH_FLAG_WRPERR |                      // FLASH Write protected error flag
    FLASH_FLAG_PGAERR |                      // FLASH Programming Alignment error flag
    FLASH_FLAG_PGPERR |                      // FLASH Programming Parallelism error flag
    FLASH_FLAG_PGSERR                        // FLASH Programming Sequence error flag
  );
}

/**
 * @brief Delete sector 5 in flash
 * 
 * @note Use func of STL to coded this func
 * 
 * @retval uint8_t variable,  0 if the status is FALSE, 1 if the status is true
 */
uint8_t bdang_erase_sector_5(void){
  FLASH_Status status;
  FLASH_Unlock();
  status = FLASH_EraseSector(FLASH_Sector_5, VoltageRange_3);
  FLASH_Lock();
  if (status == FLASH_COMPLETE)
    return 1;
  return 0;
}

/**
 * @brief Write a string to sector 5
 * 
 * @retval None
 */
void bdang_program_sector_5(char* str){
  FLASH_Unlock();
  for (uint8_t i = 0; str[i] != '\0'; i++){
    FLASH_ProgramByte(SECTOR_5_ADDR + i, str[i]);
  }
  FLASH_Lock();
}