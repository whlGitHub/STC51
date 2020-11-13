#include<STC15F2K60S2.H>
#include<intrins.h>
#include<stdio.h>
#define uchar unsigned char
#define uint unsigned int

void All_Init();
void Dis_Bit(uchar com,uchar singlenum);
void delay(uint t);
void Read_KBD();

void UartInit(void)	;
char putchar(char c);
void SendStr(unsigned char *str);

uchar key_value,read_buf;
const uchar code dscom[] = {0x00,0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0xff};
const uchar code dsnum[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff};

void Write_DS18B20(unsigned char dat);
unsigned char Read_DS18B20(void);
bit init_ds18b20(void);
unsigned char ReadTemp(void);
float ReadTempFloat(void);

uchar Init_Time[7] = {50,59,23,16,10,5,20};
uchar timeArray[7] ;
void Write_Ds1302(unsigned  char temp);
void Write_Ds1302_Byte( unsigned char address,unsigned char dat );
unsigned char Read_Ds1302_Byte ( unsigned char address );
void DS1302_Init();
void DS1302_Get();

void IIC_Delay(unsigned char i);
void IIC_Start(void);
void IIC_Stop(void);
void IIC_SendAck(bit ackbit);
bit IIC_WaitAck(void);
void IIC_SendByte(unsigned char byt);
unsigned char IIC_RecByte(void);
unsigned char Read_EEPROM(add);
void Write_EEPROM(uchar add,uchar val);

void Write_ADC(char val);
uchar Read_ADC(uchar channel);
void PCF8591_Init(uchar channel);

uint tt,freq;
void Freq_Timer0(void);
void Freq_Timer1(void);

uint PWM_NUM;
void PWM_Init();

unsigned int Check_Distance();
void send_wave(void) ;

void main()
{
	All_Init();
	PWM_Init();
	EA = 1;
	ET0 = 1;
	while(1)
	{	

	}
	
}


void All_Init()
{
	P2 = 0xA0;
	P0 = 0x00;//关闭蜂鸣器

	P2 = 0x80;
	P0 = 0xFF;//关闭led灯

	P2 = 0xC0;
	P0 = 0xFF;
	P2 = 0xE0;
	P0 = 0xFF;
}

void Dis_Bit(uchar com,uchar singlenum)
{
	P2 = 0xC0;
	P0 = dscom[com];
	P2 = 0xE0;
	P0 = dsnum[singlenum];
	P2 = 0xFF;
	delay(5);
}

void delay(uint t)
{
	uint i;
	while(t--)
		for(i=0;i<845;i++);
}

sbit r1 = P3^0;
sbit r2 = P3^1;
sbit r3 = P3^2;
sbit r4 = P3^3;
sbit c1 = P4^4;
sbit c2 = P4^2;
sbit c3 = P3^5;
sbit c4 = P3^4;

void Read_KBD()
{
	uchar key_buf = 0;
	static key_state = 0;
	c1 = 0;
	c2 = c3 = c4 = 1;
	r1 = r2 = r3 = r4 = 1;
	if(!r1) key_buf = 7;
	else if(!r2) key_buf = 6;
	else if(!r3) key_buf = 5;
	else if(!r4) key_buf = 4;

	c2 = 0;
	c1 = c3 = c4 = 1;
	r1 = r2 = r3 = r4 = 1;
	if(!r1) key_buf = 11;
	else if(!r2) key_buf = 10;
	else if(!r3) key_buf = 9;
	else if(!r4) key_buf = 8;
	
	c3 = 0;
	c1 = c2 = c4 = 1;
	r1 = r2 = r3 = r4 = 1;
	if(!r1) key_buf = 15;
	else if(!r2) key_buf = 14;
	else if(!r3) key_buf = 13;
	else if(!r4) key_buf = 12;

	c4 = 0;
	c1 = c2 = c3 = 1;
	r1 = r2 = r3 = r4 = 1;
	if(!r1) key_buf = 19;
	else if(!r2) key_buf = 18;
	else if(!r3) key_buf = 17;
	else if(!r4) key_buf = 16;

	switch(key_state)
	{
		case 0:
			if(key_buf != 0)
			{
				read_buf = key_buf;
				key_state = 1;
			}
			break;
		case 1:
			if(read_buf == key_buf)
			{
				key_value = read_buf;
				key_state = 2;
			}
			break;
		case 2:
			if(key_buf == 0)
			{
				key_state = 0;
			}
			break;
		default:break;
	}
}


void UartInit(void)		//115200bps@11.0592MHz
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x01;		//串口1选择定时器2为波特率发生器
	AUXR |= 0x04;		//定时器2时钟为Fosc,即1T
	T2L = 0xE8;		//设定定时初值
	T2H = 0xFF;		//设定定时初值
	AUXR |= 0x10;		//启动定时器2
}


char putchar(char c)
{
    SBUF=c;
    while(TI!=1);   //等待发送成功
    TI=0;           //清除发送中断标志
    return c;
}


