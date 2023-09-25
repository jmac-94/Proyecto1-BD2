#ifndef SEQUENTIAL_FILE_SEQUENTIALFILE_H
#define SEQUENTIAL_FILE_SEQUENTIALFILE_H

#include <string>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <optional>
#include <vector>

#include "../ExtendibleHashing/Record.h"

class SequentialFile {
private:
    std::string meta_data_path;
    std::string main_file_path;
    std::fstream meta_data;     // Contains essential data for correct functioning
    std::fstream main_file;     // Contains both data and aux space

    int data_file_count{};      // Number of records in data file
    int aux_file_count{};       // Number of records in aux file
    int first_record_pos = -1;  // Position of the first record in all main_file
    int aux_file_max_size{};    // Maximum number of records in aux file

    void readRecordFromMainFile(Record& record, int pos);

    void readRecordFromAuxFile(Record& record, int pos);

    void writeRecordInMainFile(Record& record, int pos);

    void writeRecordInAuxFile(Record& record);

    void updateRecordInAuxFile(Record& record, int pos);

    void writeInitialMetadata(int initial_aux_file_max_size);

    void readExistingMetadata();

    void writeCurrentMetadata();

    void reestructureFile();

    int binSearch(int id);

    int seqSearch(int id);

    int binSearchPrevPos(Record record, int low, int high);

    int lowerBound(int x);

    int upperBound(int x);

public:
    SequentialFile(const std::string& meta_data_path, const std::string& main_file_path, int initial_aux_file_max_size);

    void insertRecord(Record record_to_insert);

    std::optional<Record> searchRecord(int id);

    std::vector<Record> rangeSearchRecords(int id_low, int id_high);

    std::string toString();
};

#endif
