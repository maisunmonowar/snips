#include "support_functions.h"
#include <iostream>
#include "udp_client.h"


void ifNotGoodPrint(Dtapi::DTAPI_RESULT res)
{
	// todo
	// this thing should probably be a macro, not a function.
	if (res != DTAPI_OK)
	{
		::DtapiResult2Str(res);
	}
}

DTAPI_RESULT listDevices_f()
{
	DTAPI_RESULT dr;
	int n, NumHwFuncsFound(0);
	DtHwFuncDesc HwFuncs[40];

	// Scan for available DekTec devices
	dr = ::DtapiHwFuncScan(sizeof(HwFuncs) / sizeof(HwFuncs[0]), NumHwFuncsFound, HwFuncs);
	// todo
	// if dr is not ok, raise excepions

	// Loop through found hardware functions
	bool Found(false), SetIoConfig(false);
	int DeviceNum(0);
	char *stringBuffer;
	int stringBufferLength(127);
	stringBuffer = (char *)malloc(stringBufferLength + 1);
	DtHwFuncDesc *pHwf = &HwFuncs[0];
	for (n = 0; n < NumHwFuncsFound; n++, pHwf++)
	{
		// Skip non-inputs
		if (0 == (pHwf->m_ChanType & DTAPI_CHAN_INPUT) && (pHwf->m_Flags & DTAPI_CAP_INPUT) == 0)
			continue;
		//::DtapiDtHwFuncDesc2String(pHwf, DTAPI_HWF2STR_TYPE_AND_PORT, stringBuffer, stringBufferLength);
		// printf("%s\n", stringBuffer);
		//::DtapiDtHwFuncDesc2String(pHwf, DTAPI_HWF2STR_TYPE_AND_LOC, stringBuffer, stringBufferLength);
		// printf("%s\n", stringBuffer);
		printf("%s\n", "----------------------------------------");
		// printf("%s %d\n", "m_chanType", pHwf->m_ChanType);
		// printf("%s %d\n", "m_IndexOnDev", pHwf->m_IndexOnDvc);
		// printf("%s %d\n", "Device Category:", pHwf->m_DvcDesc.m_Category);
		printf("%s %I64d\n", "m_Serial", pHwf->m_DvcDesc.m_Serial);
		printf("%s %d\n", "PCI Bus Number:", pHwf->m_DvcDesc.m_PciBusNumber);
		printf("%s %d\n", "slot number:", pHwf->m_DvcDesc.m_SlotNumber);
		// printf("%s %d\n", "usb address", pHwf->m_DvcDesc.m_UsbAddress);
		printf("%s %d\n", "type nubmer:", pHwf->m_DvcDesc.m_TypeNumber);
		// printf("%s %d\n", "sub type", pHwf->m_DvcDesc.m_SubType);
		printf("%s %d\n", "device id", pHwf->m_DvcDesc.m_DeviceId);
		// printf("%s %d\n", "vendor id", pHwf->m_DvcDesc.m_VendorID);
		// printf("%s %d\n", "sub systme id ", pHwf->m_DvcDesc.m_SubsystemId);
		// printf("%s %d\n", "sub vendo id", pHwf->m_DvcDesc.m_SubVendorId);
		// printf("%s %d\n", "NumHwFunc", pHwf->m_DvcDesc.m_NumHwFuncs);
		// printf("%s %d\n", "m_HardwareRevision", pHwf->m_DvcDesc.m_HardwareRevision);
		// printf("%s %d\n", "m_FirmwareVersion", pHwf->m_DvcDesc.m_FirmwareVersion);
		// printf("%s %d\n", "m_FirmwareVariant", pHwf->m_DvcDesc.m_FirmwareVariant);
		// printf("%s %d\n", "m_NumDtInpChan", pHwf->m_DvcDesc.m_NumDtInpChan);
		// printf("%s %d\n", "NumDtOutpChan", pHwf->m_DvcDesc.m_NumDtOutpChan);
		printf("%s %d\n", "m_Port", pHwf->m_Port);
		printf("%s\n", "----------------------------------------");
	}
	return DTAPI_OK;
}

Dtapi::DTAPI_RESULT showDeviceStatus(int serialNumber)
{
	processSignal ourDevice;
	ourDevice = processSignal(serialNumber);
	ourDevice.f_printStatistics();
	return DTAPI_OK;
}

Dtapi::DTAPI_RESULT showDeviceStatus(
	int serialNumber,
	int modulationType,
	int receiveMode,
	double carrierFrequency)
{
	processSignal ourDevice(
		serialNumber,
		modulationType,
		carrierFrequency,
		receiveMode);

	ourDevice.f_printStatistics();
	return DTAPI_OK;
}

