#include "main_file.h"

int main(int argc, char *argv[]) {
		int actuallyRead= 13;
		
		const char * buffer_readFromHW = "Hello world!\0";

		// Initialize the queue and other variables. 
		std::queue<DataChunk> q_main2support;
		std::condition_variable cv_main2support;
		std::mutex mut_main2support;
		std::atomic<bool> ab_main2support = true;
		
		// initialize worker thread to strip BB Header
		auto myobj = bbProcessor();
		std::thread bbHeaderStripper = std::thread(
			&bbProcessor::stripBB,
			&myobj,
			std::ref(q_main2support),
			std::ref(mut_main2support),
			std::ref(cv_main2support),
			std::ref(ab_main2support));
		// loop through and send the data from file.
		for(int ii=0; ii<5; ii++) // just to simulate reading HW buffer few times.
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
			{
				// seperate scope for mutex lock.
				std::lock_guard<std::mutex> lock{ mut_main2support };
				// assume data has been read from HW buffer already.
                                std::vector<char> chunk(actuallyRead); // i think this is the right place to, since are going to read new data from hw buffer.
							// note: todo i need to verify seperately if I can use chunk.data() directly to read the data from buffer.
                                std::memcpy(chunk.data(), buffer_readFromHW,
                                            actuallyRead); // simulating copying hw buffer data to a vector. hence 
								// transfering data to a memory safe container. 

								// note: Hardware buffer has the tendency to overflow.
								// try to grab data from hardware data as quick as possible. 
				q_main2support.emplace(std::move(chunk));

				cv_main2support.notify_all();
			}	 
		// mutex is now free. since lock is out of scope. 
		}
		// we are all done. cleanup.
		

		{
                        std::lock_guard<std::mutex> lock{mut_main2support};
                        ab_main2support = false;
                        cv_main2support
                            .notify_all(); // Safe! Protected by the mutex.
                }
		std::cout << "waiting for bbProcess thread to finish. \n";
		bbHeaderStripper.join();
	return 0;
}
