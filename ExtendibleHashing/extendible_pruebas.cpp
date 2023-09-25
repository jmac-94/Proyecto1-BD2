#include <iostream>
#include "ExtendibleHashing.h"

using namespace std;

int main() {

    ExtendibleHashing<long> Exp_Hashing("Colores", 3);

    // for (int i = 0; i < 5; i++) {
    //     Record r1;
    //     r1.input();
    //     Exp_Hashing.insert(r1.getKey(), r1);
    // }

    ExtendibleHashing<long> hash2("Colores");

    hash2.remove(3);

    pair<bool, Record> srch = hash2.search(3);

    if (srch.first ) {
        srch.second.print();   
    } else {
        cout << "Value not found." << endl;
    }
    return 0;
}