#include <stm32f10x_gpio.h>
#include <Measurement.c>
#include <HD44780LCD.c>
#include <InitPeripheral.c>
#include <stm32f10x_tim.h>
#include <stm32f10x.h>

void TIM6_DAC_IRQHandler(void);
void TIM7_IRQHandler(void);

int SignCurrent(int Difference);
void CalculateCurrents(void);
void SetPWMOutput(void);

uint8_t NumberActionsButton=4;
//Current variables and defines
#define Current12V ADCBuffer[0]
#define Current5V  ADCBuffer[1]
#define Current3V3 ADCBuffer[2]
#define Zero3V3 2520
#define Zero5V 2517
#define Zero12V 2513
#define MinCurrentDetect3V3 3110
#define MinCurrentDetect5V 3110
#define MinCurrentDetect12V 3100
#define ZeroCurrent3V3 3127;
#define ZeroCurrent5V 3123;
#define ZeroCurrent12V 3118;
uint16_t PWM_3V3, PWM_5V, PWM_12V;
uint32_t I_3V3Real, I_5VReal, I_12VReal,I_12VControl, I1,I2,I3, Counter2=0;
uint32_t I_3V3Ideal, I_5VIdeal, I_12VIdeal;
//Voltage variables and defines
uint32_t V_3V3, V_5V, V_12V, V_5ST, V_Power, V1, V2, V3, V4, V5, Counter=0;
#define Voltage12V 		 ADCBuffer[6]
#define Voltage5V  		 ADCBuffer[7]
#define Voltage3V3 		 ADCBuffer[8]
#define Voltage5ST 		 ADCBuffer[9]
#define PowerVoltage 	 ADCBuffer[10]
#define PowerGoodVoltage ADCBuffer[11]
#define R1_3V3 14950
#define R2_3V3 15020
#define R1_5V 15010
#define R2_5V 7510
#define R1_12V 14990
#define R2_12V 3590
#define R1_5ST 15100
#define R2_5ST 7510
#define R1_VPower 15100
#define R2_VPower 3590
#define R1_PGood 15000
#define R2_PGood 15000
//Resistance defines
#define Resistance12V ADCBuffer[5]
#define Resistance5V  ADCBuffer[4]
#define Resistance3V3 ADCBuffer[3]
//Array uses for out text on display
char BufferArray[7];

