
#include <stm32f10x_adc.h>


//=====================================================================
uint16_t ADCToIntVoltage(uint32_t Var,uint32_t R1,uint32_t R2);
char* ADCToVoltage(uint32_t Var,char* ArrayVar,uint32_t R1,uint32_t R2);
uint16_t ADCToIntCurrent(uint32_t Var,uint32_t Zero);
char* ADCToCurrent(uint32_t Var,char* ArrayVar,uint32_t Zero);
char* ADCToResistance(uint32_t Var,char* ArrayVar);
uint32_t ADCToIntResistance(uint32_t Var);
char* IntToTime_mS(uint16_t Time,char* ArrayVar);
char* IntToPower(uint32_t Power,char* ArrayVar);
uint32_t RoundNumber(uint32_t Var);
int SignCurrent(int Difference);
void AlignCurrents(void);
int MaxCurrent(int Channel,int Power);
void CalculateAverageAV(void);
void PollOfButtons(void);
void PercentToCurrent(uint8_t Percent);
//=====================================================================

uint32_t Vcc=3300;
#define _12V 1
#define _5V  2
#define _3V3 3
#define PowerOn GPIO_WriteBit(GPIOA,GPIO_Pin_5,Bit_RESET)
#define PowerOff GPIO_WriteBit(GPIOA,GPIO_Pin_5,Bit_SET)

