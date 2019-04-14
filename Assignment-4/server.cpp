/*
 * @Author: Kaustav Vats 
 * @Roll-Number: 2016048 
 */

#include "helper.h" 

using namespace std;

struct UserFD {
    string username;
    int fd;
};


pthread_t tid[CONNECTIONS];
map<string, string> Credentials;
map<string, int> LoginStatus;
map<string, string> Ticket;
map<string, int> Fd_map;
map<string, string> Key;
map<string, string> Iv;
map<int, list<string>> Groups;

void getKeyIv() {
    
    FILE * fd;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fd = fopen("/etc/shadow", "r");
    if (fd == NULL) {
        printf("File could not be opened.\n");
        perror("open");
        return;
    }

    while ((read = getline(&line, &len, fd)) != -1) {

        char * token = strtok(line, ":");
        for (auto i = Credentials.begin(); i != Credentials.end(); i++) {
            string lol(token);
            string name(i->first);
            if (lol.compare(i->first) == 0) {
                token = strtok(NULL, ":");
                int i;
                unsigned char * key;
                unsigned char * iv;
                key = (unsigned char *) malloc(sizeof(unsigned char) * 32);   
                iv = (unsigned char *) malloc(sizeof(unsigned char) * 16); 
                if( PKCS5_PBKDF2_HMAC_SHA1(token, strlen(token), NULL, 0, 100, 32, key) == 0 ) {
                    perror("PBKDF");
                }
                if( PKCS5_PBKDF2_HMAC_SHA1(token, strlen(token), NULL, 0, 50, 16, iv) == 0 ) {
                    perror("PBKDF");
                }
                string key2((char*) key);
                string iv2((char *) iv);
                
                Key.insert(pair<string, string>(name, key2));
                Iv.insert(pair<string, string>(name, iv2));
                
                // close(fd);
                // return out;
            }
        }
    }
    fclose(fd);
    // return NULL;
}

