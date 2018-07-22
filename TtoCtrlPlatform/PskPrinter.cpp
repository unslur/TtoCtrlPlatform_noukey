#include "StdAfx.h"
#include <stdlib.h>
#include "PskPrinter.h"
 #include <iostream>
#include <string>
#include <string>
#include <vector>
//#include "sqlite3.h"
#include <iostream>
#include <fstream>
#include <list>
using namespace std;
/////////////////////////////////
typedef int (__stdcall *fOpenPorts)(char* PortFlag);
typedef int (__stdcall *fPTK_ClearBuffers)();
typedef int (__stdcall *fPTK_SetLabelHeights)
(unsigned int lheight, unsigned int gapH,int gapOffset,BOOL bBlag);
typedef int (__stdcall *fPTK_SetLabelWidths)(unsigned int lwidth);
typedef int (__stdcall *fPTK_SetDarknesss)(unsigned  int id);
typedef int (__stdcall *fPTK_SetPrintSpeeds)(unsigned int px);
typedef int (__stdcall *fPTK_DrawBar2D_QRs)(unsigned int x, unsigned int y,
										   unsigned int w, unsigned int v,
										   unsigned int o, unsigned int r,
										   unsigned int m, unsigned int g,
										   unsigned int s, char* pstr);
typedef int (__stdcall *fPTK_DrawBar2D_QRExs) ( unsigned int x, unsigned int y, unsigned int o,unsigned int r, unsigned int g,unsigned int v, unsigned int s,LPTSTR id_name,LPTSTR pstr );

typedef int (__stdcall *fPTK_DrawTexts)(unsigned int px, unsigned int py,
									   unsigned int pdirec, unsigned int pFont,
									   unsigned int pHorizontal, unsigned int pVertical,
									   char ptext, char* pstr);


typedef int (__stdcall *fPTK_DrawTextTrueTypeWs)
		                            (int x,int y,int FHeight,
									int FWidth,LPCTSTR FType,
                                    int Fspin,int FWeight,
									BOOL FItalic,BOOL FUnline,
                                    BOOL FStrikeOut,
									LPCTSTR id_name,
									LPCTSTR data);

typedef int (__stdcall *fClosePorts)(void);
typedef int (__stdcall *fPTK_PrintLabels)(unsigned int number,
										 unsigned int cpnumber);

////////////////////////////////////////////////////
HINSTANCE gt1Psk;
fOpenPorts OpenPorts = NULL;
fPTK_ClearBuffers PTK_ClearBuffers = NULL;
fPTK_SetLabelHeights PTK_SetLabelHeights = NULL;
fPTK_SetLabelWidths PTK_SetLabelWidths = NULL;
fPTK_SetDarknesss PTK_SetDarknesss = NULL;
fPTK_SetPrintSpeeds PTK_SetPrintSpeeds = NULL;
fPTK_DrawBar2D_QRs PTK_DrawBar2D_QRs = NULL;
fPTK_DrawTexts PTK_DrawTexts = NULL;
fPTK_DrawTextTrueTypeWs PTK_DrawTextTrueTypeWs = NULL;
fClosePorts ClosePorts = NULL;
fPTK_PrintLabels PTK_PrintLabels = NULL;
fPTK_DrawBar2D_QRExs PTK_DrawBar2D_QRExs=NULL;

OSVERSIONINFO os;
 int sizeOffset3=0;
 int sizeOffset=0;
 int sizeOffset_1=0;
///////////////////////////////

CPskPrinter::CPskPrinter(void)
{
}

