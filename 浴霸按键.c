#include "global.h"

//中断函数中间变量
uchar ABuf;
uchar StatusBuf;
uchar FSR_TEMP;

//系统中间变量
uchar LightTimeCount;      	   	   	   	   	   	//LED指示动态扫描
uchar TimeMsCount;
uchar TimeMs;
uchar Time10MsCount;
uchar Time100MsCount;  	   	   	   	   	   	   	//秒计时
uchar DelayCount1;
uint DelayCount;   	   	   	   	   	   	   	//延时发送指令计时
uint FlashCount;   	   	   	   	   	   	   	//自然风冷却灯闪烁延时计数
uint DataSend;
uint CommandData; 
   	 
void main(void)
{
   	BoardInit();
   	
   	while (1)
   	{
   	   	
   	   	if (sys_status_lithtflg == TRUE
   	   	|| sys_status_nithtflg == TRUE
   	   	|| sys_status_airchange == TRUE
   	   	|| sys_status_smartkey == TRUE  
   	   	|| sys_status_warmkey == TRUE
   	   	|| sys_status_stwarm == TRUE
   	   	|| sys_Colling_flg == TRUE
   	   	|| sys_status_natureair == TRUE)
   	   	{
   	   	   	LedFlag();
   	   	}
   	   	else 
   	   	{
   	   	   	Waittingflash();
   	   	}
   	} 
}


void  Clr_Ram(void)
{
       	__asm
ClrRam:
       	movai  	0x00
       	movra  	FSR

ClrRam_Loop:
       	clrr   	INDF
   	   	incr    FSR
   	   	movai  	0xff
       	rsubar 	FSR    	       	       	
       	movra  	_FSR_TEMP
       	incr   	_FSR_TEMP
       	djzr   	_FSR_TEMP      	       	
       	goto   	ClrRam_Loop
       	       	
    __endasm;
}

void BoardInit(void)
{
   	//清除RAM
   	Clr_Ram();

   	OEP0 = 0XFF;
   	OEP1 = 0XFB;
   	//端口数据配置
   	IOP0 = 0X00;
   	IOP1 = 0X00;
   	//上拉电阻配置
   	PUP0 = 0X03;   	   	   	//P00,P01开漏输出
   	PDP1 = 0X00;   	   	   	//P13开漏输出
   	//普通驱动能力16mA
   	DRVCR = 0x80; 
   	//T1初始化
   	T1LOAD = 250;
   	T1CR   = 0X8F;     	   	//HCLK,128分频,1MS

   	//T0初始化
   	T0LOAD = 160;
   	T0CR   = 0X8B;     	   	//HCLK,8分频,40uS
   	
   	
   	//使能中断
   	INTE  = 0x03;  	   	   	//使能T1,T0
   	GIE   = TRUE;  	   	   	//开总中断
   	
   	DH1_LOW();
   	DH2_HIGH();
   	DH3_LOW();
   	DH4_HIGH();
   	KH1_HIGH();
   	KH2_HIGH();
   	Delay_10ms(100);
}

