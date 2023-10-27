#pragma once
#ifndef __const_def_h__
#define __const_def_h__

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- DtRecord Version -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
#define RECORDER_VERSION_MAJOR 0
#define RECORDER_VERSION_MINOR 2
#define RECORDER_VERSION_BUGFIX 0

#include <stdarg.h>
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <thread>
#include <chrono>
#include <queue>
#include <mutex>
#include <atomic>
#include <string>

#define heartbeat_period_sec 1000
#define number_of_live_feedback 3

enum mode_switch
{
	unknown,
	listDevices,
	deviceStatus,
	recordSignal,
	showHelp,
	error_general,
	error_user,
	error_out_of_bound,
	echo_version_number
};

const char c_deviceSerialNumber[] = "sn";
const int c_BufSize = 16 * 1024 * 1024; // Data transfer buffer size

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- class CommandLineParams -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
//
class CommandLineParams
{
public:
	CommandLineParams();
	virtual ~CommandLineParams();

	char* filename_parsed = (char*)malloc(128 * sizeof(char));

	mode_switch CommandLineParams::ParseCommandLine(
		int argc,
		char* argv[],
		int* deviceSerialNumber,
		int* modultionType,
		double* carrierFrequency,
		int* receiveMode);
	int getSymbolRate();
private:
	int userSymbolRate = -1;
};

struct s
{
	char* pointerToByte;
	int numOfByte;
	s(char* p, int i) :pointerToByte{ p }, numOfByte{ i } {}
};

#endif
