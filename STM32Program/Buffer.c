#include <stm32f10x_gpio.h>
#include <InitPeripheral.c>
#include <Structures.c>
#include <Measurement.c>
#include <HD44780LCD.c>
#include <stm32f10x_tim.h>
#include <stm32f10x.h>

#define RED 1
#define GREEN 2
#define Skilled 1
#define Normal 0
#define ON 1
#define OFF 0
#define PowerOn GPIO_WriteBit(GPIOB,GPIO_Pin_6,Bit_RESET)
#define PowerOff GPIO_WriteBit(GPIOB,GPIO_Pin_6,Bit_SET)

void TIM6_DAC_IRQHandler(void);
void TIM7_IRQHandler(void);
void TIM2_IRQHandler(void);
int PressButton(int NumberButton);
void ExitWithErrors(uint8_t NumberErrors);
char* VarToString(uint32_t Var,char* ArrayVar);
uint8_t RedButton=0, GreenButton=0;
uint8_t B1=0,B2=0,CounterB1=0,CounterB2=0,i;
void ShowLogo(void);

//Array uses for out text on display
char BufferArray[7];

uint32_t Counter2=0, Counter=0;

//**************************************************************************************
int main(void)
{
	RCC_Configuration();
	GPIO_Configuration();
	InitConstants();
	InitLCD();
	ADC_Configuration();
	DMA_Configuration();
	TIM_Configuration();
	WriteText(14,1,VarToString(PowerOK.TimeOn,BufferArray));
	//Test Power=======================
	/*WriteText(1,1,"   ATX PSU Tester   ");
	WriteText(1,2,"    Version 1.00    ");
	Delay(5000000);
	ClrScr();
	if (ADCToIntVoltage(PowerVoltage,Power.R1,Power.R2)<12800)
	{
		WriteText(1,1,"      Battery       ");
		WriteText(1,2,"      supply        ");
		Delay(5000000);
		ClrScr();
		WriteText(1,1,"Battery charge:     ");
		WriteText(1,2,"      supply        ");
		int V1;
		LoopTest:
		V1=ADCToIntVoltage(PowerVoltage,Power.R1,Power.R2);
		Delay(5000000);
		if ((V1-ADCToIntVoltage(PowerVoltage,Power.R1,Power.R2)>100) || (ADCToIntVoltage(PowerVoltage,Power.R1,Power.R2)<11200)) {WriteText(1,2,"Bat.low.Turn on PSU!"); Power.PowerNorm=0; Delay(10000); goto LoopTest;}
		else {WriteText(1,2,"      Power OK      "); Power.PowerNorm=1; };
	}
	else  {WriteText(1,1,"         AC         ");}
	Delay(5000000);
	ClrScr();
	Power.PowerTest=ON;
	//==================================
	WriteText(1,1,"Test channels resist");
	//
	Delay(5000000);
	WriteText(1,2,"Resistance OK");
	Delay(5000000);
	ClrScr();
	//==================================
	WriteText(1,1," Press right button ");
	WriteText(1,2,"  for choose power  ");
	Delay(5000000);
	ClrScr();
	WriteText(1,1," Press left button  ");
	WriteText(1,2,"  for select power  ");
	Delay(5000000);
	ClrScr();
	while (RedButton!=1)
	{
		if (GreenButton==1)
		{
			SystemSettings.PSUPower+=50;
			if (SystemSettings.PSUPower==500) {SystemSettings.PSUPower=250;};
			Delay(10000);
		}
		switch (SystemSettings.PSUPower)
		{
			case 250: WriteText(1,1,"   Power:  [250W]   "); break;
			case 300: WriteText(1,1,"   Power:  [300W]   "); break;
			case 350: WriteText(1,1,"   Power:  [350W]   "); break;
			case 400: WriteText(1,1,"   Power:  [400W]   "); break;
			case 450: WriteText(1,1,"   Power:  [450W]   "); break;
		}
	};
	ClrScr();
	Delay(700000);
	//===================================
	WriteText(1,1,"  Choose type user  ");
	while (RedButton!=1)
	{
		if (GreenButton==1)
		{
			SystemSettings.TypeUser++;
			if (SystemSettings.TypeUser==2) {SystemSettings.TypeUser=0;};
			Delay(10000);
		}
		switch (SystemSettings.TypeUser)
		{
			case Normal:  WriteText(1,2," [Normal]  Skilled  "); break;
			case Skilled: WriteText(1,2,"  Normal  [Skilled] "); break;
		}
	};
	ClrScr();
	Delay(700000);
	//===================================
	WriteText(1,1,"   OK. All ready.   ");
	WriteText(1,2,"  Starting test.    ");
	Delay(500000);
	WriteText(1,2,"  Starting test..   ");
	Delay(500000);
	WriteText(1,2,"  Starting test...  ");
	Delay(5000000);
	ClrScr();
	//===============================================
	//5VSB Test
	if (ADCToIntVoltage(Channel5VSB.V_Real,Channel5VSB.R1,Channel5VSB.R2)<4750) {ExitWithErrors(5);};
	if (ADCToIntVoltage(Channel5VSB.V_Real,Channel5VSB.R1,Channel5VSB.R2)>5250) {ExitWithErrors(6);};
	//===============================================================================================================================================================
	//PowerOk Test
	PowerOn;
	SystemSettings.TimeCountOn=ON;
	while (ADCToIntVoltage(PowerGoodVoltage,PowerOK.R1,PowerOK.R2)<2400)
	{
		if (PowerOK.TimeOn>600)
		{
			SystemSettings.TimeCountOn=OFF;
			ExitWithErrors(1);
		}
	};
	SystemSettings.TimeCountOn=OFF;
	if (PowerOK.TimeOn>600)
	{
		ExitWithErrors(2);
	}
	WriteText(1,1," Stab-ion volts...");
	Delay(5000000);
	ClrScr;
	PowerOK.Monitoring=ON;
	//=========
	SystemSettings.StartTest=ON;
	//===========================================================
	if (SystemSettings.TypeUser==Normal)
	{
		ClrScr;
		WriteText(1, 1,"     Loading...    ");
		SetCursor(1,2);
		uint8_t ProgressTest=1;
		uint8_t i;
		//================
		while (SystemSettings.TestOut==0)
		{
			SetCursor(1,2);
			Delay(50000);
			for (i=1;i<=ProgressTest;i++)
			{
				WriteChar(0xFF);
			};
			Delay(1000000); ProgressTest++;
			if (ProgressTest==20) {SystemSettings.TestOut=1;};
			//
		}
	} else
	//===============================================================================================================================================================
	{
		WriteText(2,1,"P.G. time:  "); WriteText(14,1,VarToString(PowerOK.TimeOn,BufferArray)); WriteText(18,1,"ms");
		WriteText(2,2,"P.G. volts: "); WriteText(14,2,ADCToVoltage(PowerOK.V_Real,BufferArray,PowerOK.R1,PowerOK.R2));
		while (RedButton!=1) {};
		Delay(700000);
		ClrScr();
		WriteText(2,1,"5VSB Volts:"); WriteText(14,1,ADCToVoltage(Channel5VSB.V_Real,BufferArray,Channel5VSB.R1,Channel5VSB.R2));
		while (RedButton!=1) {};
		ClrScr();
		while (SystemSettings.TestOut==0)
		{
	    	WriteText(1, 1,ADCToCurrent(Channel3V3.I_Real,BufferArray,Channel3V3.VZero));
	    	WriteText(8, 1,ADCToCurrent(Channel5V.I_Real, BufferArray,Channel5V.VZero));
	    	WriteText(15,1,ADCToCurrent(Channel12V.I_Real,BufferArray,Channel12V.VZero));
	    	WriteText(1, 2,ADCToVoltage(Channel3V3.V_Real,BufferArray,Channel3V3.R1,Channel3V3.R2));
	    	WriteText(8, 2,ADCToVoltage(Channel5V.V_Real, BufferArray,Channel5V.R1,Channel5V.R2));
	    	WriteText(15,2,ADCToVoltage(Channel12V.V_Real,BufferArray,Channel12V.R1,Channel12V.R2));
	    	Delay(50000);
		}
	}
	//===============================================================================================================================================================
	PowerOff;
	InitConstants();
	TIM1->CCR1=Channel3V3.PWM;
	TIM1->CCR2=Channel5V.PWM;
	TIM1->CCR3=Channel12V.PWM;
	//--------------
	ClrScr;
	WriteText(1, 1," OK. Test finished ");
	WriteText(1, 2,"    No problems.   ");
	Delay(5000000);*/
	while (1)
    {

    	/*if (((GPIOB->IDR)&0x0008)!=0x0008)
    	{
    		if (NumberActionsButton==1) {if (ADCToIntCurrent(Channel3V3.I_Ideal,Channel3V3.VZero)<1000) {Channel3V3.I_Ideal-=12;};};
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
				//I_3V3Ideal=ZeroCurrent3V3; PWM_3V3=0;
				//I_5VIdeal=ZeroCurrent5V; PWM_5V=0;
				//I_12VIdeal=ZeroCurrent12V; PWM_12V=0;
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
    	WriteText(ADCToCurrent(Channel3V3.I_Real,BufferArray,Channel3V3.VZero));
    	SetCursor(8,1);
    	WriteText(ADCToCurrent(I_5VReal,BufferArray,Zero5V));
    	SetCursor(15,1);
    	WriteText(ADCToCurrent(I_12VReal,BufferArray,Zero12V));
    	SetCursor(1,2);
    	WriteText(ADCToVoltage(Channel3V3.V_Real,BufferArray,Channel3V3.R1,Channel3V3.R2));
    	SetCursor(8,2);
    	WriteText(ADCToVoltage(V_5V,BufferArray,R1_5V,R2_5V));
    	//WriteText(ADCToVoltage(V_5ST,BufferArray,R1_5ST,R2_5ST));
    	//WriteText(ADCToVoltage(PowerGoodVoltage,BufferArray,R1_PGood,R2_PGood));
    	SetCursor(15,2);
    	WriteText(ADCToVoltage(V_12V,BufferArray,R1_12V,R2_12V));

    	//WriteText(ADCToVoltage(V_Power,BufferArray,R1_VPower,R2_VPower));*/
    }
}
//**************************************************************************************

