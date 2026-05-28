#pragma once
#ifndef __bbProcess_h__
#define __bbProcess_h__

#include "tfProcess.h"


class bbProcessor
{
public:
	char *filename_c = nullptr;
	
	void stripBB(
		std::queue<custom_struct_for_queue> &dataToProcess,
		std::mutex &mut,
		std::condition_variable &cv,
		std::atomic<bool> &finished);
	

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