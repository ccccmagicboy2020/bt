#define ALLOCATE_EXTERN
#include "bluetooth.h"

volatile ulong Timer_Counter = 0;


u8 xdata SUM1_counter = 0; //ƫ��ƽ��ֵ�ļ��������
u8 xdata SUM0_num = 12;	   //SUM0��������
u8 xdata SUM1_num = 64;	   //SUM1��������
ulong xdata SUM01;		   //��һ�ѵ�SUM0��ֵ
ulong xdata SUM2;		   //������snapshot of the SUM1 value
ulong xdata SUM10 = 0;	   //SUM1ֵ�ļ���ƽ��ֵ��ʱ���ϵ��ͺ�ֵ
ulong xdata SUM0 = 0;	   //SUM10��ƽ��ֵ
ulong xdata SUM1 = 0;	   //ƽ�����������ۼӺϵ�˲ʱֵ
ulong xdata ALL_SUM1 = 0;  //SUM1���ۼ�ֵ
ulong xdata SUM16 = 0;	   //2^16�ε��ۼ�ֵ����
ulong xdata SUM = 0;	   //an1��raw�ۼ�ֵ
u16 xdata start_times = 1; //???
u16 xdata times = 0;	   //��ѭ������
ulong xdata TH;			   //���������ֵ������APP���õĸ�Ӧǿ��ת��
ulong xdata MAX_DELTA; //���ƫ��ֵ
u8 xdata alarm_times = 0;
u8 xdata stop_times = 0; //???

uint xdata LIGHT = 0;	  //��������ļ�����
uint xdata LIGHT_off = 0; //������Ƶķ��Ӽ�����
uint xdata average;		  //an1��rawƽ��ֵ

u8 xdata light_ad;	//����ʵʱֵraw
u8 xdata light_ad0; //������ʼ˲ʱֵraw

u8 xdata check_light_times = 8;	 //���ڹ������ļ�����
u8 xdata calc_average_times = 0; //���ڼ���ƽ��ֵ�ļ�����
u8 xdata LIGHT_TH;
u16 xdata DELAY_NUM;
u8 xdata lowlightDELAY_NUM;
u8 xdata while_1flag = 0;		  //������־ 1==����״̬ 0==���״̬
u8 xdata while_2flag = 0;		  //???

u8 xdata SWITCHflag2 = 0; //�ƿ��صı���������APP����
u8 xdata SWITCHfXBR = 1;  //�״��Ӧ���صı���������APP����
u8 xdata lightvalue = 10; //����ֵ������APP����
u8 xdata switchcnt = 0;
u8 xdata slowchcnt = 10;				  //���Ƚ���Ŀ��ֵ
u8 xdata resetbtcnt = 0;				  //Ϊ��������ģ�����õļ�����
u8 xdata XRBoffbrightvalue = 0;			  //���ر��״�ʱ��APP���õ�����ֵ(΢��ֵ)
volatile u16 xdata lowlight1mincount = 0; //timer�ļ�����1ms�Լ�
volatile u8 xdata lowlight1minflag = 0;	  //timer�ķ��ӱ�־
volatile u16 idata light1scount = 0;	  //timer�ļ�����1ms�Լ�
volatile u16 idata light1sflag = 0;		  //timer�����־


u8 xdata bt_join_cnt = 0;



u16 xdata radar_number_count = 0;
u8 xdata radar_number_send_flag = 0;
u8 xdata radar_number_send_flag2 = 0;
unsigned char xdata upload_disable = 0;
unsigned char PWM3init(unsigned char ab);
void Flash_EraseBlock(unsigned int fui_Address); //��������
void FLASH_WriteData(unsigned char fuc_SaveData, unsigned int fui_Address);
void Flash_ReadArr(unsigned int fui_Address, unsigned char fuc_Length, unsigned char *fucp_SaveArr); //��ȡ���ⳤ������
void savevar(void);
void reset_bt_module(void);

unsigned char xdata guc_Read_a[16] = {0x00}; //���ڴ�Ŷ�ȡ������
unsigned char xdata guc_Read_a1[2] = {0x00}; //���ڴ�Ŷ�ȡ������

void Flash_ReadArr(unsigned int fui_Address, unsigned char fuc_Length, unsigned char *fucp_SaveArr)
{
	while (fuc_Length--)
		*(fucp_SaveArr++) = *((unsigned char code *)(fui_Address++)); //��ȡ����
}

