#include "Clockdisplay.h"
#include "jpegdisplay.h"
#include "bmpdisplay.h"
#include "led.h"
#include "beep.h"
#include "string.h"
#include "math.h"
extern _calendar_obj Calendar;
_calendar_obj Clock1;

//volatile u8 num,curs,key;
u32 time_c;
_calendar_obj Clock1;
#define M 30
#define LCD_WX 800
#define LCD_HX 480
#define WHEELHX 200
#define WHEELWX 80 
#define FRWINWX 700
#define FRWINHX 350
//
#define ID_FRAMEWIN_0        (GUI_ID_USER + 0x00)
#define ID_LISTBOX_alarm       (GUI_ID_USER + 0x01)
#define ID_LISTBOX_0        (GUI_ID_USER + 0x02)
#define ID_LISTBOX_1        (GUI_ID_USER + 0x03)
//#define ID_TEXT_0        (GUI_ID_USER + 0x04)
#define ID_TEXT_1        (GUI_ID_USER + 0x05)
#define ID_SPINBOX_0        (GUI_ID_USER + 0x07)
#define ID_TEXT_2        (GUI_ID_USER + 0x08)
#define ID_BUTTON_0        (GUI_ID_USER + 0x0B)
#define ID_BUTTON_1        (GUI_ID_USER + 0x0C)
#define ID_BUTTON_2        (GUI_ID_USER + 0x0D)
#define ID_BUTTON_3        (GUI_ID_USER + 0x0E)
#define ID_SCROLLBAR_0        (GUI_ID_USER + 0x0F)
#define ID_SCROLLBAR_1        (GUI_ID_USER + 0x10)
#define ID_SCROLLBAR_2        (GUI_ID_USER + 0x11)
#define centerx 660
#define centery 120
//世界时区
#define ID_FRAMEWIN_ZONE       (GUI_ID_USER + 0x12)
#define ID_IMAGE_ZONE       (GUI_ID_USER + 0x13)
#define ID_RADIO_ZONE      (GUI_ID_USER + 0x14)
#define ID_TEXT_ZONE      (GUI_ID_USER + 0x15)
#define ID_BUTTON_ZONE      (GUI_ID_USER + 0x16)
#define ID_BUTTON_ZONE_EXIT      (GUI_ID_USER + 0x17)
//
BUTTON_Handle  _ahButton[8];
WM_HWIN  g_hText_time,g_hText_data,g_hText_week;                //显示日期时间星期的三个文本框句柄
WM_HWIN  g_hSDTFrame = 0 ,g_hCFGFrame=0,g_hdialog_alarm=0;      //时间设置，系统配置，闹铃对话框句柄
void clock_time_dis(unsigned int a, unsigned int b);
void clock_date_dis(unsigned int a, unsigned int b);
void clock_Week_dis(unsigned int a, unsigned int b);
typedef struct {
    WM_HWIN   hWin;
    int       LineHeight;
    const GUI_FONT GUI_UNI_PTR* pFont;
} xWheel_TypeDef ;   //用于listwheel的自定义绘画
static char _currlenDay=0;  //当前月的日数
static xWheel_TypeDef _aWheel_date[3];
static xWheel_TypeDef _aWheel_time[3];
#define _apSecond   _apMinute//用60分钟定义秒钟
//设置日期的全局变量
static char * _apYear[] = {
   "2009", "2010", "2011", "2012", "2013", "2014", "2015", "2016", "2017", "2018", "2019", "2020","2021","2022","2023","2024","2025","2026","2027","2028","2029",
};

static char * _apMonth[] = {
  "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December",
};

static char * _apDay[] = {
  "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31",
};
static char * _apHour[] = {
    "00","01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23",
};
static char * _apMinute[] = {
   "00","01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", 
   "31","32", "33", "34", "35", "36", "37", "38", "39", "40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "50", "51", "52", "53", "54", "55", "56", "57", "58", "59", 
};//从wheelList中选出时间
static int Month[]={1,2,3,4,5,6,7,8,9,10,11,12};
static int Sixty[]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59};
static int year[]={2009,2010,2011,2012,2013,2014,2015,2016,2017,2018,2019,2020,2021,2022,2023,2024,2025,2026,2027,2028,2029};
static int day[]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};
static int Hour[]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23};
static int _aSelection_date[3] = {0, 0, 0};
static int _aSelection_time[3] = {0, 0,  0};
static int _alarmtime[2]={0,0};
static int pos_alarm[2]={0,0};
static char *alarm_set[]={"0","0"};
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_0, 20, 10, 650, 460, 0, 0x64, 0 },
	{ LISTBOX_CreateIndirect, "Listbox", ID_LISTBOX_alarm, 22, 24, 250, 330, 0, 0x64, 0 },
  { LISTBOX_CreateIndirect, "Listbox", ID_LISTBOX_0, 307, 47, 112, 301, 0, 0x0, 0 },
  { LISTBOX_CreateIndirect, "Listbox", ID_LISTBOX_1, 453, 48, 113, 298, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Hour", ID_TEXT_0, 311, 26, 107, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Minute", ID_TEXT_1, 454, 28, 108, 20, 0, 0x0, 0 },
  //{ SPINBOX_CreateIndirect, "Spinbox", ID_SPINBOX_0, 611, 48, 123, 45, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "", ID_TEXT_2, 617, 19, 114, 31, 0, 0x64, 0 },
  { BUTTON_CreateIndirect, "DISABLE", ID_BUTTON_0, 45, 383, 108, 38, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "DEL", ID_BUTTON_1, 196, 383, 108, 38, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "ADD", ID_BUTTON_2, 349, 382, 108, 38, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "OK", ID_BUTTON_3, 507, 383, 108, 38, 0, 0x0, 0 },
};
static int now_zone=0;//北京
static int another[]={0,-12,6,2,-6};
//创建世界时区

