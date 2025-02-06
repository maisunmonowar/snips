// simpleAdd.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

int main()
{
    std::cout << "Hello World!\n";
    int *a = new int;
    *a = 8; 
    
    int result_int;
    int64_t result_int64, result2_int64, result3_int64;

    result_int = *a + 4;
    result_int64 = *a + 4;
    result2_int64 = static_cast<int64_t> (*a + 4);
    result3_int64 = (static_cast<int64_t> (*a)) + 4;

    std::cout << result_int;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
