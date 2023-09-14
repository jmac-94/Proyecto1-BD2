#ifndef PROYECTO1_BD2_BUCKET_H
#define PROYECTO1_BD2_BUCKET_H

#include "AVLFile/NodeAVL.h"
#include "constants.h"
#include <bitset>
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

template<typename TK>
struct Bucket {
    NodeAVL<TK> registros[FB];
    int count;
    int next;
    Bucket () {
        count = 0;
        next = -1;
    }
};

#endif//PROYECTO1_BD2_BUCKET_H