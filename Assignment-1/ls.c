// Kaustav Vats (2016048)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include "helper.h"

void Error()
{
	fprintf(stderr, "Error: ls, file does not exist.\n");
}
void SyntaxError(char *in)
{
	fprintf(stderr, "Syntax Error: %s\nUse: ls <directory>\n", in);
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
		n = scandir(".", &names, NULL, alphasort);
		if ( n == NULL )
		{
			Error();
		}
		else if ( n < 0 )
		{
			SyntaxError(argv[0]);
		}
		else
		{
			int i=0;
			while ( i < n )
			{
				char *p;
				p = names[i]->d_name;
				if ( p[0] == '.' )
				{
					i++;
					continue;
				}

                showFileDetails(names[i]->d_name, names[i]->d_name);
				free(names[i]);
				i++;
			}
			printf("\n");
			free(names);
		}

    }
    else {
        if (checkPath(argv[1]) == -1) {
            printf("Error: Path\n");
            exit(1);
        }
        // printf("argv[1]: %s\n", argv[1]);
        n = scandir(argv[1], &names, NULL, alphasort);
        if ( n < 0 )
        {
            Error();
        }
        else
        {
            int i=0;
            while ( i < n )
            {
                char *p;
                p = names[i]->d_name;
                if ( p[0] == '.' )
                {
                    i++;
                    continue;
                }
                // printf("%s\n", names[i]->d_name);  
                char temp[1024];
                memset(temp, '\0', sizeof(temp));
                strcpy(temp, argv[1]);
                // strcat(temp, "/");
                strcat(temp, names[i]->d_name);
                // printf("%s\n", temp);
                showFileDetails(temp, names[i]->d_name);
                free(names[i]);
                i++;
            }
            free(names);
            // printf("\n");
        }
    }
    seteuid(getuid());
    PrintUserDetails();
    // free(realpath);

    return 0;
}
