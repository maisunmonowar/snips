#pragma once
#include <iostream>
#include <vector>
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

using DataChunk = std::vector<char>;
inline int c_BufSize = 15;