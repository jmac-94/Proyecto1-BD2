#ifndef PROYECTO1_BD2_EXTENDIBLEHASHING_H
#define PROYECTO1_BD2_EXTENDIBLEHASHING_H

#include <bitset>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

bool fileEmpty(ifstream& file) {
    return file.peek() == ifstream::traits_type::eof();
}

class ExtendibleHash {
  private:
    // about the file
    string filename;
    fstream file;

    //

  public:
    explicit ExtendibleHash(string filename) : filename(std::move(filename)) {
        ifstream in_file(this->filename, ios::binary);

        // Empty file or it doesn't exists
        if (!in_file.is_open() | fileEmpty(in_file)) {
            ofstream out_file(this->filename, ios::binary);
            // create and write header
            out_file.close();
        }
        else {
            // read header
        }

        in_file.close();
    }
};

#endif//PROYECTO1_BD2_EXTENDIBLEHASHING_H