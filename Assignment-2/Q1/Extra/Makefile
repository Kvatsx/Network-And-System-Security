all:
	gcc -z execstack -fno-stack-protector vuln.c -o vuln
shellcode:
	nasm -f elf64 -o shellcode.o shellcode.s
	ld -o shellcode shellcode.o
	./shellcode
	# for i in `objdump -d shellcode | tr '\t' ' ' | tr ' ' '\n' | egrep '^[0-9a-f]{2}$' ` ; do echo -n "\\x$i" ; done ; echo -e "\n"
clean:
	rm shellcode
	rm shellcode.o
	rm vuln