#ifndef PROYECTO1_BD2_RECORD_H
#define PROYECTO1_BD2_RECORD_H

#include <iostream>

using namespace std;

struct Record {
    int code;
    char name[12];

    int pos = -1;
    int left = -1;
    int right = -1;

    int height = 0;
    int next_del = -2;  // no esta en la free list
    
    void showData() {
        cout << "pos: " << pos
             << " || code: " << code
             << " || name: " << name
             << " || left: " << left
             << " || right: " << right << endl;
    }
};

#endif //PROYECTO1_BD2_RECORD_H
