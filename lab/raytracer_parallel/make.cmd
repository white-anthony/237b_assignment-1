@ECHO OFF

set COMPILER=clang

set C_INCLUDE_PATH=C:\OpenCL-SDK\include
set CPLUS_INCLUDE_PATH=C:\OpenCL-SDK\include
set LIBRARY_PATH=C:\OpenCL-SDK\lib

%COMPILER% main.c lib/*.c lib/geometry/*.c -o raytracer_parallel.exe -I./lib -I./lib/geometry -L%LIBRARY_PATH% -lOpenCL