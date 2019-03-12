//  MIT License
//
//  Copyright (c) 2019 Gregory Meyer
//
//  Permission is hereby granted, free of charge, to any person
//  obtaining a copy of this software and associated documentation
//  files (the "Software"), to deal in the Software without
//  restriction, including without limitation the rights to use, copy,
//  modify, merge, publish, distribute, sublicense, and/or sell copies
//  of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice (including
//  the next paragraph) shall be included in all copies or substantial
//  portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
//  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
//  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
//  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
//  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
//  ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
//  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
//  IN THE SOFTWARE.
//

#include "avl_map.h"

#include <algorithm>
#include <map>
#include <memory>
#include <numeric>
#include <random>
#include <vector>

#include <catch2/catch.hpp>

constexpr std::size_t NUM_INSERTIONS = 32768;

TEST_CASE("sorted insert, sorted get") {
    avl::Map<int, int> map;
    std::vector<int> to_insert(NUM_INSERTIONS);
    std::iota(to_insert.begin(), to_insert.end(), 0);

    for (int i : to_insert) {
        REQUIRE_FALSE(map.insert(i, i));
    }

    for (int i : to_insert) {
        REQUIRE(map.get(i));
    }
}

TEST_CASE("sorted insert, random get") {
    avl::Map<int, int> map;
    std::vector<int> to_insert(NUM_INSERTIONS);
    std::iota(to_insert.begin(), to_insert.end(), 0);

    for (int i : to_insert) {
        REQUIRE_FALSE(map.insert(i, i));
    }

    const std::unique_ptr<std::mt19937> gen_ptr(new std::mt19937());
    std::shuffle(to_insert.begin(), to_insert.end(), *gen_ptr);

    for (int i : to_insert) {
        REQUIRE(map.get(i));
    }
}

TEST_CASE("random insert, sorted get") {
    avl::Map<int, int> map;
    std::vector<int> to_insert(NUM_INSERTIONS);
    std::iota(to_insert.begin(), to_insert.end(), 0);
    const std::unique_ptr<std::mt19937> gen_ptr(new std::mt19937());
    std::shuffle(to_insert.begin(), to_insert.end(), *gen_ptr);

    for (int i : to_insert) {
        REQUIRE_FALSE(map.insert(i, i));
    }

    std::sort(to_insert.begin(), to_insert.end());

    for (int i : to_insert) {
        REQUIRE(map.get(i));
    }
}

TEST_CASE("random insert, random get") {
    avl::Map<int, int> map;
    std::vector<int> to_insert(NUM_INSERTIONS);
    std::iota(to_insert.begin(), to_insert.end(), 0);
    const std::unique_ptr<std::mt19937> gen_ptr(new std::mt19937());
    std::shuffle(to_insert.begin(), to_insert.end(), *gen_ptr);

    for (int i : to_insert) {
        REQUIRE_FALSE(map.insert(i, i));
    }

    std::shuffle(to_insert.begin(), to_insert.end(), *gen_ptr);

    for (int i : to_insert) {
        REQUIRE(map.get(i));
    }
}
