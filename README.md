# Bloodhound

[![Travis CI][shields.io]][travis-ci.com]

Bloodhound is an ANSI C implementation of an AVL self-balancing binary search
tree with intrusive nodes.

## Installation

```sh
git clone https://github.com/Gregory-Meyer/bloodhound.git
mkdir bloodhound/build
cd bloodhound/build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j `nproc`
cmake --build . --target test
sudo cmake --build . --target install
```

## Usage

```c
#include <bloodhound.h>

typedef struct Node {
    AvlNode node;
    const char *key;
    int value;
} Node;

static int node_compare(const AvlNode *lhs, const AvlNode *rhs, void*) {
    return strcmp(((Node*) lhs)->key, ((Node*) rhs)->key);
}

static int node_het_compare(const void *lhs, const AvlNode *rhs, void*) {
    return strcmp((const char*) lhs, ((Node*) rhs)->key);
}

static void node_delete(AvlNode *, void*) { }

int main(void) {
    AvlTree map;
    Node first = {{NULL, NULL, 0}, "foo", 5};
    Node second = {{NULL, NULL, 0}, "foo", 10};
    Node third = {{NULL, NULL, 0}, "bar", 5};
    Node *node = NULL;

    AvlTree_new(&map, node_compare, NULL, node_delete, NULL);

    node = (Node*) AvlTree_insert(&map, &first.node);
    assert(!node);

    node = (Node*) AvlTree_insert(&map, &second.node);
    assert(node == &first);

    node = (Node*) AvlTree_insert(&map, &third.node);
    assert(!node);

    node = (Node*) AvlTree_remove(&map, "bar", node_het_compare, NULL);
    assert(node == &third);

    node = (Node*) AvlTree_get_mut(&map, "foo", node_het_compare, NULL);
    assert(node == &second);

    AvlTree_drop(&map);
}
```

## License

bloodhound is licensed under the MIT license.

[travis-ci.com]: https://travis-ci.com/Gregory-Meyer/bloodhound/
[shields.io]: https://img.shields.io/travis/com/Gregory-Meyer/bloodhound.svg
