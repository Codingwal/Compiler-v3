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

factorial:
    push QWORD 0
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
    pop rax
    mov [rsp + 16], rax
    ret
    add rsp, 0
if_stmt_end0:
    push QWORD 0
    push QWORD [rsp + 16]
    pop rax
    pop rbx
    cmp eax, ebx
    setge al
    dec al
    push rax
    pop rax
    or al, al
    jz if_stmt_end1
    push QWORD 1
    push QWORD 0
    pop rax
    pop rbx
    sub eax, ebx
    push rax
    pop rax
    mov [rsp + 16], rax
    ret
    add rsp, 0
if_stmt_end1:
    push QWORD [rsp + 8]
    sub rsp, 8
    push QWORD 1
    push QWORD [rsp + 32]
    pop rax
    pop rbx
    sub eax, ebx
    push rax
    call factorial
    add rsp, 8
    pop rax
    pop rbx
    imul ebx
    push rax
    pop rax
    mov [rsp + 16], rax
    ret
    add rsp, 0
main:
    sub rsp, 8
    push QWORD 10
    call factorial
    add rsp, 8
    sub rsp, 8
    push QWORD [rsp + 8]
    call exit
    add rsp, 8
    add rsp, 8
