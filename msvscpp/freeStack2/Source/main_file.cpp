//*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#* DtRecord.cpp *#*#*#*#*#*#*#*# (C) 2000-2022 DekTec
//

//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Include files -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
#include <stdio.h>
#include <iostream>
#include "main_file.h"
#include "udp_client.h"

#ifdef WINBUILD
#define NOMINMAX
#include <Windows.h>
#include <conio.h>
#else
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#endif

#ifndef WINBUILD
void ChangeTerminalMode(int dir)
{
	static struct termios OldT, NewT;

	if (dir == 1)
	{
		tcgetattr(STDIN_FILENO, &OldT);
		NewT = OldT;
		NewT.c_lflag &= ~(ICANON | ECHO);
		tcsetattr(STDIN_FILENO, TCSANOW, &NewT);
	}
	else
	{
		tcsetattr(STDIN_FILENO, TCSANOW, &OldT);
	}
}
int _kbhit()
{
	struct timeval tv;
	fd_set rdfs;

	tv.tv_sec = 0;
	tv.tv_usec = 0;

	FD_ZERO(&rdfs);
	FD_SET(STDIN_FILENO, &rdfs);

	select(STDIN_FILENO + 1, &rdfs, NULL, NULL, &tv);
	return FD_ISSET(STDIN_FILENO, &rdfs);
}
#endif

Dtapi::DTAPI_RESULT main(int argc, char* argv[])
{
	int deviceSerialNumber(-1),
		modType(-1),
		rxMode(-1);
	double carrierFreq = -1; // in what unit?
	mode_switch desiredMode;
	Dtapi::DTAPI_RESULT retValue(0), dr;

	// did the user run without any argument?
	if (argc == 1)
	{
		f_showHelp();
		return 0;
	}

	CommandLineParams m_CmdLineParams;
	try
	{
		desiredMode = m_CmdLineParams.ParseCommandLine(argc, argv, &deviceSerialNumber, &modType, &carrierFreq, &rxMode);
	}
	catch (...)
	{
#ifdef _DEBUG 
		std::cout << "exception encountered. Exiting application";
#endif
		retValue = DTAPI_E;
	}


	switch (desiredMode)
	{

		case listDevices:
		{
#ifdef _DEBUG
			std::cout << "list all devices\n";
#endif

			listDevices_f();
		}
		break;

		case deviceStatus:
		{
#ifdef _DEBUG
			std::cout << "requesting device status" << std::endl;
#endif
			if (deviceSerialNumber == -1)
			{
#ifdef _DEBUG
				std::cout << "INVALID DEVICE SERIAL.";
#endif
				retValue = DTAPI_E_INVALID_FLAGS;
				break;
			}
			if (modType != -1 && rxMode != -1 &&
				carrierFreq != -1)
			{
				try
				{
					dr = showDeviceStatus(
						deviceSerialNumber,
						modType,
						rxMode,
						carrierFreq);
					/* if (dr == DTAPI_OK)
					 {
						 std::cout << "show dev ok" << std::endl;
					 }
					 else
					 {
						 std::cout << "show dev not ok" << std::endl;
					 } todo */
				}
				catch (...)
				{
#ifdef _DEBUG
					std::cout << "something didnot go to plan" << std::endl;
#endif
				}
			}
			else
			{
				showDeviceStatus(deviceSerialNumber);
			}
		}
		break;

		case recordSignal:
		{
#ifdef _DEBUG
			std::cout << "recordSignal\n";
#endif
			// create the queue and the control varialble 
			//   for multi-threading
			std::queue<s> data;
			std::condition_variable cv;
			std::mutex mut;
			std::atomic<bool> finished = false;

			// initialize worker thread to strip BB Header
			auto bbObj = bbProcessor();
			bbObj.filename_c = m_CmdLineParams.filename_parsed;
#ifdef _DEBUG
			std::cout << "filename: " << m_CmdLineParams.filename_parsed << std::endl;
#endif
			std::thread worker = std::thread(
				&bbProcessor::stripBB,
				&bbObj,
				std::ref(data),
				std::ref(mut),
				std::ref(cv),
				std::ref(finished));

			// initialize the main obj to capture the data
			//   directly from dektec board
			processSignal processor0(
				deviceSerialNumber,
				modType,
				carrierFreq,
				rxMode);
			processor0.filename_c = m_CmdLineParams.filename_parsed;
			if (m_CmdLineParams.getSymbolRate() > 500000 && m_CmdLineParams.getSymbolRate() < 72000000)
			{
				processor0.setSymbolRate(m_CmdLineParams.getSymbolRate());
			}

			dr = processor0.f_recordSignal(
				std::ref(data),
				std::ref(mut),
				std::ref(cv),
				std::ref(finished));
			if (dr != DTAPI_OK)
			{
				::DtapiResult2Str(dr);
			}
			finished = true; // necessary to terminate all support threads
			cv.notify_all();
			worker.join(); // wait for BB stripper worker to finish
		}
		break;

		case showHelp:
		{
			f_showHelp();
			retValue = DTAPI_OK;
		}
		break;
		case echo_version_number:
		{
			std::string versionNumber =
				"v" +
				std::to_string(RECORDER_VERSION_MAJOR) +
				"." +
				std::to_string(RECORDER_VERSION_MINOR) +
				"." +
				std::to_string(RECORDER_VERSION_BUGFIX);
			std::cout << versionNumber << std::endl;

			retValue = DTAPI_OK;
		}
		break;
		default:
		{
#ifdef _DEBUG
			std::cout << "not in the enum\n";
#endif
			f_showHelp();
			retValue = DTAPI_E_INVALID_FLAGS;
		}
		break;
	}

	// find the file size which we need to tell the GS operator.
	std::ifstream in(m_CmdLineParams.filename_parsed, std::ifstream::ate | std::ifstream::binary);
	std::streampos our_recorded_file_size = -1;
	if (in.good())
	{
		our_recorded_file_size = in.tellg();
#ifdef _DEBUG
		std::cout << "filesize: " << our_recorded_file_size << std::endl;
#endif
	}

	// message the GS operator
	auto objRef = MySocketClass();
	std::string msgToBeSent = "";
	if (retValue == DTAPI_OK)
	{
		msgToBeSent.append(std::to_string(deviceSerialNumber));
		msgToBeSent.append(": Exiting main(). Filesize: ");
		msgToBeSent.append(std::to_string(our_recorded_file_size)); 
	}
	else
	{
		msgToBeSent.append("Exiting due to error code: ");
		msgToBeSent.append(std::to_string(retValue));
	}
#ifdef _DEBUG
		std::cout << msgToBeSent << std::endl;
#endif

	char* msgToBeSent_ptr = new char[512];
	// make it c_str
	strcpy_s(msgToBeSent_ptr, msgToBeSent.size() + 1, msgToBeSent.c_str());
	objRef.sendData(msgToBeSent_ptr, (int)msgToBeSent.size() + 1);
	delete[] msgToBeSent_ptr;
	return retValue;
}