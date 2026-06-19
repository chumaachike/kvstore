#include <gtest/gtest.h>
#include <string>
#include <fstream>

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
        persistence::replay_aof(path, replay_executor);
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
        persistence::replay_aof(path, replay_executor);
        EXPECT_EQ(new_kvstore.get("name"), "Edward");
        EXPECT_EQ(new_kvstore.get("age"), std::nullopt);
    }
}

TEST(ReplayAOFTest, ReplayMixedFile){
    KVStore store;

    store.set("name", "Edward");

    CommandExecutor executor(store, nullptr);

    EXPECT_NO_THROW(
        persistence::replay_aof("does_not_exist.aof", executor)
    );

    EXPECT_EQ(store.get("name"),std::optional<std::string>("Edward"));
}

TEST(AOFRewriteTest, SavesSnapshotAndClearsAOF) {
    const std::string snapshot_path = "test_dump.kv";
    const std::string aof_path = "test_appendonly.aof";

    std::remove(snapshot_path.c_str());
    std::remove(aof_path.c_str());

    KVStore store;
    store.set("name", "Edward");
    store.set("counter", "100");

    AOFLogger logger(aof_path);
    logger.append("SET old value");

    persistence::rewrite_aof(store, logger, snapshot_path, aof_path);

    KVStore loaded;
    loaded.load_snapshot(snapshot_path);

    EXPECT_EQ(loaded.get("name"), std::optional<std::string>("Edward"));
    EXPECT_EQ(loaded.get("counter"), std::optional<std::string>("100"));

    std::ifstream aof(aof_path);
    std::string content(
        (std::istreambuf_iterator<char>(aof)),
        std::istreambuf_iterator<char>()
    );

    EXPECT_EQ(content, "");

    std::remove(snapshot_path.c_str());
    std::remove(aof_path.c_str());
}

TEST(AOFRewriteTest, LoggerStillWorksAfterRewrite) {
    const std::string snapshot_path = "test_dump.kv";
    const std::string aof_path = "test_appendonly.aof";

    std::remove(snapshot_path.c_str());
    std::remove(aof_path.c_str());

    KVStore store;
    store.set("name", "Edward");

    AOFLogger logger(aof_path);

    persistence::rewrite_aof(store, logger, snapshot_path, aof_path);

    logger.append("SET city London");

    EXPECT_EQ(read_file(aof_path), "SET city London\n");

    std::remove(snapshot_path.c_str());
    std::remove(aof_path.c_str());
}