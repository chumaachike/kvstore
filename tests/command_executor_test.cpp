#include <gtest/gtest.h>
#include <fstream>
#include <cstdio>
#include <string>

#include "command_executor.hpp"
#include "store.hpp"
#include "logger.hpp"
#include "persistence.hpp"

static std::string read_file(const std::string& path) {
    std::ifstream in(path);
    std::string content;
    std::string line;

    while (std::getline(in, line)) {
        content += line + "\n";
    }

    return content;
}

class CommandExecutorAOFTest : public ::testing::Test {
protected:
    std::string path = "test_appendonly.aof";

    void SetUp() override {
        std::remove(path.c_str());
    }

    void TearDown() override {
        std::remove(path.c_str());
    }
};

TEST(CommandExecutorTest, SetThenGet){
    KVStore store;
    CommandExecutor executor{store, nullptr};

    auto set_result = executor.execute( "SET name Edward");
    ASSERT_EQ(set_result.response, "OK\n");    

    auto get_result = executor.execute("GET name");
    EXPECT_EQ(get_result.response, "Edward\n");
}

TEST(CommandExecutorTest, UnknownCommandReturnsError){
    KVStore store;

    CommandExecutor executor{store, nullptr};
    auto result =executor.execute( "BANANA");

    EXPECT_EQ(result.response, "ERR unknown command\n");
}

TEST(CommandExecutorTest, QuitReturnsQuitStatus){
    KVStore store;
    CommandExecutor executor{store, nullptr};

    auto result =executor.execute( "QUIT");

    EXPECT_EQ(result.status,CommandStatus::Quit);
}

TEST(CommandExecutorTest, SetThenDel){
    KVStore store;
    CommandExecutor executor{store, nullptr};

    executor.execute( "SET name Edward");
    auto result = executor.execute("GET name");

    ASSERT_EQ(result.response, "Edward\n");

    executor.execute("DEL name");
    result = executor.execute( "GET name");


     EXPECT_EQ(result.response, "nil\n");
}

TEST(CommandExecutorTest, Incr){
    KVStore store;
    CommandExecutor executor{store, nullptr};
    executor.execute( "INCR age");

    auto result = executor.execute( "GET age");

    EXPECT_EQ(result.response, "1\n");

}

TEST(CommandExecutorTest, GetMissingKeyReturnsNil) {
    KVStore store;
    CommandExecutor executor{store, nullptr};

    auto result = executor.execute("GET missing");

    EXPECT_EQ(result.response, "nil\n");
}

TEST(CommandExecutorTest, DelReturnsNumberDeleted) {
    KVStore store;
    CommandExecutor executor{store, nullptr};

    executor.execute("SET a 1");
    executor.execute("SET b 2");

    auto result = executor.execute("DEL a b c");

    EXPECT_EQ(result.response, "2\n");
}

TEST(CommandExecutorTest, IncrExistingInteger) {
    KVStore store;
    CommandExecutor executor{store, nullptr};

    executor.execute("SET count 41");
    auto result = executor.execute("INCR count");

    EXPECT_EQ(result.response, "42\n");
}

TEST(CommandExecutorTest, DecrExistingInteger) {
    KVStore store;
    CommandExecutor executor{store, nullptr};

    executor.execute("SET count 10");
    auto result = executor.execute("DECR count");

    EXPECT_EQ(result.response, "9\n");
}

TEST(CommandExecutorTest, IncrByWorks) {
    KVStore store;
    CommandExecutor executor{store, nullptr};

    auto result = executor.execute("INCRBY count 5");

    EXPECT_EQ(result.response, "5\n");
}

TEST(CommandExecutorTest, DecrByWorks) {
    KVStore store;
    CommandExecutor executor{store, nullptr};

    executor.execute("SET count 10");
    auto result = executor.execute("DECRBY count 3");

    EXPECT_EQ(result.response, "7\n");
}

TEST(CommandExecutorTest, AppendToMissingKeyCreatesValue) {
    KVStore store;
    CommandExecutor executor{store, nullptr};

    auto result = executor.execute("APPEND greeting hello");

    EXPECT_EQ(result.response, "hello\n");
}

