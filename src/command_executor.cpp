#include "parser.hpp"
#include "command_executor.hpp"
#include "logger.hpp"

CommandExecutor::CommandExecutor(KVStore& store, AOFLogger* logger)
    : store(store), logger(logger){}

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
            if (logger) logger->append("SET " + parsed_input->args[0] +" "+ parsed_input->args[1] + "\n"); 
            result.response = "OK";
            break;
        case CommandType::Get:{
            auto value= store.get(parsed_input->args[0]);
            result.response = (value) ? *value : "nil";
            break;
        }
        case CommandType::Del:{
            auto num_deleted = store.erase(parsed_input->args);
            result.response = std::to_string(num_deleted);
            if (logger && num_deleted > 0){
                std::string line{"DEL"};
                for (const auto& key : parsed_input->args){
                    line += " " + key;
                }
                logger->append(line + "\n");
            }
            break;
        }
        case CommandType::Incr:{
            auto value = store.increase_by(parsed_input->args[0], 1);
            result.response = (value) ? std::to_string(*value)  : "nil";
            if (logger) logger->append("INCR "+parsed_input->args[0] + "\n");
            break;
        }
        case CommandType::IncrBy:{
             auto value = store.increase_by(parsed_input->args[0], std::stoi(parsed_input->args[1]));
            result.response = value ? std::to_string(*value)   : "nil";
            if(logger) logger->append("INCRBY " + parsed_input->args[0] + " " +parsed_input->args[1] +"\n");
            break;
        }
        case CommandType::Decr:{
            auto value = store.increase_by(parsed_input->args[0], -1);
            result.response = (value) ? std::to_string(*value) : "nil";
            if (logger) logger->append("DECR "+ parsed_input->args[0] + "\n");
            break;
        }
        case CommandType::DecrBy:{
            auto value = store.increase_by(parsed_input->args[0], -std::stoi(parsed_input->args[1]));
            result.response = value ? std::to_string(*value) : "nil";
            if (logger) logger->append("DECRBY "+parsed_input->args[0] + " " + parsed_input->args[1]+"\n");
            break;
        }
        case CommandType::Append:{
            result.response = store.append(parsed_input->args[0], parsed_input->args[1]);
            if (logger) logger->append("APPEND "+ parsed_input->args[0] + " " + parsed_input->args[1] +"\n");
            break;
        }
        case CommandType::Quit:
            result.status = CommandStatus::Quit;
            result.response = "Bye";
            break;
    }
    result.response += "\n";
    return result;
}