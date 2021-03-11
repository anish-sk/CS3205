#include <dirent.h> 
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
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
struct mail {
    char from[MAX_UID_LEN];
    char to[MAX_UID_LEN];
    char date[DATE_LEN];
    char subject[MAX_SUB_LEN];
    char contents[MAX_BODY_LEN];
};
struct user {
    char userid[MAX_UID_LEN];
    int curr_number_mails;
};

struct user* users[MAX_USERS];

struct user curr_user;
FILE *curr_file;
FILE *temp_file;
int curr_lineno;

void initialize_user_array(){
    for(int i=0; i<MAX_USERS; i++){
        users[i] = NULL;
    }
    int i=0;
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            //printf("%s\n", dir->d_name);
            if(dir->d_name[0] == '.') 
                continue;
            users[i] = (struct user *)malloc(sizeof(struct user));
            strcpy(users[i]->userid, dir->d_name);
            int count = 0;
            FILE *fptr = fopen(dir->d_name, "a+");
            char buf[MAX_BODY_LEN]={0};
            while(fgets(buf, MAX_BODY_LEN, fptr)){
                if(strstr(buf, "###")){
                    count++;
                }
            }
            users[i]->curr_number_mails = count;
            i++;
            fclose(fptr);
        }
        closedir(d);
  }
}

void reset_curr_user(){
    strcpy(curr_user.userid, "\0");
    curr_user.curr_number_mails = -1;
    curr_file = NULL;
}

void del_mail(int l1, int l2){
    temp_file = fopen("temp_file", "w+");
    int i=0;
    char buf[MAX_BODY_LEN]={0};
    //printf("%d %d\n", l1, l2);
    fseek(curr_file, 0, SEEK_SET);
    while(fgets(buf, MAX_BODY_LEN, curr_file)){
        if(i<l1 || i>=l2){
            fputs(buf, temp_file);
            //printf("%s", buf);
        }
        i++;
    }
    fflush(temp_file);
    remove(curr_user.userid); 
    //char tng_buf[20]={0};
    //sprintf(tng_buf, "cat %s", "temp_file");
    //system(tng_buf);
    //system("ls -l");
    rename("temp_file", curr_user.userid);
    curr_file = fopen(curr_user.userid, "r+");
    int t = curr_lineno;
    while(t--){
        fgets(buf, MAX_BODY_LEN, curr_file);
    }
}

