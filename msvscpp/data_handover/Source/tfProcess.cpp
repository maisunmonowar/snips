#include "tfProcess.h"
void tfProcessor::sanitizeTf(
	std::queue<s> &q_tf,
	std::mutex &mut_tf,
	std::condition_variable &cv_tf,
	std::atomic<bool> &ab_tf)
{
	bool somethingWaitingToBeProcessed = false;
	char *buf_tf = new char[c_BufSize * 2];
	char *buf_sanitized = new char[c_BufSize * 2];
	int *buf_tf_len = new int;
	int *buf_sanitized_len = new int;

#ifdef _DEBUG
	std::cout << "\t\t\t\t tf::filename_c: " << filename_c << std::endl;
#endif
	while (true)
	{
		{
			// dedicated scope for the mutex
			std::unique_lock<std::mutex> lock{mut_tf};
			cv_tf.wait(lock, [&]
					   { return !q_tf.empty() || ab_tf; });

			if (!q_tf.empty())
			{
				// copy to local buffer
				// void * memcpy ( void * destination, const void * source, size_t num )
				memcpy(buf_tf, q_tf.front().pointerToByte, q_tf.front().numOfByte);
				*buf_tf_len = q_tf.front().numOfByte;
				delete q_tf.front().pointerToByte; // deallocate the heap memory.
				q_tf.pop();
			} // if !data.empty()
		}	  // mutex is out of scope
		
		if (ab_tf)
		{
			break;
		}
	} // end of infinite loop
	delete[] buf_tf;
	delete[] buf_sanitized;
	delete buf_tf_len;
	delete buf_sanitized_len;
	std::cout << "\t\t\t\t recorder.exe::tf_proc: CRC Passed = " 
		<< processing_results.crc_passed
		<< "  CRC Failed = " 
		<< processing_results.crc_failed << std::endl;
} // end of sanitize_tf