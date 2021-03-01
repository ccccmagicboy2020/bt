#ifndef __PARAMETER_H_
#define __PARAMETER_H_

#define V12 //硬件板卡的版本

#define		APP_FLASH_BEGIN							0x0000
#define		MAGIC_SECTOR_ADDRESS0					0x2E80
#define		USER_PARAMETER_START_SECTOR_ADDRESS0	0x2F00
#define		USER_PARAMETER_START_SECTOR_ADDRESS1	0x2F80
#define		BOOTLOADER_FLASH_BEGIN					0x3000

//#define RELAY

#define TH_LOW 30000
#define TH_HIGH 4000000

#define TH_DEF 40000

//允许噪声值偏差范围
#define MAX_DELTA0 20000 //最大偏差低值
#define MAX_DELTA1 60000 //最大偏差高值

#define MAX_DELAY 1800
//最大延时秒数

//感光门限-30对应8LUX左右的AD值,设置为255表示不检测感光
#define LIGHT_TH0 255
//30


#endif