CPskPrinter::~CPskPrinter(void)
{
}
void splitStrThrees( string printStr, string &Str1, string &Str2, string&Str3 ){

	string strTemp;

	int b = printStr.find("|");
	if( b != -1 ){
		Str1 = printStr.substr(0,b);
		strTemp = printStr.substr(b+1,printStr.length());
		b = 0;
		b = strTemp.find("|");

		if (b != -1) {
			Str2 = strTemp.substr(0, b);
			Str3 = strTemp.substr(b+1,strTemp.length());
		}
		else {
			if(printStr.length()!=0){
				Str2 = strTemp;
			}
		}
	}
	else{
		if(printStr.length()!=0){
			Str1 = printStr;
		}
	}

	return;
}
list<string> splitPrintvs( list<string>& printStrv, int DYJC ){
	list<string> vstr;
	for(list<string>::const_iterator iter = printStrv.begin(); iter != printStrv.end();++iter){
		string tstr = *iter; //GBK 前字81-FE 之间，尾字节在 40-FE
		while(true){
			if(tstr.length()<=DYJC){
				vstr.push_back(tstr);
				break;
			}
			bool f = false;
			for( int i = 0; i < DYJC;){
				char m = tstr.at(i);
				if( unsigned(m) < 0x80 ){
					f = true;
					i++;
				}
				else{
					i += 2;
					if(i%2==0){
						f = true;
					}else{
						f = false;
					}
				}
			}
			if(f){
				vstr.push_back(tstr.substr(0,DYJC));
				tstr = tstr.substr(DYJC,tstr.length());
			}
			else{
				vstr.push_back(tstr.substr(0,DYJC-1));
				tstr = tstr.substr(DYJC-1,tstr.length());
			}
		}
	}
	return vstr;
}


list<string> splitPrintvs( string& tstr, int DYJC ){
	list<string> vstr;
	while(true){
		if(tstr.length()<=DYJC){
			vstr.push_back(tstr);
			break;
		}
		bool f = false;
		for( int i = 0; i < DYJC;){
			char m = tstr.at(i);
			if( unsigned(m) < 0x80 ){//汉字第一个字节在）0x81-0xFE之间 ，
				f = true;
				i++;
			}
			else{
				i += 2;
				if(i%2==0){
					f = true;
				}else{
					f = false;
				}
			}
		}
		if(f){
			vstr.push_back(tstr.substr(0,DYJC));
			tstr = tstr.substr(DYJC,tstr.length());
		}
		else{
			vstr.push_back(tstr.substr(0,DYJC-1));
			tstr = tstr.substr(DYJC-1,tstr.length());
		}
	}
	return vstr;
}
void delchar(char * str,char CharNum){
	char *d = str;
	while (*str != '\0') {
			
		if (*str != CharNum) {
			*d++ = *str;
		}
			
		++str;
		
	}
	*d = '\0';
	
}
list<string> splitPrintStrs( string printStr ){
	list<string> vstr;
	while (true){
		int b = printStr.find("\n");
		if( b != -1 ){
			vstr.push_back(printStr.substr(0,b));
			printStr = printStr.substr(b+1,printStr.length());
		}
		else{
			if(printStr.length()!=0){
				vstr.push_back(printStr);
			}
			break;
		}
	}
	return vstr;
}
void splitStrTwos( string printStr, string &Str1, string &Str2 )
{

	int b = printStr.find("|");
	if( b != -1 ){
		Str1 = printStr.substr(0,b);
		Str2 = printStr.substr(b+1,printStr.length());
	}
	else{
		if(printStr.length()!=0){
			Str1 = printStr;
		}
	}

	return;
}

