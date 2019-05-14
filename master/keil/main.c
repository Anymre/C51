#include <reg51.h>			//ͷ�ļ�

//����ʱ
unsigned char buf[4];							//Ƭѡ����ܱ���
unsigned int EW_time_default = 15;  	  //����Ĭ��ֵ
unsigned int SN_time_default = 15;			//�ϱ�Ĭ��ֵ

unsigned int EW_time_adj = 15;  	  //��������ֵ
unsigned int SN_time_adj = 15;			//�ϱ�����ֵ

unsigned int EW_time_now = 7;		//��������ǰ����
unsigned int SN_time_now = 10;		//�ϱ�����ǰ����
unsigned int SN_Yellow_time_now = 3;		//�ϱ�����Ƶ�
unsigned int EW_Yellow_time_now = 3;		//��������Ƶ�

//ͨ��
unsigned int traffic_light_signal = 0; //��ͨ���źű�־λ
unsigned int SN_or_EW = 1; //0���ϱ�ͨ�� 1������ͨ��
unsigned int Open = 1;
unsigned int SN_flash = 0; //�ϱ��Ƶ���˸��־
unsigned int EW_flash = 0; //�����Ƶ���˸��־

//������ʱ
unsigned int sensor1_num = 0;
unsigned int sensor2_num = 0;
unsigned int delay_sec=0;

//������
unsigned char code LED[] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90};

//����
unsigned int time0_count; //1s��ʱ
unsigned int flash_count; //0.5s��ʱ
unsigned int flash_signl; //��˸�����ź�

//��̨ͨ��
unsigned int flag = 0;
unsigned int countdown_signal=1;//��������ʱ��־λ

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
void flash();			//�Ƶ���˸
void adjustment();			//����������������ʱ��
void sendChar(unsigned int Value);			//������Ϣ
void romete_control(unsigned int romete_control_signal); //Զ�̿���
void soft_reset();//������

void main() {
	init();
	while (1) {
		display();
	}
}

void init() {
	//�����ж�
	EA = 1;
	//�ⲿ�ж�
	IT0 = 1; //������
	EX0 = 1; //�ж�����
	IT1 = 1; //������
	EX1 = 1; //�ж�����

	ET0 = 1; //����ʱ��0
	//��ʱ��0��ʼ��
	Timer0Init();
}

void time0()
interrupt 1
{
	//���ö�ʱ��
	TH0 = 0x4c;
	TL0 = 0x00;
	//��������ʱ
	LED_light();
	//�����źŵ�
	traffic_light();
	//�����Ƶ�
	flash();
}

