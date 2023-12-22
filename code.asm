default rel
bits 64

extern GetStdHandle, WriteFile, ExitProcess

section .text
global _start

; entrypoint
_start:
    call main

; base functions
exit:
    mov rcx, [rsp + 8]
    call ExitProcess


; code section

main:
    push QWORD 2
    push QWORD 3
    pop rax
    pop rbx
    imul ebx
    push rax
    push QWORD 2
    pop rax
    pop rbx
    add eax, ebx
    push rax
    push QWORD 2
    push QWORD 3
    push QWORD 2
    pop rax
    pop rbx
    add eax, ebx
    push rax
    pop rax
    pop rbx
    imul ebx
    push rax
    push QWORD [rsp + 0]
    push QWORD [rsp + 16]
    pop rax
    pop rbx
    cmp eax, ebx
    setne al
    dec al
    push rax
    pop rax
    or al, al
    jz if_stmt_end0
    push QWORD 1
    call exit
if_stmt_end0:
    push QWORD 0
    call exit