//volatile u8 flash;
//画圆盘
void draw_Circle()
{	
		int r=110;
		int i,angle;
		double pie=3.14;
		GUI_SetColor(GUI_BLACK);	
		GUI_SetPenSize(4);
		GUI_DrawCircle(660, 120,r);
		GUI_SetColor(GUI_WHITE);
		GUI_FillCircle(660,120,r-1);
		GUI_FillCircle(660,120,5);
		GUI_DrawArc(660, 120, 100, 100, 0, 360);
		for(i=0;i<=12;i++)
		{
				angle=i*30;
				GUI_SetColor(GUI_BLACK);
				GUI_SetPenSize(4);
				GUI_DrawLine(centerx+r*cos((angle/180.0)*pie),centery-r*sin((angle/180.0)*pie),centerx+(r-20)*cos((angle/180.0)*pie),centery-(r-20)*sin((angle/180.0)*pie));
		}
}
//秒针
void draw_Secmark()
{
	int length=90;
	int second=Calendar.sec;
	double pie=3.14;
	GUI_SetPenSize(4);
	GUI_SetColor(GUI_WHITE);//设置指针的颜色为红色
	GUI_DrawLine(centerx,centery,centerx+length*cos((90-((second-1)*6))/180.0*pie),centery-length*sin((90-((second-1)*6))/180.0*pie));
	GUI_SetColor(GUI_RED);//设置指针的颜色为红色
	GUI_DrawLine(centerx,centery,centerx+length*cos((90-(second*6))/180.0*pie),centery-length*sin((90-(second*6))/180.0*pie));
}
//分针
void draw_minmark()
{
	int length=75;
	int second=Calendar.sec,minute=Calendar.min;
	double pie=3.14;
	GUI_SetPenSize(4);	
	GUI_SetColor(GUI_WHITE);//设置指针的颜色为黑色
	GUI_DrawLine(centerx,centery,centerx+length*cos((90-((second+minute*60-1)/60.0)*6)/180.0*pie),centery-length*sin((90-((second+minute*60-1)/60.0)*6)/180.0*pie));
	GUI_SetColor(GUI_BLACK);//设置指针的颜色为黑色
	GUI_DrawLine(centerx,centery,centerx+length*cos((90-((second+minute*60)/60.0)*6)/180.0*pie),centery-length*sin((90-((second+minute*60)/60.0)*6)/180.0*pie));
}
//时针
void draw_hourmark()
{
	int hourarmlength=55;
	int hour=Calendar.hour;
	int minute=Calendar.min;
	double pie=3.14;
	GUI_SetPenSize(4);
	GUI_SetColor(GUI_WHITE);//设置指针的颜色为黑色
	GUI_DrawLine(centerx,centery,centerx+hourarmlength*cos((90-((minute+hour*60-1)/60.0)*30)/180.0*pie),centery-hourarmlength*sin((90-((hour*60+minute-1)/60.0)*30)/180.0*pie));
	GUI_SetColor(GUI_BLACK);//设置指针的颜色为黑色
	GUI_DrawLine(centerx,centery,centerx+hourarmlength*cos((90-((minute+hour*60)/60.0)*30)/180.0*pie),centery-hourarmlength*sin((90-((hour*60+minute)/60.0)*30)/180.0*pie));
}
//改变时间
void changetime(const int *pSel,const int *pSel1)
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
		RTC_Set((u16)pSel[0],(u8)pSel[1],(u8)pSel[2],(u8)pSel1[0],(u8)pSel1[1],(u8)pSel1[2]);	
		RTC_ExitConfigMode(); //退出配置模式  
		BKP_WriteBackupRegister(BKP_DR1, 0X5253);	//向指定的后备寄存器中写入用户程序数据
}
//设置闹钟
void set_alarmF(const int *tim)
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
		RTC_Alarm_Seta(Calendar.year,Calendar.month,Calendar.date,tim[0],tim[1],0);
		RTC_ExitConfigMode(); //退出配置模式  
		BKP_WriteBackupRegister(BKP_DR1, 0X5253);	//向指定的后备寄存器中写入用户程序数据
}
//根据年月计算天数
int   ADJDay(unsigned int year,unsigned int month )
{
    int len=0;
    switch (month)
    {
    case 1:case 3:case 5:case 7:case 8:case 10:case 12:
        len= 31;
        break;
    case 2:
        if ( Is_Leap_Year(year) )
        {
            len= 29;
        }else 
        { 
            len= 28;
        }
        break;
    case 4:case 6:case 9:case 11:
        len = 30;
        break;
    }
    return len;
}
//设置闹钟
static void _cbDialog(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;
	int i,r,pos; 
	char am[8];
	SCROLLBAR_SetDefaultSkin(SCROLLBAR_SKIN_FLEX);
	SCROLLBAR_SetDefaultWidth(15);//设置宽度
	BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);
  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    // Initialization of 'Framewin'
    hItem = pMsg->hWin;
		FRAMEWIN_SetDefaultSkinClassic();
		FRAMEWIN_SetClientColor(hItem,GUI_BLACK);
    FRAMEWIN_SetText(hItem, "ALARM");
    FRAMEWIN_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
    FRAMEWIN_SetTitleHeight(hItem, 20);
    FRAMEWIN_SetFont(hItem, GUI_FONT_20B_ASCII);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
    TEXT_SetFont(hItem, GUI_FONT_32B_ASCII);
		TEXT_SetTextColor(hItem,GUI_WHITE);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    // Initialization of 'Minute'
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
    TEXT_SetFont(hItem, GUI_FONT_32B_ASCII);
		TEXT_SetTextColor(hItem,GUI_WHITE);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX_alarm);
		LISTBOX_SetFont(hItem, GUI_FONT_24B_ASCII);
    hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX_0);
		for(i=0;i<24;i++)
			LISTBOX_AddString(hItem, _apHour[i]);
    LISTBOX_SetFont(hItem, GUI_FONT_24B_ASCII);
		//将滚动条附加到多行文本控件
		SCROLLBAR_CreateAttached(hItem,SCROLLBAR_CF_VERTICAL);
    // Initialization of 'Listbox'
    hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX_1);
		for(i=0;i<60;i++)
			LISTBOX_AddString(hItem, _apMinute[i]);
    LISTBOX_SetFont(hItem, GUI_FONT_24B_ASCII);
		//将滚动条附加到多行文本控件
		SCROLLBAR_CreateAttached(hItem,SCROLLBAR_CF_VERTICAL);
    // Initialization of 'DISABLE'
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
    BUTTON_SetFont(hItem, GUI_FONT_20B_ASCII);
    // Initialization of 'DEL'
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
    BUTTON_SetFont(hItem, GUI_FONT_20B_ASCII);
    // Initialization of 'ADD'
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
    BUTTON_SetFont(hItem, GUI_FONT_20B_ASCII);
    // Initialization of 'OK'
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_3);
    BUTTON_SetFont(hItem, GUI_FONT_20B_ASCII);
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case ID_LISTBOX_0: // 时
			hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX_0);
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
						pos=LISTBOX_GetSel(hItem);//得到索引
						printf("\r\n闹钟小时的索引为:%d\r\n",pos);
						 pos_alarm[0]=pos;
						_alarmtime[0]=Hour[pos];
						printf("\r\n闹钟小时的时间为:%d\r\n",Hour[pos]);
						alarm_set[0]=_apHour[pos];
        break;
      }
      break;
    case ID_LISTBOX_1: // 分
			hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX_1);
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
						pos=LISTBOX_GetSel(hItem);//得到索引
						printf("\r\n闹钟分钟的索引为:%d\r\n",pos);
						 pos_alarm[1]=pos;
						_alarmtime[1]=Sixty[pos];
						printf("\r\n闹钟分钟的时间为:%d\r\n",Sixty[pos]);
						alarm_set[1]=_apMinute[pos];
        break;
      }
      break;
    case ID_BUTTON_0: // Notifications sent by 'DISABLE'
      switch(NCode) {
      case WM_NOTIFICATION_RELEASED:
        GUI_EndDialog(pMsg->hWin,r);
				draw_Circle();
        break;
      }
      break;
    case ID_BUTTON_1: // Notifications sent by 'DEL'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        break;
      case WM_NOTIFICATION_RELEASED:
        hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX_alarm);
			  LISTBOX_DeleteItem(hItem,LISTBOX_GetSel(ID_LISTBOX_alarm));
        break;
      }
      break;
    case ID_BUTTON_2: // Notifications sent by 'ADD'
      switch(NCode) {
      case WM_NOTIFICATION_RELEASED:
        hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX_alarm);//指向闹钟时间文本框
				strcpy(am,alarm_set[0]);
				strcat(am,":");
				strcat(am,alarm_set[1]);
				printf("   \r\n添加时间成功\r\n  %s",am);
				LISTBOX_AddString(hItem,am);
				printf("   \r\n添加时间成功\r\n  %s",am);
        break;
      }
      break;
    case ID_BUTTON_3: // Notifications sent by 'OK'
      switch(NCode) {
      case WM_NOTIFICATION_RELEASED:
        set_alarmF(_alarmtime);
				GUI_EndDialog(pMsg->hWin,r);
        break;
      }
      break;
    }
    break;
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}
//根据与北京时间的差值	  
//平年的月份日期表
const u8 mo_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};
void setZone(int zone)
{
		int deltime=another[zone];
		int del=another[now_zone];
		int sec,hour,date,month,year;
		u8 temp=0;
		u16 t;
		sec=(deltime-del);
		if(Calendar.hour+sec > 24)
		{Calendar.date++;
		}
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
		//
		//RTC_SetCounter(seccount);	//设置RTC计数器的值
		RTC_ExitConfigMode(); //退出配置模式  
		BKP_WriteBackupRegister(BKP_DR1, 0X5253);	//向指定的后备寄存器中写入用户程序数据
}
//世界时钟
static void _cbFrame_ZONE(WM_MESSAGE * pMsg) {
  const void * pData;
  WM_HWIN      hItem;
  U32          FileSize;
  int          NCode;
  int          Id,r,index;
  switch (pMsg->MsgId) {
		case WM_PAINT:   //WM_PAIMNT消息，窗口重绘自身
			dispbmpex("0:/PICTURE/BMP/MAP.bmp",0,0,0,2,1);break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
   /*switch(Id) {
    case ID_RADIO_ZONE: // Notifications sent by 'Radio'
      switch(NCode) {
      case WM_NOTIFICATION_RELEASED:printf("1");
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        break;
      }
      break;
    case ID_BUTTON_ZONE: // Notifications sent by 'OK'
      switch(NCode) {
      case WM_NOTIFICATION_RELEASED:
				GUI_EndDialog(pMsg->hWin,r);
        break;
      }
      break;
    case ID_BUTTON_ZONE_EXIT: // Notifications sent by 'EXIT'
      switch(NCode) {
      case WM_NOTIFICATION_RELEASED:
				GUI_EndDialog(pMsg->hWin,r);
        break;
      }
      break;
    }*/
		switch(NCode)
		{
			 case WM_NOTIFICATION_RELEASED:
				 switch(Id)
				 {
					 case GUI_ID_BUTTON5:WM_HideWindow(g_hCFGFrame);setZone(index);draw_Circle();//ok
          break;
					 case GUI_ID_BUTTON6://设置cancel
						 WM_HideWindow(g_hCFGFrame);
						draw_Circle();
						 break;
				 }
				 case WM_NOTIFICATION_VALUE_CHANGED:
					 switch(Id)
					 {
						 case ID_RADIO_ZONE:hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_RADIO0);index=RADIO_GetValue(hItem);
						 printf("得到时区的索引:%d",index);
							 break;
					 }
		break;
    break;
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}
	}