void romete_control(unsigned int romete_control_signal)//Զ�̿���
{
	switch (romete_control_signal)
	{
	case 0://����ģʽ
	traffic_light_signal=0;countdown_signal=1;SN_time_now=SN_time_default;EW_time_now=EW_time_default;break;
	case 1://ǿ�ƶ���ͨ��
	EW_time_now=99;SN_time_now=99;traffic_light_signal=0;countdown_signal=0;break;
	case 2://ǿ���ϱ�ͨ��
	EW_time_now=99;SN_time_now=99;traffic_light_signal=2;countdown_signal=0;break;
	case 3://��ֹͨ��
	EW_time_now=99;SN_time_now=99;traffic_light_signal=4;countdown_signal=0;break;
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

void traffic_light() //�źŵ�
{
	switch (traffic_light_signal) {
	case 0: //��������ͨ��  �̵���
		Green_EW = Open;
		Red_EW = !Open;
		EW_flash = !Open;

		Green_SN = !Open;
		Red_SN = Open;
		SN_flash = !Open;

		break;
	case 1: //��������ͨ��  �Ƶ���
		Green_EW = !Open;
		Red_EW = !Open;
		EW_flash = Open;

		Green_SN = !Open;
		Yellow_SN = !Open;
		Red_SN = Open;
		SN_flash = !Open;

		break;
	case 2: //�ϱ�����ͨ��  �̵���
		Green_EW = !Open;
		Red_EW = Open;
		EW_flash = !Open;

		Green_SN = Open;
		Red_SN = !Open;
		SN_flash = !Open;

		break;
	case 3: //�ϱ�����ͨ��  �Ƶ���
		Green_EW = !Open;
		Red_EW = Open;
		EW_flash = !Open;

		Green_SN = !Open;
		Red_SN = !Open;
		SN_flash = Open;

		break;
		case 4: //��ֹͨ��
		Green_EW = !Open;
		Red_EW = Open;
		EW_flash = !Open;

		Green_SN = !Open;
		Red_SN = Open;
		SN_flash = !Open;

		break;
	}
}

void flash() {
	flash_count++;
	if (flash_count >= 10) { //0.5s��˸һ��
		flash_count = 0;
		flash_signl++;
		if (flash_signl > 10)
			flash_signl = 0; //��˸�ź�����

		//�Ƶ���˸
		if (SN_flash) {
			Yellow_SN = flash_signl % 2;
		} else {
			Yellow_SN = !Open;
		}
		if (EW_flash) {
			Yellow_EW = flash_signl % 2;
		} else {
			Yellow_EW = !Open;
		}
	}
}

void LED_light() //����ʱ
{
	time0_count++;
	if (time0_count >= 20 &&countdown_signal) {
		//ÿ����һ�Σ���������ã����л�ͨ�з���
		time0_count = 0;
		SN_time_now--;
		EW_time_now--;
		//����ͨ��
		if (SN_or_EW) {
			if (EW_time_now > 0 && EW_flash == 0) //�����̵�
					{
				traffic_light_signal = 0;
				EW_flash = 0;
			}
			if (EW_time_now <= 0 && EW_flash == 0) //�̵Ƶ���ʱ�������л��Ƶ�
					{
				EW_time_now = EW_Yellow_time_now;
				EW_flash = 1;
				traffic_light_signal = 1;
			}
			if (EW_time_now <= 0 && EW_flash == 1) //�ƵƵ���ʱ�������л�ͨ�з���
					{
				EW_flash = 0; //�رջƵ�
				adjustment();//�����̵�ʱ��
				EW_time_now = SN_time_adj+EW_Yellow_time_now; 
				SN_time_now = SN_time_adj;

				SN_time_adj=SN_time_default;
				EW_time_adj=EW_time_default;


				SN_or_EW = !SN_or_EW;
			}
			if (SN_time_now <= 0)   //�����������ȴ�ʱ��
					{
				SN_time_now = EW_time_now;
			}
		}
		//�ϱ�ͨ��
		else {
			if (SN_time_now > 0 && SN_flash == 0)   //�����̵�
					{
				traffic_light_signal = 2;
				SN_flash = 0;
			}
			if (SN_time_now <= 0 && SN_flash == 0)   //�̵Ƶ���ʱ�������л��Ƶ�
					{
				SN_time_now = SN_Yellow_time_now;
				SN_flash = 1;
				traffic_light_signal = 3;
			}
			if (SN_time_now <= 0 && SN_flash == 1)   //�ƵƵ���ʱ�������л�ͨ�з���
					{
				adjustment();
				EW_time_now = EW_time_adj; 
				SN_time_now = EW_time_now+SN_Yellow_time_now;
				SN_time_adj=SN_time_default;
				EW_time_adj=EW_time_default;
						
				SN_flash = 0;
				SN_or_EW = !SN_or_EW;
			}
			if (EW_time_now <= 0)   //�ϱ��������ȴ�ʱ��
					{
				EW_time_now = SN_time_now;
			}
		}
	}
}
void adjustment() //�������������̵�ʱ��
{
	if (SN_or_EW) {delay_sec += sensor1_num / 3;}else{delay_sec += sensor2_num / 3;}
		  delay_sec=(delay_sec>10)? 10 :delay_sec;
	if (SN_or_EW) {
			SN_time_adj+=delay_sec;
	} else {
			EW_time_adj+=delay_sec;
	}
	//���͵�ǰ���ݵ���̨
	sendChar(sensor1_num+sensor1_num);
	
	//����ʱ������
	delay_sec=0;
	//���˼�������
	sensor1_num = 0;
	sensor2_num = 0;
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

	TMOD = 0x21;	//���ö�ʱ��ģʽ
	TL0 = 0x00;		//���ö�ʱ��ֵ
	TH0 = 0x4C;		//���ö�ʱ��ֵ
	TF0 = 0;		  //���TF0��־
	TR0 = 1;		  //��ʱ��0��ʼ��ʱ

	//Time1��ʼ�� ������9600
	//TMOD = 0x20;	//���ö�ʱ��ģʽ
	SCON = 0x50;
	PCON = 0x00;
	TL1 = 0xFD;
	TH1 = 0xFD;
	TR1 = 1;
	ES = 1;
}
void sendChar(unsigned int Value) {
	SBUF = Value;
	flag = 1;
}

void sensor1()
interrupt 0
{
					sensor1_num++;
}

void sensor2()
interrupt 2
{
					sensor2_num++;
}
void serial() interrupt 4 {
	if(RI)
	{
		RI = 0;
		romete_control(SBUF);
	}

	if(TI)
	{
		TI = 0;
	}
}
void soft_reset()
{
((void (code *) (void)) 0x0000) ();
}

