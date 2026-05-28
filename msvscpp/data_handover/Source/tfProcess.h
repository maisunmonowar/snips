#ifndef __TFPROCESS_H__
#define __TFPROCESS_H__
#include "common.h"
class tfProcessor {
public:
	char *filename_c;
	void sanitizeTf(
		std::queue<custom_struct_for_queue> &q_tf,
		std::mutex &mut_tf,
		std::condition_variable &cv_tf,
		std::atomic<bool> &ab_tf);


private:
	char *dataStorage = new char[30];
	int *dataStorage_len = new int(0);
	// the reason I'm not declaring this in the header file,
	// is that, I do not want this memory accessible by other programs.
	struct bbframe_results
	{
		int crc_passed;
		int crc_failed;
	} processing_results;
};

#endif