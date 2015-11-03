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
void Logo(void);
void Waiting(void);
void PreSupply(void);
void ACSupply(void);
void BatterySupply(void);
void BattteryCharging(void);
void SelectATXPower(void);
void PreTest(void);
void Test(void);
void LowBattery(void);
void WaitACPower(void);
void ExitTest(void);
void PreVSBTest(void);
void VSBTest(void);
void VSBTestFail(void);
void PreImpedanceTest(void);
void ImpedanceTest(void);
void FailImpedanceTest(void);
void FailImpedanceTestAdvice(void);
void PrePowerGoodTest(void);
void PowerGoodTest(void);
void FailPowerGoodTest(void);
void FailPowerGoodTestAdvice(void);
void VanishPowerGood();
void PreVoltageTest(void);
void VoltageTest(void);
void FailVoltageTest(void);
void FailVoltageTestAdvice(void);
void PreLoadTest(void);
void LoadTest(void);
void FailLoadTest(void);
void FailLoadTestAdvice(void);
void ATX_OK(void);
void FailPowerONTest(void);
void FailPowerONTestAdvice(void);
void TestCompleted(void);
void TestAborted(void);
void BeepTestPASSED(void);
void BeepTestFAILED(void);
//--------

//Array uses for out text on display
uint8_t BufferArray[7];

