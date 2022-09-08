_TEXT SEGMENT

EXTERN MyGetProcAddress: PROC

PUBLIC SearchProcAddress
PUBLIC CallFunction

SearchProcAddress PROC
    push rbp
    mov rbp, rsp
    mov qword ptr [rbp+10h], rcx
    push rbx
    push rdi
    push rsi
    sub rsp, 20h
    mov rdi, qword ptr [rbp+10h]
    mov rax, qword ptr gs:[60h]
    mov rax, qword ptr [rax+18h]
    mov rax, qword ptr [rax+10h]
    mov rbx, rax
    mov rsi, rax
    jmp FunctionSearch
ModuleSearch:
    mov rsi, qword ptr [rsi]
    cmp rsi, rbx
    je SearchExit
FunctionSearch:
    mov rcx, qword ptr [rsi+30h]
    mov rdx, rdi
    call MyGetProcAddress
    cmp rax, 0h
    je ModuleSearch
SearchExit:
    add rsp, 20h
    pop rsi
    pop rdi
    pop rbx
    pop rbp
    ret
SearchProcAddress ENDP

CallFunction PROC
    push rbp
    mov rbp, rsp
    mov dword ptr [rbp+10h], ecx
    mov qword ptr [rbp+18h], rdx
    mov qword ptr [rbp+20h], r8
    mov qword ptr [rbp+28h], r9
    push rbx
    push rdi
    sub rsp, 20h
    mov rcx, qword ptr [rbp+18h]
    call SearchProcAddress
    mov rdi, rax
    cmp dword ptr [rbp+10h], 4h
    jbe FuncCall
    mov ebx, dword ptr [rbp+10h]
    sub rbx, 4h
    mov rax, 8h
    mul rbx
    and eax, 0FFFFFFFFh
    sub rsp, rax
    mov rbx, 0h
SetArg:
    add rbx, 4h
    cmp dword ptr [rbp+10h], ebx
    je FuncCall
    sub rbx, 4h
    mov rax, qword ptr [rbp+40h+rbx*8h]
    mov qword ptr [rsp+20h+rbx*8h], rax
    inc rbx
    jmp SetArg
FuncCall:
    mov rcx, qword ptr [rbp+20h]
    mov rdx, qword ptr [rbp+28h]
    mov r8, qword ptr [rbp+30h]
    mov r9, qword ptr [rbp+38h]
    call rdi
    pop rdi
    pop rbx
    leave
    ret
CallFunction ENDP

_TEXT ENDS

END