#include "FunctionCaller.h"

int main()
{
    CallFunction(1, "LoadLibraryA", "user32.dll");
    CallFunction(4, "MessageBoxA", NULL, "Test", "Test", NULL);
}