void KeyScan(void)
{
   	static uchar Index;
   	static uchar KeyCount;
   	
   	DH_LOW();  	   	   	   	   	   	   	   	   	//关闭行扫描 
   	LINE_LOW();    	
   	LINEIN(); 
   	KH1_HIGH();
   	KH2_HIGH();	
   	
   	if 	((LINE1 == HIGH) || (LINE2 == HIGH) || (LINE3 == HIGH))    	   	   	   	   	   	   	   	   	   	
   	{
   	   	if (Index < 3)
   	   	{
   	   	   	if (!KeyFlag_Keystat)
   	   	   	{
   	   	   	   	if (Index == 1)
   	   	   	   	{
   	   	   	   	   	KeyFlag_Keystat = TRUE;	//有按键按下标志
   	   	   	   	   	KH1_HIGH();
   	   	   	   	   	KH2_LOW();
   	   	   	   	   	
   	   	   	   	   	if (ROWKEY1 == HIGH)
   	   	   	   	   	{
   	   	   	   	   	   	if (LINE1 == HIGH)
   	   	   	   	   	   	{
   	   	   	   	   	   	   	DataSend = LightKey;
   	   	   	   	   	   	}
   	   	   	   	   	   	else if (LINE2 == HIGH)
   	   	   	   	   	   	{
   	   	   	   	   	   	   	DataSend = NightLightKey;
   	   	   	   	   	   	}
   	   	   	   	   	   	else if (LINE3 == HIGH)
   	   	   	   	   	   	{
   	   	   	   	   	   	   	
   	   	   	   	   	   	   	DataSend = SmartKey;
   	   	   	   	   	   	}
   	   	   	   	   	}
   	   	   	   	   	
   	   	   	   	   	KH1_LOW();
   	   	   	   	   	KH2_HIGH();
   	   	   	   	   	
   	   	   	   	   	if (ROWKEY2 == HIGH)
   	   	   	   	   	{
   	   	   	   	   	   	if (LINE1 == HIGH)
   	   	   	   	   	   	{
   	   	   	   	   	   	   	DataSend = ChangeAirKey;
   	   	   	   	   	   	}
   	   	   	   	   	   	else if (LINE2 == HIGH)
   	   	   	   	   	   	{
   	   	   	   	   	   	   	if (FlashCount != 0)
   	   	   	   	   	   	   	{
   	   	   	   	   	   	   	   	sys_Colling_flg = !sys_Colling_flg;
   	   	   	   	   	   	   	}
   	   	   	   	   	   	   	DataSend = NatureAirKey;
   	   	   	   	   	   	}
   	   	   	   	   	   	else if (LINE3 == HIGH)
   	   	   	   	   	   	{
   	   	   	   	   	   	   	if (sys_status_stwarm == TRUE)
   	   	   	   	   	   	   	{
   	   	   	   	   	   	   	   	sys_status_stwarm = FAULT;
   	   	   	   	   	   	   	   	sys_status_warmkey = FAULT;
   	   	   	   	   	   	   	   	sys_status_natureair = FAULT;
   	   	   	   	   	   	   	   	FlashCount = 1000; 	   	   	   	//1000ms
   	   	   	   	   	   	   	   	sys_Colling_flg = TRUE;
   	   	   	   	   	   	   	}
   	   	   	   	   	   	   	DataSend = WarmKey;
   	   	   	   	   	   	}
   	   	   	   	   	}
   	   	   	   	}
   	   	   	}
   	   	   	
   	   	   	Index++;   	   	
   	   	}
   	   	else
   	   	{
   	   	   	Index = 0;
   	   	}
   	}
   	else
   	{
   	   	KeyFlag_Keystat = FAULT;
   	}
   	LINEOUT();
}

