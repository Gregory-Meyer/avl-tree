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

#include "avlbst.h"

#include <functional>

namespace avl {

template <typename K, typename V, typename C = std::less<K>>
class Map {
public:
    Map() noexcept {
        AvlMap_init(&impl_, Map::comparator, &comparator_, Map::deleter, nullptr);
    }

    ~Map() {
        AvlMap_destroy(&impl_);
    }

    bool insert(const K &key, const V &value) {
        K *const key_owned = new K(key);
        V *const value_owned = new V(value);

        V *const previous = static_cast<V*>(AvlMap_insert(&impl_, key_owned, value_owned));

        if (previous) {
            delete previous;
            delete key_owned;

            return true;
        } else {
            return false;
        }
    }

    void clear() noexcept {
        AvlMap_clear(&impl_);
    }

private:
    static void deleter(void *key, void *value, void*) {
        delete static_cast<K*>(key);
        delete static_cast<V*>(value);
    }

    static int comparator(const void *lhs_v, const void *rhs_v, void *comparator_v) {
        const K &lhs = *static_cast<const K*>(lhs_v);
        const K &rhs = *static_cast<const K*>(rhs_v);
        C &comparator = *static_cast<C*>(comparator_v);

        if (comparator(lhs, rhs)) { // lhs < rhs
            return -1;
        } else if (comparator(rhs, lhs)) { // rhs < lhs
            return 1;
        } else {
            return 0;
        }
    }

    AvlMap impl_;
    C comparator_;
};

} // namespace avl
