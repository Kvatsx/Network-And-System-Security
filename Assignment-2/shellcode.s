# Kaustav Vats (2016048)
# nasm -f elf64 -o shellcode.o shellcode.s
# ld -o shellcode shellcode.o
# ./shellcode
# Extract Shellcode
# for i in `objdump -d hello | tr '\t' ' ' | tr ' ' '\n' | egrep '^[0-9a-f]{2}$' ` ; do echo -n "\\x$i" ; done

global _start

section .text   

_start:
    jmp mssg

init:
    push 1
    pop rdi
    ; mov rdi, 1
    push 1
    pop rax
    ; mov rax, 1
    ; push mssg
    pop rsi
    ; mov rsi, mssg
    push 0xE
    pop rdx
    ; mov rdx, 0xE
    syscall
    
    push 60
    pop rax
    ; mov rax, 60
    push 0
    pop rdi
    ; mov rdi, 0
    syscall

mssg:
    call init

    db "Hello World!", 10

section .rodata
    ; mssg: db "Hello World!", 10
    ; mssglen: equ $ - mssg
