#include<stdio.h>
#include<unistd.h>

int main() {
    char * argv[3];
    argv[0] = "/sbin/halt";
    argv[1] = "-p";
    argv[2] = NULL;

    execve(argv[0], argv[0], NULL);
    return 0;
}