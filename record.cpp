#include <iostream>
#include <fstream>
#include "ExtendibleHashing/ExtendibleHashing.h"

#define KEY(record, atribute) record.atribute


using namespace std;

ExtendibleHashing<long> createExtendibleHashingFromAtribute(const string& filename, string atribute, int depth) {

    ifstream file(filename);
    if (!file.is_open()) throw invalid_argument("Cannot open " + filename + " file.");

    bool first = true;

    ExtendibleHashing<long> hash(filename.substr(0, filename.size() - 4), depth);

    if (atribute == "bookID") {
        string line;

        Record record;

        int lines = 0;

        while (getline(file, line)){
            if (!first) {
                record.read(line);
                hash.insert(record.bookID, record);
                lines++;
            } else {
                first = false;
            }
        }
    }

    return hash;
}


int main() {
    string filename = "books.csv";

    auto hash1 = createExtendibleHashingFromAtribute(filename, "bookID", 3);

    ExtendibleHashing<long> hash(filename.substr(0, filename.size() - 4));
    //hash.remove(14);
    auto results = hash.rangeSearch(2, 15);
    // // // // hash.read_from_direction();

    for (auto& p: results) {
        p.print();
    }

    // if (results.first) {
    //     results.second.print();
    // } else {
    //     cout << "pipipi" << endl;
    // }

    return 0;
}