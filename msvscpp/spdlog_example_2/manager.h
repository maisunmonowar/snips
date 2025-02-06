#pragma once

#include "employee.h"

class manager {
public:
  manager();
  ~manager();
  void work(int *i);
  void setName(std::string newName);

private:
  std::string name;
};