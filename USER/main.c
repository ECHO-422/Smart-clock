#include "led.h"
#include "beep.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "ILI93xx.h"
#include "usart.h"	 
#include "24cxx.h"
#include "flash.h"
#include "touch.h"
#include "sram.h"
#include "timer.h"
#include "sdio_sdcard.h"
#include "malloc.h"
#include "GUI.h"
#include "ff.h"
#include "exfuns.h"
#include "w25qxx.h"
#include "includes.h"
#include "Clockdisplay.h"
#include "clock.h"
#include "ble_app.h"
u32 Clock_count;
_calendar_obj Calendar;  //定义一个日历变量 在clock.h 声明
unsigned char RTC_Week(unsigned int year,unsigned char month,unsigned char day);
void clock_Week_dis(unsigned int a, unsigned int b);
volatile unsigned int curs,page,key,num,flash;
//任务优先级
#define START_TASK_PRIO				3
//任务堆栈大小	
#define START_STK_SIZE 				1024
//任务控制块
OS_TCB StartTaskTCB;
//任务堆栈	
CPU_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *p_arg);

//TOUCH任务
//设置任务优先级
#define TOUCH_TASK_PRIO				6
//任务堆栈大小
#define TOUCH_STK_SIZE				128
//任务控制块
OS_TCB TouchTaskTCB;
//任务堆栈
CPU_STK TOUCH_TASK_STK[TOUCH_STK_SIZE];
//touch任务
void touch_task(void *p_arg);

//LED0任务
//设置任务优先级
#define LED0_TASK_PRIO 				7
//任务堆栈大小
#define LED0_STK_SIZE				128
//任务控制块
OS_TCB Led0TaskTCB;
//任务堆栈
CPU_STK LED0_TASK_STK[LED0_STK_SIZE];
//led0任务
void led0_task(void *p_arg);

//EMWINDEMO任务
//设置任务优先级
#define EMWINDEMO_TASK_PRIO			11
//任务堆栈大小
#define EMWINDEMO_STK_SIZE			2048
//任务控制块
OS_TCB EmwindemoTaskTCB;
//任务堆栈
CPU_STK EMWINDEMO_TASK_STK[EMWINDEMO_STK_SIZE];
//emwindemo_task任务
void emwindemo_task(void *p_arg);

//任务优先级
#define TASK1_TASK_PRIO		8
//任务堆栈大小	
#define TASK1_STK_SIZE 		128
//任务控制块
OS_TCB Task1_TaskTCB;
//任务堆栈	
CPU_STK TASK1_TASK_STK[TASK1_STK_SIZE];
void task1_task(void *p_arg);

#define Key_TASK_PRIO 5
#define KEY_STK_SIZE 128
OS_TCB	KEY_TASKTCB;
CPU_STK	KEY_TASK_STK[KEY_STK_SIZE];
void key_task(void *p_arg);
//蓝牙通信
#define BLE_TASK_PRIO 9
#define BLE_STK_SIZE 128
OS_TCB  BLE_TASKTCB;
CPU_STK BLE_TASK_STK[BLE_STK_SIZE]; 
void BLE_task(void *p_arg);

