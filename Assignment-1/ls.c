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

                showFileDetails(names[i]->d_name);
				free(names[i]);
				i++;
				if ( i%10 == 0 )
				{
					printf("\n");
				}
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

                showFileDetails(names[i]->d_name);
                free(names[i]);
                i++;
                if ( i%10 == 0 )
                {
                    printf("\n");
                }
            }
            free(names);
            printf("\n");
        }
    }
    PrintUserDetails();

    return 0;
}