bool printNew2_psk(const string& printStr,int X, int Y,int& id_name,fPTK_DrawBar2D_QRs PTK_DrawBar2D_QRs,fPTK_DrawTextTrueTypeWs PTK_DrawTextTrueTypeWs){
	char buff[0x100];
	
	list<string> printv = splitPrintStrs(printStr);
	int x = 0, y = 0, z = 0, ret = 0,XX = X + 135;
	char numBuff[10]={"dda"};
	static int num=1;
	for ( list<string>::iterator iter = printv.begin();iter != printv.end(); ++iter ){
		if( 0 == z ){ //打二维码
			sprintf(numBuff,"484%d",num++);
			
			if (strstr(iter->c_str(),"http") != NULL)
			{
				//ret = PTK_DrawBar2D_QR(X+134, Y, 0, 0, 0, 3, 4, 3, 3, (char*)iter->c_str());
				ret = PTK_DrawBar2D_QRExs(X+134,Y,0,4,0,0,3,(char *)numBuff,(char*)iter->c_str());//字符串长度限制导致无法正确扫描二维码，故选取该API，
				num=num%39;
			}
			else if(strlen(iter->c_str())<5){
				break;}
			else
			{
				//ret = PTK_DrawBar2D_QR(X+134, Y, 0, 0, 0, 4, 4, 3, 3, (char*)iter->c_str());
				ret = PTK_DrawBar2D_QRExs(X+120,Y,0,5,0,0,3,(char *)numBuff,(char*)iter->c_str());//字符串长度限制导致无法正确扫描二维码，故选取该API，
				num%=3;
			}
		
			if ( ret != 0 ){
				sprintf(buff,"PTK_DrawBar2D_QR:%d",ret);
				throw(buff);
			}
			
			y = Y + 170;
		}else if( 1 == z ){//打数字码
		
			list<string> printCodes = splitPrintvs(string(*iter),28);
			int x = X;
			for ( list<string>::const_iterator citer = printCodes.begin(); citer != printCodes.end(); ++citer ){
				ret = PTK_DrawTextTrueTypeWs(x, y, 17,0,"宋体", 1, 600, 0, 0, 0, itoa(id_name++,buff,10), (char*)citer->c_str()); //x-100
				if ( ret != 0 ){
					sprintf(buff,"PTK_DrawTextTrueTypeW:%d",ret);
					throw(buff);
				}
				x += 64;
				y += 22;
			}
			*iter=iter->substr((iter->find( "溯源码:")!=-1?iter->find("溯源码:")+7:0),iter->length());
			//DelRecordByCode(citer->c_str(),DBPathName);
			//DelRecordByCode(iter->c_str(),DBPathName);
		}else if( 2 == z || 4 == z || 6 == z || 7 == z||3 == z || 5 == z ){ //品名 产地 批号 厂家
			if( 3 == z){
				delchar((char *)iter->c_str(),' ');
				// *iter="重"+iter->substr(5,50);
			
			}//
			if (iter->length()==0)
			{
				z++;
				continue;
			}
			
			//else
			ret = PTK_DrawTextTrueTypeWs(X, y, 17,0,"宋体", 1, 600, 0, 0, 0, itoa(id_name++,buff,10), (char*)iter->c_str()); //x-100
			if ( ret != 0 ){
				sprintf(buff,"PTK_DrawTextTrueTypeW:%d",ret);
				throw(buff);
			}
			
		//	if( 2 != z && 4 != z )
				y += 20;
		}
		#if 0
else if(  5 == z ){ //重量  /*生产日期*/ 2015-6-23 取消药材打印日期
			if (z == 5)
			{
				//y+=22;
				z++;
				continue;
			}

#if 0
			ret = PTK_DrawTextTrueTypeW(XX, y, 17,0,"宋体", 1, 600, 0, 0, 0, itoa(id_name++,buff,10), (char*)iter->c_str()); //x-100
			if ( ret != 0 ){
				sprintf(buff,"PTK_DrawTextTrueTypeW:%d",ret);
				throw(buff);
			}y += 22;
			#endif
			
}
#endif
		
		z++;
	}
	
	return true;

}
bool printNew2_psk_1(const string& printStr,int X, int Y,int& id_name,fPTK_DrawBar2D_QRs PTK_DrawBar2D_QRs,fPTK_DrawTextTrueTypeWs PTK_DrawTextTrueTypeWs)

