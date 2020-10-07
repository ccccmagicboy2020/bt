/****************************************Copyright (c)*************************
**                               ��Ȩ���� (C), 2015-2017, Ϳѻ�Ƽ�
**
**                                 http://www.tuya.com
**
**--------------�ļ���Ϣ-------------------------------------------------------
**��   ��   ��: protocol.c
**��        ��: �·�/�ϱ����ݴ�����
**ʹ �� ˵ �� :

                  *******�ǳ���Ҫ��һ��Ҫ��Ŷ������********

** 1���û��ڴ��ļ���ʵ�������·�/�ϱ�����
** 2��DP��ID/TYPE�����ݴ���������Ҫ�û�����ʵ�ʶ���ʵ��
** 3������ʼĳЩ�궨�����Ҫ�û�ʵ�ִ���ĺ����ڲ���#err��ʾ,��ɺ�������ɾ����#err
**
**--------------��ǰ�汾�޶�---------------------------------------------------
** ��  ��: v1.0
** �ա���: 2017��5��3��
** �衡��: 1:����Ϳѻbluetooth�Խ�MCU_SDK
**
**-----------------------------------------------------------------------------
******************************************************************************/
//#include "include.h"

#include "bluetooth.h"
//��ͬ��Ӧ�����Ӧ������ֵ������ʵ��������
#define RADIUS_1M 500000
#define RADIUS_2M 320000
#define RADIUS_3M 200000
#define RADIUS_4M 100000
#define RADIUS_5M 40000


//extern TYPE_BUFFER_S FlashBuffer;

/******************************************************************************
                                ��ֲ��֪:
1:MCU������while��ֱ�ӵ���mcu_api.c�ڵ�bt_uart_service()����
2:����������ʼ����ɺ�,���鲻���йش����ж�,�������ж�,���ж�ʱ������,���жϻ����𴮿����ݰ���ʧ
3:�������ж�/��ʱ���ж��ڵ����ϱ�����
******************************************************************************/

         
/******************************************************************************
                              ��һ��:��ʼ��
1:����Ҫʹ�õ�bt����ļ����ļ���include "bt.h"
2:��MCU��ʼ���е���mcu_api.c�ļ��е�bt_protocol_init()����
3:��MCU���ڵ��ֽڷ��ͺ�������protocol.c�ļ���uart_transmit_output������,��ɾ��#error
4:��MCU���ڽ��պ����е���mcu_api.c�ļ��ڵ�uart_receive_input����,�������յ����ֽ���Ϊ��������
5:��Ƭ������whileѭ�������mcu_api.c�ļ��ڵ�bt_uart_service()����
******************************************************************************/

extern u8 xdata LIGHT_TH,light_ad;
extern u16 xdata DELAY_NUM;
extern u8 xdata lowlightDELAY_NUM;
extern u8 xdata SWITCHflag ;
extern u8 xdata SWITCHflag2 ;
extern u8 xdata SWITCHfXBR ;

extern u8 xdata lightvalue ;
extern u8 xdata XRBoffbrightvalue ;
extern u8 xdata addr ;
extern u8 xdata devgroup;
extern u8 xdata addrend;

extern ulong xdata TH ;
extern	u8 xdata switchcnt ;


   unsigned char DPID_SWITCH_LED2count = 0;
	 unsigned char DPID_SWITCH_XBRcount = 0;
	 unsigned char DPID_BRIGHT_VALUEcount = 0;
	 unsigned char DPID_CDScount = 0;
	 unsigned char DPID_PIR_DELAYcount = 0;
	 unsigned char DPID_SWITCH_PIRcount = 0;
	 unsigned char DPID_STANDBY_TIMEcount = 0;
	 unsigned char DPID_SENSING_RADIUScount = 0;


unsigned char PWM3init(unsigned char ab);

void Flash_EraseBlock(unsigned int fui_Address);//��������
void FLASH_WriteData(unsigned char fuc_SaveData, unsigned int fui_Address);
void Delay_us_1(uint q1);
void send_data(u8 d);
void savevar(void);


