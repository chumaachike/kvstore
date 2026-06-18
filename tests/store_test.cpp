// #include <gtest/gtest.h>
// #include "store.hpp"

// // Create a populate KVStore Test Fixture
// class KVStoreFixture : public ::testing::Test {
// protected:
//     KVStore store;

//     void SetUp() override {
//         store.set("name", "Edward");
//         store.set("language", "C++");
//         store.set("framework", "GoogleTest");
//     }
// };

// // Test if store can set and get value
// TEST(KVStoreTest, SetAndGetValue) {
//     KVStore store;

//     store.set("name", "Edward");

//     auto result = store.get("name");

//     ASSERT_TRUE(result.has_value());
//     EXPECT_EQ(result.value(), "Edward");
// }

// // Test if store returns nullopt for missing values
// TEST(KVStoreTest, GetMissingKeyReturnsNullopt) {
//     KVStore store;

//     auto result = store.get("missing");

//     EXPECT_FALSE(result.has_value());
// }

// // Test if store returns true if key exists
// TEST(KVStoreTest, ExistsReturnsTrueForExistingKey) {
//     KVStore store;

//     store.set("name", "chuma");

//     EXPECT_TRUE(store.exists("name"));
// }


// TEST(KVStoreTest, SetOverwritesExistingValue) {
//     KVStore store;

//     store.set("name", "chuma");
//     ASSERT_EQ(store.get("name").value(), "chuma");

//     store.set("name", "edward");
//     EXPECT_EQ(store.get("name").value(), "edward");
// }

// TEST_F(KVStoreFixture, EraseReturnsNumberOfRemovedKeys) {
//     auto result = store.erase({ "language", "framework"});
//     EXPECT_EQ(result, 2);
// }

// TEST_F(KVStoreFixture, EraseIgnoresMissingKeys) {
//     auto result = store.erase({"name", "language", "framework", "age"});
//     EXPECT_EQ(result, 3);
// }

// TEST_F(KVStoreFixture, EraseDeletesMultipleKeys){
//     auto result = store.erase({"name", "language", "framework"});

//     EXPECT_FALSE(store.get("name").has_value());
//     EXPECT_FALSE(store.get("language").has_value());
//     EXPECT_FALSE(store.get("framework").has_value());
// }

// TEST_F(KVStoreFixture, IncreaseByCreatesMissingKey){
//     auto result = store.increase_by("age", 10);
//     ASSERT_TRUE(result.has_value());
//     EXPECT_EQ(result.value(), 10);
//     EXPECT_EQ(store.get("age"), "10");
// }

// TEST_F(KVStoreFixture, IncreaseByIncrementsExistingIntegerValue){
//     store.set("age", "15");
//     auto result = store.increase_by("age", 10);

//     ASSERT_TRUE(result.has_value());
//     EXPECT_EQ(result.value(), 25);
//     EXPECT_EQ(store.get("age"), "25");
// }

// TEST_F(KVStoreFixture, IncreaseByReturnsNulloptForNonIntegerValue) {
//     auto result = store.increase_by("name", 11);
//     EXPECT_FALSE(result.has_value());
// }

// TEST_F(KVStoreFixture, IncreaseByReturnsNulloptForPartiallyNumericValue){
//    store.set("count", "123abc");

//     auto result = store.increase_by("count", 5);

//     EXPECT_FALSE(result.has_value());
//     EXPECT_EQ(store.get("count").value(), "123abc");
// }

// TEST_F(KVStoreFixture, AppendCreatesMissingKey) {
//     auto result = store.append("surname", "achike");
//     ASSERT_EQ(result, "achike");
//     EXPECT_EQ(store.get("surname").value(), "achike");
// }

// TEST_F(KVStoreFixture, AppendAddsToExistingValue) {
//     auto result = store.append("name", " achike");

//     EXPECT_EQ(result, "Edward achike");
//     EXPECT_EQ(store.get("name").value(), "Edward achike");
// }