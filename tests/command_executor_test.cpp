#include <gtest/gtest.h>

#include "command_executor.hpp"
#include "store.hpp"

TEST(CommandExecutorTest, SetThenGet){
    KVStore store;
    CommandExecutor executor{store};

    auto set_result = executor.execute( "SET name Edward");
    ASSERT_EQ(set_result.response, "OK\n");    

    auto get_result = executor.execute("GET name");
    EXPECT_EQ(get_result.response, "Edward\n");
}

TEST(CommandExecutorTest, UnknownCommandReturnsError){
    KVStore store;

    CommandExecutor executor{store};
    auto result =executor.execute( "BANANA");

    EXPECT_EQ(result.response, "ERR unknown command\n");
}

TEST(CommandExecutorTest, QuitReturnsQuitStatus){
    KVStore store;
    CommandExecutor executor{store};

    auto result =executor.execute( "QUIT");

    EXPECT_EQ(result.status,CommandStatus::Quit);
}

TEST(CommandExecutorTest, SetThenDel){
    KVStore store;
    CommandExecutor executor{store};

    executor.execute( "SET name Edward");
    auto result = executor.execute("GET name");

    ASSERT_EQ(result.response, "Edward\n");

    executor.execute("DEL name");
    result = executor.execute( "GET name");


     EXPECT_EQ(result.response, "nil\n");
}

TEST(CommandExecutorTest, Incr){
    KVStore store;
    CommandExecutor executor{store};
    executor.execute( "INCR age");

    auto result = executor.execute( "GET age");

    EXPECT_EQ(result.response, "1\n");

}

TEST(CommandExecutorTest, GetMissingKeyReturnsNil) {
    KVStore store;
    CommandExecutor executor{store};

    auto result = executor.execute("GET missing");

    EXPECT_EQ(result.response, "nil\n");
}

TEST(CommandExecutorTest, DelReturnsNumberDeleted) {
    KVStore store;
    CommandExecutor executor{store};

    executor.execute("SET a 1");
    executor.execute("SET b 2");

    auto result = executor.execute("DEL a b c");

    EXPECT_EQ(result.response, "2\n");
}

TEST(CommandExecutorTest, IncrExistingInteger) {
    KVStore store;
    CommandExecutor executor{store};

    executor.execute("SET count 41");
    auto result = executor.execute("INCR count");

    EXPECT_EQ(result.response, "42\n");
}

TEST(CommandExecutorTest, DecrExistingInteger) {
    KVStore store;
    CommandExecutor executor{store};

    executor.execute("SET count 10");
    auto result = executor.execute("DECR count");

    EXPECT_EQ(result.response, "9\n");
}

TEST(CommandExecutorTest, IncrByWorks) {
    KVStore store;
    CommandExecutor executor{store};

    auto result = executor.execute("INCRBY count 5");

    EXPECT_EQ(result.response, "5\n");
}

TEST(CommandExecutorTest, DecrByWorks) {
    KVStore store;
    CommandExecutor executor{store};

    executor.execute("SET count 10");
    auto result = executor.execute("DECRBY count 3");

    EXPECT_EQ(result.response, "7\n");
}

TEST(CommandExecutorTest, AppendToMissingKeyCreatesValue) {
    KVStore store;
    CommandExecutor executor{store};

    auto result = executor.execute("APPEND greeting hello");

    EXPECT_EQ(result.response, "hello\n");
}

TEST(CommandExecutorTest, AppendToExistingKey) {
    KVStore store;
    CommandExecutor executor{store};

    executor.execute("SET greeting hello");
    auto result = executor.execute("APPEND greeting world");

    EXPECT_EQ(result.response, "helloworld\n");
}