void SendStr(unsigned char *str)
{
    unsigned char *p;

    p = str;
    while(*p != '\0')
    {
        SBUF = *p;
		while(TI == 0);
		TI = 0;
        p++;
    }
}
sbit DQ = P1^4;

//单总线延时函数

void Delay_OneWire(unsigned int t)  //STC12C5260S2
{
	unsigned char i;
	while(t--){
		for(i=0;i<12;i++);
	}
}

//通过单总线向DS18B20写一个字节
void Write_DS18B20(unsigned char dat)
{

	unsigned char i;
	for(i=0;i<8;i++)
	{
		DQ = 0;
		DQ = dat&0x01;
		Delay_OneWire(5);
		DQ = 1;
		dat >>= 1;
	}
	Delay_OneWire(5);
}

//从DS18B20读取一个字节
unsigned char Read_DS18B20(void)
{
	unsigned char i;
	unsigned char dat;
  
	for(i=0;i<8;i++)
	{
		DQ = 0;
		dat >>= 1;
		DQ = 1;
		if(DQ)
		{
			dat |= 0x80;
		}	    
		Delay_OneWire(5);
	}
	return dat;
}

//DS18B20初始化
bit init_ds18b20(void)
{
  	bit initflag = 0;
  	
  	DQ = 1;
  	Delay_OneWire(12);
  	DQ = 0;
  	Delay_OneWire(80); // 延时大于480us
  	DQ = 1;
  	Delay_OneWire(10);  // 14
  	initflag = DQ;     // initflag等于1初始化失败
  	Delay_OneWire(5);
  
  	return initflag;
}

//DS18B20温度采集程序：整数
unsigned char ReadTemp(void)
{
	unsigned char low,high;
	char temp;

	init_ds18b20();
	Write_DS18B20(0xCC);
	Write_DS18B20(0x44); //启动温度转换
	Delay_OneWire(200);

	init_ds18b20();
	Write_DS18B20(0xCC);
	Write_DS18B20(0xBE); //读取寄存器

	low = Read_DS18B20(); //低字节
	high = Read_DS18B20(); //高字节
	/** 精度为1摄氏度 */  
	temp = high<<4;
	temp |= (low>>4);
	return temp;
}

//DS18B20温度采集程序：浮点数
float ReadTempFloat(void)
{
    unsigned int temp;
	float temperature;
    unsigned char low,high;
  
  	init_ds18b20();
  	Write_DS18B20(0xCC);
  	Write_DS18B20(0x44); //启动温度转换
  	Delay_OneWire(200);

  	init_ds18b20();
  	Write_DS18B20(0xCC);
  	Write_DS18B20(0xBE); //读取寄存器

  	low = Read_DS18B20(); //低字节
  	high = Read_DS18B20(); //高字节
/** 精度为0.0625摄氏度 */  
	temp = (high&0x0f);
	temp <<= 8;
	temp |= low;
	temperature = temp*0.0625;
  	return temperature;
}


/*
sbit SCK=P1^7;		
sbit SDA=P2^3;		
sbit RST = P1^3;   // DS1302复位												

void Write_Ds1302(unsigned  char temp) 
{
	unsigned char i;
	for (i=0;i<8;i++)     	
	{ 
		SCK=0;
		SDA=temp&0x01;
		temp>>=1; 
		SCK=1;
	}
}   

void Write_Ds1302_Byte( unsigned char address,unsigned char dat )     
{
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
 	RST=1; 	_nop_();  
 	Write_Ds1302(address);	
 	Write_Ds1302(((dat/10)<<4)|(dat%10));		
 	RST=0; 
}

unsigned char Read_Ds1302_Byte ( unsigned char address )
{
 	unsigned char i,temp=0x00,dat1,dat2;
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
 	RST=1;	_nop_();
 	Write_Ds1302(address);
 	for (i=0;i<8;i++) 	
 	{		
		SCK=0;
		temp>>=1;	
 		if(SDA)
 		temp|=0x80;	
 		SCK=1;
	} 
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
	SCK=1;	_nop_();
	SDA=0;	_nop_();
	SDA=1;	_nop_();
	dat1 = temp/16;
	dat2 = temp%16;
	temp = dat1*10+dat2;
	return (temp);			
}

void DS1302_Init()
{
	unsigned char i,add;
	add = 0x80;
	Write_Ds1302_Byte(0x8e,0x00);
	for(i = 0;i < 7;i++)
	{
		Write_Ds1302_Byte(add,Init_Time[i]);
		add = add+2;
	}
	Write_Ds1302_Byte(0x8e,0x80);
}

void DS1302_Get()
{
	unsigned char i,add;
	add = 0x81;
	Write_Ds1302_Byte(0x8e,0x00);
	for(i = 0;i<7;i++)
	{
		timeArray[i] = Read_Ds1302_Byte(add);
		add = add+2;
	}
	Write_Ds1302_Byte(0x8e,0x80);
}

*/

#define DELAY_TIME 5

#define SlaveAddrW 0xA0
#define SlaveAddrR 0xA1

