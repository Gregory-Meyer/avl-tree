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

#ifndef UTIL_H
#define UTIL_H

#include <algorithm>
#include <memory>
#include <numeric>
#include <random>
#include <utility>
#include <vector>

inline std::vector<int> iota(std::vector<int>::size_type n, int min = 0) {
    std::vector<int> v(n);
    std::iota(v.begin(), v.end(), min);

    return v;
}

template <typename URBG>
std::vector<int> rand_iota(std::vector<int>::size_type n, URBG &&urbg, int min = 0) {
    std::vector<int> v = iota(n, min);
    std::shuffle(v.begin(), v.end(), std::forward<URBG>(urbg));

    return v;
}

template <typename URBG>
std::vector<int> shuffled(std::vector<int> &&v, URBG &&urbg) {
    std::vector<int> shuffled = std::move(v);
    std::shuffle(shuffled.begin(), shuffled.end(), std::forward<URBG>(urbg));

    return shuffled;
}

inline std::vector<int> sorted(std::vector<int> &&v) {
    std::vector<int> sorted = std::move(v);
    std::sort(sorted.begin(), sorted.end());

    return sorted;
}

inline std::unique_ptr<std::mt19937> make_urbg() {
    std::unique_ptr<std::mt19937> urbg_ptr(new std::mt19937());

    return urbg_ptr;
}

inline std::vector<int> reversed(std::vector<int> &&v) {
    std::vector<int> reversed = std::move(v);
    std::reverse(reversed.begin(), reversed.end());

    return reversed;
}

template <typename F, typename = decltype(std::declval<F>()(0))>
std::vector<int> mapped(std::vector<int> &&v, F &&f) {
    std::vector<int> mapped = std::move(v);

    for (int &i : mapped) {
        i = std::forward<F>(f)(i);
    }

    return mapped;
}

#endif