//改变时间
void changtime(const int *pSel)
{
		u8 temp=0;
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//使能PWR和BKP外设时钟   
		PWR_BackupAccessCmd(ENABLE);	//使能后备寄存器访问  			
		BKP_DeInit();	//复位备份区域 	
		RCC_LSEConfig(RCC_LSE_ON);	//设置外部低速晶振(LSE),使用外设低速晶振
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET&&temp<250)	//检查指定的RCC标志位设置与否,等待低速晶振就绪
			{
			temp++;
			delay_ms(10);
			}  
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//设置RTC时钟(RTCCLK),选择LSE作为RTC时钟    
		RCC_RTCCLKCmd(ENABLE);	//使能RTC时钟  
		RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
		RTC_WaitForSynchro();		//等待RTC寄存器同步  
		RTC_ITConfig(RTC_IT_SEC, ENABLE);		//使能RTC秒中断
		RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
		RTC_EnterConfigMode();/// 允许配置	
		RTC_SetPrescaler(32767); //设置RTC预分频的值
		RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
		RTC_Set((u16)pSel[0],(u8)pSel[1],(u8)pSel[2],(u8)pSel[3],(u8)pSel[4],(u8)pSel[5]);	
		RTC_ExitConfigMode(); //退出配置模式  
		BKP_WriteBackupRegister(BKP_DR1, 0X5253);	//向指定的后备寄存器中写入用户程序数据
}
//设置时钟
void set_alarmA(const int *tim)
{
		u8 temp=0;
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//使能PWR和BKP外设时钟   
		PWR_BackupAccessCmd(ENABLE);	//使能后备寄存器访问  			
		BKP_DeInit();	//复位备份区域 	
		RCC_LSEConfig(RCC_LSE_ON);	//设置外部低速晶振(LSE),使用外设低速晶振
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET&&temp<250)	//检查指定的RCC标志位设置与否,等待低速晶振就绪
			{
			temp++;
			delay_ms(10);
			}  
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//设置RTC时钟(RTCCLK),选择LSE作为RTC时钟    
		RCC_RTCCLKCmd(ENABLE);	//使能RTC时钟  
		RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
		RTC_WaitForSynchro();		//等待RTC寄存器同步  
		RTC_ITConfig(RTC_IT_SEC, ENABLE);		//使能RTC秒中断
		RTC_ITConfig(RTC_IT_ALR,ENABLE);
		RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
		RTC_EnterConfigMode();/// 允许配置	
		RTC_SetPrescaler(32767); //设置RTC预分频的值
		RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
		RTC_Set(Calendar.year,Calendar.month,Calendar.date,Calendar.hour,Calendar.min,Calendar.sec);
		RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
		RTC_Alarm_Seta(tim[0],tim[1],tim[2],tim[3],tim[4],0);
		RTC_ExitConfigMode(); //退出配置模式  
		BKP_WriteBackupRegister(BKP_DR1, 0X5253);	//向指定的后备寄存器中写入用户程序数据
}
// 定义一个信号量
OS_SEM Clock_SEM;

int main(void)
{	
	OS_ERR err;
	CPU_SR_ALLOC();
	
	delay_init();	    	//延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200
 	LED_Init();			    //LED端口初始化
	TFTLCD_Init();			//LCD初始化	
	KEY_Init();	 			//按键初始化
	BEEP_Init();			//初始化蜂鸣器
	FSMC_SRAM_Init();		//初始化SRAM
	RTC_Init();
	Ble_Test();                                    //ble测试
	my_mem_init(SRAMIN); 	//初始化内部内存池
	my_mem_init(SRAMEX);  	//初始化外部内存池
	
	exfuns_init();			//为fatfs文件系统分配内存
	f_mount(fs[0],"0:",1);	//挂载SD卡
	f_mount(fs[1],"1:",1);	//挂载FLASH
	
	while(SD_Init())		//检测SD卡
	{
		LCD_ShowString(30,90,200,16,16,"SD Card Failed!");
		delay_ms(200);
		LCD_Fill(30,90,200+30,70+16,WHITE);
		delay_ms(200);		    
	}
	
	TP_Init();				//触摸屏初始化
	
	OSInit(&err);			//初始化UCOSIII
	OS_CRITICAL_ENTER();	//进入临界区
	//创建开始任务
	OSTaskCreate(  (OS_TCB 	* )&StartTaskTCB,		//任务控制块
				         (CPU_CHAR	* )"start task", 		//任务名字
                 (OS_TASK_PTR )start_task, 			//任务函数
                 (void		* )0,					//传递给任务函数的参数
                 (OS_PRIO	  )START_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&START_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)START_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,					//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
                 (OS_ERR 	* )&err);				//存放该函数错误时的返回值
	OS_CRITICAL_EXIT();	//退出临界区	 
	OSStart(&err);  //开启UCOSIII
	while(1);
}

//开始任务函数
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//统计任务                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();	
#endif

