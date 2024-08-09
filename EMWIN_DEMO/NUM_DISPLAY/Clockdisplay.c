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
//����ʱ��
#define ID_FRAMEWIN_ZONE       (GUI_ID_USER + 0x12)
#define ID_IMAGE_ZONE       (GUI_ID_USER + 0x13)
#define ID_RADIO_ZONE      (GUI_ID_USER + 0x14)
#define ID_TEXT_ZONE      (GUI_ID_USER + 0x15)
#define ID_BUTTON_ZONE      (GUI_ID_USER + 0x16)
#define ID_BUTTON_ZONE_EXIT      (GUI_ID_USER + 0x17)
//
BUTTON_Handle  _ahButton[8];
WM_HWIN  g_hText_time,g_hText_data,g_hText_week;                //��ʾ����ʱ�����ڵ������ı�����
WM_HWIN  g_hSDTFrame = 0 ,g_hCFGFrame=0,g_hdialog_alarm=0;      //ʱ�����ã�ϵͳ���ã�����Ի�����
void clock_time_dis(unsigned int a, unsigned int b);
void clock_date_dis(unsigned int a, unsigned int b);
void clock_Week_dis(unsigned int a, unsigned int b);
typedef struct {
    WM_HWIN   hWin;
    int       LineHeight;
    const GUI_FONT GUI_UNI_PTR* pFont;
} xWheel_TypeDef ;   //����listwheel���Զ���滭
static char _currlenDay=0;  //��ǰ�µ�����
static xWheel_TypeDef _aWheel_date[3];
static xWheel_TypeDef _aWheel_time[3];
#define _apSecond   _apMinute//��60���Ӷ�������
//�������ڵ�ȫ�ֱ���
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
};//��wheelList��ѡ��ʱ��
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
static int now_zone=0;//����
static int another[]={0,-12,6,2,-6};
//��������ʱ��

