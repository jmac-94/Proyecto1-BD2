#ifndef PROYECTO1_BD2_AVLTREEFILE_H
#define PROYECTO1_BD2_AVLTREEFILE_H

#include <cmath>
#include <fstream>
#include <iostream>
#include <queue>
#include <stdexcept>
#include <tuple>
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
    int first_deleted_record;// -1: NULL_POS: end of list, -2: NOT_FL : not in FL, other_positive_number: in FL

    // FOR BALANCE
    int parent_last_rotated_node1 = NULL_POS;
    int parent_last_rotated_node2 = NULL_POS;

public:
    explicit AVLTreeFile(string file_name) : file_name(std::move(file_name)) {
        ifstream in_file(this->file_name, ios::binary);

        // Empty file or it doesn't exists
        if (!in_file.is_open() | fileEmpty(in_file)) {
            ofstream out_file(this->file_name, ios::binary);
            root = NULL_POS;
            first_deleted_record = NULL_POS;
            writeFileHeader(out_file);
            out_file.close();
        }
        else {
            std::tie(root, first_deleted_record) = readFileHeader(in_file);
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

    bool remove(int key) {
        file.open(file_name, ios::binary | ios::in | ios::out);

        Record parent_record;
        bool removed = remove(root, parent_record, key);
        writeFileRoot();

        file.close();

        return removed;
    }

    vector<Record> seekAll() {
        vector<Record> records;

        seekAll(root, records);

        return records;
    }

    vector<Record> seekFreeList() {
        vector<Record> records;

        seekFreeList(first_deleted_record, records);

        return records;
    }

    int getRootPos() {
        return root;
    }

    Record getRoot() {
        return getRecordByPos(root);
    }

private:
    // FILE METHODS
    // Writes root_header to the file
    void writeFileRoot() {
        file.seekp(0, ios::beg);
        file.write(reinterpret_cast<char*>(&root), ROOT_SIZE);
    }

public:
    void writeFileHeader(fstream& out_file) {
        // ROOT
        out_file.seekp(0, ios::beg);
        out_file.write(reinterpret_cast<char*>(&root), ROOT_SIZE);

        // FIRST_DELETED_RECORD
        out_file.seekp(ROOT_SIZE, ios::beg);
        out_file.write(reinterpret_cast<char*>(&first_deleted_record), FDR_SIZE);
    }

    void writeFileHeader(ofstream& out_file) {
        // ROOT
        out_file.seekp(0, ios::beg);
        out_file.write(reinterpret_cast<char*>(&root), ROOT_SIZE);

        // FIRST_DELETED_RECORD
        out_file.seekp(ROOT_SIZE, ios::beg);
        out_file.write(reinterpret_cast<char*>(&first_deleted_record), FDR_SIZE);
    }

    // pair<root, first_deleted_record>
    pair<int, int> readFileHeader(ifstream& in_file) {
        // ROOT
        int file_root;
        in_file.seekg(0, ios::beg);
        in_file.read(reinterpret_cast<char*>(&file_root), ROOT_SIZE);

        // FIRST_DELETED_RECORD
        int file_fdr;
        in_file.seekg(ROOT_SIZE, ios::beg);
        in_file.read(reinterpret_cast<char*>(&file_fdr), FDR_SIZE);

        return make_pair(file_root, file_fdr);
    }

    // USES ITS OWN FILE
    Record getRecordByPos(int pos) {
        if (pos == NULL_POS) {
            return Record{};
        }

        fstream get_file(file_name, ios::binary | ios::in);

        Record record;

        get_file.seekg(HEADER_SIZE + pos * sizeof(Record), ios::beg);
        get_file.read(reinterpret_cast<char*>(&record), sizeof(Record));

        get_file.close();

        return record;
    }

    // USES ITS OWN FILE
    void writeRecord(Record record) {
        fstream write_file(file_name, ios::binary | ios::in | ios::out);

        write_file.seekp(HEADER_SIZE + record.pos * sizeof(Record), ios::beg);
        write_file.write(reinterpret_cast<char*>(&record), sizeof(Record));

        write_file.close();
    }

    // AVL-FILE STRUCTURE METHODS
    int add(int& pos, Record parent_current, Record& record) {
        if (pos == NULL_POS) {
            if (first_deleted_record != NULL_POS) {
                Record deleted_record = getRecordByPos(first_deleted_record);
                first_deleted_record = deleted_record.next_del;

                pos = deleted_record.pos;
            }
            else {
                file.seekg(0, ios::end);
                pos = ((int) file.tellg() - (int) HEADER_SIZE) / (int) sizeof(Record);
            }

            record.pos = pos;
            writeRecord(record);

            return record.pos;
        }
        else {
            Record current;
            file.seekg(HEADER_SIZE + pos * sizeof(Record), ios::beg);
            file.read(reinterpret_cast<char*>(&current), sizeof(Record));

            if (record.code < current.code) {
                current.left = add(current.left, current, record);
            }
            else if (record.code > current.code) {
                current.right = add(current.right, current, record);
            }

            updateHeight(current);
            balance(current, parent_current);

            // Check if record has been rotated and don't write to avoid overwrite smth
            if (parent_last_rotated_node1 != current.pos && parent_last_rotated_node2 != current.pos) {
                writeRecord(current);
            }
            else {
                if (parent_last_rotated_node1 != NULL_POS && parent_last_rotated_node1 == current.pos) {
                    parent_last_rotated_node1 = NULL_POS;
                }
                else if (parent_last_rotated_node2 != NULL_POS && parent_last_rotated_node2 == current.pos) {
                    parent_last_rotated_node2 = NULL_POS;
                }
            }

            return current.pos;
        }
    }

    int height(int pos) {
        if (pos == NULL_POS) {
            return NULL_POS;
        }

        Record record = getRecordByPos(pos);

        return record.height;
    }

    void updateHeight(Record& record) {
        if (record.next_del != NOT_FL) {
            return;
        }
        record.height = max(height(record.left), height(record.right)) + 1;
    }

    int balancingFactor(Record record) {
        int l_h = height(record.left);
        int r_h = height(record.right);

        return l_h - r_h;
    }

    void balance(Record& record, Record& parent_record) {
        if (record.next_del != -2) {
            return;
        }

        // Left subtree is higher. Right rotation required.
        if (balancingFactor(record) >= 2) {
            // If child's right subtree is higher, rotate such node to the left.
            if (balancingFactor(getRecordByPos(record.left)) <= NULL_POS) {
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
        if (record.right != NULL_POS) {
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

        return NULL_POS;
    }

    int right_rota(Record& record, Record& parent_record) {
        if (record.left != NULL_POS) {
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

            if (parent_record.pos != record.pos && parent_record.pos != left_record.pos && parent_record.pos != NULL_POS) {
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

        return NULL_POS;
    }

    bool search(int pos, Record record) {
        if (pos == NULL_POS) {
            return false;
        }
        else {
            Record current;
            file.seekg(HEADER_SIZE + pos * sizeof(Record), ios::beg);
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
        if (pos == NULL_POS) {
            throw invalid_argument("The tree is empty");
        }

        Record current;
        file.seekg(HEADER_SIZE + pos * sizeof(Record), ios::beg);
        file.read(reinterpret_cast<char*>(&current), sizeof(Record));

        if (current.left == NULL_POS) {
            return current.code;
        }
        else {
            return minValue(current.left);
        }
    }

    int maxValue(int pos) {
        if (pos == NULL_POS) {
            throw invalid_argument("The tree is empty");
        }

        Record current;
        file.seekg(HEADER_SIZE + pos * sizeof(Record), ios::beg);
        file.read(reinterpret_cast<char*>(&current), sizeof(Record));

        if (current.right == NULL_POS) {
            return current.code;
        }
        else {
            return maxValue(current.right);
        }
    }

    bool isBalanced(int pos) {
        Record record = getRecordByPos(pos);
        if (pos == NULL_POS || (balancingFactor(record) >= NULL_POS && balancingFactor(record) <= 1)) {
            return true;
        }
        return false;
    }

    int size(int pos) {
        if (pos == NULL_POS) {
            return 0;
        }
        else {
            Record current;
            file.seekg(HEADER_SIZE + pos * sizeof(Record), ios::beg);
            file.read(reinterpret_cast<char*>(&current), sizeof(Record));

            return 1 + size(current.left) + size(current.right);
        }
    }

    void rangeSearch(int pos, vector<Record>& records, int& begin_key, int& end_key) {
        if (pos == NULL_POS) {
            return;
        }

        Record current = getRecordByPos(pos);

        rangeSearch(current.left, records, begin_key, end_key);

        if (begin_key <= current.code && current.code <= end_key) {
            records.push_back(current);
        }

        rangeSearch(current.right, records, begin_key, end_key);
    }

    bool remove(int pos, Record& parent_current, int key) {
        if (pos == NULL_POS) {
            return false;
        }

        Record current;
        file.seekg(HEADER_SIZE + pos * sizeof(Record), ios::beg);
        file.read(reinterpret_cast<char*>(&current), sizeof(Record));

        bool removed = false;
        if (key < current.code) {
            removed = remove(current.left, current, key);

            updateHeight(current);
            balance(current, parent_current);

            // Check if record has been rotated and don't write to avoid overwrite smth
            // Also check if its child hash been removed
            if (!removed && parent_last_rotated_node1 != current.pos && parent_last_rotated_node2 != current.pos) {
                writeRecord(current);
            }
            else {
                if (parent_last_rotated_node1 != NULL_POS && parent_last_rotated_node1 == current.pos) {
                    parent_last_rotated_node1 = NULL_POS;
                }
                else if (parent_last_rotated_node2 != NULL_POS && parent_last_rotated_node2 == current.pos) {
                    parent_last_rotated_node2 = NULL_POS;
                }
            }
        }
        else if (key > current.code) {
            removed = remove(current.right, current, key);
            updateHeight(current);
            balance(current, parent_current);

            // Check if record has been rotated and don't write to avoid overwrite smth
            if (!removed && parent_last_rotated_node1 != current.pos && parent_last_rotated_node2 != current.pos) {
                writeRecord(current);
            }
            else {
                if (parent_last_rotated_node1 != NULL_POS && parent_last_rotated_node1 == current.pos) {
                    parent_last_rotated_node1 = NULL_POS;
                }
                else if (parent_last_rotated_node2 != NULL_POS && parent_last_rotated_node2 == current.pos) {
                    parent_last_rotated_node2 = NULL_POS;
                }
            }
        }
        else {
            // LEAF NODE
            if (current.left == -1 && current.right == -1) {
                // "REMOVE" IT FROM FILE
                current.next_del = first_deleted_record;
                first_deleted_record = current.pos;

                writeRecord(current);

                fstream out_file(file_name, ios::binary | ios::in | ios::out);
                writeFileHeader(out_file);
                out_file.close();

                // UPDATE PARENT
                if (parent_current.left == pos) {
                    parent_current.left = NULL_POS;
                }
                else if (parent_current.right == pos) {
                    parent_current.right = NULL_POS;
                }

                writeRecord(parent_current);

                removed = true;
            }
            else if (current.left == NULL_POS) {
                // "REMOVE" IT FROM FILE
                current.next_del = first_deleted_record;
                first_deleted_record = current.pos;

                writeRecord(current);

                fstream out_file(file_name, ios::binary | ios::in | ios::out);
                writeFileHeader(out_file);
                out_file.close();

                // UPDATE PARENT
                if (parent_current.left == pos) {
                    parent_current.left = current.right;
                }
                else if (parent_current.right == pos) {
                    parent_current.right = current.right;
                }

                writeRecord(parent_current);

                removed = true;
            }
            else if (current.right == NULL_POS) {
                // "REMOVE" IT FROM FILE
                current.next_del = first_deleted_record;
                first_deleted_record = current.pos;

                writeRecord(current);

                fstream out_file(file_name, ios::binary | ios::in | ios::out);
                writeFileHeader(out_file);
                out_file.close();

                // UPDATE PARENT
                if (parent_current.left == pos) {
                    parent_current.left = current.left;
                }
                else if (parent_current.right == pos) {
                    parent_current.right = current.left;
                }

                writeRecord(parent_current);

                removed = true;
            }
            else {
                int temp = maxValue(current.left);
                current.code = temp;
                // TODO: missing update to attribute name
                writeRecord(current);

                removed = remove(current.left, current, temp);
            }
        }

        return removed;
    }

    void seekAll(int pos, vector<Record>& records) {
        if (pos == NULL_POS) {
            return;
        }

        Record current = getRecordByPos(pos);

        // Check if the current record is not in the free list
        records.push_back(current);

        seekAll(current.left, records);
        seekAll(current.right, records);
    }

    void seekFreeList(int pos, vector<Record>& records) {
        if (pos == NULL_POS) {
            return;
        }

        Record current = getRecordByPos(pos);

        // Check if the current record is in the free list
        records.push_back(current);

        seekFreeList(current.next_del, records);
    }
};

#endif//PROYECTO1_BD2_AVLTREEFILE_H