{
	char buff[0x100];
	list<string> printv = splitPrintStrs(printStr);
	int x = 0, y = 0, z = 0, ret = 0,XX = X + 135;
	int numTextX=X,numTextY=120,numX=X+170,numY=216;
	char numBuff[10]={"sps"};
	static int num=0;
	
	for ( list<string>::iterator iter = printv.begin();iter != printv.end(); ++iter ){
		if( 0 == z ){ //打二维码
			sprintf(numBuff,"rfdf%d",num++);
			if (strstr(iter->c_str(),"http") != NULL)
			{
				ret = PTK_DrawBar2D_QRExs(X+180,Y+92,0,3,0,0,3,(char*)numBuff,(char*)iter->c_str());//字符串长度限制导致无法正确扫描二维码，故选取该API，
				num%=2;
			}
			else if(strlen(iter->c_str())<5){
				break;}
			else
			{
				
				ret = PTK_DrawBar2D_QRExs(X+180,Y+92,0,4,0,0,3,(char*)numBuff,(char*)iter->c_str());//"psk+1"字符串：调用一次打印机，同时（init与uninit之间）打印多张二维码（此处为两张）且与其他参数不一样时，每次需要该字符串唯一，
				num%=3;
			}
		
			if ( ret != 0 ){
				sprintf(buff,"PTK_DrawBar2D_QR:%d",ret);
				throw(buff);
			}
			
			y = Y;
		}else if( 1 == z ){//打数字码
			string num=iter->substr(7,50);
			list<string> printCodes = splitPrintvs(num,16);
			
			for ( list<string>::const_iterator citer = printCodes.begin(); citer != printCodes.end(); ++citer ){
				ret = PTK_DrawTextTrueTypeWs(numX, numY, 12,0,"宋体", 1, 600, 0, 0, 0, itoa(id_name++,buff,10), (char*)citer->c_str()); //x-100
				if ( ret != 0 ){
					sprintf(buff,"PTK_DrawTextTrueTypeW:%d",ret);
					throw(buff);
				}
				
				numY += 10;
			}
			*iter=iter->substr((iter->find( "溯源码:")!=-1?iter->find("溯源码:")+7:0),iter->length());
			//DelRecordByCode(iter->c_str(),DBPathName);
		}else if( 2 == z || 4 == z || 6 == z || 7 == z ||3 == z || 5 == z){ //品名 产地 批号 厂家
			if( 7 == z){
				
				//if(strstr((char*)iter->c_str(),"厂家")!=NULL)
				//{
				//	*iter=iter->substr(5,50);
				//}
				*iter=iter->substr((iter->find( "厂家:")!=-1?iter->find("厂家:")+5:0),iter->length());
			}
			
				ret = PTK_DrawTextTrueTypeWs(numTextX, numTextY, 14,0,"宋体", 1, 600, 0, 0, 0, itoa(id_name++,buff,10), (char*)iter->c_str()); //x-100
				if ( ret != 0 ){
					sprintf(buff,"PTK_DrawTextTrueTypeW:%d",ret);
					throw(buff);
				}
			
					numTextY += 20;
		}
#if 0
		else if( 3 == z || 5 == z ){ //重量  /*生产日期*/ 2015-6-23 取消药材打印日期
			if (z == 5)
			{
				y+=22;
				z++;
				continue;
			}
			ret = PTK_DrawTextTrueTypeW(XX, y, 20,0,"微软雅黑", 1, 600, 0, 0, 0, itoa(id_name++,buff,10), (char*)iter->c_str()); //x-100
			if ( ret != 0 ){
				sprintf(buff,"PTK_DrawTextTrueTypeW:%d",ret);
				throw(buff);
			}
			y += 22;
		}
#endif
		z++;
	}
	return true;
}

