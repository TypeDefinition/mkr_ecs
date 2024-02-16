#include <gtest/gtest.h>
#include "ecs/archetype.h"

using namespace mkr;
using namespace std;

struct foo : public component {
    int val_ = 7;
    foo() = default;
    foo(int _val) : val_(_val) {}
};
struct bar : public component {
    float val_ = 54.0f;
    bar() = default;
    bar(float _val) : val_(_val) {}
};
struct baz : public component {
    char val_ = 'b';
    baz() = default;
    baz(char _val) : val_(_val) {}
};

TEST(archetype, one) {
    auto arc = archetype::make<foo>();

    mkr::id_t ent1 = 101;
    mkr::id_t ent2 = 102;

    arc->add(ent1);
    EXPECT_TRUE(arc->get<foo>(ent1)->val_ == 7);
    arc->set<foo>(ent1, foo{23});
    EXPECT_TRUE(arc->get<foo>(ent1)->val_ == 23);

    arc->add(ent2);
    arc->set<foo>(ent2, foo{52});
    EXPECT_TRUE(arc->get<foo>(ent1)->val_ == 23);
    EXPECT_TRUE(arc->get<foo>(ent2)->val_ == 52);

    delete arc;
}

TEST(archetype, two) {
    auto arc = archetype::make<foo, bar>();

    mkr::id_t ent1 = 101;
    mkr::id_t ent2 = 102;

    arc->add(ent1);
    EXPECT_TRUE(arc->get<foo>(ent1)->val_ == 7);
    arc->set<foo>(ent1, foo{23});
    EXPECT_TRUE(arc->get<foo>(ent1)->val_ == 23);

    arc->add(ent2);
    arc->set<foo>(ent2, foo{52});
    EXPECT_TRUE(arc->get<foo>(ent1)->val_ == 23);
    EXPECT_TRUE(arc->get<foo>(ent2)->val_ == 52);

    arc->set<bar>(ent2, bar{76.0f});
    EXPECT_TRUE(arc->get<foo>(ent1)->val_ == 23);
    EXPECT_TRUE(arc->get<foo>(ent2)->val_ == 52);
    EXPECT_TRUE(arc->get<bar>(ent2)->val_ == 76.0f);

    delete arc;
}

TEST(archetype, three) {
    auto arc = archetype::make<foo, bar, baz>();

    mkr::id_t ent1 = 101;
    mkr::id_t ent2 = 102;
    mkr::id_t ent3 = 103;
    mkr::id_t ent4 = 104;

    arc->add(ent1);
    EXPECT_TRUE(arc->get<foo>(ent1)->val_ == 7);
    arc->set<foo>(ent1, foo{23});
    EXPECT_TRUE(arc->get<foo>(ent1)->val_ == 23);

    arc->add(ent2);
    arc->set<foo>(ent2, foo{52});
    EXPECT_TRUE(arc->get<foo>(ent1)->val_ == 23);
    EXPECT_TRUE(arc->get<foo>(ent2)->val_ == 52);

    arc->set<bar>(ent2, bar{76.0f});
    EXPECT_TRUE(arc->get<foo>(ent1)->val_ == 23);
    EXPECT_TRUE(arc->get<foo>(ent2)->val_ == 52);
    EXPECT_TRUE(arc->get<bar>(ent2)->val_ == 76.0f);

    arc->add(ent3);
    arc->add(ent4);
    arc->set<baz>(ent3, baz('d'));
    arc->set<baz>(ent4, baz('g'));
    EXPECT_TRUE(arc->get<baz>(ent1)->val_ == 'b');
    EXPECT_TRUE(arc->get<baz>(ent2)->val_ == 'b');
    EXPECT_TRUE(arc->get<baz>(ent3)->val_ == 'd');
    EXPECT_TRUE(arc->get<baz>(ent4)->val_ == 'g');

    delete arc;
}

