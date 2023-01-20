#include "u.h"
#include <chrono>
#include <thread>
#include <iostream>

int main()
{
	auto objRef = MySocketClass();
	std::string msgToBeSent = "";
	msgToBeSent.append(": Exiting main(). Filesize: ");
	//std::cout << msgToBeSent << std::endl;
	char* msgToBeSent_ptr = new char[512];
	// make it c_str
	strcpy_s(msgToBeSent_ptr, msgToBeSent.size() + 1, msgToBeSent.c_str());
	//std::cout << "string copy done " << std::endl;
	objRef.sendData(msgToBeSent_ptr, (int)msgToBeSent.size() + 1);
	//std::cout << "udp sent" << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(5));
	//std::cout << "wake up" << std::endl;
	objRef.receiveSomething(); // to clear incoming buffer. there will be ack from the other side.
	//std::cout << "receivsomejasldfj" << std::endl;
	delete[] msgToBeSent_ptr;
	return 0;
}