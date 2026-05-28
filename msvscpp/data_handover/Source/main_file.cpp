#include "main_file.h"

int main(int argc, char *argv[]) {
		int actuallyRead= 15;
		int buffer_readFromHw_len = 15;
		const char * buffer_readFromHW = "Hello world!";

		// Initialize the queue and other variables. 
		std::queue<custom_struct_for_queue> q_main2support;
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
		for(int ii=0; ii<5; ii++)
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
			{
				// seperate scope for mutex lock.
				std::lock_guard<std::mutex> lock{ mut_main2support };
				q_main2support.emplace(new char[buffer_readFromHw_len], actuallyRead);
				memcpy(q_main2support.back().pointerToByte, buffer_readFromHW, actuallyRead); // in production, buffer_readFromHW will have data from hardware buffer. buffer type is char *. 
				// we have to receive the hardware data via a char *. but we are free to do anything before emplacing it the queue. maybe vector<char>?
				cv_main2support.notify_all();
			}	 
		// mutex is now free. since lock is out of scope. 
		}
		// we are all done. cleanup.
		ab_main2support = false; 
		cv_main2support.notify_all();
		std::cout << "waiting for bbProcess thread to finish. \n";
		bbHeaderStripper.join();
		delete[] buffer_readFromHW;
	return 0;
}
