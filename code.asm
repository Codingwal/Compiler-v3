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
    push qword [rsp + 8]
    call exit

; funcDef
main:
    ; expr
    push qword 20
    ; varDef
    ; expr
    push qword 0
    ; varDef
    ; expr
    push qword [rsp + 8]
    ; varDef
    ; expr
    push qword 15
    ; varDef
    ; funcCall
    ; expr
    push qword [rsp + 8]
    call myFunc
