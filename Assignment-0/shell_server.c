/*
 * @Author: Kaustav Vats 
 * @Roll-Number: 2016048 
 * @Date: 2019-01-08 21:04:50 
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <pthread.h>
#include <dirent.h>
#include <math.h>


#define PORT 5555
#define BUFSIZE 1024
#define CONNECTIONS 6

pthread_t tid[CONNECTIONS];

char root[BUFSIZE];

char usernames[5][6];

struct Arguments {
    int client_socket;
    int server_socket;
    char user[4];
};

void ErrorLs()
{
	fprintf(stderr, "Error: ls, file does not exist.\n");
}
void SyntaxErrorLs(char *in)
{
	fprintf(stderr, "Syntax Error: %s\nUse: ls [options] [directory] \nOptions: -a, -1, -a1 or -1a\n", in);
	exit(1);
}
void updatePath() {

}

// Return last string after slash
char * SlashParser(char * arr) {
    printf("Token: %s\n", arr); 
    int count = 0;
    int k;
    for (k=0; k<strlen(arr); k++) {
        if ( k != 0 && arr[k] == '/' ) {
            count++;
        }
    }
    printf("Count: %d\n", count);
    char * token = strtok(arr, "/");
    int j;
    for (j=0; j<count; j++) {
        token = strtok(NULL, "/");                   
        printf("After %s\n", token);
    }
    // printf("tkon: %s\n", token);
    return token;
}
int checkPath(char * path, char * user) {
    int i;
    for (i=0; i<6; i++) {
        if ( strncmp(usernames[i], user, 2) != 0 ) {
            printf("user: %s\n", usernames[i]);
            if ( strstr(path, usernames[i]) != NULL ) {
                printf("isme kya dikkat\n");
                return 0;
            }
            char pwd[1024];
	        getcwd(pwd, sizeof(pwd));
            printf("pwd: %s\n", pwd);
            if ( strstr(pwd, usernames[i]) != NULL ) {
                printf("isme \n");
                return 0;
            }
        }
    }
    printf("Ye lo\n");
    return 1;
}

char * Exist(char * name) {
    FILE* fp;
    char path[BUFSIZE];
    strcpy(path, root);
    strcat(path, "/simple_slash/fileDetails.txt");
    fp = fopen(path, "r");
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    if ( fp == NULL ) {
        perror("Error: While opening a file!\n");
        exit(1);
    }
    int i=0;
    while ((read = getline(&line, &len, fp)) != -1) {
        printf("Line: %s\n", line);
        char temp[BUFSIZE];
        strcpy(temp, line);
        char *token = strtok(temp, ":");
        printf("token Line: %s\nName: %s", token, name);
        // printf("%d %d\n", strlen(token), strlen(name));
        if ( strncmp(token, name, strlen(token)) == 0 ) {
            printf("My line: %s\n", line);
            return line;
        }
        else {
            printf("Not same\n");
        }
        i++;
    }
    fclose(fp);
    if (line) {
        free(line);
    }
    return NULL;
}

void readUsername() {
    FILE* fp;
    fp = fopen("simple_slash/etc/usergroup", "r");
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    if ( fp == NULL ) {
        perror("Error: While opening a file!\n");
        exit(1);
    }
    memset(usernames, '\0', sizeof(usernames));
    int i=0;
    while ((read = getline(&line, &len, fp)) != -1) {
        // printf("Retrieved line of length %zu:\n", read);
        char *token = strtok(line, ":");
        printf("Token: %s\n", token);
        strcpy(usernames[i], token);
        // printf("%s\n", line);
        i++;
    }
    fclose(fp);
    if (line) {
        free(line);
    }
}

char * getUserGroup(char * user) {
    FILE* fp;
    char fakePath[BUFSIZE];
    strcpy(fakePath, root);
    strcat(fakePath, "/simple_slash/etc/usergroup");
    fp = fopen(fakePath, "r");
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    if ( fp == NULL ) {
        perror("Error: While opening a file!\n");
        exit(1);
    }
    int i=0;
    while ((read = getline(&line, &len, fp)) != -1) {
        // printf("Retrieved line of length %zu:\n", read);
        char *token = strtok(line, ":");
        if ( strcmp(token, user) == 0 ) {
            token = strtok(NULL, ":");
            return token;
        }
        i++;
    }
    fclose(fp);
    if (line) {
        free(line);
    }
    return NULL;
}

int checkCDPermissions(char * cuser, char * ouser) {
    FILE* fp;
    char temp[BUFSIZE];
    memset(temp, "\0", sizeof(temp));
    strcpy(temp, root);
    strcat(temp, "/simple_slash/etc/usergroup");
    fp = fopen(temp, "r");
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    if ( fp == NULL ) {
        perror("Error: While opening a file!\n");
        exit(1);
    }
    int i=0;
    while ((read = getline(&line, &len, fp)) != -1) {
        // printf("Retrieved line of length %zu:\n", read);
        char *token = strtok(line, ":");
        printf("Token: %s\n", token);
        strcpy(usernames[i], token);
        if ( strncmp(usernames[i], ouser, 2) == 0 ) {
            token = strtok(NULL, ":");
            if ( strncmp(token, cuser, 2) == 0 ) {
                return 1;
            }
        }
        // printf("%s\n", line);
        i++;
    }
    fclose(fp);
    if (line) {
        free(line);
    }
    return 0;
}

void pwd() {
	char currentwd[1024];
	getcwd(currentwd, sizeof(currentwd));
	printf("pwd: %s\n", currentwd );
}

void * ConnectionHandler(void * args) {
    struct Arguments *arg = args;

    int client_socket = arg->client_socket;
    int server_socket = arg->server_socket;
    char current_user[2];
    strcpy(current_user, arg->user);
    printf("username: %s\n", current_user);
    pwd();
    if ( chdir("simple_slash/simple_home/") != 0 ) {
        perror("Error: chdir()!\n");
    }
    if ( chdir(current_user) != 0) {
        perror("Error: chdir()!\n");
    }
    char Path[1000];
    // strncpy(Path, "simple_slash/simple_home/", 26);
    // printf("Path1: %s\n", Path);
    // strncat(Path, current_user, 2);
    // printf("len: %d\n", strlen(current_user));
    // printf("Path: %s\n", Path);
    getcwd(Path, sizeof(Path));
    char * path_ptr = strstr(Path, "simple_slash/");
    printf("Path: %s\nLen: %d\n",path_ptr, strlen(path_ptr));
    pwd();
    if ( send(client_socket, path_ptr, strlen(path_ptr), 0) == -1 ) {
        perror("Error: send error!\n");
    }
    while(1) {
        char RecvBuffer[BUFSIZE] = {0};
        int mssg_recv_status;
        if ( (mssg_recv_status = recv(client_socket, RecvBuffer, BUFSIZE, 0)) <= 0 ) {
            if ( mssg_recv_status == 0 ) {
                printf("Error: Client disconnected!\n");
            }
            else {
                perror("Error: Recv\n");
            }
            close(client_socket);
            return NULL;
        }
        // String Parser ------------------------
        // char NewBuff[strlen(RecvBuffer)-1];
        // strcpy(NewBuff, RecvBuffer);
        // NewBuff[strlen(NewBuff)] = '\0';
        int i;
        for (i=0; i<strlen(RecvBuffer); i++) {
            printf("%d\t", RecvBuffer[i]);
        }
        printf("Len: %d\n", strlen(RecvBuffer));

        char *pArg[100];
        char *parser;
        parser = strtok(RecvBuffer, " ");
        int j = 0;
        while ( parser != NULL ) {
            pArg[j] = parser;
            parser = strtok(NULL, " ");
            j++;
        }
        pArg[j] = NULL;
        printf("J: %d\n", j);
        // String Parser  ------------------------
        
        if ( strncmp(pArg[0], "cd", 2) == 0 ) {
            if ( j == 1 ) {
                if ( chdir(root) != 0 )
				{
                    if ( send(client_socket, "Path Problem!\n", 15, 0) == -1 ) {
                        perror("Error: send error!\n");
                    }
					fprintf(stderr, "%s\n", "Path Problem!");
                }
                else {
                    pwd();
                    memset(Path, '\0', sizeof(Path));
                    printf("Yeah\n");
                    strcpy(Path, root);
                    printf("Yeah\n");
                    strcat(Path, "simple_slash/");
                    chdir("simple_slash/");
                }
            }
            else if ( j == 2 ) {
                // if ( strncmp(pArg[1], "~", 1 ) == 0 ) {
                //     if ( chdir(root) != 0 )
                //     {
                //         if ( send(client_socket, "Path Problem!\n", 15, 0) == -1 ) {
                //             perror("Error: send error!\n");
                //         }
                //         fprintf(stderr, "%s\n", "Path Problem!");
                //         strcat(Path, "/")
                //     }     
                // }
                // else {
                    if ( strncmp(pArg[1], "..", 2) == 0 ) {
                        printf("Hey\n");
                        char cwd[1024];
	                    getcwd(cwd, sizeof(cwd));
                        char * LastName = SlashParser(cwd);
                        printf("LastName: %s\n", LastName);
                        if ( strcmp(LastName, "simple_slash/") == 0 ) {
                            printf("Can't go backward!\n");
                        }
                        else {
                            char actual[BUFSIZE];
                            char *ptr;
                            printf("yeah!\n");
                            ptr = realpath(pArg[1], actual);
                            chdir("..");
                            pwd();
                            printf("yeah!\n");
                            memset(Path, "\0", sizeof(Path));
                            printf("yeah!\n");
                            strcpy(Path, actual);
                            printf("%s\n", Path);
                        }
                    }
                    else {
                        char actual[BUFSIZE];
                        char *ptr;
                        ptr = realpath(pArg[1], actual);
                        int flag = 1;
                        printf("Actual: %s\n", actual);
                        strcat(actual, "/");
                        printf("pArg: %s\n", pArg[1]);
                        if ( strstr(actual, pArg[1]) != NULL ) {
                            flag = 0;
                            int fl = 1;
                            if ( fl ) {
                                FILE * fpeter;
                                char * line;
                                size_t len = 0;
                                ssize_t read;
                                char actual[BUFSIZE];
                                char * ptr;
                                ptr = realpath("usergroup", actual);
                                fpeter = fopen(ptr, 'r');
                                if (fpeter == NULL)
                                {
                                    printf("File not found!\n");
                                }
                                else {
                                    while((read = getline(&line, &len, fpeter))!= -1){
                                        char * token = strtok(line, ":");
                                        char * tkn = strtok(pArg[1], "/");
                                        if ( strncmp(token, current_user, 2) == 0 ) {
                                            token = strtok(NULL, ":");
                                            if ( strncmp(token, tkn, 2) == 0 ) {
                                                fl = 0;
                                                chdir(tkn);
                                                memset(Path, "\0", sizeof(Path));
                                                strcpy(Path, actual);
                                                break;
                                            }
                                        }
                                    }   
                                    
                                }
                            }
                            int i;
                            for(i=0; i<5; i++) {
                                if ( strncmp(usernames[i], current_user, 2) != 0 && strstr(actual, usernames[i]) != NULL ) {
                                    fl = 0;
                                    if ( send(client_socket, "Path Permission Error!\n", 24, 0) == -1 ) {
                                        perror("Error: send error!\n");
                                    }
                                    fprintf(stderr, "%s\n", "Path Problem!");
                                    break;
                                }
                            }
                            if (fl) {
                                printf("char %d\n", actual[strlen(actual)]);
                                printf("%d\n", actual[strlen(actual)-1]);

                                if ( chdir(actual) != 0 ) {
                                    pwd();
                                    printf("ActualPath: %s %d\n", actual, strlen(actual));
                                    printf("%d %d %d\n", actual[strlen(actual)-3], actual[strlen(actual)-2], actual[strlen(actual)-1]);
                                    if ( send(client_socket, "Path Problem!\n", 15, 0) == -1 ) {
                                        perror("Error: send error!\n");
                                    }
                                    fprintf(stderr, "%s\n", "Path Problem!");
                                }
                                else {
                                    pwd();
                                    memset(Path, "\0", sizeof(Path));
                                    strcpy(Path, actual);
                                }
                            }
                        }
                    }
                    // strcpy(Path, "/");
                    // strstr
                    // char * token = strtok(pArg[1], "/");
                    // printf("Parg %s\n", pArg[1]);
                    // while (token != NULL) { 
                    //     printf("tok %s\n%d\n", token, strlen(token));
                    //     if ( strlen(token) == 2 && strncmp(token, ".", 1) == 0) {
                    //         printf("Stay Here\n");
                    //     }
                    //     else if ( strncmp(token, "..", 2) == 0 ) {
                            // printf("Hey\n");
                            // char cwd[1024];
	                        // getcwd(cwd, sizeof(cwd));
                            // char * LastName = SlashParser(cwd);
                            // printf("LastName: %s\n", LastName);
                            // if ( strncmp(LastName, "simple_slash", 13) == 0 ) {
                            //     printf("Can't go backward!\n");
                            // }
                    //         else {
                    //             if ( chdir("..") != 0 )
                    //             {
                    //                 if ( send(client_socket, "Path Problem!\n", 15, 0) == -1 ) {
                    //                     perror("Error: send error!\n");
                    //                 }
                    //                 fprintf(stderr, "%s\n", "Path Problem!");
                    //                 break;
                    //             }  
                    //         }
                    //     } 
                    //     else {
                    //         printf("token: %s\n", token);
                    //         int flag = 1;
                    //         int iter;
                    //         for (iter=0; iter<6; iter++) {
                    //             if ( strncmp(token, usernames[iter], 2) == 0 && strncmp(current_user, usernames[iter], 2) != 0 ) {
                    //                 int val = checkCDPermissions(current_user, usernames[iter]);
                    //                 if ( val == 1 ) {
                    //                     flag = 1;
                    //                     break;
                    //                 }
                    //                 else {
                    //                     flag = 0;
                    //                     if ( send(client_socket, "Permission Error!\n", 19, 0) == -1 ) {
                    //                         perror("Error: send error!\n");
                    //                     }
                    //                     fprintf(stderr, "%s\n", "Permission Error!");
                    //                 }
                    //             }
                    //         }
                    //         if ( flag == 1 ) {
                    //             char temp[BUFSIZE];
                    //             memset(temp, "\0", sizeof(temp));
                    //             strcpy(temp, token);
                    //             temp[strlen(temp)] = '\0';
                    //             printf("Temp: %s\n", temp);
                    //             if ( chdir(temp) != 0 )
                    //             {
                    //                 if ( send(client_socket, "Path Problem!\n", 15, 0) == -1 ) {
                    //                     perror("Error: send error!\n");
                    //                 }
                    //                 flag = 0;
                    //                 fprintf(stderr, "%s\n", "Path Problem!");
                    //             }
                    //         }
                    //         else if ( flag == 0 ) {
                    //             break;
                    //         }
                    //     }
                    //     pwd();
                    //     token = strtok(NULL, "/"); 
                    // } 
                // }
            }
        }
        else if ( strncmp(pArg[0], "create_dir", 11) == 0 ) {
            if ( pArg[1] != NULL ) {
                char currentwd[1024];
                getcwd(currentwd, sizeof(currentwd));

                // printf("ptr: %s\n", ptr);
                // printf("Actual: %s\n", actual);

                int i;
                int flag = 1;
                for (i=0; i<5; i++) {
                    if ( strncmp(usernames[i], current_user, 2) != 0 ) {
                        if ( strstr(pArg[1], usernames[i]) != NULL ) {
                            if ( send(client_socket, "Permission error!\n", 19, 0) == -1 ) {
                                perror("Error: send error!\n");
                            }
                            flag = 0;
                            break;
                        }
                    }
                }
                if (flag == 1) {
                    for (i=0; i<strlen(pArg[1]); i++) {
                        if ( pArg[1][i] == '.' || pArg[1][i] == '\'' || pArg[1][i] == '^' || pArg[1][i] == '&' || pArg[1][i] == '%' || pArg[1][i] == '$' || pArg[1][i] == '#' || pArg[1][i] == '@' || pArg[1][i] == '!' || pArg[1][i] == '~' ) {
                            if ( send(client_socket, "Error: File Name!\n", 19, 0) == -1 ) {
                                perror("Error: send error!\n");
                            }
                            break;
                        }
                    }
                    printf("pArg[1]: %s\n%d\n", pArg[1], strlen(pArg[1]));
                    char na[strlen(pArg[1])];
                    strcpy(na, pArg);
                    na[strlen(pArg[1])] = '\0';
                    if ( mkdir(pArg[1], S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) < 0 ) {
                        if ( send(client_socket, "Error: File Name!\n", 19, 0) == -1 ) {
                            perror("Error: send error!\n");
                        }
                    }
                    else {
                        // inherit permissions and add Entry to file System.
                        char * token = SlashParser(currentwd);
                        printf("Token %s\n", token);
                    }
                }
            }
            else {
                if ( send(client_socket, "Too few Arguments\n", 19, 0) == -1 ) {
                    perror("Error: send error!\n");
                }
            }
        }
        else if ( strncmp(pArg[0], "fput", 4) == 0 ) {
            printf("parg1: %s\n", pArg[1]);
            int val = checkPath(pArg[1], current_user);
            if ( val == 0 ) {
                if ( send(client_socket, "Permission error!\n", 19, 0) == -1 ) {
                    perror("Error: send error!\n");
                }
            }
            else {
                FILE* fptr;
                fptr = fopen(pArg[1], "a");
                if (fptr == NULL) {
                    printf("Error: while creating file!\n");
                    if ( send(client_socket, "Error: while creating file!\n", 29, 0) == -1 ) {
                        perror("Error: send error!\n");
                    }
                }
                else {
                    int i = 2;
                    while(pArg[i] != NULL) {
                        fputs(pArg[i], fptr);
                        fputs(" ", fptr);
                        i++;
                    }
                    printf("Successfully written to the file!\n");
                }
                fclose(fptr);
            }
        }
        else if ( strncmp(pArg[0], "fget", 4 ) == 0) {
            // printf("%s\t%s\n", RecvBuffer, temp);
            if (pArg[1] == NULL) {
                printf("fput <filename>\n");
            }
            else {
                char * file = SlashParser(pArg[1]);
                printf("Last String: %s\n", file);
                char * line;
                int Read_Access = 0;
                int Write_Access = 0;
                if ((line = Exist(file)) != NULL) {
                    char * token = strtok(line, ":");
                    printf("Token: %s\n", token);
                    token = strtok(NULL, ":");
                    printf("Token: %s\n", token);
                    token = strtok(NULL, ":");
                    printf("Token: %s\n", token);
                    if ( strncmp(current_user, token, strlen(token)) == 0 ) {
                        Read_Access = 1;
                        Write_Access = 1;
                    }
                    else {
                        printf("Yip, %s\n", token);
                        token = strtok(NULL, ":");
                        char * group = getUserGroup(current_user);
                        if ( strcmp(token, group) == 0 ) {
                            Read_Access = 1;
                        }
                    }
                }
                printf("%d %d\n", Read_Access, Write_Access);
                if ( Read_Access == 1 && Write_Access == 1 ) {
                    // strcpy(temp, RecvBuffer);
                    // printf("Input Name: %s\n", temp);
                    // char * token = strtok(temp, " ");
                    // token = strtok(NULL, " ");
                    // printf("%d\n", strlen(token));
                    // // token = strtok(NULL, " ");
                    FILE* fp;
                    // // memset(temp, '\0', sizeof(temp));
                    char temp2[strlen(pArg[1])];
                    memset(temp2, '\0', sizeof(temp2));
                    strncpy(temp2, pArg[1], strlen(pArg[1])-1);
                    printf("File Name: %s %d\n", pArg[1], strlen(pArg[1]));
                    pwd();
                    printf("pArg[1]: %s\n", pArg[1]);
                    fp = fopen(temp2, "r+");
                    char * line = NULL;
                    size_t len = 0;
                    ssize_t read;
                    if ( fp == NULL ) {
                        perror("Error: While opening a file!\n");
                    }
                    else {
                        int i=0;
                        while ((read = getline(&line, &len, fp)) != -1) {
                            // printf("Retrieved line of length %zu:\n", read);
                            strcat(line, "\n");
                            if ( send(client_socket, line, strlen(line), 0) == -1 ) {
                                perror("Error: send error!\n");
                            }
                            // printf("%s\n", line);
                            i++;
                        }
                    }
                    fclose(fp);
                    if (line) {
                        free(line);
                    }
                }
            }
        }

        else if ( strncmp(RecvBuffer, "ls", 2) == 0 ) {
            printf("Ls Command called\n");
            struct dirent **names;
            int n;
            if ( strlen(RecvBuffer) <= 3 )
            {
                n = scandir(".", &names, NULL, alphasort);
                if ( n == NULL )
                {
                    ErrorLs();
                }
                else if ( n < 0 )
                {
                    SyntaxErrorLs("ls");
                }
                else
                {
                    pwd();
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
                        strcat(names[i]->d_name, "\n");
                        if ( send(client_socket, names[i]->d_name, strlen(names[i]->d_name), 0) == -1 ) {
                            perror("Error: send error!\n");
                        }
                        free(names[i]);
                        i++;
                    }
                    printf("\n");
                    free(names);
                }
                printf("Print Done\n");
            }
            else
            {
                char *token = strtok(RecvBuffer, " ");
                token = strtok(NULL, "-"); 
                printf("Token: %s\n", token); 
                printf("%d\n", strlen(token));
                char tl[strlen(token)-1];
                strncpy(tl, token, strlen(token)-1);
                n = scandir(tl, &names, NULL, alphasort);
                if ( n < 0 )
                {
                    ErrorLs();
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
                        char fakePath[BUFSIZE];
                        strcpy(fakePath, root);
                        printf("fakePath: %s\n%d\n", fakePath, strlen(fakePath));
                        strcat(fakePath, "/simple_slash/fileDetails.txt");
                        printf("fakePath: %s\n%d\n", fakePath, strlen(fakePath));
                        char * line = NULL;
                        size_t len = 0;
                        ssize_t read;
                        FILE *fptr;
                        if ((fptr = fopen(fakePath, "r")) == NULL) {
                            printf("Error: opening file!\n");
                            free(names[i]);
                            i++;
                            continue;
                        }
                        int flag = 0;
                        char *token;
                        while ((read = getline(&line, &len, fptr)) != -1) {
                            // printf("Retrieved line of length %zu:\n", read);
                            token = strtok(line, ":");
                            printf("Token: %s\n", token);
                            printf("%s\n", line);
                            if (strcmp(line, names[i]->d_name) == 0) {
                                token = strtok(NULL, ":");
                                flag = 1;
                                break;
                            }
                        }
                        printf("%d\n", flag);
                        if ( flag ) {
                            printf("%s\n", (names[i]->d_name));
                            strcat(names[i]->d_name, "\n");
                            token = strtok(NULL, ":");
                            // printf("%s\n", token);
                            strcat(names[i]->d_name, "Owner: ");
                            strcat(names[i]->d_name, token);
                            strcat(names[i]->d_name, "\n");
                            token = strtok(NULL, ":");
                            strcat(names[i]->d_name, "Group: ");
                            strcat(names[i]->d_name, token);
                            // printf("%s\n", token);
                            strcat(names[i]->d_name, "\n");
                            if ( send(client_socket, names[i]->d_name, strlen(names[i]->d_name), 0) == -1 ) {
                                perror("Error: send error!\n");
                            }
                        }
                        free(names[i]);
                        i++;
                    }
                    free(names);
                    printf("\n");
                }
            }
        }
        else if (fork() == 0) {
            char *token = strtok(RecvBuffer, " ");
            char temp[BUFSIZE];
            strcpy(temp, root);
            strcat(temp, token);
            printf("Temp: %s\n", temp);
            token = strtok(NULL, " ");
            printf("Token: %s\n", token);
            if (execvp(temp, token) < 0) {
                perror("Error: Could not execute command!\n");
            }
        }
        else {
            wait(NULL);
        }
        char * path_ptr = strstr(Path, "simple_slash");
        printf("Path: %s\nLen: %d\n",path_ptr, strlen(path_ptr));
        if ( send(client_socket, path_ptr, strlen(path_ptr), 0) == -1 ) {
            perror("Error: send error!\n");
        }
    }
}

int main(int argc, char const *argv[]) {
    int ports[CONNECTIONS];
    int fds[CONNECTIONS];
    
    int fd_server, socket_new;
    struct sockaddr_in sock_address;
    int len_address = sizeof(sock_address);
    
    char Buffer[BUFSIZ];

    if ( (fd_server = socket(AF_INET, SOCK_STREAM, 0)) == 0 ) {
        perror("Socket Creation Fail!\n");
        exit(1);
    }

    sock_address.sin_family = AF_INET;
    sock_address.sin_addr.s_addr = INADDR_ANY;
    sock_address.sin_port = htons(PORT);

    memset(sock_address.sin_zero, '\0', sizeof sock_address.sin_zero);

    if ( bind(fd_server, (struct sockaddr *)&sock_address, sizeof(struct sockaddr)) < 0 ) {
        perror("Bind Fail!\n");
        exit(1);
    }

    if ( listen(fd_server, 10) < 0 ) {
        perror("Listen Fail!\n");
        exit(1);
    }

    fds[fd_server] = fd_server;

    readUsername();
    pwd();
    getcwd(root, sizeof(root));

    while(1) {

        int fd_client;
        if ( (fd_client = accept(fd_server, (struct sockaddr *)&sock_address, &len_address)) < 0 ) {
            perror("Accept Fail!\n");
            exit(1);
        }

        printf("Client: %d\n", ntohs(sock_address.sin_port));
        fds[fd_client] = fd_client;
        struct Arguments args;
        args.server_socket = fd_server;
        args.client_socket = fd_client;

        int count = 3;
        int flag = 1;
        while(count-- > 0) {
            printf("Hey\n");
            if ( send(fd_client, "Enter Username: ", 17, 0) == -1 ) {
                perror("Send Error!\n");
            }
            printf("MssgSent!\n");
            int MssgRecvStatus;
            char Buffer[BUFSIZ] = {0};
            if ( (MssgRecvStatus = recv(fd_client, Buffer, BUFSIZE, 0)) < 0 ) {
                perror("Recv Error!\n");
                close(fd_client);
                fds[fd_client] = -1;
            }
            int iter;
            for ( iter=0; iter < sizeof(usernames); iter++ ) {
                if ( strncmp(usernames[iter], Buffer, 2) == 0 ) {
                    flag = 0;
                    break;
                }
            }
            if ( flag == 0 ) {
                if ( send(fd_client, "Login Successful!\n", 19, 0) == -1 ) {
                    perror("Send Error!\n");
                }
                strncpy(args.user, Buffer, 2);
                break;
            }
            sleep(1);
        }
        if ( flag == 1 ) {
            if ( send(fd_client, "Wrong Credentials!\n", 20, 0) == -1 ) {
                perror("Send Error!\n");
            }
            close(fd_client);
            continue;
        }

        int error;
        if ( (error = pthread_create(&(tid[fd_client]), NULL, ConnectionHandler, (void *)&args)) != 0 ) {
            perror("Thread can\'t be created!\n");
        }
    }
    close(fd_server);
    return 0;
}

