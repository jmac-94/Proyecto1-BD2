#include <iostream>
#include <fstream>
//#include "Record.h"
#include "ExtendibleHashing/ExtendibleHashing.h"
#include <any>

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

        while (getline(file, line)){
            if (!first) {
                //cout << line << endl;
                record.read(line);
                //record.print();
                hash.insert(record.bookID, record);
            } else {
                first = false;
            }
        }
    }

    return hash;
}


int main() {
    string filename = "books.csv";

    //auto hash = createExtendibleHashingFromAtribute(filename, "bookID", 10);

    ExtendibleHashing<long> hash(filename.substr(0, filename.size() - 4));

    auto results = hash.search(2);

    // for (auto& p: results) {
    //     p.print();
    // }

    if (results.first) {
        results.second.print();
    } else {
        cout << "pipipi" << endl;
    }

    return 0;
}