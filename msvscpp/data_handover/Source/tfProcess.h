#ifndef __TFPROCESS_H__
#define __TFPROCESS_H__

class tfProcessor
{
public:
	char *filename_c;
	int add(int a, int b);
	uint16_t crc(char *buffer, int buffer_len);
	void sanitizeTf(
		std::queue<custom_struct_for_queue> &q_tf,
		std::mutex &mut_tf,
		std::condition_variable &cv_tf,
		std::atomic<bool> &ab_tf);

	void tfWork(
		char *buffer_dirty,
		int *buffer_dirty_len,
		char *buffer_clean,
		int *buffer_clean_len);

private:
	char *dataStorage = new char[c_BufSize * 2];
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