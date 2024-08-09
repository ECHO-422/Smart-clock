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
_calendar_obj Calendar;  //����һ���������� ��clock.h ����
unsigned char RTC_Week(unsigned int year,unsigned char month,unsigned char day);
void clock_Week_dis(unsigned int a, unsigned int b);
volatile unsigned int curs,page,key,num,flash;
//�������ȼ�
#define START_TASK_PRIO				3
//�����ջ��С	
#define START_STK_SIZE 				1024
//������ƿ�
OS_TCB StartTaskTCB;
//�����ջ	
CPU_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *p_arg);

//TOUCH����
//�����������ȼ�
#define TOUCH_TASK_PRIO				6
//�����ջ��С
#define TOUCH_STK_SIZE				128
//������ƿ�
OS_TCB TouchTaskTCB;
//�����ջ
CPU_STK TOUCH_TASK_STK[TOUCH_STK_SIZE];
//touch����
void touch_task(void *p_arg);

//LED0����
//�����������ȼ�
#define LED0_TASK_PRIO 				7
//�����ջ��С
#define LED0_STK_SIZE				128
//������ƿ�
OS_TCB Led0TaskTCB;
//�����ջ
CPU_STK LED0_TASK_STK[LED0_STK_SIZE];
//led0����
void led0_task(void *p_arg);

//EMWINDEMO����
//�����������ȼ�
#define EMWINDEMO_TASK_PRIO			11
//�����ջ��С
#define EMWINDEMO_STK_SIZE			2048
//������ƿ�
OS_TCB EmwindemoTaskTCB;
//�����ջ
CPU_STK EMWINDEMO_TASK_STK[EMWINDEMO_STK_SIZE];
//emwindemo_task����
void emwindemo_task(void *p_arg);

//�������ȼ�
#define TASK1_TASK_PRIO		8
//�����ջ��С	
#define TASK1_STK_SIZE 		128
//������ƿ�
OS_TCB Task1_TaskTCB;
//�����ջ	
CPU_STK TASK1_TASK_STK[TASK1_STK_SIZE];
void task1_task(void *p_arg);

#define Key_TASK_PRIO 5
#define KEY_STK_SIZE 128
OS_TCB	KEY_TASKTCB;
CPU_STK	KEY_TASK_STK[KEY_STK_SIZE];
void key_task(void *p_arg);
//����ͨ��
#define BLE_TASK_PRIO 9
#define BLE_STK_SIZE 128
OS_TCB  BLE_TASKTCB;
CPU_STK BLE_TASK_STK[BLE_STK_SIZE]; 
void BLE_task(void *p_arg);

//�ı�ʱ��
void changtime(const int *pSel)
{
		u8 temp=0;
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//ʹ��PWR��BKP����ʱ��   
		PWR_BackupAccessCmd(ENABLE);	//ʹ�ܺ󱸼Ĵ�������  			
		BKP_DeInit();	//��λ�������� 	
		RCC_LSEConfig(RCC_LSE_ON);	//�����ⲿ���پ���(LSE),ʹ��������پ���
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET&&temp<250)	//���ָ����RCC��־λ�������,�ȴ����پ������
			{
			temp++;
			delay_ms(10);
			}  
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//����RTCʱ��(RTCCLK),ѡ��LSE��ΪRTCʱ��    
		RCC_RTCCLKCmd(ENABLE);	//ʹ��RTCʱ��  
		RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		RTC_WaitForSynchro();		//�ȴ�RTC�Ĵ���ͬ��  
		RTC_ITConfig(RTC_IT_SEC, ENABLE);		//ʹ��RTC���ж�
		RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		RTC_EnterConfigMode();/// ��������	
		RTC_SetPrescaler(32767); //����RTCԤ��Ƶ��ֵ
		RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		RTC_Set((u16)pSel[0],(u8)pSel[1],(u8)pSel[2],(u8)pSel[3],(u8)pSel[4],(u8)pSel[5]);	
		RTC_ExitConfigMode(); //�˳�����ģʽ  
		BKP_WriteBackupRegister(BKP_DR1, 0X5253);	//��ָ���ĺ󱸼Ĵ�����д���û���������
}
//����ʱ��
void set_alarmA(const int *tim)
{
		u8 temp=0;
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//ʹ��PWR��BKP����ʱ��   
		PWR_BackupAccessCmd(ENABLE);	//ʹ�ܺ󱸼Ĵ�������  			
		BKP_DeInit();	//��λ�������� 	
		RCC_LSEConfig(RCC_LSE_ON);	//�����ⲿ���پ���(LSE),ʹ��������پ���
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET&&temp<250)	//���ָ����RCC��־λ�������,�ȴ����پ������
			{
			temp++;
			delay_ms(10);
			}  
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//����RTCʱ��(RTCCLK),ѡ��LSE��ΪRTCʱ��    
		RCC_RTCCLKCmd(ENABLE);	//ʹ��RTCʱ��  
		RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		RTC_WaitForSynchro();		//�ȴ�RTC�Ĵ���ͬ��  
		RTC_ITConfig(RTC_IT_SEC, ENABLE);		//ʹ��RTC���ж�
		RTC_ITConfig(RTC_IT_ALR,ENABLE);
		RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		RTC_EnterConfigMode();/// ��������	
		RTC_SetPrescaler(32767); //����RTCԤ��Ƶ��ֵ
		RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		RTC_Set(Calendar.year,Calendar.month,Calendar.date,Calendar.hour,Calendar.min,Calendar.sec);
		RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		RTC_Alarm_Seta(tim[0],tim[1],tim[2],tim[3],tim[4],0);
		RTC_ExitConfigMode(); //�˳�����ģʽ  
		BKP_WriteBackupRegister(BKP_DR1, 0X5253);	//��ָ���ĺ󱸼Ĵ�����д���û���������
}
// ����һ���ź���
OS_SEM Clock_SEM;

