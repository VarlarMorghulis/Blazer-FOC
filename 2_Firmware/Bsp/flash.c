#include "flash.h"

extern InterfaceParam_TypeDef InterfaceParam_t;

uint8_t flashsave_flag;
uint32_t temp_save[];
uint32_t temp_read[];
float* struct_ptr=(float*)(&InterfaceParam_t);

void Param_FlashSave(void)
{
	FLASH_EraseInitTypeDef FLASH_EraseInitstruct;
	uint32_t PageError=0;
	uint32_t flash_addr;
	volatile static uint8_t i=0;
	uint8_t param_num=sizeof(InterfaceParam_TypeDef)/sizeof(float);
	
	for(i=0;i<param_num;i++)
	{
		temp_save[i]=FloatToIntBit(struct_ptr[i]);
	}
	
	FLASH_EraseInitstruct.TypeErase = FLASH_TYPEERASE_SECTORS;//Ö´ÐÐÉÈÇø²Á³ý²Ù×÷
	FLASH_EraseInitstruct.Sector=FLASH_SECTOR_8;
	FLASH_EraseInitstruct.NbSectors=1;               
    FLASH_EraseInitstruct.VoltageRange=FLASH_VOLTAGE_RANGE_3;//²Á³ýµÄµçÑ¹·¶Î§2.7-3.6V  

	/*½âËøFlash*/
	HAL_FLASH_Unlock();
    
	/*²Á³ýÉÈÇø*/
	HAL_FLASHEx_Erase(&FLASH_EraseInitstruct, &PageError);
	
	flash_addr=ADDR_FLASH_SECTOR_8;
	for(i=0;i<param_num;i++)
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,flash_addr,temp_save[i]);	
		flash_addr+=32;
	}
	
	/*FlashÉÏËø*/
	HAL_FLASH_Lock();
}


void Param_FlashRead(void)
{
	uint32_t flash_addr;
	volatile static uint8_t i=0;
	uint8_t param_num=sizeof(InterfaceParam_TypeDef)/sizeof(float);
	
	flash_addr=ADDR_FLASH_SECTOR_8;
	for(i=0;i<param_num;i++)
	{
		temp_read[i]=(*((volatile uint32_t *)flash_addr));
		flash_addr+=32;
	}
	for(i=0;i<param_num;i++)
	{
		struct_ptr[i]=IntBitToFloat(temp_read[i]);
	}
}

void Flash_Anticogging_Clear(void)
{
	FLASH_EraseInitTypeDef FLASH_EraseInitstruct;
	uint32_t PageError=0;
	
	FLASH_EraseInitstruct.TypeErase = FLASH_TYPEERASE_SECTORS;//Ö´ÐÐÉÈÇø²Á³ý²Ù×÷
	FLASH_EraseInitstruct.Sector=FLASH_SECTOR_7;
	FLASH_EraseInitstruct.NbSectors=1;               
    FLASH_EraseInitstruct.VoltageRange=FLASH_VOLTAGE_RANGE_3;//²Á³ýµÄµçÑ¹·¶Î§2.7-3.6V  

	/*½âËøFlash*/
	HAL_FLASH_Unlock();
	/*²Á³ýÉÈÇø*/
	HAL_FLASHEx_Erase(&FLASH_EraseInitstruct, &PageError);
	/*FlashÉÏËø*/
	HAL_FLASH_Lock();
	
	FLASH_EraseInitstruct.Sector=FLASH_SECTOR_8;
	/*½âËøFlash*/
	HAL_FLASH_Unlock();
	/*²Á³ýÉÈÇø*/
	HAL_FLASHEx_Erase(&FLASH_EraseInitstruct, &PageError);
	/*FlashÉÏËø*/
	HAL_FLASH_Lock();
	
	FLASH_EraseInitstruct.Sector=FLASH_SECTOR_9;
	/*½âËøFlash*/
	HAL_FLASH_Unlock();
	/*²Á³ýÉÈÇø*/
	HAL_FLASHEx_Erase(&FLASH_EraseInitstruct, &PageError);
	/*FlashÉÏËø*/
	HAL_FLASH_Lock();
}

void Flash_Anticog_Save(uint16_t index,float *Iph)
{
	uint32_t temp[3];
	uint32_t addr[3];
	  
//	/*½âËøFlash*/
//	HAL_FLASH_Unlock();
	
	addr[0]=ADDR_FLASH_SECTOR_7+index*32;
	addr[1]=ADDR_FLASH_SECTOR_8+index*32;
	addr[2]=ADDR_FLASH_SECTOR_9+index*32;
	
	temp[0]=FloatToIntBit(*(Iph+0));
	temp[1]=FloatToIntBit(*(Iph+1));
	temp[2]=FloatToIntBit(*(Iph+2));
	
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,addr[0],temp[0]);	
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,addr[1],temp[1]);	
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,addr[2],temp[2]);	
//	for(i=0;i<3;i++)
//	{
//		addr=ADDR_FLASH_SECTOR_7+index*3*32+32*i;
//		temp[i]=FloatToIntBit(*(Iph+i));
//		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,addr,temp[i]);	
//	}
	
	//HAL_FLASH_Lock();
}

float Flash_GetAnticogCurrent(uint16_t index,uint8_t Ix)
{
	float Iph=0.0f;
	uint32_t temp=0;
	uint32_t addr;
	
	switch(Ix)
	{
		case 0:
		{
			addr=ADDR_FLASH_SECTOR_7+index*32;
		}
		break;
		
		case 1:
		{
			addr=ADDR_FLASH_SECTOR_8+index*32;
		}
		break;
		
		case 2:
		{
			addr=ADDR_FLASH_SECTOR_9+index*32;
		}	
		break;
		
		default:break;
	}
	
	temp=(*((volatile uint32_t *)addr));
	Iph=IntBitToFloat(temp);
	
	return Iph;
}
