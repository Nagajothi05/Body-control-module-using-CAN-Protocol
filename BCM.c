//node1.c
#include<lpc21xx.h>
#include"delay_header.h"
#include"can.h"
#include"uart_header.h"
#include"4bit.c"

#define SW1 1<<14
#define SW2	1<<15
#define SW3 1<<16

void CAN_INIT(void)
{
	PINSEL1|=CAN;//p0.23,p0.24 can func
	VPBDIV=1;//reset mode
	C2MOD=0x1;//accept all msgs
	AFMR=0x2;//speed 125kbps
	C2BTR=0x001c001D;//normal mode
	C2MOD=0x0;
}
void CAN2_TX(CAN2_MSG x)
{
	C2TID1=x.id;
	C2TFI1=(x.dlc<<16);
	if(x.rtr==0)//if data frame
	{
		C2TFI1|=~(1<<30);
		C2TDA1=x.byteA;
		C2TDB1=x.byteB;
	}
	else
		C2TFI1|=1<<30;
	C2CMR=(1<<0)|(1<<5);
	while(C2GSR&(1<<3)==0);
}
int main()
{	
	CAN2_MSG m1,m2,m3;	
	CAN_INIT();
	UART0_CONFIG(115200);
	//LCD_INIT();
	//LCD_STR("BODY CONTROL");

	/*sending data frame*/
	m1.id=0x1;
	m1.rtr=0;//data frame
	m1.dlc=4;
	m1.byteA=0xAAAAAAAA;
	m1.byteB=0;
	
	//message 2
	m2.id=0x2;
	m2.rtr=0;
	m2.dlc=4;
	m2.byteA=0xBBBBBBBB;
	m2.byteB=0;
	
	m3.id=0x3;
	m3.rtr=0;
	m3.dlc=4;
	m3.byteA=0xCCCCCCCC;
	m3.byteB=0;
	//UART0_STR("m1 transmission\r\n");
	while(1)
	{	if((IOPIN0&SW1)==0)
		{	
		  UART0_STR("m1 transmission\r\n");
			CAN2_TX(m1);//data-frame
			millisecond(2000);
		}
		if((IOPIN0&SW2)==0)						 
		{
			UART0_STR("m2 transmission\r\n");
			CAN2_TX(m2);
			millisecond(2000);
		}
		if((IOPIN0&SW3)==0)
		{	
		  UART0_STR("m3 transmission\r\n");
			CAN2_TX(m3);//data-frame
			millisecond(2000);
		}
	}
}
//delay_header.h
void second(unsigned int seconds)
{
	T0PR=15000000-1;
	T0TCR=0x01;
	while(T0TC<seconds);
	T0TCR=0X03;
	T0TCR=0X00;
}
void millisecond(unsigned int millisecond)
{
	T0PR=15000-1;
	T0TCR=0x01;
	while(T0TC<millisecond);
	T0TCR=0X03;
	T0TCR=0X00;
}
void microsecond(unsigned int microseconds)
{
	T0PR=15-1;
	T0TCR=0x01;
	while(T0TC<microseconds);
	T0TCR=0X03;
	T0TCR=0X00;
}
//can.h
#define CAN 0x00014000
typedef unsigned char u8;
typedef struct CAN2
{
	u8 id;
	u8 rtr;
	u8 dlc;
	u8 byteA;
	u8 byteB;
}CAN2_MSG;
void CAN2_INIT(void)
{
	PINSEL1|=CAN;//p0.23,p0.24 can func
	VPBDIV=1;//reset mode
	C2MOD=0x1;//accept all msgs
	AFMR=0x2;//speed 125kbps
	C2BTR=0x001c001D;//normal mode
	C2MOD=0x0;
}
//uart_header.h
void UART0_CONFIG(unsigned int baud);
void UART0_TX(unsigned char);
unsigned char UART0_RX(void);
void UART0_STR(unsigned char*);
void UART0_INT(int);
void UART0_FLOAT(float);
void UART0_HEX(int);

void UART0_CONFIG(unsigned int baud)
{
	PINSEL0|=0x5;
	U0LCR=0x83;
	switch(baud){	
		case 115200:U0DLL=32;	break;//PCLK=60MHz
		case 921600:U0DLL=4;	break;
		default:	U0DLL=32	;	
	}
	U0LCR=0x03;
}

void UART0_TX(unsigned char ch)
{
	while((U0LSR&(1<<5))==0);
	U0THR=ch;
}

unsigned char UART0_RX(void)
{
	while((U0LSR&1)==0);
	return U0RBR;
}

void UART0_STR(unsigned char *s)
{
	while(*s)
	UART0_TX(*s++);
}

