#include "stm32f10x.h"                  // Device header
#include "IIC.h"
#include "OLED.h"
#include "OLED_Font.h"
#include "Delay.h"
#include "my_SPI.h"

#define CHOOSE 0	//1.0控制OLED屏幕的两种显示方式，函数名一样

#if CHOOSE==1

void OLED_Write_CMD(uint8_t Byte)
{
	WriteReg(OLED_ADRESS, 0x00, Byte);
}

void OLED_Write_Data(uint8_t Byte)
{
	WriteReg(OLED_ADRESS, 0x40, Byte);
}

#else

void OLED_Write_CMD(uint8_t Byte)
{
    OLED_W_CS(0);
    OLED_W_DC(0);
    OLED_SPI_SendByte(Byte);
    OLED_W_CS(1);
}

void OLED_Write_Data(uint8_t Byte)
{
    OLED_W_CS(0);
    OLED_W_DC(1);
    OLED_SPI_SendByte(Byte);
    OLED_W_CS(1);
}

#endif

/**
  * @brief  OLED初始化，使用PB10,PB11进行硬件IIC通讯
  * @param  无
  * @retval 无
  */
void OLED_Init(void)
{
	IIC_Init(GPIO_Pin_10, GPIO_Pin_11);
	OLED_SPI_Init();
	
	Delay_ms(100);
	OLED_Write_CMD(0xAE);	//关闭显示
	OLED_Write_CMD(0xD5);	//设置显示时钟分频比/振荡器频率
	OLED_Write_CMD(0x80);
	OLED_Write_CMD(0xA8);	//设置多路复用率
	OLED_Write_CMD(0x3F);
	OLED_Write_CMD(0xD3);	//设置显示偏移
	OLED_Write_CMD(0x00);
	OLED_Write_CMD(0x40);	//设置显示开始行
	OLED_Write_CMD(0xA1);	//设置左右方向，0xA1正常 0xA0左右反置
	OLED_Write_CMD(0xC8);	//设置上下方向，0xC8正常 0xC0上下反置
	OLED_Write_CMD(0xDA);	//设置COM引脚硬件配置
	OLED_Write_CMD(0x12);
	OLED_Write_CMD(0x81);	//设置对比度控制
	OLED_Write_CMD(0xCF);
	OLED_Write_CMD(0xD9);	//设置预充电周期
	OLED_Write_CMD(0xF1);
	OLED_Write_CMD(0xDB);	//设置VCOMH取消选择级别
	OLED_Write_CMD(0x30);
	OLED_Write_CMD(0xA4);	//设置整个显示打开/关闭
	OLED_Write_CMD(0xA6);	//设置正常/倒转显示
	OLED_Write_CMD(0x8D);	//设置充电泵
	OLED_Write_CMD(0x14);
	OLED_Write_CMD(0xAF);	//开启显示
}

/**
  * @brief  开关电荷泵等
  * @param  无
  * @retval 无
  */
void OLED_ON(void)
{
	OLED_Write_CMD(0x8D);
	OLED_Write_CMD(0x14);
	OLED_Write_CMD(0xAF);
}
void OLED_OFF(void)
{
	OLED_Write_CMD(0x8D);
	OLED_Write_CMD(0x10);
	OLED_Write_CMD(0xAE);
}

/**
  * @brief  设置光标位置
  * @param  x为第几个像素格，y为行数
  * @retval 无
  */
void OLED_SetPos(uint8_t y,uint8_t x)
{
	OLED_Write_CMD(0xB0 | y);					//设置Y位置
	OLED_Write_CMD(0x10 | ((x & 0xF0) >> 4));	//设置X位置高4位
	OLED_Write_CMD(0x00 | (x & 0x0F));			//设置X位置低4位
}

/**
  * @brief  字节填充函数 在OLED屏启动后 先填充一下 防止有脏数据
  * @param  Fill_Data填充数据
  * @retval 无
  */
void OLED_Fill(uint8_t Fill_Data)
{
	uint8_t i, j;
	for (j = 0; j < 8; j++)
	{
		OLED_SetPos(j, 0);
		for(i = 0; i < 128; i++)
		{
			OLED_Write_Data(Fill_Data);
		}
	}
}

/**
  * @brief  清屏
  * @param  无
  * @retval 无
  */
void OLED_Clear(void)
{
	OLED_Fill(0x00);
}

