#include <gtest/gtest.h>
#include "store.hpp"

// Create a populate KVStore Test Fixture
class KVStoreFixture : public ::testing::Test {
protected:
    KVStore store;

    void SetUp() override {
        store.set("name", "Edward");
        store.set("language", "C++");
        store.set("framework", "GoogleTest");
    }
};

// Test if store can set and get value
TEST(KVStoreTest, SetAndGetValue) {
    KVStore store;

    store.set("name", "Edward");

    auto result = store.get("name");

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), "Edward");
}

// Test if store returns nullopt for missing values
TEST(KVStoreTest, GetMissingKeyReturnsNullopt) {
    KVStore store;

    auto result = store.get("missing");

    EXPECT_FALSE(result.has_value());
}

// Test if store returns true if key exists
TEST(KVStoreTest, ExistsReturnsTrueForExistingKey) {
    KVStore store;

    store.set("name", "chuma");

    EXPECT_TRUE(store.exists("name"));
}


TEST(KVStoreTest, SetOverwritesExistingValue) {
    KVStore store;

    store.set("name", "chuma");
    ASSERT_EQ(store.get("name").value(), "chuma");

    store.set("name", "edward");
    EXPECT_EQ(store.get("name").value(), "edward");
}

TEST_F(KVStoreFixture, EraseReturnsNumberOfRemovedKeys) {
    auto result = store.erase({ "language", "framework"});
    EXPECT_EQ(result, 2);
}

TEST_F(KVStoreFixture, EraseIgnoresMissingKeys) {
    auto result = store.erase({"name", "language", "framework", "age"});
    EXPECT_EQ(result, 3);
}

TEST_F(KVStoreFixture, EraseDeletesMultipleKeys){
    auto result = store.erase({"name", "language", "framework"});

    EXPECT_FALSE(store.get("name").has_value());
    EXPECT_FALSE(store.get("language").has_value());
    EXPECT_FALSE(store.get("framework").has_value());
}

TEST_F(KVStoreFixture, IncreaseByCreatesMissingKey){
    auto result = store.increase_by("age", 10);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 10);
    EXPECT_EQ(store.get("age").value(), "10");
}

TEST_F(KVStoreFixture, IncreaseByIncrementsExistingIntegerValue){
    store.set("age", "15");
    auto result = store.increase_by("age", 10);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 25);
    EXPECT_EQ(store.get("age").value(), "25");
}

TEST_F(KVStoreFixture, IncreaseByReturnsNulloptForNonIntegerValue) {
    auto result = store.increase_by("name", 11);
    EXPECT_FALSE(result.has_value());
}

TEST_F(KVStoreFixture, IncreaseByReturnsNulloptForPartiallyNumericValue){
   store.set("count", "123abc");

    auto result = store.increase_by("count", 5);

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(store.get("count").value(), "123abc");
}

TEST_F(KVStoreFixture, AppendCreatesMissingKey) {
    auto result = store.append("surname", "achike");
    ASSERT_EQ(result, "achike");
    EXPECT_EQ(store.get("surname").value(), "achike");
}

TEST_F(KVStoreFixture, AppendAddsToExistingValue) {
    auto result = store.append("name", " achike");

    EXPECT_EQ(result, "Edward achike");
    EXPECT_EQ(store.get("name").value(), "Edward achike");
}

TEST(KVStoreTest, ExistsReturnsFalseForMissingKey) {
    KVStore store;

    EXPECT_FALSE(store.exists("missing"));
}

TEST_F(KVStoreFixture, EraseReturnsZeroWhenNoKeysExist) {
    auto result = store.erase({"age", "city"});

    EXPECT_EQ(result, 0);
}

TEST_F(KVStoreFixture, IncreaseBySupportsNegativeAmount) {
    store.set("count", "10");

    auto result = store.increase_by("count", -3);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 7);
    EXPECT_EQ(store.get("count").value(), "7");
}

TEST_F(KVStoreFixture, AppendEmptyStringDoesNotChangeExistingValue) {
    auto result = store.append("name", "");

    EXPECT_EQ(result, "Edward");
    EXPECT_EQ(store.get("name").value(), "Edward");
}

TEST(KVStoreSnapshotTest, SaveAndLoadSnapshot){
    const std::string path = "test_dump.kv";
    {
        KVStore store;
        store.set("name", "Edward");
        store.set("age", "33");
        store.set("counter", "100");
        store.set("note", "The quick brown fox");

        store.save_snapshot(path);
    }

    {
        KVStore loaded;
        loaded.load_snapshot(path);

        EXPECT_EQ(loaded.get("name"), std::optional<std::string>("Edward"));
        EXPECT_EQ(loaded.get("age"), std::optional<std::string>("33"));
        EXPECT_EQ(loaded.get("counter"), std::optional<std::string>("100"));
        EXPECT_EQ(loaded.get("note"), std::optional<std::string>("The quick brown fox"));
        EXPECT_EQ(loaded.get("missing"), std::nullopt);
    }

    std::remove(path.c_str());
}

TEST(KVStoreSnapshotTest, LoadSnapshotReplacesExistingStore) {
    const std::string path = "test_dump.kv";

    {
        KVStore store;
        store.set("name", "Edward");
        store.save_snapshot(path);
    }

    KVStore loaded;
    loaded.set("old", "value");

    loaded.load_snapshot(path);

    EXPECT_EQ(loaded.get("name"), std::optional<std::string>("Edward"));
    EXPECT_EQ(loaded.get("old"), std::nullopt);

    std::remove(path.c_str());
}

TEST(KVStoreSnapshotTest, MissingSnapshotDoesNothing) {
    KVStore store;
    store.set("name", "Edward");

    store.load_snapshot("file_that_does_not_exist.kv");

    EXPECT_EQ(store.get("name"), std::optional<std::string>("Edward"));
}