int command_processor(char *command, char *result, char *arg, char *msg){
    if(strcmp(command,"LSTU") == 0){
        char list[MAX_RTNMSG_LEN]={0};
        for (int i=0; i<MAX_USERS; i++){
            if(users[i] == NULL) break;
            strcat(list, users[i]->userid);
            strcat(list, " ");
        }
        strcpy(result, list);
        return 0;
    }
    else if(strcmp(command, "ADDU") == 0){
        int i;
        for(i=0; i<MAX_USERS; i++){
            if(users[i] == NULL) break;
            if(strcmp(users[i]->userid, arg) == 0){
                strcpy(result, "Userid already present");
                return 0;
            }
        }
        if(i == MAX_USERS) return -1;
        FILE *new_f = fopen(arg, "a");
        users[i] = (struct user *)malloc(sizeof(struct user));
        strcpy(users[i]->userid, arg);
        users[i]->curr_number_mails = 0;
        fclose(new_f);        
        sprintf(result, "User %s successfully added", arg);
    }
    else if(strcmp(command, "USER") == 0){
        int present=0;
        int i;
        if(curr_user.curr_number_mails!=-1){
            strcpy(result, "Invalid Command");
            return 0;
        }
        for(i=0; i<MAX_USERS; i++){
            if(users[i] == NULL) break;
            if(strcmp(users[i]->userid, arg) == 0){
                present = 1;
                break;
            }
        }
        if(present == 0){
            sprintf(result, "User %s does not exist", arg);
            return 0;
        }
        sprintf(result, "User %s exists and has %d number of messages in his/her spool file", arg, users[i]->curr_number_mails);
        curr_user = *users[i];
        curr_file = fopen(arg, "r+");
        return 0;
    }
    else if(strcmp(command, "READM") == 0){
        if(curr_user.curr_number_mails <= 0){
            if(curr_user.curr_number_mails == -1)
                strcpy(result, "User not set");
            else 
                strcpy(result, "No More Mail");
            return 0;
        }
        char buf[MAX_BODY_LEN]={0};
        while(fgets(buf, MAX_BODY_LEN, curr_file)){
            strcat(result, buf);
            curr_lineno++;
            if(strstr(buf, "###")){
                break;
            }
        }
        char c = fgetc(curr_file);
        if(c==EOF){
            fseek(curr_file, 0, SEEK_SET);
            curr_lineno = 0;
        }
        else{
            ungetc(c, curr_file);
        }
        return 0;
    }
    else if(strcmp(command, "DELM") == 0){
        if(curr_user.curr_number_mails <= 0){
            if(curr_user.curr_number_mails == -1)
                strcpy(result, "User not set");
            else 
                strcpy(result, "No More Mail");
            return 0;
        }
        int l1 = curr_lineno;
        char buf[MAX_BODY_LEN]={0};
        while(fgets(buf, MAX_BODY_LEN, curr_file)){
            curr_lineno++;
            if(strstr(buf, "###")){
                break;
            }
        }
        int l2 = curr_lineno;
        char c = fgetc(curr_file);
        if(c==EOF){
            fseek(curr_file, 0, SEEK_SET);
            curr_lineno = 0;
        }
        else {
            ungetc(c, curr_file);
            curr_lineno = l1;
        }
        del_mail(l1, l2);
        curr_user.curr_number_mails--;
        for(int i=0; i<MAX_USERS; i++){
            if(users[i] == NULL) break;
            if(strcmp(users[i]->userid, arg) == 0){
                users[i]->curr_number_mails--;
                break;
            }
        }
        strcpy(result, "Message Deleted");
        return 0;
    }
    else if(strcmp(command, "SEND") == 0){
        int present=0;
        int i;
        for(i=0; i<MAX_USERS; i++){
            if(users[i] == NULL) break;
            if(strcmp(users[i]->userid, arg) == 0){
                present = 1;
                users[i]->curr_number_mails++;
                if(strcmp(curr_user.userid, arg) == 0){
                    curr_user.curr_number_mails++;
                }
                break;
            }
        }
        if(present == 0){
            sprintf(result, "User %s does not exist", arg);
            return 0;
        }
        FILE *rec_file = fopen(arg, "a+");
        char buf[MAX_BODY_LEN+10]={0};
        sprintf(buf, "From: %s\n", curr_user.userid);
        fputs(buf, rec_file);
        sprintf(buf, "To: %s\n", arg);
        fputs(buf, rec_file);
        time_t mytime = time(NULL);
        char *time_str = ctime(&mytime);
        sprintf(buf, "Date: %s", time_str);
        fputs(buf, rec_file);
        char sub[MAX_BODY_LEN]={0};
        sscanf(msg, "%[^\n]s", sub);
        if(strstr(sub, "###")){
            int x = strlen(sub);
            sub[x-3] = 0;
        }
        sprintf(buf, "Subject: %s\n", sub);
        fputs(buf, rec_file);
        int y = strlen(msg);
        if(y>=3){
            msg[y-4] = 0;
        }
        fprintf(rec_file, "%s\n", msg);
        fputs("###\n", rec_file);
        fclose(rec_file);
        sprintf(result, "Message sent to %s successfully", arg);
        return 0;
    }
    else if(strcmp(command, "DONEU") == 0){
        if(curr_user.curr_number_mails == -1)
            strcpy(result, "User not set");
        else 
            sprintf(result, "User %s done successfully", curr_user.userid);
        reset_curr_user();   
        return 0;
    }
    else {
        strcpy(result, "Invalid Command");
        return -1;
    }
}

