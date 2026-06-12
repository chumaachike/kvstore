#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "store.hpp"
#include "parser.hpp"



int main() {
    KVStore kv_store;
    std::string command_line;

    while (true) {
        std::cout << "> ";

        if (!std::getline(std::cin, command_line)) {
            break;
        }

        std::istringstream input_stream(command_line);

        std::string command_text;
        if (!(input_stream >> command_text)) {
            continue;
        }

        auto command = cli::parse_command_type(command_text);

        switch (command) {
            case cli::Command::Set: {
                std::string key;
                std::string value;
                
                auto err = cli::parse_set(input_stream, key, value);
                if (err != cli::ParseError::None) {
                    std::cerr << cli::error_message(err) << '\n';
                    continue;
                }

                kv_store.set(key, value);
                std::cout << "OK\n";
                break;
            }

            case cli::Command::Get: {
                std::string key;

                auto err = cli::parse_get(input_stream, key);
                if (err != cli::ParseError::None) {
                    std::cerr << cli::error_message(err) << '\n';
                    continue;
                }

                auto result = kv_store.get(key);

                if (result) {
                    std::cout << *result << '\n';
                } else {
                    std::cout << "nil\n";
                }

                break;
            }

            case cli::Command::Del: {
                std::vector<std::string> keys;
                
                auto err = cli::parse_del(input_stream, keys);
                if (err != cli::ParseError::None) {
                    std::cerr << cli::error_message(err) << '\n';
                    continue;
                }

                std::size_t removed_count = kv_store.remove(keys);
                std::cout << removed_count << '\n';
                break;
            }

            case cli::Command::IncrBy: {
                std::string key;
                int amount = 0;

                auto err = cli::parse_incrby(input_stream, key, amount);
                if (err != cli::ParseError::None) {
                   std::cerr << cli::error_message(err) << '\n';
                   continue;
                }

                auto result = kv_store.increase_by(key, amount);

                if (result) {
                    std::cout << *result << '\n';
                } else {
                    std::cerr << "Error: value is not an integer or out of range\n";
                }

                break;
            }

            case cli::Command::Incr: {
                std::string key;

                auto err = cli::parse_incr(input_stream, key);
                if (err != cli::ParseError::None) {
                    std::cerr << cli::error_message(err) << '\n';
                    continue;
                }

                auto result = kv_store.increase_by(key, 1);

                if (result) {
                    std::cout << *result << '\n';
                } else {
                    std::cerr << "Error: value is not an integer or out of range\n";
                }

                break;
            }

            case cli::Command::Decr: {
                std::string key;

                auto err = cli::parse_decr(input_stream, key);
                if (err != cli::ParseError::None) {
                    std::cerr << cli::error_message(err) << '\n';
                    continue;
                }

                auto result = kv_store.increase_by(key, -1);

                if (result) {
                    std::cout << *result << '\n';
                } else {
                    std::cerr << "Error: value is not an integer or out of range\n";
                }

                break;
            }

            case cli::Command::DecrBy: {
                std::string key;
                int amount = 0;

                auto err = cli::parse_decrby(input_stream, key, amount);
                if (err != cli::ParseError::None) {
                    std::cerr << cli::error_message(err) << '\n';
                    continue;
                }

                auto result = kv_store.increase_by(key, -amount);

                if (result) {
                    std::cout << *result << '\n';
                } else {
                    std::cerr << "Error: value is not an integer or out of range\n";
                }

                break;
            }

            case cli::Command::Append: {
                std::string key;
                std::string value;

                auto err = cli::parse_append(input_stream, key, value);
                if (err != cli::ParseError::None) {
                    std::cerr << cli::error_message(err) << '\n';
                    continue;
                }

                std::string result = kv_store.append(key, value);
                std::cout << result << '\n';

                break;
            }

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