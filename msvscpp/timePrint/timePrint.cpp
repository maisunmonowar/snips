#include <chrono>
#include <iostream>

int
main()
{
	std::chrono::time_point<std::chrono::system_clock> time_point;
	time_point = std::chrono::system_clock::now();
	std::time_t ttp = std::chrono::system_clock::to_time_t(time_point);
	char chr[50];
	errno_t e = ctime_s(chr, 50, &ttp);
	if (e) std::cout << "Error." << std::endl;
	else std::cout << chr << std::endl;
}