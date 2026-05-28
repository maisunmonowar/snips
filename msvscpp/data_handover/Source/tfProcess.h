#ifndef __TFPROCESS_H__
#define __TFPROCESS_H__
#include "common.h"
class tfProcessor {
public:
	char *filename_c;
	void sanitizeTf(
		std::queue<DataChunk> &q_tf,
		std::mutex &mut_tf,
		std::condition_variable &cv_tf,
		std::atomic<bool> &ab_tf);


};

#endif