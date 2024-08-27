#ifndef __FLASH_H__
#define __FLASH_H__

#include "main.h"

/*STM32F405RGT6 FLASH 扇区的起始地址*/
#define ADDR_FLASH_SECTOR_0     ((uint32_t )0x08000000)     /* 扇区0起始地址, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t )0x08004000)     /* 扇区1起始地址, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t )0x08008000)     /* 扇区2起始地址, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t )0x0800C000)     /* 扇区3起始地址, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t )0x08010000)     /* 扇区4起始地址, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t )0x08020000)     /* 扇区5起始地址, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t )0x08040000)     /* 扇区6起始地址, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t )0x08060000)     /* 扇区7起始地址, 128 Kbytes */
#define ADDR_FLASH_SECTOR_8     ((uint32_t )0x08080000)     /* 扇区8起始地址, 128 Kbytes */
#define ADDR_FLASH_SECTOR_9     ((uint32_t )0x080A0000)     /* 扇区9起始地址, 128 Kbytes */
#define ADDR_FLASH_SECTOR_10    ((uint32_t )0x080C0000)     /* 扇区10起始地址,128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t )0x080E0000)     /* 扇区11起始地址,128 Kbytes */

#include "common_inc.h"

void Flash_Save(void);
void Flash_Read(void);
void Param_FlashSave(void);
void Param_FlashRead(void);

void Flash_Anticogging_Clear(void);
void Flash_Anticog_Save(uint16_t index,float *Iph);
float Flash_GetAnticogCurrent(uint16_t index,uint8_t Ix);
void Flash_CAN_ID_Save(void);
void Flash_CAN_ID_Read(void);
#endif