int main(void)
{	
	OS_ERR err;
	CPU_SR_ALLOC();
	
	delay_init();	    	//��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
 	LED_Init();			    //LED�˿ڳ�ʼ��
	TFTLCD_Init();			//LCD��ʼ��	
	KEY_Init();	 			//������ʼ��
	BEEP_Init();			//��ʼ��������
	FSMC_SRAM_Init();		//��ʼ��SRAM
	RTC_Init();
	Ble_Test();                                    //ble����
	my_mem_init(SRAMIN); 	//��ʼ���ڲ��ڴ��
	my_mem_init(SRAMEX);  	//��ʼ���ⲿ�ڴ��
	
	exfuns_init();			//Ϊfatfs�ļ�ϵͳ�����ڴ�
	f_mount(fs[0],"0:",1);	//����SD��
	f_mount(fs[1],"1:",1);	//����FLASH
	
	while(SD_Init())		//���SD��
	{
		LCD_ShowString(30,90,200,16,16,"SD Card Failed!");
		delay_ms(200);
		LCD_Fill(30,90,200+30,70+16,WHITE);
		delay_ms(200);		    
	}
	
	TP_Init();				//��������ʼ��
	
	OSInit(&err);			//��ʼ��UCOSIII
	OS_CRITICAL_ENTER();	//�����ٽ���
	//������ʼ����
	OSTaskCreate(  (OS_TCB 	* )&StartTaskTCB,		//������ƿ�
				         (CPU_CHAR	* )"start task", 		//��������
                 (OS_TASK_PTR )start_task, 			//������
                 (void		* )0,					//���ݸ��������Ĳ���
                 (OS_PRIO	  )START_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&START_TASK_STK[0],	//�����ջ����ַ
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)START_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )0,					//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,					//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,					//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
                 (OS_ERR 	* )&err);				//��Ÿú�������ʱ�ķ���ֵ
	OS_CRITICAL_EXIT();	//�˳��ٽ���	 
	OSStart(&err);  //����UCOSIII
	while(1);
}

//��ʼ������
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//ͳ������                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//���ʹ���˲����жϹر�ʱ��
    CPU_IntDisMeasMaxCurReset();	
#endif

#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //��ʹ��ʱ��Ƭ��ת��ʱ��
	 //ʹ��ʱ��Ƭ��ת���ȹ���,ʱ��Ƭ����Ϊ1��ϵͳʱ�ӽ��ģ���1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC,ENABLE);//����CRCʱ��
	GUI_Init();  			//STemWin��ʼ��
	
	OS_CRITICAL_ENTER();	//�����ٽ���
	//STemWin Demo����	
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
	//����������
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
	//LED0����
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
								 
								 //����TASK1����
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
							//����KEY����
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
							//����BLE����
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
	//�����ź���
			OSSemCreate ((OS_SEM   *)&Clock_SEM,
                   (CPU_CHAR *)"Clock_SEM",
                   (OS_SEM_CTR)  1,
                   (OS_ERR  *) &err);
								 
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//����ʼ����			 
	OS_CRITICAL_EXIT();	//�˳��ٽ���
}

