#include "oled.h"

uint8_t u8x8_byte_4wire_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    switch (msg)
    {
        case U8X8_MSG_BYTE_SEND: /*ͨ��SPI����arg_int���ֽ�����*/
			HAL_SPI_Transmit_DMA(&hspi3, (uint8_t *)arg_ptr, arg_int);while(hspi3.TxXferCount);
			/*������DMAȡ����һ��ע�ͼ���*/
			//HAL_SPI_Transmit(&hspi3,(uint8_t *)arg_ptr,arg_int,200);
			/*����CubeMX���ɵĳ�ʼ��*/
            break;
        case U8X8_MSG_BYTE_INIT: /*��ʼ������*/
            break;
        case U8X8_MSG_BYTE_SET_DC: /*����DC����,�������͵������ݻ�������*/
			HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, (GPIO_PinState)arg_int);
            break;
        case U8X8_MSG_BYTE_START_TRANSFER: 
            u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_enable_level);
            u8x8->gpio_and_delay_cb(u8x8, U8X8_MSG_DELAY_NANO, u8x8->display_info->post_chip_enable_wait_ns, NULL);
            break;
        case U8X8_MSG_BYTE_END_TRANSFER: 
            u8x8->gpio_and_delay_cb(u8x8, U8X8_MSG_DELAY_NANO, u8x8->display_info->pre_chip_disable_wait_ns, NULL);
            u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_disable_level);
            break;
        default:
            return 0;
    }
    return 1;
}

uint8_t u8x8_stm32_gpio_and_delay(U8X8_UNUSED u8x8_t *u8x8,
    U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int,
    U8X8_UNUSED void *arg_ptr) 
{
    switch (msg)
    {
        case U8X8_MSG_DELAY_MILLI: /*��ʱ����*/
            delay_ms(arg_int);     //����˭stm32ϵͳ��ʱ����
            break;
        case U8X8_MSG_GPIO_CS: /*Ƭѡ�ź�*/ //����ֻ��һ��SPI�豸������Ƭѡ�ź��ڳ�ʼ��ʱ�Ѿ�����Ϊ����Ч
            HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, (GPIO_PinState)arg_int);
            break;
        case U8X8_MSG_GPIO_DC: /*����DC����,�������͵������ݻ�������*/
            HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, (GPIO_PinState)arg_int);
            break;
        case U8X8_MSG_GPIO_RESET:
            break;
    }
    return 1;
}

void u8g2Init(u8g2_t *u8g2)
{
/********************************************     
U8G2_R0     //����ת��������     
U8G2_R1     //��ת90��
U8G2_R2     //��ת180��   
U8G2_R3     //��ת270��
U8G2_MIRROR   //û����ת��������ʾ���Ҿ���
U8G2_MIRROR_VERTICAL    //û����ת��������ʾ����
********************************************/
//    u8g2_Setup_sh1106_128x64_noname_2(u8g2, U8G2_R0, u8x8_byte_4wire_hw_spi, u8x8_stm32_gpio_and_delay);  // ��ʼ��1.3��OLED u8g2 �ṹ��
	//u8g2_Setup_ssd1306_128x64_noname_f(u8g2, U8G2_R0, u8x8_byte_4wire_hw_spi, u8x8_stm32_gpio_and_delay);  // ��ʼ��0.96��OLED u8g2 �ṹ��
	u8g2_Setup_sh1107_128x80_f(u8g2, U8G2_R3, u8x8_byte_4wire_hw_spi, u8x8_stm32_gpio_and_delay);
	u8g2_InitDisplay(u8g2);     //��ʼ����ʾ
	u8g2_SetPowerSave(u8g2, 0); //������ʾ
	u8g2_ClearDisplay(u8g2);
}
/*�ٷ�logo��Demo*/
void draw(u8g2_t *u8g2)
{
    u8g2_SetFontMode(u8g2, 1); /*����ģʽѡ��*/
    u8g2_SetFontDirection(u8g2, 0); /*���巽��ѡ��*/
    u8g2_SetFont(u8g2, u8g2_font_inb24_mf); /*�ֿ�ѡ��*/
    u8g2_DrawStr(u8g2, 0, 20, "U");
    
    u8g2_SetFontDirection(u8g2, 1);
    u8g2_SetFont(u8g2, u8g2_font_inb30_mn);
    u8g2_DrawStr(u8g2, 21,8,"8");
        
    u8g2_SetFontDirection(u8g2, 0);
    u8g2_SetFont(u8g2, u8g2_font_inb24_mf);
    u8g2_DrawStr(u8g2, 51,30,"g");
    u8g2_DrawStr(u8g2, 67,30,"\xb2");
    
    u8g2_DrawHLine(u8g2, 2, 35, 47);
    u8g2_DrawHLine(u8g2, 3, 36, 47);
    u8g2_DrawVLine(u8g2, 45, 32, 12);
    u8g2_DrawVLine(u8g2, 46, 33, 12);
  
    u8g2_SetFont(u8g2, u8g2_font_4x6_tr);
    u8g2_DrawStr(u8g2, 1,54,"github.com/olikraus/u8g2");
	u8g2_SendBuffer(u8g2);
}

u8g2_t u8g2;
void draw_logo(void)
{
	u8g2_ClearDisplay(&u8g2);
	u8g2_SendBuffer(&u8g2);
}

void draw_start(void)
{
	u8g2_SetFont(&u8g2,u8g2_font_ncenB10_tr);
	u8g2_ClearDisplay(&u8g2);
	u8g2_DrawStr(&u8g2,10,20,"Calib");
	u8g2_DrawStr(&u8g2,10,40,"Ident");
	
	u8g2_SendBuffer(&u8g2);
}

void OLED_Init(void)
{
	OLED_RST_Set();
	u8g2Init(&u8g2);
	u8g2_SetFont(&u8g2, u8g2_font_6x10_mf);
}