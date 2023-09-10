#ifndef PROYECTO1_BD2_RECORD_H
#define PROYECTO1_BD2_RECORD_H

#include "constants.h"

using namespace std;

struct Record {
    // NORMAL
    int code;  // key
    char name[12];

    // AVL
    int pos = NULL_POS;
    int left = NULL_POS;
    int right = NULL_POS;
    int height = 0;

    // FREE LIST
    int next_del = NOT_FL;  // no esta en la free list
    
    void showData() {
        cout << "pos: " << pos
             << " || code: " << code
             << " || name: " << name
             << " || left: " << left
             << " || right: " << right << endl;
    }
};

#endif //PROYECTO1_BD2_RECORD_H
