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


; funcDef
myFunc:
    ; funcCall
    ; expr
    push qword [rsp + 16]
    call exit

; funcDef
main:
    ; funcCall
    ; expr
    push qword 10
    ; expr
    push qword 20
    call myFunc
