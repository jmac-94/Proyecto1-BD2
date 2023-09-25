#include <iostream>
#include "ExtendibleHashing.h"

using namespace std;

int main() {

    // ExtendibleHashing<long> Exp_Hashing("Colores", 3);

    // // for (int i = 0; i < 5; i++) {
    // //     Record r1;
    // //     r1.input();
    // //     Exp_Hashing.insert(r1.getKey(), r1);
    // // }

    // Record r1(1, "Rojo");
    // Record r2(2, "Verde");
    // Record r3(3, "Azul");
    // Record r4(4, "Rosa");
    // Record r5(5, "Marron");

    // Exp_Hashing.insert(r1.getKey(), r1);
    // Exp_Hashing.insert(r2.getKey(), r2);
    // Exp_Hashing.insert(r3.getKey(), r3);
    // Exp_Hashing.insert(r4.getKey(), r4);
    // Exp_Hashing.insert(r5.getKey(), r5);

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