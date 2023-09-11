#include <cstring>

#include "AVLTreeFile.h"

using namespace std;

void seekAllRecords() {
    AVLTreeFile avlTreeFile("../init.bin");

    vector<Record> records = avlTreeFile.seekAll();

    cout << "Root: " << avlTreeFile.getRootPos() << endl;
    for (auto& item: records) {
        item.showData();
    }
}

void seekRecordsFreeList() {
    AVLTreeFile avlTreeFile("../init.bin");

    vector<Record> records = avlTreeFile.seekFreeList();

    cout << "Free List: " << endl;
    for (auto& item: records) {
        item.showData();
    }
}

void testAdd() {
    AVLTreeFile avlTreeFile("../init.bin");

    Record r1, r2, r3, r4, r5;

    r1.code = 12;
    r2.code = 34;
    r3.code = 56;
    r4.code = 78;
    r5.code = 91;

    strcpy(r1.name, "John");
    strcpy(r2.name, "Mary");
    strcpy(r3.name, "Kevin");
    strcpy(r4.name, "Hanna");
    strcpy(r5.name, "Ciro");

    avlTreeFile.add(r1);
    avlTreeFile.add(r2);
    avlTreeFile.add(r3);
    avlTreeFile.add(r4);
    avlTreeFile.add(r5);

    Record r6, r7;// pos: 5, pos: 6

    r6.code = 6;
    r7.code = 10;

    strcpy(r6.name, "Hola");
    strcpy(r7.name, "Chau");

    avlTreeFile.add(r6);
    avlTreeFile.add(r7);

    Record r8, r9;

    r8.code = 100;
    r9.code = 95;

    strcpy(r8.name, "Orlando");
    strcpy(r9.name, "Neko");

    avlTreeFile.add(r8);
    avlTreeFile.add(r9);

    Record r10, r11;

    r10.code = 300;
    r11.code = 200;

    strcpy(r10.name, "Samuel");
    strcpy(r11.name, "Willy");

    avlTreeFile.add(r10);
    avlTreeFile.add(r11);
}

void testSearch() {
    AVLTreeFile avlTreeFile("../init.bin");

    Record record;

    record.code = 6;
    cout << boolalpha << avlTreeFile.search(record) << endl;// true

    record.code = 121;
    cout << boolalpha << avlTreeFile.search(record) << endl;// false
}

void testRangeSearch() {
    AVLTreeFile avlTreeFile("../init.bin");

    int begin_key = 6;
    int end_key = 12;
    vector<Record> records = avlTreeFile.rangeSearch(begin_key, end_key);

    for (auto& item: records) {
        item.showData();
    }
}

void testRemove() {
    AVLTreeFile avlTreeFile("../init.bin");

    //    avlTreeFile.remove(300);
    //    avlTreeFile.remove(200);
    avlTreeFile.remove(78);
}

int main() {
    //    testAdd();
    testRemove();

    seekAllRecords();
    seekRecordsFreeList();

    return 0;
}
