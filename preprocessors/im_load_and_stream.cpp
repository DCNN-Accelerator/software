#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdint.h>
#include <windows.h>
#include <tchar.h>

#define IM_HEIGHT 3
#define IM_WIDTH  3
#define IM_DEPTH  3

#define KN_HEIGHT 3
#define KN_WIDTH  3

using namespace std;

int kernel[KN_HEIGHT][KN_WIDTH] = {{1,2,3},{4,5,6},{7,8,9}};
int image[IM_HEIGHT][IM_WIDTH][IM_DEPTH] = {
                                    {{11,12,13},{14,15,16},{17,18,19}},
                                    {{21,22,23},{24,25,26},{27,28,29}},
                                    {{31,32,33},{34,35,36},{37,38,39}}
                                  };

int main(int argc, TCHAR *argv[])
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    int* knStream;
    int* imStream;
    knStream = (int *) malloc(sizeof(kernel));
    imStream = (int *) malloc(sizeof(image));
    ofstream toFPGA ("output.txt");

    /* Convert input kernel data from 2D array to 1D stream */
    for(int i = 0; i < KN_HEIGHT; i++){
        for(int j = 0; j < KN_WIDTH; j++){
            knStream[(i*KN_WIDTH) + j] = kernel[i][j];
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
            toFPGA << knStream[i] << " ";
        for(int i = 0; i < IM_HEIGHT*IM_WIDTH*IM_DEPTH; i++)
            toFPGA << imStream[i] << " ";
    }

    /* TESTING: compare knStream to input array of known size */
    bool pass = true;
    for(int i = 0; i < KN_HEIGHT; i++){
        for(int j = 0; j < KN_WIDTH; j++){
            if (knStream[(i*KN_WIDTH) + j] != kernel[i][j])
                pass = false;
        }
    }

    /* TESTING: compare imStream to input array of known size */
    for(int i = 0; i < IM_HEIGHT; i++){
        for(int j = 0; j < IM_WIDTH; j++){
            for(int k = 0; k < IM_DEPTH; k++){
                if (imStream[(i*IM_WIDTH*IM_DEPTH) + (j*IM_DEPTH) + k] != image[i][j][k])
                    pass = false;
            }
        }
    }

    /* TESTING: output results from test */
    if (pass) cout << "All tests passed" << endl;
    else      cout << "Tests failed" << endl;

    if(!CreateProcess(NULL, "whoami", NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)){
        cout << "CreateProcess failed (" << GetLastError() << ")." << endl;
    }

    if(!CreateProcess(NULL, "whoami", NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)){
        cout << "CreateProcess failed (" << GetLastError() << ")." << endl;
    }
}
