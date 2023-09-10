#ifndef PROYECTO1_BD2_AVLTREEFILE_H
#define PROYECTO1_BD2_AVLTREEFILE_H

#include <cmath>
#include <fstream>
#include <queue>
#include <stdexcept>
#include <utility>

#include "NodeBT.h"
#include "Record.h"
#include "constants.h"

using namespace std;

bool fileEmpty(ifstream& file) {
    return file.peek() == ifstream::traits_type::eof();
}

class AVLTreeFile {
public:
    explicit AVLTreeFile(string file_name) : file_name(std::move(file_name)) {
        ifstream inFile(this->file_name, ios::binary);

        if (!inFile.is_open() | fileEmpty(inFile)) {
            ofstream outFile(this->file_name, ios::binary);
            root = -1;
            writeFileRoot(outFile);
            outFile.close();
        }
        else {
            root = readFileRoot(inFile);
        }

        inFile.close();
    }

    ~AVLTreeFile() {}

private:
    string file_name;
    int root;
    int parent_last_rotated_node1 = -1;
    int parent_last_rotated_node2 = -1;

    void writeFileRoot(ofstream& file) {
        file.seekp(0, ios::beg);
        file.write(reinterpret_cast<char*>(&root), ROOT_SIZE);
    }

    void writeFileRoot(fstream& file) {
        file.seekp(0, ios::beg);
        file.write(reinterpret_cast<char*>(&root), ROOT_SIZE);
    }

    int readFileRoot(ifstream& file) {
        file.seekg(0, ios::beg);

        int file_root;
        file.read(reinterpret_cast<char*>(&file_root), ROOT_SIZE);

        return file_root;
    }

    Record getRecordByPos(int pos) {
        if (pos == -1) {
            return Record{};
        }

        fstream file(file_name, ios::binary | ios::in);

        Record record;

        file.seekg(ROOT_SIZE + pos * sizeof(Record), ios::beg);
        file.read(reinterpret_cast<char*>(&record), sizeof(Record));

        file.close();

        return record;
    }

    void writeRecord(Record record) {
        fstream file(file_name, ios::binary | ios::in | ios::out);

        file.seekp(ROOT_SIZE + record.pos * sizeof(Record), ios::beg);
        file.write(reinterpret_cast<char*>(&record), sizeof(Record));

        file.close();
    }

    int add(int& pos, Record parent_current, Record& record, fstream& file) {
        if (pos == -1) {
            file.seekg(0, ios::end);
            pos = ((int) file.tellg() - (int) ROOT_SIZE) / (int) sizeof(Record);
            record.pos = pos;

            writeRecord(record);

            return record.pos;
        }
        else {
            Record current;
            file.seekg(ROOT_SIZE + pos * sizeof(Record), ios::beg);
            file.read(reinterpret_cast<char*>(&current), sizeof(Record));

            if (record.code < current.code) {
                current.left = add(current.left, current, record, file);
            }
            else if (record.code > current.code) {
                current.right = add(current.right, current, record, file);
            }

            updateHeight(current);
            balance(current, parent_current, file);

            if (parent_last_rotated_node1 != current.pos && parent_last_rotated_node2 != current.pos) {
                writeRecord(current);
            }
            else {
                if (parent_last_rotated_node1 != -1 && parent_last_rotated_node1 == current.pos) {
                    parent_last_rotated_node1 = -1;
                }
                else if (parent_last_rotated_node2 != -1 && parent_last_rotated_node2 == current.pos) {
                    parent_last_rotated_node2 = -1;
                }
            }

            return current.pos;
        }
    }

    int height(int pos) {
        if (pos == -1) {
            return -1;
        }

        Record record = getRecordByPos(pos);

        return record.height;
    }

    void updateHeight(Record& record) {
        record.height = max(height(record.left), height(record.right)) + 1;
    }

    int balancingFactor(Record record) {
        return height(record.left) - height(record.right);
    }

    void balance(Record& record, Record& parent_record, fstream& file) {
        // Left subtree is higher. Right rotation required.
        if (balancingFactor(record) >= 2) {
            // If child's right subtree is higher, rotate such node to the left.
            if (balancingFactor(getRecordByPos(record.left)) <= -1) {
                Record left_record = getRecordByPos(record.left);
                parent_last_rotated_node1 = left_rota(left_record, record, file);
            }
            parent_last_rotated_node2 = right_rota(record, parent_record, file);
        }
        else if (balancingFactor(record) <= -2) {
            // If child's left subtree is higher, rotate such node to the right.
            if (balancingFactor(getRecordByPos(record.right)) >= 1) {
                Record right_record = getRecordByPos(record.right);
                parent_last_rotated_node1 = right_rota(right_record, record, file);
            }
            parent_last_rotated_node2 = left_rota(record, parent_record, file);
        }
    }

