#ifndef NODEAVL_H
#define NODEAVL_H

#include <cstring>

#include "../ExtendibleHashing/Record.h"

using namespace std;

template<typename TK>
struct NodeAVL {
    // NORMAL
    TK key{};  // key
    Record record;

    // AVL
    int pos;
    int left;
    int right;
    int height = 0;

    // FREE LIST
    int next_del = -2;  // no esta en la free list

    NodeAVL() {
        pos = -1;
        left = -1;
        right = -1;
        height = 0;
    }

    explicit NodeAVL(Record r) {
        pos = -1;
        left = -1;
        right = -1;
        height = 0;

        record = r;

        key = r.bookID;
    }

    NodeAVL(const NodeAVL<TK>& other) {
        key = other.key;

        record = other.record;

        pos = other.pos;
        left = other.left;
        right = other.right;
        height = other.height;
        next_del = other.next_del;
    }

    void copyAttributes(NodeAVL other) {
        key = other.key;
        record = other.record;
    }

    void showData() {
        cout << "pos: " << pos
             << " || key: " << key
             << " || left: " << left
             << " || right: " << right
             << " || height: " << height
             << " || next_del: " << next_del << endl;
    }
};


#endif // NODEAVL_H
