#pragma once
#ifndef __support_functions_h__
#define __support_functions_h__

#include "const_def.h"
#include <DTAPI.h>
#include "DtSdiFileFmt.h"
#include <conio.h>
#include <Windows.h>
#include <string>
#include <fstream>
#include <cstring>
#include <thread>
#include <chrono>
#include <queue>
#include <mutex>
#include <atomic>

DTAPI_RESULT listDevices_f();
Dtapi::DTAPI_RESULT showDeviceStatus(
	int serialNumber);
Dtapi::DTAPI_RESULT showDeviceStatus(
	int serialNumber,
	int modulationType,
	int receiveMode,
	double carrierFrequency);

class processSignal
{
public:
	processSignal();
	processSignal(
		int serialNumber);
	processSignal(
		int serialNumber,
		int modulationType,
		double carrierFrequency,
		int receiveMode);
	virtual ~processSignal();

	DTAPI_RESULT f_recordSignal(
		std::queue<s> & dataToProcess,
		std::mutex & mut,
		std::condition_variable & cv,
		std::atomic<bool>& finished);
	DTAPI_RESULT f_printStatistics();
	Dtapi::DTAPI_RESULT setMaxSize(__int64 size);
	char* filename_c;
	void setSymbolRate(int sps);

protected:
	Dtapi::DTAPI_RESULT f_attachToInput();
	Dtapi::DTAPI_RESULT f_initInput();
	int trnmt_startup_msg();
	void f_recordFile(std::queue<s> & dataToProcess,
		std::mutex & mut,
		std::condition_variable & cv,
		std::atomic<bool>& finished);

	int deviceSerialNumber;
	int modType, rxMode;
	int symbolRate = DTAPI_MOD_SYMRATE_AUTO;
	__int64 maxSize;
	double carrierFreq;
	DtDevice m_DtDvc;
	DtInpChannel m_DtInp; // Our input channel
	DTAPI_RESULT dr;
	bool m_Demod;  // Current input is a demodulator
	char *m_pBuf;  // Our data buffer
	FILE *m_pFile = NULL; // Our record file
	int m_Align;   // Alignment for record file (i.e. file should be
				   // multiple of alignment)
};

void f_showHelp();

#endif