//**************************************************************************************
int main(void)
{
	RCC_Configuration();
	GPIO_Configuration();
	InitLCD();
	ADC_Configuration();
	DMA_Configuration();
	TIM_Configuration();
	I_3V3Ideal=ZeroCurrent3V3; PWM_3V3=0;
	I_5VIdeal=ZeroCurrent5V; PWM_5V=0;
	I_12VIdeal=ZeroCurrent12V; PWM_12V=0;
	while (1)
    {
    	if (((GPIOB->IDR)&0x0008)!=0x0008)
    	{
    		if (NumberActionsButton==1) {if (ADCToIntCurrent(I_3V3Ideal,Zero3V3)<1000) {I_3V3Ideal-=12;};};
    		if (NumberActionsButton==2) {if (ADCToIntCurrent(I_5VIdeal,Zero5V)<700) {I_5VIdeal-=12;};};
    		if (NumberActionsButton==3) {if (ADCToIntCurrent(I_12VIdeal,Zero12V)<1500) {I_12VIdeal-=12;};NumberActionsButton=0;};
    		if (NumberActionsButton==4)
    		{
    			WriteCommand(0x01);
    			Delay(100000);
    			GPIO_WriteBit(GPIOB,GPIO_Pin_6,Bit_RESET);
    			SetCursor(1,1);
    			Delay(100000);
    			WriteText("Waiting PowerGood...");
    			Delay(10);
    			while (ADCToIntVoltage(PowerGoodVoltage,R1_PGood,R2_PGood)<4000) {};
    			SetCursor(1,2);
    			Delay(10);
    			WriteText("OK. ATX12V started!!");
    			Delay(500000);
    			WriteCommand(0x01);
    			NumberActionsButton=0;
    		}
    		NumberActionsButton++;
    	};

    	if (((GPIOB->IDR)&0x0020)!=0x0020)
    	{
    		if (NumberActionsButton!=4)
    		{
				I_3V3Ideal=ZeroCurrent3V3; PWM_3V3=0;
				I_5VIdeal=ZeroCurrent5V; PWM_5V=0;
				I_12VIdeal=ZeroCurrent12V; PWM_12V=0;
				NumberActionsButton=4;
				WriteCommand(0x01);
				Delay(100000);
				GPIO_WriteBit(GPIOB,GPIO_Pin_6,Bit_SET);
				SetCursor(1,1);
				Delay(100000);
				WriteText("Waiting PowerGood...");
				Delay(10);
				while (ADCToIntVoltage(PowerGoodVoltage,R1_PGood,R2_PGood)>1000) {};
				SetCursor(1,2);
				Delay(10);
				WriteText("OK. ATX12V stopped!!");
				Delay(500000);
				WriteCommand(0x01);
    		};
    	};
    	SetCursor(1,1);
    	WriteText(ADCToCurrent(I_3V3Real,BufferArray,Zero3V3));
    	SetCursor(8,1);
    	WriteText(ADCToCurrent(I_5VReal,BufferArray,Zero5V));
    	SetCursor(15,1);
    	WriteText(ADCToCurrent(I_12VReal,BufferArray,Zero12V));
    	SetCursor(1,2);
    	WriteText(ADCToVoltage(V_3V3,BufferArray,R1_3V3,R2_3V3));
    	SetCursor(8,2);
    	WriteText(ADCToVoltage(V_5V,BufferArray,R1_5V,R2_5V));
    	//WriteText(ADCToVoltage(V_5ST,BufferArray,R1_5ST,R2_5ST));
    	//WriteText(ADCToVoltage(PowerGoodVoltage,BufferArray,R1_PGood,R2_PGood));
    	SetCursor(15,2);
    	WriteText(ADCToVoltage(V_12V,BufferArray,R1_12V,R2_12V));
    	Delay(50000);
    	//WriteText(ADCToVoltage(V_Power,BufferArray,R1_VPower,R2_VPower));
    }
}
//**************************************************************************************
int SignCurrent(int Difference)
{
	if (Difference<0)  {return +1;};
	if (Difference==0) {return 0;};
	if (Difference>0)  {return -1;};
}
//**************************************************************************************
void SetPWMOutput(void)
{
	TIM1->CCR1=PWM_3V3;
	TIM1->CCR2=PWM_5V;
	TIM1->CCR3=PWM_12V;
}
//**************************************************************************************
void TIM6_DAC_IRQHandler(void)
{
	TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
	//Control currents up
	//if (ADCToIntCurrent(Current3V3,Zero3V3)>1100) {I_3V3Ideal=ZeroCurrent3V3;PWM_3V3=0;};
	//if (ADCToIntCurrent(Current5V,Zero5V)>800) {I_5VIdeal=ZeroCurrent5V;PWM_5V=0;};
	//if (ADCToIntCurrent(Current12V,Zero12V)>1600) {I_12VIdeal=ZeroCurrent12V;PWM_12V=0;};
	//3V3
	if (I_3V3Ideal<MinCurrentDetect3V3)
	{
		switch (SignCurrent(Current3V3-I_3V3Ideal))
		{
			case -1 : if (PWM_3V3!=2999) {PWM_3V3++;}; break;
			case  0 : /*------*/ break;
			case +1 : if (PWM_3V3!=0) {PWM_3V3--;}; break;
		};
	};
	//5V
	if (I_5VIdeal<MinCurrentDetect5V)
	{
		switch (SignCurrent(Current5V-I_5VIdeal))
		{
			case -1 : if (PWM_5V!=2999) {PWM_5V++;}; break;
			case  0 : /*------*/ break;
			case +1 : if (PWM_5V!=0) {PWM_5V--;}; break;
		};
	};
	//12V
	if (I_12VIdeal<MinCurrentDetect12V)
	{
		switch (SignCurrent(Current12V-I_12VIdeal))
		{
			case -1 : if (PWM_12V!=2999) {PWM_12V++;}; break;
			case  0 : /*------*/ break;
			case +1 : if (PWM_12V!=0) {PWM_12V--;}; break;
		};
	};
	SetPWMOutput();
}
//**************************************************************************************
void TIM7_IRQHandler(void)
{
	TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
	//Align Voltages
	V1+=Voltage3V3;
	V2+=Voltage5V;
	V3+=Voltage12V;
	V4+=Voltage5ST;
	V5+=PowerVoltage;
	Counter++;
	if (Counter==100)
	{
		Counter=0;
		V_3V3=V1/100;
		V_5V=V2/100;
		V_12V=V3/100;
		V_5ST=V4/100;
		V_Power=V5/100;
		V1=0;V2=0;V3=0;V4=0;V5=0;
	}
	//AlignCurrents
	I1+=Current3V3;
	I2+=Current5V;
	I3+=Current12V;
	Counter2++;
	if (Counter2==100)
	{
		Counter2=0;
		I_3V3Real=I1/100;
		I_5VReal =I2/100;
		I_12VReal=I3/100;
		I1=0; I2=0; I3=0;
	}
}
//**************************************************************************************
char* VarToString(uint32_t Var,char* ArrayVar)
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
//**************************************************************************************
