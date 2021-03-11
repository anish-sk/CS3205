#include <arpa/inet.h>
#include <netdb.h> 
#include <netinet/in.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <unistd.h>
#define MAX 80 
int PORT = 8080; 
#define SA struct sockaddr 
#define MAX_CMD_LEN 20
#define MAX_UID_LEN 20
#define DATE_LEN 29
#define MAX_SUB_LEN 90
#define MAX_BODY_LEN 1000
#define MAX_USERS 200
#define MAX_RTNMSG_LEN 5000
#define MAX_MSG_LEN 5000
int sockfd = 0;
int send_query(char *msg){
    int total = strlen(msg);
    //printf("%d\n", total);
    char buf[5];
    sprintf(buf, "%.4x", total);
    //printf("%s\n", buf);
    send(sockfd, buf, 4, 0);
    int nb_send = send(sockfd, msg, strlen(msg), 0);
    bzero(msg, MAX_MSG_LEN);
    return nb_send;
}
int receive_query(char *result){
    bzero(result, MAX_MSG_LEN);
    int nb_recv = 0;
    while(nb_recv < 4){
        nb_recv += recv(sockfd, result+nb_recv, 4-nb_recv, 0);
    }
    int n_bytes = strtol(result, NULL, 16);
    nb_recv=0;
    bzero(result, MAX_MSG_LEN);
    while(nb_recv<n_bytes){
        nb_recv+=recv(sockfd, result+nb_recv, MAX_MSG_LEN-nb_recv, 0);
    }
    return n_bytes;
}
int send_message_and_recv(char *msg, int *quit){
    int rval = 0;
    int nb_send = send_query(msg);
    //printf("Number of bytes sent %d\n", nb_send);
    int nb_recv = receive_query(msg);
    //printf("Number of bytes received %d\n", nb_recv);
    printf("From Server : \n%s\n", msg); 
    if(strstr(msg, "not exist"))
        rval=2;
    bzero(msg, MAX_MSG_LEN); 
    if(*quit == 1){
        return -1;
    }
    return rval;
}

int get_user_input(char *buf, int *quit){
    char inp[MAX_BODY_LEN] = {0};
    char comm[MAX_CMD_LEN] = {0};
    char use[MAX_UID_LEN] = {0};
    char use2[MAX_UID_LEN] = {0};
    printf("Main-Prompt> ");
    fgets(inp, MAX_BODY_LEN, stdin);
    //printf("%s\n", inp);
    sscanf(inp, "%s %s\n", comm, use);
    if(strcmp(comm, "Listusers") == 0){
        strcpy(buf, "LSTU");
    }
    else if(strcmp(comm, "Adduser") == 0){
        sprintf(buf, "ADDU %s", use);
    }
    else if(strcmp(comm, "Quit") == 0){
        strcpy(buf, "QUIT");
        *quit=1; 
    }
    else if(strcmp(comm, "SetUser") == 0){
        sprintf(buf, "USER %s", use);
        int x = send_message_and_recv(buf, quit);
        if (x == 2){
            return 2;
        }
        while(1){
            printf("Sub-Prompt-%s> ", use);
            fgets(inp, MAX_BODY_LEN, stdin);
            //printf("%s\n", inp);
            sscanf(inp, "%s %s\n", comm, use2);
            if(strcmp(comm, "Read") == 0){
                strcpy(buf,"READM");
                send_message_and_recv(buf, quit);
            }            
            else if(strcmp(comm, "Delete") == 0){
                strcpy(buf,"DELM");
                send_message_and_recv(buf, quit);
            }            
            else if(strcmp(comm, "Send") == 0){
                sprintf(buf, "SEND %s ", use2);
                printf("Type Message: ");
                while(1){
                    fgets(inp, MAX_BODY_LEN, stdin);
                    //printf("%s\n", inp);
                    strcat(buf, inp);
                    if(strstr(inp, "###")){
                        break;
                    }
                }
                send_message_and_recv(buf, quit);
            }
            else if(strcmp(comm, "Done") == 0){
                strcpy(buf, "DONEU");
                break;
            }
            else {
                printf("Invalid Command in Sub-Prompt\n");
            }
        }
    }    
    return 0;
}

int main(int argc, char *argv[]){
    char *ip = "127.0.0.1";
    if(argc >=2 )
        ip = argv[1];
    if(argc >= 3 )
        PORT = atoi(argv[2]);
    char buf[MAX_MSG_LEN]={0};
    int quit = 0;
    int *quit_p = &quit;
    struct sockaddr_in servaddr, cli;

    // socket create and varification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ip);
    servaddr.sin_port = htons(PORT);

    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");

    while(1){
        int y = get_user_input(buf, quit_p);
        if(y==2) continue;
        int x = send_message_and_recv(buf, quit_p);
        if(x == -1){
            break;
        }
    }
    return 0;
}