void UART0_INT(int n)
{
	unsigned char arr[80];
	signed char i=0;
	if(n==0)
		UART0_TX('0');
	else
	{
		if(n<0)
		{
			UART0_TX('-');
			n=-n;
		}
		while(n>0)
		{
			arr[i++]=n%10;
			n=n/10;
		}
		for(--i;i>=0;i--)
			UART0_TX(arr[i]+48);
	}
}
void UART0_FLOAT(float f)
{
	unsigned int temp;
	temp=f;
	UART0_INT(temp);
	UART0_TX('.');
	temp=(f-temp)*1000;
	UART0_INT(temp);
}
void UART0_HEX(int num){
	unsigned char buf[10];
	sprintf(buf,"%X",num);
	UART0_STR(buf);
}
//4bit.c
#include<lpc21xx.h>
#define LCD_D 0x0f<<20

#define RS 1<<17
#define RW 1<<18
#define E 1<<19

void LCD_INIT(void);
void LCD_CMD(unsigned char);
void LCD_DATA(unsigned char);
void LCD_STR(unsigned char *);
void LCD_INTEGER(int);
void FLOAT(float);

void LCD_INIT(void)
{
	IODIR1=LCD_D|RS|RW|E;
	IOCLR1=RW;
	LCD_CMD(0X01);
	LCD_CMD(0X02);
	LCD_CMD(0X0c);
	LCD_CMD(0X28);// 4bit mode access
	LCD_CMD(0X80);
}
void LCD_CMD(unsigned char CMD)
{
	IOCLR1=LCD_D;
	IOSET1=(CMD&0XF0)<<16;
	IOCLR1=RS;
	IOSET1=E;
	millisecond(2);
	IOCLR1=E;
	
	IOCLR1=LCD_D;
	IOSET1=(CMD&0X0F)<<20;
	IOCLR1=RS;
	IOSET1=E;
	millisecond(2);
	IOCLR1=E;
}
void LCD_DATA(unsigned char d)
{
	IOCLR1=LCD_D;
	IOSET1=(d&0XF0)<<16;
	IOSET1=RS;
	IOSET1=E;
	millisecond(2);
	IOCLR1=E;
	
	IOCLR1=LCD_D;
	IOSET1=(d&0X0F)<<20;
	IOSET1=RS;
	IOSET1=E;
	millisecond(2);
	IOCLR1=E;
}
void LCD_STR(unsigned char *p)
{
	int cnt=0;
	while(*p)
	{
		LCD_DATA(*p++);
		cnt++;
		if(cnt==16)
			LCD_CMD(0xc0);
	}
}
void LCD_INTEGER(int n)
{
	unsigned char arr[80];
	signed char i=0;
	if(n==0)
		LCD_DATA('0');
	else
	{
		if(n<0)
		{
			LCD_DATA('-');
			n=-n;
		}
		while(n>0)
		{
			arr[i++]=n%10;
			n=n/10;
		}
		for(--i;i>=0;i--)
			LCD_DATA(arr[i]+48);
	}
}
void FLOAT(float f)
{
	int temp;
	temp=f;
	LCD_INTEGER(temp);
	LCD_DATA('.');
	temp=(f-temp)*1000;
	LCD_INTEGER(temp);
}
//node4.c
#include<lpc21xx.h>
#include"delay_header.h"
#include"uart_header.h"
#include"can.h"
#include"4bit.c"
void CAN2_RX(CAN2_MSG* x)
{
	while((C2GSR&0x1)==0);
	x->id=C2RID;
	x->dlc=(C2RFS>>16)&0xF;
	x->rtr=(C2RFS>>30)&0x1;
	if(x->rtr==0)
	{ //if data frame
		x->byteA=C2RDA;
		x->byteB=C2RDB;
	}
	C2CMR=(1<<2);//free receiver buffer(imp)
}
main(){	
	int i=0;
	CAN2_MSG m3;
	IODIR0 |=3<<8;
	//IODIR0=LED1;
	CAN2_INIT();
	LCD_INIT();
	LCD_CMD(0x01);
	LCD_CMD(0x80);
	LCD_STR("node 4 TESTING..");
	UART0_CONFIG(115200);
	UART0_STR("node 4 TESTING\r\n");
	while(1)
	{		
		CAN2_RX(&m3);
		//CAN2_RX(&m2);
		if(m3.id==0x3)
		{
			UART0_STR("nodeD: motor\r\n");
			LCD_CMD(0x01);
			LCD_CMD(0x80);
			UART0_STR("CLOCK WISE\r\n");
			LCD_STR("CLOCK WISE");
	
			IOSET0 = 1<<8;
			IOCLR0 = 1<<9;
			second(5);
			IOSET0 = 1<<8|1<<9;
			second(1);
	
			LCD_CMD(0x01);
			LCD_CMD(0x80);
			UART0_STR("ANTI CLOCK WISE\r\n");
			LCD_STR("ANTI");
			LCD_CMD(0xC0);
			LCD_STR("CLOCK WISE");
  
			IOCLR0 = 1<<8;
			IOSET0 = 1<<9;
			second(5);
			IOSET0 = 1<<8|1<<9;
			second(1);
			
		}	
	}
}
//node3.c
#include<lpc21xx.h>
#include"delay_header.h"
#include"uart_header.h"
#include"can.h"
#include"4bit.c"
#define LED1 1<<17
unsigned char cgram_lut[]={0x10,0x18,0x1c,0x1d,0x1f,0x1c,0x18,0x10};
void cgram_write(char);
void CAN2_RX(CAN2_MSG* x)
{
	while((C2GSR&0x1)==0);
	x->id=C2RID;
	x->dlc=(C2RFS>>16)&0xF;
	x->rtr=(C2RFS>>30)&0x1;
	if(x->rtr==0)
	{ //if data frame
		x->byteA=C2RDA;
		x->byteB=C2RDB;
	}
	C2CMR=(1<<2);//free receiver buffer(imp)
}
main(){	
	int i=0;
	CAN2_MSG m2;
	IODIR0=LED1;
	CAN2_INIT();
	LCD_INIT();
	LCD_CMD(0x01);
	LCD_CMD(0x80);
	LCD_STR("node 3 TESTING..");
	UART0_CONFIG(115200);
	UART0_STR("node 3 TESTING\r\n");
	while(1)
	{		
		
		CAN2_RX(&m2);
		//CAN2_RX(&m2);
		if(m2.id==0x2){
			UART0_STR("nodeC: right LED\r\n");
			LCD_CMD(0x01);
			LCD_CMD(0x80);
			LCD_DATA(0x7e);
			
			second(2);
			IOCLR0=LED1;
			second(2);
			IOSET0=LED1;
			
			
			/*cgram_write(8);
			LCD_CMD(0x80);
			LCD_DATA(0);
			IOCLR0=LED1;
			millisecond(500);
			IOSET0=LED1;*/
			//LCD_CMD(0xc0);
			//LCD_STR("rled");
			//LCD_DATA(0X7E);
		}	
	}
}
void cgram_write(char nBytes)
{
	char k;
	LCD_CMD(0X40);
	for(k=0;k<nBytes;k++)
		LCD_DATA(cgram_lut[k]);
}
			/*while(i<=10)
			{
				UART0_STR("nodeB: RIGHT LED\r\n");
				cgram_write(8);
					LCD_CMD(0x80);
					LCD_DATA(0);
					IOCLR0=LED1;
				UART0_STR("nodeB: RIGHT LED in lcd\r\n");
					millisecond(500);
					LCD_CMD(0x01);
				i++;
			} */
