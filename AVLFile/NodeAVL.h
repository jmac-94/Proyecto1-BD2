#ifndef PROYECTO1_BD2_NODEAVL_H
#define PROYECTO1_BD2_NODEAVL_H

#include "../constants.h"
#include <cstring>

using namespace std;

template<typename TK>
struct NodeAVL {
    // NORMAL
    TK key{};  // key
    char name[12]{};

    // AVL
    int pos;
    int left;
    int right;
    int height = 0;

    // FREE LIST
    int next_del = NOT_FL;  // no esta en la free list

    NodeAVL() {
        pos = NULL_POS;
        left = NULL_POS;
        right = NULL_POS;
        height = 0;
    }

    NodeAVL(const NodeAVL<TK>& other) {
        key = other.key;
        std::strcpy(name, other.name);
        pos = other.pos;
        left = other.left;
        right = other.right;
        height = other.height;
        next_del = other.next_del;
    }

    void copyAttributes(NodeAVL other) {
        key = other.key;
        std::strcpy(name, other.name);
    }

    void showData() {
        cout << "pos: " << pos
             << " || key: " << key
             << " || name: " << name
             << " || left: " << left
             << " || right: " << right
             << " || height: " << height
             << " || next_del: " << next_del << endl;
    }
};

#endif//PROYECTO1_BD2_NODEAVL_H
