@ECHO OFF

set COMPILER=clang

%COMPILER% main.c lib/*.c lib/geometry/*.c -o raytracer_sequential.exe -I./lib -I./lib/geometry