void Delay_ms(uint t)
{
	Timer_Counter = 0;
	while (Timer_Counter < t)
	{
		WDTC |= 0x10; //�忴�Ź�
	}
}

void Delay_us(uint q1)
{
	uint j;
	for (j = 0; j < q1; j++)
	{
		;
	}
}

/***************************************************************************************
  * @˵��  	ϵͳ��ʼ������
  *	@����	  ��
  * @����ֵ ��
  * @ע		  ��
***************************************************************************************/
void InitSYS()
{
	/********************************ϵͳƵ�ʳ�ʼ��***************************************/

	CLKSWR = 0x51;	 //ѡ���ڲ���ƵRCΪϵͳʱ�ӣ��ڲ���ƵRC 2��Ƶ��Fosc=16MHz
	CLKDIV = 0x01;	 //Fosc 1��Ƶ�õ�Fcpu��Fcpu=16MHz
	FREQ_CLK = 0x10; //IAPƵ��

	/**********************************��ѹ��λ��ʼ��**************************************/

	//	BORC = 0xC0;											 //ʹ�ܵ�ѹ��λ1.8V��������ʹ��
	//	BORDBC = 0x01;										 //����ʱ��BORDBC*8TCPU+2TCPU

	/***********************************���ſڳ�ʼ��***************************************/
	WDTC = 0x5F;   //����WDT��λ������ģʽ�½�ֹWDT��ѡ��1024��Ƶ���ڲ���Ƶʱ��44K��
	WDTCCR = 0X20; //0X20/44	=0.73��						//0xFF;	 //���ʱ��Լ6��
				   //�������ʱ��=��WDT��Ƶϵ��*��WDTCCR+1����/�ڲ���ƵRCƵ��
}

/***************************************************************************************
  * @˵��  	��ʱ����ʼ������
  *	@����	  ��
  * @����ֵ ��
  * @ע		  ��
***************************************************************************************/
void Timer_Init()
{
	/**********************************TIM1���ó�ʼ��**************************************/
	TCON1 = 0x00; //T1��ʱ��ʱ��ΪFosc
	TMOD = 0x01;  //T1-16λ��װ�ض�ʱ��/������,T0-16λ��ʱ��

	//Tim1����ʱ�� 	= (65536 - 0xFACB) * (1 / (Fosc /Timer��Ƶϵ��))
	//				= 1333 / (16000000 / 12)
	//				= 1 ms

	//T1��ʱ1ms
	//���Ƴ�ֵ 	= 65536 - ((1/1000) / (1/(Fosc / Timer��Ƶϵ��)))
	//		   	= 65536 - ((1/1000) / (1/(16000000 / 12)))
	//			= 65536 - 1333
	//			= 0xFACB

	TH1 = 0xFA;
	TL1 = 0xCB;	  //T1��ʱ1ms
	IE |= 0x08;	  //��T1�ж�
	TCON |= 0x40; //ʹ��T1

	TH0 = 0xCB;
	TL0 = 0xEB; //T0��ʱʱ��10ms

	TCON |= 0x10; //ʹ��T0
}

/***************************************************************************************
  * @˵��  	UART1��ʼ������
  *	@����	  ��
  * @����ֵ ��
  * @ע		  ��
***************************************************************************************/
void UART1_Init()
{
	/**********************************UART���ó�ʼ��**************************************/
	P2M0 = P2M0 & 0xF0 | 0x08; //P20����Ϊ�������
	P0M2 = P0M2 & 0xF0 | 0x02; //P04����Ϊ��������
	P0_4 = 1;
	TXD_MAP = 0x20; //TXDӳ��P20
	RXD_MAP = 0x04; //RXDӳ��P04
	T4CON = 0x06;	//T4����ģʽ��UART1�����ʷ�����

	//�����ʼ���
	//������ = 1/16 * (T4ʱ��ԴƵ�� / ��ʱ��4Ԥ��Ƶ��) / (65536 - 0xFF98)
	//       = 1/16 * ((16000000 / 1) / 104)
	//		 = 9615.38(���0.16%)

	//������9600
	//���Ƴ�ֵ = (65536 - ((T4ʱ��ԴƵ�� / ��ʱ��4Ԥ��Ƶ��) * (1 / 16)) / ������)
	//		   = (65536 - (16000000 * (1 / 16) / 9600))
	//		   = (65536 - 104.167)
	//         = FF98
	//0xFF98->9600
	//0xFFCC->19200
	//0xFFEF->57600

	TH4 = 0xFF;
	TL4 = 0x98;	  //������9600		//0xEE;				//������56000
	SCON2 = 0x02; //8λUART�������ʿɱ�
	SCON = 0x10;  //�����н���
	IE |= 0X10;	  //ʹ�ܴ����ж�
				  //EA = 1;							              	 //ʹ�����ж�
}