//volatile u8 flash;
//��Բ��
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
//����
void draw_Secmark()
{
	int length=90;
	int second=Calendar.sec;
	double pie=3.14;
	GUI_SetPenSize(4);
	GUI_SetColor(GUI_WHITE);//����ָ�����ɫΪ��ɫ
	GUI_DrawLine(centerx,centery,centerx+length*cos((90-((second-1)*6))/180.0*pie),centery-length*sin((90-((second-1)*6))/180.0*pie));
	GUI_SetColor(GUI_RED);//����ָ�����ɫΪ��ɫ
	GUI_DrawLine(centerx,centery,centerx+length*cos((90-(second*6))/180.0*pie),centery-length*sin((90-(second*6))/180.0*pie));
}
//����
void draw_minmark()
{
	int length=75;
	int second=Calendar.sec,minute=Calendar.min;
	double pie=3.14;
	GUI_SetPenSize(4);	
	GUI_SetColor(GUI_WHITE);//����ָ�����ɫΪ��ɫ
	GUI_DrawLine(centerx,centery,centerx+length*cos((90-((second+minute*60-1)/60.0)*6)/180.0*pie),centery-length*sin((90-((second+minute*60-1)/60.0)*6)/180.0*pie));
	GUI_SetColor(GUI_BLACK);//����ָ�����ɫΪ��ɫ
	GUI_DrawLine(centerx,centery,centerx+length*cos((90-((second+minute*60)/60.0)*6)/180.0*pie),centery-length*sin((90-((second+minute*60)/60.0)*6)/180.0*pie));
}
//ʱ��
void draw_hourmark()
{
	int hourarmlength=55;
	int hour=Calendar.hour;
	int minute=Calendar.min;
	double pie=3.14;
	GUI_SetPenSize(4);
	GUI_SetColor(GUI_WHITE);//����ָ�����ɫΪ��ɫ
	GUI_DrawLine(centerx,centery,centerx+hourarmlength*cos((90-((minute+hour*60-1)/60.0)*30)/180.0*pie),centery-hourarmlength*sin((90-((hour*60+minute-1)/60.0)*30)/180.0*pie));
	GUI_SetColor(GUI_BLACK);//����ָ�����ɫΪ��ɫ
	GUI_DrawLine(centerx,centery,centerx+hourarmlength*cos((90-((minute+hour*60)/60.0)*30)/180.0*pie),centery-hourarmlength*sin((90-((hour*60+minute)/60.0)*30)/180.0*pie));
}
//�ı�ʱ��
void changetime(const int *pSel,const int *pSel1)
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
		RTC_Set((u16)pSel[0],(u8)pSel[1],(u8)pSel[2],(u8)pSel1[0],(u8)pSel1[1],(u8)pSel1[2]);	
		RTC_ExitConfigMode(); //�˳�����ģʽ  
		BKP_WriteBackupRegister(BKP_DR1, 0X5253);	//��ָ���ĺ󱸼Ĵ�����д���û���������
}
//��������
void set_alarmF(const int *tim)
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
		RTC_Alarm_Seta(Calendar.year,Calendar.month,Calendar.date,tim[0],tim[1],0);
		RTC_ExitConfigMode(); //�˳�����ģʽ  
		BKP_WriteBackupRegister(BKP_DR1, 0X5253);	//��ָ���ĺ󱸼Ĵ�����д���û���������
}
//�������¼�������
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
//��������
static void _cbDialog(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;
	int i,r,pos; 
	char am[8];
	SCROLLBAR_SetDefaultSkin(SCROLLBAR_SKIN_FLEX);
	SCROLLBAR_SetDefaultWidth(15);//���ÿ��
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
		//�����������ӵ������ı��ؼ�
		SCROLLBAR_CreateAttached(hItem,SCROLLBAR_CF_VERTICAL);
    // Initialization of 'Listbox'
    hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX_1);
		for(i=0;i<60;i++)
			LISTBOX_AddString(hItem, _apMinute[i]);
    LISTBOX_SetFont(hItem, GUI_FONT_24B_ASCII);
		//�����������ӵ������ı��ؼ�
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
    case ID_LISTBOX_0: // ʱ
			hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX_0);
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
						pos=LISTBOX_GetSel(hItem);//�õ�����
						printf("\r\n����Сʱ������Ϊ:%d\r\n",pos);
						 pos_alarm[0]=pos;
						_alarmtime[0]=Hour[pos];
						printf("\r\n����Сʱ��ʱ��Ϊ:%d\r\n",Hour[pos]);
						alarm_set[0]=_apHour[pos];
        break;
      }
      break;
    case ID_LISTBOX_1: // ��
			hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX_1);
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
						pos=LISTBOX_GetSel(hItem);//�õ�����
						printf("\r\n���ӷ��ӵ�����Ϊ:%d\r\n",pos);
						 pos_alarm[1]=pos;
						_alarmtime[1]=Sixty[pos];
						printf("\r\n���ӷ��ӵ�ʱ��Ϊ:%d\r\n",Sixty[pos]);
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
        hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX_alarm);//ָ������ʱ���ı���
				strcpy(am,alarm_set[0]);
				strcat(am,":");
				strcat(am,alarm_set[1]);
				printf("   \r\n���ʱ��ɹ�\r\n  %s",am);
				LISTBOX_AddString(hItem,am);
				printf("   \r\n���ʱ��ɹ�\r\n  %s",am);
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
//�����뱱��ʱ��Ĳ�ֵ	  
//ƽ����·����ڱ�
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
		//
		//RTC_SetCounter(seccount);	//����RTC��������ֵ
		RTC_ExitConfigMode(); //�˳�����ģʽ  
		BKP_WriteBackupRegister(BKP_DR1, 0X5253);	//��ָ���ĺ󱸼Ĵ�����д���û���������
}
//����ʱ��
static void _cbFrame_ZONE(WM_MESSAGE * pMsg) {
  const void * pData;
  WM_HWIN      hItem;
  U32          FileSize;
  int          NCode;
  int          Id,r,index;
  switch (pMsg->MsgId) {
		case WM_PAINT:   //WM_PAIMNT��Ϣ�������ػ�����
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
					 case GUI_ID_BUTTON6://����cancel
						 WM_HideWindow(g_hCFGFrame);
						draw_Circle();
						 break;
				 }
				 case WM_NOTIFICATION_VALUE_CHANGED:
					 switch(Id)
					 {
						 case ID_RADIO_ZONE:hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_RADIO0);index=RADIO_GetValue(hItem);
						 printf("�õ�ʱ��������:%d",index);
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
//�������ڻص�����
static void _cbBkWindow(WM_MESSAGE *pMsg)
{
	int Id,Ncode;
	WM_HWIN hItem;
	switch(pMsg->MsgId) //���ݲ�ͬ����Ϣ���ʹ���
	{
		case WM_PAINT:   //WM_PAIMNT��Ϣ�������ػ�����
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
						case WM_NOTIFICATION_RELEASED: //��ť�����²��ͷ�
							LED1=~LED1;		//LED1��ת
							WM_BringToTop( g_hSDTFrame);//�����ڷ�����ͬ������ǰ�档
							FRAMEWIN_SetText(g_hSDTFrame,"Set Date&Time");
							WM_ShowWindow( g_hSDTFrame);
							draw_Circle();
							break;	
					}
					break;
				case GUI_ID_BUTTON4://��������
							switch(Ncode)
							{
								case WM_NOTIFICATION_CLICKED:break;
								case WM_NOTIFICATION_RELEASED:
									GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);//�����Ի���
									draw_Circle();
								break;
							}
							break;
			 case GUI_ID_BUTTON3://BEEP=~BEEP;
					case WM_NOTIFICATION_RELEASED:
				 //GUI_CreateDialogBox(_aDialogCreateZONE, GUI_COUNTOF(_aDialogCreateZONE), _cbDialog_ZONE, WM_HBKWIN, 0, 0);//�����Ի���
							WM_BringToTop( g_hCFGFrame);//�����ڷ�����ͬ������ǰ�档
							FRAMEWIN_SetText(g_hCFGFrame,"Config");
							WM_ShowWindow( g_hCFGFrame);
							draw_Circle();
					break;
			}
			break;
			
		default:
			WM_DefaultProc(pMsg); //�˺����ɴ���δ�������Ϣ
	}
}

