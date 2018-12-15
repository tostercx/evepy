@echo off
@del evepy.exe
@del evepy.obj
@call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars32.bat" x86
cl /O2 evepy.cpp
