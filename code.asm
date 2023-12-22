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
    push QWORD 4
    push QWORD 4
    pop rax
    pop rbx
    cmp eax, ebx
    setg al
    dec al
    push rax
    push QWORD 4
    push QWORD 4
    pop rax
    pop rbx
    cmp eax, ebx
    setl al
    dec al
    push rax
    push QWORD 3
    push QWORD 4
    pop rax
    pop rbx
    cmp eax, ebx
    sete al
    dec al
    push rax
    pop rax
    pop rbx
    and al, bl
    push rax
    pop rax
    pop rbx
    and al, bl
    push rax
    pop rax
    or al, al
    jz if_stmt_end0
    push QWORD 1
    call exit
if_stmt_end0:
    push QWORD 0
    call exit
