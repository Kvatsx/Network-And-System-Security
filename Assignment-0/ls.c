#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

void Error()
{
	fprintf(stderr, "Error: ls, file does not exist.\n");
}
void SyntaxError(char *in)
{
	fprintf(stderr, "Syntax Error: %s\nUse: ls [options] [directory] \nOptions: -a, -1, -a1 or -1a\n", in);
	exit(1);
}

int main(int argc, char const *argv[])
{
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
				printf("%s\n", (names[i]->d_name));
				free(names[i]);
                i++;
			}
			printf("\n");
			free(names);
		}
	}
	else if ( argc == 2 )
	{
		char *p;
		p = argv[1];
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
                printf("%s\t", (names[i]->d_name));
                free(names[i]);
                i++;
            }
            free(names);
            printf("\n");
        }
	}
	return 0;
}