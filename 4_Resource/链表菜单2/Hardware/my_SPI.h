#ifndef ___H
#define ___H

/*“˝Ω≈≈‰÷√*/
#define OLED_W_D0(x)        GPIO_WriteBit(GPIOB, GPIO_Pin_12, (BitAction)(x))
#define OLED_W_D1(x)        GPIO_WriteBit(GPIOB, GPIO_Pin_13, (BitAction)(x))
#define OLED_W_RES(x)       GPIO_WriteBit(GPIOB, GPIO_Pin_14, (BitAction)(x))
#define OLED_W_DC(x)        GPIO_WriteBit(GPIOB, GPIO_Pin_15, (BitAction)(x))
#define OLED_W_CS(x)        GPIO_WriteBit(GPIOA, GPIO_Pin_8, (BitAction)(x))
 

void OLED_SPI_Init(void);
void OLED_SPI_SendByte(uint8_t Byte);

#endif
