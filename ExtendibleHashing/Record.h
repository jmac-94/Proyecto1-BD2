#ifndef RECORD_H
#define RECORD_H

#include <iostream>
#include <string>
#include <sstream>
#include <cstring>

using namespace std;

struct Record {
    // PROPERTIES
    long bookID;
    char title[200];
    char authors[800];
    float average_rating;

    bool is_deleted = false;

    // Hash
    long next;

    Record() = default;

    void read(string fila) {

        strcpy(this->title,"\0");
        strcpy(this->authors,"\0");

        int col = 0;

        string data = "";

        for (const auto& c : fila) {
            if (c == ',') {
                switch(col) {
                    case 0:
                        this->bookID = stoi(data);
                        break;
                    case 1:
                        strcpy(this->title, data.c_str());
                        break;
                    case 2:
                        strcpy(this->authors, data.c_str());
                        break;
                }
                data = "";
                col++;
            } else {
                data += c;
            }
        }

        this->average_rating = stof(data);
    }

    void empty_record() {
        this->bookID = -1;
        this->next = -1;
    }

    void print() {
        cout << endl;
        cout << "================================================================" << endl;
        cout << "BookID: " << this->bookID << endl;
        cout << "title: " << this->title << endl;
        cout << "authors: " << this->authors << endl;
        cout << "average_rating: " << this->average_rating << endl;
        cout << "================================================================" << endl;
        cout << endl;
    }

    std::string toString() {
        std::stringstream ss;
        ss << bookID << ", " << this->bookID << ", " << this->title << ", " << this->next;

        return ss.str();
    }
};

#endif