/***************************************************************************************
  * @˵��  	ADC��ʼ������
  *	@����	  ��
  * @����ֵ ��
  * @ע		  ��
***************************************************************************************/
void ADC_Init()
{

	ADCC0 |= 0x03; //�ο�ԴΪ�ڲ�2V
	ADCC0 |= 0x80; //��ADCת����Դ
	Delay_us(20);  //��ʱ20us��ȷ��ADCϵͳ�ȶ�

#ifdef XBR403_03_2
	ADCC1 = 0x00;  //ѡ���ⲿͨ��0
#endif
	
#ifdef XBR403
	ADCC1 = 0x02;  //ѡ���ⲿͨ��2
#endif

#ifdef V12
	ADCC1 = 0x01;  //ѡ���ⲿͨ��1
#endif
	
	ADCC2 = 0x4B;  //8��Ƶ	  //ת�����12λ���ݣ������Ҷ��룬ADCʱ��16��Ƶ-1MHZ//0X4B-8��Ƶ//0X49-4��Ƶ
}

/***************************************************************************************
  * @˵��  	IO�ڳ�ʼ������
  *	@����	  ��
  * @����ֵ ��
  * @ע		  ��
***************************************************************************************/
void GPIO_Init()
{
	//P0M0�ָ�4λ���4λ����4λ����P00�����������4λ����P01��������������Դ�����
	//P0M1��4����P03����4����P02
	//P1M2��4����P15����4����P14

	// 	P0M0 = P0M0&0xF0|0x08;		      //P00����Ϊ�������
	// 	P0M0 = P0M0&0x0F|0x30;				  //P01����Ϊģ������
	// 	P0M3 = P0M3&0x0F|0x30;				  //P07����Ϊģ������
	// 	P0M3 = P0M3&0xF0|0x08;		      //P06����Ϊ�������

#ifdef V11

	P0M0 = P0M0 & 0xF0 | 0x08; //P00����Ϊ�������

	P0M0 = P0M0 & 0x0F | 0x30; //P01����Ϊģ������
	//P0M3 = P0M3&0x0F|0x30;				  //P07����Ϊģ������
	//	P0M0 = P0M0&0x0F|0x80;		      //P01����Ϊ�������

	P0M2 = P0M2 & 0x0F | 0x80; //P05����Ϊ�������

	P0M3 = P0M3 & 0xF0 | 0x03; //P06����Ϊģ������  //|0x08;		      //P06����Ϊ�������

	P0M3 = P0M3 & 0x0F | 0x20; //P07����Ϊ��������

#endif

#ifdef V10

	P0M0 = P0M0 & 0xF0 | 0x08; //P00

	P0M0 = P0M0 & 0x0F | 0x30; //P01
	P0M3 = P0M3 & 0x0F | 0x30; //P07
							   //	P0M0 = P0M0&0x0F|0x80;		      //P01

	P0M3 = P0M3 & 0xF0 | 0x08; //P06

#endif

#ifdef V12

	P1M0 = P1M0 & 0xFF | 0x88; //P10����Ϊ�������
							   //P11����Ϊ�������
							   
							   

	P0M0 = P0M0 & 0x0F | 0x30; //P01����Ϊģ������

	P2M1 = P2M1 & 0xF0 | 0x03; //P22����Ϊģ������

	//P0M3 = P0M3&0x0F|0x30;				  //P07����Ϊģ������
	//	P0M0 = P0M0&0x0F|0x80;		      //P01����Ϊ�������

	//P0M2 = P0M2&0x0F|0x80;		      //P05����Ϊ�������

	//P0M3 = P0M3&0xF0|0x03;			//P06����Ϊģ������  //|0x08;		      //P06����Ϊ�������

	//	P0M3 = P0M3&0x0F|0x20;				  //P07����Ϊ��������

#endif

#ifdef XBR403
	//PWM & ADC
	P1M0 = P1M0 & 0xF0 | 0x08; //P10����Ϊ�������
	P0M0 = P0M0 & 0xFF | 0x88; //P00����Ϊ�������
							   //P01����Ϊ�������
	P0M1 = P0M1 & 0xFF | 0x83; //P03����Ϊ�������
							   //P02����Ϊģ������
	P0M3 = P0M3 & 0xF0 | 0x08; //P06����Ϊ�������
	P2M1 = P2M1 & 0xF0 | 0x03; //P22����Ϊģ������
#endif

#ifdef XBR403_03_2
	 //PWM & ADC
	 P1M0 = P1M0 & 0x0F | 0x80; //P11  r
	 P0M0 = P0M0 & 0xF0 | 0x03; //P00  if adc an0
	 P0M0 = P0M0 & 0x0F | 0x80; //P01  ww
	 P0M1 = P0M1 & 0xF0 | 0x08; //P02  g
	 P0M1 = P0M1 & 0x0F | 0x80; //P03  b
	 P0M3 = P0M3 & 0x0F | 0x30; //P07  light adc an7
	 P2M3 = P2M3 & 0x0F | 0x80; //P27  cw
#endif

}

