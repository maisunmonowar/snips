#include "u.h"
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <random>
#include <string>
#include <thread>

class FakeRadioInterface {
public:
  FakeRadioInterface(std::queue<std::string> &q, std::mutex &m,
                     std::condition_variable &cv, std::atomic<bool> &finished)
      : queue_ref(q), mutex_ref(m), cv_ref(cv), finished_ref(finished) {}

  int work() {
    std::cout << "FakeRadioInterface::work() running in thread: "
              << std::this_thread::get_id() << std::endl;

    using namespace std::chrono;
    std::mt19937 rng((unsigned)std::chrono::high_resolution_clock::now()
                         .time_since_epoch()
                         .count());
    std::uniform_int_distribution<int> snr_dist(0, 40);

    auto last_time = steady_clock::now();
    int count = 0;
    while (!finished_ref.load()) {
      auto now = steady_clock::now();
      if (duration_cast<seconds>(now - last_time).count() >= 1) {
        int snr = snr_dist(rng);
        std::string msg = "snr:" + std::to_string(snr);
        std::cout << "FakeRadioInterface: enqueue -> " << msg << std::endl;
        {
          std::lock_guard<std::mutex> lg(mutex_ref);
          queue_ref.push(msg);
        }
        cv_ref.notify_one();

        last_time = now;
        ++count;
      } else {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }
    }
    std::cout << "FakeRadioInterface: finishing work loop: "
              << " (messages sent: " << count << ")" << std::endl;
    return 0;
  }

private:
  std::queue<std::string> &queue_ref;
  std::mutex &mutex_ref;
  std::condition_variable &cv_ref;
  std::atomic<bool> &finished_ref;
};

int main() {
  std::cout << "main() starting in thread: " << std::endl;
  std::queue<std::string> q;
  std::mutex m;
  std::condition_variable cv;
  std::atomic<bool> finished(false);

  auto objRef = MySocketClass();
  if (objRef.error_status_self != MySocketClass::all_ok) {
    std::cerr << "Socket init failed. Exiting." << std::endl;
    return -1;
  }

  std::thread client_thread(&MySocketClass::run, &objRef, std::ref(q),
                            std::ref(m), std::ref(cv), std::ref(finished));
  std::cout << "Started client thread" << std::endl;

  FakeRadioInterface myRadio(q, m, cv, finished);
  int rc = myRadio.work();

  finished.store(true);
  cv.notify_all();

  if (client_thread.joinable())
    client_thread.join();
  std::cout << "client thread joined. main() exiting "
            << ")" << std::endl;

  return rc;
}