processSignal::processSignal(int serialNumber)
{
	/*
	to be used when getting the device status
	 because nothing other that serial number has been proviced,
	 other value related to device initialization must be safe.
	 */

	deviceSerialNumber = serialNumber; // user input

	modType = DTAPI_MOD_DVBS2_16APSK; // default modulation of XTx
	rxMode = DTAPI_RXMODE_STL3;		  // defualt
	carrierFreq = 1120;				  // as much I remember, one of the channels of XTX.
}

void processSignal::setSymbolRate(int sps)
{
	symbolRate = sps;
}

processSignal::processSignal()
{
	deviceSerialNumber = -1;
	modType = -1;
	rxMode = -1;
	carrierFreq = -1;
}

processSignal::~processSignal()
{
	// Detach from channel and device
	m_DtInp.SetRxControl(DTAPI_RXCTRL_IDLE);
	m_DtInp.Detach(DTAPI_INSTANT_DETACH); // will do nothing if already detached
	m_DtDvc.Detach();					  // will do nothing if already detached

	// todo take care fo buffer.
	// Free our buffer
	/*if (m_pBuf != NULL)
	{
		delete[] m_pBuf;
		m_pBuf = NULL;
	}*/
}

processSignal::processSignal(
	int serialNumber,
	int modulationType,
	double carrierFrequency,
	int receiveMode)
{
	deviceSerialNumber = serialNumber;
	modType = modulationType;
	rxMode = receiveMode;
	carrierFreq = carrierFrequency;
	maxSize = -1; // no limit
}

Dtapi::DTAPI_RESULT processSignal::f_attachToInput()
{
	int n, NumHwFuncsFound(0);
	DtHwFuncDesc HwFuncs[40];

	// Scan for available DekTec devices
	dr = ::DtapiHwFuncScan(sizeof(HwFuncs) / sizeof(HwFuncs[0]), NumHwFuncsFound, HwFuncs);
	// todo
	if (dr != DTAPI_OK)
	{
		::DtapiResult2Str(dr);
	}

	// Loop through found hardware functions
	bool Found(false), SetIoConfig(true);
	int DeviceNum(0);
	DtHwFuncDesc *pHwf = &HwFuncs[0];

	if (NumHwFuncsFound == 0)
	{
#ifdef _DEBUG
		std::cout << "no dektec boards found." << std::endl;
#endif
		return DTAPI_E_NO_DEVICE;
	}

	for (n = 0; n < NumHwFuncsFound; n++, pHwf++)
	{
		// Skip non-inputs
		if (0 == (pHwf->m_ChanType & DTAPI_CHAN_INPUT) &&
			(pHwf->m_Flags & DTAPI_CAP_INPUT) == 0)
			continue;
		if (pHwf->m_DvcDesc.m_Serial == deviceSerialNumber && pHwf->m_Port == 1)
		{
#ifdef _DEBUG
			printf("%s\n", "---------------- Selected Device ------------------------");
			printf("%s %I64d\n", "m_Serial", pHwf->m_DvcDesc.m_Serial);
			printf("%s %d\n", "PCI Bus Number:", pHwf->m_DvcDesc.m_PciBusNumber);
			printf("%s %d\n", "slot number:", pHwf->m_DvcDesc.m_SlotNumber);
			printf("%s %d\n", "type nubmer:", pHwf->m_DvcDesc.m_TypeNumber);
			printf("%s %d\n", "device id", pHwf->m_DvcDesc.m_DeviceId);
			printf("%s %d\n", "m_Port", pHwf->m_Port);
			printf("%s\n", "----------------------------------------");
#endif
			break;
		}
	}
	// Attach to the device
	dr = m_DtDvc.AttachToSerial(deviceSerialNumber);

	if (dr != DTAPI_OK)
	{
		return dr;
		ifNotGoodPrint(dr);
	}

	// Set APSK mode for DTA-2137 when modulation standard is DVB-S2 16APSK/32APSK
	// Port 2 must be disabled in order to enable APSK mode for port 1
	if (m_DtDvc.m_DvcDesc.m_TypeNumber == 2137)
	{
		if (modType == DTAPI_MOD_DVBS2_16APSK || modType == DTAPI_MOD_DVBS2_32APSK)
		{
			// Set port 4 as output (disable the loop-through of port 2),
			// otherwise port 2 can be disabled
			dr = m_DtDvc.SetIoConfig(4, DTAPI_IOCONFIG_IODIR, DTAPI_IOCONFIG_OUTPUT,
									 DTAPI_IOCONFIG_OUTPUT);
			if (dr != DTAPI_OK)
			{
#ifdef _DEBUG
				std::cout << "Setting port 4 as output failed." << std::endl;
				ifNotGoodPrint(dr);
#endif
				return dr;
			}

			// Disable port 2
			dr = m_DtDvc.SetIoConfig(2, DTAPI_IOCONFIG_IODIR, DTAPI_IOCONFIG_DISABLED);
			if (dr != DTAPI_OK)
			{
#ifdef _DEBUG
				ifNotGoodPrint(dr);
				std::cout << "Could not disable port 2." << std::endl;
#endif
				return dr;
			}
			// Enable APSK mode for port 1
			dr = m_DtDvc.SetIoConfig(1, DTAPI_IOCONFIG_SWS2APSK, DTAPI_IOCONFIG_TRUE);
			ifNotGoodPrint(dr);
			if (dr != DTAPI_OK)
			{
#ifdef _DEBUG
				std::cout << "Failed to enable APSK mode for port1" << std::endl;
#endif
				return dr;
			}
		}
	}

	// Disable APSK mode for DTA-2137 when modulation standard is not DVB-S2 16APSK/32APSK
	// Port 2 can be re-enabled
	if (m_DtDvc.m_DvcDesc.m_TypeNumber == 2137 && modType != DTAPI_MOD_DVBS2_16APSK && modType != DTAPI_MOD_DVBS2_32APSK)
	{
		// Disable APSK mode for port 1
		dr = m_DtDvc.SetIoConfig(1, DTAPI_IOCONFIG_SWS2APSK, DTAPI_IOCONFIG_FALSE);
		if (dr != DTAPI_OK)
		{
			return dr;
		}
		// Enable port 2
		dr = m_DtDvc.SetIoConfig(2, DTAPI_IOCONFIG_IODIR, DTAPI_IOCONFIG_INPUT,
								 DTAPI_IOCONFIG_INPUT);
		if (dr != DTAPI_OK)
		{
			return dr;
		}
	}

	// Set the IO-config to input?
	if (SetIoConfig)
	{
		dr = m_DtDvc.SetIoConfig(pHwf->m_Port, DTAPI_IOCONFIG_IODIR,
								 DTAPI_IOCONFIG_INPUT, DTAPI_IOCONFIG_INPUT);
		if (dr != DTAPI_OK)
		{
#ifdef _DEBUG
			std::cout << "ioconfig cannot set to input." << std::endl;
#endif
			return dr;
		}
	}

	// Attach to the input channel
	dr = m_DtInp.AttachToPort(&m_DtDvc, pHwf->m_Port);
	ifNotGoodPrint(dr);
	if (dr != DTAPI_OK)
	{
#ifdef _DEBUG
		std::cout << "Failed to attach to device. " << std::endl;
#endif
		return dr;
	}

	// Check for demodulator card
	m_Demod = (m_DtInp.m_HwFuncDesc.m_Flags & DTAPI_CAP_DEMOD) != 0;
#ifdef _DEBUG
	std::cout << "Attachment completed." << std::endl;
#endif
	return DTAPI_OK;
}

