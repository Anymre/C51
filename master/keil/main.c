#include <reg51.h>			//ͷ�ļ�


//����ʱ
unsigned char buf[4];							//Ƭѡ����ܱ���
unsigned int EW_time_default = 15;  	  //����Ĭ��ֵ
unsigned int SN_time_default = 10;			//�ϱ�Ĭ��ֵ
unsigned int EW_time_now = 15;		//��������ǰ����
unsigned int SN_time_now = 10;		//�ϱ�����ǰ����

//ͨ��
unsigned int SN_or_EW=0; //0���ϱ�ͨ�� 1������ͨ��
unsigned int Open=1;
unsigned int SN_flash=0; //�ϱ��Ƶ���˸��־
unsigned int EW_flash=0; //�����Ƶ���˸��־

//������
unsigned char code LED[] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90};


//����
unsigned int time0_count; //1s��ʱ
unsigned int flash_count; //0.5s��ʱ
unsigned int flash_signl; //��˸�����ź�

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
void traffic_light(); //�����źŵ�
void LED_light();      //���Ƶ���ʱ
void flash();					//�Ƶ���˸

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
		//�����źŵ�
		traffic_light();
	  //��������ʱ
		LED_light();
	  //�����Ƶ�
		flash();
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

void traffic_light() //�źŵ�
{         //
	if((SN_or_EW))
	{
		//��������ͨ��  �̵���
    if(EW_time_now>5){Green_EW=Open;EW_flash=!Open;} //С��5sʱ���Ƶ���˸
		else{Green_EW=!Open;EW_flash=Open;}
		Red_EW=!Open;
		//�ϱ���������� �����
		SN_flash=!Open;  //�رջƵ�
		Green_SN=!Open;
		Red_SN=Open;
  }
	else{
		//�����������  �����
		EW_flash=!Open; //�رջƵ�
    Green_EW=!Open;
		Red_EW=Open;
		//�ϱ�����ͨ�����̵���
		if(SN_time_now>5){Green_SN=Open;SN_flash=!Open;} //С��5sʱ���Ƶ���˸
		else{Green_SN=!Open; SN_flash=Open;}
		Red_SN=!Open;
}
}

void flash()
{
	flash_count++;
	if(flash_count>=10)  //0.5s��˸һ��
		{
			flash_count=0;
			flash_signl++;
			if(flash_signl>10)
				flash_signl=0; //��˸�ź�����
			
			//�Ƶ���˸
			if(SN_flash)
			{Yellow_SN=flash_signl%2;}
			else {Yellow_SN=!Open;}
			if(EW_flash)
			{Yellow_EW=flash_signl%2;}
			else {Yellow_EW=!Open;}
		}
}

void LED_light() //����ʱ
{
		time0_count++;
    if(time0_count >= 20) 
		{
			//ÿ����һ�Σ���������ã����л�ͨ�ŷ���
        time0_count = 0;
        if(EW_time_now > 0) {
            SN_time_now--;
					  EW_time_now--;
					 if(SN_time_now<=0)
						 {
							 SN_time_now=EW_time_now;
						}
        } else {
            SN_time_now = SN_time_default;
					  EW_time_now = EW_time_default;
						SN_or_EW=!SN_or_EW;
        }
			}
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

