#pragma once
#ifndef PSK_PRINTER_H
#define PSK_PRINTER_H
//#include "Funcs.h"

#include <string>
#include <vector>
//#include "sqlite3.h"
#include <iostream>
#include <fstream>
#include <list>
#include <string>
using namespace std;
class CPskPrinter
{
public:
	CPskPrinter(void);
	~CPskPrinter(void);

	bool PskInit(char* printerName);
	bool PskUnInit();
	int PrintPsk2(char* printerName,const string& printStr,LONG nPrintType);
	int PrintPsk2_1(char* printerName, const string& printStr,LONG nPrintType);
	int PrintPsk3(char* printerName, const string& printStr,LONG nPrintType);
};

#endif
