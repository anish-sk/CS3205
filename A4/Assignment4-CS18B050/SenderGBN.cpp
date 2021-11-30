#include<arpa/inet.h>
#include<bits/stdc++.h>
#include<chrono>
#include<climits>
#include<cstdio>
#include<cstdlib>
#include<mutex>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<thread>
#include<ctime>
#include<unistd.h>
using namespace std;
#define ll long long
#define MAXLINE 5024
string receiver_ip="127.0.0.1"; 
ll debug=0, receiver_port=10000, PACKET_LENGTH=100, PACKET_GEN_RATE=10, MAX_PACKETS=100, WINDOW_SIZE=20, BUFFER_SIZE = 30;
ll RTT_avg=0, num_packets_acked=0, sender_port=10001, num_packets_unacked=0, curr_seq_num = 0, num_trans=0, least_unacked=1;
bool not_exceeded_five_retrans = true;
int sockfd;
unordered_map<ll,ll> num_retrans;
struct packet_info{
    ll seq_num;
    ll start_time;
    ll len;
    ll timeout;
    string message;
};
queue<packet_info> buffer, sent_but_unacked;
void argument_handler(int argc, char **argv){
    for(int i=1; i<argc;i+=2){
        switch(argv[i][1]){
            case 'd' : 
                debug = 1;
                i-=1;
                break;
            case 's' : 
                receiver_ip = string(argv[i+1]);
                break;
            case 'p' : 
                receiver_port = stoll(argv[i+1]);
                break;
            case 'l' : 
                PACKET_LENGTH = stoll(argv[i+1]);
                break;
            case 'r' : 
                PACKET_GEN_RATE = stoll(argv[i+1]);
                break;
            case 'n' : 
                MAX_PACKETS = stoll(argv[i+1]);
                break;
            case 'w' : 
                WINDOW_SIZE = stoll(argv[i+1]);
                break;
            case 'b' : 
                BUFFER_SIZE = stoll(argv[i+1]);
                break;
            default : 
                break;
        }
    }
}
void send_message(const string &message, int client_port_no){
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(client_port_no);
    servaddr.sin_addr.s_addr = inet_addr(receiver_ip.c_str());
    sendto(sockfd, message.c_str(), message.size(), 0, (const struct sockaddr *) &servaddr, sizeof(servaddr));
    //cout<<"sending "<<stoll(message.substr(0,4), nullptr, 16)<<"\n";
    num_trans++;
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
    servaddr.sin_port = htons(sender_port);
    if(bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 ){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    return sockfd;
}
ll get_time_now(){
    return (ll) chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count();
}
void retransmit_window(){
    queue<packet_info> temp(sent_but_unacked);
    sent_but_unacked = queue<packet_info>();
    while(temp.size()){
        packet_info pack = temp.front();
        temp.pop();
        send_message(pack.message, receiver_port);
        if(num_packets_acked == 0){
            RTT_avg = get_time_now() - pack.start_time;
        }
        else{
            RTT_avg = (num_packets_acked*RTT_avg + get_time_now() - pack.start_time)/(num_packets_acked);
        }
        pack.start_time = get_time_now();
        pack.timeout = (pack.seq_num < 10)?(ll)3e5:2*RTT_avg; 
        sent_but_unacked.push(pack);
        num_retrans[pack.seq_num]++;
        if(num_retrans[pack.seq_num] > 5){
            not_exceeded_five_retrans = false;
        }
    }
}
void main_sender(){
    while(num_packets_acked < MAX_PACKETS and not_exceeded_five_retrans){
        if(buffer.size() < BUFFER_SIZE - 1){
            packet_info p;
            curr_seq_num = (curr_seq_num+1);
            p.start_time = get_time_now();
            p.len = PACKET_LENGTH;
            p.seq_num = curr_seq_num;
            char buf[5];
            sprintf(buf, "%.4x", (unsigned int)p.seq_num);
            //cout<<string(buf)<<"\n";
            p.message = string(buf) + string(p.len-4, 'a');
            buffer.push(p);
        } 
        if(buffer.size()){
            if(num_packets_unacked < WINDOW_SIZE){
                num_packets_unacked++;
                packet_info curr_pac = buffer.front();
                buffer.pop();
                curr_pac.start_time = get_time_now();
                curr_pac.timeout = (curr_pac.seq_num < 10)?(ll)1e5:2*RTT_avg; 
                sent_but_unacked.push(curr_pac);
                send_message(curr_pac.message, receiver_port);
            }                 
        }
        while(sent_but_unacked.size() and sent_but_unacked.front().seq_num < least_unacked){
            sent_but_unacked.pop();
        }
        queue<packet_info> temp(sent_but_unacked);
        while(temp.size()){
           packet_info pack = temp.front();
           temp.pop();
           if(pack.start_time + pack.timeout < get_time_now()){
               retransmit_window();
               break;
           }
        }  
        usleep((ll)(1e6)/PACKET_GEN_RATE); 
    }
}
void receiver(){
    while(num_packets_acked < MAX_PACKETS and not_exceeded_five_retrans){
        char buffer[MAXLINE];
        struct sockaddr_in cliaddr;
        memset(&cliaddr, 0, sizeof(cliaddr));
        int len,n_recv;
        len = sizeof(cliaddr);
        n_recv = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, ( struct sockaddr *) &cliaddr, (socklen_t *) &len);
        buffer[n_recv] = '\0';
        string message(buffer);
        ll ack_seq_num = stoll(message.substr(3,4), nullptr, 16);
        //cout<<ack_seq_num<<" "<<least_unacked<<"\n";
        if(ack_seq_num != least_unacked)
            continue;
        least_unacked++;
        queue<packet_info> temp(sent_but_unacked);
        while(temp.size()){
            packet_info pack = temp.front();
            temp.pop();
            if(pack.seq_num == ack_seq_num){
                ll curr_time = get_time_now();
                RTT_avg = (num_packets_acked*RTT_avg + curr_time - pack.start_time)/(num_packets_acked + 1);
                if(debug){
                    printf("Seq %lld: Time Generated: %lld:%lld RTT: %lld:%lld Number of Attempts: %lld\n", pack.seq_num, pack.start_time/1000, pack.start_time%1000, (curr_time-pack.start_time)/1000,(curr_time-pack.start_time)%1000, num_retrans[pack.seq_num]+1LL);
                }
                num_retrans.erase(pack.seq_num); 
            }
        }
        num_packets_acked++;
        num_packets_unacked--;
    }
}
int main(int argc, char **argv){
    argument_handler(argc, argv);
    sockfd = get_sock_id();
    thread t_main_sender(main_sender);
    thread t_receiver(receiver);
    t_main_sender.join();
    t_receiver.join();
    cout<<"Packet Generation Rate: "<<PACKET_GEN_RATE<<"\n";
    cout<<"Packet Length (Maximum): "<<PACKET_LENGTH<<"\n";
    cout<<"Retransmission Ratio: "<<(long double)num_trans/(long double)num_packets_acked<<"\n";
    cout<<"Average RTT value: "<<RTT_avg/1000<<":"<<RTT_avg%1000<<"\n";
    close(sockfd);
}
