#include "FunctionCaller.h"

PVOID MyGetProcAddress(HMODULE hModule, LPCSTR lpProcName)
{
    DWORD64 ImageBase = hModule;
    IMAGE_EXPORT_DIRECTORY *Export = 
        ImageBase + ((IMAGE_NT_HEADERS64 *)(ImageBase + ((IMAGE_DOS_HEADER *)hModule)->e_lfanew))->OptionalHeader.DataDirectory[0].VirtualAddress;
    
    PVOID Function = NULL;

    for (int i = 0; i < Export->NumberOfNames && Export != ImageBase; i++)
    {
        PVOID Name = ImageBase + *(DWORD *)(ImageBase + Export->AddressOfNames + i * 4);
        if (strcmp(lpProcName, Name) == 0)
        {
            WORD NameOrdinals = *(WORD *)(ImageBase + Export->AddressOfNameOrdinals + i * 2);
            Function = ImageBase + *(DWORD *)(ImageBase + Export->AddressOfFunctions + NameOrdinals * 4);
            break;
        }
    }
    
    return Function;
}

PVOID SearchProcAddress(LPCSTR lpProcName)
{
    __asm__ __volatile__ (
        "push rbx\n\t"
        "push rdi\n\t"
        "push rsi\n\t"
        "sub rsp, 0x20\n\r"
        "mov rdi, %[lpProcName]\n\t"
        "mov rax, qword ptr gs:[0x60]\n\t"
        "mov rax, qword ptr ds:[rax+0x18]\n\t"
        "mov rax, qword ptr ds:[rax+0x10]\n\t"
        "mov rbx, rax\n\t"
        "mov rsi, rax\n\t"
        "jmp FunctionSearch\n\t"
        "ModuleSearch:\n\t"
        "mov rsi, qword ptr ds:[rsi]\n\t"
        "cmp rsi, rbx\n\t"
        "je end\n\t"
        "FunctionSearch:\n\t"
        "mov rcx, qword ptr ds:[rsi+0x30]\n\t"
        "mov rdx, rdi\n\t"
        "call MyGetProcAddress\n\t"
        "cmp rax, 0x0\n\t"
        "je ModuleSearch\n\t"
        "end:\n\t"
        "add rsp, 0x20\n\t"
        "pop rsi\n\t"
        "pop rdi\n\t"
        "pop rbx\n\t"
        :
        : [lpProcName] "r" (lpProcName)
    );

    return;
}

int* CallFunction(int num, const char* FunctionName, ...)
{
    /* __declspec(naked)
    __asm__ __volatile__ (
        "push rbp\n\t"
        "push rdi\n\t"
        "sub rsp, 0x28\n\t"
        "mov qword ptr ss:[rsp+0x58], r9\n\t"
        "mov qword ptr ss:[rsp+0x50], r8\n\t"
        "mov qword ptr ss:[rsp+0x48], rdx\n\t"
        "mov qword ptr ss:[rsp+0x40], rcx\n\t"
        "lea r15, qword ptr ss:[rsp+0x50]\n\t"
        "mov rcx, qword ptr ss:[rsp+0x48]\n\t"
        "call SearchProcAddress\n\t"
        "cmp rax, 0x0\n\t"
        "je fail\n\t"
        "mov rdi, rax\n\t"
        "mov qword ptr ss:[rsp+0x20], 0x0\n\t"
        "cmp qword ptr ss:[rsp+0x40], 0x4\n\t"
        "jbe FuncCall\n\t"
        "mov rbx, qword ptr ss:[rsp+0x40]\n\t"
        "sub rbx, 0x4\n\t"
        "mov rax, 0x8\n\t"
        "mul rbx\n\t"
        "mov qword ptr ss:[rsp+0x20], rax\n\t"
        "sub rsp, rax\n\t"
        "xor rbx, rbx\n\t"
        "SetArg:\n\t"
        "add rbx, 0x4\n\t"
        "cmp qword ptr ss:[r15-0x10], rbx\n\t"
        "je FuncCall\n\t"
        "sub rbx, 0x4\n\t"
        "mov rax, qword ptr ss:[r15+0x20+rbx*0x8]\n\t"
        "mov qword ptr ss:[rsp+0x20+rbx*0x8], rax\n\t"
        "inc rbx\n\t"
        "jmp SetArg\n\t"
        "FuncCall:\n\t"
        "mov rcx, qword ptr ss:[r15]\n\t"
        "mov rdx, qword ptr ss:[r15+0x8]\n\t"
        "mov r8, qword ptr ss:[r15+0x10]\n\t"
        "mov r9, qword ptr ss:[r15+0x18]\n\t"
        "mov rax, rdi\n\t"
        "call rax\n\t"
        "add rsp, qword ptr ss:[rbp-0xA0]\n\t"
        "fail:\n\t"
        "add rsp, 0x28\n\t"
        "pop rdi\n\t"
        "pop rbp\n\t"
        "ret\n\t"
    );*/
    __asm__ __volatile__ (
        "push rbx\n\t"
        "push rdi\n\t"
        "sub rsp, 0x20\n\t"
        "mov rcx, qword ptr ss:[rbp+0x18]\n\t"
        "call SearchProcAddress\n\t"
        "mov rdi, rax\n\t"
        "cmp dword ptr ss:[rbp+0x10], 0x4\n\t"
        "jbe FuncCall\n\t"
        "mov ebx, dword ptr ss:[rbp+0x10]\n\t"
        "sub rbx, 0x4\n\t"
        "mov rax, 0x8\n\t"
        "mul rbx\n\t"
        "and eax, 0xFFFFFFFF\n\t"
        "sub rsp, rax\n\t"
        "mov rbx, 0x0\n\t"
        "SetArg:\n\t"
        "add rbx, 0x4\n\t"
        "cmp dword ptr ss:[rbp+0x10], ebx\n\t"
        "je FuncCall\n\t"
        "sub rbx, 0x4\n\t"
        "mov rax, qword ptr ss:[rbp+0x40+rbx*0x8]\n\t"
        "mov qword ptr ss:[rsp+0x20+rbx*0x8], rax\n\t"
        "inc rbx\n\t"
        "jmp SetArg\n\t"
        "FuncCall:\n\t"
        "mov rcx, qword ptr ss:[rbp+0x20]\n\t"
        "mov rdx, qword ptr ss:[rbp+0x28]\n\t"
        "mov r8, qword ptr ss:[rbp+0x30]\n\t"
        "mov r9, qword ptr ss:[rbp+0x38]\n\t"
        "call rdi\n\t"
        "pop rdi\n\t"
        "pop rbx\n\t"
        "leave\n\t"
        "ret\n\t"
    );
}