void CmdSend(uint Command)
{
   	static uchar SendTimeCount;
   	static uchar Status;
   	static uchar DataTemp;
   	static uchar DataCount;    	   	   	   	   	   	   	
   	
   	SendTimeCount++;
   	
   	switch (Status)
   	{
   	   	case 0:
   	   	
   	   	DATAOUT = HIGH;
   	   	if (SendTimeCount == 237)  	   	   	   	//9.44ms高电平236
   	   	{
   	   	   	SendTimeCount = 0;
   	   	   	DATAOUT = LOW;
   	   	   	Status++;
   	   	}
   	   	
   	   	break;
   	   	
   	   	case 1:
   	   	
   	   	if (SendTimeCount == 121)  	   	   	   	//4.8ms低电平 	120    	
   	   	{
   	   	   	SendTimeCount = 0;
   	   	   	DATAOUT = HIGH;
   	   	   	Status++;
   	   	   	CommandData = FramHead;
   	   	}
   	   	
   	   	break;
   	   	
   	   	case 2:
   	   	
   	   	if (SendTimeCount == 16)
   	   	{
   	   	   	SendTimeCount = 0;
   	   	   	DATAOUT = LOW;
   	   	   	Status++;
   	   	}

   	   	break;
   	   	
   	   	case 3:	
   	   	
   	   	if (DataCount == 0)
   	   	{
   	   	   	DataTemp =(uchar)(CommandData >> 8);
   	   	}
   	   	else if (DataCount == 8)
   	   	{
   	   	   	DataTemp = CommandData;
   	   	}
   	   	   	
   	   	if ((DataTemp & 0X80) == 0X80)
   	   	{
   	   	   	if (SendTimeCount == 16)   	   	   	//640us
   	   	   	{
   	   	   	   	SendTimeCount = 0;
   	   	   	   	DATAOUT = HIGH;	
   	   	   	   	DataTemp <<= 1;
   	   	   	   	Status--;
   	   	   	   	DataCount++;
   	   	   	}
   	   	}
   	   	
   	   	if((DataTemp & 0X80) == 0x00)
   	   	{
   	   	   	if (SendTimeCount == 47)   	   	   	//1.84ms  46
   	   	   	{
   	   	   	   	SendTimeCount = 0;
   	   	   	   	DATAOUT = HIGH;	
   	   	   	   	DataTemp <<= 1;
   	   	   	   	Status--;
   	   	   	   	DataCount++;
   	   	   	}
   	   	}
   	   	
   	   	if (DataCount == 16)
   	   	{
   	   	   	DataCount = 0;
   	   	   	Status = 4;
   	   	}
   	   	
   	   	break;
   	   	
   	   	case 4:	   	

   	   	if (SendTimeCount == 16)
   	   	{
   	   	   	
   	   	   	SendTimeCount = 0;
   	   	   	
   	   	   	if (sys_data_flag)
   	   	   	{
   	   	   	   	sys_data_flag = !sys_data_flag;
   	   	   	   	Status = 0;
   	   	   	   	DataCount = 0; 
   	   	   	   	DATAOUT = LOW; 
   	   	   	   	DataSend = 0;
   	   	   	}
   	   	   	else
   	   	   	{
   	   	   	   	sys_data_flag = !sys_data_flag;
   	   	   	   	CommandData = Command;
   	   	   	   	DATAOUT = LOW;
   	   	   	   	Status--;
   	   	   	}
   	   	   	
   	   	}
   	   	
   	   	break;
   	   	
   	   	default:
   	   	
   	   	break;
   	   	
   	}
}

void Waittingflash(void)
{
   	if (TimeMsCount <= 0)
   	{
   	   	DH1_HIGH();
   	   	DH2_LOW();
   	   	DH3_HIGH();
   	   	DH4_LOW();
   	   	LINE_LOW();
   	}
   	else if (TimeMsCount > 0)
   	{
   	   	DH_LOW();
   	   	LINE_HIGH();
   	   	if (TimeMsCount == 9)
   	   	{
   	   	   	KeyScan();
   	   	}
   	}
}

