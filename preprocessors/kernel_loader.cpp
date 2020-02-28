#include "kernel_loader.hpp"


std::vector<float> load_kernel (std::string csv_pth, int kernelSize)
{

    //Create an ifstream object to manipulate the file 
    std::ifstream file( csv_pth );

    if (!file) throw std::runtime_error("Could not open Kernel file");

    // Store kernel values into a STL vector of floats - 1D 
    std::vector<float> kernel; 


    /*
        - Iterate through each "line" of the csv, which gets read into a csv string
        - convert the line into a stringstream() object 
        - break the stringstream into tokens based on the "," delimiter
        - convert each token into a float and add it to the std::vector for the kernel
    */


    std::string fileLine; 

    while (std::getline(file,fileLine))
    {
        std::stringstream curr_line(fileLine); 

        std::string token; 

        while ( std::getline(curr_line,token,',') )
        {
            kernel.push_back( stof(token) );
        }

    }

    assert( kernel.size() == kernelSize^2 ); //assuming the kernel is a square matrix

    return kernel; 
}




int main()
{

    std::string csv_pth = "test_kernel.csv";
    int kernel_size = 7; 

    std::vector<float> kernel_test = load_kernel(csv_pth,kernel_size);

    for (int i = 0; i < kernel_test.size(); i++)
    {
        std::cout << "Kernel at idx: " << i << " = " << kernel_test[i] << "\n";
        
    }



}



