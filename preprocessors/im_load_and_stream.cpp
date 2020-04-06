#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdint.h>
#include <windows.h>
#include <tchar.h>
#include <sstream>
#include <string>
#include <tgmath.h>

#include "kernel_loader.hpp"

#define IM_HEIGHT 3
#define IM_WIDTH  3
#define IM_DEPTH  3

#define KN_HEIGHT 7
#define KN_WIDTH  7



int image[IM_HEIGHT][IM_WIDTH][IM_DEPTH] = {
                                    {{11,12,13},{14,15,16},{17,18,19}},
                                    {{21,22,23},{24,25,26},{27,28,29}},
                                    {{31,32,33},{34,35,36},{37,38,39}}
                                  };

std::vector<float> load_kernel (std::string csv_pth, int kernelHeight, int kernelWidth)
{

    //Create an ifstream object to manipulate the file
    std::ifstream file( csv_pth.c_str() );

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

    assert( kernel.size() == kernelHeight * kernelWidth );

    return kernel;
}

using namespace std;

int main(int argc, TCHAR *argv[])
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi1;
    PROCESS_INFORMATION pi2;
    GetStartupInfo(&si);
    bool send;
    bool sendwait;
    bool receive;
    bool receivewait;
    int* imStream;
    int* convImStream;
    float kernelData[KN_HEIGHT][KN_WIDTH];
    float kernelTranspose[KN_HEIGHT][KN_WIDTH];
    int zeroPadHeight = IM_HEIGHT + (KN_HEIGHT - 1);
    int zeroPadWidth  = IM_WIDTH + (KN_WIDTH - 1);
    int zeroPadTop    = floor(KN_HEIGHT/2);
    int zeroPadSide   = floor(KN_WIDTH/2);
    int zeroPadIm[zeroPadHeight][zeroPadWidth][IM_DEPTH];
    int outImage[IM_HEIGHT][IM_WIDTH][IM_DEPTH];
    string inData;
    imStream     = (int *) malloc(sizeof(image));
    convImStream = (int *) malloc(sizeof(image));
    ofstream toFPGA   ("output.txt");
    ifstream fromFPGA ("input.txt");
    string csv_pth = "test_kernel.csv";

    /* Convert input kernel data from 2D array to 1D stream */
    std::vector<float> kernel_test = load_kernel(csv_pth,KN_HEIGHT,KN_WIDTH);

    /* Transpose kernel data */
    for(int i = 0; i < KN_HEIGHT; i++){
        for(int j = 0; j < KN_WIDTH; j++){
            kernelData[i][j] = kernel_test[(i*KN_WIDTH) + j];
        }
    }
    for(int i = 0; i < KN_HEIGHT; i++){
        for(int j = 0; j < KN_WIDTH; j++){
            kernelTranspose[i][j] = kernelData[j][i];
        }
    }
    for(int i = 0; i < KN_HEIGHT; i++){
        for(int j = 0; j < KN_WIDTH; j++){
            kernel_test[(i*KN_WIDTH) + j] = kernelTranspose[i][j];
        }
    }

    /* Zero-Pad input image */
    for(int i = 0; i < IM_DEPTH; i++){
        for(int j = 0; j < zeroPadHeight; j++){
            for(int k = 0; k < zeroPadWidth; k++){
                if(j < (zeroPadTop + 1) || j > zeroPadHeight - (zeroPadTop + 1))
                    zeroPadIm[j][k][i] = 0;
                else if(k < (zeroPadSide + 1) || k > zeroPadWidth - (zeroPadSide + 1))
                    zeroPadIm[j][k][i] = 0;
                else{
                    zeroPadIm[j][k][i] = image[j-zeroPadTop-1][k-zeroPadSide-1][i];
                }
            }
        }
    }

    /* Convert input image data from 3D array to 1D stream */
    for(int i = 0; i < IM_HEIGHT; i++){
        for(int j = 0; j < IM_WIDTH; j++){
            for(int k = 0; k < IM_DEPTH; k++){
                imStream[(i*IM_WIDTH*IM_DEPTH) + (j*IM_DEPTH) + k] = image[i][j][k];
            }
        }
    }

    /* Write stream data to file */
    if (toFPGA.is_open()){
        for(int i = 0; i < KN_HEIGHT*KN_WIDTH; i++)
            toFPGA << kernel_test[i] << " ";
        for(int i = 0; i < IM_HEIGHT*IM_WIDTH*IM_DEPTH; i++)
            toFPGA << imStream[i] << " ";
        toFPGA.close();
    }

    /* Open two processes to send and receive data from FPGA */
    if (send = CreateProcess(NULL, "whoami", NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi1) == false)
        cout << "CreateProcess1 failed" << endl;
    if (receive = CreateProcess(NULL, "whoami", NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi2) == false)
        cout << "CreateProcess2 failed" << endl;

    /* Wait for both processes to finish */
    if (sendwait = WaitForSingleObject(pi1.hProcess, INFINITE) == WAIT_OBJECT_0)
        cout << "waitprocess:true" << endl;
    if (receivewait = WaitForSingleObject(pi2.hProcess, INFINITE) == WAIT_OBJECT_0)
        cout << "waitprocess:true" << endl;

    /* Write received data into image stream */
    if (fromFPGA.is_open()){
        while (getline(fromFPGA,inData))
        fromFPGA.close();
    }

    /* Parse string data into stream */
    int i = 0;
    stringstream ssin(inData);
    while (ssin.good() && i < IM_HEIGHT*IM_WIDTH*IM_DEPTH){
        ssin >> convImStream[i];
        i++;
    }

    /* Arrange stream into image style */
    for (i = 0; i < IM_DEPTH; i++){
        for (int j = 0; j < zeroPadHeight; j++){
            for (int k = 0; k < zeroPadWidth; k++)
                zeroPadIm[i][j][k] = convImStream[(i*zeroPadWidth*IM_DEPTH) + (j*IM_DEPTH) + k];
        }
    }

    /* Clip out garbage data from zero-pad output */
    for(i = 0; i < IM_DEPTH; i++){
        for(int j = 0; j < zeroPadHeight; j++){
            for(int k = 0; k < zeroPadWidth; k++){
                if(!((j < (zeroPadTop + 1) || j > zeroPadHeight - (zeroPadTop + 1)) || (k < (zeroPadSide + 1) || k > zeroPadWidth - (zeroPadSide + 1)))){
                    outImage[j-zeroPadTop-1][k-zeroPadSide-1][i] = zeroPadIm[j][k][i];
                }
            }
        }
    }

}
