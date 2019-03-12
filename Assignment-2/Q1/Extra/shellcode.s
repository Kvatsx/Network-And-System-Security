; # Kaustav Vats (2016048)
; # nasm -f elf64 -o shellcode.o shellcode.s
; # ld -o shellcode shellcode.o
; # ./shellcode
; # Extract Shellcode
; # for i in `objdump -d shellcode | tr '\t' ' ' | tr ' ' '\n' | egrep '^[0-9a-f]{2}$' ` ; do echo -n "\\x$i" ; done

global _start
section .text   
_start:
    jmp mssg
init:
    pop rsi
    xor rax, rax
    mov al, 1
    mov rdi, rax
    mov rdx, rdi
    add rdx, 0xE
    ; push 1
    ; pop rdi
    ; mov rdi, 1
    ; push 1
    ; pop rax
    ; mov rax, 1
    ; push mssg
    ; mov rsi, mssg
    ; push 0xE
    ; pop rdx
    ; mov rdx, 0xE
    syscall
    xor rax, rax
    add rax, 60
    ; mov rax, 60
    ; push 0
    ; pop rdi
    xor rdi, rdi
    ; mov rdi, 0
    syscall
mssg:
    call init
    db "Hello World!"

; section .rodata
    ; mssg: db "Hello World!", 10
    ; mssglen: equ $ - mssg
; 49 Length
; \xeb\x1e\x5e\x48\x31\xc0\xb0\x01\x48\x89\xc7\x48\x89\xfa\x48\x83\xc2\x0e\x0f\x05\x48\x31\xc0\x48\x83\xc0\x3c\x48\x31\xff\x0f\x05\xe8\xdd\xff\xff\xff\x48\x65\x6c\x6c\x6f\x20\x57\x6f\x72\x6c\x64\x21\x0d\x0a
; 0x7fffffffdd58
; \x7f\xff\xff\xff\xdd\x10