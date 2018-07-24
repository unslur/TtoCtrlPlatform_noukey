#ifndef _DEFINITION_H_
#define _DEFINITION_H_

#include <string>
#include <process.h>

using namespace std;


#define DB_IP "127.0.0.1"
#define DB_NAME "TtoDB"
#define DB_USER "root"
#define DB_PASS "123123"

#define MY_CTLCOLOR_MSGBOX         0
#define MY_CTLCOLOR_EDIT           1
#define MY_CTLCOLOR_LISTBOX        2
#define MY_CTLCOLOR_BTN            3
#define MY_CTLCOLOR_DLG            4
#define MY_CTLCOLOR_SCROLLBAR      5
#define MY_CTLCOLOR_STATIC         6
#define MY_CTLCOLOR_MAX            7

#define MY_BACK_START              34
#define MY_BACK_MID                111
#define MY_COLOR_END               0

#define  MAX_NUM_PER_PAGE		   20

//任务分配
#define TASK_NOT_MATCH "未分配"
#define TASK_HAVE_MATCH "已分配"


 #define WM_SHOWTASK (WM_USER +1)
#define WM_SYSTEMTRAY (WM_USER+2)

#define PARAM_MIDDLE_SYMBOL "|"
#define MED_QRCODE "QrCode"//二维码
#define MED_NAME "Name"//名称
#define MED_WEIGHT "Weight"//重量
#define MED_SPEC "Spec"//规格
#define MED_ORIGIN "Origin"
#define MED_NUMBER "Number"//数量
#define MED_DATE  "Date"//生产日期
#define MED_1CODE "Code1"//条形码
#define MED_DATE_TO "DateTo"//有效期
#define MED_OFFDOC  "OffDoc"//批文
#define MED_STANDARD "Standard"//执行标准
#define CPY_NAME  "CpName"//企业名称
#define MED_DEF1  "Define1"
#define MED_DEF2  "Define2"
#define MED_DEF3  "Define3"

typedef char int8;
typedef short int16;
typedef int int32;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

typedef enum
{
	E_RET_SUCCESS,
	E_RET_FAILED
}eRet;

#endif