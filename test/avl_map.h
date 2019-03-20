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

#ifndef AVL_MAP_H
#define AVL_MAP_H

#include "bloodhound.h"

#include <functional>
#include <type_traits>
#include <utility>

namespace avl {

struct Less {
    template <typename T, typename U,
              typename = decltype(std::declval<const T&>() < std::declval<const U&>())>
    bool operator()(const T &lhs, const U &rhs) const noexcept {
        return static_cast<bool>(lhs < rhs);
    }
};

template <typename K, typename V>
class Map {
public:
    Map() noexcept {
        AvlTree_new(&impl_, Map::comparator, &comparator_, Map::deleter, nullptr);
    }

    ~Map() {
        AvlTree_drop(&impl_);
    }

    template <typename L, typename W,
              typename std::enable_if<std::is_constructible<K, L>::value
                                      && std::is_constructible<V, W>::value, int>::type = 0>
    std::pair<std::pair<K, V>&, bool> insert(L &&key, W &&value) {
        Node *const node = new Node(std::forward<L>(key), std::forward<W>(value));
        Node *const previous = reinterpret_cast<Node*>(AvlTree_insert(&impl_, &node->node));

        if (previous) {
            delete previous;

            return {node->kv, true};
        }

        return {node->kv, false};
    }

    template <typename L, typename W,
              typename std::enable_if<std::is_constructible<K, L>::value
                                      && std::is_constructible<V, W>::value
                                      && std::is_assignable<V&, W>::value, int>::type = 0,
              typename = decltype(std::declval<const typename std::decay<L>::type&>() < std::declval<const K&>()),
              typename = decltype(std::declval<const K&>() < std::declval<const typename std::decay<L>::type&>())>
    std::pair<std::pair<K, V>&, bool> insert_or_assign(L &&key, W &&value) {
        using T = typename std::decay<L>::type;

        std::pair<L&&, W&&> kv(std::forward<L>(key), std::forward<W>(value));
        const T &key_ref = key;
        int inserted;

        Node *const node = reinterpret_cast<Node*>(
            AvlTree_get_or_insert(&impl_, &key_ref, Map::het_comparator<T>,
                                  &comparator_, Map::do_insert<L, W>, &kv, &inserted)
        );

        if (!inserted) {
            node->kv.second = std::forward<W>(value);

            return {node->kv, false};
        }

        return {node->kv, true};
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
            return &node->kv.second;
        }
    }

    const V* get(const K &key) const noexcept {
        const Node *const node = reinterpret_cast<const Node*>(
            AvlTree_get(&impl_, &key, Map::het_comparator<K>, &comparator_)
        );

        if (!node) {
            return nullptr;
        } else {
            return &node->kv.second;
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
        Less &comparator = *static_cast<Less*>(comparator_v);

        if (comparator(lhs, rhs.kv.first)) { // lhs < rhs
            return -1;
        } else if (comparator(rhs.kv.first, lhs)) { // rhs < lhs
            return 1;
        } else {
            return 0;
        }
    }

    static int comparator(const AvlNode *lhs_v, const AvlNode *rhs_v, void *comparator_v) {
        const Node &lhs = *reinterpret_cast<const Node*>(lhs_v);
        const Node &rhs = *reinterpret_cast<const Node*>(rhs_v);
        Less &comparator = *static_cast<Less*>(comparator_v);

        if (comparator(lhs.kv.first, rhs.kv.first)) { // lhs < rhs
            return -1;
        } else if (comparator(rhs.kv.first, lhs.kv.first)) { // rhs < lhs
            return 1;
        } else {
            return 0;
        }
    }

    template <typename L, typename W>
    static AvlNode* do_insert(const void*, void *kv_v) {
        std::pair<L&&, W&&> &kv = *static_cast<std::pair<L&&, W&&>*>(kv_v);

        Node *const node = new Node(std::forward<L>(kv.first), std::forward<W>(kv.second));

        return &node->node;
    }

    struct Node {
        template <typename L, typename W,
                  typename std::enable_if<std::is_constructible<K, L>::value
                                          && std::is_constructible<V, W>::value, int>::type = 0>
        Node(L &&l, W &&w)
        noexcept(std::is_nothrow_constructible<K, L>::value
                 && std::is_nothrow_constructible<V, W>::value)
        : kv(std::forward<L>(l), std::forward<W>(w)) { }

        AvlNode node = {nullptr, nullptr, 0};
        std::pair<K, V> kv;
    };

    AvlTree impl_;
    Less comparator_;
};

} // namespace avl

#endif