void LedFlag(void)
{
   	static uchar FlashIndex;
   	
   	if (FlashIndex == 1)   	   	   	   	//扫描第一列
   	{
   	   	LINE_HIGH();
   	   	DH_LOW();
   	   	
   	   	if (sys_status_lithtflg)
   	   	{
   	   	   	DH1_LOW();
   	   	   	DH2_HIGH();
   	   	}
   	   	else
   	   	{
   	   	   	DH1_HIGH();
   	   	   	DH2_LOW();
   	   	}
   	   	
   	   	if (sys_status_airchange)
   	   	{
   	   	   	DH3_LOW();
   	   	   	DH4_HIGH();
   	   	}
   	   	else
   	   	{
   	   	   	DH3_HIGH();
   	   	   	DH4_LOW();
   	   	}
   	   	LINE1 = LOW;
   	}
   	else if (FlashIndex == 2)
   	{
   	   	LINE_HIGH();
   	   	DH_LOW();
   	   	
   	   	if (sys_status_nithtflg)
   	   	{
   	   	   	DH1_LOW();
   	   	   	DH2_HIGH();
   	   	}
   	   	else
   	   	{
   	   	   	DH1_HIGH();
   	   	   	DH2_LOW();
   	   	}
   	   	
   	   	if (sys_Colling_flg)
   	   	{  	   	
   	   	   	if (FlashCount > 500)
   	   	   	{
   	   	   	   	DH3_LOW();
   	   	   	   	DH4_HIGH();
   	   	   	}
   	   	   	else if (FlashCount < 500)
   	   	   	{
   	   	   	   	DH3_HIGH();
   	   	   	   	DH4_LOW();
   	   	   	   	if (FlashCount == 0)
   	   	   	   	{
   	   	   	   	   	FlashCount = 1000; 	
   	   	   	   	}
   	   	   	}
   	   	}
   	   	else if (sys_status_natureair)
   	   	{
   	   	   	DH3_LOW();
   	   	   	DH4_HIGH();
   	   	}
   	   	else
   	   	{
   	   	   	DH3_HIGH();
   	   	   	DH4_LOW();
   	   	}
   	   	LINE2 = LOW;
   	}
   	else if (FlashIndex == 3)
   	{
   	   	LINE_HIGH();
   	   	DH_LOW();
   	   	
   	   	if (sys_status_warmkey)
   	   	{
   	   	   	DH3_LOW();
   	   	   	DH4_HIGH();
   	   	}
   	   	else if (sys_status_stwarm)
   	   	{
   	   	   	DH3_HIGH();
   	   	   	DH4_HIGH();
   	   	}
   	   	else
   	   	{
   	   	   	DH3_HIGH();
   	   	   	DH4_LOW();
   	   	}
   	   	
   	   	if (sys_status_smartkey)
   	   	{
   	   	   	DH1_LOW();
   	   	   	DH2_HIGH();
   	   	}
   	   	else
   	   	{
   	   	   	DH1_HIGH();
   	   	   	DH2_LOW();
   	   	}
   	   	LINE3 = LOW;
   	}
   	
   	if (LightTimeCount == 5)
   	{
   	   	LightTimeCount = 0;
   	   	FlashIndex++;
   	   	if (FlashIndex == 4)
   	   	{
   	   	   	FlashIndex = 1;
   	   	}
   	   	DH_LOW();
   	   	KeyScan();
   	}
}


void Delay_10ms(uchar Count)
{  	
   	while(Count != 0)
   	{
   	   	if(sys_10ms_flag == TRUE)
   	   	{
   	   	   	sys_10ms_flag = FAULT;
   	   	   	Count--;   
   	   	}
   	}
}

