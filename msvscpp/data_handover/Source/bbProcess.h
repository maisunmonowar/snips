#pragma once
#ifndef __bbProcess_h__
#define __bbProcess_h__

#include "tfProcess.h"


class bbProcessor
{
public:
	
	
	void stripBB(
		std::queue<DataChunk> &dataToProcess,
		std::mutex &mut,
		std::condition_variable &cv,
		std::atomic<bool> &finished);

};
#endif