void send_data(u8 d)
{
	SBUF = d;
	while (!(SCON & 0x02))
		;
	SCON &= ~0x02;
}

//return 8-bit adc raw
uchar read_ad(uchar ch)
{
	u8 i;
	uint ad_sum;

	ADCC1 = ch;	   //ѡ���ⲿͨ��
	ADCC0 |= 0x40; //����ADCת��
	while (!(ADCC0 & 0x20))
		;			//�ȴ�ADCת������
	ADCC0 &= ~0x20; //�����־λ

	Delay_us(100);

	ad_sum = 0;

	for (i = 0; i < 16; i++)
	{
		ADCC0 |= 0x40; //����ADCת��
		while (!(ADCC0 & 0x20))
			;			//�ȴ�ADCת������
		ADCC0 &= ~0x20; //�����־λ
		ad_sum += ADCR; //��ȡADC��ֵ

		Delay_us(20);
	}

#ifdef XBR403_03_2
	ADCC1 = 0x00;  //ѡ���ⲿͨ��0
#endif

#ifdef XBR403
	ADCC1 = 0x02;  //ѡ���ⲿͨ��2
#endif

#ifdef V12
	ADCC1 = 0x01;  //ѡ���ⲿͨ��1
#endif
	
	i = ad_sum >> 8;

	Delay_us(100);
	return (i);
}

void set_var(void)
{
	Flash_ReadArr(USER_PARAMETER_START_SECTOR_ADDRESS0, 16, guc_Read_a); //��ȡ��ַ��������

	//��ȡ��Ӧ����
	TH = guc_Read_a[0];
	TH <<= 8;
	TH += guc_Read_a[1];
	TH *= 1000;
	if (TH < TH_LOW || TH > TH_HIGH)
	{
		TH = TH_DEF;
	}

	//��ȡ��������
	LIGHT_TH = guc_Read_a[2];

	if (LIGHT_TH == 0)
	{
		LIGHT_TH = LIGHT_TH0;
	}

	//�״�Ŀ��أ�Ĭ���Ǵ򿪵�
	SWITCHfXBR = (~guc_Read_a[7]) & 0x01;
	
	//
	Flash_ReadArr(USER_PARAMETER_START_SECTOR_ADDRESS1, 2, guc_Read_a1); //
	resetbtcnt = guc_Read_a1[0];
	bt_join_cnt = guc_Read_a1[1];
	Flash_EraseBlock(USER_PARAMETER_START_SECTOR_ADDRESS1);	
	Delay_us(10000);
	
	resetbtcnt++;
	
	FLASH_WriteData(resetbtcnt, USER_PARAMETER_START_SECTOR_ADDRESS1 + 0);
	Delay_us(100);	

	if (0 == bt_join_cnt)
	{
		reset_bt_module();
	}
	else if (1 == bt_join_cnt)
	{
		FLASH_WriteData(bt_join_cnt, USER_PARAMETER_START_SECTOR_ADDRESS1 + 1);
		Delay_us(100);		
	}
}

