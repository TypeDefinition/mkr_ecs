#include <gtest/gtest.h>
#include "ecs/ecs_id.h"

using namespace mkr;
using namespace std;

TEST(ecs_id, exhaust_id) {
    auto test_ids = ecs_id();

    for (uint64_t i = 0; i < ECS_MAX_INDEX; ++i) {
        for (uint64_t j = 0; j < ECS_MAX_GENERATION; ++j) {
            auto x = test_ids.create_id();
            auto index = ecs_id::index_of(x);
            auto generation = ecs_id::generation_of(x);

            EXPECT_TRUE(ecs_id::index_of(x) == i);
            EXPECT_TRUE(ecs_id::generation_of(x) == j);
            EXPECT_TRUE(test_ids.is_valid(x));
            test_ids.destroy_id(x);
            EXPECT_FALSE(test_ids.is_valid(x));
        }
    }

    auto x = test_ids.create_id();
    EXPECT_TRUE(x == ecs_id::invalid_id);
    EXPECT_FALSE(test_ids.is_valid(x));
}

TEST(ecs_id, max_index) {
    auto test_ids = ecs_id();

    for (uint64_t i = 0; i < ECS_MAX_INDEX; ++i) {
        auto x = test_ids.create_id();
        EXPECT_TRUE(ecs_id::index_of(x) == i);
        EXPECT_TRUE(ecs_id::generation_of(x) == 0);
    }

    auto x = test_ids.create_id();
    EXPECT_TRUE(x == ecs_id::invalid_id);
    EXPECT_FALSE(test_ids.is_valid(x));
}

TEST(ecs_id, recycle_order) {
    auto test_ids = ecs_id();

    auto a = test_ids.create_id();
    EXPECT_TRUE(ecs_id::index_of(a) == 0);
    EXPECT_TRUE(ecs_id::generation_of(a) == 0);

    auto b = test_ids.create_id();
    EXPECT_TRUE(ecs_id::index_of(b) == 1);
    EXPECT_TRUE(ecs_id::generation_of(b) == 0);

    auto c = test_ids.create_id();
    EXPECT_TRUE(ecs_id::index_of(c) == 2);
    EXPECT_TRUE(ecs_id::generation_of(c) == 0);

    auto d = test_ids.create_id();
    EXPECT_TRUE(ecs_id::index_of(d) == 3);
    EXPECT_TRUE(ecs_id::generation_of(d) == 0);

    EXPECT_TRUE(test_ids.destroy_id(c));
    EXPECT_TRUE(test_ids.destroy_id(a));
    EXPECT_TRUE(test_ids.destroy_id(d));

    auto e = test_ids.create_id();
    EXPECT_TRUE(ecs_id::index_of(e) == 3);
    EXPECT_TRUE(ecs_id::generation_of(e) == 1);

    auto f = test_ids.create_id();
    EXPECT_TRUE(ecs_id::index_of(f) == 0);
    EXPECT_TRUE(ecs_id::generation_of(f) == 1);

    auto g = test_ids.create_id();
    EXPECT_TRUE(ecs_id::index_of(g) == 2);
    EXPECT_TRUE(ecs_id::generation_of(g) == 1);

    auto h = test_ids.create_id();
    EXPECT_TRUE(ecs_id::index_of(h) == 4);
    EXPECT_TRUE(ecs_id::generation_of(h) == 0);

    EXPECT_TRUE(test_ids.destroy_id(e));
    EXPECT_TRUE(test_ids.destroy_id(f));
    EXPECT_TRUE(test_ids.destroy_id(b));
    EXPECT_TRUE(test_ids.destroy_id(g));
    EXPECT_TRUE(test_ids.destroy_id(h));
}