#include "u.h"
#include <chrono>
#include <thread>
#include <iostream>
#include <random>
#include <string>

class FakeRadioInterface {
public:
	FakeRadioInterface(MySocketClass &ref_to_udp_client) : client(ref_to_udp_client) {}

	int work() {
		using namespace std::chrono;
		std::mt19937 rng((unsigned)std::chrono::high_resolution_clock::now().time_since_epoch().count());
		std::uniform_int_distribution<int> snr_dist(0, 40);

		auto last_time = steady_clock::now();
		while (true) {
			if (client.should_i_quit()) {
				std::cout << "FakeRadioInterface: detected quit flag. Exiting work()." << std::endl;
				return 0;
			}

			// send status every ~1s
			auto now = steady_clock::now();
			if (duration_cast<seconds>(now - last_time).count() >= 1) {
				int snr = snr_dist(rng);
				std::string msg = "snr:" + std::to_string(snr);
				std::cout << "FakeRadioInterface: sending -> " << msg << std::endl;
				// send
				std::string sendbuf = msg;
				client.sendData(const_cast<char*>(sendbuf.c_str()), (int)sendbuf.size());
				// attempt to receive server response (may be timeout)
				client.receiveSomething();

				last_time = now;
			} else {
				// small sleep to avoid busy loop
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
		}
	}

private:
	MySocketClass &client;
};


int main()
{
	/*
	Simulate radio driver sending periodic status (snr).
	Client should stop when server replies "terminate".
	*/
	auto objRef = MySocketClass();
	if (objRef.error_status_self != MySocketClass::all_ok) {
		std::cerr << "Socket init failed. Exiting." << std::endl;
		return -1;
	}
	auto myRadio = FakeRadioInterface(objRef);
	return myRadio.work();
}