//*******************************Voltage************************************************
uint16_t ADCToIntVoltage(uint32_t Var,uint32_t R1,uint32_t R2)
{
	Var=(((Var*((Vcc)*100))/(4095*100)));
	Var*=((R1*1000)/R2+1000);
	Var/=1000;
	Var=RoundNumber(Var);
	return Var;
}
char* ADCToVoltage(uint32_t Var,char* ArrayVar,uint32_t R1,uint32_t R2)
{
	Var=(((Var*((Vcc)*100))/(4095*100)));
	Var*=((R1*1000)/R2+1000);
	Var/=1000;
	Var=RoundNumber(Var);
	ArrayVar[0]=Var/10000+0x30;
	Var-=10000*(ArrayVar[0]-0x30);
	ArrayVar[1]=Var/1000+0x30;
	Var-=1000*(ArrayVar[1]-0x30);
	ArrayVar[2]='.';
	ArrayVar[3]=Var/100+0x30;
	Var-=100*(ArrayVar[3]-0x30);
	ArrayVar[4]=Var/10+0x30;
	ArrayVar[5]='V';
	ArrayVar[6]=0;
	return ArrayVar;
}
//**************************************************************************************
//******************************Current*************************************************
uint16_t ADCToIntCurrent(uint32_t Var,uint32_t Zero)
{
	long int Buffer;
	Buffer=(Vcc*Var*1)/4095;
	Buffer=Zero*1-Buffer;
	if ((Zero==2513)&&(Buffer>600)) {Buffer=Buffer+((Buffer-600)/10);} else {Buffer=Buffer;};
	//if (Buffer<0) {Buffer=0;};
	Buffer=RoundNumber(Buffer);
	return Buffer;
}
//**************************************************************************************
uint16_t CurrentToADC(uint32_t Var,uint32_t Zero)
{
	if ((Zero==2513)&&(Var>600)) {Var=(10*Var+600)/11;};
	Var=Zero*1-Var;
	Var=(4095*Var)/(Vcc*1);
	return Var;
}
//**************************************************************************************
char* ADCToCurrent(uint32_t Var,char* ArrayVar,uint32_t Zero)
{
	long int Buffer;
	Buffer=(Vcc*Var*1)/4095;
	Buffer=Zero*1-Buffer;
	if ((Zero==2513)&&(Buffer>600)) {Buffer=Buffer+((Buffer-600)/10);} else {Buffer=Buffer;};
	//if (Buffer<0) {Buffer=0;};
	Buffer=RoundNumber(Buffer);
	ArrayVar[0]=Buffer/1000+0x30;
	Buffer-=1000*(ArrayVar[0]-0x30);
	ArrayVar[1]=Buffer/100+0x30;
	Buffer-=100*(ArrayVar[1]-0x30);
	ArrayVar[2]='.';
	ArrayVar[3]=Buffer/10+0x30;
	ArrayVar[4]='A';
	ArrayVar[5]=0;
	return ArrayVar;
}
//**************************************************************************************
//****************************Resistance************************************************
char* ADCToResistance(uint32_t Var,char* ArrayVar)
{
	uint32_t Buffer;
	Var=(((Var*((Vcc-10)*100))/(4095*100)));
	Buffer=((Var*101)/((Vcc-10)-Var))*10;
	Buffer=RoundNumber(Buffer);
	if (Buffer<10000)
	{
		ArrayVar[0]=Buffer/10000+0x30;
		Buffer-=10000*(ArrayVar[0]-0x30);
		ArrayVar[1]=Buffer/1000+0x30;
		Buffer-=1000*(ArrayVar[1]-0x30);
		ArrayVar[2]=Buffer/100+0x30;
		Buffer-=100*(ArrayVar[2]-0x30);
		ArrayVar[3]=Buffer/10+0x30;
		Buffer-=10*(ArrayVar[3]-0x30);
		ArrayVar[4]='R';
		ArrayVar[5]=0;
	}  else
	{
		ArrayVar[0]=' ';
		ArrayVar[1]='O';
		ArrayVar[2]='L';
		ArrayVar[3]=' ';
		ArrayVar[4]='R';
		ArrayVar[5]=0;
	}
	return ArrayVar;
}
//**************************************************************************************
uint32_t ADCToIntResistance(uint32_t Var)
{
	uint32_t Buffer;
	Var=(((Var*((Vcc-10)*100))/(4095*100)));
	Buffer=((Var*101)/((Vcc-10)-Var))*10;
	if (Buffer>20000) {Buffer=0;};
	Buffer=RoundNumber(Buffer);
	Buffer/=10;
	return Buffer;
}
//**************************************************************************************
char* IntToTime_mS(uint16_t Time,char* ArrayVar)
{
	ArrayVar[0]=Time/100+0x30;
	Time-=100*(ArrayVar[0]-0x30);
	ArrayVar[1]=Time/10+0x30;
	Time-=10*(ArrayVar[1]-0x30);
	ArrayVar[2]=Time+0x30;
	ArrayVar[3]='m';
	ArrayVar[4]='S';
	ArrayVar[5]=0;
	return ArrayVar;
}
//**************************************************************************************
char* IntToPower(uint32_t Power,char* ArrayVar)
{
	ArrayVar[0]=Power/100000+0x30;
	Power-=100000*(ArrayVar[0]-0x30);
	ArrayVar[1]=Power/10000+0x30;
	Power-=10000*(ArrayVar[1]-0x30);
	ArrayVar[2]=Power/1000+0x30;
	Power-=1000*(ArrayVar[2]-0x30);
	ArrayVar[3]='W';
	ArrayVar[4]=0;
	return ArrayVar;
}
//**************************************************************************************
uint32_t RoundNumber(uint32_t Var)
{
	uint32_t Digits[5],i,Count,Buffer;
	Count=10000;
	Buffer=Var;
	Buffer/=10;
	for (i=0;i<5;i++)
	{
		Digits[i]=Var/Count;
		Var-=Count*Digits[i];
		Count/=10;
	}
	if (Digits[4]>=5) {Buffer++;};
	return (Buffer*10);
}
//**************************************************************************************
int SignCurrent(int Difference)
{
	if (Difference<0)  {return -1;};
	if (Difference==0) {return 0;};
	if (Difference>0)  {return +1;};
};
//Control Voltage for Gates
void AlignCurrents(void)
{
	//3V3
	if (Channel3V3.I_Ideal<Channel3V3.MinCurrentDetect)
	{
		switch (SignCurrent(Current3V3-Channel3V3.I_Ideal))
		{
			case +1 : if (Channel3V3.PWM!=2999) {Channel3V3.PWM++;}; break;
			case  0 : break;
			case -1 : if (Channel3V3.PWM!=0) {Channel3V3.PWM--;}; break;
		};
	} else {Channel3V3.PWM=0;};
	//5V
	if (Channel5V.I_Ideal<Channel5V.MinCurrentDetect)
	{
		switch (SignCurrent(Current5V-Channel5V.I_Ideal))
		{
			case +1 : if (Channel5V.PWM!=2999) {Channel5V.PWM++;}; break;
			case  0 : break;
			case -1 : if (Channel5V.PWM!=0) {Channel5V.PWM--;}; break;
		};
	} else {Channel5V.PWM=0;};
	//12V
	if (Channel12V.I_Ideal<Channel12V.MinCurrentDetect)
	{
		switch (SignCurrent(Current12V-Channel12V.I_Ideal))
		{
			case +1 : if (Channel12V.PWM!=2999) {Channel12V.PWM++;}; break;
			case  0 : break;
			case -1 : if (Channel12V.PWM!=0) {Channel12V.PWM--;}; break;
		};
	} else {Channel12V.PWM=0;};
	//Set PWM (Voltage for Gate MOSFETs)
	TIM1->CCR1=Channel3V3.PWM;
	TIM1->CCR2=Channel5V.PWM;
	TIM1->CCR3=Channel12V.PWM;
}
//**************************************************************************************
int MaxCurrent(int Channel,int Power)
{
	switch (Power)
	{
		case 250:  switch (Channel) { case _12V: return 710;  break; case _5V: return 780; break; case _3V3: return 180; break;}; break;
		case 300:  switch (Channel) { case _12V: return 830;  break; case _5V: return 450; break; case _3V3: return 680; break;}; break;
		case 350:  switch (Channel) { case _12V: return 1000; break; case _5V: return 480; break; case _3V3: return 790; break;}; break;
		case 400:  switch (Channel) { case _12V: return 1160; break; case _5V: return 560; break; case _3V3: return 800; break;}; break;
		case 450:  switch (Channel) { case _12V: return 1340; break; case _5V: return 590; break; case _3V3: return 870; break;}; break;
	}
}
//**************************************************************************************
void CalculateAverageAV(void)
{
	//Average Voltages
	Channel3V3.VBuffer+=Voltage3V3;
	Channel5V.VBuffer+=Voltage5V;
	Channel12V.VBuffer+=Voltage12V;
	Channel5VSB.VBuffer+=Voltage5ST;
	Power.VBuffer+=PowerVoltage;
	PowerOK.VBuffer+=PowerGoodVoltage;
	SystemSettings.CounterVolts++;
	if (SystemSettings.CounterVolts==100)
	{
		SystemSettings.CounterVolts=0;
		Channel3V3.V_Real=Channel3V3.VBuffer/100;   Channel3V3.VBuffer=0;
		Channel5V.V_Real=Channel5V.VBuffer/100;	    Channel5V.VBuffer=0;
		Channel12V.V_Real=Channel12V.VBuffer/100;   Channel12V.VBuffer=0;
		Channel5VSB.V_Real=Channel5VSB.VBuffer/100; Channel5VSB.VBuffer=0;
		Power.V_Real=Power.VBuffer/100;  			Power.VBuffer=0;
		PowerOK.V_Real=PowerOK.VBuffer/100;			PowerOK.VBuffer=0;
	}
	//Average Currents
	Channel3V3.IBuffer+=Current3V3;
	Channel5V.IBuffer+=Current5V;
	Channel12V.IBuffer+=Current12V;
	SystemSettings.CounterAmps++;
	if (SystemSettings.CounterAmps==100)
	{
		SystemSettings.CounterAmps=0;
		Channel3V3.I_Real=Channel3V3.IBuffer/100; Channel3V3.IBuffer=0;
		Channel5V.I_Real=Channel5V.IBuffer/100;   Channel5V.IBuffer=0;
		Channel12V.I_Real=Channel12V.IBuffer/100; Channel12V.IBuffer=0;
	}
}
//**************************************************************************************
void PollOfButtons(void)
{
	if (Button.CounterB1>=5) {Button.CounterB1++; if (Button.CounterB1==35) {Button.CounterB1=0;};}
	if ((((GPIOB->IDR)&0x0008)!=0x0008)&&(Button.B1==0)&&(Button.CounterB1==0))
	{
		Beep.State1=1;
		Button.OK=1;
		Button.B1=Button.OK;
		Button.CounterB1=5;

	} else
	{
		Button.OK=0;
		if (((GPIOB->IDR)&0x0008)==0x0008) {Button.B1=0;};
		Beep.State1=0;
	};
	//---
	if (Button.CounterB2>=5) {Button.CounterB2++; if (Button.CounterB2==35) {Button.CounterB2=0;};}
	if ((((GPIOB->IDR)&0x0020)!=0x0020)&&(Button.B2==0)&&(Button.CounterB2==0))
	{
		Beep.State2=1;
		Button.C=1;
		Button.B2=Button.C;
		Button.CounterB2=5;
	} else
	{
		Button.C=0;
		if (((GPIOB->IDR)&0x0020)==0x0020) {Button.B2=0;};
		Beep.State2=0;
	};
}
//**************************************************************************************
void PercentToCurrent(uint8_t Percent)
{
	uint16_t OnePercent;
	OnePercent=(Channel3V3.ZeroCurrent-CurrentToADC(MaxCurrent(_3V3,SystemSettings.PSUPower),Channel3V3.VZero));
	Channel3V3.I_Ideal=(Channel3V3.ZeroCurrent*100-(OnePercent*Percent))/100;

	OnePercent=(Channel5V.ZeroCurrent-CurrentToADC(MaxCurrent(_5V,SystemSettings.PSUPower),Channel5V.VZero));
	Channel5V.I_Ideal=(Channel5V.ZeroCurrent*100-(OnePercent*Percent))/100;

	OnePercent=(Channel12V.ZeroCurrent-CurrentToADC(MaxCurrent(_12V,SystemSettings.PSUPower),Channel12V.VZero));
	Channel12V.I_Ideal=(Channel12V.ZeroCurrent*100-(OnePercent*Percent))/100;
}
//**************************************************************************************
