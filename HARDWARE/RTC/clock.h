#ifndef __CLOCK_H
#define __CLOCK_H	 
/* 头文件*/
#define RTC_IT_SEC           ((uint16_t)0x0001)
#define RTC_IT_ALR           ((uint16_t)0x0002)  /*!< Alarm interrupt */
#define RTC_IT_OW            ((uint16_t)0x0004)  /*!< Overflow interrupt */
#define RTC_LSB_MASK     ((uint32_t)0x0000FFFF)
#include "sys.h"
#include "stm32f10x_bkp.h"
#include "stm32f10x_rcc.h"
#include "delay.h"
#include "includes.h"	
extern OS_SEM Clock_SEM;
extern u32 Clock_count;
/* ----------------------------------------------*/
// 时间结构体
typedef struct 
{
	vu8 hour;
	vu8 min;
	vu8 sec;			
	//公历日月年周
	vu16 year;
	vu8  month;
	vu8  date;
	vu8  week;		 
}_calendar_obj;				

//函数声明
u8 RTC_Init(void);
u8 RTC_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);
u8 Is_Leap_Year(u16 year);
//u8 RTC_Alarm_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);
u8 RTC_Alarm_Seta(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);
#endif

/********************@ SegmentFault  *************************/