    int left_rota(Record& record, Record& parent_record, fstream& file) {
        if (record.right != -1) {
            Record right_record = getRecordByPos(record.right);

            record.right = right_record.left;
            right_record.left = record.pos;

            writeRecord(record);
            writeRecord(right_record);

            updateHeight(record);
            writeRecord(record);

            updateHeight(right_record);
            writeRecord(right_record);

            writeRecord(record);
            writeRecord(right_record);

            if (parent_record.pos != record.pos && parent_record.pos != right_record.pos) {
                if (parent_record.left == record.pos) {
                    parent_record.left = right_record.pos;
                }
                else if (parent_record.right == record.pos) {
                    parent_record.right = right_record.pos;
                }

                writeRecord(parent_record);
            }

            if (record.pos == root) {
                root = right_record.pos;
            }

            return parent_record.pos;
        }

        return -1;
    }

    int right_rota(Record& record, Record& parent_record, fstream& file) {
        if (record.left != -1) {
            Record left_record = getRecordByPos(record.left);

            record.left = left_record.right;
            left_record.right = record.pos;

            writeRecord(record);
            writeRecord(left_record);

            updateHeight(record);
            writeRecord(record);

            updateHeight(left_record);
            writeRecord(left_record);

            writeRecord(record);
            writeRecord(left_record);

            if (parent_record.pos != record.pos && parent_record.pos != left_record.pos && parent_record.pos != -1) {
                if (parent_record.left == record.pos) {
                    parent_record.left = left_record.pos;
                }
                else if (parent_record.right == record.pos) {
                    parent_record.right = left_record.pos;
                }

                writeRecord(parent_record);
            }

            if (record.pos == root) {
                root = left_record.pos;
            }

            return parent_record.pos;
        }

        return -1;
    }

    bool search(int pos, Record record, fstream& file) {
        if (pos == -1) {
            return false;
        }
        else {
            Record current;
            file.seekg(ROOT_SIZE + pos * sizeof(Record), ios::beg);
            file.read(reinterpret_cast<char*>(&current), sizeof(Record));

            if (record.code < current.code) {
                search(current.left, record, file);
            }
            else if (record.code > current.code) {
                search(current.right, record, file);
            }
            else if (record.code == current.code) {
                return true;
            }
        }
    }

    int minValue(int pos, fstream& file) {
        if (pos == -1) {
            throw invalid_argument("The tree is empty");
        }

        Record current;
        file.seekg(ROOT_SIZE + pos * sizeof(Record), ios::beg);
        file.read(reinterpret_cast<char*>(&current), sizeof(Record));

        if (current.left == -1) {
            return current.code;
        }
        else {
            return minValue(current.left, file);
        }
    }

    int maxValue(int pos, fstream& file) {
        if (pos == -1) {
            throw invalid_argument("The tree is empty");
        }

        Record current;
        file.seekg(ROOT_SIZE + pos * sizeof(Record), ios::beg);
        file.read(reinterpret_cast<char*>(&current), sizeof(Record));

        if (current.right == -1) {
            return current.code;
        }
        else {
            return maxValue(current.right, file);
        }
    }

    int size(int pos, fstream& file) {
        if (pos == -1) {
            return 0;
        }
        else {
            Record current;
            file.seekg(ROOT_SIZE + pos * sizeof(Record), ios::beg);
            file.read(reinterpret_cast<char*>(&current), sizeof(Record));

            return 1 + size(current.left, file) + size(current.right, file);
        }
    }

public:
    void add(Record record) {
        fstream file(file_name, ios::binary | ios::in | ios::out);

        Record parent_record;
        add(root, parent_record, record, file);
        writeFileRoot(file);

        file.close();
    }

    int height() {
        return height(root);
    }

    int minValue() {
        fstream file(file_name, ios::binary | ios::in | ios::out);
        return minValue(root, file);
    }

    int maxValue() {
        fstream file(file_name, ios::binary | ios::in | ios::out);
        return maxValue(root, file);
    }

    bool isBalanced() {
        return isBalanced(root);
    }

