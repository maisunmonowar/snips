#include "tfProcess.h"


void tfProcessor::sanitizeTf(
	std::queue<DataChunk> &q_tf,
	std::mutex &mut_tf,
	std::condition_variable &cv_tf,
	std::atomic<bool> &ab_tf)
{
	bool somethingWaitingToBeProcessed = false;

	while (true)
	{
          DataChunk tfChunk;
		{
			// dedicated scope for the mutex
			std::unique_lock<std::mutex> lock{mut_tf};
			cv_tf.wait(lock, [&]
					   { return !q_tf.empty() || ab_tf; });

			if (!q_tf.empty())
			{
                          tfChunk = std::move(q_tf.front());				
				q_tf.pop();
                                std::cout << " TF Process says: ";
                                std::cout.write(tfChunk.data(), tfChunk.size());
                                std::cout << std::endl;
								// note: A little bit of delay in TF process is acceptable.
			} // if !data.empty()
		}	  // mutex is out of scope
		
		if (ab_tf.load() && q_tf.empty())
		{
			break;
		}
	} // end of infinite loop
	
} // end of sanitize_tf