#ifndef PROYECTO1_BD2_BUCKET_H
#define PROYECTO1_BD2_BUCKET_H

#include <bitset>
#include <fstream>
#include <iostream>
#include <string>
#include "constants.h"
#include "Record.h"
using namespace std;

struct Bucket {
    Record registros[FB];
    int count;
    int next;
    Bucket () {
        count = 0;
        next = -1;
    }
};

#endif//PROYECTO1_BD2_BUCKET_H