/******************************************************************************
                        1:dp���ݵ��������Ͷ��ձ�
          **��Ϊ�Զ����ɴ���,���ڿ���ƽ̨������޸�����������MCU_SDK**         
******************************************************************************/
const DOWNLOAD_CMD_S  xdata download_cmd[] =
{
  {DPID_SWITCH_XBR, DP_TYPE_BOOL},
  {DPID_SWITCH_LED, DP_TYPE_BOOL},
  //{DPID_WORK_MODE, DP_TYPE_ENUM},
  {DPID_BRIGHT_VALUE, DP_TYPE_VALUE},
  //{DPID_DEVICE_MODE, DP_TYPE_ENUM},
  //{DPID_PIR_STATE, DP_TYPE_ENUM},
  {DPID_CDS, DP_TYPE_ENUM},
  //{DPID_PIR_SENSITIVITY, DP_TYPE_ENUM},
  {DPID_PIR_DELAY, DP_TYPE_VALUE},
  //{DPID_SWITCH_PIR, DP_TYPE_BOOL},
  //{DPID_PIR_RESUME_COUNTDOWN, DP_TYPE_VALUE},
  {DPID_STANDBY_TIME, DP_TYPE_VALUE},
  {DPID_SENSING_RADIUS, DP_TYPE_VALUE},
	{DPID_SWITCH_LED2, DP_TYPE_BOOL},
	{DPID_dev_addr, DP_TYPE_VALUE},
  //{DPID_HANG_HIGH, DP_TYPE_VALUE},
//	{DPID_SWITCH_LED, DP_TYPE_BOOL},
//  {DPID_WORK_MODE, DP_TYPE_ENUM},
//  {DPID_BRIGHT_VALUE, DP_TYPE_VALUE},
//  {DPID_DEVICE_MODE, DP_TYPE_ENUM},
//  {DPID_PIR_STATE, DP_TYPE_ENUM},
//  {DPID_CDS, DP_TYPE_ENUM},
//  {DPID_PIR_SENSITIVITY, DP_TYPE_ENUM},
//  {DPID_PIR_DELAY, DP_TYPE_VALUE},
//  {DPID_SWITCH_PIR, DP_TYPE_BOOL},
//  {DPID_PIR_RESUME_COUNTDOWN, DP_TYPE_VALUE},
//  {DPID_STANDBY_TIME, DP_TYPE_VALUE},
//  {DPID_SENSING_RADIUS, DP_TYPE_VALUE},
//  {DPID_HANG_HIGH, DP_TYPE_VALUE},
};




/******************************************************************************
                           2:���ڵ��ֽڷ��ͺ���
�뽫MCU���ڷ��ͺ�������ú�����,�������յ���������Ϊ�������봮�ڷ��ͺ���
******************************************************************************/

/*****************************************************************************
�������� : uart_transmit_output
�������� : �����ݴ���
������� : value:�����յ��ֽ�����
���ز��� : ��
ʹ��˵�� : �뽫MCU���ڷ��ͺ�������ú�����,�������յ���������Ϊ�������봮�ڷ��ͺ���
*****************************************************************************/
void uart_transmit_output(unsigned char value)
{
// #error "�뽫MCU���ڷ��ͺ�������ú���,��ɾ������"
	
	
			SBUF = value;
		while(!(SCON & 0x02));
		SCON &=~ 0x02;
	
/*
  //ʾ��:
  extern void Uart_PutChar(unsigned char value);
  Uart_PutChar(value);	                                //���ڷ��ͺ���
*/  
}
/******************************************************************************
                           �ڶ���:ʵ�־����û�����
1:APP�·����ݴ���
2:�����ϱ�����
******************************************************************************/

/******************************************************************************
                            1:���������ϱ�����
��ǰ��������ȫ�������ϱ�(�������·�/���ϱ���ֻ�ϱ�)
  ��Ҫ�û�����ʵ�����ʵ��:
  1:��Ҫʵ�ֿ��·�/���ϱ����ݵ��ϱ�
  2:��Ҫʵ��ֻ�ϱ����ݵ��ϱ�
�˺���ΪMCU�ڲ��������
�û�Ҳ�ɵ��ô˺���ʵ��ȫ�������ϱ�
******************************************************************************/

//�Զ������������ϱ�����

/*****************************************************************************
�������� : all_data_update
�������� : ϵͳ����dp����Ϣ�ϴ�,ʵ��APP��muc����ͬ��
������� : ��
���ز��� : ��
ʹ��˵�� : �˺���SDK�ڲ������;
           MCU����ʵ�ָú����������ϱ�����;����ֻ�ϱ��Ϳ��ϱ����·�������
*****************************************************************************/
void all_data_update(void)
{
	u8 radius,light;
  //#error "���ڴ˴�����·����ϱ����ݼ�ֻ�ϱ�����ʾ��,������ɺ�ɾ������"
  //�˴���Ϊƽ̨�Զ����ɣ��밴��ʵ�������޸�ÿ�����·����ϱ�������ֻ�ϱ�����
  	mcu_dp_bool_update(DPID_SWITCH_XBR,SWITCHfXBR); //BOOL�������ϱ�;
  	mcu_dp_bool_update(DPID_SWITCH_LED2,SWITCHflag2); //BOOL�������ϱ�;
    mcu_dp_bool_update(DPID_SWITCH_LED,SWITCHflag); //BOOL�������ϱ�;
    //mcu_dp_enum_update(DPID_WORK_MODE,SWITCHflag); //ö���������ϱ�;
    mcu_dp_value_update(DPID_BRIGHT_VALUE,lightvalue); //VALUE�������ϱ�;
    //mcu_dp_enum_update(DPID_DEVICE_MODE,SWITCHflag); //ö���������ϱ�;
    //mcu_dp_enum_update(DPID_PIR_STATE,SWITCHflag); //ö���������ϱ�;
	
   // mcu_dp_enum_update(DPID_CDS,LIGHT_TH/4); //ö���������ϱ�;
	
		if(LIGHT_TH==255)
			light=0;
		else if(LIGHT_TH==200)
			light=2;
		else if(LIGHT_TH==40)
			light=3;		
		else if(LIGHT_TH==20)
			light=4;
		else //if(LIGHT_TH==200)
			light=5;
		mcu_dp_enum_update(DPID_CDS,light); //ö���������ϱ�;
	
    //mcu_dp_enum_update(DPID_PIR_SENSITIVITY,SWITCHflag); //ö���������ϱ�;
    mcu_dp_value_update(DPID_PIR_DELAY,DELAY_NUM); //VALUE�������ϱ�;
//    mcu_dp_bool_update(DPID_SWITCH_PIR,SWITCHflag); //BOOL�������ϱ�;
//    mcu_dp_value_update(DPID_PIR_RESUME_COUNTDOWN,DELAY_NUM); //VALUE�������ϱ�;
//    mcu_dp_value_update(DPID_STANDBY_TIME,DELAY_NUM); //VALUE�������ϱ�;
	
	
//		if(TH==RADIUS_1M)
//			radius=1;
//		else if(TH==RADIUS_2M)
//			radius=2;
//		else if(TH==RADIUS_3M)
//			radius=3;	
//		else if(TH==RADIUS_4M)
//			radius=4;
//		else
//			radius=5;
		
		
		radius=TH/10000;
		radius=50-radius;
		
		
		
		//savevar();
	
    mcu_dp_value_update(DPID_SENSING_RADIUS,radius);


	mcu_dp_value_update(DPID_dev_addr,addr);	

	mcu_dp_value_update(DPID_dev_addrend,addrend);		
 //   mcu_dp_value_update(DPID_SENSING_RADIUS,TH/1000); //VALUE�������ϱ�;
//    mcu_dp_value_update(DPID_HANG_HIGH,SWITCHflag); //VALUE�������ϱ�;



}


/******************************************************************************
                                WARNING!!!    
                            2:���������ϱ�����
�Զ�������ģ�庯��,�������û�����ʵ�����ݴ���
******************************************************************************/