void XBRHandle(void)
{
	u16 k;

	if (while_1flag == 0)//���״̬
	{
		ADCC0 |= 0x40; //����ADCת��
		while (!(ADCC0 & 0x20))
			;			//�ȴ�ADCת������
		ADCC0 &= ~0x20; //�����־λ
		k = ADCR;		//��ȡADC��ֵ

		times++;

		SUM += k;

		//�����ź�ֵ��ֱ����ѹƫ��ֵ
		if (k > average)
		{
			k -= average;
		}
		else
		{
			k = average - k;
		}
		SUM1 += k;

		if ((times & 0x1ff) == 0) //ÿ256��ѭ�����һ�ι���
		{
			if (LIGHT > 0) //���ڰ����Ĺ�����
			{
#ifndef RELAY
				if (slowchcnt < 100)
				{
					slowchcnt = slowchcnt + 2; //
					if (slowchcnt > 100)
					{
						slowchcnt = 100;
					}
				}
#else
				slowchcnt = 100;
#endif
				PWM3init(slowchcnt);
			}
			else if (LIGHT_off == 1) //else if((SWITCHflag2==0)&&(LIGHT_off ==1))
			{
#ifndef RELAY			
				if (slowchcnt > lightvalue)
				{
					if (slowchcnt >= 2)
						slowchcnt -= 2;
					if (slowchcnt < lightvalue)
						slowchcnt = lightvalue;
				}
#else
				slowchcnt = lightvalue;
#endif
				PWM3init(slowchcnt);
			}
		}

		if (times >= 8192) //ÿ250ms�������ж�һ��
		{

			WDTC |= 0x10; //�忴�Ź�

			times = 0;

			calc_average_times++;

			SUM16 += SUM;

			if (calc_average_times >= 8) //ÿ2.5S���¼���һ��ֱ����ѹֵ
			{
				calc_average_times = 0;

				SUM16 >>= 16;
				//SUM16/=96000;//102400;
				average += SUM16;
				average /= 2;	//�ó�ƽ��ֵ
				SUM16 = 0;
			}

			if (check_light_times < 8) //2s	��ȡһ�θй�ADֵ��˲ʱ�Ա�ֵÿ2s���Ҹ���һ��
			{
				check_light_times++;
			}
			else
			{
				if (LIGHT == 0)	//����δ��ʼ��Ҳ����δ��⵽�״�Ŀ��
				{
					
					#ifdef XBR403_03_2
						light_ad = read_ad(7); //�л���an7
					#endif

					#ifdef XBR403
						light_ad = read_ad(10); //�л���an10
					#endif

					#ifdef V12
						light_ad = read_ad(10); //�л���an10
					#endif

					if ((light_ad <= (light_ad0 + 2)) && (light_ad0 <= (light_ad + 2)))
						light_ad = light_ad0;

					light_ad0 = light_ad;

					check_light_times = 0;
				}
			}

			if (SUM0 == 0)
			{
				SUM0 = SUM1 + 5000;
				if (start_times == 0 && SUM0 > 1000000)
					SUM0 = 1000000; //��Ƴ�ֵ
			}

			if (SUM1_counter == 0)
			{
				SUM10 = SUM1;
				MAX_DELTA = 1;
			}

			if ((SUM10 < (SUM1 + MAX_DELTA)) && (SUM1 < (SUM10 + MAX_DELTA))) //???????????
			{
				SUM1_counter++;
				ALL_SUM1 += SUM1;
				SUM10 = ALL_SUM1 / SUM1_counter;
				MAX_DELTA = SUM10 >> 3; //����ͻ��(���ƫ��ֵ)
				if (MAX_DELTA < MAX_DELTA0)
					MAX_DELTA = MAX_DELTA0;
				if (MAX_DELTA > MAX_DELTA1)
					MAX_DELTA = MAX_DELTA1; //��֤���ƫ��ֵ��һ����Χ��

				if (SUM0 > SUM10)
				{
					SUM = SUM0 - SUM10;
					if (SUM > 80000)
						SUM0_num = 6;
					else if (SUM > 40000)
						SUM0_num = 9;
					else
						SUM0_num = 12;
				}
				else
				{
					SUM0_num = 12;
				}

				if ((SUM1_counter >= SUM0_num) && (SUM10 < SUM0))
				{
					if (SUM1_num > 16) //
					{
						if (SUM0_num <= 9)
							SUM0 = SUM10;
						else if (SUM0 > (SUM10 + 4000))
						{
							SUM0 += SUM10;
							SUM0 /= 2;
						}
						SUM1_counter = 0;
						ALL_SUM1 = 0;
					}
				}

				else if (SUM1_counter >= SUM1_num)
				{
					if (SUM10 > (SUM0 + 4000))
					{
						SUM = SUM10 - SUM0;

						if ((SUM10 < 8000000) && (SUM < 400000))
						{
							if (SUM1_num > 16)
							{
								SUM0 += SUM10;
								SUM0 /= 2;
							}
							else
							{
								if (SUM > 300000)
									SUM1_num = 16;
								else if (SUM > 150000)
									SUM1_num = 12;
								else
									SUM1_num = 8;
								if (SUM1_counter >= SUM1_num)
								{
									SUM0 += SUM10;
									SUM0 /= 2;
								}
							}
						}
					}
					if (SUM1_counter >= SUM1_num)
					{
						SUM1_counter = 0;
						ALL_SUM1 = 0;
					}
				}
			}
			else
			{
				SUM1_counter = 0;
				ALL_SUM1 = 0;
			}

			if (stop_times > 0) //
			{
				stop_times--;
				if ((SUM0 > (SUM01 + 6000)) && (SUM1 < (SUM01 + 15000)))
					SUM0 = SUM01 + 6000;
			}
			else
			{

				if (start_times > 0)
				{
					start_times--;

					if (start_times > 0)
					{
						if (SUM0 > 8000000)
						{
							TH = 800000;
						}
						else
						{
							SUM = SUM0 + TH;
							if (SUM > 9000000)
							{
								TH = 9000000 - SUM0;
							}
						}
					}
					else
					{
						//
					}
				}

				if (SUM1 > (SUM0 + TH))	//��ʽ�ж�
				{
					if ((light_ad <= LIGHT_TH) || (start_times > 0))	//��һ������֮�²�����
					{
						{
							if (LIGHT == 0)
								SUM01 = SUM0;
							LIGHT = 1;
							//////////////////////////////////
							////////���������������////////////
							radar_number_send_flag2 = 1;
							SUM2 = SUM1;
							//////////////////////////////////
							SUM1_num = 8;
							LIGHT_off = 0;
							light1scount = 0;
							light1sflag = 0;
						}
					}
				}
			}

			SUM2 = SUM1;
			
///////////////////////////////////////////////////
//			send_data(average >> 4);
//			send_data(light_ad);
//			send_data(SUM0 >> 16);
//			send_data(SUM0 >> 8);
//			send_data(SUM2 >> 16);
//			send_data(SUM2 >> 8); //20200927	������
//			send_data(TH >> 16);
//			send_data(TH >> 8);
////////////////////////////////////////////////////
			SUM = 0;
			SUM1 = 0;

			if (LIGHT > 0)
			{
				if (LIGHT > DELAY_NUM)
				{
					LIGHT = 0;
					while_1flag = 1;
				}
			}
		}
	}
	else
	{
		LIGHT_off = 1;
		while_1flag = 0;
		lowlight1mincount = 0;
		lowlight1minflag = 0;
		Delay_ms(250);

		SUM16 = 0;
		calc_average_times = 0;
		SUM1_num = 64;

		stop_times = 2;
		check_light_times = 6;

		SUM1_counter = 0;
		ALL_SUM1 = 0;
	}
}