bool printNew3_psk(const string& printStr,int X, int Y,int& id_name,fPTK_DrawBar2D_QRs PTK_DrawBar2D_QRs,fPTK_DrawTextTrueTypeWs PTK_DrawTextTrueTypeWs)
{
	char buff[0x100];
	list<string> printv = splitPrintStrs(printStr);
	int y = 0,z = 0,ret = 0;
	bool flag = false;
	char numBuff[10]={"pskss"};
	static int num=1;
	int division=12;
	int offset_X=0,offset_Y=0;
	for ( list<string>::iterator iter = printv.begin();iter != printv.end(); ++iter ){
		if( 0 == z ){ //打二维码

			sprintf(numBuff,"hhkh%d",num++);
			if (strstr(iter->c_str(),"http") != NULL)
			{	
				ret = PTK_DrawBar2D_QRExs(X+104,Y,0,3,0,0,3,(char *)numBuff,(char*)iter->c_str());//字符串长度限制导致无法正确扫描二维码，故选取该API，
				if ( ret != 0 ){
					sprintf(buff,"PTK_DrawBar2D_QR:%d",ret);
					throw(buff);
				}
				y = Y + 100;
				flag = true;					
				num=num%39;
			}
			else if(strlen(iter->c_str())<5){
				break;
			}
			else
			{
				
				ret = PTK_DrawBar2D_QRExs(X+104,Y,0,4,0,0,3,numBuff,(char*)iter->c_str());
				if ( ret != 0 ){
					sprintf(buff,"PTK_DrawBar2D_QR:%d",ret);
					throw(buff);
				}
				y = Y + 100;
				num=num%3;
			}
		}else if( 1 == z ){//打数字码
			*iter=iter->substr((iter->find( "溯源码:")!=-1?iter->find("溯源码:")+7:0),iter->length());
			if (iter->c_str()[0]=='8')
			{
				division=20;
				offset_X=-102;offset_Y=30;
			}
			list<string> printCodes = splitPrintvs(string(*iter),division);
			for ( list<string>::const_iterator citer = printCodes.begin(); citer != printCodes.end(); ++citer ){
				ret = PTK_DrawTextTrueTypeWs(X+106+offset_X, y, 14,0,"宋体", 1, 600, 0, 0, 0, itoa(id_name++,buff,10), (char*)citer->c_str()); //x-100
				if ( ret != 0 ){
					sprintf(buff,"PTK_DrawTextTrueTypeW:%d",ret);
					throw(buff);
				}
				y += 15;
			}
			if (iter->c_str()[0]=='6')
			{
				offset_Y=-15;

			}
		
			 y = Y + 100+offset_Y;
			//DelRecordByCode(iter->c_str(),DBPathName);
		}
		else{//其它文字
			if (2 == z)
			{
				delchar((char *)iter->c_str(),' ');
				
			}
			
			if (iter->length()==0)
			{
				z++;
				continue;
			}
			ret = PTK_DrawTextTrueTypeWs(X, y, 14,0,"宋体", 1, 600, 0, 0, 0, itoa(id_name++,buff,10), (char*)iter->c_str()); //x-100
			if ( ret != 0 ){
				sprintf(buff,"PTK_DrawTextTrueTypeW:%d",ret);
				throw(buff);
			}
			y += 15;
		}
		z++;
	}
	return true;
}

