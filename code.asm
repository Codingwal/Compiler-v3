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
    push QWORD 2
    pop rax
    pop rbx
    imul rbx
    push rax
    push QWORD 3
    push QWORD 6
    pop rax
    pop rbx
    xor rdx, rdx
    div rbx
    push rax
    push QWORD 4
    pop rax
    pop rbx
    add rax, rbx
    push rax
    pop rax
    pop rbx
    add rax, rbx
    push rax
    push QWORD [rsp + 0]
    call exit
