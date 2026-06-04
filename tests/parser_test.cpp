#include <sstream>
#include <vector>
#include <gtest/gtest.h>
#include "parser.hpp"

TEST(ParserTest, ParseCommandTypeIsCaseInsensitive) {
    EXPECT_EQ(cli::parse_command_type("set"), cli::Command::Set);
    EXPECT_EQ(cli::parse_command_type("GET"), cli::Command::Get);
    EXPECT_EQ(cli::parse_command_type("DeL"), cli::Command::Del);
}

TEST(ParserTest, ParseCommandTypeReturnsUnknownForInvalidCommand) {
    EXPECT_EQ(cli::parse_command_type("BOGUS"), cli::Command::Unknown);
}

TEST(ParserTest, ParseSetParsesKeyAndValue) {
    std::istringstream iss("name Edward");
    std::string key;
    std::string value;

    auto error = cli::parse_value(iss, key, value);

    EXPECT_EQ(error, cli::ParseError::None);
    EXPECT_EQ(key, "name");
    EXPECT_EQ(value, "Edward");
}

TEST(ParserTest, ParseSetSupportsQuotedValues) {
    std::istringstream iss("name \"Edward Achike\"");
    std::string key;
    std::string value;

    auto error = cli::parse_value(iss, key, value);

    EXPECT_EQ(error, cli::ParseError::None);
    EXPECT_EQ(key, "name");
    EXPECT_EQ(value, "Edward Achike");
}

TEST(ParserTest, ParseGetRejectsExtraTokens) {
    std::istringstream iss("name extra");
    std::string key;

    auto error = cli::parse_get(iss, key);

    EXPECT_EQ(error, cli::ParseError::ExtraTokens);
}

TEST(ParserTest, ParseDelAcceptsMultipleKeys) {
    std::istringstream iss("name age language");
    std::vector<std::string> keys;

    auto error = cli::parse_del(iss, keys);

    EXPECT_EQ(error, cli::ParseError::None);
    EXPECT_EQ(keys, std::vector<std::string>({"name", "age", "language"}));
}