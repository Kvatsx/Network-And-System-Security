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

char usernames[5][1024];
int LoginStatus[5] = {0, 0, 0, 0 ,0};

char FilesData[10][1024];
int FileCount;

struct Arguments {
    int client_socket;
    int server_socket;
    char user[2];
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

void ReadFileData() {
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    char file_loc[BUFSIZE];
    memset(file_loc, "\0", sizeof(file_loc));
    strcpy(file_loc, root);
    strcat(file_loc, "/etc/fileDetails.txt");
    printf("File_loc: %s\n", file_loc);
    fp = fopen(file_loc, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1) {
        strcpy(FilesData[FileCount], line);
        FileCount += 1;
        // printf("%s", line);
    }

    fclose(fp);
    if (line)
        free(line);
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
    char file_loc[BUFSIZE];
    memset(file_loc, "\0", sizeof(file_loc));
    strcpy(file_loc, root);
    strcat(file_loc, "/etc/usergroup");
    printf("File Path: %s\n", file_loc);
    fp = fopen(file_loc, "r");
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
        strcpy(usernames[i], line);
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
    char current_user[3];
    strcpy(current_user, arg->user);
    current_user[2] = '\0';
    printf("username: %s-\n", current_user);

    char Path[1024];
    getcwd(Path, sizeof(Path));
    printf("%s\n", Path);
    strcat(Path, "/simple_slash/simple_home/");
    printf("%s\n", Path);
    strcat(Path, current_user);
    printf("Path %s\n", Path);
    char * path_ptr = strstr(Path, "simple_slash/");
    printf("Path: %s\nLen: %d\n",path_ptr, strlen(path_ptr));

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
            int it;
            for(it=0; it<5; it++) {
                if (strncmp(usernames[it], current_user, 2) == 0) {
                    LoginStatus[it] = 0;
                }
            }
            close(client_socket);
            return NULL;
        }
        // String Parser ------------------------
        // char NewBuff[strlen(RecvBuffer)-1];
        // strcpy(NewBuff, RecvBuffer);
        // NewBuff[strlen(NewBuff)] = '\0';
        // int i;
        // for (i=0; i<strlen(RecvBuffer); i++) {
            // printf("%d\t", RecvBuffer[i]);
        // }
        // printf("Len: %d\n", strlen(RecvBuffer));

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
                char line[1024];
                memset(line, "\0", sizeof(line));
                strcpy(line, root);
                strcat(line, "/simple_slash");
                memset(Path, "\0", sizeof(Path));
                strcpy(Path, line);
            }
            else if ( j == 2 ) {
                // if ( strcmp(pArg[1], "..") == 0 ) {
                printf("Hey\n");
                char actual[1024];
                memset(actual, "\0", sizeof(actual));

                char line[1024];
                memset(line, "\0", sizeof(line));

                strcpy(line, Path);
                strcat(line, "/");
                strcat(line, pArg[1]);

                char *ptr;
                ptr = realpath(line, actual);
                printf("actual: %s\n%s\n", actual, ptr);


                char temp[1024];
                memset(temp, "\0", sizeof(temp));
                strcpy(temp, actual);
                char * LastName = SlashParser(temp);
                printf("LastName: %s\n%d\n", LastName, strlen(LastName));
                if ( strstr(actual, "simple_slash") == NULL ) {
                    if ( send(client_socket, "Wrong Path\n", 12, 0) == -1 ) {
                        perror("Error: send error!\n");
                    }
                }
                // else if ( strstr(actual, ".") == NULL ) {
                //     if ( send(client_socket, "Error: Wrong Path\n", 19, 0) == -1 ) {
                //         perror("Error: send error!\n");
                //     }
                // }
                else if ( strcmp(LastName, "simple_home") == 0 ) {
                    memset(Path, "\0", sizeof(Path));
                    strcpy(Path, actual);   
                }
                else if ( strcmp(LastName, "simple_slash") == 0 ) {
                    memset(Path, "\0", sizeof(Path));
                    strcpy(Path, actual);   
                }
                else {
                    printf("Else:\n");
                    int flag = 0;
                    int iter;
                    for(iter=0; iter<FileCount; iter++) {
                        char line2[1024];
                        memset(line2, "\0", sizeof(line2));
                        strcpy(line2, FilesData[iter]);
                        char * tok = strtok(line2, ":");
                        char * pf = strstr(actual, tok);
                        if ( pf != NULL && strcmp(pf, tok) == 0 ) {
                            tok = strtok(NULL, ":");
                            char * owner = tok;
                            tok = strtok(NULL, ":");
                            char * group = tok;
                            if ( strcmp(current_user, owner) == 0 ) {
                                flag = 1;
                                break;
                            }
                            else if ( strcmp(owner, "-") == 0 ) {
                                flag = 1;
                                break;
                            }
                            else {
                                int iter2;
                                for (iter2=0; iter2<5; iter2++) {
                                    char line3[1024];
                                    memset(line3, "\0", sizeof(line3));
                                    strcpy(line3, usernames[iter2]);
                                    char * tok = strtok(line3, ":");
                                    if ( strcmp(current_user, tok) == 0 ) {
                                        tok = strtok(NULL, ":");
                                        while(tok != NULL) {
                                            if ( strcmp(group, tok) == 0 ) {
                                                flag = 1;
                                                break;
                                            }
                                            tok = strtok(NULL, ":");
                                        }
                                    }
                                    if (flag) {
                                        break;
                                    }
                                }
                            }
                            if (flag) {
                                break;
                            }
                        }
                    }
                    if (flag) {
                        memset(Path, "\0", sizeof(Path));
                        strcpy(Path, actual);   
                    }
                    else {
                        if ( send(client_socket, "Path Permission Error!\n", 24, 0) == -1 ) {
                            perror("Error: send error!\n");
                        }
                    }
                }
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
            if ( j >= 3 ) {
                char actual[1024];
                memset(actual, "\0", sizeof(actual));
                char *ptr;
                char buff[1024];
                strcpy(buff, Path);
                strcat(buff, "/");
                strcat(buff, pArg[1]);
                printf("%s\n", pArg[1]);
                printf("buff %s\n", buff);
                ptr = realpath(buff, actual);
                printf("actual: %s\nptr: %s\n\n", actual, ptr);

                int flag = 0;
                int iter;
                for(iter=0; iter<FileCount; iter++) {
                    char line[1024];
                    memset(line, "\0", sizeof(line));
                    strcpy(line, FilesData[iter]);
                    char * token = strtok(line, ":");
                    if ( strstr(actual, token) != NULL ){
                        token = strtok(NULL, ":");
                        if ( strcmp(token, current_user) == 0 ) {
                            flag = 1;
                        }
                    }
                }
                printf("flag: %d\n", flag);
                int file_found = 1;
                FILE *file;
                if ((file = fopen(actual, "r")) == NULL)
                {
                    printf("kya bakc\n");                
                    file_found = 1;
                }
                else {
                    file_found = 0;
                    fclose(file);
                }
            
                printf("flag: %d\n", file_found);

                FILE * fp;
                char * line = NULL;
                size_t len = 0;
                ssize_t read;

                if ( flag ) {
                    fp = fopen(actual, "a");
                    if (fp == NULL) {
                        if ( send(client_socket, "Error: File not found\n", 23, 0) == -1 ) {
                            perror("Error: send error!\n");
                        }
                    }
                    else {
                        int i = 2;
                        while(pArg[i] != NULL) {
                            fputs(pArg[i], fp);
                            fputs(" ", fp);
                            i++;
                        }
                        fputs("\n", fp);
                        printf("Successfully written to the file!\n");
                    }

                    fclose(fp);
                    if (line)
                        free(line);

                    if (file_found) {
                        char buf[1024];
                        memset(buf, "\0", sizeof(buf));
                        char newfile[1024];
                        memset(newfile, "\0", sizeof(newfile));

                        strcpy(newfile, actual);

                        char* tkn = strstr(newfile, "simple_slash/");
                        strcpy(buf, tkn);
                        strcat(buf, ":");

                        char rec[BUFSIZE] = {0};
                        int status;
                        if ( (status = recv(client_socket, rec, BUFSIZE, 0)) <= 0 ) {
                            if ( status == 0 ) {
                                printf("Error: Client disconnected!\n");
                            }
                            else {
                                perror("Error: Recv\n");
                            }
                            int it;
                            for(it=0; it<5; it++) {
                                if (strncmp(usernames[it], current_user, 2) == 0) {
                                    LoginStatus[it] = 0;
                                }
                            }
                            close(client_socket);
                            return NULL;
                        }
                        strcat(buf, rec);
                        strcat(buf, ":");
                        if ( (status = recv(client_socket, rec, BUFSIZE, 0)) <= 0 ) {
                            if ( status == 0 ) {
                                printf("Error: Client disconnected!\n");
                            }
                            else {
                                perror("Error: Recv\n");
                            }
                            int it;
                            for(it=0; it<5; it++) {
                                if (strncmp(usernames[it], current_user, 2) == 0) {
                                    LoginStatus[it] = 0;
                                }
                            }
                            close(client_socket);
                            return NULL;
                        }
                        strcat(buf, rec);
                        strcat(buf, "\n");
                        strcpy(FilesData[FileCount], buf);
                        FileCount++;
                        printf("File: %s\n", FilesData[FileCount-1]);
                    }
                }
                else {
                    int fl = 1;
                    int iter2;
                    for(iter2=0; iter2<5; iter2++) {
                        char line[1024];
                        memset(line, "\0", sizeof(line));
                        strcpy(line, actual);
                        char line2[1024];
                        memset(line2, "\0", sizeof(line2));
                        strcpy(line2, usernames[iter2]);
                        char * tk =strtok(line2, ":");

                        if (strstr(line, tk) != NULL && strcmp(current_user, tk) != 0) {
                            fl = 0;
                            if ( send(client_socket, "Error: File permission\n", 24, 0) == -1 ) {
                                perror("Error: send error!\n");
                            }
                            break;
                        }
                    }
                    if (fl) {
                        fp = fopen(actual, "a");
                        if (fp == NULL) {
                            if ( send(client_socket, "Error: File not found\n", 23, 0) == -1 ) {
                                perror("Error: send error!\n");
                            }
                        }
                        else {
                            printf("writting\n");                        
                            int i = 2;
                            while(pArg[i] != NULL) {
                                fputs(pArg[i], fp);
                                fputs(" ", fp);
                                i++;
                            }
                            fputs("\n", fp);
                            printf("Successfully written to the file!\n");
                        }

                        fclose(fp);
                        if (line)
                            free(line);

                        if (file_found) {
                            char buf[1024];
                            memset(buf, "\0", sizeof(buf));
                            char newfile[1024];
                            memset(newfile, "\0", sizeof(newfile));

                            strcpy(newfile, actual);

                            char* tkn = strstr(newfile, "simple_slash/");
                            strcpy(buf, tkn);
                            strcat(buf, ":");

                            char rec[BUFSIZE] = {0};
                            int status;
                            if ( (status = recv(client_socket, rec, BUFSIZE, 0)) <= 0 ) {
                                if ( status == 0 ) {
                                    printf("Error: Client disconnected!\n");
                                }
                                else {
                                    perror("Error: Recv\n");
                                }
                                close(client_socket);
                                return NULL;
                            }
                            strcat(buf, rec);
                            strcat(buf, ":");
                            if ( (status = recv(client_socket, rec, BUFSIZE, 0)) <= 0 ) {
                                if ( status == 0 ) {
                                    printf("Error: Client disconnected!\n");
                                }
                                else {
                                    perror("Error: Recv\n");
                                }
                                close(client_socket);
                                return NULL;
                            }
                            strcat(buf, rec);
                            strcat(buf, "\n");
                            strcpy(FilesData[FileCount], buf);
                            FileCount++;
                            printf("File: %s\n", FilesData[FileCount-1]);
                        }
                    }
                }
            }
            else {
                if ( send(client_socket, "Error: Few Arguments\n", 23, 0) == -1 ) {
                    perror("Error: send error!\n");
                }
            }
            
        }
        else if ( strncmp(pArg[0], "fget", 4 ) == 0) {
            // printf("%s\t%s\n", RecvBuffer, temp);
            if (pArg[1] == NULL && j == 2) {
                printf("fput <filename>\n");
            }
            else {
                char actual[1024];
                memset(actual, "\0", sizeof(actual));
                char *ptr;
                char buff[1024];
                strcpy(buff, Path);
                strcat(buff, "/");
                strcat(buff, pArg[1]);
                ptr = realpath(buff, actual);
                printf("actual: %s\nptr: %s\n\n", actual, ptr);

                int flag = 0;
                int iter;
                for (iter=0; iter<FileCount; iter++) {
                    char line[1024];
                    strcpy(line, FilesData[iter]);
                    char * token = strtok(line, ":");
                    // if file found
                    if (strstr(actual, token) != NULL) {
                        token = strtok(NULL, ":");
                        char * user = token;
                        token = strtok(NULL, ":");
                        char * group = token;

                        if ( strcmp(current_user, user) == 0 ) {
                            flag = 2;
                        }
                        else {
                            // Check for group
                            int iter2;
                            for (iter2=0; iter2<5; iter2++) {
                                char line2[1024];
                                strcpy(line2, usernames[iter2]);
                                char * tkn = strtok(line2, ":");
                                if ( strcmp(tkn, current_user) == 0 ) {
                                    tkn = strtok(NULL, ":");
                                    while(tkn != NULL) {
                                        if ( strcmp(tkn, group) == 0 ) {
                                            flag = 1;
                                        }
                                        tkn = strtok(NULL, ":");
                                    }
                                }
                            }
                        }
                    }
                }
                if ( flag == 1 ) {
                    FILE * fp;
                    char * line = NULL;
                    size_t len = 0;
                    ssize_t read;

                    fp = fopen(actual, "r");
                    if (fp == NULL){
                        if ( send(client_socket, "Error: File not found\n", 23, 0) == -1 ) {
                            perror("Error: send error!\n");
                        }
                    }
                    else {
                        while ((read = getline(&line, &len, fp)) != -1) {
                            printf("%s", line);
                            // strcat(line, "\n");
                            if ( send(client_socket, line, strlen(line), 0) == -1 ) {
                                perror("Error: send error!\n");
                            }  
                        }
                    }
                    fclose(fp);
                    if (line)
                        free(line);
                }
                else if ( flag == 2 ) {
                    FILE * fp;
                    char * line = NULL;
                    size_t len = 0;
                    ssize_t read;

                    fp = fopen(actual, "r");
                    if (fp == NULL){
                        if ( send(client_socket, "Error: File not found\n", 23, 0) == -1 ) {
                            perror("Error: send error!\n");
                        }
                    }
                    else {
                        while ((read = getline(&line, &len, fp)) != -1) {
                            printf("%s", line);
                            strcat(line, "\n");
                            if ( send(client_socket, line, strlen(line), 0) == -1 ) {
                                perror("Error: send error!\n");
                            }  
                        }
                    }
                    fclose(fp);
                    if (line)
                        free(line);
                }
                else {
                    if ( send(client_socket, "Error: File not found\n", 23, 0) == -1 ) {
                        perror("Error: send error!\n");
                    }
                }
            }
        }

        else if ( strncmp(pArg[0], "ls", 2) == 0 ) {
            printf("Ls Command called\n");
            struct dirent **names;
            int n;
            if ( pArg[1] == NULL )
            {
                n = scandir(Path, &names, NULL, alphasort);
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
                    printf("Current Path: %s\n", Path);
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
                        
                        int iter;
                        for (iter=0; iter<FileCount; iter++) {
                            char line[1024];
                            strcpy(line, FilesData[iter]);
                            char *token = strtok(line, ":");
                            printf("Per Token: %s\n", token);
                            char tempPath[1024];
                            strcpy(tempPath, Path);
                            strcat(tempPath, "/");
                            strcat(tempPath, names[i]->d_name);
                            printf("temp: %s\n", tempPath);
                            printf("token: %s\n", token);
                            if ( strstr(tempPath, token) != NULL ) {
                                token = strtok(NULL, ":");
                                strcat(names[i]->d_name, token);
                                strcat(names[i]->d_name, "\n");
                                token = strtok(NULL, ":");
                                strcat(names[i]->d_name, token);
                                strcat(names[i]->d_name, "\n");
                                break;
                            }
                        }

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
                // char *token = strtok(pArg[1], " ");
                // token = strtok(NULL, "-"); 
                // printf("Token: %s\n", token); 
                // printf("%d\n", strlen(token));
                // char tl[strlen(token)-1];
                // strncpy(tl, token, strlen(token)-1);
                char actual[1024];
                memset(actual, "\0", sizeof(actual));
                // strcpy(actual, Path);
                // strcat(actual, "/");
                char buff[1024];
                strcpy(buff, Path);
                strcat(buff, "/");
                strcat(buff, pArg[1]);
                char *ptr;
                ptr = realpath(buff, actual);
                printf("actual: %s\n%s\n", actual, ptr);

                // strcat(actual, )


                n = scandir(actual, &names, NULL, alphasort);
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
                        printf("%s\n", (names[i]->d_name));
                        strcat(names[i]->d_name, "\n");
                        int iter;
                        for (iter=0; iter<FileCount; iter++) {
                            char line[1024];
                            strcpy(line, FilesData[iter]);
                            char *token = strtok(line, ":");
                            printf("Per Token: %s\n", token);
                            char tempPath[1024];
                            strcpy(tempPath, actual);
                            strcat(tempPath, "/");
                            strcat(tempPath, names[i]->d_name);
                            printf("temp: %s\n", tempPath);
                            printf("token: %s\n", token);
                            if ( strstr(tempPath, token) != NULL ) {
                                token = strtok(NULL, ":");
                                strcat(names[i]->d_name, token);
                                strcat(names[i]->d_name, "\n");
                                token = strtok(NULL, ":");
                                strcat(names[i]->d_name, token);
                                strcat(names[i]->d_name, "\n");
                                break;
                            }
                        }
                        if ( send(client_socket, names[i]->d_name, strlen(names[i]->d_name), 0) == -1 ) {
                            perror("Error: send error!\n");
                        }
                        free(names[i]);
                        i++;
                    }
                    free(names);
                    printf("\n");
                }
            }
        }
        else {
            char arr[1024];
            strcpy(arr, pArg[0]);
            strcat(arr, " Wrong Command!\n");
            if ( send(client_socket, arr, strlen(arr), 0) == -1 ) {
                perror("Error: send error!\n");
            }
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

    getcwd(root, sizeof(root));
    printf("%s\n", root);
    readUsername();
    pwd();
    FileCount = 0;
    ReadFileData();

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

        int flag = 1;
        while(1) {
            sleep(1);
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
            if ( strlen(Buffer) < 2 ) {
                if ( send(fd_client, "Enter valid user!\n", 19, 0) == -1 ) {
                    perror("Send Error!\n");
                }
                continue;
            }
            int iter;
            for ( iter=0; iter < sizeof(usernames); iter++ ) {
                if ( strncmp(usernames[iter], Buffer, 2) == 0 ) {
                    if ( LoginStatus[iter] == 0 ) {
                        flag = 0;
                        LoginStatus[iter] = 1;
                        break;
                    }
                    else {
                        if ( send(fd_client, "User Already Logged In!\n", 25, 0) == -1 ) {
                            perror("Send Error!\n");
                        }
                        break;
                    }
                    
                }
            }
            if ( flag == 0 ) {
                if ( send(fd_client, "Login Successful!\n", 19, 0) == -1 ) {
                    perror("Send Error!\n");
                }
                strncpy(args.user, Buffer, 2);
                break;
            }
        }
        if ( flag == 1 ) {
            if ( send(fd_client, "Try Again!\n", 12, 0) == -1 ) {
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