DTAPI_RESULT processSignal::f_initInput()
{
#ifdef _DEBUG
	std::cout << "init input" << std::endl;
#endif
	// Init channel to initial 'safe' state
	dr = m_DtInp.SetRxControl(DTAPI_RXCTRL_IDLE); // Start in IDLE mode
	// ^^ this line throws exception on debugging.
	// do not have a breakpoint between deviceAttach() and this line
	// the reason being, as soon as you attach, the fifo starts generating data
	// so you need to setRxControl to idle. then do your debugging/brekpoints.

	if (dr != DTAPI_OK)
	{
#ifdef _DEBUG
		std::cout << "Could not set it to idle mode." << std::endl;
		DtapiResult2Str(dr);
#endif
		return dr;
	}
	dr = m_DtInp.ClearFifo(); // Clear FIFO (i.e. start with zero load)
	if (dr != DTAPI_OK)
	{
		DtapiResult2Str(dr);
		return dr;
	}
	dr = m_DtInp.ClearFlags(0xFFFFFFFF); // Clear all flags
	if (dr != DTAPI_OK)
	{
		DtapiResult2Str(dr);
		return dr;
		// todo I really need to come up with better error handling.
	}

	if (modType != DTAPI_MOD_IQDIRECT)
	{
		// First do a SetIoConfig to switch between ASI and SDI
		if ((rxMode & DTAPI_RXMODE_TS) != 0)
		{
			if ((m_DtInp.m_HwFuncDesc.m_Flags & DTAPI_CAP_ASI) != 0)
				dr = m_DtInp.SetIoConfig(DTAPI_IOCONFIG_IOSTD, DTAPI_IOCONFIG_ASI, -1);
		}
		else
		{
			// Set a dummy value for now, this will be changed to the correct IOCONFIG later
			dr = m_DtInp.SetIoConfig(DTAPI_IOCONFIG_IOSTD, DTAPI_IOCONFIG_SDI,
									 DTAPI_IOCONFIG_625I50);
		}

		if (dr != DTAPI_OK)
		{
			DtapiResult2Str(dr);
			return dr;
		}
	}

	// Apply de-modulation settings (if we have a de-modulator)
	if (m_Demod)
	{
		// Set a default for non-monitor IQ direct channels?
		if (carrierFreq < 0.0 &&
			modType == DTAPI_MOD_IQDIRECT)
			carrierFreq = 578.0; // UHF-band

		// Set tuner frequency
		if (carrierFreq >= 0.0)
		{
			__int64 Freq = (__int64)(carrierFreq * 1E6);
			dr = m_DtInp.SetTunerFrequency(Freq);
			// if (dr != DTAPI_OK)
			// throw Exc(c_ErrFailSetRfControl, ::DtapiResult2Str(dr));
		}

		bool IsDvbS = (modType == DTAPI_MOD_DVBS_BPSK || modType == DTAPI_MOD_DVBS_QPSK);

		bool IsDvbS2 = (modType == DTAPI_MOD_DVBS2 || modType == DTAPI_MOD_DVBS2_QPSK || modType == DTAPI_MOD_DVBS2_8PSK || modType == DTAPI_MOD_DVBS2_16APSK || modType == DTAPI_MOD_DVBS2_32APSK);

		bool IsQAM = (modType == DTAPI_MOD_QAM4 || modType == DTAPI_MOD_QAM16 || modType == DTAPI_MOD_QAM32 || modType == DTAPI_MOD_QAM64 || modType == DTAPI_MOD_QAM128 || modType == DTAPI_MOD_QAM256);

		// Set modulation control
		if (IsDvbS || IsDvbS2)
		{
			int XtraPar1 = -1;

			// For DVB-S2: Auto detect pilots and frame size
			if (IsDvbS2)
				XtraPar1 |= DTAPI_MOD_S2_PILOTS_AUTO | DTAPI_MOD_S2_FRM_AUTO;

			dr = m_DtInp.SetDemodControl(modType,
										 DTAPI_MOD_CR_AUTO, XtraPar1,
										 symbolRate);
			if (dr != DTAPI_OK)
			{
				DtapiResult2Str(dr);
			}
		}
	}

	if (modType != DTAPI_MOD_IQDIRECT)
	{
		// Set the receive mode
		dr = m_DtInp.SetRxMode(rxMode);
#ifdef _DEBUG
		if (dr != DTAPI_OK)
		{
			std::cout << "RxMode set failure." << std::endl;
			DtapiResult2Str(dr);
		}
#endif
	}
	return DTAPI_OK;
}