unsigned char PWM3init(unsigned char ab)
{
	float i11;
	unsigned char j11;

	if (0 == ab)
	{
		//
	}
	else if (100 == ab)
	{
		//
	}
	else
	{
		//
	}
	
	if (1 == ab)
	{
		j11 = 0;
	}
	else
	{
		i11 = ab * 255 / 100;
		j11 = (unsigned char )(i11 + 0.5);
	}
	
#ifdef V11
	/************************************PWM3��ʼ��****************************************/
	//P0M3 = P0M3&0xF0|0x08;		//P06����Ϊ�������
	PWM3_MAP = 0x05; //PWM3ӳ��P05��

#endif

#ifdef V10
	PWM3_MAP = 0x06; //PWM3ӳ��P05��

#endif

#ifdef V12
	PWM3_MAP = 0x10; //PWM3ӳ��P10��

#endif

#ifdef XBR403_03_2
	PWM3_MAP = 0x01;					//PWM3ͨ��ӳ��P01��
#endif

#ifdef XBR403
	PWM3_MAP = 0x10; //PWM3ӳ��P10��
#endif


	//���ڼ��� 	= 0xFF / (Fosc / PWM��Ƶϵ��)		��Fosc��ϵͳʱ�����õĲ��֣�
	//			= 0xFF /(16000000 / 4)
	// 			= 255 /4000000
	//			= 63.75us		��15.69KHZ

	PWM3P = 0xFF; //���ڼĴ���//PWM����Ϊ0xFF
	//��Ч��ƽʱ����㣨��ռ�ձȣ�
	//			= 0x55 / (Fosc / PWM��Ƶϵ��)		��Fosc��ϵͳʱ�����õĲ��֣�
	//			= 0x55 /(16000000 / 4)
	// 			= 85 /4000000
	//			= 21.25us		ռ�ձ�Ϊ 21.25 / 63.75 = 34%

	PWM3D = j11;  //PWMռ�ձ����ã�ռ�ձȼĴ���p90
	PWM3C = 0x94; //PWM���ƼĴ�����ʹ��PWM3���ر��жϣ����������ʱ��16��Ƶ

	return 0;
}

