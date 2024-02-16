#include <gtest/gtest.h>
#include "ecs/id.h"
#include "ecs/archetype.h"

using namespace mkr;
using namespace std;

TEST(id, exhaust_id) {
    auto test_ids = id();

    for (uint64_t i = 0; i < ECS_MAX_INDEX; ++i) {
        for (uint64_t j = 0; j < ECS_MAX_GENERATION; ++j) {
            auto x = test_ids.create_id();
            auto index = id::index_of(x);
            auto generation = id::generation_of(x);

            EXPECT_TRUE(id::index_of(x) == i);
            EXPECT_TRUE(id::generation_of(x) == j);
            EXPECT_TRUE(test_ids.is_valid(x));
            EXPECT_TRUE(test_ids.is_enabled(x));
            test_ids.destroy_id(x);
            EXPECT_FALSE(test_ids.is_valid(x));
        }
    }

    auto x = test_ids.create_id();
    EXPECT_TRUE(x == id::invalid_id);
    EXPECT_FALSE(test_ids.is_valid(x));
}

TEST(id, max_index) {
    auto test_ids = id();

    for (uint64_t i = 0; i < ECS_MAX_INDEX; ++i) {
        auto x = test_ids.create_id();
        EXPECT_TRUE(id::index_of(x) == i);
        EXPECT_TRUE(id::generation_of(x) == 0);
    }

    auto x = test_ids.create_id();
    EXPECT_TRUE(x == id::invalid_id);
    EXPECT_FALSE(test_ids.is_valid(x));
}

TEST(id, recycle_order) {
    auto test_ids = id();

    auto a = test_ids.create_id();
    EXPECT_TRUE(id::index_of(a) == 0);
    EXPECT_TRUE(id::generation_of(a) == 0);

    auto b = test_ids.create_id();
    EXPECT_TRUE(id::index_of(b) == 1);
    EXPECT_TRUE(id::generation_of(b) == 0);

    auto c = test_ids.create_id();
    EXPECT_TRUE(id::index_of(c) == 2);
    EXPECT_TRUE(id::generation_of(c) == 0);

    auto d = test_ids.create_id();
    EXPECT_TRUE(id::index_of(d) == 3);
    EXPECT_TRUE(id::generation_of(d) == 0);

    EXPECT_TRUE(test_ids.destroy_id(c));
    EXPECT_TRUE(test_ids.destroy_id(a));
    EXPECT_TRUE(test_ids.destroy_id(d));

    auto e = test_ids.create_id();
    EXPECT_TRUE(id::index_of(e) == 3);
    EXPECT_TRUE(id::generation_of(e) == 1);

    auto f = test_ids.create_id();
    EXPECT_TRUE(id::index_of(f) == 0);
    EXPECT_TRUE(id::generation_of(f) == 1);

    auto g = test_ids.create_id();
    EXPECT_TRUE(id::index_of(g) == 2);
    EXPECT_TRUE(id::generation_of(g) == 1);

    auto h = test_ids.create_id();
    EXPECT_TRUE(id::index_of(h) == 4);
    EXPECT_TRUE(id::generation_of(h) == 0);

    EXPECT_TRUE(test_ids.destroy_id(e));
    EXPECT_TRUE(test_ids.destroy_id(f));
    EXPECT_TRUE(test_ids.destroy_id(b));
    EXPECT_TRUE(test_ids.destroy_id(g));
    EXPECT_TRUE(test_ids.destroy_id(h));
}