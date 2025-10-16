// live20sec.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <thread> // Required for std::this_thread::sleep_for
#include <chrono>

int main()
{
  for (int i = 0; i < 25; i++) {
    std::cout << "I'm doing something." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
  return 0;
}
