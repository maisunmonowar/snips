#include "manager.h"
#include <thread>
manager::manager() 
{ 
	spdlog::debug("constructor manager"); 
}
manager::~manager() { spdlog::debug("manager destroyed"); }
void manager::work(int *i) {
  spdlog::debug("manager work function");
  std::cout << name << " is working." << std::endl;

	auto employee_joe = employee();
        employee_joe.setName("joe");
        auto employee_thread = std::thread(&employee::work, &employee_joe, i);
        employee_thread.join();

}

void manager::setName(std::string newName)
{ 
	name = newName; 
	spdlog::debug("manager name function");
}