int processSignal::trnmt_startup_msg()
{
	/* Description:
	At the time of the startup, transmit a string. So that
	the GS operator may double check the received commands.
	current format is,
	"xxxxxxx: version,yy,demod_type,xyz,freq,zz*/
	std::string msgToBeSent;
	char* msgToBeSent_ptr = new char[512];
	
	msgToBeSent.append(std::to_string(deviceSerialNumber));
	
	msgToBeSent.append(":version,");
	msgToBeSent.append(std::to_string(RECORDER_VERSION_MAJOR));
	msgToBeSent.append(".");
	msgToBeSent.append(std::to_string(RECORDER_VERSION_MINOR));
	msgToBeSent.append(".");
	msgToBeSent.append(std::to_string(RECORDER_VERSION_BUGFIX));
	
	msgToBeSent.append(",demod_scheme,");
	switch (modType)
	{
		case DTAPI_MOD_DVBS2:
			msgToBeSent.append("DTAPI_MOD_DVBS2");
			break;
		case DTAPI_MOD_DVBS2_QPSK:
			msgToBeSent.append("DTAPI_MOD_DVBS2_QPSK");
			break; 
		case DTAPI_MOD_DVBS2_8PSK:
			msgToBeSent.append("DTAPI_MOD_DVBS2_8PSK");
			break;
		case DTAPI_MOD_DVBS2_16APSK:
			msgToBeSent.append("DTAPI_MOD_DVBS2_16PSK");
			break;
		case DTAPI_MOD_DVBS2_32APSK:
			msgToBeSent.append("DTAPI_MOD_DVBS2_32PSK");
			break;
		default:
			msgToBeSent.append("unknown");
			break;
	}

	msgToBeSent.append(",frequency,"); 
	msgToBeSent.append(std::to_string(carrierFreq));

#ifdef _DEBUG
	std::cout << msgToBeSent << std::endl;
#endif

	// make it c_str
	strcpy_s(msgToBeSent_ptr, msgToBeSent.size() + 1, msgToBeSent.c_str());

	// UDP tasks...
	auto objRef = MySocketClass();
	objRef.sendData(msgToBeSent_ptr, (int)msgToBeSent.size() + 1);
	std::this_thread::sleep_for(std::chrono::milliseconds(5));
	objRef.receiveSomething();
	delete[] msgToBeSent_ptr;
	return 0;
}

