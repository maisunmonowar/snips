#include <iostream>
#include <chrono>
#include <thread>
#include <time.h>
#include<stdio.h>

int main()
{

    auto time_start = std::chrono::high_resolution_clock::now();
    std::time_t now_c =std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    tm curr_tm;
    gmtime_s(&curr_tm, &now_c);
    char string[100]; 
    strftime(string, 100, "Today is %Y-%m-%d %H:%M:%S", &curr_tm);
    std::cout << string << std::endl;
  
    std::chrono::seconds sleepDuration(1);
    std::this_thread::sleep_for(sleepDuration);
    auto time_end = std::chrono::high_resolution_clock::now();

    auto time_elapsed = (std::chrono::duration_cast<std::chrono::seconds>)(time_end - time_start).count();
    std::cout << "time elapsed " << time_elapsed << " s" << std::endl;
}