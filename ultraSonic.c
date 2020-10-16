#include<STC15F2K60S2.H>
#include <intrins.h>

void delayMs(int ms);
void allInit();
char readKbd();
void ds(char com,char num);
unsigned int Check_Distance();
void send_wave(void) ;
unsigned char code ledStatus[]={0XFF,0XFE,0XFD,0XFB,0XF7,0XEF,0XDF,0XBF,0X7F,0X00};
unsigned char code dscom[]={0X00,0X01,0X02,0X04,0X08,0X10,0X20,0X40,0X80,0XFF};//位选
unsigned char code dsnum[]={0XC0,0XF9,0XA4,0XB0,0X99,0X92,0X82,0XF8,0X80,0X98,0X7F,0XBF,0X00,0XFF};//字符

unsigned char Init_Time[]={50,59,23,13,10,2,20};
unsigned char timeArray[7];

void main()
{
	unsigned char num = 0;
	unsigned char com = 0;
	unsigned int distance;
	allInit();
	while(1)
	{
		distance = Check_Distance();
		ds(1,distance/10);
		ds(2,distance%10);
	}
	
}
void ds(char com,char num)
{
	P2=0XC0;
	P0=dscom[com];
	P2=0XE0;
	P0=dsnum[num];
	P2=0xFF;
	delayMs(1);
}
void allInit()
{
	P2=0XA0;
	P0=0X00;

	P2=0X80;
	P0=0XFF;

	P2=0XC0;
	P0=0XFF;
	P2=0XE0;
	P0=0XFF;
}



void delayMs(int ms)
{
	int i,j;
	for(i=ms;i>0;i--)
		for(j=845;j>0;j--);
}





sbit r1=P3^0;    //4行
sbit r2=P3^1;
sbit r3=P3^2;
sbit r4=P3^3;
                 //4列
sbit c1=P4^4;
sbit c2=P4^2;
sbit c3=P3^5;
sbit c4=P3^4;
char readKbd(void)
{
	unsigned char key_value = 0;

	r1=0;
	r2=r3=r4=1;
	c1=c2=c3=c4=1;
	if(!c1) key_value=1;
	else if(!c2) key_value=2;
	else if(!c3) key_value=3;
	else if(!c4) key_value=4;

    r2=0;
	r1=r3=r4=1;
	c1=c2=c3=c4=1;
	if(!c1) key_value=5;
	else if(!c2) key_value=6;
	else if(!c3) key_value=7;
	else if(!c4) key_value=8;

	r3=0;
	r2=r1=r4=1;
	c1=c2=c3=c4=1;
	if(!c1) key_value=9;
	else if(!c2) key_value=10;
	else if(!c3) key_value=11;
	else if(!c4) key_value=12;

	r4=0;
	r2=r3=r1=1;
	c1=c2=c3=c4=1;
	if(!c1) key_value=13;
	else if(!c2) key_value=14;
	else if(!c3) key_value=15;
	else if(!c4) key_value=16;
	
	return key_value;
}


#define somenops {_nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_();}

sbit TX = P1^0; //发射引脚
sbit RX = P1^1; //接收引脚

//TX 引脚发送 40KHz 方波信号驱动超声波发送探头
void send_wave(void) 
{
	unsigned char i = 8; //发送 8 个脉冲
	
	do {
		TX = 1;
		somenops;somenops;somenops;somenops;somenops;  
		somenops;somenops;somenops;somenops;somenops;
		TX = 0;
		somenops;somenops;somenops;somenops;somenops;  
		somenops;somenops;somenops;somenops;somenops;
	} while(i--);
}

unsigned int Check_Distance()
{
	unsigned int distance,t;

    TMOD &= 0xF0;  //配置定时器工作模式
    TH0 = 0;
    TL0 = 0;  
  
    EA = 1;
    ET0 = 0;  //打开定时器0中断
    //TR0 = 1;  //启动定时器   

	send_wave();  //发送方波信号
	TR0= 1;  //启动计时
	while((RX == 1) && (TF0 == 0));  //等待收到脉冲
		 TR0 = 0;  //关闭计时

	//发生溢出
	if(TF0 == 1)
	{
		TF0= 0;
		distance = 999;  //无返回
	}
	else
	{
		/**  计算时间  */
		t = (TH0<<8)|TL0;
		distance = (unsigned int)(t*0.017);  //计算距离				
		//distance = (unsigned int)(t*17/1000);  //计算距离				
	}
	TH0 = 0;
	TL0 = 0;
	return distance;
}
