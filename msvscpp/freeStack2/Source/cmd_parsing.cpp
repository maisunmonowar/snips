#include "cmd_parsing.h"
#include <stdio.h>
#include <nlohmann/json.hpp>
#include <string>
#include <fstream>

CommandLineParams::CommandLineParams() {}

CommandLineParams::~CommandLineParams() {}

int CommandLineParams::getSymbolRate()
{
	return userSymbolRate;
}

mode_switch CommandLineParams::ParseCommandLine(
	int argc,
	char *argv[],
	int *deviceSerialNumber,
	int *modulationType,
	double *carrierFrequency,
	int *receiveMode)
{
	mode_switch returnValue = unknown;
	for (int i = 1; i < argc; i++)
	{
		char *pParam = argv[i];

#ifdef WINBUILD
		if ((pParam[0] == '-' || pParam[0] == '/'))
#else
		if (pParam[0] == '-') // For Linux only '-' can be used for options
#endif
		{
			++pParam; // remove flag specifier
			if (0 == strcmp(pParam, c_listDevices))
			{
				returnValue = listDevices;
			}
			else if (0 == strcmp(pParam, c_record))
			{
				returnValue = recordSignal;
			}
			else if (0 == strcmp(pParam, c_HelpCmdLineFlag))
			{
				returnValue = showHelp;
			}
			else if (0 == strcmp(pParam, c_showDeviceStatus))
			{
				returnValue = deviceStatus;
			}
			else if (0 == strcmp(pParam, c_version))
			{
				returnValue = echo_version_number;
			}
			else if (0 == strcmp(pParam, c_filename))
			{
				char *temp = argv[i + 1]; //temp now has the file name
				int temp_len = 0;
				while (*(temp+temp_len) != '\0')
				{
					temp_len++;
				}
				// get the path from json file
				using json = nlohmann::json;
				std::ifstream f("config.json");
				json data = json::parse(f);
				f.close();

				// get the path form config
				std::string pathRaw = data["data_location"].dump();
				// condition the path.
				pathRaw.erase(0, 1);
				while (pathRaw.find("\\\\") != -1)
				{
					auto posi = pathRaw.find("\\\\");
					pathRaw.erase(posi, 1);

				}
				{
					auto posi = pathRaw.find("\"");
					pathRaw.erase(posi, 1);
					pathRaw.append("\\");
				}

				// stich them together
				memcpy(filename_parsed, pathRaw.c_str(), pathRaw.size() + 1);
				memcpy(filename_parsed + pathRaw.size(), temp, temp_len+1);
			}
			else if (0 == strcmp(pParam, c_modulationType))
			{
				if (0 == strcmp(argv[i + 1], "DVBS2"))
				{
					*modulationType = DTAPI_MOD_DVBS2;
				}
				else if (0 == strcmp(argv[i + 1], "DVBC2"))
					*modulationType = DTAPI_MOD_DVBC2;
				else if (0 == strcmp(argv[i + 1], "DVBS2_QPSK"))
					*modulationType = DTAPI_MOD_DVBS2_QPSK;
				else if (0 == strcmp(argv[i + 1], "DVBS2_8PSK"))
					*modulationType = DTAPI_MOD_DVBS2_8PSK;
				else if (0 == strcmp(argv[i + 1], "DVBS2_16APSK"))
					*modulationType = DTAPI_MOD_DVBS2_16APSK;
				else if (0 == strcmp(argv[i + 1], "DVBS2_32APSK"))
					*modulationType = DTAPI_MOD_DVBS2_32APSK;
				// else raise error todo
			}
			else if ( 0== strcmp(pParam, c_sps))
			{
				userSymbolRate = atoi(argv[i + 1]);
			}
			else if (0 == strcmp(pParam, c_deviceSerialNumber))
			{
				*deviceSerialNumber = atoi(argv[i + 1]);
			}
			else if (0 == strcmp(pParam, c_carrierFrequency))
			{
				*carrierFrequency = atof(argv[i + 1]);
			}
			else if (0 == strcmp(pParam, c_receiveMode))
			{
				char *valuee = argv[i + 1];
				if (0 == strcmp(valuee, "ST188"))
					*receiveMode = DTAPI_RXMODE_ST188;
				else if (0 == strcmp(valuee, "ST204"))
					*receiveMode = DTAPI_RXMODE_ST204;
				else if (0 == strcmp(valuee, "STRAW"))
					*receiveMode = DTAPI_RXMODE_STRAW;
				else if (0 == strcmp(valuee, "STL3TS"))
					*receiveMode = DTAPI_RXMODE_STL3 | DTAPI_RXMODE_TIMESTAMP32;
				else if (0 == strcmp(valuee, "STL3"))
					*receiveMode = DTAPI_RXMODE_STL3;
				else if (0 == strcmp(valuee, "STL3FULLTS"))
					*receiveMode = DTAPI_RXMODE_STL3FULL | DTAPI_RXMODE_TIMESTAMP32;
				else if (0 == strcmp(valuee, "STL3FULL"))
					*receiveMode = DTAPI_RXMODE_STL3FULL;
				else if (0 == strcmp(valuee, "STL3ALLTS"))
					*receiveMode = DTAPI_RXMODE_STL3ALL | DTAPI_RXMODE_TIMESTAMP32;
				else if (0 == strcmp(valuee, "STL3ALL"))
					*receiveMode = DTAPI_RXMODE_STL3ALL;
				else if (0 == strcmp(valuee, "STTRP"))
					*receiveMode = DTAPI_RXMODE_STTRP | DTAPI_RXMODE_TIMESTAMP32;
				else if (0 == strcmp(valuee, "ST188T"))
					*receiveMode = DTAPI_RXMODE_ST188 | DTAPI_RXMODE_TIMESTAMP32;
				else if (0 == strcmp(valuee, "ST204T"))
					*receiveMode = DTAPI_RXMODE_ST204 | DTAPI_RXMODE_TIMESTAMP32;
				else if (0 == strcmp(valuee, "SDI10B"))
					*receiveMode = DTAPI_RXMODE_SDI_FULL | DTAPI_RXMODE_SDI_10B;
				else if (0 == strcmp(valuee, "SDI16B"))
					*receiveMode = DTAPI_RXMODE_SDI_FULL | DTAPI_RXMODE_SDI_16B;
				else if (0 == strcmp(valuee, "RAWASI"))
					*receiveMode = DTAPI_RXMODE_RAWASI;
				else if (0 == strcmp(valuee, "STMP2"))
					*receiveMode = DTAPI_RXMODE_STMP2;
				// else
				// todo throw Exc(c_CleInvalidArgument, m_pLastFlag);
			}
		}
	}
	return returnValue;
}