bool CPskPrinter::PskInit(char* printerName)
{
	gt1Psk = LoadLibrary("WINPSK.dll");
	OpenPorts = (fOpenPorts)GetProcAddress(gt1Psk,"OpenPort");
	ClosePorts = (fClosePorts)GetProcAddress(gt1Psk,"ClosePort");
	PTK_DrawBar2D_QRs = (fPTK_DrawBar2D_QRs)GetProcAddress(gt1Psk,"PTK_DrawBar2D_QR");
	PTK_SetLabelHeights = (fPTK_SetLabelHeights)GetProcAddress(gt1Psk,"PTK_SetLabelHeight");
	PTK_PrintLabels = (fPTK_PrintLabels)GetProcAddress(gt1Psk,"PTK_PrintLabel");
	PTK_SetDarknesss = (fPTK_SetDarknesss)GetProcAddress(gt1Psk,"PTK_SetDarkness");
	PTK_SetPrintSpeeds= (fPTK_SetPrintSpeeds)GetProcAddress(gt1Psk,"PTK_SetPrintSpeed");
	PTK_SetLabelWidths = (fPTK_SetLabelWidths)GetProcAddress(gt1Psk,"PTK_SetLabelWidth");
	PTK_ClearBuffers = (fPTK_ClearBuffers)GetProcAddress(gt1Psk,"PTK_ClearBuffer");
	PTK_DrawTexts = (fPTK_DrawTexts)GetProcAddress(gt1Psk,"PTK_DrawText");
	PTK_DrawTextTrueTypeWs = (fPTK_DrawTextTrueTypeWs)GetProcAddress(gt1Psk,"PTK_DrawTextTrueTypeW");
	PTK_DrawBar2D_QRExs=(fPTK_DrawBar2D_QRExs)GetProcAddress(gt1Psk,"PTK_DrawBar2D_QREx");
	//try{
		int ret = OpenPorts(printerName);	
		if ( ret != 0 ){
			//AfxMessageBox("无法打开打印机");
			return false;
		}
		//AfxMessageBox("1");
		return true;
}
bool CPskPrinter::PskUnInit()
{
	ClosePorts();
	FreeLibrary(gt1Psk);
	return true;
}
int CPskPrinter::PrintPsk2(char* printerName, const string& printStr,LONG nPrintType)
{
	int rtn,id_name = 0;
	bool bIsPrint = false;
	char buff[0x100]={0};
	string printStr1, printStr2;
	splitStrTwos(printStr, printStr1, printStr2);
	

	try{
		int ret = PTK_ClearBuffers();
		if ( ret != 0 ){
			sprintf(buff,"PTK_ClearBuffer:%d",ret);
			throw(buff);
		}
		ret = PTK_SetLabelHeights(368, 40,0,false);
		if ( ret != 0 ){
			sprintf(buff,"PTK_SetLabelHeight:%d",ret);
			throw(buff);
		}
		ret = PTK_SetLabelWidths(800);
		if ( ret != 0 ){
			sprintf(buff,"PTK_SetLabelWidth:%d",ret);
			throw(buff);
		}
		ret = PTK_SetDarknesss(10);
		if ( ret != 0 ){
			sprintf(buff,"PTK_SetDarkness:%d",ret);
			throw(buff);
		}
		ret = PTK_SetPrintSpeeds(5);
		if ( ret != 0 ){
			sprintf(buff,"PTK_SetPrintSpeed:%d",ret);
			throw(buff);
		}

		int X = 80,Y = 23;
		//打印左边
		if (printStr1.length() > 0 && (nPrintType == 1 || nPrintType == 3)){
			bIsPrint |= printNew2_psk(printStr1,X,Y,id_name,PTK_DrawBar2D_QRs,PTK_DrawTextTrueTypeWs);
		}

		//打印右边
		if (printStr2.length() > 0 && (nPrintType == 2 || nPrintType == 3)){
			bIsPrint |= printNew2_psk(printStr2,X+=366,Y,id_name,PTK_DrawBar2D_QRs,PTK_DrawTextTrueTypeWs);
		}
		//==================================
		ret = PTK_PrintLabels(1, 1);	
		if ( ret != 0 ){
			sprintf(buff,"PTK_PrintLabel:%d",ret);
			throw(buff);
		}
		//ClosePort();
		rtn = 1;
	}
	catch (const exception& e) {
		AfxMessageBox(e.what());
		rtn = 0;
	}
	catch (...) {
		sprintf(buff,"打印机错误:%d",GetLastError());
		AfxMessageBox(buff);
		rtn = 0;
	}
	//FreeLibrary(gt1);
	return rtn;
}
int CPskPrinter::PrintPsk2_1(char* printerName,const  string & printStr,LONG nPrintType)
{
	int rtn,id_name = 0;
	bool bIsPrint = false;
	char buff[0x100]={0};
	string printStr1, printStr2;
	splitStrTwos(printStr, printStr1, printStr2);
	

	try{
		int ret = PTK_ClearBuffers();
		if ( ret != 0 ){
			sprintf(buff,"PTK_ClearBuffer:%d",ret);
			throw(buff);
		}
		ret = PTK_SetLabelHeights(240, 24,0,false);
		if ( ret != 0 ){
			sprintf(buff,"PTK_SetLabelHeight:%d",ret);
			throw(buff);
		}
		ret = PTK_SetLabelWidths(800);
		if ( ret != 0 ){
			sprintf(buff,"PTK_SetLabelWidth:%d",ret);
			throw(buff);
		}
		ret = PTK_SetDarknesss(15);
		if ( ret != 0 ){
			sprintf(buff,"PTK_SetDarkness:%d",ret);
			throw(buff);
		}
		ret = PTK_SetPrintSpeeds(5);//用于设置每一个单位代表多少mm，表示每1单位0.125mm
		if ( ret != 0 ){
			sprintf(buff,"PTK_SetPrintSpeed:%d",ret);
			throw(buff);
		}

		int X = 80,Y = 23;
		//打印左边
		if (printStr1.length() > 0 && (nPrintType == 1 || nPrintType == 3)){
			bIsPrint |= printNew2_psk_1(printStr1,X,Y,id_name,PTK_DrawBar2D_QRs,PTK_DrawTextTrueTypeWs);
		}

		//打印右边
		if (printStr2.length() > 0 && (nPrintType == 2 || nPrintType == 3)){
			bIsPrint |= printNew2_psk_1(printStr2,X+=366,Y,id_name,PTK_DrawBar2D_QRs,PTK_DrawTextTrueTypeWs);
		}
		//==================================
		ret = PTK_PrintLabels(1, 1);	
		if ( ret != 0 ){
			sprintf(buff,"PTK_PrintLabel:%d",ret);
			throw(buff);
		}
		//ClosePort();
		rtn = 1;
	}
	catch (const exception& e) {
		AfxMessageBox(e.what());
		rtn = 0;
	}
	catch (...) {
		sprintf(buff,"打印机错误:%d",GetLastError());
		AfxMessageBox(buff);
		rtn = 0;
	}
	//FreeLibrary(gt1);
	return rtn;
}