//总线引脚定义
sbit SDA = P2^1;  /* 数据线 */
sbit SCL = P2^0;  /* 时钟线 */

void IIC_Delay(unsigned char i)
{
    do{_nop_();}
    while(i--);        
}
//总线启动条件
void IIC_Start(void)
{
    SDA = 1;
    SCL = 1;
    IIC_Delay(DELAY_TIME);
    SDA = 0;
    IIC_Delay(DELAY_TIME);
    SCL = 0;	
}

//总线停止条件
void IIC_Stop(void)
{
    SDA = 0;
    SCL = 1;
    IIC_Delay(DELAY_TIME);
    SDA = 1;
    IIC_Delay(DELAY_TIME);
}

//发送应答
void IIC_SendAck(bit ackbit)
{
    SCL = 0;
    SDA = ackbit;  					// 0：应答，1：非应答
    IIC_Delay(DELAY_TIME);
    SCL = 1;
    IIC_Delay(DELAY_TIME);
    SCL = 0; 
    SDA = 1;
    IIC_Delay(DELAY_TIME);
}

//等待应答
bit IIC_WaitAck(void)
{
    bit ackbit;
	
    SCL  = 1;
    IIC_Delay(DELAY_TIME);
    ackbit = SDA;
    SCL = 0;
    IIC_Delay(DELAY_TIME);
    return ackbit;
}

//通过I2C总线发送数据
void IIC_SendByte(unsigned char byt)
{
    unsigned char i;

    for(i=0; i<8; i++)
    {
        SCL  = 0;
        IIC_Delay(DELAY_TIME);
        if(byt & 0x80) SDA  = 1;
        else SDA  = 0;
        IIC_Delay(DELAY_TIME);
        SCL = 1;
        byt <<= 1;
        IIC_Delay(DELAY_TIME);
    }
    SCL  = 0;  
}

//从I2C总线上接收数据
unsigned char IIC_RecByte(void)
{
    unsigned char i, da;
    for(i=0; i<8; i++)
    {   
    	SCL = 1;
	IIC_Delay(DELAY_TIME);
	da <<= 1;
	if(SDA) da |= 1;
	SCL = 0;
	IIC_Delay(DELAY_TIME);
    }
    return da;    
}

void Write_EEPROM(uchar add,uchar val)
{
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(add);
	IIC_WaitAck();
	IIC_SendByte(val);
	IIC_WaitAck();
	IIC_Stop();
	delay(10);
}

unsigned char Read_EEPROM(uchar add)
{
	uchar dat;
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(add);
	IIC_WaitAck();
	IIC_Start();
	IIC_SendByte(0xa1);
	IIC_WaitAck();
	dat = IIC_RecByte();
	IIC_SendAck(1);
	IIC_Stop();
	return dat;
}

void PCF8591_Init(uchar channel)
{
	IIC_Start();
	IIC_SendByte(0x90);
	IIC_WaitAck();
	IIC_SendByte(channel);
	IIC_WaitAck();
	IIC_Stop();
	delay(1);
}

uchar Read_ADC(char channel)
{
	uchar temp;
	PCF8591_Init(channel);
	
	IIC_Start();
	IIC_SendByte(0x91);
	IIC_WaitAck();
	temp = IIC_RecByte();
	IIC_SendAck(1);
	IIC_Stop();
	return temp;
}

void Write_ADC(char val)
{
	IIC_Start();
	IIC_SendByte(0x90);
	IIC_WaitAck();
	IIC_SendByte(0x40);
	IIC_WaitAck();
	IIC_SendByte(val);
	IIC_WaitAck();
	IIC_Stop();
}

void Freq_Handler()        interrupt 3
{
	tt++;
	if(tt==1000)
	{
			TR0=0;        
			tt=0;
			freq=TH0*256+TL0;
			TH0=0;
			TL0=0;
			TR0=1;
	}        
}

void Freq_Timer1(void)		//1毫秒@11.0592MHz
{
	AUXR |= 0x40;		//定时器时钟1T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0xCD;		//设置定时初值
	TH1 = 0xD4;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	TR1 = 1;		//定时器1开始计时
}

void Freq_Timer0(void)        
{
	AUXR &= 0x80;                //定时器时钟1T模式
	TMOD |= 0x05;                //设置定时器为计数模式
	TL0 = 0x00;                    //设置定时初值
	TH0 = 0x00;                    //设置定时初值
	TF0 = 0;                    //清除TF0标志
	TR0 = 1;                    //定时器0开始计时
}

void PWM_Init(void)
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0xAE;		//设置定时初值
	TH0 = 0xFB;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
}

void PWM_Output(void)	interrupt 1
{
	tt++;
	if(tt == PWM_NUM)
	{
		Dis_Bit(1,1);
	}
	else if(tt == 100)
	{
		Dis_Bit(1,10);
		tt = 0;
		PWM_NUM++;
		if(PWM_NUM == 100)
		{
			PWM_NUM = 0;
		}
	}
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
