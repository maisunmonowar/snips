// filesize.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>

std::ifstream::pos_type filesize(const char* filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}
//"C:\Users\MaisunIbnMonowar\Documents\code\dektec_demod\pythonScripts\50M16APSK910_ch3_mode3_200MB.l3"
char* filename_parsed = (char*)malloc(128 * sizeof(char));
const char * temp_filename = "C:\\Users\\MaisunIbnMonowar\\Documents\\code\\dektec_demod\\pythonScripts\\50M16APSK910_ch3_mode3_200MB.l3";

int main()
{
    // set the file name,
    memcpy(filename_parsed, temp_filename, 107);
    std::cout << "Hello World!\n";
    auto sizee = filesize(filename_parsed);
    std::cout << sizee << std::endl;
    std::ifstream in(filename_parsed, std::ifstream::ate | std::ifstream::binary);
    std::cout << in.tellg();
    in.close();
}
