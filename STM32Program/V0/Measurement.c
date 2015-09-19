
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
//=====================================================================

uint32_t Vcc=3300;

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
