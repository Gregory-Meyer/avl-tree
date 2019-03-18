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

#include "bloodhound.h"

#include <functional>

namespace avl {

template <typename K, typename V, typename C = std::less<K>>
class Map {
public:
    Map() noexcept {
        AvlTree_new(&impl_, Map::comparator, &comparator_, Map::deleter, nullptr);
    }

    ~Map() {
        AvlTree_drop(&impl_);
    }

    bool insert(const K &key, const V &value) {
        Node *const node = new Node(key, value);
        Node *const previous = reinterpret_cast<Node*>(AvlTree_insert(&impl_, &node->node));

        if (previous) {
            delete previous;

            return true;
        }

        return false;
    }

    bool remove(const K &key) {
        Node *const previous = reinterpret_cast<Node*>(
            AvlTree_remove(&impl_, &key, Map::het_comparator<K>, &comparator_)
        );

        if (previous) {
            delete previous;

            return true;
        }

        return false;
    }

    V* get(const K &key) noexcept {
        Node *const node = reinterpret_cast<Node*>(
            AvlTree_get_mut(&impl_, &key, Map::het_comparator<K>, &comparator_)
        );

        if (!node) {
            return nullptr;
        } else {
            return &node->value;
        }
    }

    const V* get(const K &key) const noexcept {
        const Node *const node = reinterpret_cast<const Node*>(
            AvlTree_get(&impl_, &key, Map::het_comparator<K>, &comparator_)
        );

        if (!node) {
            return nullptr;
        } else {
            return node->value;
        }
    }

    void clear() noexcept {
        AvlTree_clear(&impl_);
    }

private:
    static void deleter(AvlNode *node, void*) {
        delete reinterpret_cast<Node*>(node);
    }

    template <typename L>
    static int het_comparator(const void *lhs_v, const AvlNode *rhs_v, void *comparator_v) {
        const L &lhs = *static_cast<const L*>(lhs_v);
        const Node &rhs = *reinterpret_cast<const Node*>(rhs_v);
        C &comparator = *static_cast<C*>(comparator_v);

        if (comparator(lhs, rhs.key)) { // lhs < rhs
            return -1;
        } else if (comparator(rhs.key, lhs)) { // rhs < lhs
            return 1;
        } else {
            return 0;
        }
    }

    static int comparator(const AvlNode *lhs_v, const AvlNode *rhs_v, void *comparator_v) {
        const Node &lhs = *reinterpret_cast<const Node*>(lhs_v);
        const Node &rhs = *reinterpret_cast<const Node*>(rhs_v);
        C &comparator = *static_cast<C*>(comparator_v);

        if (comparator(lhs.key, rhs.key)) { // lhs < rhs
            return -1;
        } else if (comparator(rhs.key, lhs.key)) { // rhs < lhs
            return 1;
        } else {
            return 0;
        }
    }

    struct Node {
        template <typename L, typename W>
        Node(L &&l, W &&w)
        noexcept(std::is_nothrow_constructible<K, L>::value
                 && std::is_nothrow_constructible<V, W>::value)
        : key(std::forward<L>(l)), value(std::forward<W>(w)) { }

        AvlNode node = {nullptr, nullptr, 0};
        K key;
        V value;
    };

    AvlTree impl_;
    C comparator_;
};

} // namespace avl
