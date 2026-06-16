#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <optional>

#include "store.hpp"
#include "parser.hpp"
#include "server.hpp"

namespace  {

bool handle_parse_error(cli::ParseError err) {
    if (err != cli::ParseError::None) {
        std::cerr << cli::error_message(err) << '\n';
        return true;
    }
    return false;
}

void print_integer_result(std::optional<int> result) {
    if (result) {
        std::cout << *result << '\n';
    } else {
        std::cerr << "Error: value is not an integer or out of range\n";
    }
}

void handle_set(KVStore& store, std::istringstream& input_stream){
    std::string key;
    std::string value;

    auto err = cli::parse_set(input_stream,  key, value); 
    if (handle_parse_error(err)){
        return;
    }

    store.set(key, value);
    std::cout << "OK\n";
}

void handle_get (KVStore& store, std::istringstream& input_stream){
    std::string key;

    auto err = cli::parse_get(input_stream, key);
    if (handle_parse_error(err)){
        return;
    }


    auto result = store.get(key);
    if (result) {
        std::cout << *result << '\n';
    } else {
        std::cout << "nil\n";
    }
}

void handle_delete (KVStore& store, std::istringstream& input_stream){
    std::vector<std::string> keys;

    auto err = cli::parse_del(input_stream, keys);
    if (handle_parse_error(err)){
        return;
    }


    std::size_t removed_count = store.erase(keys);
    std::cout << removed_count << '\n';
}

std::string handle_incrby(KVStore& store, std::istringstream& input_stream){
    std::string key;
    int amount = 0;

    auto err = cli::parse_incrby(input_stream, key, amount);
    if (err!= cli::ParseError::None){
        return cli::error_message(err);
    }

    auto result = store.increase_by(key, amount);
    if  (!result){
        return "Error: value is not an integer or out of range";
    }
    return std::to_string(*result);
}

void handle_incr(KVStore& store, std::istringstream& input_stream){
    std::string key;

    auto err = cli::parse_incr(input_stream, key);
    if (handle_parse_error(err)){
        return;
    }


    auto result = store.increase_by(key, 1);
    print_integer_result(result);
}

void handle_decr(KVStore& store, std::istringstream& input_stream){
    std::string key;

    auto err = cli::parse_decr(input_stream, key);
    if (handle_parse_error(err)){
        return;
    }


    auto result = store.increase_by(key, -1);
    print_integer_result(result);
}

void handle_decrby(KVStore& store, std::istringstream& input_stream){
    std::string key;
    int amount = 0;

    auto err = cli::parse_decrby(input_stream, key, amount);
    if (handle_parse_error(err)){
        return;
    }


    auto result = store.increase_by(key, -amount);
    print_integer_result(result);

}

void handle_append(KVStore& store, std::istringstream& input_stream){
    std::string key;
    std::string value;

    auto err = cli::parse_append(input_stream, key, value);
    if (handle_parse_error(err)){
        return;
    }


    std::string result = store.append(key, value);
    std::cout << result << '\n';

}
}

int main() {
    KVStore kv_store;
    //std::string command_line;

    TCPServer tcp_server;
    auto client_fd = -1;
    try{
        client_fd = tcp_server.start(8080);
    }catch(const std::exception& e){
        std::cerr<<"Error: " <<e.what()<<'\n';

    }
    
    while (true) {
        std::cout << "> ";

        auto command_line = tcp_server.recv_line(client_fd);

        if(!command_line) {
            std::cout << "Client disconnected\n";
            tcp_server.stop(client_fd);
            break;
        }

        std::istringstream input_stream(*command_line);

        std::string command_text;
        if (!(input_stream >> command_text)) {
            continue;
        }

        auto command = cli::parse_command_type(command_text);

        switch (command) {
            case cli::Command::Set:
                handle_set(kv_store, input_stream);
                break;

            case cli::Command::Get:
                handle_get(kv_store, input_stream);
                break;

            case cli::Command::Del:
                handle_delete(kv_store, input_stream);
                break;

            case cli::Command::IncrBy:
                handle_incrby(kv_store, input_stream);
                break;

            case cli::Command::Incr:
                handle_incr(kv_store, input_stream);
                break;

            case cli::Command::Decr:
                handle_decr(kv_store, input_stream);
                break;

            case cli::Command::DecrBy:
                handle_decrby(kv_store, input_stream);
                break;

            case cli::Command::Append:
                handle_append(kv_store, input_stream);
                break;

            case cli::Command::Quit:

                std::cout << "Exiting...\n";
                return 0;

            case cli::Command::Unknown:
                std::cerr << "Error: unknown command '" << command_text << "'\n";
                break;
        }
    }

    std::cout << "Exiting...\n";

    
    return 0;
}