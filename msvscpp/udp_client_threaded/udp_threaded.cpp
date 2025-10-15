#include "u.h"
#include <chrono>
#include <thread>
#include <iostream>
#include <random>
#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

class FakeRadioInterface {
public:
	// now takes references to queue/mutex/cv and finished flag
	FakeRadioInterface(std::queue<std::string> &q, std::mutex &m, std::condition_variable &cv, std::atomic<bool> &finished)
		: queue_ref(q), mutex_ref(m), cv_ref(cv), finished_ref(finished) {}

	int work() {
		// debug: print thread id where FakeRadioInterface.work runs (should be main thread)
		std::cout << "FakeRadioInterface::work() running in thread: " << std::this_thread::get_id() << std::endl;

		using namespace std::chrono;
		std::mt19937 rng((unsigned)std::chrono::high_resolution_clock::now().time_since_epoch().count());
		std::uniform_int_distribution<int> snr_dist(0, 40);

		auto last_time = steady_clock::now();
		int count = 0;
		while (!finished_ref.load()) {
			// send status every ~1s
			auto now = steady_clock::now();
			if (duration_cast<seconds>(now - last_time).count() >= 1) {
				int snr = snr_dist(rng);
				std::string msg = "snr:" + std::to_string(snr);
				std::cout << "FakeRadioInterface: enqueue -> " << msg
						  << " (thread: " << std::this_thread::get_id() << ")" << std::endl;

				{ // push to queue
					std::lock_guard<std::mutex> lg(mutex_ref);
					queue_ref.push(msg);
				}
				cv_ref.notify_one();

				last_time = now;
				++count;
			} else {
				// small sleep to avoid busy loop
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
		}
		std::cout << "FakeRadioInterface: finishing work loop in thread: " << std::this_thread::get_id()
				  << " (messages sent: " << count << ")" << std::endl;
		return 0;
	}

private:
	std::queue<std::string> &queue_ref;
	std::mutex &mutex_ref;
	std::condition_variable &cv_ref;
	std::atomic<bool> &finished_ref;
};


int main()
{
	// debug: print main thread id
	std::cout << "main() starting in thread: " << std::this_thread::get_id() << std::endl;

	/*
	Simulate radio driver sending periodic status (snr).
	Udp client runs in a separate thread consuming the queue.
	*/
	std::queue<std::string> q;
	std::mutex m;
	std::condition_variable cv;
	std::atomic<bool> finished(false);

	auto objRef = MySocketClass();
	if (objRef.error_status_self != MySocketClass::all_ok) {
		std::cerr << "Socket init failed. Exiting." << std::endl;
		return -1;
	}

	// start udp client thread
	std::thread client_thread(&MySocketClass::run, &objRef, std::ref(q), std::ref(m), std::ref(cv), std::ref(finished));
	std::cout << "Started client thread with id: " << client_thread.get_id() << std::endl;

	// fake radio runs in main thread and enqueues messages
	FakeRadioInterface myRadio(q, m, cv, finished);
	int rc = myRadio.work();

	// signal finished and wake client thread (in case it's waiting)
	finished.store(true);
	cv.notify_all();

	// join client thread
	if (client_thread.joinable()) client_thread.join();
	std::cout << "client thread joined. main() exiting (thread: " << std::this_thread::get_id() << ")" << std::endl;

	return rc;
}