/**
  * @brief  OLED显示一个字符
  * @param  Line 行位置，范围：1~4
  * @param  Column 列位置，范围：1~16
  * @param  Char 要显示的一个字符，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{      	
	uint8_t i;
	OLED_SetPos((Line - 1) * 2, (Column - 1) * 8);		//设置光标位置在上半部分
	for (i = 0; i < 8; i++)
	{
		OLED_Write_Data(F8x16[Char - ' '][i]);			//显示上半部分内容
	}
	OLED_SetPos((Line - 1) * 2 + 1, (Column - 1) * 8);	//设置光标位置在下半部分
	for (i = 0; i < 8; i++)
	{
		OLED_Write_Data(F8x16[Char - ' '][i + 8]);		//显示下半部分内容
	}
}

/**
  * @brief  OLED显示一个字符
  * @param  Line 行位置，范围：1~8
  * @param  Column 列位置，范围：1~127
  * @param  Char 要显示的一个字符，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowChar_F6x8(uint8_t Line, uint8_t Column, char Char)
{      	
	uint8_t i;
	OLED_SetPos(Line - 1, Column - 1);
	for (i = 0; i < 6; i++)
	{
		OLED_Write_Data(F6x8[Char - ' '][i]);
	}
}

/**
  * @brief  OLED反色显示一个字符
  * @param  Line 行位置，范围：1~4
  * @param  Column 列位置，范围：1~16
  * @param  Char 要显示的一个字符，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowChar2(uint8_t Line, uint8_t Column, char Char)
{      	
	uint8_t i;
	OLED_SetPos((Line - 1) * 2, (Column - 1) * 8);		//设置光标位置在上半部分
	for (i = 0; i < 8; i++)
	{
		OLED_Write_Data(~F8x16[Char - ' '][i]);			//显示上半部分内容
	}
	OLED_SetPos((Line - 1) * 2 + 1, (Column - 1) * 8);	//设置光标位置在下半部分
	for (i = 0; i < 8; i++)
	{
		OLED_Write_Data(~F8x16[Char - ' '][i + 8]);		//显示下半部分内容
	}
}

/**
  * @brief  OLED显示字符串
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  String 要显示的字符串，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowString(uint8_t Line, uint8_t Column, char* String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)
	{
		OLED_ShowChar(Line, Column + i, String[i]);
	}
}

/**
  * @brief  OLED反色显示字符串
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  String 要显示的字符串，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowString2(uint8_t Line, uint8_t Column, char* String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)
	{
		OLED_ShowChar2(Line, Column + i, String[i]);
	}
}

/**
  * @brief  OLED显示字符串
  * @param  Line 起始行位置，范围：1~8
  * @param  Column 起始列位置，范围：1~127
  * @param  String 要显示的字符串，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowString_F6x8(uint8_t Line, uint8_t Column, char* String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)
	{
		OLED_ShowChar_F6x8(Line, Column + i*6, String[i]);
	}
}


/**
  * @brief  OLED次方函数
  * @param  无
  * @retval 返回值等于X的Y次方
  */
uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y--)
	{
		Result *= X;
	}
	return Result;
}

/**
  * @brief  OLED显示数字（十进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~4294967295
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
  * @brief  OLED显示数字（十进制，正数）
  * @param  Line 起始行位置，范围：1~8
  * @param  Column 起始列位置，范围：1~127
  * @param  Number 要显示的数字，范围：0~4294967295
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void OLED_ShowNum_F6x8(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar_F6x8(Line, Column + i*6, Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

void OLED_ShowSignedNum_F6x8(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length)
{
    uint8_t i;
    uint32_t Number1;
    if (Number < 0)
    {
        OLED_ShowChar_F6x8(Line, Column, '-');
        Number1 = -Number;
    }
    else
    {
        OLED_ShowChar_F6x8(Line, Column, ' ');
        Number1 = Number;
    }
    for (i = 0; i < Length; i++)
    {
        OLED_ShowChar_F6x8(Line, Column + i*6+6, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0');
    }
}

/**
  * @brief  OLED显示数字（十进制，带符号数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：-2147483648~2147483647
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length)
{
	uint8_t i;
	uint32_t Number1;
	if (Number >= 0)
	{
		OLED_ShowChar(Line, Column, '+');
		Number1 = Number;
	}
	else
	{
		OLED_ShowChar(Line, Column, '-');
		Number1 = -Number;
	}
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i + 1, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
  * @brief  OLED显示数字（十六进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~0xFFFFFFFF
  * @param  Length 要显示数字的长度，范围：1~8
  * @retval 无
  */
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i, SingleNumber;
	for (i = 0; i < Length; i++)							
	{
		SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
		if (SingleNumber < 10)
		{
			OLED_ShowChar(Line, Column + i, SingleNumber + '0');
		}
		else
		{
			OLED_ShowChar(Line, Column + i, SingleNumber - 10 + 'A');
		}
	}
}

/**
  * @brief  OLED显示数字（二进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~1111 1111 1111 1111
  * @param  Length 要显示数字的长度，范围：1~16
  * @retval 无
  */
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(2, Length - i - 1) % 2 + '0');
	}
}

/**
  *@brief OLED显示汉字
  *@param Line 起始行位置，范围：1~4
  *@param Column 起始列位置，范围：1~8
  *@param Number 要显示汉字的序号
  *@retval 无
  */
void OLED_ShowChinese(uint8_t Line,uint8_t Column,uint8_t num)
{
    uint8_t i;
    OLED_SetPos((Line - 1) * 2, (Column - 1) * 16);
    for(i = 0;i<16;i++)
    {
        OLED_Write_Data(Font_Chinese[2*num][i]);
    }
    OLED_SetPos((Line - 1) * 2 + 1, (Column - 1) * 16);
    for(i = 0;i<16;i++)
     {
         OLED_Write_Data(Font_Chinese[2*num+1][i]);
     }
}

void OLED_ShowChinese2(uint8_t Line,uint8_t Column,uint8_t num)
{
    uint8_t i;
    OLED_SetPos((Line - 1) * 2, (Column - 1) * 16);
    for(i = 0;i<16;i++)
    {
        OLED_Write_Data(~Font_Chinese[2*num][i]);
    }
    OLED_SetPos((Line - 1) * 2 + 1, (Column - 1) * 16);
    for(i = 0;i<16;i++)
     {
         OLED_Write_Data(~Font_Chinese[2*num+1][i]);
     }
}

