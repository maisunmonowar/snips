#pragma once
#include <string>
#include "iostream"
#include "spdlog/spdlog.h"
class employee {
public:
  employee();
  ~employee();
  void work(int* i);
  void setName(std::string newName);

private:
  std::string name;
};