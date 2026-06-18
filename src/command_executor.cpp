#include "parser.hpp"
#include "command_executor.hpp"

CommandExecutor::CommandExecutor(KVStore& store)
    : store(store) {}

CommandResult CommandExecutor::execute(const std::string& input){
    auto parsed_input = parser.parse(input);
    if (!parsed_input){
        return {CommandStatus::Continue, std::string(parser.parse_error(parsed_input.error())) +"\n"};
    }
    auto command = parsed_input->type;

    CommandResult result{CommandStatus::Continue, ""};
    switch (command) {
        case CommandType::Set:
            store.set(parsed_input->args[0], parsed_input->args[1]);
            result.response = "OK";
            break;
        case CommandType::Get:{
            auto value= store.get(parsed_input->args[0]);
            result.response = (value) ? *value : "nil";
            break;
        }
        case CommandType::Del:
            result.response = std::to_string(store.erase(parsed_input->args));
            break;
        case CommandType::Incr:{
            auto value = store.increase_by(parsed_input->args[0], 1);
            result.response = (value) ? std::to_string(*value)  : "nil";
            break;
        }
        case CommandType::IncrBy:{
             auto value = store.increase_by(parsed_input->args[0], std::stoi(parsed_input->args[1]));
            result.response = value ? std::to_string(*value)   : "nil";
            break;
        }
        case CommandType::Decr:{
            auto value = store.increase_by(parsed_input->args[0], -1);
            result.response = (value) ? std::to_string(*value) : "nil";
            break;
        }
        case CommandType::DecrBy:{auto value = store.increase_by(parsed_input->args[0], -std::stoi(parsed_input->args[1]));
            result.response = value ? std::to_string(*value) : "nil";
            break;
        }
        case CommandType::Append:
            result.response = store.append(parsed_input->args[0], parsed_input->args[1]);
            break;
        case CommandType::Quit:
            result.status = CommandStatus::Quit;
            result.response = "Bye";
            break;
    }
    result.response += "\n";
    return result;
}