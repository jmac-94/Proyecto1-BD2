#include <iostream>
#include <string>

using namespace std;

class Record {
    long bookID;
    string title;
    string authors;
    float average_rating;

  public:

    Record(string fila) {
        int col = 0;

        string data = "";

        for (const auto& c : fila) {
            if (c == ',') {
                switch(col) {
                    case 0:
                        this->bookID = stoi(data);
                        break;
                    case 1:
                        this->title = data;
                        break;
                    case 2:
                        this->authors = data;
                        break;
                }
                data = "";
                col++;
            } else {
                data += c;
            }
        }
        this->average_rating = stof(data);

        cout << "Record created" << endl;
        this->print();
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

};