//背景窗口回调函数
static void _cbBkWindow(WM_MESSAGE *pMsg)
{
	int Id,Ncode;
	WM_HWIN hItem;
	switch(pMsg->MsgId) //根据不同的消息类型处理
	{
		case WM_PAINT:   //WM_PAIMNT消息，窗口重绘自身
			dispbmpex("0:/PICTURE/BMP/BMP2.bmp",0,0,0,2,1);//displayjpegex("0:/PICTURE/JPEG/JPG2.jpg",0,0,0,2,1);
		GUI_SetColor(GUI_BLUE);
	 GUI_FillRoundedRect(20,50,360,280,20);	
		GUI_SetColor(GUI_WHITE);
		GUI_DrawRoundedRect(19,51,361,281,20);
		draw_Circle();
		break;
		case WM_NOTIFY_PARENT:
			Id = WM_GetId(pMsg->hWinSrc);
			Ncode=pMsg->Data.v;
			switch(Id)
			{
				case GUI_ID_BUTTON2:
					switch(Ncode)
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED: //按钮被按下并释放
							LED1=~LED1;		//LED1反转
							WM_BringToTop( g_hSDTFrame);//将窗口放在其同属窗口前面。
							FRAMEWIN_SetText(g_hSDTFrame,"Set Date&Time");
							WM_ShowWindow( g_hSDTFrame);
							draw_Circle();
							break;	
					}
					break;
				case GUI_ID_BUTTON4://设置闹钟
							switch(Ncode)
							{
								case WM_NOTIFICATION_CLICKED:break;
								case WM_NOTIFICATION_RELEASED:
									GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);//创建对话框
									draw_Circle();
								break;
							}
							break;
			 case GUI_ID_BUTTON3://BEEP=~BEEP;
					case WM_NOTIFICATION_RELEASED:
				 //GUI_CreateDialogBox(_aDialogCreateZONE, GUI_COUNTOF(_aDialogCreateZONE), _cbDialog_ZONE, WM_HBKWIN, 0, 0);//创建对话框
							WM_BringToTop( g_hCFGFrame);//将窗口放在其同属窗口前面。
							FRAMEWIN_SetText(g_hCFGFrame,"Config");
							WM_ShowWindow( g_hCFGFrame);
							draw_Circle();
					break;
			}
			break;
			
		default:
			WM_DefaultProc(pMsg); //此函数可处理未处理的消息
	}
}