//node2.c
#include<lpc21xx.h>
#include"delay_header.h"
#include"uart_header.h"
#include"can.h"
#include"4bit.c"
#define LED1 1<<17
unsigned char cgram_lut[]={0x10,0x18,0x1c,0x1d,0x1f,0x1c,0x18,0x10};
void cgram_write(char);
void CAN2_RX(CAN2_MSG* x)
{
	while((C2GSR&0x1)==0);
	x->id=C2RID;
	x->dlc=(C2RFS>>16)&0xF;
	x->rtr=(C2RFS>>30)&0x1;
	if(x->rtr==0)
	{ //if data frame
		x->byteA=C2RDA;
		x->byteB=C2RDB;
	}
	C2CMR=(1<<2);//free receiver buffer(imp)
}
main(){	
	int i=0;
	CAN2_MSG m1;
	IODIR0=LED1;
	CAN2_INIT();
	LCD_INIT();
	LCD_CMD(0x01);
	LCD_CMD(0x80);
	LCD_STR("node 2 TESTING..");
	UART0_CONFIG(115200);
	UART0_STR("node 2 TESTING\r\n");
	while(1)
	{		
		CAN2_RX(&m1);
		//CAN2_RX(&m2);
		if(m1.id==0x1){
			UART0_STR("node2: LEFT LED\r\n");
			LCD_CMD(0x01);
			LCD_CMD(0x80);
			LCD_DATA(0x7f);
			
			second(2);
			IOCLR0=LED1;
			second(2);
			IOSET0=LED1;
		}	
	}
}
void cgram_write(char nBytes)
{
	char k;
	LCD_CMD(0X40);
	for(k=0;k<nBytes;k++)
		LCD_DATA(cgram_lut[k]);
}
			/*while(i<=10)
			{
				UART0_STR("nodeB: RIGHT LED\r\n");
				cgram_write(8);
					LCD_CMD(0x80);
					LCD_DATA(0);
					IOCLR0=LED1;
				UART0_STR("nodeB: RIGHT LED in lcd\r\n");
					millisecond(500);
					LCD_CMD(0x01);
				i++;
			} */
