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

TEST(ParserTest, ParseValueParsesKeyAndValue) {
    std::istringstream iss("name Edward");
    std::string key;
    std::string value;

    auto error = cli::parse_value(iss, key, value);

    EXPECT_EQ(error, cli::ParseError::None);
    EXPECT_EQ(key, "name");
    EXPECT_EQ(value, "Edward");
}

TEST(ParserTest, ParseValueSupportsQuotedValues) {
    std::istringstream iss("name \"Edward Achike\"");
    std::string key;
    std::string value;

    auto error = cli::parse_value(iss, key, value);

    EXPECT_EQ(error, cli::ParseError::None);
    EXPECT_EQ(key, "name");
    EXPECT_EQ(value, "Edward Achike");
}

TEST(ParserTest, ParseValueRejectsExtraTokens) {
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

TEST(ParserTest, ParseValueRejectsUnterminatedQuote){
    std::istringstream iss ("name \"start quote without end");
    std::string key, value;
    auto error = cli::parse_value(iss, key, value);

    EXPECT_EQ(error, cli::ParseError::UnterminatedQuote);
}

TEST(ParserTest, ParseIncrByParsesAmount) {
    std::istringstream iss("counter 5");

    std::string key;
    int amount;

    auto error = cli::parse_incrby(iss, key, amount);

    EXPECT_EQ(error, cli::ParseError::None);
    EXPECT_EQ(key, "counter");
    EXPECT_EQ(amount, 5);
}

TEST(ParserTest, ParseIncrByRejectsMissingAmount) {
    std::istringstream iss("counter   ");
    std::string key;
    int amount = 0;

    auto error = cli::parse_incrby(iss, key, amount);

    EXPECT_EQ(error, cli::ParseError::MissingValue);
}

TEST(ParserTest, ParseIncrByRejectsInvalidAmount) {
    std::istringstream iss("counter five");
    std::string key;
    int amount = 0;

    auto error = cli::parse_incrby(iss, key, amount);

    EXPECT_EQ(error, cli::ParseError::InvalidInteger);
}

TEST(ParserTest, ParseIncrByRejectsExtraTokens) {
    std::istringstream iss("counter 5 extra");
    std::string key;
    int amount = 0;

    auto error = cli::parse_incrby(iss, key, amount);

    EXPECT_EQ(error, cli::ParseError::ExtraTokens);
}