void lv_Init(void)
{
	LVDC = 0xAA; //LVD����2.4V,��ֹ�ж�
	//	����ʱ�� = 	(0xFF + 2) * 1/Fcpu
	//			 =	(0xFF + 2) / 16000000	����ǰ��CPUʱ�ӣ�
	//			 =	16.0625us
	LVDDBC = 0xFF; //��������ʱ��
	LVDC &= ~0x08; //���LVD�жϱ�־λ
}
/***************************************************************************************
  * @˵��  	������
  *	@����	  ��
  * @����ֵ ��
  * @ע		  ��
***************************************************************************************/
void main()
{
	bt_protocol_init(); //mcu_sdk
	InitSYS();			//clock init
	GPIO_Init();		//gpio init
	Timer_Init();		//timer1 init
	UART1_Init();		//uart1 init
	ADC_Init();			//adc init
	lv_Init();			//lv init
	PWM3init(0);		//�����

	EA = 1;
	Delay_ms(500);		//test timer delay
	EA = 0;

	set_var();			//��flash��ȡ�������ĳ�ֵ
	PWM3init(100);		//��ȡ��ɺ�ѵƴ�

	EA = 1;
	Delay_ms(500);	
	PWM3init(0);		//500ms�����

	SUM = 0;
	upload_disable = 0;
	
	if (resetbtcnt > 3)
	{
		reset_bt_module();
		if (resetbtcnt > 12)
		{
			go_bootloader_ota();
		}
		resetbtcnt = 0;
	}
	else
	{
		savevar();
	}	
	while (1)
	{
		if (upload_disable == 0)
		{
//			if (person_in_range_flag != person_in_range_flag_last)
//			{
//				mcu_dp_enum_update(DPID_PERSON_IN_RANGE,person_in_range_flag);
//				person_in_range_flag_last = person_in_range_flag;
//			}
			
			if (1 == radar_number_send_flag)
			{
				radar_number_send_flag = 0;
				if (1 == radar_number_send_flag2)
				{
					radar_number_send_flag2 = 0;
					mcu_dp_value_update(DPID_IF_SUM, SUM2);
					mcu_dp_value_update(DPID_NOISE_SUM, SUM0);
				}
			}	
		}

		WDTC |= 0x10; //�忴�Ź�

		if (while_1flag == 0)//���״̬
		{
			if ((times & 0x1f) == 0)
			{
				bt_uart_service();	//���ڽ���첽����			
			}
		}

		if (SWITCHfXBR == 1) //�״￪, app����
		{
			if (while_2flag == 0)
			{
				while_1flag = 0;

				while_2flag = 1;
				slowchcnt = lightvalue;

				SUM16 = 0;
				calc_average_times = 0;
				SUM1_num = 64;

				stop_times = 2;
				check_light_times = 6;

				SUM1_counter = 0;
				ALL_SUM1 = 0;
			}

			XBRHandle();

			if (LIGHT_off > 0) //�ص���ʱ
			{
				if (lowlight1minflag == 1)
				{
					lowlight1minflag = 0;
					LIGHT_off++;
					if (LIGHT_off >= lowlightDELAY_NUM)
					{
						LIGHT_off = 0;
						PWM3init(0);
					}
				}
			}
			
			if (LIGHT > 0) //������ʱ
			{
				if (light1sflag == 1)
				{
					light1sflag = 0;
					LIGHT++;
				}
			}


		}
		else
		{ //�״��
			while_2flag = 0;
			if (SWITCHflag2 == 0) //�ص�
			{
				PWM3init(0);
			}
			else
			{ //����
				PWM3init(XRBoffbrightvalue);

				while_1flag = 0;

				slowchcnt = lightvalue;

				SUM16 = 0;
				calc_average_times = 0;
				SUM1_num = 64;

				stop_times = 2;

				check_light_times = 6;

				SUM1_counter = 0;
				ALL_SUM1 = 0;
			}
		}
	}
}