//Ϊ���μ�Բ�ǿ� nr����Բ�� ��w:�߿��b:�߿���ɫ��n:�ڲ���ɫ
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

//Ϊ���μ�Բ�� w:�߿��b:�߿���ɫ��n:�ڲ���ɫ
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
//���б�
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
      //��ʶ�����ڵ�����
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
  case WIDGET_ITEM_DRAW_BACKGROUND://�Զ��屳��
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
  case WIDGET_ITEM_DRAW_OVERLAY://�Զ���ǰ������
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
    GUI_SetAlpha(0);//�������һ��Ҫ����Ϊ0
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
//����ʱ��ҳ��
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
		case WM_NOTIFY_PARENT://��֪���������
			Id=WM_GetId(pMsg->hWinSrc);
			Ncode=pMsg->Data.v;
		switch(Ncode)
		{
			 case WM_NOTIFICATION_RELEASED:
				 switch(Id)
				 {
					 case GUI_ID_BUTTON0:WM_HideWindow(g_hSDTFrame);draw_Circle();
          break;
					 case GUI_ID_BUTTON1://����ʱ��
						 printf("���õ�����Ϊ:%d %d %d\n",_aSelection_date[0],_aSelection_date[1],_aSelection_date[2]);
							printf("���õ�ʱ��Ϊ:%d %d %d\n",_aSelection_time[0],_aSelection_time[1],_aSelection_time[2]);
						 changetime(_aSelection_date,_aSelection_time);
						 WM_HideWindow(g_hSDTFrame);
						draw_Circle();
						 break;
				 }
		break;
		case WM_NOTIFICATION_MOVED_OUT:
      switch (Id) {
      case GUI_ID_LISTWHEEL0://
						temp = LISTWHEEL_GetPos(_aWheel_date[0].hWin);//���ص�ǰ����������� ������Ϊ��׼����
						/* LISTWHEEL_SetSel(_aWheel_date[0].hWin,temp);
						LISTWHEEL_GetItemText(_aWheel_date[0].hWin,temp,buffer,5);//��ȡ��ѡ�е�*/
						_aSelection_date[0]=year[temp]; //��ȡ����
						printf("year����Ϊ:%d\n",temp);
						LISTWHEEL_SetSel(_aWheel_date[0].hWin,temp);
      case GUI_ID_LISTWHEEL1: 
							temp = LISTWHEEL_GetPos(_aWheel_date[1].hWin);//���ص�ǰ����������� ������Ϊ��׼����
						_aSelection_date[1]=Month[temp]; //��ȡ����
						printf("Month����Ϊ:%d\n",temp);
						LISTWHEEL_SetSel(_aWheel_date[1].hWin,temp);
      case GUI_ID_LISTWHEEL2:
						temp = LISTWHEEL_GetPos(_aWheel_date[2].hWin);//���ص�ǰ����������� ������Ϊ��׼����
						_aSelection_date[2]=day[temp]; //��ȡ����
						printf("date����Ϊ:%d\n",temp);//�򴮿ڷ�������
						LISTWHEEL_SetSel(_aWheel_date[2].hWin,temp);
        break;
      case GUI_ID_LISTWHEEL3:
				    temp = LISTWHEEL_GetPos(_aWheel_time[0].hWin);//���ص�ǰ����������� ������Ϊ��׼����
						LISTWHEEL_SetSel(_aWheel_time[0].hWin,temp); 
						_aSelection_time[0]=Hour[temp]; //��ȡ����
						printf("time0����Ϊ:%d\n",temp);
						LISTWHEEL_SetSel(_aWheel_time[0].hWin,temp);
      case GUI_ID_LISTWHEEL3+1:
						temp = LISTWHEEL_GetPos(_aWheel_time[1].hWin);//���ص�ǰ����������� ������Ϊ��׼����
						_aSelection_time[1]=Sixty[temp]; //��ȡ����
						printf("time1����Ϊ:%d\n",temp);
						LISTWHEEL_SetSel(_aWheel_time[1].hWin,temp);
      case GUI_ID_LISTWHEEL3+2:
						temp = LISTWHEEL_GetPos(_aWheel_time[2].hWin);//���ص�ǰ����������� ������Ϊ��׼����
						_aSelection_time[2]=Sixty[temp]; //��ȡ����
						printf("time2����Ϊ:%d\n",temp);
						LISTWHEEL_SetSel(_aWheel_time[2].hWin,temp);
          //WM_InvalidateWindow(pMsg->hWin);
          break;
      }
		}
    break;
	}
}