void * KDC_Server(void * argv) {
    int fd_kdc;
    int sock_kdc;
    int opt = 1;
    struct sockaddr_in addr_kdc;
    int addrlen = sizeof(addr_kdc);

    if ((fd_kdc = socket(AF_INET, SOCK_STREAM, 0)) == 0)  { 
        perror("Socket failed"); 
        pthread_exit(NULL);
    } 

    if (setsockopt(fd_kdc, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) { 
        perror("Setsockopt failed"); 
        pthread_exit(NULL);
    } 
    addr_kdc.sin_family = AF_INET; 
    addr_kdc.sin_addr.s_addr = INADDR_ANY; 
    addr_kdc.sin_port = htons(KDC_PORT); 

    if (bind(fd_kdc, (struct sockaddr *)&addr_kdc, sizeof(addr_kdc))<0) { 
        perror("Bind failed"); 
        pthread_exit(NULL);
    } 
    if (listen(fd_kdc, 3) < 0) { 
        perror("Listen"); 
        pthread_exit(NULL);
    } 
    cout << "--------------- KDC Live -----------------" << endl;

    while (1) {
        // KDC Connections
        if ((sock_kdc = accept(fd_kdc, (struct sockaddr *)&addr_kdc, (socklen_t*)&addrlen))<0)  { 
            perror("accept"); 
            pthread_exit(NULL);
        }
        char username[100];
        char password[100];
        if (recv(sock_kdc, username, 100, 0) <= 0) {
            cout << "Client Closed or Recv Error" << endl;
        }
        sleep(1);
        if (recv(sock_kdc, password, 100, 0) <= 0) {
            cout << "Client Closed or Recv Error" << endl;
        }
        string user(username);
        string pass(password);
        // cout << "Received Username and Password" << endl;
        // cout << "Username: " << user << "\nPassword: " << pass << endl;

        cout << "Hehe\t" << Credentials.find(user)->second << endl;

        if (Credentials.find( user ) != Credentials.end() && Credentials.find(user)->first.compare(pass) == 0) {
            cout << "User found" << endl;
            char reply[] = "Login successful!";
            if (send(sock_kdc, reply, strlen(reply), 0) == -1) {
                perror("send error\n");
            }
            // TODO: Ticket implementation
            unsigned char *out;
            out = (unsigned char *) malloc(sizeof(unsigned char) * 32);
            if( PKCS5_PBKDF2_HMAC_SHA1(password, strlen(password), NULL, 0, 100, 32, out) == 0 ) {
                perror("PBKDF");
            }
            sleep(1);
            cout << "Ticket\t" << out << endl;
            if (send(sock_kdc, out, strlen((const char *)out), 0) == -1) {
                perror("send error\n");
            }
            string ticket((char *)out);
            cout << "Prev ticket\t" << ticket << endl;
            Ticket.insert(pair<string, string>(user, ticket));
        }
        else {
            cout << "User not found" << endl;
            char reply[] = "No user exist with these credentials!";
            if (send(sock_kdc, reply, strlen(reply), 0) == -1) {
                perror("send error\n");
            }
        }
        close(sock_kdc);
    }
    pthread_exit(NULL);
}

void CreateFakeUsers() {
    Credentials.insert(pair<string, string>("kvats", "kvats"));
    Credentials.insert(pair<string, string>("flash", "flash"));
    Credentials.insert(pair<string, string>("thor", "thor"));
    LoginStatus.insert(pair<string,  int>("kvats", 0));
    LoginStatus.insert(pair<string,  int>("flash", 0));
    LoginStatus.insert(pair<string,  int>("thor", 0));
    Fd_map.insert(pair<string,  int>("kvats", -1));
    Fd_map.insert(pair<string,  int>("flash", -1));
    Fd_map.insert(pair<string,  int>("thor", -1));
    cout << "Created 3 Fake users" << endl;
}

void * ConnectionHandler(void * argv) {
    struct UserFD * args;
    args = (struct UserFD *) argv;
    string username = args->username;
    int fd = args->fd;
    // int fd = *((int*) argv);
    char reply[] = "Chat connection Live!";
    sleep(1);
    if (send(fd, reply, strlen(reply), 0) == -1) {
        perror("send error\n");
        close(fd);
        pthread_exit(NULL);
    }
    cout << "FD " << fd << endl;
    while(1) {
        char message[BUFSIZE];
        memset(message, '\0', sizeof(message));
        if (recv(fd, message, sizeof(message), 0) <= 0) {
            cout << "Client Closed or Recv Error" << endl;
            close(fd);
            LoginStatus[username] = -1;
            pthread_exit(NULL);
        }
        char * tkn = strtok(message, " ");
        cout << tkn  << " len: " << strlen(tkn) << endl;
        if (strcmp(tkn, "/who") == 0) {
            // cout << tkn << endl;
            string reply = "";
            for (auto i = LoginStatus.begin(); i != LoginStatus.end(); i++) {
                if (i->second == 1) {
                    reply += i->first;
                    reply += "\n";
                }
            }
            char out[reply.size() + 1];
            memset(out, '\0', sizeof(out));
            strcpy(out, reply.c_str());
            cout << "Replying with -> " << out << endl;
            sleep(0.5);
            if (send(fd, out, sizeof(out), 0) == -1) {
                perror("send error\n");
            }
        }
        else if (strcmp(tkn, "/write_all") == 0) {
            string reply = "";
            tkn = strtok(NULL, " ");
            while (tkn != NULL) {
                cout << "tkn " << tkn << endl;
                reply += tkn;
                reply += " ";
                tkn = strtok(NULL, " ");
            }
            char out[reply.size() + 1];
            memset(out, '\0', sizeof(out));
            strcpy(out, reply.c_str());
            cout << "Replying with -> " << out << endl;
            for (auto i = LoginStatus.begin(); i != LoginStatus.end(); i++) {
                if (i->second == 1 && Fd_map[i->first] != fd) {
                    int temp_fd = Fd_map[i->first];
                    if (send(temp_fd, out, sizeof(out), 0) == -1) {
                        perror("send error\n");
                    }
                }
            }
        }
        else if (strcmp(tkn, "/create_group") == 0) {
            int number = rand() % 9000 + 1000;
            cout << "Rand: " << number << endl;
            list<string> ls;
            ls.push_back(username);
            Groups.insert(pair<int, list<string> >(number, ls));
            string num = to_string(number);
            char const *pchar = num.c_str();
            if (send(fd, pchar, sizeof(pchar), 0) == -1) {
                perror("send error\n");
            }
        }
        else if (strcmp(tkn, "/group_invite") == 0) {
            tkn = strtok(NULL, " ");
            char gid[5];
            memset(gid, '\0', sizeof(gid));
            strncpy(gid, tkn, 4);

            tkn = strtok(NULL, " ");
            while(tkn != NULL) {
                string name(tkn);
                if (LoginStatus.find(name) != LoginStatus.end()) {
                    if (LoginStatus[name] != 0) {
                        int temp_fd = Fd_map[name];
                        if (send(temp_fd, gid, sizeof(gid), 0) == -1) {
                            perror("send error\n");
                        }
                    }
                }
                tkn = strtok(NULL, " ");
            }
        }
        else if (strcmp(tkn, "/group_invite_accept") == 0) {
            tkn = strtok(NULL, " ");
            if (tkn == NULL) {
                continue;
            }
            int gid = atoi(tkn);
            if (Groups.find(gid) != Groups.end()) {
                Groups[gid].push_back(username);
            }
        }
        else if (strcmp(tkn, "/request_public_key") == 0) {
            tkn = strtok(NULL, " ");
            if (tkn == NULL) {
                continue;
            }
            string user(tkn);
            for (auto i = LoginStatus.begin(); i != LoginStatus.end(); i++) {
                if (user.compare(i->first) == 0) {
                    if (i->second != 0){
                        string reply = "";
                        reply += username;
                        reply += " ";
                        reply += "has requested your public key";

                        char chararray[reply.length() + 1]; 
                        strcpy(chararray, reply.c_str());
                        int temp_fd = Fd_map[i->first];
                        if (send(temp_fd, chararray, sizeof(chararray), 0) == -1) {
                            perror("send error\n");
                        }
                    }
                }
            }
        }
        else if (strcmp(tkn, "/send_public_key") == 0) {
            tkn = strtok(NULL, " ");
            if (tkn == NULL) {
                continue;
            }
            string user(tkn);
            tkn = strtok(NULL, " ");
            if (tkn == NULL) {
                continue;
            }
            string pubkey(tkn);
            for (auto i = LoginStatus.begin(); i != LoginStatus.end(); i++) {
                if (user.compare(i->first) == 0) {
                    if (LoginStatus[user] != 0) {
                        string reply = "";
                        reply += pubkey;

                        char chararray[reply.length() + 1]; 
                        strcpy(chararray, reply.c_str());
                        int temp_fd = Fd_map[user];
                        if (send(temp_fd, chararray, sizeof(chararray), 0) == -1) {
                            perror("send error\n");
                        }
                    }
                }
            }
        }
        else if (strcmp(tkn, "/write_group") == 0) {
            tkn = strtok(NULL, " ");
            if (tkn == NULL) {
                continue;
            }
            int gid = atoi(tkn);
            string reply = "";
            tkn = strtok(NULL, " ");
            while (tkn != NULL) {
                reply += tkn;
                reply += " ";
                tkn = strtok(NULL, " ");
            }
            char chararray[reply.length() + 1]; 
            strcpy(chararray, reply.c_str());
            list<string> group_list = Groups[gid];

            for (list<string>::const_iterator i = group_list.begin(); i != group_list.end(); i++) {
                string user(i->c_str());
                if (LoginStatus[user] != 0) {
                    int temp_fd = Fd_map[user];
                    if (send(temp_fd, chararray, sizeof(chararray), 0) == -1) {
                        perror("send error\n");
                    }
                }
            }
        }
        else if (strcmp(tkn, "/list_user_files") == 0) {
            tkn = strtok(NULL, " ");
            string user(tkn);
            string path = "/fakeslash/fakehome/";
            path += user;
            path += "/";

            string reply = "";
            tkn = strtok(NULL, " ");
            if (tkn == NULL) {
                struct dirent *name;
                DIR * dir = opendir(path.c_str());

                if (dir == NULL) {
                    cout << "Wrong Path!" << endl;
                    continue;
                }
                while ((name = readdir(dir)) != NULL) {
                    // printf("d_name %s\n", name->d_name);
                    string temp = path;
                    temp += name->d_name;
                    int ret = checkFilePermissions(temp.c_str(), username);
                    cout << "Return: " << ret << endl;
                    if (ret == 1 || ret == 2) {
                        reply += name->d_name;
                        reply += "\t";
                    }
                }
                closedir(dir);
            }
            else {
                path += tkn;
                path += "/";
                struct dirent *name;
                DIR * dir = opendir(path.c_str());

                if (dir == NULL) {
                    cout << "Wrong Path!" << endl;
                    continue;
                }
                while ((name = readdir(dir)) != NULL) {
                    // printf("d_name %s\n", name->d_name);
                    string temp = path;
                    temp += name->d_name;
                    int ret = checkFilePermissions(temp.c_str(), username);
                    cout << "Return: " << ret << endl;
                    if (ret == 1 || ret == 2) {
                        reply += name->d_name;
                        reply += "\t";
                    }
                }
                closedir(dir);
            }
            char chararray[reply.length() + 1]; 
            strcpy(chararray, reply.c_str());
            if (send(fd, chararray, sizeof(chararray), 0) == -1) {
                perror("send error\n");
            }
        }
        else {
            cout << "Why?" << endl;
        }
        // else if() {}
    }
}

int main(int argc, char const *argv[]) {
    CreateFakeUsers();
    getKeyIv();
    int thread_count = 0;
    if (pthread_create(&tid[thread_count], NULL, KDC_Server, NULL)) {
        cout << "Unable to create a thread" << endl;
    }   
    thread_count++;
    
    // Chat Server
    int fd_chat;
    int sock_chat;
    int opt = 1;
    struct sockaddr_in addr_chat;
    int addrlen = sizeof(addr_chat);

    if ((fd_chat = socket(AF_INET, SOCK_STREAM, 0)) == 0)  { 
        perror("Socket failed"); 
        exit(1); 
    } 

    if (setsockopt(fd_chat, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) { 
        perror("Setsockopt failed"); 
        exit(1); 
    } 
    addr_chat.sin_family = AF_INET; 
    addr_chat.sin_addr.s_addr = INADDR_ANY; 
    addr_chat.sin_port = htons(CHAT_PORT); 

    if (bind(fd_chat, (struct sockaddr *)&addr_chat, sizeof(addr_chat))<0) { 
        perror("Bind failed"); 
        exit(1); 
    } 
    if (listen(fd_chat, 3) < 0) { 
        perror("Listen"); 
        exit(1); 
    } 
    while(1) {
        // Accepting new connections to chat server
        if ((sock_chat = accept(fd_chat, (struct sockaddr *)&addr_chat, (socklen_t*)&addrlen))<0)  { 
            perror("accept"); 
            exit(1); 
        }
        
        char username[100];
        char ticket[100];
        memset(username, '\0', sizeof(username));
        memset(ticket, '\0', sizeof(ticket));
        if (recv(sock_chat, username, 100, 0) <= 0) {
            cout << "Client Closed or Recv Error" << endl;
        }

        if (recv(sock_chat, ticket, 100, 0) <= 0) {
            cout << "Client Closed or Recv Error" << endl;
        }
        string user(username);
        string pass(ticket);
        // cout << "User: " << user << "_" << user.size() << endl;

        cout << "Received ticket for chat:\t" << ticket << endl;
        // cout << "Testing:\t" << Ticket.find(user)->second << endl;
        if (Credentials.find( user ) != Credentials.end() && Ticket.find( user ) != Ticket.end() && Ticket.find(user)->second.compare(pass) == 0 ) {
            cout << "User authenticated from KDC" << endl;
            cout << "Starting server client chat ...." << endl;
            Ticket.erase(user);
            LoginStatus[user] = 1;
            Fd_map[user] = sock_chat;
            char reply[] = "User authenticated from KDC!";
            if (send(sock_chat, reply, strlen(reply), 0) == -1) {
                perror("send error\n");
            }
            // Create Thread for recv and send
            struct UserFD args;
            args.username = user;
            args.fd = sock_chat;
            cout << "Sock chat " << sock_chat << endl;
            if (pthread_create(&tid[thread_count], NULL, ConnectionHandler, (void *) &args)) {
            // if (pthread_create(&tid[thread_count], NULL, ConnectionHandler, (void *) &sock_chat)) {
                cout << "Unable to create a thread" << endl;
            }
            thread_count++;

        }
        else {
            cout << "Ticket not valid!" << endl;
            char reply[] = "Ticket not valid!";
            if (send(sock_chat, reply, strlen(reply), 0) == -1) {
                perror("send error\n");
                LoginStatus[user] = 0;
            }
            close(sock_chat);
        }
    }

    return 0;
}