//**************************************************************************************
int main(void)
{
	RCC_Configuration();
	GPIO_Configuration();
	uint32_t i=1000000;
	while (i!=0) {i--;};
	InitLCD();
	ADC_Configuration();
	DMA_Configuration();
	TIM_Configuration();
	Scenario=scLogo;
	Beep.State3=1;
	while (1)
    {
		if (SystemSettings.TimerOn==1)
		{
			if ((Power.PowerTest==1)&&(ADCToIntVoltage(Power.V_Real,Power.R1,Power.R2)<11200)) {Scenario=scLowBattery;};
			if ((PowerOK.Monitoring==1)&&(ADCToIntVoltage(PowerOK.V_Real,PowerOK.R1,PowerOK.R2)<2400)) {Scenario=scVanishPowerGood;};
			if ((SystemSettings.IsTestStarted==1)&&(Button.C==1)) {Scenario=scTestAborted;};
			switch (Scenario)
			{
				case scLogo             		: Logo(); 			  		break;
				case scPreSupply        		: PreSupply(); 		  		break;
				case scACSupply	        		: ACSupply(); 		  		break;
				case scBatterySupply   			: BatterySupply(); 	  		break;
				case scBattteryCharging 		: BattteryCharging(); 		break;
				case scSelectATXPower   		: SelectATXPower();	  		break;
				case scPreTest		    		: PreTest();		  		break;
				case scTest			    		: Test();			  		break;
				case scWaiting	        		: Waiting(); 		  		break;
				case scLowBattery	    		: LowBattery();	      		break;
				case scWaitACPower      		: WaitACPower();	  		break;
				case scExitTest		    		: ExitTest();		  		break;
				case scPreVSBTest	    		: PreVSBTest();		  		break;
				case scVSBTest		    		: VSBTest();	      		break;
				case scVSBTestFail	    		: VSBTestFail();	  		break;
				case scPreImpedanceTest 		: PreImpedanceTest(); 		break;
				case scImpedanceTest    		: ImpedanceTest();	  		break;
				case scFailImpedanceTest		: FailImpedanceTest();		break;
				case scFailImpedanceTestAdvice  : FailImpedanceTestAdvice();break;
				case scPrePowerGoodTest			: PrePowerGoodTest();		break;
				case scPowerGoodTest			: PowerGoodTest();			break;
				case scFailPowerGoodTest		: FailPowerGoodTest();		break;
				case scFailPowerGoodTestAdvice	: FailPowerGoodTestAdvice();break;
				case scVanishPowerGood			: VanishPowerGood();		break;
				case scPreVoltageTest			: PreVoltageTest();			break;
				case scVoltageTest				: VoltageTest();			break;
				case scFailVoltageTest			: FailVoltageTest();		break;
				case scFailVoltageTestAdvice	: FailVoltageTestAdvice();	break;
				case scPreLoadTest				: PreLoadTest();			break;
				case scLoadTest					: LoadTest();				break;
				case scFailLoadTest				: FailLoadTest();			break;
				case scFailLoadTestAdvice		: FailLoadTestAdvice();		break;
				case scATX_OK					: ATX_OK();					break;
				case scFailPowerONTest			: FailPowerONTest();		break;
				case scFailPowerONTestAdvice	: FailPowerONTestAdvice();  break;
				case scTestCompleted			: TestCompleted();			break;
				case scTestAborted				: TestAborted();			break;
				case scBeepTestPASSED			: BeepTestPASSED();			break;
				case scBeepTestFAILED			: BeepTestFAILED();			break;
			}
			PollOfButtons();
			if (SystemSettings.IsRefreshLCD) {RefreshLCD();};
			SystemSettings.TimerOn=0;
		};
    }
}
//**************************************************************************************
void Logo(void)
{
	FullReset();
	WriteText(1,1,"   ATX PSU Tester   ");
	WriteText(1,2,"    Version 2.10    ");
	SystemSettings.Wait10ms=300;
	Scenario=scWaiting;
	PostScenario=scPreSupply;
	Beep.State3=0;
}
//**************************************************************************************
void PreSupply(void)
{
	if ((ADCToIntVoltage(PowerVoltage,Power.R1,Power.R2))>16000) {Scenario=scACSupply; Power.PowerTest=1;} else {Scenario=scBatterySupply;};
}
//**************************************************************************************
void ACSupply(void)
{
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
	uint16_t Power_mV;
	Power_mV=ADCToIntVoltage(Power.V_Real,Power.R1,Power.R2);
	if ((Power_mV>=11200)&&(Power_mV<15000))
	{
		Power.PowerTest=1;
		WriteText(1,1,"Battery charge: Norm");
		WriteText(1,2,"     Press <OK>     ");
		if (Button.OK==1) {Scenario=scSelectATXPower;};
	}
	if (Power_mV<11200)
	{
		WriteText(1,1,"Battery charge:  Low");
		WriteText(1,2," Please connect AC! ");
		Scenario=scWaitACPower;
	}
	if (Power_mV>=15000)
	{
		Power.PowerTest=1;
		Scenario=scACSupply;
	}
}
//**************************************************************************************
void SelectATXPower(void)
{
	WriteText(1,1,"  Select ATX Power  ");
	if (Button.C==1) {SystemSettings.PSUPower+=50;
	if (SystemSettings.PSUPower==500) {SystemSettings.PSUPower=250;};};
	switch (SystemSettings.PSUPower)
	{
		case 250: WriteText(1,2,"    <C> - [250W]    "); break;
		case 300: WriteText(1,2,"    <C> - [300W]    "); break;
		case 350: WriteText(1,2,"    <C> - [350W]    "); break;
		case 400: WriteText(1,2,"    <C> - [400W]    "); break;
		case 450: WriteText(1,2,"    <C> - [450W]    "); break;
	}
	if (Button.OK==1) {Scenario=scPreTest;};
}
//**************************************************************************************
void PreTest(void)
{
	WriteText(1,1,"  <OK> - Start test ");
	WriteText(1,2,"  <C>  - Back       ");
	if (Button.OK==1) {Scenario=scTest;};
	if (Button.C==1) {Scenario=scSelectATXPower;};
}
//**************************************************************************************
void Test(void)
{
	ClrScr();
	WriteText(1,1,"     Starting!      ");
	WriteText(1,2,"Do not power off ATX");
	Beep.State3=1;
	SystemSettings.Wait10ms=30;
	Scenario=scWaiting;
	PostScenario=scPreVSBTest;
	SystemSettings.IsTestStarted=1;
}
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
void PreVSBTest(void)
{
	if (Beep.State3==1)
	{
		Beep.State3=0;
		SystemSettings.Wait10ms=270;
		Scenario=scWaiting;
		PostScenario=scPreVSBTest;
		return;
	};
	ClrScr();
	Beep.State3=0;
	WriteText(1,1,"1.VSB test          ");
	SystemSettings.Wait10ms=150;
	Scenario=scWaiting;
	PostScenario=scVSBTest;
}
//**************************************************************************************
void VSBTest(void)
{
	if ((ADCToIntVoltage(Channel5VSB.V_Real,Channel5VSB.R1,Channel5VSB.R2)>4750)&&(ADCToIntVoltage(Channel5VSB.V_Real,Channel5VSB.R1,Channel5VSB.R2)<5250))
	{
		WriteText(1,2,"       PASSED       ");
		PostScenario=scPreImpedanceTest;
		Scenario=scBeepTestPASSED;
	} else
	{
		WriteText(1,2,"       FAILED       ");
		PostScenario=scVSBTestFail;
		Scenario=scBeepTestFAILED;
	}
	SystemSettings.Wait10ms=200;
}
//**************************************************************************************
void VSBTestFail(void)
{
	WriteText(1,1,"     VSB=           ");
	WriteText(10,1,ADCToVoltage(Channel5VSB.V_Real,BufferArray,Channel5VSB.R1,Channel5VSB.R2));
	WriteText(1,2," Check VSB circuit! ");
	Scenario=scExitTest;
}
//**************************************************************************************
void PreImpedanceTest(void)
{
	ClrScr();
	WriteText(1,1,"2.Impedance test    ");
	SystemSettings.Wait10ms=150;
	Scenario=scWaiting;
	PostScenario=scImpedanceTest;
	if ((ADCToIntVoltage(Channel3V3.V_Real,Channel3V3.R1,Channel3V3.R2)<100)&&(ADCToIntVoltage(Channel5V.V_Real,Channel5V.R1,Channel5V.R2)<100)&&(ADCToIntVoltage(Channel12V.V_Real,Channel12V.R1,Channel12V.R2)<100))
	{
		GPIO_WriteBit(GPIOC,GPIO_Pin_5,Bit_RESET);
	} else
	{
		GPIO_WriteBit(GPIOC,GPIO_Pin_5,Bit_SET);
	}
}
//**************************************************************************************
void ImpedanceTest(void)
{
	if (((ADCToIntResistance(Resistance12V)>5)&&(ADCToIntResistance(Resistance12V)<1000))&&((ADCToIntResistance(Resistance5V)>5)&&(ADCToIntResistance(Resistance5V)<1000))&&((ADCToIntResistance(Resistance3V3)>5)&&(ADCToIntResistance(Resistance3V3)<1000))&&((GPIOC->ODR&0x0010)==0x0000))
	{
		WriteText(1,2,"       PASSED       ");
		GPIO_WriteBit(GPIOC,GPIO_Pin_5,Bit_SET);
		Scenario=scBeepTestPASSED;
		PostScenario=scPrePowerGoodTest;
	} else
	{
		if ((GPIOC->ODR&0x0020)==0x0020)
		{
			WriteText(1,2,"       ERROR!       ");
			Scenario=scBeepTestFAILED;
			PostScenario=scFailPowerONTest;
		} else
		{
			WriteText(1,2,"       FAILED       ");
			Scenario=scBeepTestFAILED;
			PostScenario=scFailImpedanceTest;
		}
	}
	SystemSettings.Wait10ms=200;
}
//**************************************************************************************
void FailImpedanceTest(void)
{
	uint8_t i=0;
	ClrScr();
	if ((ADCToIntResistance(Resistance3V3)<5)||(ADCToIntResistance(Resistance3V3)>1000))
	{
		WriteText(6,1,"3V3=");
		WriteText(10,1,ADCToResistance(Resistance3V3,BufferArray));
		i++;
	}
	if ((ADCToIntResistance(Resistance5V)<5)||(ADCToIntResistance(Resistance5V)>1000))
	{
		switch (i)
		{
			case 0: WriteText(7,1,"5V="); WriteText(10,1,ADCToResistance(Resistance5V,BufferArray));break;
			case 1:
			{
				if ((ADCToIntResistance(Resistance12V)<5)||(ADCToIntResistance(Resistance12V)>1000))
				{
					WriteText(2,2,"5V="); WriteText(5,2,ADCToResistance(Resistance5V,BufferArray));
				} else
				{
					WriteText(7,2,"5V="); WriteText(10,2,ADCToResistance(Resistance5V,BufferArray));
				}
				break;
			}
		}
		i++;
	}
	if ((ADCToIntResistance(Resistance12V)<5)||(ADCToIntResistance(Resistance12V)>1000))
	{
		switch (i)
		{
			case 0: WriteText(6,1,"12V="); WriteText(10,1,ADCToResistance(Resistance12V,BufferArray));break;
			case 1: WriteText(6,2,"12V="); WriteText(10,2,ADCToResistance(Resistance12V,BufferArray));break;
			case 2: WriteText(11,2,"12V="); WriteText(15,2,ADCToResistance(Resistance12V,BufferArray));break;
		}
	}
	i=0;
	Scenario=scFailImpedanceTestAdvice;
	GPIO_WriteBit(GPIOC,GPIO_Pin_5,Bit_SET);
}
//**************************************************************************************
void FailImpedanceTestAdvice(void)
{
	if ((Button.OK==1)||(Button.C==1))
	{
		ClrScr();
		WriteText(1,1," Diode breakdown or ");
		WriteText(1,2,"capacitor breakdown!");
		Scenario=scExitTest;
	};
}
//**************************************************************************************
void PrePowerGoodTest(void)
{
	PowerOK.TimeOn=0;
	ClrScr();
	WriteText(1,1,"3.PowerGood test    ");
	PowerOn;
	PowerOK.IsCountTimeOn=1;
	Scenario=scPowerGoodTest;
}
//**************************************************************************************
void PowerGoodTest(void)
{
	if (PowerOK.IsCountTimeOn==0)
	{
		if ((PowerOK.TimeOn>100)&&(PowerOK.TimeOn<500))
		{
			WriteText(1,2,"       PASSED       ");
			Scenario=scBeepTestPASSED;
		} else
		{
			WriteText(1,2,"       FAILED       ");
			Scenario=scBeepTestFAILED;
		};
		SystemSettings.Wait10ms=200;
		PostScenario=scPreVoltageTest;
		return;
	}
	if ((PowerOK.TimeOn>100)&&(PowerOK.TimeOn<500)&&(ADCToIntVoltage(PowerGoodVoltage,PowerOK.R1,PowerOK.R2)>2400))
	{
		PowerOK.IsCountTimeOn=0;
		SystemSettings.Wait10ms=200;
		Scenario=scWaiting;
		PostScenario=scPowerGoodTest;
	};
	if ((PowerOK.TimeOn>1000)||(((PowerOK.TimeOn<100)&&(ADCToIntVoltage(PowerGoodVoltage,PowerOK.R1,PowerOK.R2)>2400))))
	{
		PowerOK.IsCountTimeOn=0;
		SystemSettings.Wait10ms=200;
		Scenario=scWaiting;
		PostScenario=scPowerGoodTest;
	}
}
//**************************************************************************************
void PreVoltageTest(void)
{
	ClrScr();
	WriteText(1,1,"4.Voltage test      ");
	SystemSettings.Wait10ms=150;
	Scenario=scWaiting;
	PostScenario=scVoltageTest;
}
//**************************************************************************************
void VoltageTest(void)
{
	if (((ADCToIntVoltage(Channel3V3.V_Real,Channel3V3.R1,Channel3V3.R2)>3100)&&(ADCToIntVoltage(Channel3V3.V_Real,Channel3V3.R1,Channel3V3.R2)<3500))&&
	   ((ADCToIntVoltage(Channel5V.V_Real,Channel5V.R1,Channel5V.R2)>4700)&&(ADCToIntVoltage(Channel5V.V_Real,Channel5V.R1,Channel5V.R2)<5500))&&
	   ((ADCToIntVoltage(Channel12V.V_Real,Channel12V.R1,Channel12V.R2)>11000)&&(ADCToIntVoltage(Channel12V.V_Real,Channel12V.R1,Channel12V.R2)<13100)))
	{
		WriteText(1,2,"       PASSED       ");
		Scenario=scBeepTestPASSED;
		if ((PowerOK.TimeOn>500)||(PowerOK.TimeOn<100))
		{
			PostScenario=scFailPowerGoodTest;
		}
		else
		{
			PostScenario=scPreLoadTest;
		}
	} else
	{
		WriteText(1,2,"       FAILED       ");
		Scenario=scBeepTestFAILED;
		PostScenario=scFailVoltageTest;
	}
	SystemSettings.Wait10ms=200;
}
//**************************************************************************************
void FailPowerGoodTest(void)
{
	ClrScr();
	if (PowerOK.TimeOn>1000)
	{
		WriteText(1,1,"No PowerGood signal!");
	} else
	{
		WriteText(1,1,"P.G. Rise Time=");
		WriteText(16,1,IntToTime_mS(PowerOK.TimeOn,BufferArray));
	}
	WriteText(1,2,"P.G. Voltage=");
	WriteText(14,2,ADCToVoltage(PowerOK.V_Real,BufferArray,PowerOK.R1,PowerOK.R2));
	PowerOff;
	Scenario=scFailPowerGoodTestAdvice;
}
//**************************************************************************************
void FailPowerGoodTestAdvice(void)
{
	if ((Button.OK==1)||(Button.C==1))
	{
		ClrScr();
		WriteText(1,1,"  Check PowerGood   ");
		WriteText(1,2,"circuit or PWM chip!");
		Scenario=scExitTest;
	};
}
//**************************************************************************************
void FailVoltageTest(void)
{
	uint8_t i=0;
	ClrScr();
	if ((ADCToIntVoltage(Channel3V3.V_Real,Channel3V3.R1,Channel3V3.R2)<3100)||(ADCToIntVoltage(Channel3V3.V_Real,Channel3V3.R1,Channel3V3.R2)>3500))
	{
		WriteText(6,1,"3V3=");
		WriteText(10,1,ADCToVoltage(Channel3V3.V_Real,BufferArray,Channel3V3.R1,Channel3V3.R2));
		i++;
	}
	if ((ADCToIntVoltage(Channel5V.V_Real,Channel5V.R1,Channel5V.R2)<4700)||(ADCToIntVoltage(Channel5V.V_Real,Channel5V.R1,Channel5V.R2)>5500))
	{
		switch (i)
		{
			case 0: WriteText(7,1,"5V="); WriteText(10,1,ADCToVoltage(Channel5V.V_Real,BufferArray,Channel5V.R1,Channel5V.R2));break;
			case 1:
			{
				if ((ADCToIntVoltage(Channel12V.V_Real,Channel12V.R1,Channel12V.R2)<11000)||(ADCToIntVoltage(Channel12V.V_Real,Channel12V.R1,Channel12V.R2)>13100))
				{
					WriteText(1,2,"5V="); WriteText(4,2,ADCToVoltage(Channel5V.V_Real,BufferArray,Channel5V.R1,Channel5V.R2));
				} else
				{
					WriteText(7,2,"5V="); WriteText(10,2,ADCToVoltage(Channel5V.V_Real,BufferArray,Channel5V.R1,Channel5V.R2));
				}
				break;
			}
		}
		i++;
	}
	if ((ADCToIntVoltage(Channel12V.V_Real,Channel12V.R1,Channel12V.R2)<11000)||(ADCToIntVoltage(Channel12V.V_Real,Channel12V.R1,Channel12V.R2)>13100))
	{
		switch (i)
		{
			case 0: WriteText(6,1,"12V="); WriteText(10,1,ADCToVoltage(Channel12V.V_Real,BufferArray,Channel12V.R1,Channel12V.R2));break;
			case 1: WriteText(6,2,"12V="); WriteText(10,2,ADCToVoltage(Channel12V.V_Real,BufferArray,Channel12V.R1,Channel12V.R2));break;
			case 2: WriteText(11,2,"12V="); WriteText(15,2,ADCToVoltage(Channel12V.V_Real,BufferArray,Channel12V.R1,Channel12V.R2));break;
		}
	}
	i=0;
	PowerOff;
	Scenario=scFailVoltageTestAdvice;
}
//**************************************************************************************
uint8_t CounterAdviceFailVoltag=0;
void FailVoltageTestAdvice(void)
{
	if (((Button.OK==1)||(Button.C==1))&&(CounterAdviceFailVoltag==0))
	{

		ClrScr();
		CounterAdviceFailVoltag++;
		WriteText(1,1,"  Diode breakdown,  ");
		WriteText(1,2," electrolyte drying,");
		return;
	};
	if (((Button.OK==1)||(Button.C==1))&&(CounterAdviceFailVoltag==1))
	{
		CounterAdviceFailVoltag=0;
		ClrScr();
		WriteText(1,1," check PWM circuit, ");
		WriteText(1,1,"  feedback control. ");
		Scenario=scExitTest;
	}
}
//**************************************************************************************
void PreLoadTest(void)
{
	ClrScr();
	WriteText(1,1,"5.Load test         ");
	SystemSettings.Wait10ms=150;
	Scenario=scWaiting;
	PostScenario=scLoadTest;
	PowerOK.Monitoring=1;
	SystemSettings.IsControlCurrent=1;
	SystemSettings.PercentLoad=0;
}
//**************************************************************************************
uint32_t PowerATX=0;
uint16_t a=0,b=0,c=0;
void LoadTest(void)
{
	SystemSettings.Wait10ms=50;
	Scenario=scWaiting;
	PostScenario=scLoadTest;
	PercentToCurrent(SystemSettings.PercentLoad);

	SystemSettings.PercentLoad++;

	PowerATX=(ADCToIntVoltage(Channel3V3.V_Real,Channel3V3.R1,Channel3V3.R2)*(ADCToIntCurrent(Channel3V3.I_Real,Channel3V3.VZero)*10))/1000;
	PowerATX+=(ADCToIntVoltage(Channel5V.V_Real,Channel5V.R1,Channel5V.R2)*(ADCToIntCurrent(Channel5V.I_Real,Channel5V.VZero)*10))/1000;
	PowerATX+=(ADCToIntVoltage(Channel12V.V_Real,Channel12V.R1,Channel12V.R2)*(ADCToIntCurrent(Channel12V.I_Real,Channel12V.VZero)*10))/1000;

	WriteText(16,1,IntToPower(PowerATX,BufferArray));

	WriteChar(SystemSettings.PercentLoad/5,2,'>');

	if (SystemSettings.PercentLoad==100)
	{
		SystemSettings.Wait10ms=1000;
	}

	if (SystemSettings.PercentLoad>20)
	{
		if (((ADCToIntVoltage(Channel3V3.V_Real,Channel3V3.R1,Channel3V3.R2)>3140)&&(ADCToIntVoltage(Voltage3V3,Channel3V3.R1,Channel3V3.R2)<3470))&&
		   ((ADCToIntVoltage(Channel5V.V_Real,Channel5V.R1,Channel5V.R2)>4750)&&(ADCToIntVoltage(Voltage5V,Channel5V.R1,Channel5V.R2)<5250))&&
		   ((ADCToIntVoltage(Channel12V.V_Real,Channel12V.R1,Channel12V.R2)>11400)&&(ADCToIntVoltage(Voltage12V,Channel12V.R1,Channel12V.R2)<12650)))
		{
			if (SystemSettings.PercentLoad==101)
			{
				WriteText(1,2,"       PASSED       ");
				SystemSettings.Wait10ms=150;
				Scenario=scBeepTestPASSED;
				PowerOK.Monitoring=0;
				PostScenario=scATX_OK;
			};
		} else
		{
			PowerOK.Monitoring=0;
			a=Channel3V3.V_Real;
			b=Channel5V.V_Real;
			c=Channel12V.V_Real;
			PercentToCurrent(0);
			PowerOff;
			WriteText(1,2,"       FAILED       ");
			SystemSettings.Wait10ms=100;
			Scenario=scBeepTestFAILED;
			PostScenario=scFailLoadTest;
		};
	};
}
//**************************************************************************************
void VanishPowerGood()
{
	PowerOff;
	PercentToCurrent(0);
	PowerOK.Monitoring=0;
	ClrScr();
	WriteText(1,1," Vanish PowerGood!  ");
	PostScenario=scFailPowerGoodTestAdvice;
	SystemSettings.Wait10ms=150;
	Scenario=scWaiting;
}
//**************************************************************************************
void FailLoadTest(void)
{
	ClrScr();
	WriteText(1,1,"    Power:  ");
	WriteText(13,1,IntToPower(PowerATX,BufferArray));
	WriteText(1,2,ADCToVoltage(a,BufferArray,Channel3V3.R1,Channel3V3.R2));
	WriteText(8,2,ADCToVoltage(b,BufferArray,Channel5V.R1,Channel5V.R2));
	WriteText(15,2,ADCToVoltage(c,BufferArray,Channel12V.R1,Channel12V.R2));
	Scenario=scFailLoadTestAdvice;
}
//**************************************************************************************
void FailLoadTestAdvice(void)
{
	if ((Button.OK==1)||(Button.C==1))
	{
		ClrScr();
		WriteText(1,1," Voltage aberration ");
		WriteText(1,2,"      on            ");
		WriteText(10,2,IntToPower(PowerATX,BufferArray));
		Scenario=scExitTest;
	};
}
//**************************************************************************************
void ATX_OK(void)
{
	SystemSettings.IsTestStarted=0;
	ClrScr();
	if (SystemSettings.PercentLoad>0) {SystemSettings.PercentLoad--;};
	PercentToCurrent(SystemSettings.PercentLoad);
	WriteText(1,1,"  Test completing   ");
	WriteText(1,2,"   wait please...   ");
	if (SystemSettings.PercentLoad==0)
	{
		PostScenario=scTestCompleted;
		SystemSettings.Wait10ms=SystemSettings.PSUPower;
		Scenario=scWaiting;
	}
}
//**************************************************************************************
void TestCompleted(void)
{
	SystemSettings.IsControlCurrent=0;
	ClrScr();
	PowerOff;
	WriteText(1,1,"   Test completed   ");
	WriteText(1,2,"       ATX OK       ");
	if ((Button.OK==1)||(Button.C==1)) {Scenario=scSelectATXPower;};
};
//**************************************************************************************
void FailPowerONTest(void)
{
	ClrScr();
	WriteText(1,1,"ATX already started!");
	SystemSettings.Wait10ms=200;
	Scenario=scWaiting;
	PostScenario=scFailPowerONTestAdvice;
}
//**************************************************************************************
void FailPowerONTestAdvice(void)
{
	if ((Button.OK==1)||(Button.C==1))
	{
		ClrScr();
		WriteText(1,1,"--------------------");
		WriteText(1,2,"--------------------");
		Scenario=scExitTest;
	};
}
//**************************************************************************************
uint8_t Tick=0;
void BeepTestPASSED(void)
{
	Tick++;
	if (Tick==1)
	{
		Beep.State3=1;
	}
	if (Tick==5)
	{
		Beep.State3=0;
		Tick=0;
		Scenario=scWaiting;
	}
}
//**************************************************************************************
void BeepTestFAILED(void)
{
	Tick++;
	if ((Tick%10)==0)
	{
		Beep.State3=Beep.State3^(1<<0);
		if (Tick==60)
		{
			Beep.State3=0;
			Tick=0;
			Scenario=scWaiting;
		}
	}
}
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
void TestAborted(void)
{
	SystemSettings.IsTestStarted=0;
	GPIO_WriteBit(GPIOC,GPIO_Pin_5,Bit_SET);
	PowerOff;
	PercentToCurrent(0);
	AlignCurrents();
	Power.PowerTest=0;
	PowerOK.Monitoring=0;
	SystemSettings.IsControlCurrent=0;
	ClrScr();
	WriteText(1,1,"   Test aborted!    ");
	SystemSettings.Wait10ms=150;
	Scenario=scBeepTestFAILED;
	PostScenario=scSelectATXPower;
}
//**************************************************************************************
void ExitTest(void)
{
	SystemSettings.IsTestStarted=0;
	SystemSettings.IsControlCurrent=0;
	if ((Button.OK==1)||(Button.C==1))
	{
		ClrScr();
		WriteText(1,1,"   Test complete!   ");
		SystemSettings.Wait10ms=200;
		Scenario=scWaiting;
		PostScenario=scSelectATXPower;
	};
}
//**************************************************************************************
void WaitACPower(void)
{
	Beep.Counter++;
	if (Beep.Counter==8)
	{
		Beep.Counter=0;
		Beep.State3=Beep.State3^(1<<0);
	};
	if (ADCToIntVoltage(Power.V_Real,Power.R1,Power.R2)>11200)
	{
		SystemSettings.Wait10ms=50;
		Scenario=scWaiting;
		PostScenario=scBatterySupply;
		Beep.State3=0;
	};
}
//**************************************************************************************
void LowBattery(void)
{
	GPIO_WriteBit(GPIOC,GPIO_Pin_5,Bit_SET);
	PowerOff;
	PercentToCurrent(0);
	AlignCurrents();
	Power.PowerTest=0;
	PowerOK.Monitoring=0;
	SystemSettings.IsControlCurrent=0;
	SystemSettings.Wait10ms=50;
	Scenario=scWaiting;
	PostScenario=scBatterySupply;
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
	if (SystemSettings.IsControlCurrent==1) {AlignCurrents();};
	if (PowerOK.IsCountTimeOn==1) {PowerOK.TimeOn++;};
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
	if (Beep.State1+Beep.State2+Beep.State3>0) {GPIOB->ODR=GPIOB->ODR^(1<<0);} else {GPIOB->ODR=(GPIOB->ODR&0xFFFE);};
	SystemSettings.DelayScenario++;
	if (SystemSettings.DelayScenario==19)
	{
		if (SystemSettings.TimerOn==0) {SystemSettings.TimerOn=1;};
		SystemSettings.DelayScenario=0;
	};

}
//**************************************************************************************
