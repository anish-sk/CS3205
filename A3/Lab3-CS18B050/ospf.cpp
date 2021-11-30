#include<arpa/inet.h>
#include<bits/stdc++.h>
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
#define POFF 10000
#define MAXLINE 1024
ll id=0,hi=1,lsai=5,spfi=20,n_routers=0, n_links=0, port_no=POFF, n_neighbours=0, start_time=0, n_hello_send=0;
ll hreply_counter=0;
vector<ll> lsa_seq_num;
int sockfd;
string infile="input.txt", outfile="output.txt";
struct gt{
    ll mi;
    ll mx;
    ll wt;
    gt(ll a, ll b, ll c){
        mi=a;
        mx=b;
        wt=c;
    }
};
vector<vector<gt>> graph;
ll hello_ack_counter = 0;
mutex hello_ack_counter_mutex, graph_mutex, lsa_seq_num_mutex;
void argument_handler(int argc, char **argv){
    for(int i=1; i<argc-1;i+=2){
        switch(argv[i][1]){
            case 'i' : 
                id = stoll(argv[i+1]);
                port_no = POFF+id;
                break;
            case 'f' : 
                infile = string(argv[i+1]);
                break;
            case 'o' : 
                outfile = string(argv[i+1]);
                break;
            case 'h' : 
                hi = stoll(argv[i+1]);
                break;
            case 'a' : 
                lsai = stoll(argv[i+1]);
                break;
            case 's' : 
                spfi = stoll(argv[i+1]);
                break;
            default : 
                break;
        }
    }
}
void initialize_graph(){
    ifstream fin(infile);
    fin>>n_routers>>n_links;
    lsa_seq_num = vector<ll>(n_routers, -1);
    graph = vector<vector<gt>>(n_routers, vector<gt>(n_routers,gt(-1,-1,-1)));
    for(int i=0; i<n_links; i++){
        int a,b;
        fin>>a>>b;
        fin>>graph[a][b].mi;
        fin>>graph[a][b].mx;
        graph[a][b].wt = graph[a][b].mx;
        graph[b][a].wt = graph[a][b].wt;
        graph[b][a].mx = graph[a][b].mx;
        graph[b][a].mi = graph[a][b].mi;
    }
    for(int i=0; i<n_routers; i++){
        if(i == id) continue;
        if(graph[id][i].mx == -1) continue;
        n_neighbours++;
        if(i > id) n_hello_send++;
    }
    //hello_ack_counter = n_neighbours;
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
    servaddr.sin_port = htons(port_no);
    if(bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 ){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    return sockfd;
}
void send_message(const string &message, int client_port_no){
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(client_port_no);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    sendto(sockfd, message.c_str(), message.size(), 0, (const struct sockaddr *) &servaddr, sizeof(servaddr));
    //cout<<"sending "<<message<<"\n";
}
void hello(){
    sleep(hi);
    while(1){
        unique_lock<mutex> guard(hello_ack_counter_mutex);
        if(hello_ack_counter == 0){
            for(int i=port_no-POFF+1; i<n_routers; i++){
                if(graph[id][i].mx != -1){
                    char buf[5];
                    sprintf(buf, "%.4x", (unsigned int)id);
                    string message = "HELLO" + string(buf);
                    send_message(message, POFF+i);
                }
            }
        }
        guard.unlock();
        sleep(hi);
    }
}
void lsa(){
    sleep(lsai);
    while(1){
        unique_lock<mutex> guard(graph_mutex);
        string message = "LSA";
        char buf[5];
        sprintf(buf, "%.4x",(unsigned int) id);
        message += string(buf);
        unique_lock<mutex> lsa_guard(lsa_seq_num_mutex);
        lsa_seq_num[id]++;
        lsa_guard.unlock();
        sprintf(buf, "%.4x",(unsigned int) lsa_seq_num[id]);
        message+=string(buf);
        ll no_edges=0;
        string edge_list="";
        for(int i=0; i<n_routers; i++){
            if(i == id) continue;
            if(graph[id][i].mx == -1) continue;
            no_edges++;
            sprintf(buf, "%.4x",(unsigned int) i);
            edge_list+=string(buf);
            sprintf(buf, "%.4x",(unsigned int) graph[id][i].wt);
            edge_list+=string(buf);
        }
        sprintf(buf, "%.4x",(unsigned int) no_edges);
        message+=string(buf);
        message+=edge_list;
        for(int i=0; i<n_routers; i++){
            if(i == id) continue;
            if(graph[id][i].mx != -1){
                send_message(message, POFF+i);
            }
        }
        guard.unlock();
        sleep(lsai);
    }
}
void print_header_outfile(ofstream &fout){
    fout<<"Routing Table for Node No. "<<id<<" at Time "<<time(0)-start_time<<"\n";
    fout<<setw(35)<<"Destination"<<setw(35)<<"Path"<<setw(35)<<"Cost"<<"\n";
}
void ospf(){
    sleep(spfi);
    while(1){
        unique_lock<mutex> guard(graph_mutex);
        vector<ll> distance(n_routers, LLONG_MAX);
        vector<ll> parent(n_routers,-1);
        vector<bool> path_exists(n_routers, false);
        distance[id]=0;
        for(int i=0; i<n_routers; i++){
            int v = -1;
            for(int j=0; j<n_routers; j++){
                if(!path_exists[j] && (v == -1 || distance[j] < distance[v]))
                    v=j;
            }
            if(distance[v] == LLONG_MAX)
                break;
            path_exists[v]=true;
            for(int j=0; j<n_routers; j++){
                if(graph[v][j].mx != -1){
                    if(distance[v] + graph[v][j].wt < distance[j]){
                        distance[j] = distance[v] + graph[v][j].wt;
                        parent[j] = v;
                    }
                }
            }
        }
        ofstream fout(outfile, ofstream::out | ofstream::app);
        print_header_outfile(fout);
        for(int i=0; i<n_routers; i++){
            if(i == id) continue;
            if(distance[i] == LLONG_MAX) continue;
            string path_string= "";
            for(int v=i; v!=id; v=parent[v]){
                path_string = to_string(v)+"-"+path_string;
            }
            path_string = to_string(id)+"-"+path_string;
            path_string.pop_back();
            fout<<setw(35)<<to_string(i)<<setw(35)<<path_string<<setw(35)<<distance[i]<<"\n";
        }
        fout.close();
        guard.unlock();
        //cout<<id<<" "<<hreply_counter<<"\n";
        sleep(spfi);
    }
}
void receiver(){
    char buffer[MAXLINE];
    struct sockaddr_in cliaddr;
    memset(&cliaddr, 0, sizeof(cliaddr));
    int len,n_recv;
    len = sizeof(cliaddr);
    n_recv = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, ( struct sockaddr *) &cliaddr, (socklen_t *) &len);
    buffer[n_recv] = '\0';
    string message(buffer);
    //cout<<"received "<<message<<"\n";
    if(message.substr(0,3) == "LSA"){
        ll sender = (ll)ntohs(cliaddr.sin_port) - POFF;
        ll source_id = stoll(message.substr(3,4), nullptr, 16); 
        ll curr_seq = stoll(message.substr(7,4), nullptr, 16);
        ll last_seq;
        unique_lock<mutex> lsa_guard(lsa_seq_num_mutex);
        last_seq = lsa_seq_num[source_id];
        lsa_seq_num[source_id] = max(last_seq, curr_seq);
        lsa_guard.unlock();
        if(curr_seq > last_seq){
            unique_lock<mutex> guard(graph_mutex);
            ll no_edges = stoll(message.substr(11, 4), nullptr, 16);
            for(int i=15; i<message.size()-8; i+=8){
                ll neigh = stoll(message.substr(i,4), nullptr, 16);
                //cout<<message.substr(i+4,4)<<endl;
                ll weight = stoll(message.substr(i+4,4), nullptr, 16);
                graph[source_id][neigh].wt = weight;
                graph[neigh][source_id].wt = weight;
            }
            guard.unlock();
            for(int i=0; i<n_routers; i++){
                if(i == id) continue;
                //if(i == source_id) continue;
                if(i == sender) continue;
                if(graph[id][i].mx != -1){
                    send_message(message, POFF+i);
                }
            }
        }
    }
    else if(message.substr(0,10) == "HELLOREPLY"){
        //cout<<"received "<<message<<"\n";
        unique_lock<mutex> guard(hello_ack_counter_mutex);
        if(n_neighbours)
        hello_ack_counter = (hello_ack_counter + n_hello_send - 1)%n_hello_send;
        guard.unlock();
        //cout<<message.substr(14,4)<<flush;
        ll source_id = stoll(message.substr(10,4), nullptr, 16);
        ll new_value_link = stoll(message.substr(18,4), nullptr, 16);
        //cout<<new_value_link<<endl;
        unique_lock<mutex> graph_guard(graph_mutex);
        graph[id][source_id].wt = new_value_link;
        graph[source_id][id].wt = new_value_link;
        graph_guard.unlock();
    }
    else if(message.substr(0,5) == "HELLO"){
        //cout<<message.substr(5,4)<<endl;
        hreply_counter++; 
        ll source_id = stoll(message.substr(5,4), nullptr, 16); 
        string reply = "HELLOREPLY";
        char buf[5];
        sprintf(buf, "%.4x",(unsigned int) id);
        reply+=string(buf);
        sprintf(buf, "%.4x",(unsigned int) source_id);
        reply+=string(buf);
        gt cur = graph[id][source_id];
        //random_device rd;
        //mt19937 gen(rd());
        //uniform_int_distribution<> distrib(cur.mi, cur.mx);
        ll new_value_link = (cur.mx == cur.mi)?(cur.mx):(cur.mi+(ll)(rand())%(cur.mx - cur.mi+1));
        //ll new_value_link = distrib(gen);
        //cout<<id<<" "<<source_id<<" "<<cur.mi<<" "<<cur.mx<<" "<<new_value_link<<"\n";
        sprintf(buf, "%.4x",(unsigned int) new_value_link);
        reply+=string(buf);
        unique_lock<mutex> guard(graph_mutex);
        graph[id][source_id].wt = new_value_link;
        graph[source_id][id].wt = new_value_link;
        guard.unlock();
        send_message(reply, POFF+source_id);
    }
    else return;
}
void receive_wrapper(){
    while(1){
        receiver();
    }
}
int main(int argc, char **argv){
    start_time = (ll)time(0);
    argument_handler(argc, argv);
    outfile+="-";
    outfile+=to_string(id);
    outfile+=".txt";
    initialize_graph();
    sockfd = get_sock_id();
    int seed = 0;
    srand(seed);
    thread t_hello(hello);
    thread t_lsa(lsa);
    thread t_ospf(ospf);
    thread t_receiver(receive_wrapper);
    t_hello.join();
    t_lsa.join();
    t_ospf.join();
    t_receiver.join();
}
