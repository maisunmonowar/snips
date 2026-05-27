#include "main_file.h"
#include <iostream>
#include <stdio.h>
#include "bbProcess.h"
#include <thread>
#include <mutex>


#define NOMINMAX
#include <Windows.h>
#include <conio.h>

main(int argc, char *argv[]) {
	auto time_start = std::chrono::high_resolution_clock::now();
	std::ifstream inputFile_testData;
	if (argc < 2)
	{
		inputFile_testData.open("test.l3", std::ifstream::binary);
	}
	else
	{
		inputFile_testData.open(argv[2], std::ifstream::binary);
	}
	
	int buffer_readFromHw_len = c_BufSize; // buffer length used to feed bbProcess. 
	if (inputFile_testData) 
	{
		// get length of file:
		inputFile_testData.seekg(0, inputFile_testData.end);
		auto lengthofFile = inputFile_testData.tellg();
		int actuallyRead;
		inputFile_testData.seekg(0, inputFile_testData.beg);
		auto currentPos = inputFile_testData.tellg();

		char * buffer_readFromHW = new char[buffer_readFromHw_len];

		// Initialize the queue and other variables. 
		std::queue<custom_struct_for_queue> q_main2support;
		std::condition_variable cv_main2support;
		std::mutex mut_main2support;
		std::atomic<bool> ab_main2support = false;
		
		// initialize worker thread to strip BB Header
		auto myobj = bbProcessor();
		myobj.filename_c = "output.bin\0"; // This file with have the cleaned up TF packets.
 		std::thread bbHeaderStripper = std::thread(
			&bbProcessor::stripBB,
			&myobj,
			std::ref(q_main2support),
			std::ref(mut_main2support),
			std::ref(cv_main2support),
			std::ref(ab_main2support));
		// loop through and send the data from file.
		for(int ii; ii++; ii < 5;)
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
			
			
				inputFile_testData.read(buffer_readFromHW, buffer_readFromHw_len); 
		

			if (!inputFile_testData){break;}
			{
				// seperate scope for mutex lock.
				std::lock_guard<std::mutex> lock{ mut_main2support };
				q_main2support.emplace(new char[buffer_readFromHw_len], actuallyRead);
				memcpy(q_main2support.back().pointerToByte, buffer_readFromHW, actuallyRead);
				cv_main2support.notify_all();
			}	 
		// mutex is now free. since lock is out of scope. 
		 
		// we are all done. cleanup.
		ab_main2support = false; 
		cv_main2support.notify_all();
		std::cout << "waiting for bbProcess thread to finish. \n";
		bbHeaderStripper.join();
		inputFile_testData.close();
		delete[] buffer_readFromHW;
	}
	else
	{
		std::cout << "Something wrong with the file. ";
	}
	auto time_end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_seconds = time_end - time_start;
        std::cout << "Time elapse: " << elapsed_seconds.count() << " seconds" << std::endl;

	return 0;
}