//EMWINDEMO����
void emwindemo_task(void *p_arg)
{

	GUI_CURSOR_Show(); //��ʾ���
	
	while(1)
	{
	 numdisplay_demo();
		//GUI_Delay(100);
	}
}
//KEY����
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
				OSSchedLock(&err); //����������
					printf("\r\n�����ϼ�\r\n");BEEP=!BEEP;
				OSSchedUnlock(&err); //����������
			}
	}
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err);//��ʱ10ms
}
}
//BLE����
void BLE_task(void *p_arg)
{
		u8 key = 0;
    u8 t = 0;
    u8 BLE_LINK = 0; //1:�������� 0:δ����
    static u8 flag1, flag2 = 0;
		int time[6]={0},flagp=0;
    printf("\r\nKEY1:��������\r\n");

    while (1)
    {

        key = KEY_Scan(0);

        if(key == KEY1_PRES) //��������
        {
            if (BLE_LINK) /*����������*/
            {
                Ble_SendData();/*���ݷ��ʹ���*/
            }
        }

        /*���ݽ��մ���*/
        flagp=Ble_ReceData(time);
				if(flagp == 1)
				{
						flagp=0;
					printf("\r\nͨ����������ʱ��ɹ�!%d-%d-%d %d:%d:%d\r\n",time[0],time[1],time[2],time[3],time[4],time[5]);
						changtime(time);
				}else if(flagp == 2){
						flagp=0;
						printf("\r\nͨ�������������ӳɹ�! %d:%d:%d\r\n",time[3],time[4],time[5]);
						set_alarmA(time);
				}
        /*BLE���Ӵ���*/
        if (BLE_STA)
        {
            BLE_LINK = 1; //��������

            if (!flag1)
            {
                flag1 = 1;
                flag2 = 0;
                printf("\r\nble connect!\r\n");
            }
        }
        else
        {
            BLE_LINK = 0; //�����Ͽ�

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
//TOUCH����
void touch_task(void *p_arg)
{
	OS_ERR err;
	while(1)
	{
		GUI_TOUCH_Exec();	
		OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_PERIODIC,&err);//��ʱ5ms
	}
}

//LED0����
void led0_task(void *p_arg)
{
	OS_ERR err;
	while(1)
	{
		LED0 = !LED0;
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_PERIODIC,&err);//��ʱ500ms
	}
}

//task1������
void task1_task(void *p_arg)
{
	
	u32 temp,ytemp;//������������һ����ʾ�� һ����ʾ ��
	OS_ERR err;
	p_arg = p_arg;
	
	while(1)
	{
		OSSemPend (  (OS_SEM *) &Clock_SEM,
                 (OS_TICK )  0,
                 (OS_OPT  )  OS_OPT_PEND_BLOCKING,//����ʽ
                 (CPU_TS *) 0,
                 (OS_ERR* ) &err);   //�ȴ��жϽ��д�������
		 temp = Clock_count/86400;       //������õ���   
		    ytemp=1970;    					      //��1970�꿪ʼ����
	   if(temp!=0)   //�õ���
			 {						  		   	                   
					while(temp>=365) 
						{ 
							if(ytemp%4==0)
								{                   //�Ƿ����� 
											if(ytemp>=366) temp-=366;   	  //��������� 
								 else{
											 ytemp++;
										 break;
									 } 
								} 
							 else temp-=365;               	  //������ 
							   ytemp++; 			 
						}
	     }
			 Calendar.year = ytemp;
			 
			  ytemp=0; 
	   while(temp>=28)					  	  //������һ���� 
	   { 
	   	  if((Calendar.year%4==0)&&ytemp==1)     //���� 2��
		  { 
			if(temp>=29)temp-=29;			  //����2����29��
			else break; 
		  } 
		  else if((Calendar.year%4!=0)&&ytemp==1)//�������� 2�� 
		  { 
			if(temp>=28)temp-=28;			  //������2����28��
			else break; 
		  } 
		  else if(ytemp==0||ytemp==2||ytemp==4||ytemp==6||ytemp==7||ytemp==9||ytemp==11)    //�����·�  
		  { 
			if(temp>=31)temp-=31;		
			else break; 
		  }  
		  else if(ytemp==3||ytemp==5||ytemp==8||ytemp==10)									//С���·� 
		  { 
			if(temp>=30)temp-=30;		
			else break; 
		  }  
		  ytemp++; 
		} 
		 
		Calendar.month=ytemp+1;						//�·� 
		Calendar.date=temp+1; 							//�� 
	 
		temp=Clock_count%86400; 					//�õ������� 
		Calendar.hour=temp/3600; 						//�����Сʱ 
		Calendar.min =(temp%3600)/60; 					//��������� 
		Calendar.sec =(temp%3600)%60; 					//��������� 
		Calendar.week =RTC_Week( Calendar.year,Calendar.month,Calendar.date); //�������		
	}
}
/****************************************************************************
* ��    �ƣ�unsigned char RTC_Week(unsigned int year,unsigned char month,unsigned char day)
* ��    �ܣ���������ڼ��ĺ���
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
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


