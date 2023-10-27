#pragma once
#ifndef __cmd_parsing_h__

#define __cmd_parsing_h__
#include "DtSdiFileFmt.h"
#include "const_def.h"
#include <DTAPI.h>
#include <iostream>

// Command line option flags
const char c_listDevices[]		= "listDev";
const char c_showDeviceStatus[] = "devStatus";
const char c_modulationType[]	= "modType";		//modulation type
const char c_carrierFrequency[] = "carrierFreq";	// carrierFreqency
const char c_receiveMode[]		= "receiveMode";	// receiveMode
const char c_filename[]			= "filename";
const char c_HelpCmdLineFlag[]	= "help";
const char c_record[]			= "record";
const char c_version[]			= "version";
const char c_sps[]              = "sps";
#endif