void DataRcv(void)
{
   	static uchar   	Status;
   	static uint    	RcvTimeCount;
   	static uchar   	DataBitCount;  	   	   	   	   	//接收到数据位计数
   	static uchar   	RcvTemp;       	   	   	   	   	//接收缓存
   	static uint    	RcvData1;  	   	   	   	   	   	//接收到的第一组数据
   	static uint    	RcvData2;  	   	   	   	   	   	//接收到的第二组数据
   	uchar CmdDataHigh;
   	uchar CmdDataLow;
   	
   	switch (Status)
   	{
   	   	
   	   	case 0:
   	   	// 9.44ms高电平  236
   	   	if (DATAIN == HIGH)
   	   	{
   	   	   	RcvTimeCount++; 
   	   	   	if (RcvTimeCount > 310)
   	   	   	{
   	   	   	   	RcvTimeCount = 0;
   	   	   	}
   	   	}
   	   	
   	   	if (DATAIN == LOW)
   	   	{
   	   	   	if (RcvTimeCount > 45 && RcvTimeCount < 300)
   	   	   	{
   	   	   	   	RcvTimeCount = 1;
   	   	   	   	Status++;
   	   	   	}
   	   	   	else
   	   	   	{
   	   	   	   	RcvTimeCount = 0;
   	   	   	}
   	   	}
   	   	
   	   	break;
   	   	
   	   	case 1:
   	   	
   	   	if (DATAIN == LOW)
   	   	{
   	   	   	RcvTimeCount++;
   	   	   	if (RcvTimeCount > 200)
   	   	   	{
   	   	   	   	RcvTimeCount = 0;
   	   	   	   	Status = 0;
   	   	   	}
   	   	}
   	   	
   	   	// 4.8ms低电平  120
   	   	if (DATAIN == HIGH)
   	   	{
   	   	   	if (RcvTimeCount > 100 && RcvTimeCount < 190)
   	   	   	{
   	   	   	   	Status++;
   	   	   	   	RcvTimeCount = 1;
   	   	   	}
   	   	   	else 
   	   	   	{
   	   	   	   	RcvTimeCount = 0;
   	   	   	   	Status = 0;
   	   	   	}
   	   	}
   	   	
   	   	break;
   	   	
   	   	case 2:
   	   	
   	   	if (DATAIN == HIGH)
   	   	{
   	   	   	RcvTimeCount++;
   	   	   	if (RcvTimeCount > 50)
   	   	   	{
   	   	   	   	RcvTimeCount = 0;
   	   	   	   	Status = 0;
   	   	   	}
   	   	}
   	   	
   	   	// 640us高电平  16
   	   	if (DATAIN == LOW)
   	   	{
   	   	   	if (RcvTimeCount > 10 && RcvTimeCount < 50) 
   	   	   	{
   	   	   	   	//第33位低电平时判断，此时计数32
   	   	   	   	if (DataBitCount == 32)
   	   	   	   	{
   	   	   	   	   	RcvData2 = RcvData2 << 8;
   	   	   	   	   	RcvData2 &= 0XFF00;
   	   	   	   	   	RcvData2 |= RcvTemp;
   	   	   	   	   	
   	   	   	   	   	DataBitCount = 0;
   	   	   	   	   	RcvTimeCount = 0;
   	   	   	   	   	Status = 0;	
   	   	   	   	   	CmdDataHigh = RcvData2 >> 8;
   	   	   	   	   	CmdDataLow  = RcvData2;
   	   	   	   	   	
   	   	   	   	   	if (FramHead == RcvData1)
   	   	   	   	   	{  	   	   
   	   	   	   	   	   	switch (CmdDataHigh)
   	   	   	   	   	   	{
   	   	   	   	   	   	   	case ZMKEY:
   	   	   	   	   	   	   	
   	   	   	   	   	   	   	if (CmdDataLow == ZMON)
   	   	   	   	   	   	   	{
   	   	   	   	   	   	   	   	sys_status_lithtflg = TRUE;
   	   	   	   	   	   	   	   	sys_status_nithtflg = FAULT;
   	   	   	   	   	   	   	}
   	   	   	   	   	   	   	else if (CmdDataLow == XYDON)
   	   	   	   	   	   	   	{
   	   	   	   	   	   	   	   	sys_status_nithtflg = TRUE;
   	   	   	   	   	   	   	   	sys_status_lithtflg = FAULT;
   	   	   	   	   	   	   	}
   	   	   	   	   	   	   	else if (CmdDataLow == SYSOFF)
   	   	   	   	   	   	   	{
   	   	   	   	   	   	   	   	sys_status_lithtflg = FAULT;
   	   	   	   	   	   	   	   	sys_status_nithtflg = FAULT;
   	   	   	   	   	   	   	}
   	   	   	   	   	   	   	
   	   	   	   	   	   	   	break;
   	   	   	   	   	   	   	
   	   	   	   	   	   	   	case HQKEY:
   	   	   	   	   	   	   	
   	   	   	   	   	   	   	if (CmdDataLow == SHZON)
   	   	   	   	   	   	   	{
   	   	   	   	   	   	   	   	sys_status_airchange = TRUE;
   	   	   	   	   	   	   	}
   	   	   	   	   	   	   	else if (CmdDataLow == SYSOFF)
   	   	   	   	   	   	   	{
   	   	   	   	   	   	   	   	sys_status_airchange = FAULT;
   	   	   	   	   	   	   	}
   	   	   	   	   	   	   	
   	   	   	   	   	   	   	break;
   	   	   	   	   	   	   	
   	   	   	   	   	   	   	case SMKEY:
   	   	   	   	   	   	   	
   	   	   	   	   	   	   	if (CmdDataLow == SHZON)
   	   	   	   	   	   	   	{
								//
   	   	   	   	   	   	   	   	if ((sys_status_stwarm == TRUE || sys_status_warmkey == TRUE)
   	   	   	   	   	   	   	   	   	&& sys_status_lithtflg == TRUE && sys_status_airchange == TRUE
   	   	   	   	   	   	   	   	   	&& sys_status_natureair == TRUE)
   	   	   	   	   	   	   	   	{
   	   	   	   	   	   	   	   	   	sys_status_stwarm = FAULT;
   	   	   	   	   	   	   	   	   	sys_status_warmkey = FAULT;
   	   	   	   	   	   	   	   	   	FlashCount = 1000; 	   	   	   	//1000ms
   	   	   	   	   	   	   	   	   	sys_Colling_flg = TRUE;
   	   	   	   	   	   	   	   	   	DelayCount = 367;         	   	   	   	   	//延时366ms
									sys_switch_light = TRUE;
									sys_switch_airchange = TRUE;
   	   	   	   	   	   	   	   	}
   	   	   	   	   	   	   	   	else if (sys_status_stwarm == TRUE || sys_status_warmkey == TRUE)
   	   	   	   	   	   	   	   	{
   	   	   	   	   	   	   	   	   	sys_status_stwarm = FAULT;
   	   	   	   	   	   	   	   	   	sys_status_warmkey = FAULT;
   	   	   	   	   	   	   	   	   	FlashCount = 1000; 	   	   	   	//1000ms
   	   	   	   	   	   	   	   	   	sys_Colling_flg = TRUE;
   	   	   	   	   	   	   	   	   	if (sys_status_lithtflg == TRUE || sys_status_airchange == TRUE)
   	   	   	   	   	   	   	   	   	{
   	   	   	   	   	   	   	   	   	   	DelayCount1 = 53;
   	   	   	   	   	   	   	   	   	}  	   	   	   	   	   	   	   	
   	   	   	   	   	   	   	   	}
   	   	   	   	   	   	   	   	else
   	   	   	   	   	   	   	   	{
   	   	   	   	   	   	   	   	   	if (sys_status_lithtflg)
   	   	   	   	   	   	   	   	   	{
   	   	   	   	   	   	   	   	   	   	sys_switch_light = TRUE;
   	   	   	   	   	   	   	   	   	}
   	   	   	   	   	   	   	   	   	
   	   	   	   	   	   	   	   	   	if (sys_status_airchange)
   	   	   	   	   	   	   	   	   	{
   	   	   	   	   	   	   	   	   	   	sys_switch_airchange = TRUE;
   	   	   	   	   	   	   	   	   	}
   	   	   	   	   	   	   	   	   	DelayCount1 = 53;      	   	   	   	   	//延时52ms
   	   	   	   	   	   	   	   	   	sys_smkey_off = TRUE;
   	   	   	   	   	   	   	   	}  	
   	   	   	   	   	   	   	   	sys_status_smartkey = TRUE;
   	   	   	   	   	   	   	}
   	   	   	   	   	   	   	else if (CmdDataLow == SYSOFF)
   	   	   	   	   	   	   	{
   	   	   	   	   	   	   	   	sys_status_smartkey = FAULT;
   	   	   	   	   	   	   	   	sys_status_airchange = FAULT;
   	   	   	   	   	   	   	   	sys_Colling_flg = FAULT;
   	   	   	   	   	   	   	   	sys_status_stwarm = FAULT;
   	   	   	   	   	   	   	   	sys_status_warmkey = FAULT;
   	   	   	   	   	   	   	   	sys_status_nithtflg = FAULT;
   	   	   	   	   	   	   	   	sys_status_lithtflg = FAULT;
   	   	   	   	   	   	   	}
   	   	   	   	   	   	   	
   	   	   	   	   	   	   	break;
   	   	   	   	   	   	   	
   	   	   	   	   	   	   	case QNKEY:
   	   	   	   	   	   	   	
   	   	   	   	   	   	   	
   	   	   	   	   	   	   	if (CmdDataLow == SHZON)
   	   	   	   	   	   	   	{
   	   	   	   	   	   	   	   	sys_status_natureair = TRUE;
   	   	   	   	   	   	   	   	sys_status_warmkey = TRUE;
   	   	   	   	   	   	   	}
   	   	   	   	   	   	   	else if (CmdDataLow == QNON)
   	   	   	   	   	   	   	{
   	   	   	   	   	   	   	   	sys_status_natureair = TRUE;
   	   	   	   	   	   	   	   	sys_status_warmkey = FAULT;
   	   	   	   	   	   	   	   	sys_status_stwarm = TRUE;  	
   	   	   	   	   	   	   	}
   	   	   	   	   	   	   	else if (CmdDataLow == SYSOFF)
   	   	   	   	   	   	   	{
   	   	   	   	   	   	   	   	sys_status_warmkey = FAULT;
   	   	   	   	   	   	   	   	sys_status_stwarm = FAULT;
   	   	   	   	   	   	   	}
   	   	   	   	   	   	   	
   	   	   	   	   	   	   	break;
   	   	   	   	   	   	   	
   	   	   	   	   	   	   	case SHZON:
   	   	   	   	   	   	   	
   	   	   	   	   	   	   	if (CmdDataLow == SHZON)
   	   	   	   	   	   	   	{
   	   	   	   	   	   	   	   	sys_status_natureair = TRUE;
   	   	   	   	   	   	   	   	if (sys_Colling_flg)
   	   	   	   	   	   	   	   	{
   	   	   	   	   	   	   	   	   	sys_Colling_flg = !sys_Colling_flg;
   	   	   	   	   	   	   	   	}
   	   	   	   	   	   	   	}
   	   	   	   	   	   	   	else if (CmdDataLow == SYSOFF)
   	   	   	   	   	   	   	{
   	   	   	   	   	   	   	   	sys_status_natureair = FAULT;
   	   	   	   	   	   	   	   	if (sys_switch_light == TRUE || sys_switch_airchange == TRUE)
   	   	   	   	   	   	   	   	{
   	   	   	   	   	   	   	   	   	DelayCount = 366;  	   	   	//延时365ms
   	   	   	   	   	   	   	   	}
   	   	   	   	   	   	   	   	
   	   	   	   	   	   	   	   	if (sys_Colling_flg)
   	   	   	   	   	   	   	   	{
   	   	   	   	   	   	   	   	   	sys_Colling_flg = !sys_Colling_flg;
   	   	   	   	   	   	   	   	}
   	   	   	   	   	   	   	}
   	   	   	   	   	   	   	else if (CmdDataLow == 0XF7)
   	   	   	   	   	   	   	{
   	   	   	   	   	   	   	   	sys_Colling_flg = TRUE;
   	   	   	   	   	   	   	   	sys_status_natureair = TRUE;
   	   	   	   	   	   	   	}
   	   	   	   	   	   	   	
   	   	   	   	   	   	   	
   	   	   	   	   	   	   	break;
   	   	   	   	   	   	   	
   	   	   	   	   	   	   	default :
   	   	   	   	   	   	   	
   	   	   	   	   	   	   	break;
   	   	   	   	   	   	}
   	   	   	   	   	}
   	   	   	   	}
   	   	   	   	else
   	   	   	   	{
   	   	   	   	   	RcvTimeCount = 1;
   	   	   	   	   	Status++;
   	   	   	   	}
   	   	   	   	
   	   	   	   	if (DataBitCount == 8)
   	   	   	   	{
   	   	   	   	   	RcvData1 = RcvTemp;
   	   	   	   	}
   	   	   	   	else if (DataBitCount == 16)
   	   	   	   	{
   	   	   	   	   	RcvData1 = RcvData1 << 8;
   	   	   	   	   	RcvData1 &= 0XFF00;
   	   	   	   	   	RcvData1 |= RcvTemp;
   	   	   	   	}
   	   	   	   	else if (DataBitCount == 24)
   	   	   	   	{
   	   	   	   	   	RcvData2 = RcvTemp;
   	   	   	   	}
   	   	   	}  	
   	   	}
   	   	
   	   	break;
   	   	
   	   	case 3:
   	   	
   	   	if (DATAIN == LOW)
   	   	{
   	   	   	RcvTimeCount++;
   	   	   	if (RcvTimeCount > 100)
   	   	   	{
   	   	   	   	RcvTimeCount = 0;
   	   	   	   	Status = 0;
   	   	   	}
   	   	}
   	   	
   	   	if (DATAIN == HIGH)
   	   	{
   	   	   	// 1.84ms低电平   46
   	   	   	if (RcvTimeCount > 35 && RcvTimeCount < 80)
   	   	   	{
   	   	   	   	RcvTimeCount = 1;
   	   	   	   	DataBitCount++;
   	   	   	   	Status--;
   	   	   	   	RcvTemp <<= 1;
   	   	   	   	RcvTemp &= 0xfe;
   	   	   	}
   	   	   	// 640us低电平  16
   	   	   	else if(RcvTimeCount > 9 && RcvTimeCount < 25)
   	   	   	{
   	   	   	   	RcvTimeCount = 1;
   	   	   	   	DataBitCount++;
   	   	   	   	Status--;
   	   	   	   	RcvTemp <<= 1;
   	   	   	   	RcvTemp |= 0x01;
   	   	   	}
   	   	}
   	   	
   	   	break;
   	   	
   	   	default :
   	   	
   	   	break;
   	}
} 


