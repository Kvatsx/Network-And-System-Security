# Kaustav Vats (2016048)
# nasm -f elf64 -o shellcode.o shellcode.s
# ld -o shellcode shellcode.o
# ./shellcode

global _start

section .text   

_start:
    push 1
    pop rdi
    ; mov rdi, 1
    push 1
    pop rax
    ; mov rax, 1
    push mssg
    pop rsi
    ; mov rsi, mssg
    push mssglen
    pop rdx
    ; mov rdx, mssglen
    syscall
    
    push 60
    pop rax
    ; mov rax, 60
    push 0
    pop rdi
    ; mov rdi, 0
    syscall
section .rodata
    mssg: db "Hello World!", 10
    mssglen: equ $ - mssg
