// #include <gtest/gtest.h>

// #include "command_executor.hpp"
// #include "store.hpp"

// TEST(CommandExecutorTest, SetThenGet){
//     KVStore store;

//     execute_command(store, "SET name Edward");

//     auto result = execute_command(store, "GET name");

//     EXPECT_EQ(result.response, "Edward\n");
// }

// TEST(CommandExecutorTest, UnknownCommandReturnsError){
//     KVStore store;

//     auto result =
//         execute_command(store, "BANANA");

//     EXPECT_EQ(result.response, "Error: unknown command 'BANANA'\n");
// }

// TEST(CommandExecutorTest, QuitReturnsQuitStatus){
//     KVStore store;

//     auto result =
//         execute_command(store, "QUIT");

//     EXPECT_EQ(result.status,CommandStatus::Quit);
// }

// TEST(CommandExecutorTest, SetThenDel){
//     KVStore store;

//     execute_command(store, "SET name Edward");
//     auto result = execute_command(store, "GET name");

//     ASSERT_EQ(result.response, "Edward\n");

//     execute_command(store, "DEL name");
//     result = execute_command(store, "GET name");


//      EXPECT_EQ(result.response, "nil\n");
// }

// TEST(CommandExecutorTest, Incr){
//     KVStore store;
//     execute_command(store, "INCR age");

//     auto result = execute_command(store, "GET age");

//     EXPECT_EQ(result.response, "1\n");

// }