/***************************************************************************************
  * @˵��  	T1�жϷ�����
  *	@����	  ��
  * @����ֵ ��
  * @ע		  ��
***************************************************************************************/
void TIMER1_Rpt(void) interrupt TIMER1_VECTOR
{
	Timer_Counter++;

	lowlight1mincount++;
	if (lowlight1mincount >= 60000)
	{
		lowlight1mincount = 0;
		lowlight1minflag = 1;
	}
	light1scount++;
	if (light1scount >= 1000)
	{
		light1scount = 0;
		light1sflag = 1;
	}
	radar_number_count++;
	if (radar_number_count >= 1000)
	{
		radar_number_count = 0;
		radar_number_send_flag = 1;
	}	
}

/***************************************************************************************
  * @˵��  	UART1�жϷ�����
  *	@����	  ��
  * @����ֵ ��
  * @ע		  ��
***************************************************************************************/

void UART1_Rpt(void) interrupt UART1_VECTOR
{
	u8 i;
	//u16 t;

	if (SCON & 0x01) //�жϽ����жϱ�־λ
	{
		i = SBUF;
		uart_receive_input(i); //mcu_sdk
		SCON &= ~0x01;		   //��������жϱ�־λ
		EA = 1;
	}
}

void Flash_EraseBlock(unsigned int fui_Address)
{
	while (1)
	{
		LVDC &= ~0x08; //���LVD�жϱ�־λ
		P0_0 = 0;
		if ((LVDC & 0x08) == 0)
			break;
	}
	P0_0 = 1;
	EA = 0;
	IAP_CMD = 0xF00F;		//Flash����
	IAP_ADDR = fui_Address; //д�������ַ
	IAP_CMD = 0xD22D;		//ѡ�������ʽ�� ��������
	IAP_CMD = 0xE11E;		//������IAP_ADDRL&IAP_ADDRHָ��0xFF��ͬʱ�Զ�����
							//EA=1;
}

/**
  * @˵��  	д��һ���ֽ����ݵ�Flash����
  *         �ú�������Ե�ַ���룬���������IAP����Ӧ���ֲ�
  * @����  	fui_Address ��FLASH��ַ
  *	@����	  fucp_SaveData��д�������
  * @����ֵ ��
  * @ע		  д֮ǰ�����ȶԲ������������в���
  */
void FLASH_WriteData(unsigned char fuc_SaveData, unsigned int fui_Address)
{
	while (1)
	{
		LVDC &= ~0x08; //���LVD�жϱ�־λ
		P0_0 = 0;
		if ((LVDC & 0x08) == 0)
			break;
	}
	P0_0 = 1;
	EA = 0;
	IAP_DATA = fuc_SaveData;
	IAP_CMD = 0xF00F; //Flash����
	IAP_ADDR = fui_Address;
	IAP_CMD = 0xB44B; //�ֽڱ��
	IAP_CMD = 0xE11E; //����һ�β���
}

void savevar(void)
{
	unsigned char i;
	Flash_EraseBlock(USER_PARAMETER_START_SECTOR_ADDRESS0);
	Delay_us(10000);

	i=(TH/1000)>>8;
	FLASH_WriteData(i,USER_PARAMETER_START_SECTOR_ADDRESS0+0);
	Delay_us(100);
	
    i=(TH/1000)&0xff;
	FLASH_WriteData(i,USER_PARAMETER_START_SECTOR_ADDRESS0+1);
	Delay_us(100);
	
    i=LIGHT_TH;
	FLASH_WriteData(i,USER_PARAMETER_START_SECTOR_ADDRESS0+2);
	Delay_us(100);
	
	i=~SWITCHfXBR;
	FLASH_WriteData(i,USER_PARAMETER_START_SECTOR_ADDRESS0+7);
	Delay_us(100);
	
	Flash_EraseBlock(USER_PARAMETER_START_SECTOR_ADDRESS1);
	Delay_us(10000);
	FLASH_WriteData(0, USER_PARAMETER_START_SECTOR_ADDRESS1+0);//clear resetbtcnt
	Delay_us(100);
	FLASH_WriteData(1, USER_PARAMETER_START_SECTOR_ADDRESS1+1);//clear join count
	Delay_us(100);
	
	EA=1;
}
