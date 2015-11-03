
#include <stm32f10x_gpio.h>
#include <stm32f10x.h>

#define LCD_E	GPIO_Pin_9
#define LCD_RS  GPIO_Pin_8
#define LCDDataPort GPIOC
#define LCDPort GPIOB
//===========================================
void Delay(volatile uint32_t nCount);
void InitLCD(void);
void WriteData(uint32_t Data);
void WriteCommand(uint32_t Command);
void ResetControllerLcd(void);
void SetCursor(uint8_t x,uint8_t y);
void WriteChar(uint8_t Char);
void WriteText(char *Text);
char* IntToString(uint32_t Var,char* ArrayVar);
//===========================================

void Delay(volatile uint32_t nCount)
{
    for (; nCount > 0; nCount--);
};

void WriteData(uint32_t Data)
{
	GPIO_WriteBit(LCDPort,LCD_RS,Bit_SET);
    (LCDDataPort->ODR)=((LCDDataPort->ODR&0xFF0F)+(Data&0xF0));
    GPIO_WriteBit(LCDPort,LCD_E ,Bit_SET);
    Delay(10);
    GPIO_WriteBit(LCDPort,LCD_E ,Bit_RESET);
    (LCDDataPort->ODR)=((LCDDataPort->ODR&0xFF0F)+((Data&0x0F)<<4));
    GPIO_WriteBit(LCDPort,LCD_E ,Bit_SET);
    Delay(10);
    GPIO_WriteBit(LCDPort,LCD_E ,Bit_RESET);
    GPIO_WriteBit(LCDPort,LCD_RS,Bit_RESET);
    Delay(300);
};

void WriteCommand(uint32_t Command)
{
	GPIO_WriteBit(LCDPort,LCD_RS,Bit_RESET);
	(LCDDataPort->ODR)=((LCDDataPort->ODR&0xFF0F)+(Command&0xF0));
    GPIO_WriteBit(LCDPort,LCD_E ,Bit_SET);
    Delay(10);
    GPIO_WriteBit(LCDPort,LCD_E ,Bit_RESET);
    (LCDDataPort->ODR)=((LCDDataPort->ODR&0xFF0F)+((Command&0x0F)<<4));
    GPIO_WriteBit(LCDPort,LCD_E ,Bit_SET);
    Delay(10);
    GPIO_WriteBit(LCDPort,LCD_E ,Bit_RESET);
    Delay(300);
};

void SetCursor(uint8_t x,uint8_t y)
{
	uint8_t Buffer;
	Buffer=(y-1)*0x40+(x-1);
	WriteCommand(0x80+Buffer);
};

void InitLCD(void)
{
	Delay(300000);
	GPIO_WriteBit(LCDPort,LCD_RS,Bit_RESET);
	GPIO_WriteBit(LCDPort,LCD_E ,Bit_SET);
    (LCDDataPort->ODR)=((LCDDataPort->ODR&0xFF0F)+0x20);
    GPIO_WriteBit(LCDPort,LCD_E ,Bit_RESET);
	Delay(300);
    WriteCommand(0x2C);
    WriteCommand(0x0C);
    WriteCommand(0x06);
    WriteCommand(0x01);
};

void WriteChar(uint8_t Char)
{
	WriteData(Char);
};

void WriteText(char *Text)
{
	while (*Text)
	{
		WriteChar(*Text++);
	}
};

char* IntToString(uint32_t Var,char* ArrayVar)
{
	ArrayVar[0]=Var/1000+0x30;
	Var-=1000*(ArrayVar[0]-0x30);
	ArrayVar[1]=Var/100+0x30;
	Var-=100*(ArrayVar[1]-0x30);
	ArrayVar[2]=Var/10+0x30;
	Var-=10*(ArrayVar[2]-0x30);
	ArrayVar[3]=Var/1+0x30;
	Var-=1*(ArrayVar[3]-0x30);
	ArrayVar[4]=0;
	return ArrayVar;
}
