default rel
bits 64

extern GetStdHandle, WriteFile, ExitProcess

section .text
global main

main:
    ; get handle
    mov rcx, -11
    call GetStdHandle

    ; write to console
    mov rcx, rax
    mov rdx, msg
    mov r8, msg.len
    mov r9, trash
    call WriteFile

    ; exit
    xor rcx, rcx
    call ExitProcess

section .data
msg:    db "Hello, World!", 13, 10
.len: equ $-msg

section .bss
trash: resb 8