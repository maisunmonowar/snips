#include <iostream>
#include <sys/stat.h>
#include <filesystem>

int main()
{
    // Path to the directory
    const char* dir = "C:\\Users\\MaisunIbnMonowar\\mos.bin";

    // Structure which would store the metadata
    struct stat sb;

    // Calls the function with path as argument
    // If the file/directory exists at the path returns 0
    // If block executes if path exists
    if (stat(dir, &sb) == 0)
        std::cout << "The path is valid!" << std::endl;
    else
        std::cout << "The Path is invalid!" << std::endl;

    if std::filesystem::exists()
    {
        std::cout << "file existed";

    }
    else
    {
        std::cout << "file doesn't exist.";
    }

    return 0;    
}