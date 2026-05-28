#pragma once
#include <iostream>
#include <mutex>
#include <queue>
#include <stdio.h>
#include <thread>
#include <cstring>
#define NOMINMAX
#include <Windows.h>
#include <conio.h>


#include <atomic>
#include <condition_variable>

struct custom_struct_for_queue {
  char *pointerToByte;
  int numOfByte;

  custom_struct_for_queue(char *p, int n) : pointerToByte(p), numOfByte(n) {}
};
inline int c_BufSize = 15;