void int_isr(void) __interrupt
{
   	
   	__asm
   	   	   	movra   _ABuf
   	   	   	swapar  _STATUS
   	   	   	movra   _StatusBuf
   	__endasm;
//========= T0/T1中断 ==============   
    if (T1IF)          	   	   	   	//1ms
   	{
   	   	T1IF = FAULT;
   	   	TimeMsCount++;
   	   	LightTimeCount++;
   	   	
   	   	if(TimeMsCount >= 10)
   	   	{
   	   	   	Time10MsCount++;
   	   	   	TimeMsCount = 0;
   	   	   	sys_10ms_flag = TRUE;
   	   	}
   	   	
   	   	if (FlashCount != 0)
   	   	{
   	   	   	FlashCount--;
   	   	}
   	   	
   	   	if (DelayCount1 != 0)
   	   	{
   	   	   	DelayCount1--;
   	   	   	if (DelayCount1 == 1)
   	   	   	{
   	   	   	   	if (sys_smkey_off)
   	   	   	   	{
   	   	   	   	   	sys_smkey_off = !sys_smkey_off;
   	   	   	   	   	DataSend = NatureAirOFF;
   	   	   	   	}
   	   	   	   	
   	   	   	   	if (sys_Colling_flg)
   	   	   	   	{
   	   	   	   	   	if (sys_status_lithtflg)
   	   	   	   	   	{
   	   	   	   	   	   	DataSend = NightLightKey;
   	   	   	   	   	}
   	   	   	   	   	else if (sys_status_airchange)
   	   	   	   	   	{
   	   	   	   	   	   	DataSend = ChangeAirOFF;
   	   	   	   	   	}
   	   	   	   	}
   	   	   	}
   	   	}
   	   	
   	   	if (DelayCount != 0)
   	   	{
   	   	   	DelayCount--;
   	   	   	if (DelayCount == 1)
   	   	   	{
   	   	   	   	//指令返送
   	   	   	   	if (sys_switch_light == TRUE && sys_switch_airchange == TRUE)
   	   	   	   	{
   	   	   	   	   	sys_switch_light = !sys_switch_light;
   	   	   	   	   	DelayCount = 158;
   	   	   	   	   	DataSend = NightLightON;
   	   	   	   	}
   	   	   	   	else if (sys_switch_light)
   	   	   	   	{
   	   	   	   	   	sys_switch_light = !sys_switch_light;
   	   	   	   	   	DataSend = NightLightON;
   	   	   	   	}
   	   	   	   	else if (sys_switch_airchange)
   	   	   	   	{
   	   	   	   	   	sys_switch_airchange = !sys_switch_airchange;
   	   	   	   	   	DataSend = ChangeAirOFF;
   	   	   	   	}
   	   	   	}
   	   	}
   	}
   	
   	if (T0IF)  	   	   	   	   	   	//40us
   	{
   	   	T0IF = FAULT;
   	   	
   	   	DataRcv();
   	   	
   	   	if (DataSend != 0)
   	   	{
   	   	   	CmdSend(DataSend);
   	   	}  	   	
   	}

//========= 外部中断 ==============

   	__asm
   	   	   	swapar  _StatusBuf
   	   	   	movra   _STATUS
   	   	   	swapr   _ABuf
   	   	   	swapar  _ABuf
   	__endasm;
}

