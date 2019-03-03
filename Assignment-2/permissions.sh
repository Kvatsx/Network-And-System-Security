# -fno-stack-protector
# -fno-stack-protector-all
# #pragma check_stack(off)
echo 0 | sudo tee /proc/sys/kernel/randomize_va_space
