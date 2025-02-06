#include "employee.h"

employee::employee() { spdlog::debug("an employee was born"); }

employee::~employee() { spdlog::debug("employee died"); }

void employee::work(int* i) 
{
  spdlog::debug("employee work function");
	std::cout << name << " is working" << std::endl;
  std::cout << *i;
spdlog::info("Logging an integer: {}", *i);
spdlog::warn("Logging a boolean: {}", true);
spdlog::error("Logging a float: {}", 3.14f);
spdlog::critical("Logging a string: {}", "critical error");
spdlog::info("Logging multiple types: int: {}, float: {}, string: {}", 42, 2.718, "example");

}

void employee::setName(std::string newName) 
{ 
	name = newName; 
	spdlog::debug("employee has a name. ");
}