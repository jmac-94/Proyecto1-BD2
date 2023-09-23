#include <iostream>
#include "ExtendibleHashing.h"

using namespace std;

int main() {

    ExtendibleHashing<long> Exp_Hashing("Expandible", 4);

    for (int i = 0; i < 5; i++) {
        Record r1{};
        r1.input();
        Exp_Hashing.insert(r1.getKey(), r1);
    }


    pair<bool, Record> srch = Exp_Hashing.search(3);

    if (srch.first ) {
        srch.second.print();   
    } else {
        cout << "Value not found." << endl;
    }
    return 0;
}