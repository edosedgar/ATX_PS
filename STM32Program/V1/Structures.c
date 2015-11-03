typedef struct
{
	uint16_t VZero;
	uint16_t MinCurrentDetect;
	uint16_t ZeroCurrent;
	uint16_t R1;
	uint16_t R2;
	uint16_t PWM;
	uint32_t I_Real;
	uint32_t I_Ideal;
	uint32_t IBuffer;
	uint32_t V_Real;
	uint32_t VBuffer;
} TypeChannel3V3;

typedef struct
{
	uint16_t VZero;
	uint16_t MinCurrentDetect;
	uint16_t ZeroCurrent;
	uint16_t R1;
	uint16_t R2;
	uint16_t PWM;
	uint32_t I_Real;
	uint32_t I_Ideal;
	uint32_t IBuffer;
	uint32_t V_Real;
	uint32_t VBuffer;
} TypeChannel5V;

typedef struct
{
	uint16_t VZero;
	uint16_t MinCurrentDetect;
	uint16_t ZeroCurrent;
	uint16_t R1;
	uint16_t R2;
	uint16_t PWM;
	uint32_t I_Real;
	uint32_t I_Ideal;
	uint32_t IBuffer;
	uint32_t V_Real;
	uint32_t VBuffer;
} TypeChannel12V;

typedef struct
{
	uint16_t R1;
	uint16_t R2;
	uint32_t V_Real;
	uint32_t VBuffer;
	uint8_t  PowerNorm;
	uint8_t PowerTest;
} TypePower;

typedef struct
{
	uint16_t R1;
	uint16_t R2;
	uint32_t V_Real;
	uint32_t VBuffer;
	uint16_t TimeOn;
	uint8_t Monitoring;
} TypePOK;

typedef struct
{
	uint16_t R1;
	uint16_t R2;
	uint32_t V_Real;
	uint32_t VBuffer;
} TypeChannel5VSB;

typedef struct
{
	uint16_t PSUPower;
	uint8_t TypeUser;
	uint8_t StartTest;
	uint8_t TestOut;
	uint8_t TimeCountOn;
} TypeSystemSettings;
//*******************************************************************
void InitConstants(void);
//*******************************************************************
TypeChannel3V3  Channel3V3;
TypeChannel5V   Channel5V;
TypeChannel12V  Channel12V;
TypePower 	    Power;
TypePOK		    PowerOK;
TypeChannel5VSB	Channel5VSB;
TypeSystemSettings SystemSettings;
//*******************************************************************
void InitConstants(void)
{
	//Init Struct Channel3V3
	Channel3V3.VZero=2520;
	Channel3V3.MinCurrentDetect=3110;
	Channel3V3.ZeroCurrent=3127;
	Channel3V3.R1=14950;
	Channel3V3.R2=15020;
	Channel3V3.PWM=0;
	Channel3V3.I_Ideal=Channel3V3.ZeroCurrent;
	Channel3V3.I_Real=Channel3V3.I_Ideal;
	Channel3V3.IBuffer=0;
	Channel3V3.V_Real=0;
	Channel3V3.VBuffer=0;

	//Init Struct Channel5V
	Channel5V.VZero=2517;
	Channel5V.MinCurrentDetect=3110;
	Channel5V.ZeroCurrent=3123;
	Channel5V.R1=15010;
	Channel5V.R2=7510;
	Channel5V.PWM=0;
	Channel5V.I_Ideal=Channel5V.ZeroCurrent;
	Channel5V.I_Real=Channel5V.I_Ideal;
	Channel5V.IBuffer=0;
	Channel5V.V_Real=0;
	Channel5V.VBuffer=0;

	//Init Struct Channel12V
	Channel12V.VZero=2513;
	Channel12V.MinCurrentDetect=3100;
	Channel12V.ZeroCurrent=3118;
	Channel12V.R1=14990;
	Channel12V.R2=3590;
	Channel12V.PWM=0;
	Channel12V.I_Ideal=Channel12V.ZeroCurrent;
	Channel12V.I_Real=Channel12V.I_Ideal;
	Channel12V.IBuffer=0;
	Channel12V.V_Real=0;
	Channel12V.VBuffer=0;

	//Init Struct Power
	Power.R1=15100;
	Power.R2=3590;
	Power.V_Real=0;
	Power.VBuffer=0;
	Power.PowerNorm=0;
	Power.PowerTest=0;

	//Init Struct PowerOK
	PowerOK.R1=15000;
	PowerOK.R2=15000;
	PowerOK.V_Real=0;
	PowerOK.VBuffer=0;
	PowerOK.TimeOn=0;
	PowerOK.Monitoring=0;

	//Init Struct Channel5VSB
	Channel5VSB.R1=15100;
	Channel5VSB.R2=7510;
	Channel5VSB.V_Real=0;
	Channel5VSB.VBuffer=0;

	//Init System settings
	SystemSettings.StartTest=0;
	SystemSettings.TimeCountOn=0;
	SystemSettings.TestOut=0;
	SystemSettings.PSUPower=250;
	SystemSettings.TypeUser=0;
}
//**************************************************************************************
