// Kaustav Vats (2016048)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include "helper.h"

void Error()
{
	fprintf(stderr, "Error: ls, file does not exist.\n");
    seteuid(getuid());
    PrintUserDetails();
    exit(1);
}
void SyntaxError(char *in)
{
	fprintf(stderr, "Syntax Error: %s\nUse: ls <directory>\n", in);
    seteuid(getuid());
    PrintUserDetails();
	exit(1);
}

int main(int argc, char const *argv[]) {
    if (argc > 2) {
        printf("Usage: ls <Directory/Empty>\nEx: ls foo or ls\n");
        exit(1);
    }

    PrintUserDetails();
    // char * realpath;
    // realpath = malloc(1024*sizeof(char));
    // ActualPath(argv[1], realpath);
    // printf("asda: %s\n", realpath);

    struct dirent **names;
    int n;

    if ( argc == 1 )
	{
        struct dirent *name;
        DIR * dir = opendir(".");

        if (dir == NULL) {
            Error();
        }
        while ((name = readdir(dir)) != NULL) {
            // printf("d_name %s\n", name->d_name);
            showFileDetails(name->d_name, name->d_name);
        }
        closedir(dir);

    }
    else {
        if (checkPath(argv[1]) == -1) {
            printf("Error: Path\n");
            exit(1);
        }
        struct dirent *name;
        DIR * dir = opendir(argv[1]);

        if (dir == NULL) {
            Error();
        }
        while ((name = readdir(dir)) != NULL) {
            // printf("d_name %s\n", name->d_name);
            char temp[1024];
            memset(temp, '\0', sizeof(temp));
            strcpy(temp, argv[1]);
            // strcat(temp, "/");
            strcat(temp, name->d_name);
            // printf("%s\n", temp);
            showFileDetails(temp, name->d_name);
        }
        closedir(dir);
    }
    seteuid(getuid());
    PrintUserDetails();
    // free(realpath);

    return 0;
}