//为矩形加圆角框 nr：内圆角 ，w:边框宽，b:边框颜色，n:内部颜色
void disp_decoration(GUI_RECT rect ,short nr, short w,GUI_COLOR b, GUI_COLOR n)
{
    short dr=nr*0.707;
    dr=nr-dr;
    GUI_SetColor(n);
    GUI_DrawRoundedFrame(rect.x0-dr,rect.y0-dr,rect.x1+dr,rect.y1+dr,nr,dr);
    GUI_SetColor(b);
    dr+=w;
    GUI_DrawRoundedFrame(rect.x0-dr,rect.y0-dr,rect.x1+dr,rect.y1+dr,nr+w,w);
}

//为矩形加圆框 w:边框宽，b:边框颜色，n:内部颜色
void disp_decorationCirc(GUI_RECT rect , short w,GUI_COLOR b, GUI_COLOR n)
{
    short nr=(rect.x1-rect.x0)*1.207/2+0.5;
    short xr=(rect.x1+rect.x0)/2,yr=(rect.y1+rect.y0)/2;
    short pensize = (rect.x1-rect.x0)*0.414/2+4.5;
    GUI_SetColor(n);
    GUI_SetPenSize(pensize);
    GUI_DrawEllipse(xr,yr,nr,nr);
    GUI_SetColor(b);
    nr+=(pensize+w)/2-0.5;
    GUI_SetPenSize(w);
    GUI_DrawEllipse(xr,yr,nr,nr);
    GUI_SetPenSize(1);
}
//画列表
static int _OwnerDraw(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo) {
  WM_HWIN   hWin;
  xWheel_TypeDef   * pWheel;
  int  xSizeWin;
  int  ySizeWin;
  int  LineHeight;
  int  i;
  int  temp;
  hWin = pDrawItemInfo->hWin;
  //
  // Process messages
  //
  switch (pDrawItemInfo->Cmd) {
  case WIDGET_ITEM_GET_XSIZE:
    //
    // Return x-size of item
    //
    return LISTWHEEL_OwnerDraw(pDrawItemInfo);
  case WIDGET_ITEM_GET_YSIZE:
    //
    // Return y-size of item
    //
    return LISTWHEEL_OwnerDraw(pDrawItemInfo);
  case WIDGET_ITEM_DRAW:
    //
    // Draw item
    //
      //标识不存在的日期
      if(pDrawItemInfo->hWin ==_aWheel_date[2].hWin )
      if( pDrawItemInfo->ItemIndex+1 > _currlenDay ) 
      {
          GUI_SetColor(0x008080D0);
          temp = pDrawItemInfo->x1-pDrawItemInfo->x0;
          GUI_DrawHLine((pDrawItemInfo->y0+pDrawItemInfo->y1)/2,pDrawItemInfo->x0+temp/4,pDrawItemInfo->x1-temp/4);
          LISTWHEEL_SetTextColor(pDrawItemInfo->hWin,LISTWHEEL_CI_UNSEL,GUI_WHITE);
      }
     LISTWHEEL_OwnerDraw(pDrawItemInfo);
     LISTWHEEL_SetTextColor(pDrawItemInfo->hWin,LISTWHEEL_CI_UNSEL,GUI_BLACK);
   
    return 0 ;
  case WIDGET_ITEM_DRAW_BACKGROUND://自定义背景
    xSizeWin = WM_GetWindowSizeX(hWin);
    ySizeWin = WM_GetWindowSizeY(hWin);
    //
    // Draw background
    //
    GUI_SetColor(0x00F0F0F0);
    GUI_FillRoundedRect(1, 1, xSizeWin - 1, ySizeWin - 1,2);
    GUI_SetColor(0x00c0c0c0);
    GUI_DrawRoundedRect(2, 2, xSizeWin - 2, ySizeWin - 2,2);
    
    break;
  case WIDGET_ITEM_DRAW_OVERLAY://自定义前景覆盖
    LISTWHEEL_GetUserData(hWin, &pWheel, sizeof(pWheel));

    xSizeWin = WM_GetWindowSizeX(hWin);
    ySizeWin = WM_GetWindowSizeY(hWin);
    LineHeight = pWheel->LineHeight;
  
    // Draw overlay
    //
    GUI_EnableAlpha(1);
    GUI_SetColor(0x00505050);
    for (i=0;i<6;i++)
    {
        GUI_SetAlpha(180-i*8);
        GUI_DrawHLine((ySizeWin-LineHeight)/2+i,2,xSizeWin-2);
        GUI_DrawHLine((ySizeWin+LineHeight)/2-i,2,xSizeWin-2);
    }
    GUI_FillRect(2,(ySizeWin-LineHeight)/2+i,xSizeWin-2,(ySizeWin+LineHeight)/2-i);
    GUI_SetAlpha(0);//这里，画完一定要设置为0
    GUI_EnableAlpha(0);
    
    break;
  }
  return 0;
}
static int CreatListWheel(int x, int y, int xSize, int ySize, int Id, char ** apText, int NumItems, int TextAlign, WM_HWIN hParent, xWheel_TypeDef * pWheel)
{
		WM_HWIN  hWin;
		int i;
		int LineHeight;
		const GUI_FONT GUI_UNI_PTR * pFont;
	pFont         = &GUI_Font6x8 ;
  LineHeight    = 40;
  pWheel->pFont = pFont;
  pWheel->LineHeight=LineHeight;
  hWin  = LISTWHEEL_CreateUser(x, y, xSize, ySize, hParent, WM_CF_SHOW | WM_CF_HASTRANS, 0, Id, NULL, sizeof(void *));
  LISTWHEEL_SetFont(hWin, &GUI_Font8x16_ASCII);
  LISTWHEEL_SetTextAlign(hWin, TextAlign);
  LISTWHEEL_SetSnapPosition(hWin, (ySize - LineHeight) / 2);
  LISTWHEEL_SetOwnerDraw(hWin, _OwnerDraw);
  LISTWHEEL_SetUserData(hWin, &pWheel, sizeof(pWheel));
  LISTWHEEL_SetLineHeight(hWin, LineHeight);
  LISTWHEEL_SetTextColor(hWin, LISTWHEEL_CI_SEL, GUI_LIGHTBLUE);
  for (i = 0; i < NumItems; i++) {
    LISTWHEEL_AddString(hWin, *(apText + i));
  }
  if (TextAlign & GUI_TA_RIGHT) {
    LISTWHEEL_SetRBorder(hWin, 10);
  }
  pWheel->hWin = hWin;
  return 0;

}
//设置时间页面
static void _cbFrame(WM_MESSAGE * pMsg)
{
	int Ncode;
	int Id;
	int Index;
	int yPos;
	char *buffer;
	int temp;
	switch(pMsg->MsgId)
	{
		case WM_NOTIFY_PARENT://告知父窗口情况
			Id=WM_GetId(pMsg->hWinSrc);
			Ncode=pMsg->Data.v;
		switch(Ncode)
		{
			 case WM_NOTIFICATION_RELEASED:
				 switch(Id)
				 {
					 case GUI_ID_BUTTON0:WM_HideWindow(g_hSDTFrame);draw_Circle();
          break;
					 case GUI_ID_BUTTON1://设置时间
						 printf("设置的日期为:%d %d %d\n",_aSelection_date[0],_aSelection_date[1],_aSelection_date[2]);
							printf("设置的时间为:%d %d %d\n",_aSelection_time[0],_aSelection_time[1],_aSelection_time[2]);
						 changetime(_aSelection_date,_aSelection_time);
						 WM_HideWindow(g_hSDTFrame);
						draw_Circle();
						 break;
				 }
		break;
		case WM_NOTIFICATION_MOVED_OUT:
      switch (Id) {
      case GUI_ID_LISTWHEEL0://
						temp = LISTWHEEL_GetPos(_aWheel_date[0].hWin);//返回当前吸入项的索引 （以零为基准）。
						/* LISTWHEEL_SetSel(_aWheel_date[0].hWin,temp);
						LISTWHEEL_GetItemText(_aWheel_date[0].hWin,temp,buffer,5);//获取所选中的*/
						_aSelection_date[0]=year[temp]; //获取数据
						printf("year索引为:%d\n",temp);
						LISTWHEEL_SetSel(_aWheel_date[0].hWin,temp);
      case GUI_ID_LISTWHEEL1: 
							temp = LISTWHEEL_GetPos(_aWheel_date[1].hWin);//返回当前吸入项的索引 （以零为基准）。
						_aSelection_date[1]=Month[temp]; //获取数据
						printf("Month索引为:%d\n",temp);
						LISTWHEEL_SetSel(_aWheel_date[1].hWin,temp);
      case GUI_ID_LISTWHEEL2:
						temp = LISTWHEEL_GetPos(_aWheel_date[2].hWin);//返回当前吸入项的索引 （以零为基准）。
						_aSelection_date[2]=day[temp]; //获取数据
						printf("date索引为:%d\n",temp);//向串口发送数据
						LISTWHEEL_SetSel(_aWheel_date[2].hWin,temp);
        break;
      case GUI_ID_LISTWHEEL3:
				    temp = LISTWHEEL_GetPos(_aWheel_time[0].hWin);//返回当前吸入项的索引 （以零为基准）。
						LISTWHEEL_SetSel(_aWheel_time[0].hWin,temp); 
						_aSelection_time[0]=Hour[temp]; //获取数据
						printf("time0索引为:%d\n",temp);
						LISTWHEEL_SetSel(_aWheel_time[0].hWin,temp);
      case GUI_ID_LISTWHEEL3+1:
						temp = LISTWHEEL_GetPos(_aWheel_time[1].hWin);//返回当前吸入项的索引 （以零为基准）。
						_aSelection_time[1]=Sixty[temp]; //获取数据
						printf("time1索引为:%d\n",temp);
						LISTWHEEL_SetSel(_aWheel_time[1].hWin,temp);
      case GUI_ID_LISTWHEEL3+2:
						temp = LISTWHEEL_GetPos(_aWheel_time[2].hWin);//返回当前吸入项的索引 （以零为基准）。
						_aSelection_time[2]=Sixty[temp]; //获取数据
						printf("time2索引为:%d\n",temp);
						LISTWHEEL_SetSel(_aWheel_time[2].hWin,temp);
          //WM_InvalidateWindow(pMsg->hWin);
          break;
      }
		}
    break;
	}
}