int udp_heartbeat(std::atomic<bool> &keep_alive_var, DtStatistic *statistics_ptr, int statistics_count, int sn)
{
	// this function is intended to run in an independant thread
	// std::cout << "\tHeartBeat" << std::endl;
	// first, convert the int values in a string.
	std::string msgToBeSent;
	char *msgToBeSent_ptr = (char*)malloc(512);
	msgToBeSent.append(std::to_string(sn));
	msgToBeSent.append(":lock,");
	msgToBeSent.append(statistics_ptr[0].m_ValueBool ? "yes" : "no");
	if (statistics_ptr[0].m_ValueBool)
	{
		msgToBeSent.append(",snr[dB],");
		double snr_in_db = statistics_ptr[1].m_ValueInt * .1;
		msgToBeSent.append(std::to_string(snr_in_db));
		msgToBeSent.append(",RF_level_ch(dBm),");
		double dBmVx10 = (double)statistics_ptr[2].m_ValueInt;
		double dBm = (dBmVx10 / 10) - 48.7506; // dBm(75ohm)
		msgToBeSent.append(std::to_string(dBm));
	}
#ifdef _DEBUG
	std::cout << msgToBeSent << std::endl;
#endif

	// make it c_str
	strcpy_s(msgToBeSent_ptr, msgToBeSent.size() + 1, msgToBeSent.c_str());

	// UDP tasks...
	auto objRef = MySocketClass();
	objRef.sendData(msgToBeSent_ptr, (int)msgToBeSent.size() + 1);
	std::this_thread::sleep_for(std::chrono::milliseconds(5));
	keep_alive_var = objRef.receiveSomething();
	// std::cout << "udp data sent\n";
	free(msgToBeSent_ptr);
	return 0;
}

Dtapi::DTAPI_RESULT processSignal::setMaxSize(__int64 size)
{
	// todo check if we actually have 'size' available
	// on our HDD

	maxSize = size;
	return DTAPI_OK;
}

