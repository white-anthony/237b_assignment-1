@ECHO OFF

set COMPILER=clang

set C_INCLUDE_PATH=C:\OpenCL-SDK\include
set CPLUS_INCLUDE_PATH=C:\OpenCL-SDK\include
set LIBRARY_PATH=C:\OpenCL-SDK\lib

%COMPILER% main.c -o device_query.exe -L%LIBRARY_PATH% -lOpenCL -I../../helper_lib ../../helper_lib/*.o