TEST(archetype, remove) {
    auto arc = archetype::make<foo, bar, baz>();

    mkr::id_t ent1 = 101;
    mkr::id_t ent2 = 102;
    mkr::id_t ent3 = 103;
    mkr::id_t ent4 = 104;

    arc->add(ent1);
    arc->set<foo>(ent1, foo{23});
    arc->set<bar>(ent1, bar{12.7f});
    arc->set<baz>(ent1, baz{'p'});

    arc->add(ent2);
    arc->set<foo>(ent2, foo{52});
    arc->set<bar>(ent2, bar{76.0f});
    arc->set<baz>(ent2, baz{'q'});

    arc->add(ent3);
    arc->set<foo>(ent3, foo{10});
    arc->set<bar>(ent3, bar{123.0f});
    arc->set<baz>(ent3, baz('d'));

    arc->add(ent4);
    arc->set<foo>(ent4, foo{455});
    arc->set<bar>(ent4, bar{870.456f});
    arc->set<baz>(ent4, baz('g'));

    EXPECT_TRUE(arc->get<foo>(ent1)->val_ == 23);
    EXPECT_TRUE(arc->get<bar>(ent1)->val_ == 12.7f);
    EXPECT_TRUE(arc->get<baz>(ent1)->val_ == 'p');

    EXPECT_TRUE(arc->get<foo>(ent2)->val_ == 52);
    EXPECT_TRUE(arc->get<bar>(ent2)->val_ == 76.0f);
    EXPECT_TRUE(arc->get<baz>(ent2)->val_ == 'q');

    EXPECT_TRUE(arc->get<foo>(ent3)->val_ == 10);
    EXPECT_TRUE(arc->get<bar>(ent3)->val_ == 123.0f);
    EXPECT_TRUE(arc->get<baz>(ent3)->val_ == 'd');

    EXPECT_TRUE(arc->get<foo>(ent4)->val_ == 455);
    EXPECT_TRUE(arc->get<bar>(ent4)->val_ == 870.456f);
    EXPECT_TRUE(arc->get<baz>(ent4)->val_ == 'g');

    arc->remove(ent2);

    EXPECT_TRUE(arc->get<foo>(ent1)->val_ == 23);
    EXPECT_TRUE(arc->get<bar>(ent1)->val_ == 12.7f);
    EXPECT_TRUE(arc->get<baz>(ent1)->val_ == 'p');

    EXPECT_TRUE(arc->get<foo>(ent3)->val_ == 10);
    EXPECT_TRUE(arc->get<bar>(ent3)->val_ == 123.0f);
    EXPECT_TRUE(arc->get<baz>(ent3)->val_ == 'd');

    EXPECT_TRUE(arc->get<foo>(ent4)->val_ == 455);
    EXPECT_TRUE(arc->get<bar>(ent4)->val_ == 870.456f);
    EXPECT_TRUE(arc->get<baz>(ent4)->val_ == 'g');

    arc->add(ent2);
    arc->set<foo>(ent2, foo{15});
    arc->set<bar>(ent2, bar{67.0f});
    arc->set<baz>(ent2, baz{'i'});

    EXPECT_TRUE(arc->get<foo>(ent1)->val_ == 23);
    EXPECT_TRUE(arc->get<bar>(ent1)->val_ == 12.7f);
    EXPECT_TRUE(arc->get<baz>(ent1)->val_ == 'p');

    EXPECT_TRUE(arc->get<foo>(ent2)->val_ == 15);
    EXPECT_TRUE(arc->get<bar>(ent2)->val_ == 67.0f);
    EXPECT_TRUE(arc->get<baz>(ent2)->val_ == 'i');

    EXPECT_TRUE(arc->get<foo>(ent3)->val_ == 10);
    EXPECT_TRUE(arc->get<bar>(ent3)->val_ == 123.0f);
    EXPECT_TRUE(arc->get<baz>(ent3)->val_ == 'd');

    EXPECT_TRUE(arc->get<foo>(ent4)->val_ == 455);
    EXPECT_TRUE(arc->get<bar>(ent4)->val_ == 870.456f);
    EXPECT_TRUE(arc->get<baz>(ent4)->val_ == 'g');

    delete arc;
}