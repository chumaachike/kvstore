#include <iostream>
#include <cctype>
#include <sstream>
#include <algorithm>

#include "../internal/store/store.hpp"
#include "../internal/cli/parser.hpp"

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
            string key;
            string value;

            if (!cli::parse_set(iss, key, value)){
                continue;
            }
            kv.set(key, value);
            cout << "OK\n";
        }else if (cmd == "GET"){
            string key;
            
            if (!cli::parse_get(iss, key)){
                continue;
            }
            auto result = kv.get(key);
            string value = result? *result : "nil";
            cout << value << '\n';
            
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

