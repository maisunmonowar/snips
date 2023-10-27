#ifndef __bbProcess_h__
#define __bbProcess_h__

#include "const_def.h"

class bbProcessor
{
public:
	char* filename_c = nullptr;
	int substract(int a, int b);
	void stripBB(
		std::queue<s>& dataToProcess,
		std::mutex& mut,
		std::condition_variable& cv,
		std::atomic<bool>& finished);
	void work(
		char* buffer_withHeader,
		int* buffer_withHeader_len,
		char* buffer_withoutHeader,
		int* buffer_withoutHeader_len,
		bool enableLogging);
private:

	int buffer_incompleteData_len = 0;
	char* buffer_incompleteData = new char[c_BufSize * 2]; // size is todo to revised
	// the reason the buffer is declared here globally
	// instead of bbProcess.h is that,
	// I don't want the memory to be accessable by the 
	// other files. 
	uint8_t prev_bbFrameID = 0; // todo clean up. this is for debug purpose. 

};

#endif