int process_message(char *buf, char *result){
    char comm[MAX_CMD_LEN]={0};
    char use[MAX_UID_LEN]={0};
    sscanf(buf, "%s %s ", comm, use);
    int l1 = strlen(comm);
    int l2 = strlen(use);
    char *msg;
    msg = &buf[l1+l2+2];
    if(strcmp(comm, "QUIT") == 0){
        strcpy(result, "Session Ended.");
        return -1;
    }
    int x = command_processor(comm, result, use, msg);
    return 0;
}

int send_query(char *msg, int sockfd){
    int total = strlen(msg);
    //printf("%d\n", total);
    char buf[5];
    sprintf(buf, "%.4x", total);
    //printf("%s\n", buf);
    send(sockfd, buf, 4, 0);
    int nb_send = send(sockfd, msg, strlen(msg), 0);
    //printf("To client :\n %s \n", msg);
    bzero(msg, MAX_MSG_LEN);
    return nb_send;
}
int receive_query(char *result, int sockfd){
    bzero(result, MAX_MSG_LEN);
    int nb_recv = 0;
    while(nb_recv < 4){
        nb_recv += recv(sockfd, result+nb_recv, 4-nb_recv, 0);
    }
    //printf("%s\n", result);
    int n_bytes = strtol(result, NULL, 16);
    //printf("%d\n", n_bytes);
    nb_recv=0;
    bzero(result, MAX_MSG_LEN);
    while(nb_recv<n_bytes){
        nb_recv+=recv(sockfd, result+nb_recv, MAX_MSG_LEN-nb_recv, 0);
    }
    return n_bytes;
}
//Network stuff - get message
void get_message(char *buf, int connfd){
    // read the message from client and copy it in buffer 
    int nb_recv = receive_query(buf, connfd);
    //printf("Number of bytes received %d\n", nb_recv);
    // print buffer which contains the client contents 
    printf("From client: %s\n", buf); 
}

//Network stuff - send result back
void send_result(char *result, int connfd){
    //write(connfd, result, sizeof(result));
    int nb_send = send_query(result, connfd);
    //printf("number of bytes sent %d\n", nb_send);
}

int main(int argc, char *argv[]){
    mkdir("MAILSERVER", 0700);
    chdir("MAILSERVER");
    initialize_user_array();
    if(argc >= 2)
        PORT = atoi(argv[1]);
    
    int sockfd, connfd, len; 
    struct sockaddr_in servaddr, cli; 
  
    // socket create and verification 
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
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(PORT); 
     
    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, 
                                                &opt, sizeof(opt))) { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    // Binding newly created socket to given IP and verification 
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
        
        //while(bind(sockfd, (SA*)&servaddr, sizeof(servaddr)));
        printf("socket bind failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully binded..\n"); 

    // Now server is ready to listen and verification 
    if ((listen(sockfd, 5)) != 0) { 
        printf("Listen failed...\n"); 
        exit(0); 
    } 
    else
        printf("Server listening..\n"); 

        
    while(1){
        reset_curr_user();
        
        char buf[MAX_MSG_LEN]={0};
        char result[MAX_RTNMSG_LEN]={0};
        len = sizeof(cli); 
      
        // Accept the data packet from client and verification 
        connfd = accept(sockfd, (SA*)&cli, &len); 
        if (connfd < 0) { 
            printf("server acccept failed...\n"); 
            exit(0); 
        } 
        else
            printf("server acccept the client...\n"); 
      
        while(1){
            get_message(buf, connfd);
            int x = process_message(buf, result);
            send_result(result, connfd);
            if(x == -1){
                //Close connection and maybe set result
                close(connfd);
                break;
            }
        }
    }
    return 0;
}
