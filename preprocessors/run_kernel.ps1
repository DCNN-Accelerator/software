# Simple Powershell Script to run both the Python and C++ scripts for testing the kernel loader

Remove-Item test_kernel.csv
python create_csv.py

g++ kernel_loader.cpp 
./a