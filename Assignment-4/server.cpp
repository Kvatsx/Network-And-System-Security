/*
 * @Author: Kaustav Vats 
 * @Roll-Number: 2016048 
 */

#include "helper.h" 

using namespace std;

pthread_t tid[CONNECTIONS];
map<string, string> Credentials;
map<string, string> Ticket;

void * KDC_Server(void * argv) {
    int fd_kdc;
    int sock_kdc;
    int opt = 1;
    struct sockaddr_in addr_kdc;
    int addrlen = sizeof(addr_kdc);

    if ((fd_kdc = socket(AF_INET, SOCK_STREAM, 0)) == 0)  { 
        perror("Socket failed"); 
        exit(1); 
    } 

    if (setsockopt(fd_kdc, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) { 
        perror("Setsockopt failed"); 
        exit(1); 
    } 
    addr_kdc.sin_family = AF_INET; 
    addr_kdc.sin_addr.s_addr = INADDR_ANY; 
    addr_kdc.sin_port = htons(KDC_PORT); 

    if (bind(fd_kdc, (struct sockaddr *)&addr_kdc, sizeof(addr_kdc))<0) { 
        perror("Bind failed"); 
        exit(1); 
    } 
    if (listen(fd_kdc, 3) < 0) { 
        perror("Listen"); 
        exit(1); 
    } 
    cout << "--------------- KDC Live -----------------" << endl;

    while (1) {
        // KDC Connections
        if ((sock_kdc = accept(fd_kdc, (struct sockaddr *)&addr_kdc, (socklen_t*)&addrlen))<0)  { 
            perror("accept"); 
            exit(1); 
        }
        char username[100];
        char password[100];
        if (recv(sock_kdc, username, 100, 0) <= 0) {
            cout << "Client Closed or Recv Error" << endl;
        }
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
            if( PKCS5_PBKDF2_HMAC_SHA1(username, strlen(username), NULL, 0, 100, 32, out) == 0 ) {
                perror("PBKDF");
            }
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
    Credentials.insert(pair<string, string>("kaustav", "kaustav"));
    Credentials.insert(pair<string, string>("admin", "admin"));
    Credentials.insert(pair<string, string>("naruto", "naruto"));
    cout << "Created 3 Fake users" << endl;
}

void * ConnectionHandler(void * argv) {
    int fd = *((int*)(&argv));
    // char reply[] = "Chat connection Live!";
    // if (send(fd, reply, strlen(reply), 0) == -1) {
    //     perror("send error\n");
    //     exit(1);
    // }
    cout << "Hello\n";
    while(1) {

    }
}

int main(int argc, char const *argv[]) {
    CreateFakeUsers();
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
            char reply[] = "User authenticated from KDC!";
            if (send(sock_chat, reply, strlen(reply), 0) == -1) {
                perror("send error\n");
            }
            // Create Thread for recv and send
            if (pthread_create(&tid[thread_count], NULL, ConnectionHandler, (void *) &sock_chat)) {
                cout << "Unable to create a thread" << endl;
            }
            thread_count++;

        }
        else {
            cout << "Ticket not valid!" << endl;
            char reply[] = "Ticket not valid!";
            if (send(sock_chat, reply, strlen(reply), 0) == -1) {
                perror("send error\n");
            }
            close(sock_chat);
        }
    }

    return 0;
}

