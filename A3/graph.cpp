#include <bits/stdc++.h>
using namespace std;

int main(int argc, char *argv[]) {
  srand(time(0));
  int N = stoi(argv[1]), M = stoi(argv[2]);
  set<pair<int, int>> s;
  default_random_engine generator(
      std ::chrono ::system_clock ::now().time_since_epoch().count());
  uniform_int_distribution<int> wts(1, 60);
  fstream fout(argv[3], ios ::out);
  fstream f1("temp.txt", ios ::out);
  fout << N << " " << M << endl;
  f1 << N << " " << M << endl;
  while (s.size() < M) {
    int T = s.size();
    int u = rand() % (N - 1);
    int v = rand() % (N - u - 1) + u + 1;
    s.insert({u, v});
    if (s.size() == T) {
      continue;
    }
    int mW = wts(generator), MW = wts(generator);
    if (mW > MW) {
      swap(mW, MW);
    }
    fout << u << " " << v << " " << mW << " " << MW << endl;
    f1 << u << " " << v << " "
       << "(" << mW << "," << MW << ")" << endl;
  }
  return 0;
}
