#include <cctype>
#include <iostream>
#include <sstream>
#include <string>

#include "store.hpp"
#include "parser.hpp"


int main() {
    KVStore kv_store;

    std::string command_line;

    while (true) {
        std::cout << "> ";

        // Stop the CLI if input is closed, for example with Ctrl+D.
        if (!std::getline(std::cin, command_line)) {
            break;
        }

        std::istringstream input_stream(command_line);

        std::string command;
        if (!(input_stream >> command)) {
            continue; // Ignore empty lines.
        }

        command = cli::to_upper(command);

        if (command == "SET") {
            std::string key;
            std::string value;

            if (!cli::parse_set(input_stream, key, value)) {
                continue;
            }

            kv_store.set(key, value);
            std::cout << "OK\n";

        } else if (command == "GET") {
            std::string key;

            if (!cli::parse_get(input_stream, key)) {
                continue;
            }

            auto result = kv_store.get(key);

            if (result) {
                std::cout << *result << '\n';
            } else {
                std::cout << "nil\n";
            }

        } else if (command == "DEL") {
            std::vector<std::string>keys;

            if(!cli::parse_del(input_stream, keys)){
                continue;
            }

            std::size_t removed_count = kv_store.remove(keys);
            std::cout << removed_count << '\n';

        } else if (command == "INCRBY"){
            std::string key;
            int amount =0;
            if (!cli::parse_incrby(input_stream, key, amount)) continue;

            auto result = kv_store.increase_by(key,  amount);
            
            if (result){
                std::cout << *result << '\n';
            }else{
                std::cout << "Error: value is not an integer or out of range\n";
            }

        }else if (command == "INCR"){
            std::string key;

            if (!cli::parse_incr(input_stream, key)){
                continue;
            }

            auto result = kv_store.increase_by(key, 1);

            if (result){
                std::cout << *result << '\n';
            }else {
                std::cerr << "Error: value is not an integer or out of range \n";
            }

        }else if (command == "QUIT" || command == "EXIT") {
            break;

        } else {
            std::cerr << "Error: unknown command '" << command << "'\n";
        }
    }

    std::cout << "Exiting...\n";
    return 0;
}