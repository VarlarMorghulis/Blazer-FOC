#ifndef __IIC_H__
#define __IIC_H__

void IIC_Init(uint16_t GPIO_PinA, uint16_t GPIO_PinB);
void WaitEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT);
void WriteReg(uint8_t Address, uint8_t RegAddress, uint8_t Data);
uint8_t ReadReg(uint8_t Address, uint8_t RegAddress);

#endif
