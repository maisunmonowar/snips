#include <chrono>
#include <iostream>
#include <thread>


void main()
{
	auto time_before = std::chrono::high_resolution_clock::now();
	std::this_thread::sleep_for(std::chrono::seconds(1));
	auto time_now = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> elapsed_time = time_now - time_before; // ms
	std::cout << "Time difference: " << elapsed_time.count() << " ms" << std::endl;
}