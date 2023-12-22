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
    push QWORD 5
    push QWORD 0
for_loop_start0:
    push QWORD 10
    push QWORD [rsp + 8]
    pop rax
    pop rbx
    cmp eax, ebx
    setge al
    dec al
    push rax
    pop rax
    or al, al
    jz for_loop_end0
    push QWORD 1
    push QWORD [rsp + 8]
    pop rax
    pop rbx
    add eax, ebx
    push rax
    pop rax
    mov [rsp + 0], rax
    push QWORD 1
    push QWORD [rsp + 16]
    pop rax
    pop rbx
    add eax, ebx
    push rax
    pop rax
    mov [rsp + 8], rax
    jmp for_loop_start0
for_loop_end0:
    pop rax
    push QWORD [rsp + 0]
    call exit
