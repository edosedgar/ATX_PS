#include <stm32f10x_gpio.h>
#include <InitPeripheral.c>
#include <Structures.c>
#include <Measurement.c>
#include <HD44780LCD.c>
#include <stm32f10x_tim.h>
#include <stm32f10x.h>

void TIM6_DAC_IRQHandler(void);
void TIM7_IRQHandler(void);
void TIM2_IRQHandler(void);
//Scenario
void PreLogo(void);
void Logo(void);
void Waiting(void);
void PreSupply(void);
void ACSupply(void);
void BatterySupply(void);
void BattteryCharging(void);
void SelectATXPower(void);
void SelectTestMode(void);
void PreTest(void);
void Test(void);
void LowBattery(void);
void WaitAC(void);
//--------

//Array uses for out text on display
uint8_t BufferArray[7];

//**************************************************************************************
int main(void)
{
	RCC_Configuration();
	GPIO_Configuration();
	InitLCD();
	ADC_Configuration();
	DMA_Configuration();
	TIM_Configuration();
	Scenario=scProgramReset;
	PostScenario=scLogo;
	while (1)
    {
		if (SystemSettings.IsRefreshLCD) {RefreshLCD();};
    }
}
//**************************************************************************************
void ProgramReset(void)
{
	FullReset();
	Scenario=PostScenario;
}
//**************************************************************************************
void Logo(void)
{
	WriteText(1,1,"   ATX PSU Tester   ");
	WriteText(1,2,"    Version 2.00    ");
	SystemSettings.Wait10ms=300;
	Scenario=scWaiting;
	PostScenario=scPreSupply;
}
//**************************************************************************************
void PreSupply(void)
{
	if ((ADCToIntVoltage(PowerVoltage,Power.R1,Power.R2))>14000) {Scenario=scACSupply;} else {Scenario=scBatterySupply;};
}
//**************************************************************************************
void ACSupply(void)
{
	Power.PowerTest=1;
	WriteText(1,1,"         AC         ");
	WriteText(1,2,"       supply       ");
	SystemSettings.Wait10ms=300;
	Scenario=scWaiting;
	PostScenario=scBattteryCharging;
}
//**************************************************************************************
void BattteryCharging(void)
{
	WriteText(1,1,"Battery charging");
	SystemSettings.ShowingCharging++;
	switch ((SystemSettings.ShowingCharging/64))
	{
		case 0: WriteText(18,1,">  "); break;
		case 1: WriteText(18,1,">> "); break;
		case 2: WriteText(18,1,">>>"); break;
		case 3: WriteText(18,1,"   "); break;
	}
	WriteText(1,2,"     Press <OK>     ");
	if (Button.OK==1) {Scenario=scSelectATXPower;};
}
//**************************************************************************************
void BatterySupply(void)
{
	int PercentCharge;
	PercentCharge=(13800-ADCToIntVoltage(PowerVoltage,Power.R1,Power.R2))/26;
	if (PercentCharge<0) {PercentCharge=100;} else PercentCharge=100-PercentCharge;
	if (PercentCharge<0) {PercentCharge=0;};
	WriteText(1,1,"Battery charge:");
	WriteText(16,1,IntToString(PercentCharge,BufferArray)); WriteChar(20,1,'%');
	if (PercentCharge>10) {WriteText(1,2,"     Press <OK>     "); if (Button.OK==1) {Scenario=scSelectATXPower;};}
	else
	{
		WriteText(1,2," Please connect AC! ");
		Scenario=scWaitAC;
	}
}
//**************************************************************************************
void SelectATXPower(void)
{
	WriteText(1,1,"  Select ATX Power  ");
	if (Button.Select==1) {SystemSettings.PSUPower+=50; if (SystemSettings.PSUPower==500) {SystemSettings.PSUPower=250;};};
	switch (SystemSettings.PSUPower)
	{
		case 250: WriteText(1,2,"       [250W]       "); break;
		case 300: WriteText(1,2,"       [300W]       "); break;
		case 350: WriteText(1,2,"       [350W]       "); break;
		case 400: WriteText(1,2,"       [400W]       "); break;
		case 450: WriteText(1,2,"       [450W]       "); break;
	}
	if (Button.OK==1) {Scenario=scSelectTestMode;};
}
//**************************************************************************************
void SelectTestMode(void)
{
	WriteText(1,1,"  Select test mode  ");
	if (Button.Select==1) {SystemSettings.TypeUser++; if (SystemSettings.TypeUser==2) {SystemSettings.TypeUser=0;};};
	switch (SystemSettings.TypeUser)
	{
		case 0: WriteText(1,2,"      [Normal]      "); break;
		case 1: WriteText(1,2,"      [Expert]      "); break;
	}
	if (Button.OK==1) {Scenario=scPreTest;};
}
//**************************************************************************************
void PreTest(void)
{
	WriteText(1,1,"  <OK> - Start Test ");
	WriteText(1,2,"<Select> - Settings ");
	if (Button.OK==1) {Scenario=scTest;};
	if (Button.Select==1) {Scenario=scSelectATXPower;};
	//
}
//**************************************************************************************
void Test(void)
{
	ClrScr();
	//
}
//**************************************************************************************

//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
void LowBattery(void)
{
	Power.PowerTest=0;
	FullReset();
	Scenario=scBatterySupply;
}
//**************************************************************************************
void WaitAC(void)
{
	if ((ADCToIntVoltage(PowerVoltage,Power.R1,Power.R2))>14000) {Scenario=scACSupply;};
}
//**************************************************************************************
void Waiting(void)
{
	//Waiting Between Scenario
	if (SystemSettings.Wait10ms--<2) {Scenario=PostScenario;};
}
//***************************************************************************************************************************************************************************************
void TIM6_DAC_IRQHandler(void)
{
	TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
	//ControlCurrents
}
//**************************************************************************************
void TIM7_IRQHandler(void)
{
	TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
	CalculateAverageAV();
}
//**************************************************************************************

void TIM2_IRQHandler(void)
{
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	if ((Power.PowerTest==1)&&(ADCToIntVoltage(PowerVoltage,Power.R1,Power.R2)<11200)) {Scenario=scLowBattery;};
	switch (Scenario)
	{
		case scProgramReset    : ProgramReset();      break;
		case scLogo            : Logo(); 			  break;
		case scPreSupply       : PreSupply(); 		  break;
		case scACSupply	       : ACSupply(); 		  break;
		case scBatterySupply   : BatterySupply(); 	  break;
		case scBattteryCharging: BattteryCharging();  break;
		case scSelectATXPower  : SelectATXPower();	  break;
		case scSelectTestMode  : SelectTestMode();    break;
		case scPreTest		   : PreTest();			  break;
		case scTest			   : Test();			  break;
		case scWaiting	       : Waiting(); 		  break;
		case scLowBattery	   : LowBattery();	      break;
		case scWaitAC		   : WaitAC();			  break;
	}
	PollOfButtons();
}
//**************************************************************************************
