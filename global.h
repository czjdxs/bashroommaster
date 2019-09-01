#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include "mc32p7311.h"

#define TRUE  1
#define FAULT 0
#define ON 	  1
#define OFF   0
#define HIGH  1
#define LOW   0

#define FramHead		0XB4FD			//帧头
#define SmartKey		0XFC0F			//智能关机开指令
#define LightKey		0XFA0F			//照明开关开指令
#define NightLightKey	0XFA0E  	   	//小夜灯开关开指令
#define NightLightON	0XFAF5			//小夜灯开指令
#define WarmKey			0XFD0F			//取暖开关开指令
#define NatureAirKey	0XFE0E			//自然风开关开指令
#define NatureAirOFF	0XFEFF			//自然风关闭指令
#define	ChangeAirKey	0XFB0F			//换气按键指令
#define ChangeAirOFF	0XFBFF			//换气关指令


#define ZMKEY			0XFA			//照明
#define HQKEY			0XFB			//换气
#define SMKEY			0XFC			//智能关机 
#define QNKEY			0XFD			//取暖
#define SYSOFF			0XFF			//关闭
#define ZMON			0X9F			//照明开指令
#define SHZON			0XFE			//取暖，自然风，换气，智能开关开
#define	QNON			0XFD			//强暖
#define XYDON			0XF5			//小夜灯指令



#define DATAIN			P12D
#define DATAOUT			P11D
#define LINE1			P15D
#define	LINE2			P02D
#define LINE3			P03D
#define ROW1			P14D
#define ROW2			P04D
#define ROW3			P17D
#define ROW4			P16D
#define ROWKEY1			P00D
#define ROWKEY2			P01D

//L1-P15,L2-P02,L3-P03
#define LINEOUT()	{OEP1 |= 0X20;OEP0 |= 0X0C;}					//列扫描配置成输出模式
#define LINEIN()	{OEP1 &= 0XDF;OEP0 &= 0XF3;}					//列扫描配置成输入模式
//DH1-P14,DH2-P04,DH3-P17,DH4-P16,KH1-P00,KH2-P01
#define	DH1_HIGH()		ROW1 	= HIGH									
#define	DH2_HIGH()		ROW2 	= HIGH								
#define DH3_HIGH()		ROW3 	= HIGH								
#define DH4_HIGH()		ROW4 	= HIGH								
#define KH1_HIGH()		ROWKEY1 = HIGH
#define KH2_HIGH()		ROWKEY2 = HIGH
#define DH_HIGH()		{IOP0 |= 0X10;IOP1 |= 0XD0;}
#define LINE_HIGH()		{IOP0 |= 0X0C;IOP1 |= 0X20;}


#define DH1_LOW()		ROW1 	= LOW
#define DH2_LOW()		ROW2 	= LOW
#define DH3_LOW()		ROW3 	= LOW
#define DH4_LOW()		ROW4 	= LOW
#define KH1_LOW()		ROWKEY1 = LOW
#define KH2_LOW()		ROWKEY2 = LOW
#define DH_LOW()		{IOP0 &= 0XEF;IOP1 &= 0X2F;}
#define LINE_LOW()		{IOP0 &= 0XF3;IOP1 &= 0XDF;}



typedef union
{
   	struct
   	{
   	   	uchar flg0:1;
   	   	uchar flg1:1;
   	   	uchar flg2:1;
   	   	uchar flg3:1;
   	   	uchar flg4:1;
   	   	uchar flg5:1;
   	   	uchar flg6:1;
   	   	uchar flg7:1;
   	}bits;
}flag;

flag Flag;

#define KeyFlag_Keystat			Flag.bits.flg0			//有按键按下标志
#define sys_10ms_flag 			Flag.bits.flg1     	   	//1ms标志位
#define sys_rcv_end_flg			Flag.bits.flg2			//接收完成标志位
#define sys_rcv_head_flg		Flag.bits.flg3			//接收到帧头标志
#define sys_data_flag			Flag.bits.flg4			//数据是否发送完成,1-正在发送0-发送完成
#define sys_Colling_flg			Flag.bits.flg5			//自然风冷却状态
#define sys_switch_light		Flag.bits.flg6			//从照明状态切换
#define sys_switch_airchange	Flag.bits.flg7			//从换气状态切换

flag status;

#define sys_status_stwarm			status.bits.flg0		//强暖标志
#define sys_status_airchange		status.bits.flg1		//换气标志
#define sys_status_smartkey			status.bits.flg2		//智能关机
#define sys_status_warmkey			status.bits.flg3		//取暖标志
#define sys_status_natureair		status.bits.flg4		//自然风标志
#define sys_status_nithtflg			status.bits.flg5		//夜灯按键标志
#define sys_status_lithtflg			status.bits.flg6		//照明按键标志
#define sys_smkey_off				status.bits.flg7		//智能按键关闭指令发送


void BoardInit();
void  Clr_Ram();
void KeyScan();
void CmdSend(unsigned int Command);
void Waittingflash();
void LedFlag();
void Delay_10ms(uchar Count);

#endif