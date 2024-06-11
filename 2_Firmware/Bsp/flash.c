#include "flash.h"

extern Encoder_TypeDef TLE5012B_t;
extern Encoder_TypeDef ABZ_t;
extern Motor_TypeDef Motor_t;
extern CurrentOffset_TypeDef CurrentOffset_t;
extern ReceiveMsg_TypeDef ReceiveMsg_t;

#define ADDR_FLASH_START			ADDR_FLASH_SECTOR_8

uint32_t FloatToIntBit(float x)
{
	uint32_t *pInt;
	pInt=(uint32_t*)(&x);
	return *pInt;
}

float IntBitToFloat(uint32_t x)
{
	float * pInt;
	pInt=(float *)(&x);
	return *pInt;
}

/**
   * @brief  电机极对数和编码器参数保存 使用扇区9
   * @param  
   * @retval
   */
void Flash_Save(void)
{
	FLASH_EraseInitTypeDef FLASH_EraseInitstruct;
	uint32_t PageError=0;
	uint32_t temp[6];
	uint32_t addr;
	uint8_t i;
	
	FLASH_EraseInitstruct.TypeErase = FLASH_TYPEERASE_SECTORS;//执行扇区擦除操作
	FLASH_EraseInitstruct.Sector=FLASH_SECTOR_9;
	FLASH_EraseInitstruct.NbSectors=1;               
    FLASH_EraseInitstruct.VoltageRange=FLASH_VOLTAGE_RANGE_3;//擦除的电压范围2.7-3.6V  

	/*解锁Flash*/
	HAL_FLASH_Unlock();
    
	/*擦除扇区*/
	HAL_FLASHEx_Erase(&FLASH_EraseInitstruct, &PageError);
	
#ifdef USE_ABZ_ENCODER
	temp[0]=(uint32_t)ABZ_t.sensor_dir;
	temp[1]=(uint32_t)ABZ_t.zero_enc_offset;
	temp[2]=(uint32_t)Motor_t.Pole_Pairs;
	temp[3]=(uint32_t)CurrentOffset_t.A_Offset;
	temp[4]=(uint32_t)CurrentOffset_t.B_Offset;
	temp[5]=(uint32_t)CurrentOffset_t.C_Offset;
#endif

#ifdef USE_SPI_ENCODER
	temp[0]=(uint32_t)TLE5012B_t.sensor_dir;
	temp[1]=(uint32_t)TLE5012B_t.zero_enc_offset;
	temp[2]=(uint32_t)Motor_t.Pole_Pairs;
	temp[3]=(uint32_t)CurrentOffset_t.A_Offset;
	temp[4]=(uint32_t)CurrentOffset_t.B_Offset;
	temp[5]=(uint32_t)CurrentOffset_t.C_Offset;
#endif

	addr=ADDR_FLASH_SECTOR_9;
	for(i=0;i<6;i++)
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,addr,temp[i]);	
		addr+=32;
	}
	
	/*Flash上锁*/
	HAL_FLASH_Lock();
}

void Flash_Read(void)
{
	uint32_t temp[6];
	uint32_t addr;
	uint8_t i;
	
	addr=ADDR_FLASH_SECTOR_9;
	for(i=0;i<6;i++)
	{
		temp[i]=(*((volatile uint32_t *)addr));
		addr+=32;
	}
	
#ifdef USE_ABZ_ENCODER
	ABZ_t.sensor_dir=(int8_t)temp[0];
	ABZ_t.zero_enc_offset=(uint16_t)temp[1];
	ABZ_TIM->CNT=ABZ_t.zero_enc_offset;
	Motor_t.Pole_Pairs=(uint8_t)temp[2];
	CurrentOffset_t.A_Offset=(uint16_t)temp[3];
	CurrentOffset_t.B_Offset=(uint16_t)temp[4];
	CurrentOffset_t.C_Offset=(uint16_t)temp[5];
#endif
	
#ifdef USE_SPI_ENCODER
	TLE5012B_t.sensor_dir=(int8_t)temp[0];
	TLE5012B_t.zero_enc_offset=(uint16_t)temp[1];
	Motor_t.Pole_Pairs=(uint8_t)temp[2];
	CurrentOffset_t.A_Offset=(uint16_t)temp[3];
	CurrentOffset_t.B_Offset=(uint16_t)temp[4];
	CurrentOffset_t.C_Offset=(uint16_t)temp[5];
#endif
}

void Flash_Anticogging_Clear(void)
{
	FLASH_EraseInitTypeDef FLASH_EraseInitstruct;
	uint32_t PageError=0;
	
	FLASH_EraseInitstruct.TypeErase = FLASH_TYPEERASE_SECTORS;//执行扇区擦除操作
	FLASH_EraseInitstruct.Sector=FLASH_SECTOR_7;
	FLASH_EraseInitstruct.NbSectors=1;               
    FLASH_EraseInitstruct.VoltageRange=FLASH_VOLTAGE_RANGE_3;//擦除的电压范围2.7-3.6V  

	/*解锁Flash*/
	HAL_FLASH_Unlock();
	/*擦除扇区*/
	HAL_FLASHEx_Erase(&FLASH_EraseInitstruct, &PageError);
	/*Flash上锁*/
	HAL_FLASH_Lock();
	
	FLASH_EraseInitstruct.Sector=FLASH_SECTOR_8;
	/*解锁Flash*/
	HAL_FLASH_Unlock();
	/*擦除扇区*/
	HAL_FLASHEx_Erase(&FLASH_EraseInitstruct, &PageError);
	/*Flash上锁*/
	HAL_FLASH_Lock();
	
	FLASH_EraseInitstruct.Sector=FLASH_SECTOR_9;
	/*解锁Flash*/
	HAL_FLASH_Unlock();
	/*擦除扇区*/
	HAL_FLASHEx_Erase(&FLASH_EraseInitstruct, &PageError);
	/*Flash上锁*/
	HAL_FLASH_Lock();
}

void Flash_Anticog_Save(uint16_t index,float *Iph)
{
	uint32_t temp[3];
	uint32_t addr[3];
	  
//	/*解锁Flash*/
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

/**
   * @brief  CAN_ID参数保存 使用扇区8
   * @param  无
   * @retval 无
   */
void Flash_CAN_ID_Save(void)
{
	FLASH_EraseInitTypeDef FLASH_EraseInitstruct;
	uint32_t PageError=0;
	uint32_t temp;
	uint32_t addr;
	
	FLASH_EraseInitstruct.TypeErase = FLASH_TYPEERASE_SECTORS;//执行扇区擦除操作
	FLASH_EraseInitstruct.Sector=FLASH_SECTOR_8;
	FLASH_EraseInitstruct.NbSectors=1;               
    FLASH_EraseInitstruct.VoltageRange=FLASH_VOLTAGE_RANGE_3;//擦除的电压范围2.7-3.6V  

	/*解锁Flash*/
	HAL_FLASH_Unlock();
    
	/*擦除扇区*/
	HAL_FLASHEx_Erase(&FLASH_EraseInitstruct, &PageError);
	
	temp=(uint32_t)ReceiveMsg_t.ID;
	
	addr=ADDR_FLASH_SECTOR_8;

	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,addr,temp);	

	/*Flash上锁*/
	HAL_FLASH_Lock();
	
}

void Flash_CAN_ID_Read(void)
{
	uint32_t temp;
	uint32_t addr;
	
	addr=ADDR_FLASH_SECTOR_8;

	temp=(*((volatile uint32_t *)addr));
	
	if(temp>0x08)
		temp=0x01;
	
	ReceiveMsg_t.ID=(uint8_t)temp;
}