void processSignal::f_recordFile(std::queue<s> &dataToProcess,
								 std::mutex &mut,
								 std::condition_variable &cv,
								 std::atomic<bool> &finished)
{
#ifdef _DEBUG
	std::cout << "processSignal::f_recordFile" << std::endl;
#endif
	int VidStd = -1, SdiSubValue = -1; // DTAPI_VIDSTD_UNKNOWN;
	int FifoLoad, NumToRead;
	bool ValidSignal = false, HwFifoOvf = false;
	__int64 NumBytesStored = 0; // Number bytes stored in file
	std::atomic<bool> shoul_i_stay_alive = true;
	DtStatistic statisticData[number_of_live_feedback];

	// Set the statistics id
	statisticData[0].SetId(DTAPI_STAT_LOCK); // result is bool
	statisticData[1].SetId(DTAPI_STAT_SNR);
	statisticData[1].m_IdXtra[0] = 0;
	statisticData[2].SetId(DTAPI_STAT_RFLVL_CHAN);

	// startup message
	trnmt_startup_msg();
	//-.-.-.-.-.-.-.-.-.-.-.-.-.- First loop wait for a signal -.-.-.-.-.-.-.-.-.-.-.-.-.-
	while (!_kbhit() && !ValidSignal)
	{
		// ###
		// Fixing https://github.com/i-qps/dektec_demod/issues/10
		// Since this loop maybe an infinity loop,
		// adding a udp IPC to terminate the process.
		if (!shoul_i_stay_alive)
		{
#ifdef _DEBUG
			std::cout << "support function::recordfile: UDP said to quit." << std::endl;
#endif
			break;
		}
		std::thread t2(udp_heartbeat, std::ref(shoul_i_stay_alive), statisticData, number_of_live_feedback, deviceSerialNumber);
		t2.join();
		// ###

#ifdef WINBUILD
		Sleep(500); // Give the demodulator some time to tune and lock
#else
		usleep(500000); // Give the demodulator some time to tune and lock
#endif
		int CarrierDetect, n;
#ifdef _DEBUG
		std::cout << "waiting for valid signal" << std::endl;
#endif
		if (m_Demod)
		{

			bool RLock;
			// Check if demod has a valid signal
			dr = m_DtInp.GetStatistic(DTAPI_STAT_LOCK, RLock);
			// if (dr != DTAPI_OK)
			// throw Exc(c_ErrFailGetDemodStatus, ::DtapiResult2Str(dr));
			if (!RLock)
				continue;
		}
		else
		{
			dr = m_DtInp.GetStatus(n, n, CarrierDetect, n, n, n);
			// Check for valid carrier
			if (CarrierDetect != DTAPI_CLKDET_OK)
				continue;
		}
		ValidSignal = true;
	}
	if (!ValidSignal)
	{
		while (_kbhit())
		{
			_getch();
		} // bug-fix.
		// while testing, if you quit the program by hitting keyboard,
		// next time you cannot start the process by mqtt. because kbhit return non-zero.
		// so clear it before next use.
		return; // No signal found i.e. user must have pressed a key to exit loop
	}
	// For SDI mode we need to add a dtsdi file header

	// Init alignment
	if ((rxMode & DTAPI_RXMODE_TS_MASK) == DTAPI_RXMODE_ST204)
	{
		m_Align = 204; // Align on multiples of 204 bytes
					   // Do we need to account for timestamps?
		if ((rxMode & DTAPI_RXMODE_TIMESTAMP32) != 0)
			m_Align += 4;
	}
	else if ((rxMode & DTAPI_RXMODE_TS_MASK) == DTAPI_RXMODE_STRAW)
		m_Align = 4; // Align on multiples of 4 bytes
	else if ((rxMode & DTAPI_RXMODE_TS_MASK) == DTAPI_RXMODE_STTRP)
		m_Align = 212; // Align on multiples of 212 bytes (TRP-packet + timestamp)
	else
	{
		m_Align = 188; // Align on multiples of 188 bytes
					   // Do we need to account for timestamps?
		if ((rxMode & DTAPI_RXMODE_TIMESTAMP32) != 0)
			m_Align += 4;
	}
#ifdef _DEBUG
	std::cout << "alignment complete. starting reception." << std::endl;
#endif
	// Start reception
	dr = m_DtInp.SetRxControl(DTAPI_RXCTRL_RCV);
	if (dr != DTAPI_OK)
	{
#ifdef _DEBUG
		std::cout << "cannot receive. exiting." << std::endl;
#endif
		return;
	}

	auto time_before = std::chrono::high_resolution_clock::now();
	auto time_now = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> elapsed_time = time_now - time_before; // ms

	//-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Second loop record data -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
	while (!_kbhit())
	{
		// communicate with UDP server
		time_now = std::chrono::high_resolution_clock::now();
		elapsed_time = time_now - time_before; // ms

		if (elapsed_time.count() > heartbeat_period_sec)
		{
			if (!shoul_i_stay_alive)
			{
#ifdef _DEBUG
				printf("main loop -> atomic bool ended the process. \n");
#endif
				break;
			}
			// update the statistics
			dr = m_DtInp.GetStatistics(number_of_live_feedback, statisticData);
			if (dr != DTAPI_OK)
			{
#ifdef _DEBUG
				::DtapiResult2Str(dr);
				std::cout << "Get statistics failed." << std::endl;
#endif
				continue;
			}
			std::thread t1(udp_heartbeat, std::ref(shoul_i_stay_alive), statisticData, number_of_live_feedback, deviceSerialNumber);
			t1.detach();
			time_before = time_now;
		}
		// auto start = std::chrono::high_resolution_clock::now();
		// std::cout << "receiving. press any key to exit...\r";
		// std::cout.flush();
		//  Get FIFO load. Continue if too few bytes available in receive FIFO
		dr = m_DtInp.GetFifoLoad(FifoLoad);
		if (FifoLoad < 2 * 1024 * 1024)
		{
			// Sleep 1ms, before checking load again
#ifdef WINBUILD
			Sleep(1);
#else
			usleep(1000);
#endif
			continue;
		}

		// Read aligned number of data from input channel
		NumToRead = (min(FifoLoad, c_BufSize) / m_Align) * m_Align;
		dr = m_DtInp.Read(m_pBuf, NumToRead);
#ifdef _DEBUG
		if (dr != DTAPI_OK)
		{

			std::cout << "fbuuffer read error." << std::endl;
		}
#endif

		// Compute number of bytes left to store
		__int64 NumLeftToStore;
		if (maxSize <= 0)
			NumLeftToStore = NumToRead + 1; // No max => we can store all
		else
			NumLeftToStore = (maxSize * 1024 * 1024) - NumBytesStored;

		// Compute number of bytes to write to file
		int NumToWrite = 0;

		// Account our allignment requirements
		NumLeftToStore = ((NumLeftToStore + m_Align - 1) / m_Align) * m_Align;

		NumToWrite = (NumLeftToStore >= NumToRead) ? NumToRead : (int)NumLeftToStore;

		// copy the data into the first queue
		// but first, create a scope for the mutex lock
		{
			std::lock_guard<std::mutex> lock{mut};
			// auto mutex_after = std::chrono::high_resolution_clock::now();
			// std::chrono::duration<double, std::milli> elapsed_mutex = mutex_after - mutex_before;
			// std::cout << "Waited " << elapsed_mutex.count() << " ms\n";

			dataToProcess.emplace(new char[NumToWrite], NumToWrite);
			memcpy(dataToProcess.back().pointerToByte, m_pBuf, NumToWrite);

			cv.notify_all();
		}
#ifdef _DEBUG
		// Write data to our file

		// Update number stored
		NumBytesStored += NumToWrite;
		// printf("\r%d MB written", NumBytesStored/(1024*1024));        // debug only

		// Should we stop?
		NumLeftToStore -= NumToWrite;
		if (NumLeftToStore <= 0)
			break; // Reached max size => exit record loop
#endif
		// Check for overflow (only report overflow once)
		int Flags = 0, Latched = 0;
		dr = m_DtInp.GetFlags(Flags, Latched);
		if (0 != (Latched & DTAPI_RX_FIFO_OVF) && !HwFifoOvf)
		{
			// LogF(c_WarnHwFifoOverflow); todo
			HwFifoOvf = true; // Only log warning once
		}
		dr = m_DtInp.ClearFlags(Latched);
	}
	finished = true;
	cv.notify_all();
}

