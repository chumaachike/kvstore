#include <iostream>
#include <cctype>
#include <sstream>
#include <algorithm>

#include "../internal/store/store.hpp"

using namespace std;

static inline string to_upper(string s) {
  transform(s.begin(), s.end(), s.begin(),[](unsigned char c) { return toupper(c); });
  return s;
}

int main (){
    KVStore kv;
    string line;
    while (true){
        cout << "> ";
        if (!getline(cin, line)) break;

        istringstream iss(line);
        string cmd;

        if (!(iss >> cmd)) continue;

        cmd = to_upper(cmd);
        if (cmd == "SET"){
            continue;
        }else if (cmd == "GET"){
            continue;
        }else if (cmd == "DEL"){
            continue;
        }else if (cmd == "QUIT"){
            break;
        }else{
            cerr << " ERR unknown command "<< cmd << ", with args beginning with:\n";
            continue;
        }

    }
    cout << "Exiting... \n";
    return 0;
}