#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
	 //使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC,ENABLE);//开启CRC时钟
	GUI_Init();  			//STemWin初始化
	
	OS_CRITICAL_ENTER();	//进入临界区
	//STemWin Demo任务	
	OSTaskCreate(  (OS_TCB*     )&EmwindemoTaskTCB,		
				         (CPU_CHAR*   )"Emwindemo task", 		
                 (OS_TASK_PTR )emwindemo_task, 			
                 (void*       )0,					
                 (OS_PRIO	    ) EMWINDEMO_TASK_PRIO,     
                 (CPU_STK*    )&EMWINDEMO_TASK_STK[0],	
                 (CPU_STK_SIZE)EMWINDEMO_STK_SIZE/10,	
                 (CPU_STK_SIZE)EMWINDEMO_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);
	//触摸屏任务
	OSTaskCreate((OS_TCB*     )&TouchTaskTCB,		
				 (CPU_CHAR*   )"Touch task", 		
                 (OS_TASK_PTR )touch_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )TOUCH_TASK_PRIO,     
                 (CPU_STK*    )&TOUCH_TASK_STK[0],	
                 (CPU_STK_SIZE)TOUCH_STK_SIZE/10,	
                 (CPU_STK_SIZE)TOUCH_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);			 
	//LED0任务
	OSTaskCreate((OS_TCB*     )&Led0TaskTCB,		
				 (CPU_CHAR*   )"Led0 task", 		
                 (OS_TASK_PTR )led0_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )LED0_TASK_PRIO,     
                 (CPU_STK*    )&LED0_TASK_STK[0],	
                 (CPU_STK_SIZE)LED0_STK_SIZE/10,	
                 (CPU_STK_SIZE)LED0_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);
								 
								 //创建TASK1任务
	OSTaskCreate((OS_TCB 	* )&Task1_TaskTCB,		
				 (CPU_CHAR	* )"Task1 task", 		
                 (OS_TASK_PTR )task1_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )TASK1_TASK_PRIO,     
                 (CPU_STK   * )&TASK1_TASK_STK[0],	
                 (CPU_STK_SIZE)TASK1_STK_SIZE/10,	
                 (CPU_STK_SIZE)TASK1_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);	
							//创建KEY任务
	OSTaskCreate((OS_TCB 	* )&KEY_TASKTCB,		
				 (CPU_CHAR	* )"KEY task", 		
                 (OS_TASK_PTR )key_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )Key_TASK_PRIO,     
                 (CPU_STK   * )&KEY_TASK_STK[0],	
                 (CPU_STK_SIZE)KEY_STK_SIZE/10,	
                 (CPU_STK_SIZE)KEY_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);	
							//创建BLE任务
	OSTaskCreate((OS_TCB 	* )&BLE_TASKTCB,		
				 (CPU_CHAR	* )"BLE task", 		
                 (OS_TASK_PTR )BLE_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )BLE_TASK_PRIO,     
                 (CPU_STK   * )&BLE_TASK_STK[0],	
                 (CPU_STK_SIZE)BLE_STK_SIZE/10,	
                 (CPU_STK_SIZE)BLE_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);		
	//创建信号量
			OSSemCreate ((OS_SEM   *)&Clock_SEM,
                   (CPU_CHAR *)"Clock_SEM",
                   (OS_SEM_CTR)  1,
                   (OS_ERR  *) &err);
								 
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//挂起开始任务			 
	OS_CRITICAL_EXIT();	//退出临界区
}

//EMWINDEMO任务
void emwindemo_task(void *p_arg)
{

	GUI_CURSOR_Show(); //显示鼠标
	
	while(1)
	{
	 numdisplay_demo();
		//GUI_Delay(100);
	}
}
//KEY任务
void key_task(void *p_arg)
{
	u8 key;
	OS_ERR err;
	while(1)
	{
			key =KEY_Scan(0); 
		if(key)
		{
			if(key==WKUP_PRES)//if(WK_UP== key)
			{
				OSSchedLock(&err); //调度器上锁
					printf("\r\n按下上键\r\n");BEEP=!BEEP;
				OSSchedUnlock(&err); //调度器解锁
			}
	}
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err);//延时10ms
}
}
//BLE任务
void BLE_task(void *p_arg)
{
		u8 key = 0;
    u8 t = 0;
    u8 BLE_LINK = 0; //1:蓝牙连接 0:未连接
    static u8 flag1, flag2 = 0;
		int time[6]={0},flagp=0;
    printf("\r\nKEY1:发送数据\r\n");

    while (1)
    {

        key = KEY_Scan(0);

        if(key == KEY1_PRES) //发送数据
        {
            if (BLE_LINK) /*蓝牙已连接*/
            {
                Ble_SendData();/*数据发送处理*/
            }
        }

        /*数据接收处理*/
        flagp=Ble_ReceData(time);
				if(flagp == 1)
				{
						flagp=0;
					printf("\r\n通过蓝牙设置时间成功!%d-%d-%d %d:%d:%d\r\n",time[0],time[1],time[2],time[3],time[4],time[5]);
						changtime(time);
				}else if(flagp == 2){
						flagp=0;
						printf("\r\n通过蓝牙设置闹钟成功! %d:%d:%d\r\n",time[3],time[4],time[5]);
						set_alarmA(time);
				}
        /*BLE连接处理*/
        if (BLE_STA)
        {
            BLE_LINK = 1; //蓝牙连接

            if (!flag1)
            {
                flag1 = 1;
                flag2 = 0;
                printf("\r\nble connect!\r\n");
            }
        }
        else
        {
            BLE_LINK = 0; //蓝牙断开

            if (!flag2)
            {

                flag2 = 1;
                flag1 = 0;
                printf("\r\nble disconnect!\r\n");
            }
        }

        t++;
        if (t == 20)
        {
            t = 0;
            LED1 = ~LED1;
        }
        delay_ms(10);

    }
}
//TOUCH任务
void touch_task(void *p_arg)
{
	OS_ERR err;
	while(1)
	{
		GUI_TOUCH_Exec();	
		OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_PERIODIC,&err);//延时5ms
	}
}