DTAPI_RESULT processSignal::f_recordSignal(
	std::queue<s> &dataToProcess,
	std::mutex &mut,
	std::condition_variable &cv,
	std::atomic<bool> &finished)
{
#ifdef _DEBUG
	std::cout << "Serial " << deviceSerialNumber << std::endl;
	std::cout << "modType " << modType << std::endl;
	std::cout << "freq " << carrierFreq << std::endl;
	std::cout << "rxMode " << rxMode << std::endl;
#endif
	try
	{
#ifdef _DEBUG
		m_pFile = fopen(filename_c, "wb");
#endif
		dr = f_attachToInput();
		if (dr != DTAPI_OK)
		{
#ifdef _DEBUG
			std::cout << "attachment was not successful" << std::endl;
#endif
			return dr;
		}
		f_initInput();
		// Create our transfer buffer
		m_pBuf = new char[c_BufSize];
		f_recordFile(
			std::ref(dataToProcess),
			std::ref(mut),
			std::ref(cv),
			std::ref(finished));
	}
	catch (...)
	{
#ifdef _DEBUG
		std::cout << "unknown exception occured." << std::endl;
#endif
		// todo
	}
	// Detach from channel and device
	m_DtInp.SetRxControl(DTAPI_RXCTRL_IDLE);
	m_DtInp.Detach(DTAPI_INSTANT_DETACH);
	m_DtDvc.Detach();

	// Free our buffer
	if (m_pBuf != NULL)
	{
		delete[] m_pBuf;
		m_pBuf = NULL;
	}

	// donot forget to close our file
	if (m_pFile != NULL)
	{
		::fclose(m_pFile);
		m_pFile = NULL;
	}

	return DTAPI_OK;
}