void numdisplay_demo(void)//主要初始函数
{
	 WM_HWIN hWin1,hWin2,hWin3,hItem;  
		WM_HWIN hSDTFrameClient,hCFGFrameClient;
   u8 c,d;
   u16 temp;
   u32 tm;
	 WM_CALLBACK *_cbOldBK;
	 int i;
		RADIO_SKINFLEX_PROPS radioProps;
	GUI_CURSOR_Show();    //可用游标 
	WM_SetCreateFlags(WM_CF_MEMDEV); //自动在所有窗口上使用存储设备
	d=1; c=1;
	//GUI_SetColor(GUI_WHITE); //设置前景色
	//显示时钟
	 //WM_GetInsideRect(&Rect);
	 
	 //主界面三个按钮
	 BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);
	_ahButton[0] = BUTTON_CreateEx((LCD_WX-150)/2, LCD_HX-62, 150, 50, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_BUTTON2);
	 BUTTON_SetText(_ahButton[0],"Set Date&Time");
	_ahButton[1] = BUTTON_CreateEx((LCD_WX/3-150)/2, LCD_HX-62, 150, 50, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_BUTTON3);
	 BUTTON_SetText(_ahButton[1],"Configure");
	_ahButton[2] = BUTTON_CreateEx((LCD_WX/3-150)/2+LCD_WX/3*2, LCD_HX-62, 150, 50, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_BUTTON4);
	BUTTON_SetText(_ahButton[2],"Set Alarm");
	for(i=0;i<7;i++)//设置字体
	 BUTTON_SetFont(_ahButton[i],GUI_FONT_24_ASCII);

 	//为背景窗口设置回调函数
		_cbOldBK = WM_SetCallback(WM_HBKWIN,_cbBkWindow);
	//设置时间窗口界面
	FRAMEWIN_SetDefaultSkinClassic();//FRAMEWIN_SetDefaultSkin(FRAMEWIN_SKIN_FLEX);//设置皮肤
	g_hSDTFrame = FRAMEWIN_CreateEx(20, 20,FRWINWX, FRWINHX, WM_HBKWIN,  WM_CF_HIDE,0, 1,"Set Date&Time",NULL);//默认不显示
	FRAMEWIN_SetMoveable(g_hSDTFrame, 1);//设置窗口可移动
	hSDTFrameClient = WM_GetClientWindow(g_hSDTFrame);//工作区句柄
	WM_SetCallback(hSDTFrameClient,_cbFrame);
	//取消按钮
	_ahButton[3]=BUTTON_CreateEx(FRWINWX/4*1, FRWINHX-45-45, 80, 45, hSDTFrameClient, WM_CF_SHOW, 0, GUI_ID_BUTTON0);
	BUTTON_SetText(_ahButton[3], "Cancel");
	//OK按钮
	_ahButton[4] = BUTTON_CreateEx(FRWINWX/3*2, FRWINHX-45-45, 80, 45, hSDTFrameClient, WM_CF_SHOW, 0, GUI_ID_BUTTON1);
  BUTTON_SetText(_ahButton[4], "OK");
	_currlenDay = ADJDay(Calendar.year,Calendar.month);//初始化每月有多少天
	//list
	CreatListWheel( 10,40,  WHEELWX,   WHEELHX, GUI_ID_LISTWHEEL0, _apYear,   GUI_COUNTOF(_apYear), GUI_TA_VCENTER | GUI_TA_HCENTER, hSDTFrameClient, &_aWheel_date[0]);
  CreatListWheel(10+WHEELWX,   40,  WHEELWX*2, WHEELHX, GUI_ID_LISTWHEEL1, _apMonth, GUI_COUNTOF(_apMonth), GUI_TA_VCENTER | GUI_TA_RIGHT,   hSDTFrameClient, &_aWheel_date[1]);
  CreatListWheel(10+WHEELWX*3, 40,  WHEELWX,   WHEELHX, GUI_ID_LISTWHEEL2, _apDay,    GUI_COUNTOF(_apDay),  GUI_TA_VCENTER | GUI_TA_HCENTER, hSDTFrameClient, &_aWheel_date[2]);
	CreatListWheel(10+WHEELWX*4+8, 40 , WHEELWX, WHEELHX, GUI_ID_LISTWHEEL3,     _apHour, GUI_COUNTOF(_apHour),    GUI_TA_VCENTER | GUI_TA_HCENTER, hSDTFrameClient, &_aWheel_time[0]);
  CreatListWheel(10+WHEELWX*5+8, 40 , WHEELWX, WHEELHX, GUI_ID_LISTWHEEL3+1, _apMinute, GUI_COUNTOF(_apMinute),  GUI_TA_VCENTER | GUI_TA_HCENTER, hSDTFrameClient, &_aWheel_time[1]);
  CreatListWheel(10+WHEELWX*6+8, 40 , WHEELWX, WHEELHX, GUI_ID_LISTWHEEL3+2, _apSecond, GUI_COUNTOF(_apSecond),  GUI_TA_VCENTER | GUI_TA_HCENTER, hSDTFrameClient, &_aWheel_time[2]);
	for (i = 0; i < 3; i++) {//将 LISTWHEEL 移到指定位置。
        LISTWHEEL_MoveToPos(_aWheel_date[i].hWin, 0);
        LISTWHEEL_MoveToPos(_aWheel_time[i].hWin, 0);
    }
		//设置世界时间
		//FRAMEWIN_SetDefaultSkinClassic();
		g_hCFGFrame=FRAMEWIN_CreateEx(70, 70,FRWINWX, 400, WM_HBKWIN,  WM_CF_HIDE,0, 1,"Configure",NULL);//默认不显示
		 FRAMEWIN_AddMaxButton(g_hCFGFrame,FRAMEWIN_BUTTON_RIGHT,2);//创建关闭按钮
		FRAMEWIN_SetMoveable(g_hCFGFrame, 1);//设置窗口可移动
		hCFGFrameClient = WM_GetClientWindow(g_hCFGFrame);//工作区句柄
		WM_SetCallback(hCFGFrameClient,_cbFrame_ZONE);
		//OK按钮
		_ahButton[5] = BUTTON_CreateEx(178, 309, 128, 50, hCFGFrameClient, WM_CF_SHOW, 0, GUI_ID_BUTTON5);
		BUTTON_SetText(_ahButton[5], "OK");
		//取消按钮
	  _ahButton[6]=BUTTON_CreateEx(450, 309, 128, 50, hCFGFrameClient, WM_CF_SHOW, 0, GUI_ID_BUTTON6);
	  BUTTON_SetText(_ahButton[6], "Cancel");
		//选项组
		
		 hWin1=RADIO_CreateEx(227, 77, 207, 279,hCFGFrameClient,WM_CF_SHOW,GUI_ID_RADIO0 ,1234,5,50);
     RADIO_GetSkinFlexProps(&radioProps, RADIO_SKINFLEX_PI_UNPRESSED);
     radioProps.ButtonSize = 24;
     RADIO_SetSkinFlexProps(&radioProps, RADIO_SKINFLEX_PI_UNPRESSED);
     RADIO_GetSkinFlexProps(&radioProps, RADIO_SKINFLEX_PI_PRESSED);
     radioProps.ButtonSize = 24;
     RADIO_SetSkinFlexProps(&radioProps, RADIO_SKINFLEX_PI_PRESSED);
			//RADIO_SetFont(hWin1,GUI_FONT_24_ASCII);
			RADIO_SetText( hWin1, "Beijing", 0);
			RADIO_SetText( hWin1, "Washington", 1);//-12
			RADIO_SetText( hWin1, "Tokyo", 2);//1
			RADIO_SetText( hWin1, "London", 3);//-7
			RADIO_SetText( hWin1, "Canberra ", 4);//6
			RADIO_SetText( hWin1, "Cairo", 5);//-6
			RADIO_SetFont( hWin1, GUI_FONT_32B_ASCII);
	num=0;
  while (d==1)
  {	
  	if(curs==0){            				    //正常时钟显示模式
  	Clock1.year=Calendar.year;
	  Clock1.month=Calendar.month;
	  Clock1.date=Calendar.date;
	  Clock1.hour=Calendar.hour;
	  Clock1.min=Calendar.min;
	  Clock1.sec=Calendar.sec;
	}											
		 WM_ExecIdle(); 									     //刷新屏幕
		clock_time_dis(30,70);							 //显示时间
		clock_date_dis(30,150);							 //显示日期
		clock_Week_dis(80,225);							 //显示星期
		draw_hourmark();
		draw_Secmark();
		draw_minmark();
		
  }
}

 
/****************************************************************************
* 名    称：void clock_time_dis(unsigned int a, unsigned int b)
* 功    能：时间显示程序
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：
****************************************************************************/
void clock_time_dis(unsigned int a, unsigned int b){

	
	GUI_SetFont(&GUI_FontD32);  
 	if(curs==4){										  //时
		if(flash==1||num!=0){
			GUI_SetColor(GUI_BLUE);
			GUI_DispDecAt(Clock1.hour, a, b,2);	
		}
		else if(flash==0) {
			GUI_DispStringAt("  ", a, b);
		}  		
	}
	else{
		GUI_SetColor(GUI_WHITE);
		GUI_DispDecAt(Clock1.hour, a, b,2);
	}

	GUI_SetColor(GUI_WHITE);
	GUI_DispCharAt(':', a+60, b);

	if(curs==5){										  //分
		if(flash==1||num!=0){
			GUI_SetColor(GUI_BLUE);
			GUI_DispDecAt(Clock1.min, a+95, b,2);	
		}
		else if(flash==0) {
			GUI_DispStringAt("  ", a+95, b);
		}  		
	}
	else{
		GUI_SetColor(GUI_WHITE);
		GUI_DispDecAt(Clock1.min, a+95, b,2);
	}
		
    GUI_SetColor(GUI_WHITE);
	GUI_DispCharAt(':', a+155, b);						 //秒
	if(curs==6){
		if(flash==1||num!=0){
			GUI_SetColor(GUI_BLUE);
			GUI_DispDecAt(Clock1.sec, a+190, b,2);	
		}
		else if(flash==0) {
			GUI_DispStringAt("  ", a+190, b);
		}  		
	}
	else{
		GUI_SetColor(GUI_WHITE);
		GUI_DispDecAt(Clock1.sec, a+190, b,2);
	}
}


