#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "my_SPI.h"

/*引脚初始化*/
void OLED_SPI_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStructure;
     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
     GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
     GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
     GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
     GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
     GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    OLED_W_D0(1);
    OLED_W_D1(1);
    OLED_W_RES(1);
    OLED_W_DC(1);
    OLED_W_CS(1);
}
 

/**
  * @brief  SPI发送一个字节
  * @param  Byte 要发送的一个字节
  * @retval 无
  */
void OLED_SPI_SendByte(uint8_t Byte)
{
    uint8_t i;
    for (i = 0; i < 8; i++)
    {
        OLED_W_D0(0);
        OLED_W_D1(Byte & (0x80 >> i));
        OLED_W_D0(1);
    }
}
 

