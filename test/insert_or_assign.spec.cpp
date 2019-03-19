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

#include "avl_map.h"

#include <algorithm>
#include <map>
#include <numeric>
#include <random>
#include <vector>

#include <catch2/catch.hpp>

constexpr const char *const LONG_STRING = "this string is so long that it certainly wouldn't be SSO";

TEST_CASE("insert or assign") {
    avl::Map<std::string, int> map;

    std::string s(LONG_STRING);
    REQUIRE(map.insert_or_assign(std::move(s), 42));
    REQUIRE(s.empty()); // moved from
    s = LONG_STRING;
    REQUIRE_FALSE(map.insert_or_assign(std::move(s), 42));
    REQUIRE(s == LONG_STRING); // not moved from
}
