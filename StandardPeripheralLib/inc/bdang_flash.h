#ifndef BDANG_FLASH
#define BDANG_FLASH

#include <stdint.h>

typedef struct{
    uint32_t ACR;
    uint32_t KEYR;
    uint32_t OPTKEYR;
    uint32_t SR;
    uint32_t CR;
    uint32_t OPTCR; 
} FLash_TypeDef;

#define FLASH_BD ((volatile FLash_TypeDef*)0x40023C00)

#endif