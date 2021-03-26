#include <bits/stdc++.h>
#include <cfloat>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <sys/stat.h>
#define ll long long 
#define ld long double
using namespace std;
ld Ki=1, Km=1, Kn=1, Kf=0.4, Ps=0.99;
string outfile="output.txt";
ll T=1000;
void argument_handler(int argc, char **argv){
    //parse the command line arguments and set the corresponding parameters using switch-case
    for(int i=1; i<argc-1;i+=2){
        switch(argv[i][1]){
            case 'i' : 
                Ki = stold(argv[i+1]);
                if(Ki>4 or Ki<1){
                    cout<<"Ki should be between 1 and 4"<<endl;
                    exit(0);
                }
                break;
            case 'm' : 
                Km = stold(argv[i+1]);
                if(Km>2 or Km<0.5){
                    cout<<"Km should be between 0.5 and 2"<<endl;
                    exit(0);
                }
                break;
            case 'n' : 
                Kn = stold(argv[i+1]);
                if(Kn>2 or Kn<0.5){
                    cout<<"Kn should be between 0.5 and 2"<<endl;
                    exit(0);
                }
                break;
            case 'f' : 
                Kf = stold(argv[i+1]);
                if(Kf>(0.5+100*LDBL_EPSILON) or Kf<(0.1-100*LDBL_EPSILON)){
                    cout<<"Kf should be between 0.1 and 0.5"<<endl;
                    exit(0);
                }
                break;
            case 's' : 
                Ps = stold(argv[i+1]);
                Ps = 1-Ps;
                if(Ps>1 or Ps<0){
                    cout<<"Ps should be between 0 and 1"<<endl;
                    exit(0);
                }
                break;
            case 'T' : 
                T = stoll(argv[i+1]);
                break;
            case 'o' : 
                outfile = string(argv[i+1]);
                break;
        }
    }
}
void plot(string outfile, string plotfile){
    //execute plot.py wo=ith the appropriare command line arguments
    string comm = "python plot.py " + outfile + " " + plotfile;
    system(comm.c_str());
}
int main(int argc, char **argv){
    //process arguments, run the simulation and generate the plots
    argument_handler(argc,argv);
    mkdir("outputs", 0777);
    mkdir("plots", 0777);
    string plotfile = "plots/"+outfile+".png";
    outfile = "outputs/" + outfile;
    ofstream fout(outfile, ios::trunc);
    ll n_updates = 1;
    ld RWS = 1024;
    ld MSS = 1;
    ld CW = Ki * MSS;
    ld ssthresh = LDBL_MAX;
    ll N;
    ll seed = 1616662186;
    default_random_engine gen(seed);
    bernoulli_distribution dis(Ps);
    fout<<fixed<<setprecision(10);
    fout<<CW<<"\n";
    while(n_updates < T){
        N = ceil(CW/MSS);
        for(int i=0;i<N and n_updates<T;i++){
            bool random_value = dis(gen);
            if(!random_value){
               ssthresh = CW/2.0;
               CW = max((ld)1.0, Kf * CW);
               fout<<CW<<"\n";
               n_updates++;
               break;
            }
            else{
                if(CW>=ssthresh){
                    CW = min(CW+Kn*MSS*MSS/CW, RWS);
                }
                else{
                    CW = min(CW+Km*MSS, RWS);
                }
                fout<<CW<<"\n";
                n_updates++;
            }
        }
    }
    fout.close();
    plot(outfile, plotfile);
}