/*****************************************************************************
�������� : dp_download_switch_led_handle
�������� : ���DPID_SWITCH_LED�Ĵ�����
������� : value:����Դ����
        : length:���ݳ���
���ز��� : �ɹ�����:SUCCESS/ʧ�ܷ���:ERROR
ʹ��˵�� : ���·����ϱ�����,��Ҫ�ڴ��������ݺ��ϱ���������app
*****************************************************************************/
static unsigned char dp_download_switch_led_handle(const unsigned char value[], unsigned short length)
{
    //ʾ��:��ǰDP����ΪBOOL
    unsigned char ret;
    //0:��/1:��
    unsigned char switch_led;
    
    switch_led = mcu_get_dp_download_bool(value,length);
    if(switch_led == 0) {
        //���ع�
        //LIGHT_OFF;
    }else {
        //���ؿ�
        //LIGHT_ON;
    }
  
    //������DP���ݺ�Ӧ�з���
    ret = mcu_dp_bool_update(DPID_SWITCH_LED,switch_led);
    if(ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}
static unsigned char dp_download_switch_led2_handle(const unsigned char value[], unsigned short length)
{
    //ʾ��:��ǰDP����ΪBOOL
    unsigned char ret;
    //0:��/1:��
    unsigned char switch_led;
    
    switch_led = mcu_get_dp_download_bool(value,length);
		DPID_SWITCH_LED2count++;
		if(switch_led==SWITCHflag2)
		{
			if(DPID_SWITCH_LED2count<2)
			{
				//DPID_SWITCH_LED2count = 0;
				mcu_dp_bool_mesh_update(DPID_SWITCH_LED2,switch_led);
			}
		
		}
		else
		{
			DPID_SWITCH_LED2count=0;
			mcu_dp_bool_mesh_update(DPID_SWITCH_LED2,switch_led);
		
		}
		
    if(switch_led == 0) {
        //�ƿ��ع�
        //LIGHT_OFF;
				if(SWITCHfXBR==1)
				{
					PWM3init(0);
				}
        
        SWITCHflag2=0;
    }else {
        //�ƿ��ؿ�
        //LIGHT_ON;
        if(SWITCHfXBR==1)
				{
					PWM3init(100);
				}
        SWITCHflag2=1;
    }
  	
		
		
		
    //������DP���ݺ�Ӧ�з���
    ret = mcu_dp_bool_update(DPID_SWITCH_LED2,switch_led);
    if(ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}

static unsigned char dp_download_switch_XBR_handle(const unsigned char value[], unsigned short length)
{
    //ʾ��:��ǰDP����ΪBOOL
    unsigned char ret;
    //0:��/1:��
    unsigned char switch_led;
    
    switch_led = mcu_get_dp_download_bool(value,length);
		DPID_SENSING_RADIUScount++;
		if(switch_led==SWITCHfXBR)
		{
			if(DPID_SENSING_RADIUScount<2)
			{
				//DPID_SWITCH_LED2count = 0;
				mcu_dp_bool_mesh_update(DPID_SWITCH_XBR,switch_led);
			}
		
		}
		else
		{
			DPID_SENSING_RADIUScount=0;
			mcu_dp_bool_mesh_update(DPID_SWITCH_XBR,switch_led);
		
		}
    if(switch_led == 0) {
        //�״￪�ع�
        //LIGHT_OFF;
        //PWM3init(0);
        SWITCHfXBR=0;
    }else {
        //�״￪�ؿ�
        //LIGHT_ON;
        //PWM3init(100);
        SWITCHfXBR=1;
    }
  	
    //������DP���ݺ�Ӧ�з���
    ret = mcu_dp_bool_update(DPID_SWITCH_XBR,switch_led);
    if(ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}

/*****************************************************************************
�������� : dp_download_work_mode_handle
�������� : ���DPID_WORK_MODE�Ĵ�����
������� : value:����Դ����
        : length:���ݳ���
���ز��� : �ɹ�����:SUCCESS/ʧ�ܷ���:ERROR
ʹ��˵�� : ���·����ϱ�����,��Ҫ�ڴ��������ݺ��ϱ���������app
*****************************************************************************/
//static unsigned char dp_download_work_mode_handle(const unsigned char value[], unsigned short length)
//{
//    //ʾ��:��ǰDP����ΪENUM
//    unsigned char ret;
//    unsigned char work_mode;
//    
//    work_mode = mcu_get_dp_download_enum(value,length);
//    switch(work_mode) {
//        case 0:
//        break;
//        
//        case 1:
//        break;
//        
//        default:
//    
//        break;
//    }
//    
//    //������DP���ݺ�Ӧ�з���
//    ret = mcu_dp_enum_update(DPID_WORK_MODE, work_mode);
//    if(ret == SUCCESS)
//        return SUCCESS;
//    else
//        return ERROR;
//}
/*****************************************************************************
�������� : dp_download_bright_value_handle
�������� : ���DPID_BRIGHT_VALUE�Ĵ�����
������� : value:����Դ����
        : length:���ݳ���
���ز��� : �ɹ�����:SUCCESS/ʧ�ܷ���:ERROR
ʹ��˵�� : ���·����ϱ�����,��Ҫ�ڴ��������ݺ��ϱ���������app
*****************************************************************************/
static unsigned char dp_download_bright_value_handle(const unsigned char value[], unsigned short length)
{
    //ʾ��:��ǰDP����ΪVALUE
    //55 AA 00 07 00 08 03 02 00 04 00 00 00 0A 21
    unsigned char ret;
    unsigned long bright_value;
    
    bright_value = mcu_get_dp_download_value(value,length);
    /*
    //VALUE�������ݴ���
    
    
    */
		DPID_BRIGHT_VALUEcount++;
		if(bright_value==lightvalue)
		{
			if(DPID_BRIGHT_VALUEcount<2)
			{
				//DPID_BRIGHT_VALUEcount = 0;
				mcu_dp_value_mesh_update(DPID_BRIGHT_VALUE,bright_value);
			}
		
		}
		else
		{
			DPID_BRIGHT_VALUEcount=0;
			mcu_dp_value_mesh_update(DPID_BRIGHT_VALUE,bright_value);
		
		}
    lightvalue = bright_value;
		if(SWITCHfXBR==0)
		{
			XRBoffbrightvalue = bright_value;
		
		}
		
		savevar();
		
    //������DP���ݺ�Ӧ�з���
    ret = mcu_dp_value_update(DPID_BRIGHT_VALUE,bright_value);
    if(ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}
/*****************************************************************************
�������� : dp_download_device_mode_handle
�������� : ���DPID_DEVICE_MODE�Ĵ�����
������� : value:����Դ����
        : length:���ݳ���
���ز��� : �ɹ�����:SUCCESS/ʧ�ܷ���:ERROR
ʹ��˵�� : ���·����ϱ�����,��Ҫ�ڴ��������ݺ��ϱ���������app
*****************************************************************************/
//static unsigned char dp_download_device_mode_handle(const unsigned char value[], unsigned short length)
//{
//    //ʾ��:��ǰDP����ΪENUM
//    unsigned char ret;
//    unsigned char device_mode;
//    
//    device_mode = mcu_get_dp_download_enum(value,length);
//    switch(device_mode) {
//        case 0:
//        break;
//        
//        case 1:
//        break;
//        
//        default:
//    
//        break;
//    }
//    
//    //������DP���ݺ�Ӧ�з���
//    ret = mcu_dp_enum_update(DPID_DEVICE_MODE, device_mode);
//    if(ret == SUCCESS)
//        return SUCCESS;
//    else
//        return ERROR;
//}
/*****************************************************************************
�������� : dp_download_cds_handle
�������� : ���DPID_CDS�Ĵ�����
������� : value:����Դ����
        : length:���ݳ���
���ز��� : �ɹ�����:SUCCESS/ʧ�ܷ���:ERROR
ʹ��˵�� : ���·����ϱ�����,��Ҫ�ڴ��������ݺ��ϱ���������app
*****************************************************************************/
static unsigned char dp_download_cds_handle(const unsigned char value[], unsigned short length)
{
    //ʾ��:��ǰDP����ΪENUM
    unsigned char ret;
    unsigned char cds;
    static unsigned char cdsvalue;
    cds = mcu_get_dp_download_enum(value,length);
	
	
		DPID_CDScount++;
		if(cds==cdsvalue)
		{
			if(DPID_CDScount<2)
			{
				//DPID_BRIGHT_VALUEcount = 0;
				mcu_dp_enum_mesh_update(DPID_CDS,cds);
			}
			if((cds==5)&&(light_ad!=LIGHT_TH))
			{
				DPID_CDScount=0;
			
			}
		}
		else
		{
			DPID_CDScount=0;
			mcu_dp_enum_mesh_update(DPID_CDS,cds);
		
		}
	
	
	
    switch(cds) {
//         case 5:
// 			LIGHT_TH=cds*4;
//         break;
//         
//         case 10:
// 			LIGHT_TH=cds*4;
//         break;
//         
//         case 50:
// 			LIGHT_TH=cds*4;
//         break;
//         
//         case 300:
//         break;
//         
//         case 2000:
//         break;
        
        case 0:		//2000LUS
					cdsvalue = 0;
			LIGHT_TH=255;//cds*4;
        break;
        
        case 1:		//300LUX
					cdsvalue = 1;
			LIGHT_TH=255;//cds*4;
        break;
        
        case 2:		//50LUX
					cdsvalue = 2;
			LIGHT_TH=200;
        break;
        
        case 3:	//10LUX
					cdsvalue = 3;
					LIGHT_TH=40;
			
        break;
        
        case 4:	//5LUX
					cdsvalue = 4;
					LIGHT_TH=20;
        break;
        
				case 5:
					cdsvalue = 5;
					LIGHT_TH=light_ad;
					break;
				
        default:
    
        break;
    }
	
	 
		savevar();
	
    //������DP���ݺ�Ӧ�з���
    ret = mcu_dp_enum_update(DPID_CDS, cds);
    if(ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}
/*****************************************************************************
�������� : dp_download_pir_sensitivity_handle
�������� : ���DPID_PIR_SENSITIVITY�Ĵ�����
������� : value:����Դ����
        : length:���ݳ���
���ز��� : �ɹ�����:SUCCESS/ʧ�ܷ���:ERROR
ʹ��˵�� : ���·����ϱ�����,��Ҫ�ڴ��������ݺ��ϱ���������app
*****************************************************************************/
//static unsigned char dp_download_pir_sensitivity_handle(const unsigned char value[], unsigned short length)
//{
//    //ʾ��:��ǰDP����ΪENUM
//    unsigned char ret;
//    unsigned char pir_sensitivity;
//    
//    pir_sensitivity = mcu_get_dp_download_enum(value,length);
//    switch(pir_sensitivity) {
//        case 0:
//        break;
//        
//        case 1:
//        break;
//        
//        case 2:
//        break;
//        
//        default:
//    
//        break;
//    }
//    
//    //������DP���ݺ�Ӧ�з���
//    ret = mcu_dp_enum_update(DPID_PIR_SENSITIVITY, pir_sensitivity);
//    if(ret == SUCCESS)
//        return SUCCESS;
//    else
//        return ERROR;
//}
/*****************************************************************************
�������� : dp_download_pir_delay_handle
�������� : ���DPID_PIR_DELAY�Ĵ�����
������� : value:����Դ����
        : length:���ݳ���
���ز��� : �ɹ�����:SUCCESS/ʧ�ܷ���:ERROR
ʹ��˵�� : ���·����ϱ�����,��Ҫ�ڴ��������ݺ��ϱ���������app
*****************************************************************************/
static unsigned char dp_download_pir_delay_handle(const unsigned char value[], unsigned short length)
{
    //ʾ��:��ǰDP����ΪVALUE
    unsigned char ret;
    unsigned long pir_delay;
    
    pir_delay = mcu_get_dp_download_value(value,length);
    /*
    //VALUE�������ݴ���
    
    */
		DPID_PIR_DELAYcount++;
		if(pir_delay==DELAY_NUM)
		{
			if(DPID_PIR_DELAYcount<2)
			{
				//DPID_BRIGHT_VALUEcount = 0;
				mcu_dp_value_mesh_update(DPID_PIR_DELAY,pir_delay);
			}
		
		}
		else
		{
			DPID_PIR_DELAYcount=0;
			mcu_dp_value_mesh_update(DPID_PIR_DELAY,pir_delay);
		
		}
    DELAY_NUM = pir_delay;
		savevar();
	
    //������DP���ݺ�Ӧ�з���
    ret = mcu_dp_value_update(DPID_PIR_DELAY,pir_delay);
    if(ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}
/*****************************************************************************
�������� : dp_download_switch_pir_handle
�������� : ���DPID_SWITCH_PIR�Ĵ�����
������� : value:����Դ����
        : length:���ݳ���
���ز��� : �ɹ�����:SUCCESS/ʧ�ܷ���:ERROR
ʹ��˵�� : ���·����ϱ�����,��Ҫ�ڴ��������ݺ��ϱ���������app
*****************************************************************************/
static unsigned char dp_download_switch_pir_handle(const unsigned char value[], unsigned short length)
{
    //ʾ��:��ǰDP����ΪBOOL
    unsigned char ret;
    //0:��/1:��
    unsigned char switch_pir;
    
    switch_pir = mcu_get_dp_download_bool(value,length);
    if(switch_pir == 0) {
        //���ع�
    }else {
        //���ؿ�
    }
  
    //������DP���ݺ�Ӧ�з���
    ret = mcu_dp_bool_update(DPID_SWITCH_PIR,switch_pir);
    if(ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}



/*****************************************************************************
�������� : dp_download_standby_time_handle
�������� : ���DPID_STANDBY_TIME�Ĵ�����
������� : value:����Դ����
        : length:���ݳ���
���ز��� : �ɹ�����:SUCCESS/ʧ�ܷ���:ERROR
ʹ��˵�� : ���·����ϱ�����,��Ҫ�ڴ��������ݺ��ϱ���������app
*****************************************************************************/
static unsigned char dp_download_standby_time_handle(const unsigned char value[], unsigned short length)
{
    //ʾ��:��ǰDP����ΪVALUE
    unsigned char ret;
    unsigned long standby_time;
    
    standby_time = mcu_get_dp_download_value(value,length);
    /*
    //VALUE�������ݴ���
    
    */
    
    //������DP���ݺ�Ӧ�з���
	
		DPID_STANDBY_TIMEcount++;
		if(standby_time==lowlightDELAY_NUM)
		{
			if(DPID_STANDBY_TIMEcount<2)
			{
				//DPID_BRIGHT_VALUEcount = 0;
				mcu_dp_value_mesh_update(DPID_STANDBY_TIME,standby_time);
			}
		
		}
		else
		{
			DPID_STANDBY_TIMEcount=0;
			mcu_dp_value_mesh_update(DPID_STANDBY_TIME,standby_time);
		
		}
	
	
	
    lowlightDELAY_NUM=standby_time;
    ret = mcu_dp_value_update(DPID_STANDBY_TIME,standby_time);
    if(ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}


static unsigned char dp_download_addr_handle(const unsigned char value[], unsigned short length)
{
    //ʾ��:��ǰDP����ΪVALUE
    unsigned char ret;
    unsigned long standby_time;
    
    standby_time = mcu_get_dp_download_value(value,length);
    /*
    //VALUE�������ݴ���
    
    */
    
    //������DP���ݺ�Ӧ�з���
    addr=standby_time;
    ret = mcu_dp_value_update(DPID_dev_addr,standby_time);
    if(ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}

static unsigned char dp_download_addrend_handle(const unsigned char value[], unsigned short length)
{
    //ʾ��:��ǰDP����ΪVALUE
    unsigned char ret;
    unsigned long standby_time;
    
    standby_time = mcu_get_dp_download_value(value,length);
    /*
    //VALUE�������ݴ���
    
    */
    
    //������DP���ݺ�Ӧ�з���
    addrend=standby_time;
    ret = mcu_dp_value_update(DPID_dev_addrend,standby_time);
    if(ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}


/*****************************************************************************
�������� : dp_download_sensing_radius_handle
�������� : ���DPID_SENSING_RADIUS�Ĵ�����
������� : value:����Դ����
        : length:���ݳ���
���ز��� : �ɹ�����:SUCCESS/ʧ�ܷ���:ERROR
ʹ��˵�� : ���·����ϱ�����,��Ҫ�ڴ��������ݺ��ϱ���������app
*****************************************************************************/
static unsigned char dp_download_sensing_radius_handle(const unsigned char value[], unsigned short length)
{
    //ʾ��:��ǰDP����ΪVALUE
    unsigned char ret,radius;
    unsigned long sensing_radius;
		static unsigned long sensing_radiustem = 0;
    
    sensing_radius = mcu_get_dp_download_value(value,length);
	
	

    /*
    //VALUE�������ݴ���
    
    */
   // TH=sensing_radius*1000;
	
		if(sensing_radius>=50)sensing_radius=49;
		else if(sensing_radius==0)sensing_radius=1;
		radius=sensing_radius;
	
	
		DPID_SENSING_RADIUScount++;
		if(radius==sensing_radiustem)
		{
			if(DPID_SENSING_RADIUScount<2)
			{
				//DPID_BRIGHT_VALUEcount = 0;
				mcu_dp_value_mesh_update(DPID_SENSING_RADIUS,radius);
			}
		
		}
		else
		{
			DPID_SENSING_RADIUScount=0;
			mcu_dp_value_mesh_update(DPID_SENSING_RADIUS,radius);
		
		}
		sensing_radiustem = radius;
	
//		if(sensing_radius==1)
//			TH=RADIUS_1M;
//		else if(sensing_radius==2)
//			TH=RADIUS_2M;
//		else if(sensing_radius==3)
//			TH=RADIUS_3M;
//		else if(sensing_radius==4)
//			TH=RADIUS_4M;
//		else 
//			TH=RADIUS_5M;

		sensing_radius=50-sensing_radius;
		TH=sensing_radius*10000;
		
	//	sensing_radius*=4;
		
	//	sensing_radius=24-sensing_radius;
		
		
		//TH=sensing_radius*10000;	//5m-60000,4m-110000,3m-160000 ....1m-260000---��������ֵ	-20200927
	
		savevar();
		
    //������DP���ݺ�Ӧ�з���
    //ret = mcu_dp_value_update(DPID_SENSING_RADIUS,sensing_radius);
		ret = mcu_dp_value_update(DPID_SENSING_RADIUS,radius);	//-20200927
		
    if(ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}
/*****************************************************************************
�������� : dp_download_hang_high_handle
�������� : ���DPID_HANG_HIGH�Ĵ�����
������� : value:����Դ����
        : length:���ݳ���
���ز��� : �ɹ�����:SUCCESS/ʧ�ܷ���:ERROR
ʹ��˵�� : ���·����ϱ�����,��Ҫ�ڴ��������ݺ��ϱ���������app
*****************************************************************************/
//static unsigned char dp_download_hang_high_handle(const unsigned char value[], unsigned short length)
//{
//    //ʾ��:��ǰDP����ΪVALUE
//    unsigned char ret;
//    unsigned long hang_high;
//    
//    hang_high = mcu_get_dp_download_value(value,length);
//    /*
//    //VALUE�������ݴ���
//    
//    */
//    
//    //������DP���ݺ�Ӧ�з���
//    ret = mcu_dp_value_update(DPID_HANG_HIGH,hang_high);
//    if(ret == SUCCESS)
//        return SUCCESS;
//    else
//        return ERROR;
//}


///******************************************************************************
//                                WARNING!!!                     
//�˴���ΪSDK�ڲ�����,�밴��ʵ��dp����ʵ�ֺ����ڲ�����
//******************************************************************************/
//#ifdef SUPPORT_MCU_FIRM_UPDATE
///*****************************************************************************
//�������� : mcu_firm_update_handle
//�������� : MCU����̼�����ģʽ
//������� : value:�̼�������
//           position:��ǰ���ݰ����ڹ̼�λ��
//           length:��ǰ�̼�������(�̼�������Ϊ0ʱ,��ʾ�̼����������)
//���ز��� : ��
//ʹ��˵�� : MCU��Ҫ����ʵ�ָù���
//*****************************************************************************/
//unsigned char mcu_firm_update_handle(const unsigned char value[],unsigned long position,unsigned short length)
//{
//  #error "���������MCU�̼���������,��ɺ���ɾ������"
//  unsigned long addr;
// 
//  if(length == 0)
//  {
//#ifdef ENABLE_BOOT
//    //�̼����ݷ������
//    FlashBuffer.magic_code = FIREWARE_UPDATE_FLAG;
//    
//    if(Earse_Flash(PARA_ADDR) == ERROR)
//      return ERROR;
//    
//    //д��������־
//    if(Write_Flash(PARA_ADDR,(unsigned char *)&FlashBuffer,sizeof(FlashBuffer)) == ERROR)
//      return ERROR;
//    
//    Reset();
//#endif
//  }
//  else
//  {
//    //�̼����ݴ���
//    addr = FIREWARE_ADDR_H;
//     
//    if(position % 1024 == 0)
//    {
//      if(Earse_Flash(addr + position) == ERROR)
//        return ERROR;
//    }
//    
//    if(Write_Flash(addr + position,(unsigned char *)value,length) == ERROR)
//      return ERROR;
//  }

//  return SUCCESS;
//}
//#endif
/******************************************************************************
                                WARNING!!!                     
���º����û������޸�!!
******************************************************************************/

void savevar(void)
{
	unsigned char i;
	Flash_EraseBlock(0X2F00);
	Delay_us_1(10000);
	i=(TH/1000)>>8;
	FLASH_WriteData(i,0X2F00+0);
	Delay_us_1(100);
	i=(TH/1000)&0xff;
	FLASH_WriteData(i,0X2F00+1);
	Delay_us_1(100);
	i=LIGHT_TH;
	FLASH_WriteData(i,0X2F00+2);
	Delay_us_1(100);
	
	i=DELAY_NUM>>8;
	FLASH_WriteData(i,0X2F00+3);
	Delay_us_1(100);
	i=DELAY_NUM&0xff;//&0xff;
	FLASH_WriteData(i,0X2F00+4);
	Delay_us_1(100);
	
	i=lightvalue;
	FLASH_WriteData(i,0X2F00+5);
	Delay_us_1(100);
	
	i=lowlightDELAY_NUM;
	FLASH_WriteData(i,0X2F00+6);
	Delay_us_1(100);
	
	i=SWITCHfXBR;//&0xff;
	FLASH_WriteData(i,0X2F00+7);
	Delay_us_1(100);
	
//	i=addr;//&0xff;
//	FLASH_WriteData(i,0X2F00+7);
//	Delay_us_1(100);
//	
//	i=devgroup;//&0xff;
//	FLASH_WriteData(i,0X2F00+8);
//	Delay_us_1(100);

//	i=addrend;
//	FLASH_WriteData(i,0X2F00+9);
//	Delay_us_1(100);
	
	Flash_EraseBlock(0X2F80);
	Delay_us_1(10000);
	FLASH_WriteData(0,0X2F80+0);
	
	EA=1;				//-20200927

}




/*****************************************************************************
�������� : dp_download_handle
�������� : dp�·�������
������� : dpid:DP���
           value:dp���ݻ�������ַ
           length:dp���ݳ���
���ز��� : �ɹ�����:SUCCESS/ʧ�ܷ���:ERRO
ʹ��˵�� : �ú����û������޸�
*****************************************************************************/
unsigned char dp_download_handle(unsigned char dpid,const unsigned char value[], unsigned short length)
{
  /*********************************
  ��ǰ����������·�/���ϱ����ݵ���                    
  ���庯������Ҫʵ���·����ݴ���
  �������Ҫ��������������APP��,����APP����Ϊ�·�ʧ��
  ***********************************/
  unsigned char ret;

	//static unsigned char DPID_dev_addrcount = 0;
	
  switch(dpid)
  {
        case DPID_SWITCH_LED:
            //�Ƴ��豸���ش����� 
            ret = dp_download_switch_led_handle(value,length);
						if(ret==1)
						{
								switchcnt ++;
								if(switchcnt>=5)
								{
										switchcnt = 0;
										send_data(0x55);
										send_data(0xAA);
										send_data(0X00);
										send_data(0X04);
										send_data(0X00);
										send_data(0X00);
										send_data(0X03);
								}
						
						}
						
        break;
				case DPID_SWITCH_LED2:
            //�ƿ��ش�����
            ret = dp_download_switch_led2_handle(value,length);
					switchcnt=0;
						
        break;		
				case DPID_SWITCH_XBR:
            //�״￪�ش�����
            ret = dp_download_switch_XBR_handle(value,length);
					switchcnt=0;
						
        break;		
//        case DPID_WORK_MODE:
//            //ģʽ������
//            ret = dp_download_work_mode_handle(value,length);
//        break;
        case DPID_BRIGHT_VALUE:
            //����ֵ������
            ret = dp_download_bright_value_handle(value,length);
						switchcnt=0;
        break;
//        case DPID_DEVICE_MODE:
//            //�豸ģʽ������
//            ret = dp_download_device_mode_handle(value,length);
//        break;
        case DPID_CDS:
            //��������������
            ret = dp_download_cds_handle(value,length);
						switchcnt=0;
        break;
//        case DPID_PIR_SENSITIVITY:
//            //�����ȴ�����
//            ret = dp_download_pir_sensitivity_handle(value,length);
//        break;
        case DPID_PIR_DELAY:
            //��Ӧ��ʱ������
            ret = dp_download_pir_delay_handle(value,length);
						switchcnt=0;
        break;
        case DPID_SWITCH_PIR:
            //��Ӧ���ش�����
            ret = dp_download_switch_pir_handle(value,length);
			switchcnt=0;
        break;
        case DPID_STANDBY_TIME:
            //������ʱ������
            ret = dp_download_standby_time_handle(value,length);
			switchcnt=0;
        break;
        case DPID_SENSING_RADIUS:
            //��Ӧ�뾶������
            ret = dp_download_sensing_radius_handle(value,length);
						switchcnt=0;
        break;
//        case DPID_HANG_HIGH:
//            //�Ҹߴ�����
//            ret = dp_download_hang_high_handle(value,length);
//        break;

//				case DPID_dev_addr:
//            //��Ӧ�뾶������
//            ret = dp_download_addr_handle(value,length);
//						switchcnt=0;
//        break;				
  default:
    break;
  }

	


  
  return ret;
}
/*****************************************************************************
�������� : get_download_cmd_total
�������� : ��ȡ����dp�����ܺ�
������� : ��
���ز��� : �·������ܺ�
ʹ��˵�� : �ú����û������޸�
*****************************************************************************/
unsigned char get_download_cmd_total(void)
{
  return(sizeof(download_cmd) / sizeof(download_cmd[0]));
}