TEST(CommandExecutorTest, AppendToExistingKey) {
    KVStore store;
    CommandExecutor executor{store, nullptr};

    executor.execute("SET greeting hello");
    auto result = executor.execute("APPEND greeting world");

    EXPECT_EQ(result.response, "helloworld\n");
}

TEST_F(CommandExecutorAOFTest, SetCommandLogsToAOF) {
    KVStore store;
    AOFLogger logger(path);
    CommandExecutor executor(store, &logger);


    auto result = executor.execute("SET name Edward");

    EXPECT_EQ(result.response, "OK\n");
    EXPECT_EQ(store.get("name"), std::string("Edward"));

    EXPECT_EQ(read_file(path), "SET name Edward\n");
}

TEST_F(CommandExecutorAOFTest, GetCommandDoesNotLogToAOF) {
    KVStore store;
    AOFLogger logger(path);
    CommandExecutor executor(store, &logger);

    store.set("name", "Edward");

    auto result = executor.execute("GET name");

    EXPECT_EQ(result.response, "Edward\n");
    EXPECT_EQ(read_file(path), "");
}

TEST_F(CommandExecutorAOFTest, DelCommandLogsWhenKeyRemoved) {
    KVStore store;
    AOFLogger logger(path);
    CommandExecutor executor(store, &logger);

    store.set("name", "Edward");
    store.set("age", "32");

    auto result = executor.execute("DEL name age");

    EXPECT_EQ(result.response, "2\n");
    EXPECT_EQ(store.get("name"), std::nullopt);
    EXPECT_EQ(store.get("age"), std::nullopt);

    EXPECT_EQ(read_file(path), "DEL name age\n");
}

TEST_F(CommandExecutorAOFTest, DelCommandDoesNotLogWhenNothingRemoved) {
    KVStore store;
    AOFLogger logger(path);
    CommandExecutor executor(store, &logger);

    auto result = executor.execute("DEL missing");

    EXPECT_EQ(result.response, "0\n");
    EXPECT_EQ(read_file(path), "");
}

TEST_F(CommandExecutorAOFTest, ExecutorWorksWithoutLogger) {
    KVStore store;
    CommandExecutor executor(store, nullptr);

    auto result = executor.execute("SET name Edward");

    EXPECT_EQ(result.response, "OK\n");
    EXPECT_EQ(store.get("name"), std::optional<std::string>("Edward"));
}

TEST (ReplayAOFTest, ReplaySetCommands){
    const std::string path = "test_appendonly.aof";
    {
        KVStore store;
        AOFLogger logger(path);
        CommandExecutor aof_executor(store, &logger);
        aof_executor.execute("SET name Edward");
        aof_executor.execute("SET age 32");
    }
    {
        KVStore new_kvstore;
        CommandExecutor replay_executor(new_kvstore, nullptr);
        replay_aof(path, replay_executor);
        EXPECT_EQ(new_kvstore.get("name"), "Edward");
        EXPECT_EQ(new_kvstore.get("age"), "32");
    }
}

TEST(ReplayAOFTest, ReplayMixedCommands){
    const std::string path = "test_appendonly.aof";
    {
        KVStore store;
        AOFLogger logger(path);
        CommandExecutor aof_executor(store, &logger);
        aof_executor.execute("SET name Edward");
        aof_executor.execute("SET age 32");
        aof_executor.execute("DEL age");
    }
    {
        KVStore new_kvstore;
        CommandExecutor replay_executor(new_kvstore, nullptr);
        replay_aof(path, replay_executor);
        EXPECT_EQ(new_kvstore.get("name"), "Edward");
        EXPECT_EQ(new_kvstore.get("age"), std::nullopt);
    }
}

TEST(ReplayAOFTest, ReplayMixedFile){
    KVStore store;

    store.set("name", "Edward");

    CommandExecutor executor(store, nullptr);

    EXPECT_NO_THROW(
        replay_aof("does_not_exist.aof", executor)
    );

    EXPECT_EQ(store.get("name"),std::optional<std::string>("Edward"));
}