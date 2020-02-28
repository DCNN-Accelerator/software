#include <fstream>
#include <vector> 
#include <iostream>
#include <string> 
#include <sstream>

#include "assert.h" 

/*
Header file for the kernel loader function

params:
    - std::string csv_pth : the path of the .csv file containing the kernel values
    - int kernelSize      : the size of the kernel array, assuming a square size 

*/

std::vector<float> load_kernel (std::string csv_pth, int kernelSize); 

                                    