int CPskPrinter::PrintPsk3(char* printerName, const string& printStr,LONG nPrintType)
{
	int rtn,id_name = 0;
	bool bIsPrint = false;
	char buff[0x100]={0};
	
	string printStr1, printStr2,printStr3;
	splitStrThrees(printStr, printStr1, printStr2, printStr3);

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	
	try{
		int ret = PTK_ClearBuffers();
		if ( ret != 0 ){
			sprintf(buff,"PTK_ClearBuffer:%d",ret);
			throw(buff);
		}
		ret = PTK_SetLabelHeights(200, 24,0,false);
		if ( ret != 0 ){
			sprintf(buff,"PTK_SetLabelHeight:%d",ret);
			throw(buff);
		}
		ret = PTK_SetLabelWidths(800);
		if ( ret != 0 ){
			sprintf(buff,"PTK_SetLabelWidth:%d",ret);
			throw(buff);
		}
		ret = PTK_SetDarknesss(13);
		if ( ret != 0 ){
			sprintf(buff,"PTK_SetDarkness:%d",ret);
			throw(buff);
		}
		ret = PTK_SetPrintSpeeds(5);
		if ( ret != 0 ){
			sprintf(buff,"PTK_SetPrintSpeed:%d",ret);
			throw(buff);
		}

		//int X = 42,Y = 10;//old
		int X = 37,Y = 18;//new
		//sprintf(numBuff,"%d",num++)
		//打印左边
		if (printStr1.length() > 0 && (nPrintType == 1 || nPrintType == 4)){
			bIsPrint |= printNew3_psk(printStr1,X,Y,id_name,PTK_DrawBar2D_QRs,PTK_DrawTextTrueTypeWs);
		}

		//打印中间
		if (printStr2.length() > 0 && (nPrintType == 2 || nPrintType == 4)){
			bIsPrint |= printNew3_psk(printStr2,X+=260,Y,id_name,PTK_DrawBar2D_QRs,PTK_DrawTextTrueTypeWs);
		}

		//打印右边
		if (printStr3.length() > 0 && (nPrintType == 3 || nPrintType == 4)){
			bIsPrint |= printNew3_psk(printStr3,X+=260,Y,id_name,PTK_DrawBar2D_QRs,PTK_DrawTextTrueTypeWs);
		}
		//==================================
		if (bIsPrint){
			ret = PTK_PrintLabels(1, 1);	
			if ( ret != 0 ){
				sprintf(buff,"PTK_PrintLabel:%d",ret);
				throw(buff);
			}
		}
	
		rtn = 1;
	}
	catch (const exception& e) {
		AfxMessageBox(e.what());
		rtn = 0;
	}
	catch (...) {
		sprintf(buff,"打印机错误:%d",GetLastError());
		AfxMessageBox(buff);
		rtn = 0;
	}
	
	return rtn;
}