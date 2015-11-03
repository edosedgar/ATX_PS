
#include <stm32f10x_adc.h>


//=====================================================================
uint16_t ADCToIntVoltage(uint32_t Var,uint32_t R1,uint32_t R2);
char* ADCToVoltage(uint32_t Var,char* ArrayVar,uint32_t R1,uint32_t R2);
uint16_t ADCToIntCurrent(uint32_t Var,uint32_t Zero);
char* ADCToCurrent(uint32_t Var,char* ArrayVar,uint32_t Zero);
char* ADCToResistance(uint32_t Var,char* ArrayVar);
uint32_t CalculateDeviation(uint32_t SourceVar,uint32_t IdealVar);
char* DeviationToString(uint32_t Deviation,char* ArrayVar);
uint32_t RoundNumber(uint32_t Var);
int SignCurrent(int Difference);
void AlignCurrents(void);
int MaxCurrent(int Channel,int Power);
//=====================================================================

uint32_t Vcc=3300;
#define _12V 1
#define _5V  2
#define _3V3 3

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
	Buffer=(((Var*((Vcc)*100))/(4095*100)));
	Buffer=((Buffer)*100000)/(3300-Buffer)/100;
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
	return ArrayVar;
}
//**************************************************************************************
//***************************Deviation**************************************************
uint32_t CalculateDeviation(uint32_t SourceVar,uint32_t IdealVar)
{
	uint32_t Buffer;
	if (SourceVar>=IdealVar) {SourceVar-=IdealVar;} else {SourceVar=IdealVar-SourceVar;};
	SourceVar*=100000;
	Buffer=SourceVar/IdealVar;
	if (Buffer>=100000) {Buffer=99999;};
	return Buffer;
}
char* DeviationToString(uint32_t Deviation,char* ArrayVar)
{
	ArrayVar[0]=Deviation/10000+0x30;
	Deviation-=10000*(ArrayVar[0]-0x30);
	ArrayVar[1]=Deviation/1000+0x30;
	Deviation-=1000*(ArrayVar[1]-0x30);
	ArrayVar[2]='.';
	ArrayVar[3]=Deviation/100+0x30;
	Deviation-=100*(ArrayVar[3]-0x30);
	ArrayVar[4]=Deviation/10+0x30;
	Deviation-=10*(ArrayVar[4]-0x30);
	ArrayVar[5]='%';
	ArrayVar[6]=0;
	return ArrayVar;
}
//**************************************************************************************
uint32_t RoundNumber(uint32_t Var)
{
	uint32_t Digits[5],i,Count,Buffer;
	Count=10000;
	Buffer=Var;
	for (i=0;i<5;i++)
	{
		Digits[i]=Var/Count;
		Var-=Count*Digits[i];
		Count/=10;
	}
	if (Digits[4]>=5) {Buffer++;};
	return Buffer;
}
//**************************************************************************************
int SignCurrent(int Difference)
{
	if (Difference<0)  {return +1;};
	if (Difference==0) {return 0;};
	if (Difference>0)  {return -1;};
};
//Control Voltage for Gates
void AlignCurrents(void)
{
	//3V3
	if (Channel3V3.I_Ideal<Channel3V3.MinCurrentDetect)
	{
		switch (SignCurrent(Current3V3-Channel3V3.I_Ideal))
		{
			case -1 : if (Channel3V3.PWM!=2999) {Channel3V3.PWM++;}; break;
			case  0 : break;
			case +1 : if (Channel3V3.PWM!=0) {Channel3V3.PWM--;}; break;
		};
	};
	//5V
	if (Channel5V.I_Ideal<Channel5V.MinCurrentDetect)
	{
		switch (SignCurrent(Current5V-Channel5V.I_Ideal))
		{
			case -1 : if (Channel5V.PWM!=2999) {Channel5V.PWM++;}; break;
			case  0 : break;
			case +1 : if (Channel5V.PWM!=0) {Channel5V.PWM--;}; break;
		};
	};
	//12V
	if (Channel12V.I_Ideal<Channel12V.MinCurrentDetect)
	{
		switch (SignCurrent(Current12V-Channel12V.I_Ideal))
		{
			case -1 : if (Channel12V.PWM!=2999) {Channel12V.PWM++;}; break;
			case  0 : break;
			case +1 : if (Channel12V.PWM!=0) {Channel12V.PWM--;}; break;
		};
	};
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