/****************************************************************************
* 名    称：void clock_date_dis(unsigned int a, unsigned int b)
* 功    能：日期显示程序
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：
****************************************************************************/
void clock_date_dis(unsigned int a, unsigned int b){

	
	GUI_SetFont(&GUI_FontD32);  //GUI_SetFont(&GUI_Font8x8);//
	//GUI_SetColor(GUI_WHITE);
	GUI_SetBkColor(GUI_BLUE);
 	if(curs==1){											//年
		if(flash==1||num!=0){
			GUI_SetColor(GUI_BLUE);
			GUI_DispDecAt(Clock1.year, a, b,4);	
		}
		else if(flash==0) {
			GUI_DispStringAt("    ", a, b);
		}  		
	}
	else{
		GUI_SetColor(GUI_WHITE);
		GUI_DispDecAt(Clock1.year, a, b,4);
	}

	GUI_SetColor(GUI_WHITE);
	GUI_DispCharAt('-', a+110, b);

	if(curs==2){										   //月
		if(flash==1||num!=0){
			GUI_SetColor(GUI_BLUE);
			GUI_DispDecAt(Clock1.month, a+145, b,2);	
		}
		else if(flash==0) {
			GUI_DispStringAt("  ", a+145, b);
		}  		
	}
	else{
		GUI_SetColor(GUI_WHITE);
		GUI_DispDecAt(Clock1.month, a+145, b,2);
	}	
	GUI_SetColor(GUI_WHITE);
	GUI_DispCharAt('-', a+205, b);
	if(curs==3){										   //日
		if(flash==1||num!=0){
			GUI_SetColor(GUI_BLUE);
			GUI_DispDecAt(Clock1.date, a+240, b,2);	
		}
		else if(flash==0) {
			GUI_DispStringAt("   ", a+240, b);
		}  		
	}
	else{
		GUI_SetColor(GUI_WHITE);
		GUI_DispDecAt(Clock1.date, a+240, b,2);
	}
}

/****************************************************************************
* 名    称：void clock_Week_dis(unsigned int a, unsigned int b)
* 功    能：星期显示程序
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：
****************************************************************************/
//void clock_Week_dis(unsigned int a, unsigned int b){

//	GUI_SetColor(GUI_WHITE);
//	GUI_SetFont(GUI_FONT_32B_1);  
// 	if(Clock1.week==1) GUI_DispStringAt("Monday", a, b);
//	else if(Clock1.week==2) GUI_DispStringAt("Tuesday", a, b);
//	else if(Clock1.week==3) GUI_DispStringAt("Wednesday", a, b);
//	else if(Clock1.week==4) GUI_DispStringAt("Thursday", a, b);
//	else if(Clock1.week==5) GUI_DispStringAt("Friday", a, b);
//	else if(Clock1.week==6) GUI_DispStringAt("Saturday", a, b);
//	else if(Clock1.week==7) GUI_DispStringAt("Sunday", a, b);
//	//GUI_DispDecAt(Clock.Week, a, b,1);

//} 
