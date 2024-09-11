#include "flash.h"

extern InterfaceParam_TypeDef InterfaceParam_t;

uint8_t flashsave_flag;
uint32_t param_num=sizeof(InterfaceParam_TypeDef)/sizeof(float);
uint32_t param_save[64];
uint32_t param_read[64];
float* struct_ptr=(float*)(&InterfaceParam_t);

void Param_FlashSave(void)
{
	FLASH_EraseInitTypeDef FLASH_EraseInitstruct;
	uint32_t PageError=0;
	uint32_t flash_addr;
	volatile uint32_t i=0;
	
	for(i=0;i<param_num;i++)
	{
		param_save[i]=FloatToIntBit(struct_ptr[i]);
	}
	
	FLASH_EraseInitstruct.TypeErase = FLASH_TYPEERASE_SECTORS;//ִ��������������
	FLASH_EraseInitstruct.Sector=FLASH_SECTOR_9;
	FLASH_EraseInitstruct.NbSectors=1;               
    FLASH_EraseInitstruct.VoltageRange=FLASH_VOLTAGE_RANGE_3;//�����ĵ�ѹ��Χ2.7-3.6V  

	/*����Flash*/
	HAL_FLASH_Unlock();
    
	/*��������*/
	HAL_FLASHEx_Erase(&FLASH_EraseInitstruct, &PageError);
	
	flash_addr=ADDR_FLASH_SECTOR_9;
	for(i=0;i<param_num;i++)
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,flash_addr,param_save[i]);	
		flash_addr+=4;
	}
	
	/*Flash����*/
	HAL_FLASH_Lock();
}

void Param_FlashRead(void)
{
	uint32_t flash_addr;
	volatile uint32_t i=0;
	
	flash_addr=ADDR_FLASH_SECTOR_9;
	for(i=0;i<param_num;i++)
	{
		param_read[i]=(*((volatile uint32_t *) flash_addr));
		flash_addr+=4;
	}
	for(i=0;i<param_num;i++)
	{
		struct_ptr[i]=IntBitToFloat(param_read[i]);
	}
}

