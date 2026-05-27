#pragma once
#ifndef __bbProcess_h__
#define __bbProcess_h__

class bbProcessor
{
	// bbProcessor or Base Band Processor's job is strip
	// away the base band header and pass rest of the 
	// the date to tfProcessor (Transfer Frame Processor)
	//
	// I forgot the actual definition and alternative 
	// names for BBFrame. Basically it is a header that
	// starts with 0xB8. It will have datafield length
	// and some other properties. 
	// For the full definition, please check my document
	// on Dektec Demodulator boards. 
	// TLDR: The protocol we are using has bunch of header.
	// Think of it like a Train. Carrying multiple container.
	// Each container having similar crates. Each crates 
	// containing standard envelopes. 
	// It is this abstraction created by the CCSDS protocol
	// that gives users the flexibility of arranging their data.
	// And at the same time it gives the service providers
	// the necessary tools to route data to their intended 
	// receipients. 
public:
	char *filename_c = nullptr;
	int substract(int a, int b);
	void stripBB(
		std::queue<custom_struct_for_queue> &dataToProcess,
		std::mutex &mut,
		std::condition_variable &cv,
		std::atomic<bool> &finished);
	void work(
		char *buffer_withHeader,
		int *buffer_withHeader_len,
		char *buffer_withoutHeader,
		int *buffer_withoutHeader_len,
		bool enableLogging);

private:
	int buffer_incompleteData_len = 0;
	char *buffer_incompleteData = new char[c_BufSize * 2]; // size is todo to revised
	// the reason the buffer is declared here globally
	// instead of bbProcess.h is that,
	// I don't want the memory to be accessable by the
	// other files.
	uint8_t prev_bbFrameID = 0; // todo clean up. this is for debug purpose.
	struct
	{
		int missing_bbframe;
		bool first_loop; // flag. in the first loop, there is no prev_bb_id.
	} processing_results;
};

#endif