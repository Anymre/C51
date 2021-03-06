#include<reg51.h>
#define uchar unsigned char
#define uint unsigned int
sbit A1 = P1 ^ 1;
sbit B1 = P1 ^ 0;
sbit BU = P1 ^ 2;
uint time = 10;
uint count = 20;
uint flag = 0;
char code LED[] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90};
void delay(int ms)  	//延时子程序
{
    uint j, k;
    for (j = 0; j < ms; j++)			//延时ms
        for (k = 0; k < 124; k++)
            ;		//大约1毫秒的延时
}
void Delay10us()		//@11.0592MHz
{
    unsigned char i;

    i = 35;
    while (--i);
}



void display();

void init()
{
    EA = 1;
    TMOD = 0x01;
    ET0 = 1;						//定时器0中断开启
    TH0 = 0x4c;
    TL0 = 0x00;
    TR0 = 1;
    TF0 = 0;
}
void init2()
{
    TMOD = 0x20;
    SCON = 0x50;
    PCON = 0x00;
    TL1 = 0xFD;
    TH1 = 0xFD;
    TR1 = 1;
    EA = 1;
    ES = 1;
}

void sendChar(uchar Value)
{
    SBUF = Value;
    flag = 1;
    while(flag);
}

void sendAll(uchar *pValue)
{
    while((*pValue) != '\0') {
        sendChar(*pValue);
        pValue++;
    }
}

void main()
{
    init2();
    while(1) {
        display();
        if(BU==0) {
            Delay10us();
                sendAll("hello world!");
								time=20;
        }
    }
}

void display()   //显示子程序
{
    A1 = 1;
    P0 = LED[time % 10];
    delay(1);
    A1 = 0;
    B1 = 1;
    P0 = LED[time / 10];
    delay(1);
    B1 = 0;
}

void testinter()
{

    TH0 = 0x4c;
    TL0 = 0x00;
    if(count < 20) {
        count++;
    } else {
        count = 0;
        if(time > 0) {
            time--;
        } else {
            time = 10;
        }
    }
}
void serial() interrupt  4
{
    if(RI) {
        RI = 0;
        P0 = SBUF;
        flag = 1;
    }

    if(TI) {
        TI = 0;
        flag = 0;
    }
}