void numdisplay_demo(void)//��Ҫ��ʼ����
{
	 WM_HWIN hWin1,hWin2,hWin3,hItem;  
		WM_HWIN hSDTFrameClient,hCFGFrameClient;
   u8 c,d;
   u16 temp;
   u32 tm;
	 WM_CALLBACK *_cbOldBK;
	 int i;
		RADIO_SKINFLEX_PROPS radioProps;
	GUI_CURSOR_Show();    //�����α� 
	WM_SetCreateFlags(WM_CF_MEMDEV); //�Զ������д�����ʹ�ô洢�豸
	d=1; c=1;
	//GUI_SetColor(GUI_WHITE); //����ǰ��ɫ
	//��ʾʱ��
	 //WM_GetInsideRect(&Rect);
	 
	 //������������ť
	 BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);
	_ahButton[0] = BUTTON_CreateEx((LCD_WX-150)/2, LCD_HX-62, 150, 50, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_BUTTON2);
	 BUTTON_SetText(_ahButton[0],"Set Date&Time");
	_ahButton[1] = BUTTON_CreateEx((LCD_WX/3-150)/2, LCD_HX-62, 150, 50, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_BUTTON3);
	 BUTTON_SetText(_ahButton[1],"Configure");
	_ahButton[2] = BUTTON_CreateEx((LCD_WX/3-150)/2+LCD_WX/3*2, LCD_HX-62, 150, 50, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_BUTTON4);
	BUTTON_SetText(_ahButton[2],"Set Alarm");
	for(i=0;i<7;i++)//��������
	 BUTTON_SetFont(_ahButton[i],GUI_FONT_24_ASCII);

 	//Ϊ�����������ûص�����
		_cbOldBK = WM_SetCallback(WM_HBKWIN,_cbBkWindow);
	//����ʱ�䴰�ڽ���
	FRAMEWIN_SetDefaultSkinClassic();//FRAMEWIN_SetDefaultSkin(FRAMEWIN_SKIN_FLEX);//����Ƥ��
	g_hSDTFrame = FRAMEWIN_CreateEx(20, 20,FRWINWX, FRWINHX, WM_HBKWIN,  WM_CF_HIDE,0, 1,"Set Date&Time",NULL);//Ĭ�ϲ���ʾ
	FRAMEWIN_SetMoveable(g_hSDTFrame, 1);//���ô��ڿ��ƶ�
	hSDTFrameClient = WM_GetClientWindow(g_hSDTFrame);//���������
	WM_SetCallback(hSDTFrameClient,_cbFrame);
	//ȡ����ť
	_ahButton[3]=BUTTON_CreateEx(FRWINWX/4*1, FRWINHX-45-45, 80, 45, hSDTFrameClient, WM_CF_SHOW, 0, GUI_ID_BUTTON0);
	BUTTON_SetText(_ahButton[3], "Cancel");
	//OK��ť
	_ahButton[4] = BUTTON_CreateEx(FRWINWX/3*2, FRWINHX-45-45, 80, 45, hSDTFrameClient, WM_CF_SHOW, 0, GUI_ID_BUTTON1);
  BUTTON_SetText(_ahButton[4], "OK");
	_currlenDay = ADJDay(Calendar.year,Calendar.month);//��ʼ��ÿ���ж�����
	//list
	CreatListWheel( 10,40,  WHEELWX,   WHEELHX, GUI_ID_LISTWHEEL0, _apYear,   GUI_COUNTOF(_apYear), GUI_TA_VCENTER | GUI_TA_HCENTER, hSDTFrameClient, &_aWheel_date[0]);
  CreatListWheel(10+WHEELWX,   40,  WHEELWX*2, WHEELHX, GUI_ID_LISTWHEEL1, _apMonth, GUI_COUNTOF(_apMonth), GUI_TA_VCENTER | GUI_TA_RIGHT,   hSDTFrameClient, &_aWheel_date[1]);
  CreatListWheel(10+WHEELWX*3, 40,  WHEELWX,   WHEELHX, GUI_ID_LISTWHEEL2, _apDay,    GUI_COUNTOF(_apDay),  GUI_TA_VCENTER | GUI_TA_HCENTER, hSDTFrameClient, &_aWheel_date[2]);
	CreatListWheel(10+WHEELWX*4+8, 40 , WHEELWX, WHEELHX, GUI_ID_LISTWHEEL3,     _apHour, GUI_COUNTOF(_apHour),    GUI_TA_VCENTER | GUI_TA_HCENTER, hSDTFrameClient, &_aWheel_time[0]);
  CreatListWheel(10+WHEELWX*5+8, 40 , WHEELWX, WHEELHX, GUI_ID_LISTWHEEL3+1, _apMinute, GUI_COUNTOF(_apMinute),  GUI_TA_VCENTER | GUI_TA_HCENTER, hSDTFrameClient, &_aWheel_time[1]);
  CreatListWheel(10+WHEELWX*6+8, 40 , WHEELWX, WHEELHX, GUI_ID_LISTWHEEL3+2, _apSecond, GUI_COUNTOF(_apSecond),  GUI_TA_VCENTER | GUI_TA_HCENTER, hSDTFrameClient, &_aWheel_time[2]);
	for (i = 0; i < 3; i++) {//�� LISTWHEEL �Ƶ�ָ��λ�á�
        LISTWHEEL_MoveToPos(_aWheel_date[i].hWin, 0);
        LISTWHEEL_MoveToPos(_aWheel_time[i].hWin, 0);
    }
		//��������ʱ��
		//FRAMEWIN_SetDefaultSkinClassic();
		g_hCFGFrame=FRAMEWIN_CreateEx(70, 70,FRWINWX, 400, WM_HBKWIN,  WM_CF_HIDE,0, 1,"Configure",NULL);//Ĭ�ϲ���ʾ
		 FRAMEWIN_AddMaxButton(g_hCFGFrame,FRAMEWIN_BUTTON_RIGHT,2);//�����رհ�ť
		FRAMEWIN_SetMoveable(g_hCFGFrame, 1);//���ô��ڿ��ƶ�
		hCFGFrameClient = WM_GetClientWindow(g_hCFGFrame);//���������
		WM_SetCallback(hCFGFrameClient,_cbFrame_ZONE);
		//OK��ť
		_ahButton[5] = BUTTON_CreateEx(178, 309, 128, 50, hCFGFrameClient, WM_CF_SHOW, 0, GUI_ID_BUTTON5);
		BUTTON_SetText(_ahButton[5], "OK");
		//ȡ����ť
	  _ahButton[6]=BUTTON_CreateEx(450, 309, 128, 50, hCFGFrameClient, WM_CF_SHOW, 0, GUI_ID_BUTTON6);
	  BUTTON_SetText(_ahButton[6], "Cancel");
		//ѡ����
		
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
  	if(curs==0){            				    //����ʱ����ʾģʽ
  	Clock1.year=Calendar.year;
	  Clock1.month=Calendar.month;
	  Clock1.date=Calendar.date;
	  Clock1.hour=Calendar.hour;
	  Clock1.min=Calendar.min;
	  Clock1.sec=Calendar.sec;
	}											
		 WM_ExecIdle(); 									     //ˢ����Ļ
		clock_time_dis(30,70);							 //��ʾʱ��
		clock_date_dis(30,150);							 //��ʾ����
		clock_Week_dis(80,225);							 //��ʾ����
		draw_hourmark();
		draw_Secmark();
		draw_minmark();
		
  }
}

 
/****************************************************************************
* ��    �ƣ�void clock_time_dis(unsigned int a, unsigned int b)
* ��    �ܣ�ʱ����ʾ����
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/
void clock_time_dis(unsigned int a, unsigned int b){

	
	GUI_SetFont(&GUI_FontD32);  
 	if(curs==4){										  //ʱ
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

	if(curs==5){										  //��
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
	GUI_DispCharAt(':', a+155, b);						 //��
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
* ��    �ƣ�void clock_date_dis(unsigned int a, unsigned int b)
* ��    �ܣ�������ʾ����
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/
void clock_date_dis(unsigned int a, unsigned int b){

	
	GUI_SetFont(&GUI_FontD32);  //GUI_SetFont(&GUI_Font8x8);//
	//GUI_SetColor(GUI_WHITE);
	GUI_SetBkColor(GUI_BLUE);
 	if(curs==1){											//��
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

	if(curs==2){										   //��
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
	if(curs==3){										   //��
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
* ��    �ƣ�void clock_Week_dis(unsigned int a, unsigned int b)
* ��    �ܣ�������ʾ����
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
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
