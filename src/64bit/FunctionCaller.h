#pragma once

#include <stdio.h>
#include <windows.h>

#ifdef __GNUC__
VOID* CallFunction(int num, const char *FunctionName, ...);
#elif _MSC_VER
extern VOID* CallFunction(int num, const char *FunctionName, ...);
#endif