//LED0任务
void led0_task(void *p_arg)
{
	OS_ERR err;
	while(1)
	{
		LED0 = !LED0;
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_PERIODIC,&err);//延时500ms
	}
}

//task1任务函数
void task1_task(void *p_arg)
{
	
	u32 temp,ytemp;//定义两个变量一个表示天 一个表示 年
	OS_ERR err;
	p_arg = p_arg;
	
	while(1)
	{
		OSSemPend (  (OS_SEM *) &Clock_SEM,
                 (OS_TICK )  0,
                 (OS_OPT  )  OS_OPT_PEND_BLOCKING,//阻塞式
                 (CPU_TS *) 0,
                 (OS_ERR* ) &err);   //等待中断进行处理数据
		 temp = Clock_count/86400;       //根据秒得到天   
		    ytemp=1970;    					      //从1970年开始计算
	   if(temp!=0)   //得到年
			 {						  		   	                   
					while(temp>=365) 
						{ 
							if(ytemp%4==0)
								{                   //是否闰年 
											if(ytemp>=366) temp-=366;   	  //闰年的秒数 
								 else{
											 ytemp++;
										 break;
									 } 
								} 
							 else temp-=365;               	  //非闰年 
							   ytemp++; 			 
						}
	     }
			 Calendar.year = ytemp;
			 
			  ytemp=0; 
	   while(temp>=28)					  	  //超过了一个月 
	   { 
	   	  if((Calendar.year%4==0)&&ytemp==1)     //闰年 2月
		  { 
			if(temp>=29)temp-=29;			  //闰年2月有29天
			else break; 
		  } 
		  else if((Calendar.year%4!=0)&&ytemp==1)//不是闰年 2月 
		  { 
			if(temp>=28)temp-=28;			  //非闰年2月有28天
			else break; 
		  } 
		  else if(ytemp==0||ytemp==2||ytemp==4||ytemp==6||ytemp==7||ytemp==9||ytemp==11)    //大月月份  
		  { 
			if(temp>=31)temp-=31;		
			else break; 
		  }  
		  else if(ytemp==3||ytemp==5||ytemp==8||ytemp==10)									//小月月份 
		  { 
			if(temp>=30)temp-=30;		
			else break; 
		  }  
		  ytemp++; 
		} 
		 
		Calendar.month=ytemp+1;						//月份 
		Calendar.date=temp+1; 							//日 
	 
		temp=Clock_count%86400; 					//得到秒钟数 
		Calendar.hour=temp/3600; 						//计算出小时 
		Calendar.min =(temp%3600)/60; 					//计算出分钟 
		Calendar.sec =(temp%3600)%60; 					//计算出秒钟 
		Calendar.week =RTC_Week( Calendar.year,Calendar.month,Calendar.date); //计算出周		
	}
}
/****************************************************************************
* 名    称：unsigned char RTC_Week(unsigned int year,unsigned char month,unsigned char day)
* 功    能：计算出星期几的函数
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
unsigned char RTC_Week(unsigned int year,unsigned char month,unsigned char day)
{	
	unsigned int tmp;
	unsigned char YH,YL;
	unsigned char week_M[12]={0,3,3,6,1,4,6,2,5,0,3,5};     
	YH=year/100;	
	YL=year%100;	 
	if (YH>19)YL+=100;
	tmp=YL+YL/4;
	tmp=tmp%7; 
	tmp=tmp+day+week_M[month-1];
	if (YL%4==0&&month<3)tmp--;
	return(tmp%7);
}

void clock_Week_dis(unsigned int a, unsigned int b){
	GUI_SetColor(GUI_WHITE);
	GUI_SetFont(GUI_FONT_32B_1);  
 	if(Calendar.week==1) GUI_DispStringAt("Monday", a, b);
	else if(Calendar.week==2) GUI_DispStringAt("Tuesday", a, b);
	else if(Calendar.week==3) GUI_DispStringAt("Wednesday", a, b);
	else if(Calendar.week==4) GUI_DispStringAt("Thursday", a, b);
	else if(Calendar.week==5) GUI_DispStringAt("Friday", a, b);
	else if(Calendar.week==6) GUI_DispStringAt("Saturday", a, b);
	else if(Calendar.week==7) GUI_DispStringAt("Sunday", a, b);

} 


