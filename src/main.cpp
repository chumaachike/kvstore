#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <optional>

#include "store.hpp"
#include "parser.hpp"
#include "server.hpp"


enum class CommandStatus {
    Continue,
    Quit
};

struct CommandResult {
    CommandStatus status;
    std::string response;
};

namespace  {
std::string handle_set(KVStore& store, std::istringstream& input_stream){
    std::string key;
    std::string value;

    auto err = cli::parse_set(input_stream,  key, value); 
    if (err != cli::ParseError::None) return cli::error_message(err);

    store.set(key, value);
    return "OK\n";
}

std::string handle_get (KVStore& store, std::istringstream& input_stream){
    std::string key;

    auto err = cli::parse_get(input_stream, key);
    if (err != cli::ParseError::None) return cli::error_message(err);


    auto result = store.get(key);
    if (result) {
        return *result + '\n';
    } else {
      return "nil\n";
    }
}

std::string handle_delete (KVStore& store, std::istringstream& input_stream){
    std::vector<std::string> keys;

    auto err = cli::parse_del(input_stream, keys);
    if (err != cli::ParseError::None) return cli::error_message(err);

    std::size_t removed_count = store.erase(keys);
    return std::to_string(removed_count) + '\n';
}

std::string handle_incrby(KVStore& store, std::istringstream& input_stream){
    std::string key;
    int amount = 0;

    auto err = cli::parse_incrby(input_stream, key, amount);

    if (err!= cli::ParseError::None) return cli::error_message(err);    

    auto result = store.increase_by(key, amount);

    if (!result) return "Error: value is not an integer or out of range\n";

    return std::to_string(*result) + '\n';
}

std::string handle_incr(KVStore& store, std::istringstream& input_stream){
    std::string key;

    auto err = cli::parse_incr(input_stream, key);
    if (err != cli::ParseError::None) return cli::error_message(err);

    auto result = store.increase_by(key, 1);
    if (!result) return "Error: value is not an integer or out of range\n";

    return std::to_string(*result) + '\n';
}

std::string handle_decr(KVStore& store, std::istringstream& input_stream){
    std::string key;

    auto err = cli::parse_decr(input_stream, key);
    if (err != cli::ParseError::None) return cli::error_message(err);


    auto result = store.increase_by(key, -1);
    if (!result) return "Error: value is not an integer or out of range\n";

    return std::to_string(*result) + '\n';
}

std::string handle_decrby(KVStore& store, std::istringstream& input_stream){
    std::string key;
    int amount = 0;

    auto err = cli::parse_decrby(input_stream, key, amount);
    if (err != cli::ParseError::None) return cli::error_message(err);


    auto result = store.increase_by(key, -amount);

    if (!result) return "Error: value is not an integer or out of range\n";

    return std::to_string(*result) + '\n';

}

std::string handle_append(KVStore& store, std::istringstream& input_stream){
    std::string key;
    std::string value;

    auto err = cli::parse_append(input_stream, key, value);
    if (err != cli::ParseError::None) return cli::error_message(err);

    std::string result = store.append(key, value);
    return result + '\n';

}
}

CommandResult execute_command(KVStore& kv_store, const std::string& command_line){
    std::istringstream input_stream(command_line);

    std::string command_text;
    if (!(input_stream >> command_text)) {
        return {CommandStatus::Continue, "\n"};
    }
    auto command = cli::parse_command_type(command_text);
    
    CommandResult result{CommandStatus::Continue, ""};
    switch (command) {
        case cli::Command::Set:
            result.response = handle_set(kv_store, input_stream);
            break;

        case cli::Command::Get:
            result.response = handle_get(kv_store, input_stream);
            break;

        case cli::Command::Del:
            result.response = handle_delete(kv_store, input_stream);
            break;

        case cli::Command::IncrBy:
            result.response = handle_incrby(kv_store, input_stream);
            break;

        case cli::Command::Incr:
            result.response = handle_incr(kv_store, input_stream);
            break;

        case cli::Command::Decr:
            result.response = handle_decr(kv_store, input_stream);
            break;

        case cli::Command::DecrBy:
            result.response = handle_decrby(kv_store, input_stream);
            break;

        case cli::Command::Append:
            result.response = handle_append(kv_store, input_stream);
            break;

        case cli::Command::Quit:
           return {CommandStatus::Quit, "Bye\n"};

        case cli::Command::Unknown:
                result.response = "Error: unknown command '" + command_text + "'\n";
            break;
        }
        return result;
}

int main() {
    KVStore kv_store;
    TCPServer tcp_server;
    auto client_fd = -1;

    try{
        client_fd = tcp_server.start(8080);
    }catch(const std::exception& e){
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    std::cout << "Client Connected\n";
    
    while (true) {
        auto command_line = tcp_server.recv_line(client_fd);

        if (!command_line) {
            std::cout << "Client disconnected\n";
            tcp_server.stop(client_fd);
            break;
        }

        auto result = execute_command(kv_store, *command_line);

        if (!tcp_server.send_all(client_fd, result.response.c_str(), result.response.size())) {
            std::cout << "Failed to send response\n";
            tcp_server.stop(client_fd);
            break;
        }

        if (result.status == CommandStatus::Quit) {
            std::cout << "Exiting...\n";
            tcp_server.stop(client_fd);
            break;
        }
    }    
    return 0;
}