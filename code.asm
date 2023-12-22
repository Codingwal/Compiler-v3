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
    push QWORD 1
    push QWORD 0
    push QWORD [rsp + 24]
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
    add rsp, 8
    add rsp, 0
if_stmt_end0:
while_loop_start0:
    push QWORD 0
    push QWORD [rsp + 24]
    pop rax
    pop rbx
    cmp eax, ebx
    sete al
    dec al
    push rax
    pop rax
    or al, al
    jz while_loop_end0
    push QWORD [rsp + 16]
    push QWORD [rsp + 8]
    pop rax
    pop rbx
    imul ebx
    push rax
    pop rax
    mov [rsp + 0], rax
    push QWORD 1
    push QWORD [rsp + 24]
    pop rax
    pop rbx
    sub eax, ebx
    push rax
    pop rax
    mov [rsp + 16], rax
    add rsp, 0
    jmp while_loop_start0
while_loop_end0:
    push QWORD [rsp + 0]
    call exit
    add rsp, 8
    add rsp, 8
main:
    push QWORD 10
    call factorial
    add rsp, 8
    add rsp, 0
