#include <iostream>
#include <fstream>

int main()
{
    char* oupput_filename_ptr = (char*)(malloc(64 * sizeof(char)));
    memcpy(oupput_filename_ptr, "test2.txt", 20);
    std::ofstream outfile(oupput_filename_ptr);
    char* ch = (char*)(malloc(64*sizeof(char)));
    memcpy(ch, "this is a test message.", 20);

    outfile.write(ch, 20);
    return 0;
}