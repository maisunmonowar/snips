#include <chrono>
#include <iostream>

int
main()
{
	std::chrono::time_point<std::chrono::system_clock> time_point;
	time_point = std::chrono::system_clock::now(); // basiclly timestamp. but not an int
	auto c_time_int = std::chrono::duration_cast<std::chrono::seconds>(time_point.time_since_epoch()).count(); //this is an int
	//std::cout << time_point << std::endl; 
	std::time_t ttp = std::chrono::system_clock::to_time_t(time_point);
	char chr[50];
	errno_t e = ctime_s(chr, 50, &ttp);
	if (e) std::cout << "Error." << std::endl;
	else std::cout << chr << std::endl;
	return 0;
}