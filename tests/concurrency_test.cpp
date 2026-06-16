#include <gtest/gtest.h>
#include <thread>
#include <vector>

#include "store.hpp"
TEST(KVStoreConcurrencyTest, MultipleThreadsCanIncrementSameKeySafely){
    KVStore store;

    constexpr int thread_count = 10;
    constexpr int increments_per_thread = 1000;

    std::vector<std::thread> threads;

    for (int i = 0; i < thread_count; ++i){
        threads.emplace_back([&store]{
            for (int j = 0; j < increments_per_thread; ++j){
                store.increase_by("counter", 1);
            }
        });
    }

    for (auto& thread : threads){
        thread.join();
    }

    auto result = store.get("counter");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, "10000");
}


TEST(KVStoreConcurrencyTest, ReadersAndWritersCanRunTogetherSafely) {
    KVStore store;
    store.set("name", "Edward");

    std::vector<std::thread> threads;

    // Create X threads that would read from value Y times
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([&store]() {
            for (int j = 0; j < 1000; ++j) {
                store.get("name");
            }
        });
    }

    // Create X threads that would write Value Y times
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([&store, i]() {
            for (int j = 0; j < 1000; ++j) {
                store.set("name", "Edward" + std::to_string(i));
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    auto result = store.get("name");
    ASSERT_TRUE(result.has_value());
}

TEST(KVStoreConcurrencyTest, WritersCanWriteMultipleKeys){
    KVStore store;
    std::vector<std::thread>name_threads;
    std::vector<std::thread>age_threads;

    for (int i = 0; i < 5; ++i){
        name_threads.emplace_back([&store, i](){
            for (int j = 0; j < 1000; ++j){
                store.set("name", "Chuma" + std::to_string(i));
            }
        });
    }
    for (int i = 0; i < 5; ++i){
        age_threads.emplace_back([&store, i](){
            for (int j = 0; j < 1000; ++j){
                store.set("age", std::to_string(j + i));
            }
        });
    }

    for (auto& name_thread : name_threads){
        name_thread.join();
    }

    for (auto& age_thread: age_threads){
        age_thread.join();
    }

    auto name = store.get("name");
    auto age = store.get("age");

    EXPECT_TRUE(age.has_value());
    EXPECT_TRUE(name.has_value());
}

TEST(KVStoreConcurrencyTest, ReadersCanDeleteMultipleWhileGet) {
    KVStore store;

    constexpr int N = 100;
    constexpr int readers = 8;
    constexpr int removers = 8;

    for (int i = 0; i < N; ++i) {
        store.set(std::to_string(i), "value");
    }

    std::atomic<bool> start{false};
    std::atomic<bool> stop{false};
    std::atomic<int> bad_reads{0};

    std::vector<std::thread> threads;

    for (int t = 0; t < readers; ++t) {
        threads.emplace_back([&] {
            while (!start.load(std::memory_order_acquire)) {}

            while (!stop.load(std::memory_order_acquire)) {
                for (int i = 0; i < N; ++i) {
                    auto result = store.get(std::to_string(i));

                    if (result.has_value() && result.value() != "value") {
                        bad_reads.fetch_add(1);
                    }
                }
            }
        });
    }

    for (int t = 0; t < removers; ++t) {
        threads.emplace_back([&] {
            while (!start.load(std::memory_order_acquire)) {}

            for (int i = 0; i < N; ++i) {
                store.erase({std::to_string(i)});
            }
        });
    }

    start.store(true, std::memory_order_release);

    // Wait for removers
    for (int t = readers; t < readers + removers; ++t) {
        threads[t].join();
    }

    //stop reading 
    stop.store(true, std::memory_order_release);

    for (int t = 0; t < readers; ++t) {
        threads[t].join();
    }

    EXPECT_EQ(bad_reads.load(), 0);
}

TEST(KVStoreConcurrencyTest, MultipleAppendOnSameKey){
    KVStore store;
    constexpr int num_threads = 16;
    constexpr int appends_per_thread = 100;

    const std::string key = "my_key";
     const std::string token = "x";

    std::vector<std::thread>threads;
    for (int t = 0; t < num_threads; ++t){
        threads.emplace_back([&, t](){
            for (int i = 0; i < appends_per_thread; ++i){
                store.append(key, token);
            }
        });
    }

    for (auto& th : threads){
        th.join();
    }

    auto result = store.get(key);
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result->size() == num_threads * appends_per_thread);   
}

TEST(KVStoreConcurrencyTest, WritersCanIncrementMultipleKeys) {
    KVStore store;

    constexpr int num_by_1 = 8;
    constexpr int num_by_2 = 8;
    constexpr int num_increments = 100;

    const std::string key_1 = "by_1";
    const std::string key_2 = "by_2";

    std::vector<std::thread> threads;

    for (int t = 0; t < num_by_1; ++t) {
        threads.emplace_back([&store, &key_1]() {
            for (int i = 0; i < num_increments; ++i) {
                store.increase_by(key_1, 1);
            }
        });
    }

    for (int t = 0; t < num_by_2; ++t) {
        threads.emplace_back([&store, &key_2]() {

            for (int i = 0; i < num_increments; ++i) {
                store.increase_by(key_2, 2);
            }
        });
    }


    for (auto& thread : threads) {
        thread.join();
    }

    auto res1 = store.get(key_1);
    auto res2 = store.get(key_2);

    ASSERT_TRUE(res1.has_value());
    ASSERT_TRUE(res2.has_value());

    EXPECT_EQ(res1.value(), std::to_string(num_by_1 * num_increments));
    EXPECT_EQ(res2.value(), std::to_string(num_by_2 * num_increments * 2));
}