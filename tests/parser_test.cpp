#include <gtest/gtest.h>
#include "parser.hpp"

TEST(ParserTest, ParseCommandTypeIsCaseInsensitive) {
    Parser parser;

    auto set = parser.parse("set name Edward");
    ASSERT_TRUE(set);
    EXPECT_EQ(set->type, CommandType::Set);

    auto get = parser.parse("GET name");
    ASSERT_TRUE(get);
    EXPECT_EQ(get->type, CommandType::Get);

    auto del = parser.parse("DeL name");
    ASSERT_TRUE(del);
    EXPECT_EQ(del->type, CommandType::Del);
}

TEST(ParserTest, ParseCommandTypeReturnsUnknownForInvalidCommand) {
    Parser parser;

    auto result = parser.parse("BOGUS");

    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), ParseError::UnknownCommand);
}

TEST(ParserTest, ParseSetParsesKeyAndValue) {
    Parser parser;

    auto result = parser.parse("SET name Edward");

    ASSERT_TRUE(result.has_value());

    EXPECT_EQ(result->type, CommandType::Set);
    EXPECT_EQ(result->args[0], "name");
    EXPECT_EQ(result->args[1], "Edward");
}

TEST(ParserTest, ParseSetSupportsQuotedValues) {
    Parser parser;
    auto result = parser.parse("SET name \"Edward Achike\"");
    ASSERT_TRUE(result.has_value());

    EXPECT_EQ(result->type,CommandType::Set);
    EXPECT_EQ(result->args[0], "name");
    EXPECT_EQ(result->args[1], "Edward Achike");
}

TEST(ParserTest, ParseSetReturnsUnterminatedQuoteForMissingClosingQuote){
    Parser parser;
    auto result = parser.parse("SET name \" Edward");
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), ParseError::UnterminatedQuote);
}

TEST(ParserTest, ParseSetSupportsMultiWordQuotedValue) {
    Parser parser;

    auto result = parser.parse("SET name \"Edward Achike\"");

    ASSERT_TRUE(result);
    EXPECT_EQ(result->type, CommandType::Set);
    EXPECT_EQ(result->args[0], "name");
    EXPECT_EQ(result->args[1], "Edward Achike");
}

TEST(ParserTest, ParseSetSupportsOpeningAndClosingQuoteAsOwnTokens) {
    Parser parser;

    auto result = parser.parse("SET name \" Edward \"");

    ASSERT_TRUE(result);
    EXPECT_EQ(result->type, CommandType::Set);
    EXPECT_EQ(result->args[0], "name");
    EXPECT_EQ(result->args[1], " Edward ");
}

TEST(ParserTest, ParseValueRejectsExtraTokens) {
    Parser parser;

    auto result = parser.parse("SET name \" Edward \" extra tokens");

    ASSERT_FALSE(result);

    EXPECT_EQ(result.error(), ParseError::InvalidArguments);
}

TEST(ParserTest, ParseDelAcceptsMultipleKeys) {
    Parser parser;

    auto result = parser.parse("DEL name age language");

    ASSERT_TRUE(result.has_value());

    EXPECT_EQ(result->type, CommandType::Del);
    ASSERT_EQ(result->args.size(), 3);

    EXPECT_EQ(result->args[0], "name");
    EXPECT_EQ(result->args[1], "age");
    EXPECT_EQ(result->args[2], "language");
}

TEST(ParserTest, ParseIncrByParsesAmount) {
    Parser parser;
    auto result = parser.parse("INCRBY counter 5");

    ASSERT_TRUE(result);

    EXPECT_EQ(result->args[0], "counter");
    EXPECT_EQ(result->args[1], "5");
}

TEST(ParserTest, ParseIncrByRejectsMissingAmount) {
    Parser parser;
    auto result = parser.parse("INCRBY counter   ");

    ASSERT_FALSE(result);

    EXPECT_EQ(result.error(), ParseError::InvalidArguments);
}

TEST(ParserTest, ParseIncrByRejectsInvalidAmount) {
    Parser parser;
    auto result = parser.parse("INCR counter five");
    ASSERT_FALSE(result);

    EXPECT_EQ(result.error(), ParseError::InvalidArguments);
}

TEST(ParserTest, ParseIncrByRejectsExtraTokens) {
    Parser parser;
    auto result = parser.parse("INCRBY counter 5 extra");

    ASSERT_FALSE(result);

    EXPECT_EQ(result.error(), ParseError::InvalidArguments);
}

TEST(ParserTest, ParseEmptyInputReturnsEmptyInputError) {
    Parser parser;

    auto result = parser.parse("");

    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), ParseError::EmptyInput);
}

TEST(ParserTest, ParseGetRejectsExtraTokens) {
    Parser parser;

    auto result = parser.parse("GET name extra");

    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), ParseError::InvalidArguments);
}

TEST(ParserTest, ParseDelRejectsMissingKey) {
    Parser parser;

    auto result = parser.parse("DEL");

    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), ParseError::InvalidArguments);
}

TEST(ParserTest, ParseQuitRejectsArguments) {
    Parser parser;

    auto result = parser.parse("QUIT now");

    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), ParseError::InvalidArguments);
}

TEST(ParserTest, ParseSetRejectsMissingValue) {
    Parser parser;

    auto result = parser.parse("SET name");

    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), ParseError::InvalidArguments);
}

TEST(ParserTest, ParseIncrByParsesCommandTypeKeyAndAmount) {
    Parser parser;

    auto result = parser.parse("INCRBY counter 5");

    ASSERT_TRUE(result);
    EXPECT_EQ(result->type, CommandType::IncrBy);
    EXPECT_EQ(result->args[0], "counter");
    EXPECT_EQ(result->args[1], "5");
}