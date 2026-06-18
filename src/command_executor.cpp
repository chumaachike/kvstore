#include "parser.hpp"
#include "command_executor.hpp"

CommandResult CommandExecutor::execute(const std::string& input){
    auto parsed_input = parser.parse(input);
    if (!parsed_input){
        return {CommandStatus::Continue, "Err"};
    }
    auto command = parsed_input->type;

    CommandResult result{CommandStatus::Continue, ""};
    switch (command) {
        case CommandType::Set:
            store.set(parsed_input->args[0], parsed_input->args[1]);
            result.response = "OK\n";
            break;
        case CommandType::Get:
            result.response = store.get(parsed_input->args[0]).value();
            break;
        case CommandType::Del:
            result.response = store.erase(parsed_input->args);
            break;
        case CommandType::Incr:
            result.response = store.increase_by(parsed_input->args[0], 1).value();
            break;
        case CommandType::IncrBy:
            result.response = store.increase_by(parsed_input->args[0], std::stoi(parsed_input->args[1])).value();
            break;
        case CommandType::Decr:
            result.response = store.increase_by(parsed_input->args[0], -1).value();
            break;
        case CommandType::DecrBy:
            result.response = store.increase_by(parsed_input->args[0], -std::stoi(parsed_input->args[1])).value();
            break;
        case CommandType::Append:
            result.response = store.append(parsed_input->args[0], parsed_input->args[1]);
            break;
        case CommandType::Quit:
            result.status = CommandStatus::Quit;
            result.response = "Bye\n";
            break;
    }
    return result;
}