void TIM6_DAC_IRQHandler(void)
{
	TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
	/*if (SystemSettings.TimeCountOn==ON) {PowerOK.TimeOn++;};
	if (Power.PowerTest==ON) {if (PowerVoltage<2650) {Power.PowerNorm=0; ExitWithErrors(3);};};
	if (PowerOK.Monitoring==ON) {if (PowerGoodVoltage<1480) {ExitWithErrors(4);};};
	if (SystemSettings.StartTest==ON) {AlignCurrents();};
	if (CounterB1>=5) {CounterB1++; if (CounterB1==50) {CounterB1=0;};}

	if ((((GPIOB->IDR)&0x0008)!=0x0008)&&(B1==0)&&CounterB1==0)
	{
		RedButton=1;
		B1=RedButton;
		CounterB1=5;
	} else
	{
		RedButton=0;
		if (((GPIOB->IDR)&0x0008)==0x0008) {B1=0;};
	};
	//---
	if (CounterB2>=5) {CounterB2++; if (CounterB2==50) {CounterB2=0;};}
	if ((((GPIOB->IDR)&0x0020)!=0x0020)&&(B2==0)&&CounterB2==0)
	{
		GreenButton=1;
		B2=GreenButton;
		CounterB2=5;
	} else
	{
		GreenButton=0;
		if (((GPIOB->IDR)&0x0020)==0x0020) {B2=0;};
	};*/
	//Control currents up
	//if (ADCToIntCurrent(Current3V3,Zero3V3)>1100) {I_3V3Ideal=ZeroCurrent3V3;PWM_3V3=0;};
	//if (ADCToIntCurrent(Current5V,Zero5V)>800) {I_5VIdeal=ZeroCurrent5V;PWM_5V=0;};
	//if (ADCToIntCurrent(Current12V,Zero12V)>1600) {I_12VIdeal=ZeroCurrent12V;PWM_12V=0;};

}
//**************************************************************************************
void TIM7_IRQHandler(void)
{
	TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
	//Align Voltages
	/*Channel3V3.VBuffer+=Voltage3V3;
	Channel5V.VBuffer+=Voltage5V;
	Channel12V.VBuffer+=Voltage12V;
	Channel5VSB.VBuffer+=Voltage5ST;
	Power.VBuffer+=PowerVoltage;
	PowerOK.VBuffer+=PowerGoodVoltage;
	Counter++;
	if (Counter==100)
	{
		Counter=0;
		Channel3V3.V_Real=Channel3V3.VBuffer/100;   Channel3V3.VBuffer=0;
		Channel5V.V_Real=Channel5V.VBuffer/100;	    Channel5V.VBuffer=0;
		Channel12V.V_Real=Channel12V.VBuffer/100;   Channel12V.VBuffer=0;
		Channel5VSB.V_Real=Channel5VSB.VBuffer/100; Channel5VSB.VBuffer=0;
		Power.V_Real=Power.VBuffer/100;  			Power.VBuffer=0;
		PowerOK.V_Real=PowerOK.VBuffer/100;			PowerOK.VBuffer=0;
	}
	//AlignCurrents
	Channel3V3.IBuffer+=Current3V3;
	Channel5V.IBuffer+=Current5V;
	Channel12V.IBuffer+=Current12V;
	Counter2++;
	if (Counter2==100)
	{
		Counter2=0;
		Channel3V3.I_Real=Channel3V3.IBuffer/100; Channel3V3.IBuffer=0;
		Channel5V.I_Real=Channel5V.IBuffer/100;   Channel5V.IBuffer=0;
		Channel12V.I_Real=Channel12V.IBuffer/100; Channel12V.IBuffer=0;
	}*/
}
//**************************************************************************************
char* VarToString(uint32_t Var,char* ArrayVar)
{
	uint8_t j;
	ArrayVar[0]=Var/1000+0x30;
	Var-=1000*(ArrayVar[0]-0x30);
	ArrayVar[1]=Var/100+0x30;
	Var-=100*(ArrayVar[1]-0x30);
	ArrayVar[2]=Var/10+0x30;
	Var-=10*(ArrayVar[2]-0x30);
	ArrayVar[3]=Var/1+0x30;
	Var-=1*(ArrayVar[3]-0x30);
	ArrayVar[4]=0;
	j=0;
	while (ArrayVar[j]=='0')
	{
		ArrayVar[j]=0xFF;
		j++;
	}
	return ArrayVar;
}
//**************************************************************************************
int PressButton(int NumberButton)
{
	switch (NumberButton)
	{
		case  RED  : if (((GPIOB->IDR)&0x0008)!=0x0008) {return 1;} else {return 0;}; break;
		case GREEN : if (((GPIOB->IDR)&0x0020)!=0x0020) {return 1;} else {return 0;}; break;
	}
}
//**************************************************************************************
void ExitWithErrors(uint8_t NumberErrors)
{
	PowerOff;
	InitConstants();
	TIM1->CCR1=Channel3V3.PWM;
	TIM1->CCR2=Channel5V.PWM;
	TIM1->CCR3=Channel12V.PWM;
	Delay(300000);
	ClrScr();
	Delay(300000);
	WriteText(1,1," Error number: "); WriteText(16,1,VarToString(NumberErrors,BufferArray));
	while(1) {};
	//
};
//**************************************************************************************
void TIM2_IRQHandler(void)
{
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	switch (Scenario)
	{
		case scLogo: ShowLogo();
	}
}
//**************************************************************************************