DTAPI_RESULT processSignal::f_printStatistics()
{
	DtStatistic statisticData[8];
	Dtapi::DTAPI_RESULT retVal = DTAPI_OK;
	int FifoLoad, NumToRead;

#ifdef _DEBUG
	// debug
	std::cout << "Serial " << deviceSerialNumber << std::endl;
	std::cout << "modType " << modType << std::endl;
	std::cout << "freq " << carrierFreq << std::endl;
	std::cout << "rxMode " << rxMode << std::endl;
	// end of debug
#endif

	dr = f_attachToInput();
	ifNotGoodPrint(dr);
	if (dr != DTAPI_OK)
	{
		return dr;
	}
	f_initInput();
	// todo if attach input fails, who calss detach input.
	// need to let go of the device.

	// overall signal lock.
	statisticData[0].SetId(DTAPI_STAT_LOCK);
	statisticData[0].m_IdXtra[0] = 0;
	// carrier lock:
	statisticData[1].SetId(DTAPI_STAT_CARRIER_LOCK);
	statisticData[1].m_IdXtra[0] = 0;
	// SNR:
	statisticData[2].SetId(DTAPI_STAT_SNR);
	statisticData[2].m_IdXtra[0] = 0;

	// temp
	statisticData[3].SetId(DTAPI_STAT_TEMP_TUNER);
	// result is int.

	statisticData[4].SetId(DTAPI_STAT_RFLVL_CHAN_QS);
	// result is int. multiply by 0.1 dbmv.

	statisticData[5].SetId(DTAPI_STAT_RFLVL_NARROW_QS);
	// result is int. multiply by 0.1 dbmv.

	statisticData[6].SetId(DTAPI_STAT_PACKET_LOCK);
	// result is bool

	statisticData[7].SetId(DTAPI_STAT_STREAM_LOCK);
	// result is bool

	int attempt = 0;
	while (attempt < 900)
	{
		// fetch the statistics
		dr = m_DtInp.GetStatistics(8, statisticData);
		if (dr != DTAPI_OK)
		{
#ifdef _DEBUG
			::DtapiResult2Str(dr);
			std::cout << "Get statistics failed." << std::endl;
#endif
			retVal = DTAPI_E;
		}
		else
		{

			// print out the statistics
			// 0 overall lock 1 carrier lock 2 is snr
			if (statisticData[0].m_ValueBool)
			{
				// clear buffer. but if you call clear(), rx mode will be set to idle.
				// so just read it. and do nothing with the data.
				m_DtInp.GetFifoLoad(FifoLoad);
				NumToRead = min(FifoLoad, c_BufSize);
				m_DtInp.Read(m_pBuf, NumToRead); // this will clear the buffer.

				std::cout << "Overall lock ok. " << std::endl;
				std::cout << "SNR: " << statisticData[2].m_ValueInt << std::endl;
				std::cout << "Temperature:" << statisticData[3].m_ValueInt << std::endl;
				std::cout << "was it ok? " << statisticData[3].m_Result;
				std::cout << " value type: " << statisticData[3].m_ValueType;
				std::cout << "RFLVL_CHAN:" << statisticData[4].m_ValueInt << "dBmV" << std::endl;
				std::cout << "RFLVL_NARRO:" << statisticData[5].m_ValueInt << "dBmV" << std::endl;
				if (statisticData[6].m_ValueBool)
				{
					std::cout << "STAT_PACKET_LOCK" << std::endl;
				}
				else
				{
					std::cout << "failed stat_packet_lock" << std::endl;
				}
				if (statisticData[7].m_ValueBool)
				{
					std::cout << "STAT_STREAM_LOCK" << std::endl;
				}
				else
				{
					std::cout << "failed stat_stream_lock" << std::endl
							  << std::endl;
				}

				/* notes
				m_valueType can be STAT_VT_INT or STAT_VT_bool etc*/
			}
			else
			{
				std::cout << "Waiting for overall lock. " << std::endl;
			}
			if (statisticData[1].m_ValueBool)
			{
				std::cout << "Carrier lock ok. " << std::endl;
			}
			else
			{
				std::cout << "Waiting for carrier lock. " << std::endl;
			}
		}
		// sleep for 1 sec
		std::this_thread::sleep_for(std::chrono::microseconds(200));
		attempt++;
	}
	// Detach from channel and device
	m_DtInp.SetRxControl(DTAPI_RXCTRL_IDLE);
	m_DtInp.Detach(DTAPI_INSTANT_DETACH);
	m_DtDvc.Detach();
	std::cout << "Print statiscs done. device detached." << std::endl;
	return retVal;
}

// for log file
/*
auto somoy = std::chrono::system_clock::now();

time_t tt = std::chrono::system_clock::to_time_t(somoy);
struct tm now_gmt {};
struct tm now_local {};
gmtime_s(&now_gmt, &tt);
localtime_s(&now_local, &tt);

std::string logFileName = "log";
logFileName.append(std::to_string(now_gmt.tm_mon + 1));
logFileName.append(std::to_string(now_gmt.tm_mday));
logFileName.append(".txt");

std::string message = "first message";
std::string message2 = "second message";
messageLogger logg(logFileName);
logg.logMessage(message);
logg.logMessage(message2);

*/

void f_showHelp()
{
	std::cout << std::endl
			  << "Usage: " << std::endl;

	std::cout << "-help\t: Show this menu." << std::endl;
	std::cout << "-record\t: Start the demodulator in recording mode." << std::endl;
	std::cout << "-listDev\t: List all the available DekTek boards." << std::endl;
	std::cout << "-devStatus\t: Show device status of a speified board." << std::endl
			  << std::endl;

	std::cout << "-modType\t: Modulation type of the expected signal." << std::endl;
	std::cout << "-carrierFreq\t: Carrier frequency of the expected signal." << std::endl;
	std::cout << "-receiveMode\t: Specify receiving mode for the expected signal. " << std::endl;
	std::cout << "-filename\t: filename for the recorded signal." << std::endl;
	std::cout << "-sps\t: (Optional) To set symbol rate manually. Has to be between 500000 and 72000000. (500k ~ 72M)" << std::endl
			  << std::endl;

	std::cout << "------- Example usage ------------------" << std::endl;
	std::cout << "dektec_recorder.exe -listDev" << std::endl;
	std::cout << "dektec_recorder.exe -devStatus -sn 2137012053 -modType DVBS2_16APSK -carrierFreq 1120 -receiveMode STL3" << std::endl;
	std::cout << "dektec_recorder.exe -record -modType DVBS2_16APSK -carrierFreq 1120 -receiveMode STL3 -sn 2137012053 -filename test.l3" << std::endl
			  << std::endl;
}