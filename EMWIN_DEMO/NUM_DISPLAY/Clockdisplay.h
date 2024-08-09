#ifndef _NUMDISPLAY_H
#define _NUMDISPLAY_H
#include "DIALOG.h"
#include "GUI.h"
#include "WM.h"
#include "sys.h"
#include "clock.h"
#include "includes.h"

#define GUI_KEY_F1 (GUI_ID_USER + 0x01)  //key 的键值 在 调用GUI_GetKey() 返回的就这键值
#define GUI_KEY_F2 (GUI_ID_USER + 0x02)
#define GUI_KEY_F3 (GUI_ID_USER + 0x03)
#define GUI_KEY_F4 (GUI_ID_USER + 0x04) 
#define GUI_KEY_F5 (GUI_ID_USER + 0x05)
#define ID_FRAMEWIN_0 (GUI_ID_USER + 0x00)

#define ID_TEXT_0 (GUI_ID_USER + 0x06)

void numdisplay_demo(void);
//void clock_Week_dis(unsigned int a, unsigned int b);

extern volatile unsigned int curs,page,key,num,flash;

#endif


