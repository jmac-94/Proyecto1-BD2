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
private:
    // FILE
    string file_name;
    fstream file;

    // HEADER
    int root;
    int first_deleted_record = -1;

    // FOR BALANCE
    int parent_last_rotated_node1 = -1;
    int parent_last_rotated_node2 = -1;

public:
    explicit AVLTreeFile(string file_name) : file_name(std::move(file_name)) {
        ifstream in_file(this->file_name, ios::binary);

        if (!in_file.is_open() | fileEmpty(in_file)) {
            ofstream out_file(this->file_name, ios::binary);
            root = -1;
            writeFileRoot(out_file);
            out_file.close();
        }
        else {
            root = readFileRoot(in_file);
        }

        in_file.close();
    }

    ~AVLTreeFile() {}

    void add(Record record) {
        file.open(file_name, ios::binary | ios::in | ios::out);

        Record parent_record;
        add(root, parent_record, record);
        writeFileRoot();

        file.close();
    }

    int height() {
        return height(root);
    }

    int minValue() {
        file.open(file_name, ios::binary | ios::in | ios::out);

        int min_value = minValue(root);

        file.close();

        return min_value;
    }

    int maxValue() {
        file.open(file_name, ios::binary | ios::in | ios::out);

        int max_value = maxValue(root);

        file.close();

        return max_value;
    }

    bool isBalanced() {
        return isBalanced(root);
    }

    int size() {
        file.open(file_name, ios::binary | ios::in);

        int sz = size(root);

        file.close();

        return sz;
    }

    bool search(Record record) {
        file.open(file_name, ios::binary | ios::in | ios::out);

        bool found = search(root, record);

        file.close();

        return found;
    }

    vector<Record> rangeSearch(int begin_key, int end_key) {
        vector<Record> records;

        rangeSearch(root, records, begin_key, end_key);

        return records;
    }

private:
    // FILE METHODS
    void writeFileRoot(ofstream& out_file) {
        out_file.seekp(0, ios::beg);
        out_file.write(reinterpret_cast<char*>(&root), ROOT_SIZE);
    }

    // Writes root_header to the file
    void writeFileRoot() {
        file.seekp(0, ios::beg);
        file.write(reinterpret_cast<char*>(&root), ROOT_SIZE);
    }

    int readFileRoot(ifstream& in_file) {
        in_file.seekg(0, ios::beg);

        int file_root;
        in_file.read(reinterpret_cast<char*>(&file_root), ROOT_SIZE);

        return file_root;
    }

    // USES ITS OWN FILE
    Record getRecordByPos(int pos) {
        if (pos == -1) {
            return Record{};
        }

        fstream get_file(file_name, ios::binary | ios::in);

        Record record;

        get_file.seekg(ROOT_SIZE + pos * sizeof(Record), ios::beg);
        get_file.read(reinterpret_cast<char*>(&record), sizeof(Record));

        get_file.close();

        return record;
    }

    // USES ITS OWN FILE
    void writeRecord(Record record) {
        fstream write_file(file_name, ios::binary | ios::in | ios::out);

        write_file.seekp(ROOT_SIZE + record.pos * sizeof(Record), ios::beg);
        write_file.write(reinterpret_cast<char*>(&record), sizeof(Record));

        write_file.close();
    }

    // AVL-FILE STRUCTURE METHODS
    int add(int& pos, Record parent_current, Record& record) {
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
                current.left = add(current.left, current, record);
            }
            else if (record.code > current.code) {
                current.right = add(current.right, current, record);
            }

            updateHeight(current);
            balance(current, parent_current);

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

    void balance(Record& record, Record& parent_record) {
        // Left subtree is higher. Right rotation required.
        if (balancingFactor(record) >= 2) {
            // If child's right subtree is higher, rotate such node to the left.
            if (balancingFactor(getRecordByPos(record.left)) <= -1) {
                Record left_record = getRecordByPos(record.left);
                parent_last_rotated_node1 = left_rota(left_record, record);
            }
            parent_last_rotated_node2 = right_rota(record, parent_record);
        }
        else if (balancingFactor(record) <= -2) {
            // If child's left subtree is higher, rotate such node to the right.
            if (balancingFactor(getRecordByPos(record.right)) >= 1) {
                Record right_record = getRecordByPos(record.right);
                parent_last_rotated_node1 = right_rota(right_record, record);
            }
            parent_last_rotated_node2 = left_rota(record, parent_record);
        }
    }

    int left_rota(Record& record, Record& parent_record) {
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

    int right_rota(Record& record, Record& parent_record) {
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

    bool search(int pos, Record record) {
        if (pos == -1) {
            return false;
        }
        else {
            Record current;
            file.seekg(ROOT_SIZE + pos * sizeof(Record), ios::beg);
            file.read(reinterpret_cast<char*>(&current), sizeof(Record));

            if (record.code < current.code) {
                search(current.left, record);
            }
            else if (record.code > current.code) {
                search(current.right, record);
            }
            else if (record.code == current.code) {
                return true;
            }
        }
    }

    int minValue(int pos) {
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
            return minValue(current.left);
        }
    }

    int maxValue(int pos) {
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
            return maxValue(current.right);
        }
    }

    bool isBalanced(int pos) {
        Record record = getRecordByPos(pos);
        if (pos == -1 || (balancingFactor(record) >= -1 && balancingFactor(record) <= 1)) {
            return true;
        }
        return false;
    }

    int size(int pos) {
        if (pos == -1) {
            return 0;
        }
        else {
            Record current;
            file.seekg(ROOT_SIZE + pos * sizeof(Record), ios::beg);
            file.read(reinterpret_cast<char*>(&current), sizeof(Record));

            return 1 + size(current.left) + size(current.right);
        }
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
};

#endif//PROYECTO1_BD2_AVLTREEFILE_H