    int size() {
        fstream file(file_name, ios::binary | ios::in);
        return size(root, file);
    }

//    gpt remove
//    bool remove(int delete_key) {
//        fstream file(file_name, ios::binary | ios::in | ios::out);
//
//        bool removed = remove(root, Record{}, delete_key, file);
//        writeFileRoot(file);
//
//        file.close();
//
//        return removed;
//    }
//
//    bool remove(int& pos, Record parent_current, int delete_key, fstream& file) {
//        if (pos == -1) {
//            return false;
//        }
//        else {
//            Record current;
//            file.seekg(ROOT_SIZE + pos * sizeof(Record), ios::beg);
//            file.read(reinterpret_cast<char*>(&current), sizeof(Record));
//
//            if (delete_key < current.code) {
//                if (remove(current.left, current, delete_key, file)) {
//                    // The key was found and removed in the left subtree
//                    updateHeight(current);
//                    balance(current, parent_current, file);
//
//                    if (parent_last_rotated_node1 != current.pos && parent_last_rotated_node2 != current.pos) {
//                        writeRecord(current);
//                    }
//                    else {
//                        if (parent_last_rotated_node1 != -1 && parent_last_rotated_node1 == current.pos) {
//                            parent_last_rotated_node1 = -1;
//                        }
//                        else if (parent_last_rotated_node2 != -1 && parent_last_rotated_node2 == current.pos) {
//                            parent_last_rotated_node2 = -1;
//                        }
//                    }
//                }
//                return true;
//            }
//            else if (delete_key > current.code) {
//                if (remove(current.right, current, delete_key, file)) {
//                    // The key was found and removed in the right subtree
//                    updateHeight(current);
//                    balance(current, parent_current, file);
//
//                    if (parent_last_rotated_node1 != current.pos && parent_last_rotated_node2 != current.pos) {
//                        writeRecord(current);
//                    }
//                    else {
//                        if (parent_last_rotated_node1 != -1 && parent_last_rotated_node1 == current.pos) {
//                            parent_last_rotated_node1 = -1;
//                        }
//                        else if (parent_last_rotated_node2 != -1 && parent_last_rotated_node2 == current.pos) {
//                            parent_last_rotated_node2 = -1;
//                        }
//                    }
//                }
//                return true;
//            }
//            else {
//                // Key found, mark as deleted
//                current.deleted = true;
//                writeRecord(current);
//
//                // Return true to indicate removal
//                return true;
//            }
//        }
//    }

    //    bool remove(int remove_key) {
    //        fstream file(file_name, ios::binary | ios::in | ios::out);
    //
    //        // buscar si es que existe la remove key
    //        // si es que existe proseguir
    //        // si es que no retornar que no es posible realizar el remove
    //
    //        Record current = getRecordByPos(root);
    //        Record current_parent{};
    //        return remove(current, current_parent, remove_key, file);
    //    }

    bool search(Record record) {
        fstream file(file_name, ios::binary | ios::in | ios::out);
        return search(root, record, file);
    }

    vector<Record> rangeSearch(int begin_key, int end_key) {
        vector<Record> records;
        rangeSearch(root, records, begin_key, end_key);

        return records;
    }

    void rangeSearch(int pos, vector<Record>& records, int& begin_key, int& end_key) {
        if (pos == -1) {
            return;
        }

        Record current = getRecordByPos(pos);

        rangeSearch(current.left, records, begin_key, end_key);

        if (begin_key <= current.code && current.code <= end_key) {
            records.push_back(current);
        }

        rangeSearch(current.right, records, begin_key, end_key);
    }

    bool isBalanced(int pos) {
        Record record = getRecordByPos(pos);
        if (pos == -1 || (balancingFactor(record) >= -1 && balancingFactor(record) <= 1)) {
            return true;
        }
        return false;
    }

    //    bool remove(Record& current, Record current_parent, int remove_key, fstream& file) {
    //        if (current.pos == -1) {
    //            return false;
    //        }
    //
    //        if (remove_key < current.code) {
    //            Record current_left = getRecordByPos(current.left);
    //
    //            remove(current_left, current, remove_key, file);
    //            writeRecord(current);
    //
    //            updateHeight(current);
    //            balance(current, current_parent, file);
    //        }
    //        else if (remove_key > current.code) {
    //            Record current_right = getRecordByPos(current.right);
    //            remove(current_right, current, remove_key, file);
    //            updateHeight(current);
    //            balance(current, current_parent, file);
    //        }
    //        else {
    //        }
    //    }
};

#endif//PROYECTO1_BD2_AVLTREEFILE_H
