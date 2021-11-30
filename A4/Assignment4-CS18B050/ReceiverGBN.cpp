#include<arpa/inet.h>
#include<bits/stdc++.h>
#include<chrono>
#include<climits>
#include<cstdio>
#include<cstdlib>
#include<mutex>
#include<netinet/in.h>
#include <random>
#include<sys/socket.h>
#include<sys/types.h>
#include<thread>
#include<ctime>
#include<unistd.h>
using namespace std;
#define ll long long
#define ld long double
#define MAXLINE 5024
string receiver_ip="127.0.0.1"; 
ll debug=0, receiver_port=10000, MAX_PACKET_LENGTH=100, MAX_PACKETS=100;
ld PACKET_ERROR_RATE=0.5;
ll num_packets_acked=0, least_unacked=1;
deque<pair<ll, ll>> list_of_acks;
int sockfd;
ll seed = 1616662186;
default_random_engine gen(seed);
bernoulli_distribution dis;
void argument_handler(int argc, char **argv){
    for(int i=1; i<argc;i+=2){
        switch(argv[i][1]){
            case 'd' : 
                debug = 1;
                i-=1;
                break;
            case 'p' : 
                receiver_port = stoll(argv[i+1]);
                break;
            case 'n' : 
                MAX_PACKETS = stoll(argv[i+1]);
                break;
            case 'e' : 
                PACKET_ERROR_RATE = stold(argv[i+1]);
                dis = bernoulli_distribution(PACKET_ERROR_RATE);
                break;
            default : 
                break;
        }
    }
}
void send_message(const string &message, int client_port_no, struct sockaddr_in servaddr){
    sendto(sockfd, message.c_str(), message.size(), 0, (const struct sockaddr *) &servaddr, sizeof(servaddr));
    //cout<<"sending "<<message<<"\n";
}
int get_sock_id(){
    int sockfd;
    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
      
    // Filling server information
    servaddr.sin_family    = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(receiver_port);
    if(bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 ){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    return sockfd;
}
ll get_time_now(){
    return (ll) chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count();
}
void receiver(){
    while(num_packets_acked < MAX_PACKETS){
        char buffer[MAXLINE];
        struct sockaddr_in cliaddr;
        memset(&cliaddr, 0, sizeof(cliaddr));
        int len,n_recv;
        len = sizeof(cliaddr);
        n_recv = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, ( struct sockaddr *) &cliaddr, (socklen_t *) &len);
        buffer[n_recv] = '\0';
        string message(buffer);
        ll ack_seq_num = stoll(message.substr(0,4), nullptr, 16);
        bool random_value = dis(gen);
        if(!random_value){
            if(ack_seq_num == least_unacked){
                num_packets_acked++;
                least_unacked++;
                char buf[5];
                sprintf(buf, "%.4x", (unsigned int)ack_seq_num);
                message = "ACK"+ string(buf);
                send_message(message, ntohs(cliaddr.sin_port), cliaddr);
                if(debug){
                    ll tim = get_time_now();
                        printf("Seq %lld:, Time Received: %lld:%lld Packet dropped: false\n", ack_seq_num, tim/1000, tim%1000);
                }
            }
        }  
    }
}
int main(int argc, char **argv){
    argument_handler(argc, argv);
    sockfd = get_sock_id();
    receiver();
    close(sockfd);
}
