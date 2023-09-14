#include "AVLFile/AVLTreeFile.hpp"

#include <filesystem>

namespace fs = std::filesystem;

using namespace std;

void seekAllRecords() {
    AVLTreeFile<int> avlTreeFile("../AVLFile/init.bin");

    vector<NodeAVL<int>> records = avlTreeFile.seekAll();

    cout << "Root: " << avlTreeFile.getRootPos() << endl;
    for (auto& item: records) {
        item.showData();
    }
}

void seekRecordsFreeList() {
    AVLTreeFile<int> avlTreeFile("../AVLFile/init.bin");

    vector<NodeAVL<int>> records = avlTreeFile.seekFreeList();

    cout << "Free List: " << endl;
    for (auto& item: records) {
        item.showData();
    }
}

void testAdd() {
    AVLTreeFile<int> avlTreeFile("../AVLFile/init.bin");

    NodeAVL<int> r1, r2, r3, r4, r5;

    r1.key = 12;
    r2.key = 34;
    r3.key = 56;
    r4.key = 78;
    r5.key = 91;

    std::strcpy(r1.name, "John");
    std::strcpy(r2.name, "Mary");
    std::strcpy(r3.name, "Kevin");
    std::strcpy(r4.name, "Hanna");
    std::strcpy(r5.name, "Ciro");

    avlTreeFile.add(r1);
    avlTreeFile.add(r2);
    avlTreeFile.add(r3);
    avlTreeFile.add(r4);
    avlTreeFile.add(r5);

    NodeAVL<int> r6, r7;// pos: 5, pos: 6

    r6.key = 6;
    r7.key = 10;

    std::strcpy(r6.name, "Hola");
    std::strcpy(r7.name, "Chau");

    avlTreeFile.add(r6);
    avlTreeFile.add(r7);

    NodeAVL<int> r8, r9;

    r8.key = 100;
    r9.key = 95;

    std::strcpy(r8.name, "Orlando");
    std::strcpy(r9.name, "Neko");

    avlTreeFile.add(r8);
    avlTreeFile.add(r9);

    NodeAVL<int> r10, r11;

    r10.key = 300;
    r11.key = 200;

    std::strcpy(r10.name, "Samuel");
    std::strcpy(r11.name, "Willy");

    avlTreeFile.add(r10);
    avlTreeFile.add(r11);
}

void testSearch() {
    AVLTreeFile<int> avlTreeFile("../AVLFile/init.bin");

    int code = 6;
    cout << boolalpha << avlTreeFile.search(code) << endl;// true

    code = 121;
    cout << boolalpha << avlTreeFile.search(code) << endl;// false
}

void testRangeSearch() {
    AVLTreeFile<int> avlTreeFile("../AVLFile/init.bin");

    int begin_key = 6;
    int end_key = 12;
    vector<NodeAVL<int>> records = avlTreeFile.rangeSearch(begin_key, end_key);

    for (auto& item: records) {
        item.showData();
    }
}

void testRemove() {
    AVLTreeFile<int> avlTreeFile("../AVLFile/init.bin");

    avlTreeFile.remove(300);
    avlTreeFile.remove(200);
    avlTreeFile.remove(78);
}

int main() {
    cout << fs::current_path() << endl;

    testAdd();
    testRemove();
    testSearch();

    seekAllRecords();
    seekRecordsFreeList();

    return 0;
}
