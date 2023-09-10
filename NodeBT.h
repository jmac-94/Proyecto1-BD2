#ifndef PROYECTO1_BD2_NODEBT_H
#define PROYECTO1_BD2_NODEBT_H

template<typename T>
struct NodeBT {
public:
    T data;
    int height;

    NodeBT* left;
    NodeBT* right;

    NodeBT() : left(nullptr), right(nullptr), height(0) {}

    explicit NodeBT(T value) : data(value), left(nullptr), right(nullptr), height(0) {}

    void killSelf() {
        if (left != nullptr) left->killSelf();
        if (right != nullptr) right->killSelf();
        delete this;
    }
};

#endif //PROYECTO1_BD2_NODEBT_H
