// Kaustav Vats (2016048)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <grp.h>

void Error() {
	fprintf(stderr, "Cannot create directory using mkdir\n");
	exit(1);
}
void SyntaxError(const char *in) {
	fprintf(stderr, "Syntax Error: %s\nUse: create_dir [name]\nEx: mkdir foo\n", in);
	exit(1);
}

int main(int argc, char const *argv[]) {
    char * owner = NULL;
    char * group = NULL;
    size_t size;
    printf("Enter Owner: ");
    if (getline(&owner, &size, stdin) == -1) {
        printf("Input Error!\n");
    }
    printf("Enter Group: ");
    if (getline(&group, &size, stdin) == -1) {
        printf("Input Error!\n");
    }
    owner = strtok(owner, "\n");
    group = strtok(group, "\n");
    if ( argc == 2 )
	{
		char *p;
        p = argv[1];
		if ( p[0] == '-' )
		{
			SyntaxError(argv[0]);
		}
		else if ( mkdir(argv[1], S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP) < 0 )
		{
			Error();
		}
        else
        {
            if (strcmp(owner, "\n") == 0  && strcmp(group, "\n") == 0) {
                printf("Default Permissions!\n");
            }
            else {
                uid_t uid;
                gid_t gid;
                struct passwd *pwd;
                char * endptr;

                uid = strtol(owner, &endptr, 10);
                if (*endptr != '\0') {
                    printf("%s\n%d\n", owner, strlen(owner));
                    pwd = getpwnam(owner);
                    if (pwd == NULL) {
                        perror("User Exists?");
                        exit(1);
                    }
                    uid = pwd->pw_uid;
                }
                gid = strtol(group, &endptr, 10);
                if (*endptr != '\0') {
                    pwd = getpwnam(group);
                    if (pwd == NULL) {
                        perror("Group Exists?");
                        exit(1);
                    }
                    gid = pwd->pw_gid;
                }

                if (chown(argv[1], uid, gid) == -1) {
                    perror("chown");
                }
                else {
                    printf("Chown Worked!\n");
                }
            }
        }
        
    }
    else
	{
		SyntaxError(argv[0]);
    }

    return 0;
}
