#include <reg51.h>			//ͷ�ļ�


//����ʱ
unsigned char buf[4];							//����ʾ�ı���
unsigned int EW_time_default = 25;  	  //����Ĭ��ֵ
unsigned int SN_time_default = 30;			//�ϱ�Ĭ��ֵ
unsigned int EW_time_now = 25;		//��������ǰ����
unsigned int SN_time_now = 30;		//�ϱ�����ǰ����

//ͨ��
unsigned int SN_or_EW=0; //0���ϱ�ͨ�� 1������ͨ��

//������
unsigned char code LED[] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90};


unsigned char code NUM[] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x40}; 


//����
unsigned int time0_count;

//���̵�
sbit Green_EW = P2 ^ 1;		//����
sbit Yellow_EW = P2 ^ 2;
sbit Red_EW = P2 ^ 3;

sbit Red_SN = P2 ^ 6;		//�ϱ�
sbit Yellow_SN = P2 ^ 5;
sbit Green_SN = P2 ^ 4;

//��������
void delay(int ms);			//��ʱ
void display();				//��ʾ����ʱ
void init();   				//������ʼ��
void Timer0Init();       //��ʱ��0

void main()
{
    init();
    while (1) {
        display();
    }
}

void init()
{
    //�����ж�
    EA = 1;
	  ET0 = 1;      //����ʱ��0
    //��ʱ��0
    Timer0Init();

}

void time0() interrupt 1
{
    //���ö�ʱ��
    TH0 = 0x4c;
    TL0 = 0x00;
	  time0_count++;
    if(time0_count < 20) {}
    else {
        time0_count = 0;
        if(SN_time_now > 0) {
            SN_time_now--;
        } else {
            SN_time_now = EW_time_default;
        }
        if(EW_time_now > 0) {
            EW_time_now--;
        } else {
            EW_time_now = EW_time_default;
        }
    }
}

void display() //��ʾ�ӳ���
{
	    
	
    //�ϱ������λʮλ
    buf[0] = SN_time_now / 10;
    buf[1] = SN_time_now % 10;
    //���������λʮλ
    buf[2] = EW_time_now / 10;
    buf[3] = EW_time_now % 10;

    //�����ϱ����򵹼�ʱ
    P1 = 0x01;           		//ƬѡLED1
    P0 = LED[buf[0]];			//���ϱ�ʱ��ʮλ������ܱ���
    delay(1);				//��ʱ			
	
    P1 = 0x02;             	//ƬѡLED2
    P0 = LED[buf[1]];
    delay(1);
		
    P1 = 0X04;		  		//ƬѡLED3
    P0 = LED[buf[2]];		    //�Ͷ���ʱ��ʮλ������ܱ���
    delay(1);				//��ʱ
		
    P1 = 0X08;				//ƬѡLED4
    P0 = LED[buf[3]];
    delay(1);	
}

void delay(int ms)			//��ʱ�ӳ���
{
    unsigned int j, k;
    for (j = 0; j < ms; j++)
        for (k = 0; k < 124; k++)
            ;
}

void Timer0Init()		//50����@11.0592MHz
{
    TMOD = 0x01;	//���ö�ʱ��ģʽ
    TL0 = 0x00;		//���ö�ʱ��ֵ
    TH0 = 0x4C;		//���ö�ʱ��ֵ
    TF0 = 0;		  //���TF0��־
    TR0 = 1;		  //��ʱ��0��ʼ��ʱ
}

